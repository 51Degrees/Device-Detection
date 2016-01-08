#include <nginx.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include "src/pattern/51Degrees.h"

//Nginx function declarations
static char *ngx_http_51D_match(ngx_conf_t *cf, void *post, void *data);
static char *ngx_http_51D_properties(ngx_conf_t *cf, void *post, void *data);
static char *ngx_http_51D_filePath(ngx_conf_t *cf, void *post, void *data);
static char *ngx_http_51D_cache(ngx_conf_t *cf, void *post, void *data);
static char *ngx_http_51D_pool(ngx_conf_t *cf, void *post, void *data);
static char *ngx_http_51D_init(ngx_conf_t *cf, void *post, void *data);
ngx_table_elt_t *get_user_agent(ngx_http_request_t *r);

//Nginx handler pointers to above functions
static ngx_conf_post_handler_pt ngx_http_51D_match_p = ngx_http_51D_match;
static ngx_conf_post_handler_pt ngx_http_51D_properties_p = ngx_http_51D_properties;
static ngx_conf_post_handler_pt ngx_http_51D_filePath_p = ngx_http_51D_filePath;
static ngx_conf_post_handler_pt ngx_http_51D_cache_p = ngx_http_51D_cache;
static ngx_conf_post_handler_pt ngx_http_51D_pool_p = ngx_http_51D_pool;
static ngx_conf_post_handler_pt ngx_http_51D_init_p = ngx_http_51D_init;
static void *get_match(ngx_pool_t *ngx_pool, char *userAgent);

//Main Nginx module handler declaration
static ngx_int_t ngx_http_51D_handler(ngx_http_request_t *r);

//Simple string join function declaration
static char *join(ngx_pool_t* ngx_pool, const char* s1, const char* s2);

void fod_log(char *msg);

//51Degrees declarations
static	fiftyoneDegreesDataSet dataSet;
static	fiftyoneDegreesWorkset *ws = NULL;
static	fiftyoneDegreesResultsetCache *cache = NULL;
static	fiftyoneDegreesWorksetPool *pool = NULL;

//Variable declarations
static char* properties_array[10];
static char* prefix_properties_array[10];
static char* property_values_array[10];
static int number_of_properties;
static char* dataFile;
static int cacheSize;
static int poolSize;
FILE *FODlog;

//Input variables
typedef struct {
	ngx_str_t properties_in;
	ngx_str_t dataFile_in;
//TODO: Pass in cache and pool size as integers so they don't need to be converted.
	ngx_str_t cache_in;
	ngx_str_t pool_in;
	int detect;
} ngx_http_51D_loc_conf_t;

//Post handler, outputs headers after match
static ngx_int_t
ngx_http_51D_post_match(ngx_conf_t *cf)
{
	fod_log("in post_match\n");
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

//Server configuration
static void *
ngx_http_51D_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_51D_loc_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_51D_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    return conf;
}

//Definitions of functions which can be called in 'nginx.conf'
//--51D_properties takes one string argument, a comma separated
//list of properties to be returned. Is called within server
//block.
//--51D_match takes no arguments, sets requested properties as
//headers. Is called within location block.
//--51D_filePath takes one string argument, the path to a
//51Degrees data file. Is called within server block.
static ngx_command_t  ngx_http_51D_commands[] = {
	{ ngx_string("51D_match"),
	NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
	ngx_conf_set_str_slot,
	NGX_HTTP_LOC_CONF_OFFSET,
	0,
	&ngx_http_51D_match_p },

	{ ngx_string("51D_properties"),
	NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
	ngx_conf_set_str_slot,
	NGX_HTTP_LOC_CONF_OFFSET,
	offsetof(ngx_http_51D_loc_conf_t, properties_in),
	&ngx_http_51D_properties_p },

	{ ngx_string("51D_filePath"),
	NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
	ngx_conf_set_str_slot,
	NGX_HTTP_LOC_CONF_OFFSET,
	offsetof(ngx_http_51D_loc_conf_t, dataFile_in),
	&ngx_http_51D_filePath_p },

	{ ngx_string("51D_cache"),
	NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
	ngx_conf_set_str_slot,
	NGX_HTTP_LOC_CONF_OFFSET,
	offsetof(ngx_http_51D_loc_conf_t, cache_in),
	&ngx_http_51D_cache_p },

	{ ngx_string("51D_pool"),
	NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
	ngx_conf_set_str_slot,
	NGX_HTTP_LOC_CONF_OFFSET,
	offsetof(ngx_http_51D_loc_conf_t, pool_in),
	&ngx_http_51D_pool_p },

	{ ngx_string("51D_init"),
	NGX_HTTP_SRV_CONF|NGX_CONF_NOARGS,
	ngx_http_51D_init,
	0,
	0,
	NULL },

	ngx_null_command
};

