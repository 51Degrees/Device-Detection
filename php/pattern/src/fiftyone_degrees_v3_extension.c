/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright © 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 *
 * This Source Code Form is the subject of the following patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent Application No. 13192291.6; and
 * United States Patent Application Nos. 14/085,223 and 14/085,301.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is “Incompatible With Secondary Licenses”, as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */
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
  PHP_FE(fiftyone_info, NULL)
  PHP_FE(fiftyone_match, NULL)
  PHP_FE(fiftyone_match_with_useragent, NULL)
  PHP_FE(fiftyone_match_with_headers, NULL)
  PHP_FE(fiftyone_degrees_get_properties, NULL)
  PHP_FE(fiftyone_get_http_headers, NULL)
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
  PHP_RINIT(fiftyone_degrees_request_init),
  PHP_RSHUTDOWN(fiftyone_degrees_request_shutdown),
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

/* Private methods */

/**
 * Adds a new element to the array with the key Id and the value as the device
 * Id.
 * @param returnArray the array being built
 */
void addDeviceIdToArray(zval *returnArray) {
    int componentIndex;
    int bufferLength = FIFTYONE_G(ws)->dataSet->header.components.count * 10;
    char *buffer = (char *)malloc(bufferLength * sizeof(char));
    if (buffer != NULL) {
        fiftyoneDegreesGetDeviceId(FIFTYONE_G(ws), buffer, bufferLength);
        add_assoc_string(returnArray, "Id", buffer, 1);
        free(buffer);
    }
}

/**
 * Internal function that goes through every property specified in the PHP.ini
 * file and for each of the properties checks that such property exists in the
 * data set properties array. If a property exists, then a corresponding value
 * is added to the array that will be returned to PHP. If a property does not
 * exist, then a "Switch Data Set" message is returned as the property value
 * @param returnArray a pointer to the Zend array to store results in.
 */
void buildArray(zval *returnArray)
{
    char* propertyName;
    char *valueString;
    char* methodString;
    int propertyIndex;
    int valueCount;
    int valueIndex;
    int32_t sigRank;

    for(propertyIndex = 0;
        propertyIndex < FIFTYONE_G(ws)->dataSet->requiredPropertyCount;
        propertyIndex++) {
        propertyName = (char*)fiftyoneDegreesGetPropertyName(
                FIFTYONE_G(ws)->dataSet,
                *(FIFTYONE_G(ws)->dataSet->requiredProperties + propertyIndex));
        valueCount = fiftyoneDegreesSetValues(FIFTYONE_G(ws), propertyIndex);

        if (valueCount == 1) {
            valueString = (char*)fiftyoneDegreesGetValueName(
                FIFTYONE_G(ws)->dataSet,
                *FIFTYONE_G(ws)->values);
            add_assoc_string(returnArray, propertyName, valueString, 1);
        } else if (valueCount > 1) {
            zval* valueArray;
            ALLOC_INIT_ZVAL(valueArray);
            array_init(valueArray);
            add_assoc_zval(returnArray, propertyName, valueArray);
            for(valueIndex = 0; valueIndex < valueCount; valueIndex++) {
                valueString = (char*)fiftyoneDegreesGetValueName(
                    FIFTYONE_G(ws)->dataSet,
                    *(FIFTYONE_G(ws)->values + valueIndex));
                add_next_index_string(valueArray, valueString, 1);
            }
        }
    }
    // Add signature rank information.
    sigRank = fiftyoneDegreesGetSignatureRank(FIFTYONE_G(ws));
    // SignatureRank is obsolete and will be removed in future versions.
    // Use Rank instead.
    add_assoc_long(returnArray, "SignatureRank", sigRank);
    add_assoc_long(returnArray, "Rank", sigRank);
    add_assoc_long(returnArray, "Difference", FIFTYONE_G(ws)->difference);

    // Add method that was used for detection to the results array.
    switch(FIFTYONE_G(ws)->method){
        case NONE: methodString = "None"; break;
        case EXACT: methodString = "Exact"; break;
        case NUMERIC: methodString = "Numeric"; break;
        case NEAREST: methodString = "Nearest"; break;
        case CLOSEST: methodString = "Closest"; break;
        default: methodString = "Unknown"; break;
    }
    add_assoc_string(returnArray, "Method", methodString, 1);
    addDeviceIdToArray(returnArray);
}

