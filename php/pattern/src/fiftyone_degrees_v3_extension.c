#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <SAPI.h>
#include "php.h"
#include "php_ini.h"
#include "php_fiftyone_degrees_v3_extension.h"

ZEND_DECLARE_MODULE_GLOBALS(fiftyone_degrees)

// list of custom PHP functions provided by this extension
// set {NULL, NULL, NULL} as the last record to mark the end of list
static zend_function_entry fiftyone_degrees_detector_functions[] = {
  PHP_FE(fiftyone_degrees_test_function, NULL)
  PHP_FE(fiftyone_degrees_get_properties, NULL)
  PHP_FE(fiftyone_match, NULL)
  PHP_FE(fiftyone_info, NULL)
  PHP_FE(fiftyone_http_headers, NULL)
  PHP_FE(fiftyone_match_with_headers, NULL)
  {NULL, NULL, NULL}
};

// the following code creates an entry for the module and registers it with Zend.
zend_module_entry fiftyone_degrees_detector_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  PHP_EXTENSION_EXTNAME,
  fiftyone_degrees_detector_functions,
  PHP_MINIT(fiftyone_degrees_detector_init),
  PHP_MSHUTDOWN(fiftyone_degrees_detector_shutdown),
  PHP_RINIT(fiftyone_degrees_request_init), // name of the RINIT function or NULL if not applicable
  PHP_RSHUTDOWN(fiftyone_degrees_request_shutdown), // name of the RSHUTDOWN function or NULL if not applicable
  NULL, // name of the MINFO function or NULL if not applicable
#if ZEND_MODULE_API_NO >= 20010901
  PHP_EXTENSION_VERSION,
#endif
  STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(fiftyone_degrees_detector)

/* Resources that will be used for detection. */

fiftyoneDegreesDataSet* dataSet;
fiftyoneDegreesDataSetInitStatus initStatus;
fiftyoneDegreesResultsetCache* cache;
fiftyoneDegreesWorksetPool* pool;

/* Implementation of functions exposed to PHP */

/**
 * To test the detecor is working.
 */
PHP_FUNCTION(fiftyone_degrees_test_function)
{
  RETURN_STRING("HELLO WORLD", 1);
}

/**
 * Function replaces the fiftyone_degrees_get_properties function
 * as the name is shorter and moe logical. Function takes in a
 * user agent string from $_SERVER['HTTP_USER_AGENT'] and returns
 * an array with device properties.
 * @return a PHP array of property:value pairs.
 */
PHP_FUNCTION(fiftyone_match) {
    //Stop if there was a problem initialising the dataset.
    if (datasetInitSuccess() == 0)
    {
        php_printf("Dataset initialisation failed.");
        return;
    }

    char* useragent;
    int useragent_len;

    //Check that user agent was passed to this function.
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &useragent, &useragent_len) == SUCCESS) {
        //Initialise array for returned values.
        array_init(return_value);
        //Match the provided user agent.
        matchUserAgent(useragent, useragent_len, return_value);
    }
    else
    {
        php_error(E_WARNING, "Could not parse arguments.");
    }
}

/**
 * Function performs device matching based on the multiple HTTP user agents.
 * @param headers: a string of HTTP header:value pairs where each line is
 * separated by the new line character. The header name is separated from the
 * header value either by space or by colon.
 * @return a PHP array of property:value entries.
 */
PHP_FUNCTION(fiftyone_match_with_headers)
{
    //Stop if there was a problem initialising the dataset.
    if (datasetInitSuccess() == 0)
    {
        return;
    }

    char *headers;
    int headers_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &headers, &headers_len) == SUCCESS) {
        //Initialise array for returned values.
        array_init(return_value);
        //Match the provided user agent.
        matchHttpHeaders(headers, headers_len, return_value);
    }
    else
    {
        php_error(E_WARNING, "Could not parse arguments.");
    }
}

/**
 * Experimental function to get access to the HTTP headers directly from this
 * module. Do not use.
 */
