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

%module "FiftyOneDegrees::TrieV3"
%{

	#include "../../../src/trie/51Degrees.h"

	#ifdef __cplusplus
	#define EXTERNAL extern "C"
	#else
	#define EXTERNAL
	#endif

	fiftyoneDegreesDataSetInitStatus initStatus;
	fiftyoneDegreesDataSetInitStatus getInitStatus() {
		return initStatus;
	}

%}

%include "../../../src/trie/51Degrees.h"

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

	// The size of the output buffer containing the returned JSON string.
	static int maxBufferSize = 10;

	// The name of the file used to initialise the current data.
	static char* currentFileName = NULL;

	/* Releases the memory used for device detection ready for
	 * reinitialisation.
	 */
	void destroy() {
		fiftyoneDegreesDestroy();
		free(currentFileName);
		currentFileName = NULL;
	}

	/* Initialises the detector using the file provided. The JSON structure
	 * returned from a match will contain the properties specified.
	 */
	void dataSetInitWithPropertyString(char *fileName, char* properties) {
		if (currentFileName == NULL) {
			initStatus = (fiftyoneDegreesDataSetInitStatus)fiftyoneDegreesInitWithPropertyString(fileName, properties);
			if (initStatus != DATA_SET_INIT_STATUS_SUCCESS) {
				fiftyoneDegreesDestroy();
			}
			currentFileName = strdup(fileName);
		}
		else if (strcmp(currentFileName, fileName) != 0) {
			initStatus = DATA_SET_INIT_STATUS_NOT_SET;
		}
	}

	/* Returns the HTTP header name for the index provided, or NULL if
	 * no header exists at the index.
	 */
	char* getHttpHeaderName(int httpHeaderIndex) {
		if (httpHeaderIndex >= 0 &&
			httpHeaderIndex < fiftyoneDegreesGetHttpHeaderCount()) {
			return fiftyoneDegreesGetPrefixedUpperHttpHeaderName(httpHeaderIndex);
		}
		return NULL;
	}

	/* Methods used for matching. */

	/* Returns a JSON string with the required properties set for the
	 * User-Agent provided.
	 */
	char* getMatch(char* userAgent) {
		int bufferSize = maxBufferSize;
		char *output = (char*)malloc(bufferSize * sizeof(char));
		int deviceOffset = fiftyoneDegreesGetDeviceOffset(userAgent);
		int result = fiftyoneDegreesProcessDeviceJSON(deviceOffset, output, bufferSize);
		while (result < 0) {
			free(output);
			bufferSize += 1000;
			output = (char*)malloc(bufferSize * sizeof(char));
			result = fiftyoneDegreesProcessDeviceJSON(deviceOffset, output, bufferSize);
			if (result > 0) {
				maxBufferSize = bufferSize;
			}
		}
		return output;
	}

	/* Returns a JSON string with the required properties set for the
	 * HTTP headers provided.
	 */
	char* getMatchWithHeaders(char* userHeader) {
		int bufferSize = maxBufferSize;
		char *output = (char*)malloc(bufferSize * sizeof(char));
		fiftyoneDegreesDeviceOffsets* deviceOffsets = fiftyoneDegreesGetDeviceOffsetsWithHeadersString(
			userHeader,
			strlen(userHeader));
		int result = fiftyoneDegreesProcessDeviceOffsetsJSON(deviceOffsets, output, bufferSize);
		while (result < 0) {
			free(output);
			bufferSize += 100;
			output = (char*)malloc(bufferSize * sizeof(char));
			result = fiftyoneDegreesProcessDeviceOffsetsJSON(deviceOffsets, output, bufferSize);
			if (result > 0) {
				maxBufferSize = bufferSize;
			}
		}
		free(deviceOffsets);
		return output;
  	}
%}