static ngx_http_module_t ngx_http_51D_module_ctx = {
	NULL,                          /* preconfiguration */
	ngx_http_51D_post_match,       /* postconfiguration */

	NULL,                          /* create main configuration */
	NULL,                          /* init main configuration */

	ngx_http_51D_create_loc_conf,  /* create server configuration */
	NULL,                          /* merge server configuration */

	ngx_http_51D_create_loc_conf,  /* create location configuration */
	NULL    /* merge location configuration */
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

//Module handler, gets match for User-Agent
//by calling get_match and sets all property
//headers.
//NOTE: this should ideally be in ngx_http_match so as not
//to run at Nginx start-up. This is minor and does not
//affect matching speed.
//TODO: Move setting of headers to ngx_http_51D_match.
static ngx_int_t
ngx_http_51D_handler(ngx_http_request_t *r)
{
	fod_log("In handler\n");

	ngx_http_51D_loc_conf_t *conf;
	fod_log("creating conf\n");
	conf = ngx_http_conf_get_module_loc_conf(r, ngx_http_51D_module);
	fod_log("created conf\n");
	char msg[100];
	sprintf(msg, "detect = %d\n", conf->detect);
	fod_log("got conf detect\n");
	fod_log(msg);

	if (r->main->internal) {
		return NGX_DECLINED;
	}
	r->main->internal = 1;
//TODO: Implement multiple http headers for get_match.
	ngx_table_elt_t *h[number_of_properties];
	int i;
	char* prefix_name;
	if (r->headers_in.user_agent) {
		fod_log("ua = ");
		fod_log(r->headers_in.user_agent[0].value.data);
		fod_log("\n");
		get_match(r->pool, r->headers_in.user_agent[0].value.data);
	}
	else {
		fod_log("no ua\n");
		get_match(r->pool, "");
	}
	for (i=0; i<number_of_properties; i++) {
		h[i] = ngx_list_push(&r->headers_in.headers);
		h[i]->hash = i;
		prefix_name = join(r->pool, "FiftyoneDegrees-", properties_array[i]);
        h[i]->key.data = prefix_name;
		h[i]->key.len = ngx_strlen(h[i]->key.data);
		h[i]->value.data = property_values_array[i];
		h[i]->value.len = ngx_strlen(h[i]->value.data);
    }

	return NGX_DECLINED;
}

//Match function, in future versions this will contain the above,
//but is just place-holder at present.
static char *
ngx_http_51D_match(ngx_conf_t *cf, void *post, void *data)
{
	fod_log("In ngx_http_51D_match\n");
	ngx_http_core_loc_conf_t *clcf;

	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_51D_handler;

	ngx_http_51D_loc_conf_t *conf;
	conf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_51D_module);

	char msg[100];
	sprintf(msg, "detect = %d\n", conf->detect);
	fod_log(msg);
	conf->detect = 1;

	return NGX_CONF_OK;
}

