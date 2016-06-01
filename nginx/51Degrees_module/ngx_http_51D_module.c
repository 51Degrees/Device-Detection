#include <nginx.h>
//#define FIFTYONEDEGREES_PATTERN
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_rbtree.h>
#include <ngx_resolver.h>
#include <ngx_string.h>
#ifdef FIFTYONEDEGREES_PATTERN
#include "src/pattern/51Degrees.h"
#endif // FIFTYONEDEGREES_PATTERN
#ifdef FIFTYONEDEGREES_TRIE
#include "src/trie/51Degrees.h"
#endif // FIFTYONEDEGREES_TRIE

// Define default settings.
#define FIFTYONEDEGREES_DEFAULTFILE (u_char*) "51Degrees.dat"
#ifndef FIFTYONEDEGREES_PROPERTY_NOT_AVAILABLE
#define FIFTYONEDEGREES_PROPERTY_NOT_AVAILABLE "NA"
#endif // FIFTYONEDEGREES_PROPERTY_NOT_AVAILABLE
#ifndef FIFTYONEDEGREES_MAX_STRING
#define FIFTYONEDEGREES_MAX_STRING 100
#endif // FIFTYONEDEGREES_MAX_STRING

// Module config functions to enable matching in selected locations.
static char *ngx_http_51D_set(ngx_conf_t* cf, ngx_command_t *cmd, void *conf);

// Module declaration.
ngx_module_t ngx_http_51D_module;

// Configuration function declarations.
static void *ngx_http_51D_create_main_conf(ngx_conf_t *cf);
static void *ngx_http_51D_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_51D_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);

// Handler declaration.
static ngx_int_t ngx_http_51D_handler(ngx_http_request_t *r);
static ngx_shm_zone_t *ngx_http_51D_shm_dataSet, *ngx_http_51D_shm_cache;
ngx_atomic_t ngx_http_51D_shm_tag = 1;

static ngx_int_t ngx_http_51D_init_shm_dataSet(ngx_shm_zone_t *shm_zone, void *data);
static ngx_int_t ngx_http_51D_init_shm_cache(ngx_shm_zone_t *shm_zone, void *data);

static int ngx_http_51D_total_headers_to_set = -1;
static int ngx_http_51D_cacheSize;
typedef struct ngx_http_51D_header_to_set_t {
    ngx_uint_t multi;
    ngx_uint_t propertyCount;
    ngx_str_t **property;
    ngx_str_t name;
    ngx_str_t lowerName;
#ifdef FIFTYONEDEGREES_PATTERN
    size_t maxString;
#endif // FIFTYONEDEGREES_PATTERN
} ngx_http_51D_header_to_set;

typedef struct {
	ngx_str_t name;
	u_char* lowerName;
	ngx_str_t value;
} ngx_http_51D_matched_header_t;

// Module location config.
typedef struct {
	uint32_t key;
	ngx_uint_t hasMulti;
    ngx_uint_t headerCount;
    ngx_http_51D_header_to_set **header;
} ngx_http_51D_loc_conf_t;

// Module main config.
typedef struct {
	char properties[FIFTYONEDEGREES_MAX_STRING];
    ngx_str_t dataFile;
#ifdef FIFTYONEDEGREES_PATTERN
    ngx_uint_t cacheSize;
    fiftyoneDegreesWorkset *ws;
#endif // FIFTYONEDEGREES_PATTERN
	fiftyoneDegreesDataSet *dataSet;
} ngx_http_51D_main_conf_t;


typedef struct {
	ngx_rbtree_node_t node;
	ngx_str_t name;
	ngx_uint_t headersCount;
	ngx_http_51D_matched_header_t** header;
} ngx_http_51D_cache_node_t;

typedef struct ngx_http_51D_cache_lru_list_s ngx_http_51D_cache_lru_list_t;

struct ngx_http_51D_cache_lru_list_s {
	ngx_http_51D_cache_node_t *node;
	ngx_http_51D_cache_lru_list_t *prev;
	ngx_http_51D_cache_lru_list_t *next;
};

typedef struct {
	ngx_http_51D_cache_lru_list_t *lru;
	ngx_rbtree_t *tree;
} ngx_http_51D_cache_t;

size_t ngx_http_51D_get_cache_shm_size(int cacheSize)
{
	size_t size = 0;
	size += sizeof(ngx_shm_zone_t);
	size += sizeof(ngx_http_51D_cache_t);
	size += sizeof(ngx_http_51D_cache_lru_list_t) * cacheSize;
	size += sizeof(ngx_rbtree_t);
	size += sizeof(ngx_http_51D_cache_node_t) * cacheSize;
	size += sizeof(u_char) * FIFTYONEDEGREES_MAX_STRING * 4 * cacheSize;

	return size;
}

/**
 * Module post config. Adds the module to the HTTP access phase array and
 * and allocates a fresh shared memory zone to store the provider in.
 */
static ngx_int_t
ngx_http_51D_post_conf(ngx_conf_t *cf)
{
	ngx_http_handler_pt *h;
	ngx_http_core_main_conf_t *cmcf;
	ngx_http_51D_main_conf_t *fdmcf;
	ngx_atomic_int_t tagOffset;
	ngx_str_t dataSetName, cacheName;

	cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);
	fdmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_51D_module);

	h = ngx_array_push(&cmcf->phases[NGX_HTTP_ACCESS_PHASE].handlers);
	if (h == NULL) {
		return NGX_ERROR;
	}

	*h = ngx_http_51D_handler;

	// Set default data file if necessary.
	if ((int)fdmcf->dataFile.len < 0) {
		fdmcf->dataFile.data = FIFTYONEDEGREES_DEFAULTFILE;
		fdmcf->dataFile.len = ngx_strlen(fdmcf->dataFile.data);
	}
#ifdef FIFTYONEDEGREES_PATTERN
	if ((int)fdmcf->cacheSize < 0) {
		fdmcf->cacheSize = 0;
	}
	ngx_http_51D_cacheSize = (int)fdmcf->cacheSize;