/**
 * Internal function that takes in a string of HTTP headers and values as
 * the first parameter, invokes another function to convert the string into
 * device offsets and finally passes control to another function that builds
 * an array of Property -> Value pairs.
 * @param httpHeaders a string of characters where HTTP header name must be
 * separated from the HTTP header value either by a colon or by space. In
 * addition the HTTP header and value pairs must be separated by the new line
 * character 'n'.
 * @param httpHeadersLength length of the httpHeaders string.
 * @param returnArray pointer to the Zend array for storing results.
 */
void matchHttpHeaders(char *httpHeaders, int httpHeadersLength, zval *returnArray) {
    // Match the provided user agent.
    fiftyoneDegreesSetHttpHeaders(FIFTYONE_G(ws), httpHeaders, httpHeadersLength);
    fiftyoneDegreesMatchForHttpHeaders(FIFTYONE_G(ws));
    // Build the PHP array with the result.
    buildArray(returnArray);
}

/**
 * Internal function that performs match with a single User-Agent HTTP header
 * string and invokes the buildArray function to process the results.
 * @param userAgent is the User-Agent string to be matched.
 * @param userAgentLength is the length of the User-Agent string.
 * @param returnArray a pointer to the Zend array to store results in.
 */
void matchUserAgent(char *userAgent, int userAgentLength, zval *returnArray) {
    // Match the provided user agent.
    fiftyoneDegreesMatch(FIFTYONE_G(ws), userAgent);
    // Build the PHP array with the result.
    buildArray(returnArray);
}

/**
 * Internal function that performs match for all of the relevant headers. All
 * relevant HTTP headers must be set in the Work Set prior to calling this
 * function.
 * @param returnArray a pointer to the Zend array to store results in.
 */
void matchHttpHeadersAuto(zval *returnArray) {
    fiftyoneDegreesMatchForHttpHeaders(FIFTYONE_G(ws));
    buildArray(returnArray);
}

/**
 * Internal function examines the DataSet init status and returns 0 if there
 * was a problem or 1 otherwise.
 * @return 1 if dataset successfully initialised, 0 otherwise.
 */
int datasetInitSuccess()
{
    switch (initStatus) {
    case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
        php_error(E_WARNING, "51Degrees data set could not be created as not "
            "enough memory could be allocated.");
        return 0;
    case DATA_SET_INIT_STATUS_CORRUPT_DATA:
        php_error(E_WARNING, "51Degrees data set could not be created as a "
            "corrupt data file has been provided. Check it is uncompressed.");
          return 0;
    case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
          php_error(E_WARNING, "51Degrees data set could not be created as the "
          "data file is an unsupported version. Check you have the latest "
          "version of the data and api.");
          return 0;
    case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
          php_error(E_WARNING, "51Degrees data set could not be created as a "
          "data file could not be found.");
          return 0;
    case DATA_SET_INIT_STATUS_SUCCESS: // do nothing
        return 1;
    }
}

/* Implementation of functions exposed to PHP */

/**
 * Function returns an array of information relating to the data file such as
 * when the data file was published, when the next data file of the same type
 * will be released.
 * Function takes into account the initialisation status of the dataSet and
 * prints the relevant message based on the initialisation status. If the
 * initialisation of the dataSet fails, then only the initialisation status
 * code and a short description of the problem will be returned.
 * @returns array with data file information.
 */
