/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2014 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */

/*
 * Please review the README.md file for instructions to build this
 * code using SWIG. This code is dependent upon the 51Degees.h which
 * is now contained with the main C library
 */

%module "FiftyOneDegrees::PatternV3"
%{
	#include "../../../src/pattern/51Degrees.h"
	#ifdef __cplusplus
	#define EXTERNAL extern "C"
	#else
	#define EXTERNAL
	#endif

	fiftyoneDegreesDataSetInitStatus initStatus;
	fiftyoneDegreesDataSetInitStatus getInitStatus() {
		return initStatus;
	}
	typedef struct fiftyoneDegrees_instance_t {
		fiftyoneDegreesDataSet *dataSet;
		fiftyoneDegreesResultsetCache *cache;
		fiftyoneDegreesWorksetPool *pool;
	} fiftyoneDegreesInstance;
%}

%include "../../../src/pattern/51Degrees.h"

%include exception.i

/*
 * Exceptions returned by the C code are handled here.
 */
%exception dataSetInitWithPropertyString {

 	$action;
	fiftyoneDegreesDataSetInitStatus initStatus = getInitStatus();
	switch (initStatus) {
		case DATA_SET_INIT_STATUS_SUCCESS: // nothing to do
			break;
		case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
        	SWIG_exception(SWIG_MemoryError, "Insufficient memory allocated.");
      		break;
      	case DATA_SET_INIT_STATUS_CORRUPT_DATA:
			SWIG_exception(SWIG_RuntimeError, "The data was not the correct format. Check it is uncompressed.");
			break;
		case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
			SWIG_exception(SWIG_RuntimeError, "The data is an unsupported version. Check you have the latest data and API.");
			break;
		case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
			SWIG_exception(SWIG_IOError, "The data file could not be found. Check the file path and that the program has sufficient read permissions.");
			break;
	}
}

/* Methods that return new objects that SWIG should be responsible for freeing.
 */
%newobject getMatch;
%newobject getMatchWithHeaders;

%inline %{

	/* Initialise the dataset using the datafile and properties required. This
	 * method also initialises the cache and pool.
	 */
	fiftyoneDegreesInstance* dataSetInitWithPropertyString(char* fileName, char* propertyString, int cacheSize, int poolSize) {
		fiftyoneDegreesInstance *instance = (fiftyoneDegreesInstance*)malloc(sizeof(fiftyoneDegreesInstance));
		instance->dataSet = (fiftyoneDegreesDataSet*)malloc(sizeof(fiftyoneDegreesDataSet));
		initStatus = fiftyoneDegreesInitWithPropertyString((char*)fileName, instance->dataSet, propertyString);
		if (initStatus != DATA_SET_INIT_STATUS_SUCCESS)
		{
			free(instance->dataSet);
			instance->dataSet = NULL;
		}
        instance->cache = fiftyoneDegreesResultsetCacheCreate(instance->dataSet, cacheSize);
        instance->pool = fiftyoneDegreesWorksetPoolCreate(instance->dataSet, instance->cache, poolSize);
        return instance;
	}

	/* This method is exposed to the PERL and is used to free all pools, cache
	 * and dataset.
	 */
	void destroyDataset(fiftyoneDegreesInstance* instance) {
		// Free the cache if one was created.
		if (instance->cache != NULL){
			fiftyoneDegreesResultsetCacheFree(instance->cache);
			instance->cache = NULL;
		}
		// Free the pool if one was created.
		if (instance->pool != NULL) {
			fiftyoneDegreesWorksetPoolFree(instance->pool);
			instance->pool = NULL;
		}
		// Free the dataset if one was created.
		if (instance->dataSet != NULL) {
			fiftyoneDegreesDataSetFree(instance->dataSet);
			instance->dataSet = NULL;
		}
	}

	/* Returns the HTTP header name for the index provided, or NULL if
	 * no header exists at the index.
	 */
	char* getHttpHeaderName(fiftyoneDegreesInstance* instance, int httpHeaderIndex) {
		return fiftyoneDegreesGetPrefixedUpperHttpHeaderName(instance->dataSet, httpHeaderIndex);
	}

	/* Methods used for matching. */

	/* Returns a JSON string with the required properties set for the
	 * User-Agent provided.
	 */
	char* getMatch(fiftyoneDegreesInstance* instance, char* userAgent) {
		fiftyoneDegreesWorkset *ws = fiftyoneDegreesWorksetPoolGet(instance->pool);
		fiftyoneDegreesMatch(ws, userAgent);
        char *output = fiftyoneDegreesJSONCreate(ws);
		fiftyoneDegreesProcessDeviceJSON(ws, output);
		fiftyoneDegreesWorksetPoolRelease(instance->pool, ws);
		return output;
	}

	/* Returns a JSON string with the required properties set for the
	 * HTTP headers provided.
	 */
	char* getMatchWithHeaders(fiftyoneDegreesInstance* instance, char* userHeader) {
        fiftyoneDegreesWorkset *ws = fiftyoneDegreesWorksetPoolGet(instance->pool);
        fiftyoneDegreesMatchWithHeadersString(ws, userHeader, strlen(userHeader));
        char *output = fiftyoneDegreesJSONCreate(ws);
        fiftyoneDegreesProcessDeviceJSON(ws, output);
        fiftyoneDegreesWorksetPoolRelease(instance->pool, ws);
        return output;
	}
%}