//Get match function, gets match for User-Agent with
//51Degrees detector. Then assigns all requested properties
//to elements in property_values_array.
//If a property does not exist 'NA' is returned.
static void *
get_match(ngx_pool_t *ngx_pool, char *userAgent)
{
	fod_log("In get_match\n");

	ws = fiftyoneDegreesWorksetPoolGet(pool);

	fiftyoneDegreesMatch(ws, userAgent);
	char *methodName;
	char* property_name;
	int j, i=0, found;

	while (properties_array[i]) {
		found = 0;
		if (strcmp("Method", properties_array[i]) == 0) {
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
		else if (strcmp("Difference", properties_array[i]) == 0) {
			char buffer[20];
			sprintf(buffer, "%d", ws->difference);
			property_values_array[i] = join(ngx_pool, "", buffer);
			found = 1;
		}
		else if (strcmp("Rank", properties_array[i]) == 0) {
			char buffer[20];
			sprintf(buffer, "%d", fiftyoneDegreesGetSignatureRank(ws));
			property_values_array[i] = join(ngx_pool, "", buffer);
			found = 1;
		}
		else if (strcmp("Id", properties_array[i]) == 0) {
			char buffer[24];
			if (fiftyoneDegreesGetDeviceId(ws, &buffer, 24));
			property_values_array[i] = join(ngx_pool, "", buffer);
			found = 1;
		}
		else {
			for (j = 0; j < ws->dataSet->requiredPropertyCount; j++) {
				property_name = fiftyoneDegreesGetPropertyName(ws->dataSet, ws->dataSet->requiredProperties[j]);
				if (strcmp(property_name, properties_array[i]) == 0) {
					fiftyoneDegreesSetValues(ws, j);
					property_values_array[i] = fiftyoneDegreesGetValueName(ws->dataSet, *ws->values);
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
	fiftyoneDegreesWorksetPoolRelease(pool, ws);
	//fiftyoneDegreesWorksetFree(ws);
	ws = NULL;
}

//Set properties function. Is passed a comma separated
//list of properties from nginx.conf and assigns
//each to an element in properties_array.
//It then initialises the detector.
//TODO: Set prefixed_properties_array elements here so it only happenes once.
static char *
ngx_http_51D_properties(ngx_conf_t *cf, void *post, void *data)
{
	ngx_http_core_loc_conf_t *clcf;

	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_51D_handler;

	ngx_str_t  *properties_in = data;

	if (ngx_strcmp(properties_in->data, "") == 0) {
		return NGX_CONF_ERROR;
	}

	int i = 0;

	char *tok = strtok(properties_in->data, ",");
	while (tok != NULL) {
		properties_array[i++] = tok;
		tok = strtok(NULL, ",");
	}
	number_of_properties = i;
	printf("51Degrees properties set to:\n");
	for (i=0;i<number_of_properties;i++)
    {
        printf("    property %d = %s\n", i+1, properties_array[i]);
    }
	return NGX_CONF_OK;

}

//Init function, initialises the detector.
static char *
ngx_http_51D_init(ngx_conf_t *cf, void *post, void *data)
{
//TODO: Trap and report on initialisation errors returned by fiftyoneDegreesInitWithPropertyArray.
	switch(fiftyoneDegreesInitWithPropertyArray(dataFile, &dataSet, properties_array, number_of_properties)) {
		default:
			cache = fiftyoneDegreesResultsetCacheCreate(&dataSet, cacheSize);
			pool = fiftyoneDegreesWorksetPoolCreate(&dataSet, cache, poolSize);
			break;
	}
	printf("51Degrees mobile detector initialized\n");
	char msg[100];
	sprintf(msg, "initialized, dataFile=%s cacheSize=%d poolSize=%d\n", dataFile, cacheSize, poolSize);
    fod_log(msg);
	return NGX_CONF_OK;
}

//Set filePath function, is passed path to data file
//as a string and assigns to the variable 'dataFile'.
static char *
ngx_http_51D_filePath(ngx_conf_t *cf, void *post, void *data)
{
	ngx_http_core_loc_conf_t *clcf;

	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_51D_handler;
	ngx_str_t *dataFile_in = data;
	dataFile = dataFile_in->data;
	printf("51Degrees data file set to: %s\n", dataFile);
	return NGX_CONF_OK;
}

//Set cacheSize function, is passed cache size as string,
//converts to int and assigns to the variable 'cacheSize'.
static char *
ngx_http_51D_cache(ngx_conf_t *cf, void *post, void *data)
{
	ngx_http_core_loc_conf_t *clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_51D_handler;
	ngx_str_t *cache_in = data;
	cacheSize = atoi(cache_in->data);
	printf("51Degrees cache size set to: %d\n", cacheSize);
	return NGX_CONF_OK;
}

//Set poolSize function, is passed pool size as string,
//converts to int and assigns to the variable 'poolSize'.
static char *
ngx_http_51D_pool(ngx_conf_t *cf, void *post, void *data)
{
	ngx_http_core_loc_conf_t *clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_51D_handler;
	ngx_str_t *pool_in = data;
	poolSize = atoi(pool_in->data);
	printf("51Degrees pool size set to: %d\n", poolSize);
	return NGX_CONF_OK;
}

//Simple string join function, is used when setting
//prefixed headers.
char *join(ngx_pool_t* ngx_pool, const char* s1, const char* s2)
{
//TODO: Change to not use malloc.
    //char* result = ngx_palloc(strlen(s1) + strlen(s2) + 1);
    char* result = ngx_pcalloc(ngx_pool, ngx_strlen(s1) + ngx_strlen(s2) + 1);
    if (result)
    {
        strcpy(result, s1);
        strcat(result, s2);
    }
    return result;
}

void fod_log(char *msg)
{
	FODlog = fopen("FOD.log", "a");
	fprintf(FODlog, msg);
	fclose(FODlog);
}
