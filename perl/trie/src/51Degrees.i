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

%inline %{

	static int outputBuffferSize = 10;
	static char* currentFileName = NULL;

	void destroy() {
		fiftyoneDegreesDestroy();
		free(currentFileName);
		currentFileName = NULL;
	}

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

	char* getMatch(char* userAgent) {
		char *output = (char*)malloc(outputBuffferSize * sizeof(char));
		int deviceOffset = fiftyoneDegreesGetDeviceOffset(userAgent);
		int result = fiftyoneDegreesProcessDeviceJSON(deviceOffset, output, outputBuffferSize);
		while (result < 0) {
			free(output);
			outputBuffferSize += 100;
			output = (char*)malloc(outputBuffferSize * sizeof(char));
			result = fiftyoneDegreesProcessDeviceJSON(deviceOffset, output, outputBuffferSize);
		}
		return output;
	}

	char* getMatchWithHeaders(char* userHeader) {
		char *output = (char*)malloc(outputBuffferSize * sizeof(char));
		fiftyoneDegreesDeviceOffsets* deviceOffsets = fiftyoneDegreesGetDeviceOffsetsWithHeadersString(
			userHeader,
			strlen(userHeader));
		int result = fiftyoneDegreesProcessDeviceOffsetsJSON(deviceOffsets, output, outputBuffferSize);
		while (result < 0) {
			free(output);
			outputBuffferSize += 100;
			output = (char*)malloc(outputBuffferSize * sizeof(char));
			result = fiftyoneDegreesProcessDeviceOffsetsJSON(deviceOffsets, output, outputBuffferSize);
		}
		free(deviceOffsets);
		return output;
  	}
%}