#endif // FIFTYONEDEGREES_PATTERN

	dataSetName.data = (u_char*) "51Degrees Shared Data Set";
	dataSetName.len = ngx_strlen(dataSetName.data);
	cacheName.data = (u_char*) "51Degrees Shared Cache";
	cacheName.len = ngx_strlen(cacheName.data);
	tagOffset = ngx_atomic_fetch_add(&ngx_http_51D_shm_tag, (ngx_atomic_int_t)1);
#ifdef FIFTYONEDEGREES_PATTERN
	size_t size = (size_t)fiftyoneDegreesGetProviderSizeWithPropertyString((const char*)fdmcf->dataFile.data, (const char*)fdmcf->properties, 0, 0);
	//size_t cacheSize= (size_t)ngx_http_51D_get_cache_shm_size(ngx_http_51D_cacheSize);
#endif // FIFTYONEDEGREES_PATTERN
#ifdef FIFTYONEDEGREES_TRIE
	size_t size = (size_t)fiftyoneDegreesGetDataSetSizeWithPropertyString((const char*)fdmcf->dataFile.data, (const char*)fdmcf->properties);
#endif // FIFTYONEDEGREES_TRIE
	if ((int)size < 1) {
		ngx_log_stderr(NGX_ERROR_ERR, "no data file");
		return NGX_ERROR;
	}
	size *= 1.1;
	ngx_http_51D_shm_dataSet = ngx_shared_memory_add(cf, &dataSetName, size, &ngx_http_51D_module + tagOffset);
	ngx_http_51D_shm_dataSet->init = ngx_http_51D_init_shm_dataSet;
	ngx_http_51D_shm_cache = ngx_shared_memory_add(cf, &cacheName, 10000000, &ngx_http_51D_module + tagOffset);
	ngx_http_51D_shm_cache->init = ngx_http_51D_init_shm_cache;

	return NGX_OK;
}

/**
 * Create main config. Allocates memory to the configuration and initialises
 * cacheSize and poolSize to -1 (unset).
 */
static void *
ngx_http_51D_create_main_conf(ngx_conf_t *cf)
{
    ngx_http_51D_main_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_51D_main_conf_t));
    if (conf == NULL) {
        return NULL;
    }
    ngx_log_debug0(NGX_LOG_DEBUG_ALL, cf->log, 0, "51Degrees create main");
#ifdef FIFTYONEDEGREES_PATTERN
	conf->cacheSize = NGX_CONF_UNSET_UINT;
#endif // FIFTYONEDEGREES_PATTERN
    return conf;
}

/**
 * Count matches function. Counts the number of matches defined in the config
 * file. This is used in create location conf to determine how many match
 * structures to allocate.
 */
static void ngx_http_51D_count_matches(ngx_conf_t *cf)
{
	const char *buffer = (char*)cf->conf_file->buffer->start;
	const char* searchString = "51D_match";
	char *header;

	ngx_http_51D_total_headers_to_set = 0;
	header = strstr(buffer, searchString);
	while (header != NULL) {
		ngx_http_51D_total_headers_to_set++;
		header = strstr((const char*)header + strlen(searchString), searchString);
	}
}

/**
 * Create location config. Allocates memory to the configuration and each header
 * structure. Initialises count to -1 (unset).
 */
static void *
ngx_http_51D_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_51D_loc_conf_t *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_51D_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }
    if (ngx_http_51D_total_headers_to_set < 0) {
		ngx_http_51D_count_matches(cf);
    }
    conf->key = 0;
    conf->hasMulti = 0;
	conf->headerCount = 0;
	conf->header = (ngx_http_51D_header_to_set**)ngx_palloc(cf->pool, sizeof(ngx_http_51D_header_to_set*) * ngx_http_51D_total_headers_to_set);

    return conf;
}

/**
 * Merge location config. Either gets the value of count that is set, or sets
 * to the default of 0.
 */
static char *
ngx_http_51D_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
	ngx_http_51D_loc_conf_t  *prev = parent;
	ngx_http_51D_loc_conf_t  *conf = child;

	ngx_conf_merge_uint_value(conf->headerCount, prev->headerCount, 0);

	return NGX_CONF_OK;
}

/**
 * Throws an error if data set initialisation fails. Used by the init module
 * function.
 */
static ngx_int_t reportDatasetInitStatus(ngx_cycle_t *cycle, fiftyoneDegreesDataSetInitStatus status,
										const char* fileName) {
	switch (status) {
	case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
		ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "Insufficient memory to load '%s'.", fileName);
		break;
	case DATA_SET_INIT_STATUS_CORRUPT_DATA:
		ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "Device data file '%s' is corrupted.", fileName);
		break;
	case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
		ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "Device data file '%s' is not correct version.", fileName);
		break;
	case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
		ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "Device data file '%s' not found.", fileName);
		break;
#ifdef FIFTYONEDEGREES_PATTERN
	case DATA_SET_INIT_STATUS_NULL_POINTER:
		ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "Null pointer to the existing dataset or memory location.");
		break;
	case DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS:
		ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "Allocated continuous memory containing 51Degrees data file "
			"appears to be smaller than expected. Most likely because the"
			" data file was not fully loaded into the allocated memory.");
		break;
#endif // FIFTYONEDEGREES_PATTERN
	default:
		ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "Device data file '%s' could not be loaded.", fileName);
		break;
	}
	return NGX_ERROR;
}

/**
 * Init shared memory zone. Allocates space for the provider in the shared
 * memory zone.
 */
