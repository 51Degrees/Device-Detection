#include <stdio.h>

#include <common/cfgparse.h>
#include <common/chunk.h>
#include <common/buffer.h>
#include <proto/arg.h>
#include <proto/log.h>
#include <proto/sample.h>
#include <import/xxhash.h>
#include <import/lru.h>

#include <import/51d.h>
#include "../../Device-Detection/src/pattern/51Degrees.h"

struct _51d_property_names {
	struct list list;
	char *name;
};

static struct lru64_head *_51d_lru_tree = NULL;
static unsigned long long _51d_lru_seed;

static int _51d_data_file(char **args, int section_type, struct proxy *curpx,
                          struct proxy *defpx, const char *file, int line,
                          char **err)
{
	if (*(args[1]) == 0) {
		memprintf(err,
		          "'%s' expects a filepath to a 51Degrees trie or pattern data file.",
		          args[0]);
		return -1;
	}

	if (global._51degrees.data_file_path)
		free(global._51degrees.data_file_path);
	global._51degrees.data_file_path = strdup(args[1]);

	return 0;
}

static int _51d_property_name_list(char **args, int section_type, struct proxy *curpx,
                                  struct proxy *defpx, const char *file, int line,
                                  char **err)
{
	int cur_arg = 1;
	struct _51d_property_names *name;

	if (*(args[cur_arg]) == 0) {
		memprintf(err,
		          "'%s' expects at least one 51Degrees property name.",
		          args[0]);
		return -1;
	}

	while (*(args[cur_arg])) {
		name = calloc(1, sizeof(struct _51d_property_names));
		name->name = strdup(args[cur_arg]);
		LIST_ADDQ(&global._51degrees.property_names, &name->list);
		++cur_arg;
	}

	return 0;
}

static int _51d_property_separator(char **args, int section_type, struct proxy *curpx,
                                   struct proxy *defpx, const char *file, int line,
                                   char **err)
{
	if (*(args[1]) == 0) {
		memprintf(err,
		          "'%s' expects a single character.",
		          args[0]);
		return -1;
	}
	if (strlen(args[1]) > 1) {
		memprintf(err,
		          "'%s' expects a single character, got '%s'.",
		          args[0], args[1]);
		return -1;
	}

	global._51degrees.property_separator = *args[1];

	return 0;
}

static int _51d_cache_size(char **args, int section_type, struct proxy *curpx,
                           struct proxy *defpx, const char *file, int line,
                           char **err)
{
	if (*(args[1]) == 0) {
		memprintf(err,
		          "'%s' expects a positive numeric value.",
		          args[0]);
		return -1;
	}

	global._51degrees.cache_size = atoi(args[1]);
	if (global._51degrees.cache_size < 0) {
		memprintf(err,
		          "'%s' expects a positive numeric value, got '%s'.",
		          args[0], args[1]);
		return -1;
	}

	return 0;
}

static int _51d_fetch_check(struct arg *arg, char **err_msg)
{
	if (global._51degrees.data_file_path)
		return 1;

	memprintf(err_msg, "51Degrees data file is not specified (parameter '51degrees-data-file')");
	return 0;
}

/* Skips over the first line of the buffer as this contains the method which
 * is not relevant for device detection.
 */
static char* _51d_skip_method(struct buffer *buf) {
    char *v = buf->p, *e = buf->p + buf->i;
    while (v <= e &&
           *v != '\r' &&
           *v != '\n') {
           v++;
    }
    while (v <= e && (
           *v == '\r' ||
           *v == '\n')) {
           v++;
    }
    return v;
}

#ifdef FIFTYONEDEGREES_H_PATTERN_INCLUDED
/* Provides a hash code for the important HTTP headers.
 */
unsigned long long _51d_req_hash(const struct arg *args, fiftyoneDegreesWorkset* ws) {
    unsigned long long seed = _51d_lru_seed ^ (long)args;
    unsigned long long hash = 0;
    int i;
    for(i = 0; i < ws->importantHeadersCount; i++) {
        hash ^= ws->importantHeaders[i].header->headerNameOffset;
        hash ^= XXH64(
            ws->importantHeaders[i].headerValue,
            ws->importantHeaders[i].headerValueLength,
            seed);
    }
    return hash;
}
#endif