PHP_FUNCTION(fiftyone_http_headers)
{

    int entries = zend_llist_count(&SG(sapi_headers).headers);
    php_printf("Zend header Entries: %d<br/>", entries);

    zend_llist_position pos;
    sapi_header_struct* h;

    for (h = (sapi_header_struct*)zend_llist_get_first_ex(&SG(sapi_headers).headers, &pos);
         h;
         h = (sapi_header_struct*)zend_llist_get_next_ex(&SG(sapi_headers).headers, &pos))
    {
        php_printf("SAPI %.*s <br/>", h->header_len, h->header);
    }

    // This code makes sure $_SERVER has been initialized
    if (!zend_hash_exists(&EG(symbol_table), "_SERVER", 8)) {
        zend_auto_global* auto_global;
        if (zend_hash_find(CG(auto_globals), "_SERVER", 8, (void **)&auto_global) != FAILURE) {
            auto_global->armed = auto_global->auto_global_callback(auto_global->name, auto_global->name_len TSRMLS_CC);
        }
    }

    // This fetches $_SERVER['PHP_SELF']
    zval** arr;
    char* script_name;
    if (zend_hash_find(&EG(symbol_table), "_SERVER", 8, (void**)&arr) != FAILURE) {
        HashTable* ht = Z_ARRVAL_P(*arr);
        zval** val;
        if (zend_hash_find(ht, "http_user_agent", 16, (void**)&val) != FAILURE) {
            script_name = Z_STRVAL_PP(val);
        }
    }
    php_printf("%s <br>", script_name);
}

/**
 * Function returns an array of information relating to the data file such as
 * when the data file was published, when the next data file of the same type
 * will be released.
 *
 * Function takes into account the initialisation status of the dataSet and
 * prints the relevant message based on the initialisation status. If the
 * initialisation of the dataSet fails, then only the initialisation status
 * code and a short description of the problem will be returned.
 *
 * @return array with data file information.
 */
PHP_FUNCTION(fiftyone_info)
{
    //Return the init code even if the dataset was not initialised.
    array_init(return_value);
    add_assoc_long(return_value, "dataSetInitStatus", (long)initStatus);

    //Add a messge to briefly describe what the issue with initialisation is, if any.
    switch(initStatus)
    {
        case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
            add_assoc_string(return_value, "dataSetInitMessage", "FAILED: Insufficien memory", 1);
            break;
        case DATA_SET_INIT_STATUS_CORRUPT_DATA:
            add_assoc_string(return_value, "dataSetInitMessage", "FAILED: Data file corrupt", 1);
            break;
        case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
            add_assoc_string(return_value, "dataSetInitMessage", "FAILED: Data file is of incorrect version", 1);
            break;
        case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
            add_assoc_string(return_value, "dataSetInitMessage", "FAILED: Data file not found", 1);
            break;
        case DATA_SET_INIT_STATUS_SUCCESS:
            add_assoc_string(return_value, "dataSetInitMessage", "SUCCESS", 1);
            break;
        default:
            add_assoc_string(return_value, "dataSetInitMessage", "Unknown init status", 1);
            break;
    }

    //Stop if there was a problem initialising the dataset.
    if (datasetInitSuccess() == 0)
    {
        php_printf("Dataset initialisation failed.");
        return;
    }

    //Assuming initialisation successfull, add data file info.
    //Published.
    add_assoc_long(return_value, "publishedDay", (long)dataSet->header.published.day);
    add_assoc_long(return_value, "publishedMonth", (long)dataSet->header.published.month);
    add_assoc_long(return_value, "publishedYear", (long)dataSet->header.published.year);
    //Next update.
    add_assoc_long(return_value, "nextUpdateDay", (long)dataSet->header.nextUpdate.day);
    add_assoc_long(return_value, "nextUpdateMonth", (long)dataSet->header.nextUpdate.month);
    add_assoc_long(return_value, "nextUpdateYear", (long)dataSet->header.nextUpdate.year);
    //Data file version.
    add_assoc_long(return_value, "dataSetVersionMajor", (long)dataSet->header.versionMajor);
    add_assoc_long(return_value, "dataSetVersionMinor", (long)dataSet->header.versionMinor);
    add_assoc_long(return_value, "dataSetVersionBuild", (long)dataSet->header.versionBuild);
    add_assoc_long(return_value, "dataSetRevision", (long)dataSet->header.versionRevision);
}

