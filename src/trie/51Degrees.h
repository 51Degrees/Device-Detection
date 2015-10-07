/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
	size_t length; /* Number of characters in the matched User-Agent*/
	const char *userAgent; /* Pointer to the User-Agent */
	int difference; /* Difference in length between the target and matched User-Agent*/
} fiftyoneDegreesDeviceOffset;

/* Used to return results from a device detection operation */
typedef struct fiftyoneDegrees_device_offsets_t {
	int size; /* The number of records in the array */
	fiftyoneDegreesDeviceOffset *firstOffset; /* First item in the array of offsets */
} fiftyoneDegreesDeviceOffsets;

// Initialises the memory using the file and properies provided.
EXTERNAL fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyArray(const char* fileName, const char** properties, int propertyCount);
EXTERNAL fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyString(const char *fileName, const char *properties);

// Returns the offset to a matching device based on the useragent provided.
EXTERNAL int fiftyoneDegreesGetDeviceOffset(const char *userAgent);

// Sets the offsets structure passed to the method for the useragent provided.
EXTERNAL void fiftyoneDegreesSetDeviceOffset(const char* userAgent, int httpHeaderIndex, fiftyoneDegreesDeviceOffset *offset);

// Returns the offsets to a matching devices based on the http headers provided.
EXTERNAL fiftyoneDegreesDeviceOffsets* fiftyoneDegreesGetDeviceOffsetsWithHeadersString(char *httpHeaders, size_t length);

// Creates a new device offsets structure with memory allocated.
EXTERNAL fiftyoneDegreesDeviceOffsets* fiftyoneDegreesCreateDeviceOffsets();

// Frees the memory used by the offsets.
EXTERNAL void fiftyoneDegreesFreeDeviceOffsets(fiftyoneDegreesDeviceOffsets* offsets);

// Returns the offsets to a matching devices based on the http headers provided.
EXTERNAL void fiftyoneDegreesSetDeviceOffsetsWithHeadersString(fiftyoneDegreesDeviceOffsets *offsets, char *httpHeaders, size_t size);

// Returns the index of the property requested, or -1 if not available.
EXTERNAL int fiftyoneDegreesGetPropertyIndex(const char *value);

// Takes the results of getDeviceOffset and getPropertyIndex to return a value.
EXTERNAL const char* fiftyoneDegreesGetValue(int deviceOffset, int propertyIndex);

// Returns how many properties have been loaded in the dataset.
EXTERNAL int fiftyoneDegreesGetRequiredPropertiesCount(void);

// Returns the names of the properties loaded in the dataset.
EXTERNAL const char** fiftyoneDegreesGetRequiredPropertiesNames(void);

// Returns the index in the array of required properties for this name, or -1 if not found.
EXTERNAL int fiftyoneDegreesGetRequiredPropertyIndex(const char *propertyName);

// Frees the memory.
EXTERNAL void fiftyoneDegreesDestroy(void);

// Returns the number of HTTP headers relevent to device detection.
EXTERNAL int fiftyoneDegreesGetHttpHeaderCount(void);

// Returns the HTTP header name offset at the index provided.
EXTERNAL int fiftyoneDegreesGetHttpHeaderNameOffset(int httpHeaderIndex);

// Returns a pointer to the HTTP header name at the index provided.
EXTERNAL const char* fiftyoneDegreesGetHttpHeaderNamePointer(int httpHeaderIndex);

// Returns a pointer to the prefixed upper HTTP header name at the index provided.
EXTERNAL const char* fiftyoneDegreesGetPrefixedUpperHttpHeaderName(int httpHeaderIndex);

// Sets the http header string to the header name at the index provided.
EXTERNAL int fiftyoneDegreesGetHttpHeaderName(int httpHeaderIndex, char* httpHeader, int size);

// Gets the unique index of the header, or -1 if the header isn't important.
EXTERNAL int fiftyoneDegreesGetUniqueHttpHeaderIndex(char* httpHeaderName, int length);

// Sets the propertyname string to the property name at the index provided.
EXTERNAL int fiftyoneDegreesGetRequiredPropertyName(int requiredPropertyIndex, char* propertyName, int size);

// Sets the values string to the property values for the device offests and index provided.
EXTERNAL int fiftyoneDegreesGetValueFromOffsets(fiftyoneDegreesDeviceOffsets* deviceOffsets, int requiredPropertyIndex, char* values, int size);

// Returns a pointer to the value for the property based on the device offsets provided.
EXTERNAL const char* fiftyoneDegreesGetValuePtrFromOffsets(fiftyoneDegreesDeviceOffsets* deviceOffsets, int requiredPropertyIndex);

// Converts the device offset to a CSV string returning the number of
// characters used.
EXTERNAL int fiftyoneDegreesProcessDeviceCSV(int deviceOffset, char* result, int resultLength);

// Converts the device offset to a JSON string returning the number of
// characters used.
EXTERNAL int fiftyoneDegreesProcessDeviceJSON(int deviceOffset, char* result, int resultLength);

// Process device properties into a JSON string for the device offsets provided.
EXTERNAL int fiftyoneDegreesProcessDeviceOffsetsJSON(fiftyoneDegreesDeviceOffsets *deviceOffsets, char* result, int resultLength);

// Returns the number of characters which matched in the trie.
EXTERNAL int fiftyoneDegreesGetMatchedUserAgentLength(char *userAgent);

#endif // 51DEGREES_H_INCLUDED
