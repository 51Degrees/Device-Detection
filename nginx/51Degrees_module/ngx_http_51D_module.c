#include <nginx.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include "src/pattern/51Degrees.h"

// Define default settings.
#define FIFTYONEDEGREES_DEFAULTFILE "51Degrees.dat"
#define FIFTYONEDEGREES_DEFAULTCACHE 10000
#define FIFTYONEDEGREES_DEFAULTPOOL 20

#ifndef FIFTYONEDEGREES_MAX_PROPERTIES
#define FIFTYONEDEGREES_MAX_PROPERTIES 20
#endif

#ifndef FIFTYONEDEGREES_HTTP_PREFIX
#define FIFTYONEDEGREES_HTTP_PREFIX "51D-"
#endif

// 51Degrees http header prefix.
const char* prefix = FIFTYONEDEGREES_HTTP_PREFIX;

// Module config functions to enable matching in selected locations.
static char *ngx_http_51D_single(ngx_conf_t *cf, void *post, void *data);
static char *ngx_http_51D_multi(ngx_conf_t *cf, void *post, void *data);

// Nginx handler pointers to above functions.
static ngx_conf_post_handler_pt ngx_http_51D_single_p = ngx_http_51D_single;
static ngx_conf_post_handler_pt ngx_http_51D_multi_p = ngx_http_51D_multi;

// Module declaration.
ngx_module_t ngx_http_51D_module;

// Configuration function declarations.
static void *ngx_http_51D_create_main_conf(ngx_conf_t *cf);
static void *ngx_http_51D_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_51D_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);

// Handler declaration.
static ngx_int_t ngx_http_51D_handler(ngx_http_request_t *r);

// Declaration of string functions.
static char *join(ngx_pool_t* ngx_pool, const char* s1, const char* s2);
void lower_string(char s[]);

// Module location config.
typedef struct {
	ngx_uint_t single;
	ngx_uint_t multi;
	ngx_str_t properties_string;
	char *properties[FIFTYONEDEGREES_MAX_PROPERTIES];
	char *prefixed_properties[FIFTYONEDEGREES_MAX_PROPERTIES];
	char *lower_prefixed_properties[FIFTYONEDEGREES_MAX_PROPERTIES];
	ngx_uint_t properties_n;
} ngx_http_51D_loc_conf_t;

// Module main config.
typedef struct {
    char *properties[FIFTYONEDEGREES_MAX_PROPERTIES];
    ngx_uint_t properties_n;
    ngx_uint_t cacheSize;
    ngx_uint_t poolSize;
    ngx_str_t dataFile;
    fiftyoneDegreesProvider provider;
} ngx_http_51D_main_conf_t;

// Module post config. Added module handler to main config.
static ngx_int_t
ngx_http_51D_post_conf(ngx_conf_t *cf)
{
	ngx_http_handler_pt *h;
	ngx_http_core_main_conf_t *cmcf;

	cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

	h = ngx_array_push(&cmcf->phases[NGX_HTTP_ACCESS_PHASE].handlers);
	if (h == NULL) {
		return NGX_ERROR;
	}

	*h = ngx_http_51D_handler;

	return NGX_OK;
}

// Create main config. Allocates memory to the configuration and initialises
// integers to -1.
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
    conf->dataFile.len = NGX_CONF_UNSET_SIZE;

    return conf;
}

// Create location config. Allocates memory to the configuration and initialises
// integers to -1.
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

// Merges location config. Either gets the value set, or sets to the default
// of 0.
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