static ngx_int_t ngx_http_51D_init_shm_dataSet(ngx_shm_zone_t *shm_zone, void *data)
{
	ngx_slab_pool_t *shpool;
	fiftyoneDegreesDataSet *dataSet;
	shpool = (ngx_slab_pool_t *) ngx_http_51D_shm_dataSet->shm.addr;
	if (data) {
		shm_zone->data = data;
		ngx_log_error(NGX_LOG_ERR, shm_zone->shm.log, 0, "51Degrees shared memory exists and has not been reinitialised correctly.");
		return NGX_ERROR;
	}
	dataSet = (fiftyoneDegreesDataSet*)ngx_slab_alloc(shpool, sizeof(fiftyoneDegreesDataSet));
	shm_zone->data = dataSet;
	if (dataSet == NULL) {
		ngx_log_error(NGX_LOG_ERR, shm_zone->shm.log, 0, "51Degrees shared memory could not be allocated.");
		return NGX_ERROR;
	}
	ngx_log_debug1(NGX_LOG_DEBUG_ALL, shm_zone->shm.log, 0, "51Degrees initialised shared memory with size %d.", shm_zone->shm.size);

	return NGX_OK;
}

void
ngx_http_51D_insert_node(ngx_rbtree_node_t *temp,
    ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel)
{
    ngx_http_51D_cache_node_t      *n, *t;
    ngx_rbtree_node_t  **p;

    for ( ;; ) {

        n = (ngx_http_51D_cache_node_t *) node;
        t = (ngx_http_51D_cache_node_t *) temp;

        if (node->key != temp->key) {

            p = (node->key < temp->key) ? &temp->left : &temp->right;

        } else if (n->name.len != t->name.len) {

            p = (n->name.len < t->name.len) ? &temp->left : &temp->right;

        } else {
            p = (ngx_memcmp(n->name.data, t->name.data, n->name.len) < 0)
                 ? &temp->left : &temp->right;
        }

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    ngx_rbt_red(node);
}

static ngx_int_t ngx_http_51D_init_shm_cache(ngx_shm_zone_t *shm_zone, void *data)
{
	ngx_slab_pool_t *shpool;
	ngx_rbtree_t *tree;
	ngx_rbtree_node_t *sentinel;
	ngx_http_51D_cache_t *cache;

	if (data) {
		shm_zone->data = data;
		return NGX_OK;
	}

	shpool = (ngx_slab_pool_t*) shm_zone->shm.addr;
	tree = ngx_slab_alloc(shpool, sizeof(ngx_rbtree_t));
	if (tree == NULL) {
		return NGX_ERROR;
	}

	sentinel = ngx_slab_alloc(shpool, sizeof(ngx_rbtree_node_t));
	if (sentinel == NULL) {
		return NGX_ERROR;
	}

	ngx_rbtree_sentinel_init(sentinel);
	tree->root = sentinel;
	tree->sentinel = sentinel;
	tree->insert = ngx_http_51D_insert_node;

	cache = (ngx_http_51D_cache_t*)ngx_slab_alloc(shpool, sizeof(ngx_http_51D_cache_t));
	cache->tree = tree;
	cache->lru = (ngx_http_51D_cache_lru_list_t*)ngx_slab_alloc(shpool, sizeof(ngx_http_51D_cache_lru_list_t));
	cache->lru->node = NULL;

	int i = 1;
	ngx_http_51D_cache_lru_list_t *current;
	current = cache->lru;
	while (i < ngx_http_51D_cacheSize) {
		current->next = (ngx_http_51D_cache_lru_list_t*)ngx_slab_alloc(shpool, sizeof(ngx_http_51D_cache_lru_list_t));
		current->next->prev = current;
		current->next->node = NULL;
		current = current->next;
		i++;
	}
	current->next = NULL;
	cache->lru->prev = current;
	//todo mutex.

	shm_zone->data = cache;

	return NGX_OK;
}

static ngx_http_51D_cache_node_t *
ngx_http_51D_lookup_node(ngx_rbtree_t *rbtree, ngx_str_t *name, uint32_t hash)
{
    ngx_int_t           rc;
    ngx_http_51D_cache_node_t     *n;
    ngx_rbtree_node_t  *node, *sentinel;

    node = rbtree->root;
    sentinel = rbtree->sentinel;

    while (node != sentinel) {

        n = (ngx_http_51D_cache_node_t*) node;

        if (hash != node->key) {
            node = (hash < node->key) ? node->left : node->right;
            continue;
        }

        if (name->len != n->name.len) {
            node = (name->len < n->name.len) ? node->left : node->right;
            continue;
        }

        rc = ngx_memcmp(name->data, n->name.data, name->len);

        if (rc < 0) {
            node = node->left;
            continue;
        }

        if (rc > 0) {
            node = node->right;
            continue;
        }

        return n;
    }

    return NULL;
}

ngx_http_51D_cache_node_t *ngx_http_51D_lookup_node_lru(ngx_str_t *name, int32_t hash)
{
	ngx_http_51D_cache_node_t *node;
	ngx_http_51D_cache_lru_list_t *lruItem;
	ngx_slab_pool_t *shpool = (ngx_slab_pool_t*)ngx_http_51D_shm_cache->shm.addr;
	ngx_http_51D_cache_t *cache = (ngx_http_51D_cache_t*)ngx_http_51D_shm_cache->data;

	node = ngx_http_51D_lookup_node(cache->tree, name, hash);
	if (node != NULL && node != cache->lru->node) {
		ngx_shmtx_lock(&shpool->mutex);
		// Node exists in the cache, and is not at the top of the lru.
		lruItem = cache->lru;
		while (lruItem->next != NULL) {
			if (lruItem->node == node) {
				// Found node in the lru, so move it to the top.
				lruItem->next->prev = lruItem->prev;
				lruItem->prev->next = lruItem->next;
				lruItem->prev = cache->lru->prev;
				lruItem->next = cache->lru;
				cache->lru->prev = lruItem;
				cache->lru = lruItem;
				break;
			}
			lruItem = lruItem->next;
		}
		ngx_shmtx_unlock(&shpool->mutex);
	}
	return node;
}

/**
 * Shared memory alloc function. Replaces fiftyoneDegreesMalloc to store
 * the data set in the shared memory zone.
 */
void *ngx_http_51D_shm_alloc(size_t __size)
{
	void *ptr = NULL;
	ngx_slab_pool_t *shpool;
	shpool = (ngx_slab_pool_t *) ngx_http_51D_shm_dataSet->shm.addr;
	ptr = ngx_slab_alloc_locked(shpool, __size);
	ngx_log_debug2(NGX_LOG_DEBUG_ALL, ngx_cycle->log, 0, "51Degrees shm alloc %d %p", __size, ptr);
	if (ptr == NULL) {
		ngx_log_error(NGX_LOG_ERR, ngx_cycle->log, 0, "51Degrees shm failed to allocate memory, not enough shared memory.");
	}
	return ptr;
}

/**
 * Shared memory free function. Replaces fiftyoneDegreesFree to free pointers
 * to the shared memory zone.
 */
void ngx_http_51D_shm_free(void *__ptr)
{
	ngx_slab_pool_t *shpool;
	shpool = (ngx_slab_pool_t *) ngx_http_51D_shm_dataSet->shm.addr;
	ngx_log_debug1(NGX_LOG_DEBUG_ALL, ngx_cycle->log, 0, "51Degrees shm free %p", __ptr);
	if ((u_char *) __ptr < shpool->start || (u_char *) __ptr > shpool->end) {
	ngx_log_debug1(NGX_LOG_DEBUG_ALL, ngx_cycle->log, 0, "51Degrees shm free (non shared) %p", __ptr);
		free(__ptr);
	}
	else {
		ngx_slab_free_locked(shpool, __ptr);
	}
}

#ifdef FIFTYONEDEGREES_PATTERN
/**
 * Allocate string function. Gets the maximum possible length of the returned
 * list of values for specified header structure and allocates space for it.
 */
static void
ngx_http_51D_set_max_string(const fiftyoneDegreesDataSet *dataSet, ngx_http_51D_header_to_set *header)
{
	ngx_uint_t i, tmpLength, length = 0;
	for (i = 0; i < header->propertyCount; i++) {
		if (i != 0) {
			length++;
		}
		tmpLength = fiftyoneDegreesGetMaxValueLength(dataSet, (char*)header->property[i]->data);
	tmpLength = 20;
		if ((int)tmpLength > 0) {
			length += (int)tmpLength;
		}
		else {
			length += ngx_strlen(FIFTYONEDEGREES_PROPERTY_NOT_AVAILABLE);
		}
	}
	header->maxString = (size_t) length * sizeof(char);
}
#endif // FIFTYONEDEGREES_PATTERN

/**
 * Init module function. Initialises the provider with the given initialisation
 * parameters. Throws an error if the data set could not be initialised.
 */
static ngx_int_t
ngx_http_51D_init_module(ngx_cycle_t *cycle)
{
	fiftyoneDegreesDataSetInitStatus status;
	ngx_http_51D_main_conf_t *fdmcf;

	// Get module main config.
	fdmcf = ngx_http_cycle_get_module_main_conf(cycle, ngx_http_51D_module);

	fdmcf->dataSet = (fiftyoneDegreesDataSet*)ngx_http_51D_shm_dataSet->data;

	// Set the memory allocation functions to use the shared memory zone.
	fiftyoneDegreesMalloc = ngx_http_51D_shm_alloc;
	fiftyoneDegreesFree = ngx_http_51D_shm_free;

	// Initialise the provider.
	//fdmcf->cache = fiftyoneDegreesResultsetCacheCreate(fdmcf->dataSet, (int)fdmcf->cacheSize);
	status = fiftyoneDegreesInitWithPropertyString((const char*)fdmcf->dataFile.data, fdmcf->dataSet, (const char*)fdmcf->properties);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return reportDatasetInitStatus(cycle, status, (const char*)fdmcf->dataFile.data);
	}
	ngx_log_debug2(NGX_LOG_DEBUG_ALL, cycle->log, 0, "51Degrees initialised from file '%s' with properties '%s'.", (char*)fdmcf->dataFile.data, fdmcf->properties);

	fiftyoneDegreesMalloc = malloc;
	fiftyoneDegreesFree = free;

	return NGX_OK;
}