/**
 * LEGACY Function creates a new workset for each request and does not care
 * for the existence of pool or cache. Deprecated and should not be used.
 * @deprecated instead use fiftyone_match or
 *             fiftyone_match_with_headers.
 */
PHP_FUNCTION(fiftyone_degrees_get_properties)
{
    //Stop if there was a problem initialising the dataset.
    if (datasetInitSuccess() == 0)
    {
        php_printf("Dataset initialisation failed.");
        return;
    }

    char* useragent;
    int useragent_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &useragent, &useragent_len) == SUCCESS) {
        array_init(return_value);
        matchUserAgent(useragent, useragent_len, return_value);
    } else {
        php_error(E_WARNING, "Could not parse arguments.");
    }
}

/* Support functions */

void matchHttpHeaders(char *httpHeaders, int httpHeadersLength, zval *returnArray) {
    // Match the provided user agent.
    fiftyoneDegreesSetHttpHeaders(FIFTYONE_G(ws), httpHeaders, httpHeadersLength);
    fiftyoneDegreesMatchForHttpHeaders(FIFTYONE_G(ws));
    // Build the PHP array with the result.
    buildArray(returnArray);
}

void matchUserAgent(char *userAgent, int userAgentLength, zval *returnArray) {
    // Match the provided user agent.
    fiftyoneDegreesMatch(FIFTYONE_G(ws), userAgent);
    // Build the PHP array with the result.
    buildArray(returnArray);
}

void buildArray(zval *returnArray)
{
    int propertyIndex;
    for(propertyIndex = 0; propertyIndex < FIFTYONE_G(ws)->dataSet->requiredPropertyCount; propertyIndex++) {
        char* propertyName = (char*)fiftyoneDegreesGetPropertyName(FIFTYONE_G(ws)->dataSet, *(FIFTYONE_G(ws)->dataSet->requiredProperties + propertyIndex));
        int valueCount = fiftyoneDegreesSetValues(FIFTYONE_G(ws), propertyIndex);
        if (valueCount == 1) {
            char *valueString = (char*)fiftyoneDegreesGetValueName(FIFTYONE_G(ws)->dataSet, *FIFTYONE_G(ws)->values);
            add_assoc_string(returnArray, propertyName, valueString, 1);
        } else if (valueCount > 1) {
            zval* valueArray;
            ALLOC_INIT_ZVAL(valueArray);
            array_init(valueArray);
            add_assoc_zval(returnArray, propertyName, valueArray);
            int valueIndex;
            for(valueIndex = 0; valueIndex < valueCount; valueIndex++) {
                char *valueString = (char*)fiftyoneDegreesGetValueName(FIFTYONE_G(ws)->dataSet, *(FIFTYONE_G(ws)->values + valueIndex));
                add_next_index_string(valueArray, valueString, 1);
            }
        }
    }

    //Add signature rank information.
    int32_t sigRank = fiftyoneDegreesGetSignatureRank(FIFTYONE_G(ws));
    // SignatureRank is obsolete and will be removed in future versions. Use Rank instead.
    add_assoc_long(returnArray, "SignatureRank", sigRank);
    add_assoc_long(returnArray, "Rank", sigRank);
    add_assoc_long(returnArray, "Difference", FIFTYONE_G(ws)->difference);

    //Add method that was used for detection to the results array.
    char* methodString;
    switch(FIFTYONE_G(ws)->method){
        case NONE: methodString = "None"; break;
        case EXACT: methodString = "Exact"; break;
        case NUMERIC: methodString = "Numeric"; break;
        case NEAREST: methodString = "Nearest"; break;
        case CLOSEST: methodString = "Closest"; break;
        default: methodString = "Unknown"; break;
    }
    add_assoc_string(returnArray, "Method", methodString, 1);
}