// Throws an error if data set initialisation fails.
static ngx_int_t reportDatasetInitStatus(fiftyoneDegreesDataSetInitStatus status,
										const char* fileName) {
	switch (status) {
	case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
		ngx_log_stderr(0, "Insufficient memory to load '%s'.", fileName);
		break;
	case DATA_SET_INIT_STATUS_CORRUPT_DATA:
		ngx_log_stderr(0, "Device data file '%s' is corrupted.", fileName);
		break;
	case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
		ngx_log_stderr(0, "Device data file '%s' is not correct version.", fileName);
		break;
	case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
		ngx_log_stderr(0, "Device data file '%s' not found.", fileName);
		break;
	case DATA_SET_INIT_STATUS_NULL_POINTER:
		ngx_log_stderr(0, "Null pointer to the existing dataset or memory location.");
		break;
	case DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS:
		ngx_log_stderr(0, "Allocated continuous memory containing 51Degrees data file "
			"appears to be smaller than expected. Most likely because the"
			" data file was not fully loaded into the allocated memory.");
		break;
	default:
		ngx_log_stderr(0, "Device data file '%s' could not be loaded.", fileName);
		break;
	}
	return NGX_ERROR;
}

// Initialises the provider on process start.
static ngx_int_t
ngx_http_51D_init_process(ngx_cycle_t *cycle)
{
	fiftyoneDegreesDataSetInitStatus status;
	ngx_http_51D_main_conf_t *fdmcf;

	// Get module main config.
	fdmcf = ngx_http_cycle_get_module_main_conf(cycle, ngx_http_51D_module);

	// If a setting is not set, set the default.
	if ((int)fdmcf->dataFile.len < 0) {
		fdmcf->dataFile.data = FIFTYONEDEGREES_DEFAULTFILE;
		fdmcf->dataFile.len = strlen(fdmcf->dataFile.data);
	}
	if ((int)fdmcf->cacheSize < 0) {
		fdmcf->cacheSize = FIFTYONEDEGREES_DEFAULTCACHE;
	}
	if ((int)fdmcf->poolSize < 0) {
		fdmcf->poolSize = FIFTYONEDEGREES_DEFAULTPOOL;
	}

	// Initialise the provider or return an error on failure.
	status = fiftyoneDegreesInitProviderWithPropertyArray((const char*)fdmcf->dataFile.data, &fdmcf->provider, (const char**)fdmcf->properties, (int)fdmcf->properties_n, fdmcf->poolSize, fdmcf->cacheSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return reportDatasetInitStatus(status, (const char*)fdmcf->dataFile.data);
	}

	return NGX_OK;
}

// Frees the provider on process close.
static void
ngx_http_51D_exit_process(ngx_cycle_t *cycle)
{
	ngx_http_51D_main_conf_t *fdmcf = ngx_http_cycle_get_module_main_conf(cycle, ngx_http_51D_module);

	// Free the provider.
	fiftyoneDegreesProviderFree(&fdmcf->provider);
}

// Definitions of functions which can be called in 'nginx.conf'
// --51D_single takes one string argument, a comma separated
// list of properties to be returned. Is called within location
// block. Enables User-Agent matching.
// --51D_multi takes one string argument, a comma separated
// list of properties to be returned. Is called within location
// block. Enables multiple http header matching.
// --51D_filePath takes one string argument, the path to a
// 51Degrees data file. Is called within server block.
// --51D_cache takes one integer argument, the size of the
// 51Degrees cache.
// --51D_pool takes one integer argument, the size of the
// 51Degrees pool.
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

// 51Degres module context.
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

// 51Degrees module definition.
ngx_module_t ngx_http_51D_module = {
	NGX_MODULE_V1,
	&ngx_http_51D_module_ctx,      /* module context */
	ngx_http_51D_commands,         /* module directives */
	NGX_HTTP_MODULE,               /* module type */
	NULL,                          /* init master */
	NULL,                          /* init module */
	ngx_http_51D_init_process,     /* init process */
	NULL,                          /* init thread */
	NULL,                          /* exit thread */
	ngx_http_51D_exit_process,     /* exit process */
	NULL,                          /* exit master */
	NGX_MODULE_V1_PADDING
};

