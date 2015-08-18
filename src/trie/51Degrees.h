/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright © 2014 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
#ifndef FIFTYONEDEGREES_H_INCLUDED
#define FIFTYONEDEGREES_H_INCLUDED

#ifndef FIFTYONEDEGREES_H_TRIE_INCLUDED
#define FIFTYONEDEGREES_H_TRIE_INCLUDED
#endif

#ifdef __cplusplus
#define EXTERNAL extern "C"
#else
#define EXTERNAL
#endif

/* Used to provide the status of the data set initialisation */
typedef enum e_fiftyoneDegreesDataSetInitStatus {
    DATA_SET_INIT_STATUS_SUCCESS,
    DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY,
    DATA_SET_INIT_STATUS_CORRUPT_DATA,
    DATA_SET_INIT_STATUS_INCORRECT_VERSION,
    DATA_SET_INIT_STATUS_FILE_NOT_FOUND,
	DATA_SET_INIT_STATUS_NOT_SET
} fiftyoneDegreesDataSetInitStatus;

/* Relates a http header index to to a device offset */
typedef struct fiftyoneDegrees_device_offset_t {
	int httpHeaderOffset; /* Offset to the http header string */
	int deviceOffset; /* Offset to the device */
} fiftyoneDegreesDeviceOffset;

/* Used to return results from a device detection operation */
typedef struct fiftyoneDegrees_device_offsets_t {
	int size; /* The number of records in the array */
	fiftyoneDegreesDeviceOffset firstOffset; /* First item in the array of offsets */
} fiftyoneDegreesDeviceOffsets;

// Initialises the memory using the file and properies provided.
EXTERNAL fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyArray(char* fileName, char** properties, int propertyCount);
EXTERNAL fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyString(char *fileName, char *properties);

// Returns the offset to a matching device based on the useragent provided.
EXTERNAL int fiftyoneDegreesGetDeviceOffset(char *userAgent);

// Returns the offsets to a matching devices based on the http headers provided.
EXTERNAL fiftyoneDegreesDeviceOffsets* fiftyoneDegreesGetDeviceOffsetsWithHeadersString(char *httpHeaders, size_t length);

// Returns the index of the property requested, or -1 if not available.
EXTERNAL int fiftyoneDegreesGetPropertyIndex(char *value);

// Takes the results of getDeviceOffset and getPropertyIndex to return a value.
EXTERNAL char* fiftyoneDegreesGetValue(int deviceOffset, int propertyIndex);

// Returns how many properties have been loaded in the dataset.
EXTERNAL int fiftyoneDegreesGetRequiredPropertiesCount(void);

// Returns the names of the properties loaded in the dataset.
EXTERNAL char** fiftyoneDegreesGetRequiredPropertiesNames(void);

// Frees the memory.
EXTERNAL void fiftyoneDegreesDestroy(void);

// Sets the http header string to the header name at the index provided.
EXTERNAL int fiftyoneDegreesGetHttpHeaderName(int httpHeaderIndex, char* httpHeader, int size);

// Sets the propertyname string to the property name at the index provided.
EXTERNAL int fiftyoneDegreesGetRequiredPropertyName(int requiredPropertyIndex, char* propertyName, int size);

// Sets the values string to the property values for the device offests and index provided.
EXTERNAL int fiftyoneDegreesGetValueFromOffsets(fiftyoneDegreesDeviceOffsets* deviceOffsets, int requiredPropertyIndex, char* values, int size);

// Converts the device offset to a CSV string returning the number of
// characters used.
EXTERNAL int fiftyoneDegreesProcessDeviceCSV(int deviceOffset, char* result, int resultLength);

// Converts the device offset to a JSON string returning the number of
// characters used.
EXTERNAL int fiftyoneDegreesProcessDeviceJSON(int deviceOffset, char* result, int resultLength);

// Returns the number of characters which matched in the trie.
EXTERNAL int fiftyoneDegreesGetMatchedUserAgentLength(char *userAgent);

#endif // 51DEGREES_H_INCLUDED