/**
 * Definitions of functions which can be called from the config file.
 * --51D_match_single takes two string arguments, the name of the header
 * to be set and a comma separated list of properties to be returned.
 * Is called within location block. Enables User-Agent matching.
 * --51D_match_all takes two string arguments, the name of the header to
 * be set and a comma separated list of properties to be returned. Is
 * called within location block. Enables multiple http header matching.
 * --51D_filePath takes one string argument, the path to a
 * 51Degrees data file. Is called within server block.
 * --51D_cache takes one integer argument, the size of the
 * 51Degrees cache.
 * --51D_pool takes one integer argument, the size of the
 * 51Degrees pool.
 */
static ngx_command_t  ngx_http_51D_commands[] = {

	{ ngx_string("51D_match_single"),
	NGX_HTTP_LOC_CONF|NGX_CONF_TAKE2,
	ngx_http_51D_set,
	NGX_HTTP_LOC_CONF_OFFSET,
    0,
	NULL },

	{ ngx_string("51D_match_all"),
	NGX_HTTP_LOC_CONF|NGX_CONF_TAKE2,
	ngx_http_51D_set,
	NGX_HTTP_LOC_CONF_OFFSET,
    0,
	NULL },

	{ ngx_string("51D_filePath"),
	NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
	ngx_conf_set_str_slot,
	NGX_HTTP_MAIN_CONF_OFFSET,
	offsetof(ngx_http_51D_main_conf_t, dataFile),
	NULL },

#ifdef FIFTYONEDEGREES_PATTERN
	{ ngx_string("51D_cache"),
	NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
	ngx_conf_set_num_slot,
	NGX_HTTP_MAIN_CONF_OFFSET,
	offsetof(ngx_http_51D_main_conf_t, cacheSize),
	NULL },
#endif // FIFTYONEDEGREES_PATTERN

	ngx_null_command
};

/**
 * Module context. Sets the configuration functions.
 */
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

