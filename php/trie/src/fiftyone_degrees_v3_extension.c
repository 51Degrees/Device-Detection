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
fiftyoneDegreesDataSetInitStatus initStatus;

/* Internal functions */

/**
 * Internal function that goes through every device offset that was set by one
 * of the other functions and for each offset it retrieves the values for
 * every property that the detector was initialised with. The results are put
 * into the array that gets retured to PHP upon completion.
 * @param returnArray pointer to the Zend array for storing results.
 */
void buildArray(zval *returnArray) {
    const char* propertyName;
    char* valueString;
    int requiredPropertyIndex;
    // If no properties return nothing.
	if (fiftyoneDegreesGetRequiredPropertiesCount() == 0) {
		return;
	}
	for(requiredPropertyIndex = 0;
        requiredPropertyIndex <= fiftyoneDegreesGetRequiredPropertiesCount();
        requiredPropertyIndex++) {

        propertyName = *(fiftyoneDegreesGetRequiredPropertiesNames() +
                requiredPropertyIndex);
        valueString = (char*)fiftyoneDegreesGetValuePtrFromOffsets(
                FIFTYONE_G(deviceOffsets),
                requiredPropertyIndex);

        if (valueString != NULL) {
            add_assoc_string(returnArray, propertyName, valueString, 1);
        } else {
            add_assoc_null(returnArray, propertyName);
        }
	}
}

/**
 * Function sets the size of the device offsets to 0. This will cause any
 * future interactions with the deviceOffsets to write over the existing
 * data, hence re-using the structure.
 */
