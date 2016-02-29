#include <nginx.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include "src/pattern/51Degrees.h"

#define MAX_51D_PROPERTIES 10
//Nginx function declarations
//static char *ngx_http_51D_set_properties(ngx_conf_t *cf, void *post, void *data);
static char *ngx_http_51D_single(ngx_conf_t *cf, void *post, void *data);
static char *ngx_http_51D_multi(ngx_conf_t *cf, void *post, void *data);

//Nginx handler pointers to above functions
//static ngx_conf_post_handler_pt ngx_http_51D_set_properties_p = ngx_http_51D_set_properties;
static ngx_conf_post_handler_pt ngx_http_51D_single_p = ngx_http_51D_single;
static ngx_conf_post_handler_pt ngx_http_51D_multi_p = ngx_http_51D_multi;

ngx_module_t ngx_http_51D_module;

//Main Nginx module handler declaration
static ngx_int_t ngx_http_51D_handler(ngx_http_request_t *r);

//Simple string join function declaration
static char *join(ngx_pool_t* ngx_pool, const char* s1, const char* s2);

void lower_string(char s[]);

//51Degrees declarations
const char* prefix = "51D-";

typedef struct {
	ngx_uint_t single;
	ngx_uint_t multi;
	ngx_str_t properties_string;
	char *properties[MAX_51D_PROPERTIES];
	char *prefixed_properties[MAX_51D_PROPERTIES];
	char *lower_prefixed_properties[MAX_51D_PROPERTIES];
	ngx_uint_t properties_n;
} ngx_http_51D_loc_conf_t;

typedef struct {
    char *properties[MAX_51D_PROPERTIES];
    ngx_uint_t properties_n;
    ngx_uint_t cacheSize;
    ngx_uint_t poolSize;
    ngx_str_t dataFile;
    fiftyoneDegreesDataSet dataSet;
    fiftyoneDegreesResultsetCache *cache;
    fiftyoneDegreesWorksetPool *pool;
    fiftyoneDegreesWorkset *ws;
} ngx_http_51D_main_conf_t;

static void *ngx_http_51D_create_main_conf(ngx_conf_t *cf);

static ngx_int_t
ngx_http_51D_post_conf(ngx_conf_t *cf)
{
	ngx_http_handler_pt *h;
	ngx_http_core_main_conf_t *cmcf;
	ngx_http_51D_main_conf_t *fdmcf;

	cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);
    fdmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_51D_module);

	h = ngx_array_push(&cmcf->phases[NGX_HTTP_ACCESS_PHASE].handlers);
	if (h == NULL) {
		return NGX_ERROR;
	}

	*h = ngx_http_51D_handler;

	if (fdmcf->dataFile.len > 0) {
		switch(fiftyoneDegreesInitWithPropertyArray((const char*)fdmcf->dataFile.data, &fdmcf->dataSet, (const char**)fdmcf->properties, fdmcf->properties_n)) {
			case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
				ngx_log_stderr(0, "Insufficient memory to load '%s'.", fdmcf->dataFile.data);
				return NGX_ERROR;
			case DATA_SET_INIT_STATUS_CORRUPT_DATA:
				ngx_log_stderr(0, "Device data file '%s' is corrupted.", fdmcf->dataFile.data);
				return NGX_ERROR;
			case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
				ngx_log_stderr(0, "Device data file '%s' is not correct version.", fdmcf->dataFile.data);
				return NGX_ERROR;
			case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
				ngx_log_stderr(0, "Device data file '%s' not found.", fdmcf->dataFile.data);
				return NGX_ERROR;
			case DATA_SET_INIT_STATUS_NOT_SET:
				ngx_log_stderr(0, "Device data file '%s' could not be loaded.", fdmcf->dataFile.data);
				return NGX_ERROR;
			case DATA_SET_INIT_STATUS_SUCCESS:
				break;
		}

		// Create a cache if a cache size is provided.
		if ((int)fdmcf->cacheSize > 0) {
			fdmcf->cache = fiftyoneDegreesResultsetCacheCreate(&fdmcf->dataSet, fdmcf->cacheSize);
		}
		else {
			fdmcf->cache = NULL;
			ngx_log_error(NGX_LOG_NOTICE, cf->log, 0, "51D: cache size not set in config");
		}

		// Create a workset pool if a pool size is provided, else create a single
		// workset.
		if ((int)fdmcf->poolSize > 0) {
			fdmcf->pool = fiftyoneDegreesWorksetPoolCreate(&fdmcf->dataSet, fdmcf->cache, fdmcf->poolSize);
		}
		else {
			fdmcf->pool = NULL;
			fdmcf->ws = fiftyoneDegreesWorksetCreate(&fdmcf->dataSet, fdmcf->cache);
			ngx_log_error(NGX_LOG_NOTICE, cf->log, 0, "51D: pool size not set in config");
		}
		return NGX_OK;
	}
	else {
		ngx_log_stderr(0, "The 51Degrees data file has not been set in the config");
		return NGX_ERROR;
	}
}