static int _51d_fetch(const struct arg *args, struct sample *smp, const char *kw, void *private)
{
	char no_data[] = "NoData";  /* response when no data could be found */
	struct chunk *temp;
    char *headers;
    int headersLength, j, i, found;
	const struct http_msg *msg;
	const char* property_name;
#ifdef FIFTYONEDEGREES_H_PATTERN_INCLUDED
	fiftyoneDegreesWorkset* ws; /* workset for detection */
	struct lru64 *lru = NULL;
#endif
#ifdef FIFTYONEDEGREES_H_TRIE_INCLUDED
	char valuesBuffer[1024];
	fiftyoneDegreesDeviceOffsets *deviceOffsets; /* offsets for each header */
	char **requiredProperties;
	int requiredPropertiesCount;
#endif

    /* Get a pointer to the start of the headers and the length of the headers. */
	msg = ((smp->opt & SMP_OPT_DIR) == SMP_OPT_DIR_REQ) ? &smp->strm->txn->req : &smp->strm->txn->rsp;
	headers = _51d_skip_method(msg->chn->buf);
	headersLength = msg->chn->buf->i - (int)(headers - msg->chn->buf->p);

#ifdef FIFTYONEDEGREES_H_PATTERN_INCLUDED

    /* Get only the headers needed for device detection so they can be used
     * with the cache to return previous results. Pattern is slower than
     * Trie so caching will help improve performance.
     */

	/* Get a workset from the pool which will later contain detection results. */
	ws = fiftyoneDegreesWorksetPoolGet(global._51degrees.pool);
	if (!ws)
		return 0;

    /* Set the important HTTP headers for this request in the workset. */
    fiftyoneDegreesSetHttpHeaders(ws, headers, headersLength);

	/* Check the cache to see if there's results for these headers already. */
	if (_51d_lru_tree) {
		lru = lru64_get(_51d_req_hash(args, ws),
		                _51d_lru_tree, global._51degrees.data_file_path, 0);
		if (lru && lru->domain) {
			smp->flags |= SMP_F_CONST;
			smp->data.str.str = lru->data;
			smp->data.str.len = strlen(smp->data.str.str);
			return 1;
		}
	}

    fiftyoneDegreesMatchForHttpHeaders(ws);

#endif

#ifdef FIFTYONEDEGREES_H_TRIE_INCLUDED

    /* Trie is very fast so all the headers can be passed in and the result
     * returned faster than the hashing algorithm process.
     */

	deviceOffsets = fiftyoneDegreesGetDeviceOffsetsWithHeadersString(headers, headersLength);
	if (!deviceOffsets)
        return 0;
	requiredProperties = fiftyoneDegreesGetRequiredPropertiesNames();
	requiredPropertiesCount = fiftyoneDegreesGetRequiredPropertiesCount();

#endif

	temp = get_trash_chunk();
	i = 0;

	/* Loop through property names passed to the filter and fetch them from the dataset. */
	while (args[i].data.str.str) {
		/* Try to find request property in dataset. */
		found = 0;
#ifdef FIFTYONEDEGREES_H_PATTERN_INCLUDED
		for (j = 0; j < ws->dataSet->requiredPropertyCount; j++) {
			property_name = fiftyoneDegreesGetPropertyName(ws->dataSet, ws->dataSet->requiredProperties[j]);
			if (strcmp(property_name, args[i].data.str.str) == 0) {
				found = 1;
				fiftyoneDegreesSetValues(ws, j);
				chunk_appendf(temp, "%s", fiftyoneDegreesGetValueName(ws->dataSet, *ws->values));
				break;
			}
		}
#endif
#ifdef FIFTYONEDEGREES_H_TRIE_INCLUDED
		found = 0;
		for (j = 0; j < requiredPropertiesCount; j++) {
			property_name = requiredProperties[j];
			if (strcmp(property_name, args[i].data.str.str) == 0 &&
                fiftyoneDegreesGetValueFromOffsets(deviceOffsets, j, valuesBuffer, 1024) > 0) {
				found = 1;
				chunk_appendf(temp, "%s", valuesBuffer);
				break;
			}
		}
#endif
		if (!found) {
			chunk_appendf(temp, "%s", no_data);
		}
		/* Add separator. */
		chunk_appendf(temp, "%c", global._51degrees.property_separator);
		++i;
	}

	if (temp->len) {
		--temp->len;
		temp->str[temp->len] = '\0';
	}

	smp->data.str.str = temp->str;
	smp->data.str.len = strlen(smp->data.str.str);

#ifdef FIFTYONEDEGREES_H_PATTERN_INCLUDED
	fiftyoneDegreesWorksetPoolRelease(global._51degrees.pool, ws);
	if (lru) {
		smp->flags |= SMP_F_CONST;
		lru64_commit(lru, smp->data.str.str, global._51degrees.data_file_path, 0, free);
	}
#endif
#ifdef FIFTYONEDEGREES_H_TRIE_INCLUDED
    free(deviceOffsets);
#endif

	return 1;
}