void clearOffsets() {
    if(FIFTYONE_G(deviceOffsets)->firstOffset != NULL
        && FIFTYONE_G(deviceOffsets)->size > 0) {
        FIFTYONE_G(deviceOffsets)->size = 0;
    }
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
void matchHttpHeaders(char *headers, int headersLength, zval *returnArray) {
    clearOffsets();
    fiftyoneDegreesSetDeviceOffsetsWithHeadersString(
            FIFTYONE_G(deviceOffsets),
			headers,
			headersLength);
    buildArray(returnArray);
}

/**
 * Internal function that takes in a single HTTP User-Agent header and adds it
 * to the device offsets and passes control to another function that builds an
 * array of Property -> Value pairs.
 * @param userAgent is the HTTP User-Agent header to match.
 * @param returnArray pointer to the Zend array for storing results.
 */
void matchUserAgent(char *userAgent, zval *returnArray) {
    int userAgentIndex;
    clearOffsets();
    userAgentIndex = fiftyoneDegreesGetUniqueHttpHeaderIndex(
            "User-Agent",
            strlen("User-Agent"));
    fiftyoneDegreesSetDeviceOffset(userAgent,
            userAgentIndex,
            FIFTYONE_G(deviceOffsets)->firstOffset);
    FIFTYONE_G(deviceOffsets)->size = 1;
    buildArray(returnArray);
}

/**
 * Internal function that examines the DataSet init status and returns 0 if
 * there was a problem or 1 otherwise.
 * @return 1 if dataset successfully initialised, 0 otherwise.
 */
int datasetInitSuccess()
{
    switch (initStatus) {
    case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
          php_error(E_WARNING, "51Degrees data set could not be created as not"
          " enough memory could be allocated.");
          return 0;
    case DATA_SET_INIT_STATUS_CORRUPT_DATA:
          php_error(E_WARNING, "51Degrees data set could not be created as a "
          "corrupt data file has been provided. Check it is uncompressed.");
          return 0;
    case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
          php_error(E_WARNING, "51Degrees data set could not be created as the"
          " data file is an unsupported version. Check you have the latest "
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

/* 51Degrees functions exposed to PHP. */

/**
 * Function returns information related to the Trie Data Set that the detector
 * has been initialised with. Trie data file does not provide information like
 * the Published Date and the Next Update Date, so 'N/A' will be returned for
 * these properties.
 * @returns a PHP array with Data Set information.
 */
PHP_FUNCTION(fiftyone_info)
{
    // Return the init code even if the dataset was not initialised.
    array_init(return_value);
    add_assoc_long(return_value, "dataSetInitStatus", (long)initStatus);
    // Add messge to describe what the issue with initialisation is, if any.
    switch(initStatus)
    {
        case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
            add_assoc_string(return_value,
                            "dataSetInitMessage",
                            "FAILED: Insufficient memory", 1);
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
    //Published.
    add_assoc_string(return_value, "publishedDay", "N/A", 1);
    add_assoc_string(return_value, "publishedMonth", "N/A", 1);
    add_assoc_string(return_value, "publishedYear", "N/A", 1);
    //Next update.
    add_assoc_string(return_value, "nextUpdateDay", "N/A", 1);
    add_assoc_string(return_value, "nextUpdateMonth", "N/A", 1);
    add_assoc_string(return_value, "nextUpdateYear", "N/A", 1);
    //Data file version.
    add_assoc_string(return_value, "dataSetVersionMajor", "N/A", 1);
    add_assoc_string(return_value, "dataSetVersionMinor", "N/A", 1);
    add_assoc_string(return_value, "dataSetVersionBuild", "N/A", 1);
    add_assoc_string(return_value, "dataSetRevision", "N/A", 1);
    //Dataset name.
    add_assoc_string(return_value, "datasetName", "Trie", 1);
    //Add device combinations
    add_assoc_string(return_value, "dataSetDeviceCombinations", "N/A", 1);
}

/**
 * Function performs device detection by retrieving the relevant HTTP headers
 * from the _SERVER array, if the said array is present. All of the important
 * HTTP headers are used for device detection. No user input is required.
 * Function is exposed to PHP.
 * @returns a PHP array of Property -> Value pairs.
 */
PHP_FUNCTION(fiftyone_match)
{
    const char server[] = "_SERVER";
    const char* header;
    char* propertyValue;
    int header_len;
    int i;
    zval** arr;
    zval** val;
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
    // Reset the offsets.
    clearOffsets();
    if (FIFTYONE_G(deviceOffsets)->firstOffset != NULL) {
        // Check space for deviceOffsets is allocated.
        if (zend_hash_find(&EG(symbol_table), server, sizeof(server),
                                                (void**)&arr) != FAILURE) {
            HashTable* ht = Z_ARRVAL_P(*arr);
            // Loop through important HTTP headers.
            for (i = 0; i < fiftyoneDegreesGetHttpHeaderCount(); i++) {
                // Get header name.
                header = fiftyoneDegreesGetPrefixedUpperHttpHeaderName(i);
                header_len = strlen(header) + 1;
                // Get header value.
                if (zend_hash_find(ht, header, header_len, (void**)&val)
                                                                != FAILURE) {
                    propertyValue = Z_STRVAL_PP(val);
                    // Get deviceOffset reference.
                    fiftyoneDegreesDeviceOffset *offset =
                        (fiftyoneDegreesDeviceOffset*)
                            (FIFTYONE_G(deviceOffsets)->
                                firstOffset + FIFTYONE_G(deviceOffsets)->size);
                    // Populate relevant fields in the current deviceOffset.
                    fiftyoneDegreesSetDeviceOffset(propertyValue, i, offset);
                    FIFTYONE_G(deviceOffsets)->size++;
                }
            }
        }
    }
    array_init(return_value);
    buildArray(return_value);
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
        php_error(E_WARNING, "51Degrees dataset initialisation failed.");
        return;
    }
    // Check that User-Agent was passed to this function.
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                        "s", &useragent, &useragent_len) == SUCCESS) {
        // Initialise array for returned values.
        array_init(return_value);
        // Match the provided User-Agent.
        matchUserAgent(useragent, return_value);
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
        php_error(E_WARNING, "51Degrees dataset initialisation failed.");
        return;
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                        "s", &headers, &headers_len) == SUCCESS) {
        // Initialise array for returned values.
        array_init(return_value);
        // Match the provided User-Agent.
        matchHttpHeaders(headers, headers_len, return_value);
    }
    else
    {
        php_error(E_WARNING, "Could not parse arguments.");
    }
}

/**
 * LEGACY Function. Returns the same result as the
 * fiftyone_match_with_useragent, exists for compatibility purposes. Deprecated
 * and should not be used. Function is exposed to PHP.
 * @param useragent a string containing the HTTP User-Agent to identify.
 * @returns a PHP array of Property -> Value pairs.
 * @deprecated
 */
PHP_FUNCTION(fiftyone_degrees_get_properties)
{
    char* useragent;
    int useragent_len;
    // Stop if there was a problem initialising the dataset.
    if (datasetInitSuccess() == 0)
    {
        php_error(E_WARNING, "51Degrees dataset initialisation failed.");
        return;
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                        "s", &useragent, &useragent_len) == SUCCESS) {
        array_init(return_value);
        matchUserAgent(useragent, return_value);
    } else {
        php_error(E_WARNING, "Could not parse arguments.");
    }
}