/**
 * Function examines the DataSet init status and returns 0 if there was a
 * problem or 1 otherwise.
 * @return 1 if dataset successfully initialised, 0 otherwise.
 */
int datasetInitSuccess()
{
    switch (initStatus) {
    case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
          php_error(E_WARNING, "51Degrees data set could not be created as not enough memory could be allocated.");
          return 0;
    case DATA_SET_INIT_STATUS_CORRUPT_DATA:
          php_error(E_WARNING, "51Degrees data set could not be created as a corrupt data file has been provided. Check it is uncompressed.");
          return 0;
    case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
          php_error(E_WARNING, "51Degrees data set could not be created as the data file is an unsupported version. Check you have the latest version of the data and api.");
          return 0;
    case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
          php_error(E_WARNING, "51Degrees data set could not be created as a data file could not be found.");
          return 0;
    case DATA_SET_INIT_STATUS_SUCCESS: // do nothing
        return 1;
    }
}

/* PHP Zend init and shutdown functions */

/**
 * This section is responsible for loading settigns from the php.ini file.
 */
PHP_INI_BEGIN()
PHP_INI_ENTRY("fiftyone_degrees.data_file", "/usr/lib/php5/51Degrees-Lite.dat", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("fiftyone_degrees.number_worksets", "10", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("fiftyone_degrees.cache_size", "100", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("fiftyone_degrees.property_list", NULL, PHP_INI_ALL, NULL)
PHP_INI_END()

/**
 * Module startup function. All fo the resources with long lifespan get
 * initialised here. First memory allocation takes place and a new dataset
 * structure gets created. Then cach and pool get created. Cache is used
 * to speed up detections by storing the most frequent requests in memory.
 * The pool holds several Workset structures which are used for device
 * detection.
 */
PHP_MINIT_FUNCTION(fiftyone_degrees_detector_init)
{
    REGISTER_INI_ENTRIES();
    char* dataFilePath = INI_STR("fiftyone_degrees.data_file");
    int numberWorksets = INI_INT("fiftyone_degrees.number_worksets");
    int cacheSize = INI_INT("fiftyone_degrees.cache_size");
    dataSet = (fiftyoneDegreesDataSet*)malloc(sizeof(fiftyoneDegreesDataSet));
    char* propertyList = INI_STR("fiftyone_degrees.property_list");
    initStatus = fiftyoneDegreesInitWithPropertyString((char*)dataFilePath, dataSet, propertyList);
    cache = fiftyoneDegreesResultsetCacheCreate(dataSet, cacheSize);
    pool = fiftyoneDegreesWorksetPoolCreate(dataSet, cache, numberWorksets);
    return SUCCESS;
}

/**
 * Module shutdown function. Used to free all of the resources with
 * long lifespan.
 */
PHP_MSHUTDOWN_FUNCTION(fiftyone_degrees_detector_shutdown)
{
  UNREGISTER_INI_ENTRIES();
  if (initStatus == SUCCESS) {
    fiftyoneDegreesWorksetPoolFree(pool);
    fiftyoneDegreesResultsetCacheFree(cache);
    fiftyoneDegreesDataSetFree(dataSet);
  }
  return SUCCESS;
}

/**
 * Request startup function. Called upon each request.
 */
PHP_RINIT_FUNCTION(fiftyone_degrees_request_init) {
    FIFTYONE_G(ws) = fiftyoneDegreesWorksetPoolGet(pool);
    return SUCCESS;
}

/**
 * Request shutdown function. Called at the end of each request.
 */
PHP_RSHUTDOWN_FUNCTION(fiftyone_degrees_request_shutdown) {
    fiftyoneDegreesWorksetPoolRelease(pool, FIFTYONE_G(ws));
    return SUCCESS;
}