PHP_FUNCTION(fiftyone_info)
{
    // Return the init code even if the dataset was not initialised.
    array_init(return_value);
    add_assoc_long(return_value, "dataSetInitStatus", (long)initStatus);
    // Add a message to briefly describe what the issue with initialisation is, if any.
    switch(initStatus)
    {
        case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
            add_assoc_string(return_value,
                            "dataSetInitMessage",
                            "FAILED: Insufficien memory", 1);
            break;
        case DATA_SET_INIT_STATUS_CORRUPT_DATA:
            add_assoc_string(return_value,
                            "dataSetInitMessage",
                            "FAILED: Data file corrupt", 1);
            break;
        case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
            add_assoc_string(return_value,
                            "dataSetInitMessage",
                            "FAILED: Data file is of incorrect version", 1);
            break;
        case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
            add_assoc_string(return_value,
                            "dataSetInitMessage",
                            "FAILED: Data file not found", 1);
            break;
        case DATA_SET_INIT_STATUS_SUCCESS:
            add_assoc_string(return_value,
                            "dataSetInitMessage",
                            "SUCCESS", 1);
            break;
        default:
            add_assoc_string(return_value,
                            "dataSetInitMessage",
                            "Unknown init status", 1);
            break;
    }
    // Stop if there was a problem initialising the dataset.
    if (datasetInitSuccess() == 0)
    {
        php_printf("Dataset initialisation failed.");
        return;
    }
    // Assuming initialisation successful, add data file info.
    // Published.
    add_assoc_long(return_value,
                    "publishedDay",
                    (long)dataSet->header.published.day);
    add_assoc_long(return_value,
                    "publishedMonth",
                    (long)dataSet->header.published.month);
    add_assoc_long(return_value,
                    "publishedYear",
                    (long)dataSet->header.published.year);
    // Next update.
    add_assoc_long(return_value,
                    "nextUpdateDay",
                    (long)dataSet->header.nextUpdate.day);
    add_assoc_long(return_value,
                    "nextUpdateMonth",
                    (long)dataSet->header.nextUpdate.month);
    add_assoc_long(return_value,
                    "nextUpdateYear",
                    (long)dataSet->header.nextUpdate.year);
    // Data file version.
    add_assoc_long(return_value,
                    "dataSetVersionMajor",
                    (long)dataSet->header.versionMajor);
    add_assoc_long(return_value,
                    "dataSetVersionMinor",
                    (long)dataSet->header.versionMinor);
    add_assoc_long(return_value,
                    "dataSetVersionBuild",
                    (long)dataSet->header.versionBuild);
    add_assoc_long(return_value,
                    "dataSetRevision",
                    (long)dataSet->header.versionRevision);
    // Dataset name.
    char *datasetName = (char*)(&fiftyoneDegreesGetString(
                    dataSet, dataSet->header.nameOffset)->firstByte);
    add_assoc_string(return_value, "datasetName", datasetName, 1);
    // Add device combinations
    add_assoc_long(return_value,
                    "dataSetDeviceCombinations",
                    (long)dataSet->header.deviceCombinations);
}

/**
 * Function performs device detection by retrieving the relevant HTTP headers
 * from the _SERVER array, if the said array is present. All of the important
 * HTTP headers are used for device detection. No user input is required.
 * Function is exposed to PHP.
 * @returns a PHP array of Proper
 */
PHP_FUNCTION(fiftyone_match)
{
    const char server[] = "_SERVER";
    const char* httpHeader;
    char *httpHeaderValue;
    int httpHeaderIndex;
    int httpHeaderLength;

    // Stop if there was a problem initialising the dataset.
    if (datasetInitSuccess() == 0)
    {
        php_error(E_WARNING, "51Degrees dataset initialisation failed.");
        return;
    }
    // Check if the SERVER global array is present.
    if (!zend_hash_exists(&EG(symbol_table), server, sizeof(server))) {
        zend_auto_global* auto_global;
        if (zend_hash_find(CG(auto_globals),
            server, sizeof(server), (void **)&auto_global) != FAILURE) {
            auto_global->armed = auto_global->auto_global_callback(
                    auto_global->name,
                    auto_global->name_len TSRMLS_CC);
        }
    }
    httpHeaderLength = 0;
    httpHeaderIndex = 0;
    httpHeader = NULL;
    FIFTYONE_G(ws)->importantHeadersCount = 0;
    do {
        httpHeader = fiftyoneDegreesGetPrefixedUpperHttpHeaderName(
                                                           dataSet,
                                                           httpHeaderIndex);
        if (httpHeader != NULL) {

            httpHeaderLength = strlen(httpHeader) + 1;
            // Find the header.
            zval** arr;
            if (zend_hash_find(&EG(symbol_table), server, sizeof(server),
                                                (void**)&arr) != FAILURE) {
                HashTable* ht = Z_ARRVAL_P(*arr);
                zval** val;
                if (zend_hash_find(ht, httpHeader, httpHeaderLength,
                                                (void**)&val) != FAILURE) {
                    httpHeaderValue = Z_STRVAL_PP(val);
                    FIFTYONE_G(ws)->importantHeaders[FIFTYONE_G(ws)->
                            importantHeadersCount].header = FIFTYONE_G(ws)->
                                        dataSet->httpHeaders + httpHeaderIndex;
                    FIFTYONE_G(ws)->importantHeaders[FIFTYONE_G(ws)->
                            importantHeadersCount].headerValue = httpHeaderValue;
                    FIFTYONE_G(ws)->importantHeaders[FIFTYONE_G(ws)->
                            importantHeadersCount].headerValueLength =
                                                        strlen(httpHeaderValue);
                    FIFTYONE_G(ws)->importantHeadersCount++;
                }
            }
        }
        httpHeaderIndex++;
    } while(httpHeader != NULL);

    array_init(return_value);
    matchHttpHeadersAuto(return_value);
}

