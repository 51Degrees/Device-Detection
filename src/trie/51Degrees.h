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

#include <stdint.h>

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

// Used to map a byte from the data file.
#define BYTE unsigned char

// The maximum value of a byte.
#define BYTE_MAX 255

// The values of the possible offset types.
#define BITS16 (BYTE)0
#define BITS32 (BYTE)1
#define BITS64 (BYTE)2

#define HTTP_PREFIX_UPPER "HTTP_"

#pragma pack(push, 1)
typedef struct t_node_children {
	BYTE numberOfChildren;
	BYTE offsetType;
	union {
		uint16_t b16;
		uint32_t b32;
		int64_t b64;
	} childrenOffsets;
} NODE_CHILDREN;
#pragma pack(pop)

// Type used to represent a full node that
// includes a device index.
#pragma pack(push, 1)
typedef struct t_node_full {
	int lookupListOffset;
	int deviceIndex;
	NODE_CHILDREN children;
} NODE_FULL;
#pragma pack(pop)

// Type used to represent a node that does not
// include a device index.
#pragma pack(push, 1)
typedef struct t_node_no_device_index {
	int lookupListOffset;
	NODE_CHILDREN children;
} NODE_NO_DEVICE_INDEX;
#pragma pack(pop)

// Type used to represent a look up list header.
#pragma pack(push, 1)
typedef struct t_lookup_header {
	BYTE lowest;
	BYTE highest;
	BYTE start;
} LOOKUP_HEADER;
#pragma pack(pop)

// A property including references to HTTP headers.
#pragma pack(push, 4)
typedef struct fiftyoneDegrees_property_t {
	const int32_t stringOffset;
	const int32_t headerCount;
	const int32_t headerFirstIndex;
} fiftyoneDegreesProperty;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_dataset_t {
	int32_t copyrightSize; /* The size of the copyright notice at the top of the data file. */
	char *copyright = NULL; /* Pointer to the copyright notice held in the data file. */
	int32_t stringsSize; /* The size of the strings data array. */
	char *strings = NULL; /* Pointer to the start of the strings data array. */
	int32_t httpHeadersSize; /* The size of the HTTP headers data array. */
	int32_t *httpHeaders; /* Pointer to the start of the HTTP headers data array. */
	int32_t uniqueHttpHeaderCount; /* The number of unique http headers. */
	int32_t *uniqueHttpHeaders = NULL; /* Pointer to the unique list of HTTP headers. */
	const char **prefixedUpperHttpHeaders = NULL; /* Pointer to an array of prefixed upper HTTP headers. */
	int32_t propertiesCount; /* The number of properties contained in the system. */
	int32_t propertiesSize; /* The size of the properties data array. */
	fiftyoneDegreesProperty *properties = NULL; /* Pointer to the start of the properties data array. */
	int32_t *devices = NULL; /* Pointer to the start of the devices data array. */
	int32_t devicesSize; /* The size of the devices data array. */
	int32_t lookupListSize; /* The size of the memory reserved for lookup lists. */
	LOOKUP_HEADER *lookupList = NULL; /* Pointer to the start of the lookup lists. */
	int32_t *rootNode = NULL; /* Offset in the device data file for the root node. */
	int64_t nodesSize; /* The size of the data array containing the nodes. */
	int requiredPropertiesCount; /* The number of properties to be returned. */
	uint32_t *requiredProperties = NULL; /* A list of required property indexes. */
	const char **requiredPropertiesNames = NULL; /* A list of pointers to the names of the properties. */
} fiftyoneDegreesDataSet;
#pragma pack(pop)

// Initialises the memory using the file and properies provided.
EXTERNAL fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyArray(const char* fileName, fiftyoneDegreesDataSet *dataSet, const char** properties, int propertyCount);
EXTERNAL fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyString(const char *fileName, fiftyoneDegreesDataSet *dataSet, const char *properties);

// Returns the offset to a matching device based on the useragent provided.
EXTERNAL int fiftyoneDegreesGetDeviceOffset(fiftyoneDegreesDataSet *dataSet, const char *userAgent);

// Sets the offsets structure passed to the method for the useragent provided.
EXTERNAL void fiftyoneDegreesSetDeviceOffset(fiftyoneDegreesDataSet *dataSet, const char* userAgent, int httpHeaderIndex, fiftyoneDegreesDeviceOffset *offset);

// Returns the offsets to a matching devices based on the http headers provided.
EXTERNAL fiftyoneDegreesDeviceOffsets* fiftyoneDegreesGetDeviceOffsetsWithHeadersString(fiftyoneDegreesDataSet *dataSet, char *httpHeaders, size_t length);