static void *
ngx_http_51D_create_main_conf(ngx_conf_t *cf)
{
    ngx_http_51D_main_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_51D_main_conf_t));
    if (conf == NULL) {
        return NULL;
    }
    conf->cacheSize = NGX_CONF_UNSET_UINT;
    conf->poolSize = NGX_CONF_UNSET_UINT;
    conf->properties_n = NGX_CONF_UNSET_UINT;

    return conf;
}

static void *
ngx_http_51D_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_51D_loc_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_51D_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }
    conf->single = NGX_CONF_UNSET_UINT;
    conf->multi = NGX_CONF_UNSET_UINT;
    conf->properties_n = NGX_CONF_UNSET_UINT;

    return conf;
}

static char *
ngx_http_51D_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
	ngx_http_51D_loc_conf_t  *prev = parent;
	ngx_http_51D_loc_conf_t  *conf = child;

	ngx_conf_merge_uint_value(conf->single, prev->single, 0);
	ngx_conf_merge_uint_value(conf->multi, prev->multi, 0);
	ngx_conf_merge_uint_value(conf->properties_n, prev->properties_n, 0);

return NGX_CONF_OK;
}

//Definitions of functions which can be called in 'nginx.conf'
//--51D_properties takes one string argument, a comma separated
//list of properties to be returned. Is called within server
//block.
//--51D_filePath takes one string argument, the path to a
//51Degrees data file. Is called within server block.
static ngx_command_t  ngx_http_51D_commands[] = {

	{ ngx_string("51D_single"),
	NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
	ngx_conf_set_str_slot,
	NGX_HTTP_LOC_CONF_OFFSET,
	offsetof(ngx_http_51D_loc_conf_t, properties_string),
	&ngx_http_51D_single_p },

	{ ngx_string("51D_multi"),
	NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
	ngx_conf_set_str_slot,
	NGX_HTTP_LOC_CONF_OFFSET,
	offsetof(ngx_http_51D_loc_conf_t, properties_string),
	&ngx_http_51D_multi_p },

	{ ngx_string("51D_filePath"),
	NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
	ngx_conf_set_str_slot,
	NGX_HTTP_MAIN_CONF_OFFSET,
	offsetof(ngx_http_51D_main_conf_t, dataFile),
	NULL },

	{ ngx_string("51D_cache"),
	NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
	ngx_conf_set_num_slot,
	NGX_HTTP_MAIN_CONF_OFFSET,
	offsetof(ngx_http_51D_main_conf_t, cacheSize),
	NULL },

	{ ngx_string("51D_pool"),
	NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
	ngx_conf_set_num_slot,
	NGX_HTTP_MAIN_CONF_OFFSET,
	offsetof(ngx_http_51D_main_conf_t, poolSize),
	NULL },

	ngx_null_command
};

static ngx_http_module_t ngx_http_51D_module_ctx = {
	NULL,                          /* preconfiguration */
	ngx_http_51D_post_conf,        /* postconfiguration */

	ngx_http_51D_create_main_conf, /* create main configuration */
	NULL,                          /* init main configuration */

	NULL,                          /* create server configuration */
	NULL,                          /* merge server configuration */

	ngx_http_51D_create_loc_conf,  /* create location configuration */
	ngx_http_51D_merge_loc_conf    /* merge location configuration */
};


ngx_module_t ngx_http_51D_module = {
	NGX_MODULE_V1,
	&ngx_http_51D_module_ctx,      /* module context */
	ngx_http_51D_commands,         /* module directives */
	NGX_HTTP_MODULE,               /* module type */
	NULL,                          /* init master */
	NULL,                          /* init module */
	NULL,                          /* init process */
	NULL,                          /* init thread */
	NULL,                          /* exit thread */
	NULL,                          /* exit process */
	NULL,                          /* exit master */
	NGX_MODULE_V1_PADDING
};