/**
 * Function returns an array of HTTP headers relevant for device detection.
 * Function is exposed to PHP.
 * @return a PHP array of HTTP headers relevant for device detection.
 */
PHP_FUNCTION(fiftyone_get_http_headers) {
    const char* header;
    int index;
    // Stop if there was a problem initialising the dataset.
    if (datasetInitSuccess() == 0)
    {
        php_error(E_WARNING, "51Degrees dataset initialisation failed.");
        return;
    }

    array_init(return_value);
    index = 0;
    header = fiftyoneDegreesGetPrefixedUpperHttpHeaderName(index);
    while(header != NULL) {
        if (header != NULL) {
            add_next_index_string(return_value, header, 1);
        }
        header = fiftyoneDegreesGetPrefixedUpperHttpHeaderName(index);
        index++;
    }
}

/* PHP Zend init and shutdown functions */

/**
 * This section is responsible for loading settigns from the php.ini file.
 */
PHP_INI_BEGIN()
PHP_INI_ENTRY("fiftyone_degrees.data_file", "/usr/lib/php5/51Degrees.dat", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("fiftyone_degrees.property_list", NULL, PHP_INI_ALL, NULL)
PHP_INI_END()

/**
 * Module startup function. All of the resources with long lifespan get
 * initialised here.
 */
PHP_MINIT_FUNCTION(fiftyone_degrees_detector_init)
{
    REGISTER_INI_ENTRIES();
    char* dataFilePath = INI_STR("fiftyone_degrees.data_file");
    char* propertyList = INI_STR("fiftyone_degrees.property_list");
    initStatus = fiftyoneDegreesInitWithPropertyString(
                    dataFilePath,
                    propertyList);
    return SUCCESS;
}

/**
 * Module shutdown function. Used to free all of the resources with
 * long lifespan.
 */
PHP_MSHUTDOWN_FUNCTION(fiftyone_degrees_detector_shutdown)
{
    UNREGISTER_INI_ENTRIES();
    fiftyoneDegreesDestroy();
    return SUCCESS;
}

/**
 * Request startup function. Called upon each request.
 * Allocates space to store device offsets that will be used to fetch
 * detection results.
 */
PHP_RINIT_FUNCTION(fiftyone_degrees_request_init) {
    FIFTYONE_G(deviceOffsets) = fiftyoneDegreesCreateDeviceOffsets();
    return SUCCESS;
}

/**
 * Request shutdown function. Called at the end of each request.
 * Releases space allocated for the device offsets.
 */
PHP_RSHUTDOWN_FUNCTION(fiftyone_degrees_request_shutdown) {
    fiftyoneDegreesFreeDeviceOffsets(FIFTYONE_G(deviceOffsets));
    return SUCCESS;
}