// Creates a new device offsets structure with memory allocated.
EXTERNAL fiftyoneDegreesDeviceOffsets* fiftyoneDegreesCreateDeviceOffsets(fiftyoneDegreesDataSet *dataSet);

// Frees the memory used by the offsets.
EXTERNAL void fiftyoneDegreesFreeDeviceOffsets(fiftyoneDegreesDeviceOffsets* offsets);

// Returns the offsets to a matching devices based on the http headers provided.
EXTERNAL void fiftyoneDegreesSetDeviceOffsetsWithHeadersString(fiftyoneDegreesDataSet *dataSet, fiftyoneDegreesDeviceOffsets *offsets, char *httpHeaders, size_t size);

// Returns the index of the property requested, or -1 if not available.
EXTERNAL int fiftyoneDegreesGetPropertyIndex(fiftyoneDegreesDataSet *dataSet, const char *value);

// Takes the results of getDeviceOffset and getPropertyIndex to return a value.
EXTERNAL const char* fiftyoneDegreesGetValue(fiftyoneDegreesDataSet *dataSet, int deviceOffset, int propertyIndex);

// Returns how many properties have been loaded in the dataset.
EXTERNAL int fiftyoneDegreesGetRequiredPropertiesCount(fiftyoneDegreesDataSet *dataSet);

// Returns the names of the properties loaded in the dataset.
EXTERNAL const char** fiftyoneDegreesGetRequiredPropertiesNames(fiftyoneDegreesDataSet *dataSet);

// Returns the index in the array of required properties for this name, or -1 if not found.
EXTERNAL int fiftyoneDegreesGetRequiredPropertyIndex(fiftyoneDegreesDataSet *dataSet, const char *propertyName);

// Frees the memory.
EXTERNAL void fiftyoneDegreesDestroy(fiftyoneDegreesDataSet *dataSet);

// Returns the number of HTTP headers relevent to device detection.
EXTERNAL int fiftyoneDegreesGetHttpHeaderCount(fiftyoneDegreesDataSet *dataSet);

// Returns the HTTP header name offset at the index provided.
EXTERNAL int fiftyoneDegreesGetHttpHeaderNameOffset(fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex);

// Returns a pointer to the HTTP header name at the index provided.
EXTERNAL const char* fiftyoneDegreesGetHttpHeaderNamePointer(fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex);

// Returns a pointer to the prefixed upper HTTP header name at the index provided.
EXTERNAL const char* fiftyoneDegreesGetPrefixedUpperHttpHeaderName(fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex);

// Sets the http header string to the header name at the index provided.
EXTERNAL int fiftyoneDegreesGetHttpHeaderName(fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex, char* httpHeader, int size);

// Gets the unique index of the header, or -1 if the header isn't important.
EXTERNAL int fiftyoneDegreesGetUniqueHttpHeaderIndex(fiftyoneDegreesDataSet *dataSet, char* httpHeaderName, int length);

// Sets the propertyname string to the property name at the index provided.
EXTERNAL int fiftyoneDegreesGetRequiredPropertyName(fiftyoneDegreesDataSet *dataSet, int requiredPropertyIndex, char* propertyName, int size);

// Sets the values string to the property values for the device offests and index provided.
EXTERNAL int fiftyoneDegreesGetValueFromOffsets(fiftyoneDegreesDataSet *dataSet, fiftyoneDegreesDeviceOffsets* deviceOffsets, int requiredPropertyIndex, char* values, int size);

// Returns a pointer to the value for the property based on the device offsets provided.
EXTERNAL const char* fiftyoneDegreesGetValuePtrFromOffsets(fiftyoneDegreesDataSet *dataSet, fiftyoneDegreesDeviceOffsets* deviceOffsets, int requiredPropertyIndex);

// Converts the device offset to a CSV string returning the number of
// characters used.
EXTERNAL int fiftyoneDegreesProcessDeviceCSV(fiftyoneDegreesDataSet *dataSet, int deviceOffset, char* result, int resultLength);

// Converts the device offset to a JSON string returning the number of
// characters used.
EXTERNAL int fiftyoneDegreesProcessDeviceJSON(fiftyoneDegreesDataSet *dataSet, int deviceOffset, char* result, int resultLength);

// Process device properties into a JSON string for the device offsets provided.
EXTERNAL int fiftyoneDegreesProcessDeviceOffsetsJSON(fiftyoneDegreesDataSet *dataSet, fiftyoneDegreesDeviceOffsets *deviceOffsets, char* result, int resultLength);

// Returns the number of characters which matched in the trie.
EXTERNAL int fiftyoneDegreesGetMatchedUserAgentLength(fiftyoneDegreesDataSet *dataSet, char *userAgent);

#endif // 51DEGREES_H_INCLUDED