#ifdef FIFTYONEDEGREES_PATTERN
static ngx_int_t
ngx_http_51D_init_process(ngx_cycle_t *cycle)
{
	ngx_http_51D_main_conf_t *fdmcf;
	fdmcf = ngx_http_cycle_get_module_main_conf(cycle, ngx_http_51D_module);

	fdmcf->ws = fiftyoneDegreesWorksetCreate(fdmcf->dataSet, NULL);

	return NGX_OK;
}

void
ngx_http_51D_exit_process(ngx_cycle_t *cycle)
{
	ngx_http_51D_main_conf_t *fdmcf;
	fdmcf = ngx_http_cycle_get_module_main_conf(cycle, ngx_http_51D_module);

	fiftyoneDegreesWorksetFree(fdmcf->ws);
}
#endif // FIFTYONEDEGREES_PATTERN

/**
 * Module definition. Set the module context, commands, type and init function.
 */
ngx_module_t ngx_http_51D_module = {
	NGX_MODULE_V1,
	&ngx_http_51D_module_ctx,      /* module context */
	ngx_http_51D_commands,         /* module directives */
	NGX_HTTP_MODULE,               /* module type */
	NULL,                          /* init master */
	ngx_http_51D_init_module,      /* init module */
#ifdef FIFTYONEDEGREES_PATTERN
	ngx_http_51D_init_process,     /* init process */
#else
	NULL,                          /* init process */
#endif // FIFTYONEDEGREES_PATTERN
	NULL,                          /* init thread */
	NULL,                          /* exit thread */
#ifdef FIFTYONEDEGREES_PATTERN
	ngx_http_51D_exit_process,     /* exit process */
#else
	NULL,                          /* exit process */
#endif // FIFTYONEDEGREES_PATTERN
	NULL,                          /* exit master */
	NGX_MODULE_V1_PADDING
};

/**
 * Search headers function. Searched request headers for the name supplied.
 * Used when matching multiple http headers to find important headers.
 * See:
 * https://www.nginx.com/resources/wiki/start/topics/examples/headers_management
 */
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

/**
 * Add value function. Appends a string to a comma separated list.
 */
static void add_value(char *val, char *buffer)
{
	if (buffer[0] != '\0') {
		strcat(buffer, ",");
	}
	strcat(buffer, val);
}

/**
 * Get match function. Gets a match from the workset for either a single
 * User-Agent or all request headers.
 */
#ifdef FIFTYONEDEGREES_PATTERN
void ngx_http_51D_get_match(fiftyoneDegreesWorkset *ws, ngx_http_request_t *r, int multi)
{
	int headerIndex;
	ngx_table_elt_t *searchResult;

	// If single requested, match for single User-Agent.
	if (multi == 0) {
	ngx_log_debug1(NGX_LOG_DEBUG_ALL, ngx_cycle->log, 0, "51Degrees match get %p", ws);
		if (r->headers_in.user_agent) {
			fiftyoneDegreesMatch(ws, (const char*)r->headers_in.user_agent[0].value.data);
		}
		else {
			fiftyoneDegreesMatch(ws, "");
		}
		ngx_log_debug1(NGX_LOG_DEBUG_ALL, ngx_cycle->log, 0, "51Degrees match got %p", ws);
	}
	// If multi requested, match for multiple http headers.
	else if (multi == 1) {
		ws->importantHeadersCount = 0;
		for (headerIndex = 0; headerIndex < ws->dataSet->httpHeadersCount; headerIndex++) {
			searchResult = search_headers_in(r, (u_char*)ws->dataSet->httpHeaders[headerIndex].headerName, strlen(ws->dataSet->httpHeaders[headerIndex].headerName));
			if (searchResult) {
				ws->importantHeaders[ws->importantHeadersCount].header = ws->dataSet->httpHeaders + headerIndex;
				ws->importantHeaders[ws->importantHeadersCount].headerValue = (const char*) searchResult->value.data;
				ws->importantHeaders[ws->importantHeadersCount].headerValueLength = searchResult->value.len;
				ws->importantHeadersCount++;
			}
			fiftyoneDegreesMatchForHttpHeaders(ws);
		}
	}
}
#endif // FIFTYONEDEGREES_PATTERN

#ifdef FIFTYONEDEGREES_TRIE
void ngx_http_51D_get_match(fiftyoneDegreesDataSet *dataSet, ngx_http_request_t *r, int multi, fiftyoneDegreesDeviceOffsets *offsets)
{
	int headerIndex;

	// If single requested, match for single User-Agent.
	if (multi == 0) {
		offsets->size = 1;
		if (r->headers_in.user_agent) {
            fiftyoneDegreesSetDeviceOffset(dataSet, (const char*)r->headers_in.user_agent[0].value.data, 0, offsets->firstOffset);
		}
		else {
			fiftyoneDegreesSetDeviceOffset(dataSet, "", 0, offsets->firstOffset);
		}
	}
	// If multi requested, match for multiple http headers.
	else if (multi == 1) {
		headerIndex = 0;
		offsets->size = 0;
		ngx_table_elt_t *httpHeaderValue;
		const char *httpHeaderName = fiftyoneDegreesGetHttpHeaderNamePointer(dataSet, headerIndex);
		while (httpHeaderName != NULL) {
			httpHeaderValue = search_headers_in(r, (u_char*)httpHeaderName, ngx_strlen(httpHeaderName));
			if (httpHeaderValue != NULL) {
				fiftyoneDegreesSetDeviceOffset(
					dataSet,
					(const char*)httpHeaderValue->value.data,
					headerIndex,
					&offsets->firstOffset[offsets->size]);
				offsets->size++;
			}
			headerIndex++;
			httpHeaderName = fiftyoneDegreesGetHttpHeaderNamePointer(dataSet, headerIndex);
		}
	}
}
#endif // FIFTYONEDEGREES_TRIE
/**
 * Get value function. Gets the requested value for the current match and
 * appends the value to the comma separated list of values.
 */
 #ifdef FIFTYONEDEGREES_PATTERN