static ngx_table_elt_t *
search_headers_in(ngx_http_request_t *r, u_char *name, size_t len) {
    ngx_list_part_t            *part;
    ngx_table_elt_t            *h;
    ngx_uint_t                  i;

    /*
    Get the first part of the list. There is usual only one part.
    */
    part = &r->headers_in.headers.part;
    h = part->elts;

    /*
    Headers list array may consist of more than one part,
    so loop through all of it
    */
    for (i = 0; /* void */ ; i++) {
        if (i >= part->nelts) {
            if (part->next == NULL) {
                /* The last part, search is done. */
                break;
            }

            part = part->next;
            h = part->elts;
            i = 0;
        }

        /*
        Just compare the lengths and then the names case insensitively.
        */
        if (len != h[i].key.len || ngx_strcasecmp(name, h[i].key.data) != 0) {
            /* This header doesn't match. */
            continue;
        }

        /*
        Ta-da, we got one!
        Note, we'v stop the search at the first matched header
        while more then one header may fit.
        */
        return &h[i];
    }

    /*
    No headers was found
    */
    return NULL;
}

//Module handler, gets match for User-Agent
//and sets all property headers.
static ngx_int_t
ngx_http_51D_handler(ngx_http_request_t *r)
{
	ngx_http_51D_loc_conf_t *conf;
	conf = ngx_http_get_module_loc_conf(r, ngx_http_51D_module);

	if (r->main->internal) {
		return NGX_DECLINED;
	}

	if (conf->single | conf->multi)
	{
		r->main->internal = 1;
	//TODO: Implement multiple http headers for get_match.

        ngx_http_51D_main_conf_t *fdmcf;
        fdmcf = ngx_http_get_module_main_conf(r, ngx_http_51D_module);

        fiftyoneDegreesWorkset *ws;
        if (fdmcf->pool != NULL) {
			ws = fiftyoneDegreesWorksetPoolGet(fdmcf->pool);
        }
        else {
			ws = fdmcf->ws;
        }
		if (conf->single) {
			if (r->headers_in.user_agent)
				fiftyoneDegreesMatch(ws, (const char*)r->headers_in.user_agent[0].value.data);
			else
				fiftyoneDegreesMatch(ws, "");
		}
		else if (conf->multi) {
			//find headers get match/
			int headerIndex;
			ngx_table_elt_t *searchResult;
			ws->importantHeadersCount = 0;
			for (headerIndex = 0; headerIndex < fdmcf->dataSet.httpHeadersCount; headerIndex++) {
				searchResult = search_headers_in(r, (u_char*)fdmcf->dataSet.httpHeaders[headerIndex].headerName, strlen(fdmcf->dataSet.httpHeaders[headerIndex].headerName));
                if (searchResult) {
					ws->importantHeaders[ws->importantHeadersCount].header = fdmcf->dataSet.httpHeaders + headerIndex;
					ws->importantHeaders[ws->importantHeadersCount].headerValue = searchResult->value.data;
					ws->importantHeaders[ws->importantHeadersCount].headerValueLength = searchResult->value.len;
					ws->importantHeadersCount++;
                }
                fiftyoneDegreesMatchForHttpHeaders(ws);
			}
			//fdmcf->dataSet->httpHeaders->headerName
		}

        char *methodName;
        char* property_name;
        int j, i=0, found;
		static char* property_values_array[10];

        while (conf->properties[i]) {
            found = 0;
            if (strcmp("Method", conf->properties[i]) == 0) {
                switch(ws->method) {
                    case EXACT: methodName = "Exact"; break;
                    case NUMERIC: methodName = "Numeric"; break;
                    case NEAREST: methodName = "Nearest"; break;
                    case CLOSEST: methodName = "Closest"; break;
                    default:
                    case NONE: methodName = "None"; break;
                }
                property_values_array[i] = methodName;
                found = 1;
            }
            else if (strcmp("Difference", conf->properties[i]) == 0) {
                char buffer[20];
                sprintf(buffer, "%d", ws->difference);
                property_values_array[i] = join(r->pool, "", buffer);
                found = 1;
            }
            else if (strcmp("Rank", conf->properties[i]) == 0) {
                char buffer[20];
                sprintf(buffer, "%d", fiftyoneDegreesGetSignatureRank(ws));
                property_values_array[i] = join(r->pool, "", buffer);
                found = 1;
            }
            else if (strcmp("DeviceId", conf->properties[i]) == 0) {
                char buffer[24];
					fiftyoneDegreesGetDeviceId(ws, buffer, 24);
					property_values_array[i] = join(r->pool, "", buffer);
					found = 1;

            }
            else {
                for (j = 0; j < ws->dataSet->requiredPropertyCount; j++) {
                    property_name = (char*)fiftyoneDegreesGetPropertyName(ws->dataSet, ws->dataSet->requiredProperties[j]);
                    if (strcmp(property_name, conf->properties[i]) == 0) {
                        fiftyoneDegreesSetValues(ws, j);
                        property_values_array[i] = (char*)fiftyoneDegreesGetValueName(ws->dataSet, *ws->values);
                        found = 1;
                        break;
                    }
                    if (!found) {
                        property_values_array[i] = "NA";
                    }
                }
            }
            ++i;
        }

        if (fdmcf->pool != NULL) {
			fiftyoneDegreesWorksetPoolRelease(fdmcf->pool, ws);
			ws = NULL;
        }

        ngx_table_elt_t *h[conf->properties_n];

		for (i=0; i<(int)conf->properties_n; i++) {
			h[i] = ngx_list_push(&r->headers_in.headers);
			h[i]->hash = i;
			h[i]->key.data = (u_char*)conf->prefixed_properties[i];
			h[i]->key.len = ngx_strlen(h[i]->key.data);
			h[i]->value.data = (u_char*)property_values_array[i];
			h[i]->value.len = ngx_strlen(h[i]->value.data);
			h[i]->lowcase_key = (u_char*)conf->lower_prefixed_properties[i];
		}
	}

	return NGX_DECLINED;

}