/**
 * Function performs device detection based on the provided User-Agent HTTP
 * header string, usually accessed via: $_SERVER['HTTP_USER_AGENT'] in PHP.
 * Function is exposed to PHP.
 * @param useragent a string containing the HTTP User-Agent to identify.
 * @returns a PHP array of Property -> Value pairs.
 */
PHP_FUNCTION(fiftyone_match_with_useragent) {
    char* useragent;
    int useragent_len;
    // Stop if there was a problem initialising the dataset.
    if (datasetInitSuccess() == 0)
    {
        php_printf("Dataset initialisation failed.");
        return;
    }
    // Check that user agent was passed to this function.
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                        "s", &useragent, &useragent_len) == SUCCESS) {
        array_init(return_value);
        matchUserAgent(useragent, useragent_len, return_value);
    }
    else
    {
        php_error(E_WARNING, "Could not parse arguments.");
    }
}

/**
 * Function performs device detection based on a set of HTTP headers supplied
 * as input. The input must be in the form of a string where the HTTP header
 * name is separated from the header value by either a space or a colon symbol.
 * Each Header -> Value pair must end with a new line character '\n'.
 * Function is exposed to PHP.
 * @param headers: a string of HTTP header:value pairs where each line is
 * separated by the new line character. The header name is separated from the
 * header value either by space or by colon.
 * @returns a PHP array of Property -> Value pairs.
 */
PHP_FUNCTION(fiftyone_match_with_headers)
{
    char *headers;
    int headers_len;
    // Stop if there was a problem initialising the dataset.
    if (datasetInitSuccess() == 0)
    {
        return;
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                        "s", &headers, &headers_len) == SUCCESS) {
        array_init(return_value);
        matchHttpHeaders(headers, headers_len, return_value);
    }
    else
    {
        php_error(E_WARNING, "Could not parse arguments.");
    }
}

/**
 * LEGACY Function creates a new workset for each request and does not care
 * for the existence of pool or cache. Deprecated and should not be used.
 * @deprecated instead use fiftyone_match or
 *             fiftyone_match_with_headers.
 */
PHP_FUNCTION(fiftyone_degrees_get_properties)
{
    char* useragent;
    int useragent_len;
    // Stop if there was a problem initialising the dataset.
    if (datasetInitSuccess() == 0)
    {
        php_printf("Dataset initialisation failed.");
        return;
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                        "s", &useragent, &useragent_len) == SUCCESS) {
        array_init(return_value);
        matchUserAgent(useragent, useragent_len, return_value);
    } else {
        php_error(E_WARNING, "Could not parse arguments.");
    }
}

/**
 * Function goes through every HTTP header set in the _SERVER array and
 * retrieves all important headers that are set for the current reqest.
 * A PHP array is then returned with the list of important HTTP headers and
 * header as keys and HTTP header values as values. If an HTTP header was not
 * set for the current request the value for that header is set to
 * 'Header not set'.
 * @returns A PHP array with important HTTP headers and the corresponding values
 * for each header for the current request.
 */
PHP_FUNCTION(fiftyone_get_http_headers) {
    const char* ucHeader;
    int index;
    //Stop if there was a problem initialising the dataset.
    if (datasetInitSuccess() == 0)
    {
        php_error(E_WARNING, "51Degrees dataset initialisation failed.");
        return;
    }

    array_init(return_value);
    index = 0;
    ucHeader = NULL;
    ucHeader = fiftyoneDegreesGetPrefixedUpperHttpHeaderName(dataSet, index);
    do {
        if (ucHeader != NULL) {
            add_next_index_string(return_value, ucHeader, 1);
        }
        ucHeader = fiftyoneDegreesGetPrefixedUpperHttpHeaderName(dataSet, index);
        index++;
    } while(ucHeader != NULL);
}

/* PHP Zend init and shutdown functions */

/**
 * This section is responsible for loading settigns from the php.ini file.
 */
PHP_INI_BEGIN()
PHP_INI_ENTRY("fiftyone_degrees.data_file", "/usr/lib/php5/51Degrees.dat", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("fiftyone_degrees.number_worksets", "10", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("fiftyone_degrees.cache_size", "10000", PHP_INI_ALL, NULL)
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