void ngx_http_51D_get_value(fiftyoneDegreesWorkset *ws, char *values_string, const char *requiredPropertyName)
{
	char *methodName, *propertyName;
	char buffer[24];
	int i, found = 0;
	if (strcmp("Method", requiredPropertyName) == 0) {
		switch(ws->method) {
			case EXACT: methodName = "Exact"; break;
			case NUMERIC: methodName = "Numeric"; break;
			case NEAREST: methodName = "Nearest"; break;
			case CLOSEST: methodName = "Closest"; break;
			default:
			case NONE: methodName = "None"; break;
		}
		add_value(methodName, values_string);
		found = 1;
	}
	else if (strcmp("Difference", requiredPropertyName) == 0) {
		sprintf(buffer, "%d", ws->difference);
		add_value(buffer, values_string);
		found = 1;
	}
	else if (strcmp("Rank", requiredPropertyName) == 0) {
		sprintf(buffer, "%d", fiftyoneDegreesGetSignatureRank(ws));
		add_value(buffer, values_string);
		found = 1;
	}
	else if (strcmp("DeviceId", requiredPropertyName) == 0) {
			fiftyoneDegreesGetDeviceId(ws, buffer, 24);
			add_value(buffer, values_string);
			found = 1;

	}
	else {
		for (i = 0; i < ws->dataSet->requiredPropertyCount; i++) {
			propertyName = (char*)fiftyoneDegreesGetPropertyName(ws->dataSet, ws->dataSet->requiredProperties[i]);
			if (strcmp(propertyName, requiredPropertyName) == 0) {
				fiftyoneDegreesSetValues(ws, i);
				add_value((char*)fiftyoneDegreesGetValueName(ws->dataSet, *ws->values), values_string);
				found = 1;
				break;
			}
		}
		if (!found) {
			add_value(FIFTYONEDEGREES_PROPERTY_NOT_AVAILABLE, values_string);
		}
	}
}
#endif // FIFTYONEDEGREES_PATTERN
#ifdef FIFTYONEDEGREES_TRIE
void ngx_http_51D_get_value(fiftyoneDegreesDataSet *dataSet, char *values_string, const char *requiredPropertyName, fiftyoneDegreesDeviceOffsets *offsets)
{
	int requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(dataSet, requiredPropertyName);
	if (requiredPropertyIndex >= 0 &&
			requiredPropertyIndex <
			fiftyoneDegreesGetRequiredPropertiesCount(dataSet)) {
		char *value = (char*)fiftyoneDegreesGetValuePtrFromOffsets(
			dataSet,
			offsets,
			requiredPropertyIndex);
		if (value != NULL) {
			add_value(value, values_string);
		}
	}
	else {
		add_value(FIFTYONEDEGREES_PROPERTY_NOT_AVAILABLE, values_string);
	}
}
#endif // FIFTYONEDEGREES_TRIE

void ngx_http_51D_cache_clean(ngx_http_51D_cache_t *cache, int count)
{
	int i = 0, j;
	ngx_http_51D_cache_node_t *node;
	ngx_slab_pool_t *shpool = (ngx_slab_pool_t*)ngx_http_51D_shm_cache->shm.addr;
	ngx_http_51D_cache_lru_list_t *lruItem = cache->lru->prev;

	while (lruItem->node == NULL) {
		lruItem = lruItem->prev;
	}

	while (i < count) {
		node = lruItem->node;
		ngx_rbtree_delete(cache->tree, (ngx_rbtree_node_t*)node);

		// todo free all headers.
		for (j = 0; j < (int)node->headersCount; j++) {
			ngx_slab_free_locked(shpool, node->header[j]->lowerName);
			ngx_slab_free_locked(shpool, node->header[j]->name.data);
			ngx_slab_free_locked(shpool, node->header[j]->value.data);
			ngx_slab_free_locked(shpool, node->header[j]);
		}
		ngx_slab_free_locked(shpool, node->name.data);
		ngx_slab_free_locked(shpool, node->header);
		ngx_slab_free_locked(shpool, node);
		lruItem->node = NULL;
		lruItem = lruItem->prev;
		i++;
	}
}

void *ngx_http_51D_cache_alloc(size_t __size)
{
	ngx_slab_pool_t *shpool = (ngx_slab_pool_t*)ngx_http_51D_shm_cache->shm.addr;
	ngx_http_51D_cache_t *cache = (ngx_http_51D_cache_t*)ngx_http_51D_shm_cache->data;
	void *ptr = ngx_slab_alloc(shpool, __size);
	if (ptr == NULL) {
		ngx_shmtx_lock(&shpool->mutex);
		ngx_http_51D_cache_clean(cache, 10);
		ngx_slab_alloc_locked(shpool, __size);
		ngx_shmtx_unlock(&shpool->mutex);
	}
	return ptr;
}

void ngx_http_51D_add_header_to_node(ngx_http_51D_cache_node_t *node, ngx_table_elt_t *h)
{
	ngx_http_51D_matched_header_t *header;
	header = node->header[(int)node->headersCount];
	header->name.data = (u_char*)ngx_http_51D_cache_alloc(sizeof(u_char) *(h->key.len + 1));
	ngx_cpystrn(header->name.data, h->key.data, h->key.len + 1);
	header->name.len = h->key.len;
	header->lowerName = (u_char*)ngx_http_51D_cache_alloc(sizeof(u_char) * (ngx_strlen(h->lowcase_key) + 1));
	ngx_cpystrn(header->lowerName, h->lowcase_key, ngx_strlen(h->lowcase_key) + 1);
	//todo clean cache if no space
	header->value.data = (u_char*)ngx_http_51D_cache_alloc(h->value.len);
	ngx_cpystrn(header->value.data, h->value.data, h->value.len +1);
	header->value.len = h->value.len;
	node->headersCount++;

}