//Set properties function. Is passed a comma separated
//list of properties from nginx.conf and assigns
//each to an element in properties.
//It then initialises the detector.
void
ngx_http_51D_set_properties(ngx_conf_t *cf, ngx_http_51D_main_conf_t *fdmcf, ngx_http_51D_loc_conf_t *fdlcf, ngx_str_t *properties)
{
	if ((int)fdmcf->properties_n <0)
		fdmcf->properties_n =0;

	int i = 0, j, found;
	char *tok = strtok((char*)properties->data, (const char*)",");
	while (tok != NULL) {
		found = 0;
		fdlcf->properties[i++] = tok;
		tok = strtok(NULL, ",");
	}
	fdlcf->properties_n = i;

	for (i=0;i<(int)fdlcf->properties_n;i++) {
		found = 0;
		for (j=0; j<(int)fdmcf->properties_n; j++) {
			if (strcmp(fdlcf->properties[i], fdmcf->properties[j]) == 0)
				found = 1;
		}
		if (!found) {
			fdmcf->properties[fdmcf->properties_n] = fdlcf->properties[i];
			fdmcf->properties_n = fdmcf->properties_n + 1;
		}
		fdlcf->prefixed_properties[i] = join(cf->pool, prefix, fdlcf->properties[i]);
		fdlcf->lower_prefixed_properties[i] = join(cf->pool, prefix, fdlcf->properties[i]);

		lower_string(fdlcf->lower_prefixed_properties[i]);
	}
}

static char *ngx_http_51D_single(ngx_conf_t* cf, void* post, void* data)
{
	ngx_http_51D_main_conf_t *fdmcf;
	fdmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_51D_module);
	ngx_http_51D_loc_conf_t *fdlcf;
	fdlcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_51D_module);

	ngx_str_t  *properties = data;

	fdlcf->single = 1;

	ngx_http_51D_set_properties(cf, fdmcf, fdlcf, properties);

	return NGX_OK;
}

static char *ngx_http_51D_multi(ngx_conf_t* cf, void* post, void* data)
{
	ngx_http_51D_main_conf_t *fdmcf;
	fdmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_51D_module);
	ngx_http_51D_loc_conf_t *fdlcf;
	fdlcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_51D_module);

	ngx_str_t  *properties = data;

	fdlcf->multi = 1;

	ngx_http_51D_set_properties(cf, fdmcf, fdlcf, properties);

	return NGX_OK;
}

//Simple string join function, is used when setting
//prefixed headers.
char *join(ngx_pool_t* ngx_pool, const char* s1, const char* s2)
{
    char* result = ngx_pcalloc(ngx_pool, ngx_strlen(s1) + ngx_strlen(s2) + 1);
    if (result)
    {
        strcpy(result, s1);
        strcat(result, s2);
    }
    return result;
}

void lower_string(char s[]) {
   int c = 0;

   while (s[c] != '\0') {
      if (s[c] >= 'A' && s[c] <= 'Z') {
         s[c] = s[c] + 32;
      }
      c++;
   }
}