int init_51degrees(void)
{
	int i = 0;
	struct chunk *temp;
	struct _51d_property_names *name;
	char **_51d_property_list = NULL;
	fiftyoneDegreesDataSetInitStatus _51d_dataset_status = DATA_SET_INIT_STATUS_NOT_SET;

	if (!global._51degrees.data_file_path)
		return -1;

	if (!LIST_ISEMPTY(&global._51degrees.property_names)) {
		i = 0;
		list_for_each_entry(name, &global._51degrees.property_names, list)
			++i;
		_51d_property_list = calloc(i, sizeof(char *));

		i = 0;
		list_for_each_entry(name, &global._51degrees.property_names, list)
			_51d_property_list[i++] = name->name;
	}

#ifdef FIFTYONEDEGREES_H_PATTERN_INCLUDED
	_51d_dataset_status = fiftyoneDegreesInitWithPropertyArray(global._51degrees.data_file_path, &global._51degrees.data_set, _51d_property_list, i);
#endif
#ifdef FIFTYONEDEGREES_H_TRIE_INCLUDED
	_51d_dataset_status = fiftyoneDegreesInitWithPropertyArray(global._51degrees.data_file_path, _51d_property_list, i);
#endif

	temp = get_trash_chunk();
	chunk_reset(temp);

	switch (_51d_dataset_status) {
		case DATA_SET_INIT_STATUS_SUCCESS:
#ifdef FIFTYONEDEGREES_H_PATTERN_INCLUDED
            global._51degrees.pool = fiftyoneDegreesWorksetPoolCreate(&global._51degrees.data_set, NULL, 10);
#endif
			break;
		case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
			chunk_printf(temp, "Insufficient memory.");
			break;
		case DATA_SET_INIT_STATUS_CORRUPT_DATA:
#ifdef FIFTYONEDEGREES_H_PATTERN_INCLUDED
			chunk_printf(temp, "Corrupt data file. Check that the data file provided is uncompressed and Pattern data format.");
#endif
#ifdef FIFTYONEDEGREES_H_TRIE_INCLUDED
			chunk_printf(temp, "Corrupt data file. Check that the data file provided is uncompressed and Trie data format.");
#endif
			break;
		case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
#ifdef FIFTYONEDEGREES_H_PATTERN_INCLUDED
			chunk_printf(temp, "Incorrect version. Check that the data file provided is uncompressed and Pattern data format.");
#endif
#ifdef FIFTYONEDEGREES_H_TRIE_INCLUDED
			chunk_printf(temp, "Incorrect version. Check that the data file provided is uncompressed and Trie data format.");
#endif
			break;
		case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
			chunk_printf(temp, "File not found.");
			break;
		case DATA_SET_INIT_STATUS_NOT_SET:
			chunk_printf(temp, "Data set not initialised.");
			break;
	}
	if (_51d_dataset_status != DATA_SET_INIT_STATUS_SUCCESS) {
		if (temp->len)
			Alert("51Degrees Setup - Error reading 51Degrees data file. %s\n", temp->str);
		else
			Alert("51Degrees Setup - Error reading 51Degrees data file.\n");
		exit(1);
	}
	free(_51d_property_list);

	_51d_lru_seed = random();
	if (global._51degrees.cache_size)
		_51d_lru_tree = lru64_new(global._51degrees.cache_size);

	return 0;
}

void deinit_51degrees(void)
{
	struct _51d_property_names *_51d_prop_name, *_51d_prop_nameb;

#ifdef FIFTYONEDEGREES_H_PATTERN_INCLUDED
    fiftyoneDegreesWorksetPoolFree(global._51degrees.pool);
	fiftyoneDegreesDataSetFree(&global._51degrees.data_set);
#endif
#ifdef FIFTYONEDEGREES_H_TRIE_INCLUDED
	fiftyoneDegreesDestroy();
#endif

	free(global._51degrees.data_file_path); global._51degrees.data_file_path = NULL;
	list_for_each_entry_safe(_51d_prop_name, _51d_prop_nameb, &global._51degrees.property_names, list) {
		LIST_DEL(&_51d_prop_name->list);
		free(_51d_prop_name);
	}

	while (lru64_destroy(_51d_lru_tree));
}

static struct cfg_kw_list _51dcfg_kws = {{ }, {
	{ CFG_GLOBAL, "51degrees-data-file", _51d_data_file },
	{ CFG_GLOBAL, "51degrees-property-name-list", _51d_property_name_list },
	{ CFG_GLOBAL, "51degrees-property-separator", _51d_property_separator },
	{ CFG_GLOBAL, "51degrees-cache-size", _51d_cache_size },
	{ 0, NULL, NULL },
}};

/* Note: must not be declared <const> as its list will be overwritten */
static struct sample_fetch_kw_list sample_fetch_keywords = {ILH, {
	{ "51d", _51d_fetch, ARG5(1,STR,STR,STR,STR,STR), _51d_fetch_check, SMP_T_STR, SMP_USE_HRQHV },
	{ NULL, NULL, 0, 0, 0 },
}};

__attribute__((constructor))
static void __51d_init(void)
{
	/* register sample fetch keywords */
	sample_register_fetches(&sample_fetch_keywords);
	cfg_register_keywords(&_51dcfg_kws);
}