ngx_http_51D_cache_node_t *ngx_http_51D_create_node(ngx_str_t *name, uint32_t hash, int headerCount)
{
	int i;
	ngx_http_51D_cache_node_t *node;
	node = (ngx_http_51D_cache_node_t*)ngx_http_51D_cache_alloc(sizeof(ngx_http_51D_cache_node_t));
	node->node.key = hash;
	node->headersCount = 0;
	node->header = (ngx_http_51D_matched_header_t**)ngx_http_51D_cache_alloc(sizeof(ngx_http_51D_matched_header_t*)*headerCount);
	for (i = 0; i < headerCount; i++) {
		node->header[i] = (ngx_http_51D_matched_header_t*)ngx_http_51D_cache_alloc(sizeof(ngx_http_51D_matched_header_t));
	}

	node->name.data = (u_char*)ngx_http_51D_cache_alloc(name->len + 1);
	ngx_cpystrn(node->name.data, name->data, name->len + 1);
	node->name.len = name->len;
	return node;
}

void ngx_http_51D_cache_insert(ngx_http_51D_cache_node_t *node)
{
	ngx_slab_pool_t *shpool = (ngx_slab_pool_t*)ngx_http_51D_shm_cache->shm.addr;
	ngx_http_51D_cache_t *cache = (ngx_http_51D_cache_t*)ngx_http_51D_shm_cache->data;
	ngx_shmtx_lock(&shpool->mutex);

	if (cache->lru->prev->node != NULL) {
		// The cache is full, so purge the last 3 from the lru.
		ngx_http_51D_cache_clean(cache, 3);
	}
	ngx_rbtree_insert(cache->tree, (ngx_rbtree_node_t*)node);

	// Add node to lru.
	cache->lru->prev->next = cache->lru;
	cache->lru->prev->node = node;
	cache->lru->prev->prev->next = NULL;
	cache->lru = cache->lru->prev;

	ngx_shmtx_unlock(&shpool->mutex);
}

/**
 * Module handler, gets a match using either the User-Agent or multiple http
 * headers, then sets properties as headers.
 */
static ngx_int_t
ngx_http_51D_handler(ngx_http_request_t *r)
{

	ngx_http_51D_main_conf_t *fdmcf;
	ngx_http_51D_loc_conf_t *fdlcf;

#ifdef FIFTYONEDEGREES_PATTERN
	fiftyoneDegreesWorkset *ws;
#endif // FIFTYONEDEGREES_PATTERN
#ifdef FIFTYONEDEGREES_TRIE
	fiftyoneDegreesDataSet *dataSet;
	fiftyoneDegreesDeviceOffsets *offsets;
#endif // FIFTYONEDEGREES_TRIE
	ngx_uint_t matchIndex, multi;

	if (r->main->internal) {
		return NGX_DECLINED;
	}
	r->main->internal = 1;

	// Get 51Degrees location config.
	fdlcf = ngx_http_get_module_loc_conf(r, ngx_http_51D_module);
	// Get 51Degrees main config.
	fdmcf = ngx_http_get_module_main_conf(r, ngx_http_51D_module);

	if ((int)fdlcf->headerCount == 0) {
		return NGX_DECLINED;
	}
	ngx_table_elt_t *h[fdlcf->headerCount];

	// Get a workset from the pool.
#ifdef FIFTYONEDEGREES_PATTERN
	ws = fdmcf->ws;
#endif // FIFTYONEDEGREES_PATTERN
#ifdef FIFTYONEDEGREES_TRIE
	dataSet = fdmcf->dataSet;
#endif // FIFTYONEDEGREES_TRIE
	ngx_http_51D_matched_header_t *header;

	ngx_http_51D_cache_node_t *node;
	uint32_t hash;
	ngx_table_elt_t *searchResult;
	int headerIndex;
	hash = ngx_hash(fdlcf->key, ngx_hash_key(r->headers_in.user_agent[0].value.data, r->headers_in.user_agent[0].value.len));
	for (headerIndex = 0; headerIndex < ws->dataSet->httpHeadersCount; headerIndex++) {
		searchResult = search_headers_in(r, (u_char*)ws->dataSet->httpHeaders[headerIndex].headerName, strlen(ws->dataSet->httpHeaders[headerIndex].headerName));
		if (searchResult != NULL) {
			hash = ngx_hash(hash, ngx_hash_key(searchResult->value.data, searchResult->value.len));
		}
	}
	node = ngx_http_51D_lookup_node_lru(&r->headers_in.user_agent[0].value, hash);

	if (node == NULL) {
		node = ngx_http_51D_create_node(&r->headers_in.user_agent[0].value, hash, (int)fdlcf->headerCount);

		for (multi = 0; multi < 2; multi++) {
			for (matchIndex=0;matchIndex<fdlcf->headerCount;matchIndex++)
			{
				if (fdlcf->header[matchIndex]->multi == multi) {
	#ifdef FIFTYONEDEGREES_PATTERN
					if ((int)fdlcf->header[matchIndex]->maxString < 0) {
						ngx_http_51D_set_max_string(fdmcf->dataSet, fdlcf->header[matchIndex]);
					}
					char *valueString = (char*)ngx_palloc(r->pool, fdlcf->header[matchIndex]->maxString);
	#endif // FIFTYONEDEGREES_PATTERN
	#ifdef FIFTYONEDEGREES_TRIE
					char *valueString = (char*)ngx_palloc(r->pool, FIFTYONEDEGREES_MAX_STRING);
	#endif // FIFTYONEDEGREES_TRIE
					valueString[0] = '\0';
					// Get a match.
	#ifdef FIFTYONEDEGREES_PATTERN
					ngx_http_51D_get_match(ws, r, fdlcf->header[matchIndex]->multi);
	#endif // FIFTYONEDEGREES_PATTERN
	#ifdef FIFTYONEDEGREES_TRIE
					offsets = fiftyoneDegreesCreateDeviceOffsets(dataSet);
					ngx_http_51D_get_match(dataSet, r, fdlcf->header[matchIndex]->multi, offsets);
	#endif // FIFTYONEDEGREES_TRIE
					// For each property, set the value in values_string_array.
					ngx_uint_t property_index;
					for (property_index=0; property_index < fdlcf->header[matchIndex]->propertyCount; property_index++) {
	#ifdef FIFTYONEDEGREES_PATTERN
						ngx_http_51D_get_value(ws, valueString, (const char*) fdlcf->header[matchIndex]->property[property_index]->data);
	#endif // FIFTYONEDEGREES_PATTERN
	#ifdef FIFTYONEDEGREES_TRIE
						ngx_http_51D_get_value(dataSet, valueString, (const char*)fdlcf->header[matchIndex]->property[property_index]->data, offsets);
	#endif // FIFTYONEDEGREES_TRIE
					}
					// For each property value pair, set a new header name and value.
					h[matchIndex] = ngx_list_push(&r->headers_in.headers);
					h[matchIndex]->key.data = (u_char*)fdlcf->header[matchIndex]->name.data;
					h[matchIndex]->key.len = ngx_strlen(h[matchIndex]->key.data);
					h[matchIndex]->hash = ngx_hash_key(h[matchIndex]->key.data, h[matchIndex]->key.len);
					h[matchIndex]->value.data = (u_char*)valueString;
					h[matchIndex]->value.len = ngx_strlen(h[matchIndex]->value.data);
					h[matchIndex]->lowcase_key = (u_char*)fdlcf->header[matchIndex]->lowerName.data;
					ngx_http_51D_add_header_to_node(node, h[matchIndex]);

	#ifdef FIFTYONEDEGREES_TRIE
		fiftyoneDegreesFreeDeviceOffsets(offsets);
	#endif // FIFTYONEDEGREES_TRIE
				}
			}
			//todo add to queue
		}
		ngx_http_51D_cache_insert(node);
	}
	else {
		for (matchIndex = 0; matchIndex < fdlcf->headerCount; matchIndex++) {
			header = node->header[(int)matchIndex];
			h[matchIndex] = ngx_list_push(&r->headers_in.headers);
			h[matchIndex]->key = header->name;
			h[matchIndex]->hash = ngx_hash_key(h[matchIndex]->key.data, h[matchIndex]->key.len);
			h[matchIndex]->value = header->value;
			h[matchIndex]->lowcase_key = header->lowerName;
		}
	}
	return NGX_DECLINED;
}