// Used when matching multiple http headers to find important headers.
// See:
// https://www.nginx.com/resources/wiki/start/topics/examples/headers_management
static ngx_table_elt_t *
search_headers_in(ngx_http_request_t *r, u_char *name, size_t len) {
    ngx_list_part_t            *part;;
    ngx_table_elt_t            *h;
    ngx_uint_t                  i;

    part = &r->headers_in.headers.part;
    h = part->elts;

    for (i = 0; /* void */ ; i++) {
        if (i >= part->nelts) {
            if (part->next == NULL) {
                break;
            }

            part = part->next;
            h = part->elts;
            i = 0;
        }

        if (len != h[i].key.len || ngx_strcasecmp(name, h[i].key.data) != 0) {
            continue;
        }

        return &h[i];
    }

    return NULL;
}

// Module handler, gets a match using either the User-Agent or multiple http
// headers, then sets properties as headers.
static ngx_int_t
ngx_http_51D_handler(ngx_http_request_t *r)
{
	ngx_http_51D_main_conf_t *fdmcf;
	ngx_http_51D_loc_conf_t *fdlcf;
	fiftyoneDegreesWorkset *ws;
	int headerIndex, i, j, found;
	ngx_table_elt_t *searchResult;
	char *methodName, *property_name, *property_values_array[FIFTYONEDEGREES_MAX_PROPERTIES];

	if (r->main->internal) {
		return NGX_DECLINED;
	}

	// Get 51Degrees location config.
	fdlcf = ngx_http_get_module_loc_conf(r, ngx_http_51D_module);

	if (fdlcf->single | fdlcf->multi)
	{
		r->main->internal = 1;

		// Get 51Degrees main config.
        fdmcf = ngx_http_get_module_main_conf(r, ngx_http_51D_module);

		ws = fiftyoneDegreesProviderWorksetGet(&fdmcf->provider);

		// If single requested, match for single User-Agent.
		if (fdlcf->single) {
			if (r->headers_in.user_agent)
				fiftyoneDegreesMatch(ws, (const char*)r->headers_in.user_agent[0].value.data);
			else
				fiftyoneDegreesMatch(ws, "");
		}
		// If multi requested, match for multiple http headers.
		else if (fdlcf->multi) {
			ws->importantHeadersCount = 0;
			for (headerIndex = 0; headerIndex < ws->dataSet->httpHeadersCount; headerIndex++) {
				searchResult = search_headers_in(r, (u_char*)ws->dataSet->httpHeaders[headerIndex].headerName, strlen(ws->dataSet->httpHeaders[headerIndex].headerName));
                if (searchResult) {
					ws->importantHeaders[ws->importantHeadersCount].header = ws->dataSet->httpHeaders + headerIndex;
					ws->importantHeaders[ws->importantHeadersCount].headerValue = searchResult->value.data;
					ws->importantHeaders[ws->importantHeadersCount].headerValueLength = searchResult->value.len;
					ws->importantHeadersCount++;
                }
                fiftyoneDegreesMatchForHttpHeaders(ws);
			}
		}

		// For each property, set the value in property_values_array.
		i = 0;
        while (fdlcf->properties[i]) {
            found = 0;
            if (strcmp("Method", fdlcf->properties[i]) == 0) {
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
            else if (strcmp("Difference", fdlcf->properties[i]) == 0) {
                char buffer[20];
                sprintf(buffer, "%d", ws->difference);
                property_values_array[i] = join(r->pool, "", buffer);
                found = 1;
            }
            else if (strcmp("Rank", fdlcf->properties[i]) == 0) {
                char buffer[20];
                sprintf(buffer, "%d", fiftyoneDegreesGetSignatureRank(ws));
                property_values_array[i] = join(r->pool, "", buffer);
                found = 1;
            }
            else if (strcmp("DeviceId", fdlcf->properties[i]) == 0) {
                char buffer[24];
					fiftyoneDegreesGetDeviceId(ws, buffer, 24);
					property_values_array[i] = join(r->pool, "", buffer);
					found = 1;

            }
            else {
                for (j = 0; j < ws->dataSet->requiredPropertyCount; j++) {
                    property_name = (char*)fiftyoneDegreesGetPropertyName(ws->dataSet, ws->dataSet->requiredProperties[j]);
                    if (strcmp(property_name, fdlcf->properties[i]) == 0) {
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

		// Release the workset back to the pool.
		fiftyoneDegreesWorksetRelease(ws);

		// For each property value pair, set a new header name and value.
        ngx_table_elt_t *h[fdlcf->properties_n];
		for (i=0; i<(int)fdlcf->properties_n; i++) {
			h[i] = ngx_list_push(&r->headers_in.headers);
			h[i]->hash = i;
			h[i]->key.data = (u_char*)fdlcf->prefixed_properties[i];
			h[i]->key.len = ngx_strlen(h[i]->key.data);
			h[i]->value.data = (u_char*)property_values_array[i];
			h[i]->value.len = ngx_strlen(h[i]->value.data);
			h[i]->lowcase_key = (u_char*)fdlcf->lower_prefixed_properties[i];
		}
	}
	return NGX_DECLINED;

}

// Set properties function. Is passed a comma separated
// list of properties from nginx.conf and assigns
// each to an element in properties.
// It then initialises the detector.
void
ngx_http_51D_set_properties(ngx_conf_t *cf, ngx_http_51D_main_conf_t *fdmcf, ngx_http_51D_loc_conf_t *fdlcf)
{
	ngx_str_t *properties = &fdlcf->properties_string;

	// Check that properties count is initialised (-1 = not initialised).
	if ((int)fdmcf->properties_n < 0)
		fdmcf->properties_n = 0;

	// Sepparate the properties by commas into an array.
	int i = 0, j, found;
	char *tok = strtok((char*)properties->data, (const char*)",");
	while (tok != NULL) {
		found = 0;
		fdlcf->properties[i++] = tok;
		tok = strtok(NULL, ",");
	}
	// Set the properties count.
	fdlcf->properties_n = i;

	// Add any properties that are not already in the main config.
	for (i = 0; i < (int)fdlcf->properties_n; i++) {
		found = 0;
		for (j = 0; j < (int)fdmcf->properties_n; j++) {
			if (strcmp(fdlcf->properties[i], fdmcf->properties[j]) == 0)
				found = 1;
		}
		if (!found) {
			fdmcf->properties[fdmcf->properties_n] = fdlcf->properties[i];
			fdmcf->properties_n = fdmcf->properties_n + 1;
		}
		// Set the prefixed, and lowercase prefixed property names.
		fdlcf->prefixed_properties[i] = join(cf->pool, prefix, fdlcf->properties[i]);
		fdlcf->lower_prefixed_properties[i] = join(cf->pool, prefix, fdlcf->properties[i]);
		lower_string(fdlcf->lower_prefixed_properties[i]);
	}
}

// Enables User-Agent matching in the selected location with the properties
// string in data.
static char *ngx_http_51D_single(ngx_conf_t* cf, void* post, void* data)
{
	ngx_http_51D_main_conf_t *fdmcf;
	ngx_http_51D_loc_conf_t *fdlcf;

	// Get the modules location and main config.
	fdmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_51D_module);
	fdlcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_51D_module);

	// Enable single User-Agent matching.
	fdlcf->single = 1;

	// Set the properties for the selected location.
	ngx_http_51D_set_properties(cf, fdmcf, fdlcf);

	return NGX_OK;
}

// Enables multiple http header matching in the selected location with the
// properties string in data.
static char *ngx_http_51D_multi(ngx_conf_t* cf, void* post, void* data)
{
	ngx_http_51D_main_conf_t *fdmcf;
	ngx_http_51D_loc_conf_t *fdlcf;

	// Get the modules location and main config.
	fdmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_51D_module);
	fdlcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_51D_module);

	// Enable multiple http header matching.
	fdlcf->multi = 1;

	// Set the properties for the deected location.
	ngx_http_51D_set_properties(cf, fdmcf, fdlcf);

	return NGX_OK;
}

// Simple string join function.
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

// Convert a string to lower case.
void lower_string(char s[]) {
   int c = 0;

   while (s[c] != '\0') {
      if (s[c] >= 'A' && s[c] <= 'Z') {
         s[c] = s[c] + 32;
      }
      c++;
   }
}