/**
 * Set match function. Initialises the header structure for a given occurrence
 * of "51D_match_single" or "51D_match_all" in the config file. Allocates
 * space required and sets the name and properties.
 */
void
ngx_http_51D_set_header(ngx_conf_t *cf, ngx_http_51D_header_to_set *header, ngx_str_t *value, ngx_http_51D_main_conf_t *fdmcf)
{
	char *tok;
	int propertiesCount, charPos;
	header->propertyCount = 0;
#ifdef FIFTYONEDEGREES_PATTERN
	header->maxString = NGX_CONF_UNSET_SIZE;
#endif // FIFTYONEDEGREES_PATTERN

	// Set the name of the header.
	header->name.data = (u_char*)ngx_palloc(cf->pool, sizeof(value[1]));
	header->lowerName.data = (u_char*)ngx_palloc(cf->pool, sizeof(value[1]));
	header->name.data = value[1].data;
	header->name.len = value[1].len;
	ngx_strlow(header->lowerName.data, header->name.data, header->name.len);
	header->lowerName.len = header->name.len;

    char *propertiesString = (char*)value[2].data;

	propertiesCount = 1;
	for (charPos = 0; charPos < (int)value[2].len; charPos++) {
		if (propertiesString[charPos] == ',') {
			propertiesCount++;
		}
	}
	header->property = (ngx_str_t**)ngx_palloc(cf->pool, sizeof(ngx_str_t*)*propertiesCount);

	tok = strtok((char*)propertiesString, (const char*)",");
	while (tok != NULL) {
		header->property[header->propertyCount] = (ngx_str_t*)ngx_palloc(cf->pool, sizeof(ngx_str_t));
		header->property[header->propertyCount]->data = (u_char*)ngx_palloc(cf->pool, sizeof(u_char)*(ngx_strlen(tok)));
		header->property[header->propertyCount]->data = (u_char*)tok;
		header->property[header->propertyCount]->len = ngx_strlen(header->property[header->propertyCount]->data);
		if (ngx_strstr(fdmcf->properties, tok) == NULL) {
			add_value(tok, fdmcf->properties);
		}
		header->propertyCount++;
		tok = strtok(NULL, ",");
	}
}

/**
 * Set function. Is called for each occurrence of "51D_match_single" or
 * "51D_match_all". Allocates space for the header structure and initialises
 * it with the set header function.
 */
static char *ngx_http_51D_set(ngx_conf_t* cf, ngx_command_t *cmd, void *conf)
{
	ngx_http_51D_main_conf_t *fdmcf;
	ngx_http_51D_loc_conf_t *fdlcf;
	ngx_str_t *value;
	value = cf->args->elts;
	// Get the modules location and main config.
	fdmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_51D_module);
	fdlcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_51D_module);

	fdlcf->header[fdlcf->headerCount] = (ngx_http_51D_header_to_set*)ngx_palloc(cf->pool, sizeof(ngx_http_51D_header_to_set));

	// Enable single User-Agent matching.
	if (ngx_strcmp(cmd->name.data, "51D_match_single") == 0) {
		fdlcf->header[fdlcf->headerCount]->multi = 0;
	}
	else if (ngx_strcmp(cmd->name.data, "51D_match_all") == 0) {
		fdlcf->header[fdlcf->headerCount]->multi = 1;
		fdlcf->hasMulti = 1;
	}

	// Set the properties for the selected location.
	ngx_http_51D_set_header(cf, fdlcf->header[fdlcf->headerCount], value, fdmcf);

	fdlcf->key = ngx_hash(fdlcf->key, (ngx_uint_t)value->data);

	fdlcf->headerCount++;

	return NGX_OK;
}
