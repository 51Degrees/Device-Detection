﻿#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "51Degrees.h"

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
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */

/**
 * PROBLEM METHODS
 */

/* Change snprintf to the Microsoft version */
#ifdef _MSC_FULL_VER
#define snprintf _snprintf
#endif

// Used to map a byte from the data file.
#define BYTE unsigned char

// The maximum value of a byte.
#define BYTE_MAX 255

// The values of the possible offset types.
#define BITS16 (BYTE)0
#define BITS32 (BYTE)1
#define BITS64 (BYTE)2

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

// The size of the copyright notice at the top of the data file.
static int32_t _copyrightSize;

// Pointer to the copyright notice held in the data file.
static char* _copyright = NULL;

// The size of the strings data array.
static int32_t _stringsSize;

// Pointer to the start of the strings data array.
static char* _strings = NULL;

// The size of the HTTP headers data array.
static int32_t _httpHeadersSize;

// Pointer to the start of the HTTP headers data array.
static int32_t* _httpHeaders = NULL;

// The number of unique http headers.
static int32_t _uniqueHttpHeaderCount;

// Pointer to the unique list of HTTP headers.
static int32_t* _uniqueHttpHeaders = NULL;

// The number of properties contained in the system.
static int32_t _propertiesCount;

// The size of the profiles data array.
static int32_t _propertiesSize;

// Pointer to the start of the pointers data array.
static fiftyoneDegreesProperty* _properties = NULL;

// Pointer to the start of the devices data array.
static int32_t* _devices = NULL;

// The size of the deviecs data array.
static int32_t _devicesSize;

// The size of the memory reserved for lookup lists.
static int32_t _lookupListSize;

// Pointer to the start of the lookup lists.
static LOOKUP_HEADER* _lookupList = NULL;

// Offset in the device data file for the root node.
static int32_t* _rootNode = NULL;

// The size of the data array containing the nodes.
static int64_t _nodesSize;

// The number of properties to be returned.
static int _requiredPropertiesCount;

// A list of the required property indexes.
static uint32_t* _requiredProperties = NULL;

// A list of pointers to the names of the properties.
static char** _requiredPropertiesNames = NULL;

// Reads the strings from the file.
fiftyoneDegreesDataSetInitStatus readStrings(FILE *inputFilePtr) {
	if (fread(&_stringsSize, sizeof(int32_t), 1, inputFilePtr) != 1)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	_strings = (char*)malloc(_stringsSize);
	if (_strings == NULL)
        return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(_strings, sizeof(BYTE), (size_t)_stringsSize, inputFilePtr) != (size_t)_stringsSize)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
    return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the HTTP headers from the file.
fiftyoneDegreesDataSetInitStatus readHttpHeaders(FILE *inputFilePtr) {
	int headerIndex, uniqueHeaderIndex;
	if (fread(&_httpHeadersSize, sizeof(int32_t), 1, inputFilePtr) != 1)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	_httpHeaders = (int32_t*)malloc(_httpHeadersSize);
	if (_httpHeaders == NULL)
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(_httpHeaders, sizeof(BYTE), (size_t)_httpHeadersSize, inputFilePtr) != (size_t)_httpHeadersSize)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	
	// Set the unique HTTP header names;
	_uniqueHttpHeaders = (int32_t*)malloc(_httpHeadersSize);
	for (headerIndex = 0; headerIndex < (int)(_httpHeadersSize / sizeof(int32_t)); headerIndex++) {
		for (uniqueHeaderIndex = 0; uniqueHeaderIndex < _uniqueHttpHeaderCount; uniqueHeaderIndex++) {
			if (*(_uniqueHttpHeaders + uniqueHeaderIndex) == *(_httpHeaders + headerIndex)) {
				break;
			}
		}
		if (uniqueHeaderIndex == _uniqueHttpHeaderCount) {
			*(_uniqueHttpHeaders + _uniqueHttpHeaderCount) = *(_httpHeaders + headerIndex);
			_uniqueHttpHeaderCount++;
		}
	}

	return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the properties from the file.
fiftyoneDegreesDataSetInitStatus readProperties(FILE *inputFilePtr) {
	if(fread(&_propertiesSize, sizeof(int32_t), 1, inputFilePtr) != 1)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	_properties = (fiftyoneDegreesProperty*)malloc(_propertiesSize);
	if (_properties == NULL)
        return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(_properties, sizeof(BYTE), (size_t)_propertiesSize, inputFilePtr) != (size_t)_propertiesSize)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	_propertiesCount = _propertiesSize / sizeof(fiftyoneDegreesProperty);
	return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the profiles from the file.
fiftyoneDegreesDataSetInitStatus readDevices(FILE *inputFilePtr) {
	if (fread(&_devicesSize, sizeof(int32_t), 1, inputFilePtr) != 1)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	_devices = (int32_t*)malloc(_devicesSize);
	if (_devices == NULL)
        return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(_devices, sizeof(BYTE), (size_t)_devicesSize, inputFilePtr) != (size_t)_devicesSize)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
    return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the lookups from the input file provided.
fiftyoneDegreesDataSetInitStatus readLookupList(FILE *inputFilePtr) {
	if (fread(&_lookupListSize, sizeof(int32_t), 1, inputFilePtr) != 1)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	_lookupList = (LOOKUP_HEADER*)malloc(_lookupListSize);
	if (_lookupList ==NULL)
        return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(_lookupList, sizeof(BYTE), _lookupListSize, inputFilePtr) != (size_t)_lookupListSize)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
    return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the nodes byte array into memory.
fiftyoneDegreesDataSetInitStatus readNodes(FILE *inputFilePtr) {
	if (fread(&_nodesSize, sizeof(int64_t), 1, inputFilePtr) != 1)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	_rootNode = (int32_t*)malloc((size_t)_nodesSize);
    if (_rootNode == 0)
        return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (_rootNode > 0) {
        if (fread(_rootNode, sizeof(BYTE), (size_t)_nodesSize, inputFilePtr) != (size_t)_nodesSize) {
            return DATA_SET_INIT_STATUS_CORRUPT_DATA;
        }
	}

    return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the copyright message into memory.
fiftyoneDegreesDataSetInitStatus readCopyright(FILE *inputFilePtr) {
	if (fread(&_copyrightSize, sizeof(int32_t), 1, inputFilePtr) != 1)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	_copyright = (char*)malloc(_copyrightSize);
	if (_copyright == NULL)
        return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(_copyright, sizeof(BYTE), (size_t)_copyrightSize, inputFilePtr) != (size_t)_copyrightSize)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
    return DATA_SET_INIT_STATUS_SUCCESS;
}

// Fress the memory.
void fiftyoneDegreesDestroy(void) {
	if (_copyright != NULL) {
		free(_copyright);
		_copyright = NULL;
	}
	if (_requiredProperties != NULL) {
		free(_requiredProperties); _requiredProperties = NULL;
	}
	if (_rootNode != NULL) {
		free(_rootNode); _rootNode = NULL;
	}
	if (_lookupList != NULL) {
		free(_lookupList); _lookupList = NULL;
	}
	if (_devices != NULL) {
		free(_devices); _devices = NULL;
	}
	if (_properties != NULL) {
		free(_properties); _properties = NULL;
	}
	if (_uniqueHttpHeaders != NULL) {
		free(_uniqueHttpHeaders); _uniqueHttpHeaders = NULL;
	}
	if (_httpHeaders != NULL) {
		free(_httpHeaders); _httpHeaders = NULL;
	}
	if (_strings != NULL) {
		free(_strings); _strings = NULL;
	}
}

// Reads the version value from the start of the file and returns
// 0 if the file is in a format that can be read by this code.
fiftyoneDegreesDataSetInitStatus readVersion(FILE *inputFilePtr) {
	uint16_t version;
	if (fread(&version, sizeof(uint16_t), 1, inputFilePtr) != -1) {
	   if (version != 32)
           return DATA_SET_INIT_STATUS_INCORRECT_VERSION;
       return DATA_SET_INIT_STATUS_SUCCESS;
   }
   return DATA_SET_INIT_STATUS_CORRUPT_DATA;
}

// Reads the input file into memory returning 1 if it
// was read unsuccessfully, otherwise 0.
fiftyoneDegreesDataSetInitStatus readFile(char* fileName) {
	#define READMETHODS 8
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;
	FILE *inputFilePtr;
	int readMethod;
	fiftyoneDegreesDataSetInitStatus(*m[READMETHODS]) (FILE *inputFilePtr);
	m[0] = readVersion;
	m[1] = readCopyright;
	m[2] = readStrings;
	m[3] = readHttpHeaders;
	m[4] = readProperties;
	m[5] = readDevices;
	m[6] = readLookupList;
	m[7] = readNodes;

	// Open the file and hold on to the pointer.
	inputFilePtr = fopen(fileName, "rb");

	// If the file didn't open return -1.
	if (inputFilePtr == NULL) {
        return DATA_SET_INIT_STATUS_FILE_NOT_FOUND;
	}
	// Read the various data segments if the version is
	// one we can read.
	for (readMethod = 0; readMethod < READMETHODS; readMethod++) {
		status = m[readMethod](inputFilePtr);
		if (status != DATA_SET_INIT_STATUS_SUCCESS) {
			fiftyoneDegreesDestroy();
			break;
		}
	}
	fclose(inputFilePtr);

	return status;
}

// Returns the index of the property requested, or -1 if not available.
int getPropertyIndexRange(char *property, size_t length) {
	int32_t i = 0;
	for(i = 0; i < _propertiesCount; i++) {
		if(strncmp(
			_strings + (_properties + i)->stringOffset,
			property,
			length) == 0) {
			return i;
		}
	}
	return -1;
}

// Initialises the properties provided.
void initSpecificProperties(char* properties) {
	char *start, *end;
	int propertyIndex, currentIndex = 0;

	// Count the number of valid properties.
	_requiredPropertiesCount = 0;
	start = properties;
	end = properties - 1;
	do {
		end++;
		if (*end == '|' || *end == ',' || *end == '\0') {
			// Check the property we've just reached is valid and
			// if it is then increase the count.
			if (getPropertyIndexRange(start, (end - start)) > 0)
				_requiredPropertiesCount++;
			start = end + 1;
		}
	} while (*end != '\0');

	// Create enough memory for the properties.
	_requiredProperties = (uint32_t*)malloc(_requiredPropertiesCount * sizeof(int));
	_requiredPropertiesNames = (char**)malloc(_requiredPropertiesCount * sizeof(char*));

	start = properties;
	end = properties - 1;
	do {
		end++;
		if (*end == '|' || *end == ',' || *end == '\0') {
			// If this is a valid property add it to the list.
			propertyIndex = getPropertyIndexRange(start, (end - start));
			if (propertyIndex > 0) {
				*(_requiredProperties + currentIndex) = propertyIndex;
				*(_requiredPropertiesNames + currentIndex) = _strings + (_properties + propertyIndex)->stringOffset;
				currentIndex++;
			}
			start = end + 1;
		}

	} while (*end != '\0');
}

// Initialises the properties provided.
void initSpecificPropertiesFromArray(char** properties, int count) {
    int i;
    int propertyIndex, currentIndex = 0;
    char *currentProperty;
    int currentLength = 0;

	// Count the number of valid properties.
	_requiredPropertiesCount = 0;
    for (i = 0; i < count; i++) {
      currentProperty = properties[i];
      currentLength = (int)strlen(currentProperty);
      if (getPropertyIndexRange(currentProperty, currentLength) > 0)
            _requiredPropertiesCount++;
    }

	// Create enough memory for the properties.
	_requiredProperties = (uint32_t*)malloc(_requiredPropertiesCount * sizeof(int));
	_requiredPropertiesNames = (char**)malloc(_requiredPropertiesCount * sizeof(char*));
	
	// Initialise the requiredProperties array.
	for (i = 0; i < count; i++ ) {
      currentProperty = properties[i];
      currentLength = (int)strlen(currentProperty);
      // If this is a valid property add it to the list.
		propertyIndex = getPropertyIndexRange(currentProperty, currentLength);
		if (propertyIndex > 0) {
			*(_requiredProperties + currentIndex) = propertyIndex;
			*(_requiredPropertiesNames + currentIndex) = _strings + (_properties + propertyIndex)->stringOffset;
			currentIndex++;
		}
   }
}

// Initialises all the available properties.
void initAllProperties(void) {
	int32_t i;

	// Set to include all properties.
	_requiredPropertiesCount = _propertiesCount;

	// Create enough memory for the properties.
	_requiredProperties = (uint32_t*)malloc(_requiredPropertiesCount * sizeof(int));
	_requiredPropertiesNames = (char**)malloc(_requiredPropertiesCount * sizeof(char*));

	// Add all the available properties.
	for(i = 0; i < _propertiesCount; i++) {
		*(_requiredProperties + i) = i;
		*(_requiredPropertiesNames + i) = _strings + (_properties + i)->stringOffset;
	}
}

// Initialises the memory using the file provided and a string of properties.
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyString(char* fileName, char* properties) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;
    status = readFile(fileName);
	if (status == DATA_SET_INIT_STATUS_SUCCESS) {
		// If no properties are provided then use all of them.
		if (properties == NULL || strlen(properties) == 0)
			initAllProperties();
		else
			initSpecificProperties(properties);
	}
	return status;
}

// Initialises the memory using the file provided.
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyArray(char* fileName, char** properties, int propertyCount) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;
    status = readFile(fileName);
    if (status != DATA_SET_INIT_STATUS_SUCCESS) {
        return status;
    }
    initSpecificPropertiesFromArray(properties, propertyCount);

	return status;
}


// Returns the index of the property requested, or -1 if not available.
int fiftyoneDegreesGetPropertyIndex(char *value) {
	int32_t i;
	for(i = 0; i < _propertiesCount; i++) {
		if(strcmp(
			_strings + (_properties + i)->stringOffset,
			value) == 0) {
			return i;
		}
	}
	return -1;
}

// Returns the index of the child of the current node based on
// the value of the current character being compared.
BYTE getChildIndex(char value, int32_t lookupListPosition) {
	LOOKUP_HEADER *lookup = (LOOKUP_HEADER*)(((BYTE*)_lookupList) + lookupListPosition);
	if (value < lookup->lowest ||
		value > lookup->highest)
		return BYTE_MAX;

	// Return the child index.
	return *(&lookup->start + value - lookup->lowest);
}

// Returns the size in bytes of the child offsets for the
// child type provided.
int getSizeOfOffsets(NODE_CHILDREN* children) {
    switch(children->offsetType)
    {
        case BITS16: return sizeof(uint16_t);
        case BITS32: return sizeof(uint32_t);
        default: return sizeof(int64_t);
    }
}

int32_t* getNextNode(NODE_CHILDREN* children, BYTE childIndex) {
    uint16_t *offset16;
    uint32_t *offset32;
    int64_t *offset64;

    // If there's only one child the next node will appear
    // immediately afterwards as there's no list of children.
    if (children->numberOfChildren == 1) {
        return (int32_t*)&children->offsetType;
    }

    // There is more than 1 child so work out the next pointer for this
    // node index.
    if (childIndex == 0) {
        return (int32_t*)(((BYTE*)&(children->childrenOffsets)) + (getSizeOfOffsets(children) * (children->numberOfChildren - 1)));
    } else {
        switch(children->offsetType) {
            case BITS16:
                offset16 = &(children->childrenOffsets.b16) + childIndex - 1;
                return (int32_t*)(((BYTE*)offset16) + *offset16);
            case BITS32:
                offset32 = &(children->childrenOffsets.b32) + childIndex - 1;
                return (int32_t*)(((BYTE*)offset32) + *offset32);
            default:
                offset64 = &(children->childrenOffsets.b64) + childIndex - 1;
                return (int32_t*)(((BYTE*)offset64) + *offset64);
        }
    }
}

// Declaration of main device index function.
int32_t getDeviceIndexForNode(char* userAgent, int32_t* node, int32_t parentDeviceIndex);

int32_t getDeviceIndexChildren(char* userAgent, BYTE childIndex, NODE_CHILDREN *children, int parentDeviceIndex) {
    return getDeviceIndexForNode(
        &userAgent[1],
        getNextNode(children, childIndex),
        parentDeviceIndex);
}

int32_t getDeviceIndexFullNode(char* userAgent, NODE_FULL* node) {
    BYTE childIndex = getChildIndex(*userAgent, node->lookupListOffset);

    // If the child index is invalid then return this device index.
    if (childIndex >= node->children.numberOfChildren)
		return node->deviceIndex;

	// Move to the next child.
    return getDeviceIndexChildren(userAgent, childIndex, &(node->children), node->deviceIndex);
}

int32_t getDeviceIndexNoDeviceNode(char* userAgent, NODE_NO_DEVICE_INDEX* node, int32_t parentDeviceIndex) {
    BYTE childIndex = getChildIndex(*userAgent, abs(node->lookupListOffset));

    // If the child index is invalid then return this device index.
    if (childIndex >= node->children.numberOfChildren)
		return parentDeviceIndex;

	// Move to the next child.
    return getDeviceIndexChildren(userAgent, childIndex, &(node->children), parentDeviceIndex);
}

// Gets the index of the device associated with the user agent pointer
// provided. The method moves right along the user agent by shifting
// the pointer to the user agent left.
int32_t getDeviceIndexForNode(char* userAgent, int32_t* node, int32_t parentDeviceIndex) {
    if (*node >= 0)
		return getDeviceIndexFullNode(userAgent, (NODE_FULL*)node);
	return getDeviceIndexNoDeviceNode(userAgent, (NODE_NO_DEVICE_INDEX*)node, parentDeviceIndex);
}

// Returns the index to a matching device based on the useragent provided.
int32_t getDeviceIndex(char* userAgent) {
	return getDeviceIndexForNode(userAgent, _rootNode, -1);
}

// Returns the offset in the properties list to the first value for the device.
int32_t fiftyoneDegreesGetDeviceOffset(char* userAgent) {
    return getDeviceIndex(userAgent) * _propertiesCount;
}

// Sets name to the start of the http header name and returns the length of the string.
int setNextHttpHeaderName(char* start, char** name) {
	int index = 0;
	char *current = start, *lastChar = start;
	while (*current != 0) {
		if (*current == ' ') {
			*name = lastChar;
			return (int)(current - lastChar);
		}
		if (*current == '\r' ||
			*current == '\n') {
			lastChar = current + 1;
		}
		current++;
	}
	return 0;
}

// Sets the value pointer to the start of the next HTTP header value and returns the length.
int setNextHttpHeaderValue(char* start, char** value) {
	int index = 0;
	char *current = start, *lastChar = start;
	*value = lastChar;
	while (*current != 0) {
		if (*current == '\r' ||
			*current == '\n') {
			*value = lastChar;
			break;
		}
		current++;
	}
	return (int)(current - lastChar);
}

// Returns the index of the unique header, or -1 if the header is not important.
int getUniqueHttpHeaderIndex(char* httpHeaderName, int length) {
	int uniqueHeaderIndex;
	for (uniqueHeaderIndex = 0; uniqueHeaderIndex < _uniqueHttpHeaderCount; uniqueHeaderIndex++) {
		if (strlen(_strings + _uniqueHttpHeaders[uniqueHeaderIndex]) == length &&
			memcmp(_strings + _uniqueHttpHeaders[uniqueHeaderIndex], httpHeaderName, length) == 0) {
			return uniqueHeaderIndex;
		}
	}
	return -1;
}

// Returns the offsets to a matching devices based on the http headers provided.
fiftyoneDegreesDeviceOffsets* fiftyoneDegreesGetDeviceOffsetsWithHeadersString(char *httpHeaders) {
	char *headerName, *headerValue;
	int headerNameLength, headerValueLength, uniqueHeaderIndex = 0;
	fiftyoneDegreesDeviceOffsets* offsets = (fiftyoneDegreesDeviceOffsets*)malloc(_uniqueHttpHeaderCount * sizeof(fiftyoneDegreesDeviceOffsets));
	offsets->size = 0;
	headerNameLength = setNextHttpHeaderName(httpHeaders, &headerName);
	while (headerNameLength > 0 &&
		   offsets->size < _uniqueHttpHeaderCount) {
		headerValueLength = setNextHttpHeaderValue(headerName + headerNameLength + 1, &headerValue);
		uniqueHeaderIndex = getUniqueHttpHeaderIndex(headerName, headerNameLength);
		if (uniqueHeaderIndex >= 0) {
			(&offsets->firstOffset + offsets->size)->httpHeaderOffset = *(_uniqueHttpHeaders + uniqueHeaderIndex);
			(&offsets->firstOffset + offsets->size)->deviceOffset = fiftyoneDegreesGetDeviceOffset(headerValue);
			offsets->size++;
		}
		headerNameLength = setNextHttpHeaderName(headerValue + headerValueLength, &headerName);
	}
	return offsets;
}

char* getValueFromDevice(int32_t* device, int32_t propertyIndex) {
	return _strings + *(device + propertyIndex);
}

// Takes the results of getDeviceOffset and getPropertyIndex to return a value.
char* fiftyoneDegreesGetValue(int deviceOffset, int propertyIndex) {
    return getValueFromDevice(_devices + deviceOffset, propertyIndex);
}

// Sets the http header string to the header name at the index provided.
int fiftyoneDegreesGetHttpHeaderName(int httpHeaderIndex, char* httpHeader, int size) {
	int length;
	if (httpHeaderIndex < _uniqueHttpHeaderCount) {
		length = (int)strlen(_strings + _uniqueHttpHeaders[httpHeaderIndex]);
		if (length <= size) {
			// Copy the string and return the length.
			strcpy(httpHeader, _strings + _uniqueHttpHeaders[httpHeaderIndex]);
			return length;
		}
		else {
			// The http header is not large enough. Return it's required length.
			// as a negative.
			return -length;
		}
	}
	// No property at this index so return 0.
	return 0;
}

// Sets the propertyname string to the property name at the index provided.
int fiftyoneDegreesGetRequiredPropertyName(int requiredPropertyIndex, char* propertyName, int size) {
	int length;
	if (requiredPropertyIndex < _requiredPropertiesCount) {
		length = (int)strlen(_requiredPropertiesNames[requiredPropertyIndex]);
		if (length <= size) {
			// Copy the string and return the length.
			strcpy(propertyName, _requiredPropertiesNames[requiredPropertyIndex]);
			return length;
		}
		else {
			// The property name is not large enough. Return it's required length.
			// as a negative.
			return -length;
		}
	}
	// No property at this index so return 0.
	return 0;
}

int setValueFromDeviceOffset(int32_t deviceOffset, int32_t propertyIndex, char* values, int size) {
	char *value = fiftyoneDegreesGetValue(deviceOffset, propertyIndex);
	int length = (int)strlen(value);
	if (length <= size) {
		strcpy(values, value);
		return length;
	}
	else {
		return -length;
	}
}

// Sets the values string to the property values for the device offests and index provided.
int fiftyoneDegreesGetValueFromOffsets(fiftyoneDegreesDeviceOffsets* deviceOffsets, int requiredPropertyIndex, char* values, int size) {
	int deviceHttpHeaderIndex, propertyHttpHeaderIndex;
	int32_t propertyHttpHeaderOffset;
	fiftyoneDegreesProperty *property;
	if (deviceOffsets->size == 1) {
		return setValueFromDeviceOffset(
			deviceOffsets->firstOffset.deviceOffset, 
			*(_requiredProperties + requiredPropertyIndex),
			values,
			size);
	}
	else {
		property = _properties + _requiredProperties[requiredPropertyIndex];
		for (propertyHttpHeaderIndex = 0; propertyHttpHeaderIndex < property->headerCount; propertyHttpHeaderIndex++) {
			propertyHttpHeaderOffset = *(_httpHeaders + property->headerFirstIndex + propertyHttpHeaderIndex);
			for (deviceHttpHeaderIndex = 0; deviceHttpHeaderIndex < deviceOffsets->size; deviceHttpHeaderIndex++) {
				if (propertyHttpHeaderOffset == (&deviceOffsets->firstOffset + deviceHttpHeaderIndex)->httpHeaderOffset) {
					return setValueFromDeviceOffset(
						(&deviceOffsets->firstOffset + deviceHttpHeaderIndex)->deviceOffset,
						_requiredProperties[requiredPropertyIndex],
						values,
						size);
				}
			}
		}
	}
	return 0;
}

// Returns how many properties have been loaded in the dataset.
int32_t fiftyoneDegreesGetRequiredPropertiesCount(void) {
  return _requiredPropertiesCount;
}

// Returns the names of the properties loaded in the dataset.
char ** fiftyoneDegreesGetRequiredPropertiesNames(void) {
  return _requiredPropertiesNames;
}

// Process device properties into a CSV string.
int fiftyoneDegreesProcessDeviceCSV(int32_t deviceOffset, char* result, int resultLength) {
	char* currentPos = result;
	char* endPos = result + resultLength;
	int32_t i;
	int32_t* device = _devices + deviceOffset;

	// If no properties return nothing.
	if (_requiredPropertiesCount == 0) {
        *currentPos = 0;
		return 0;
	}

	// Process each line of data using the relevant value separator. In this case, a pipe.
	for(i = 0; i < _requiredPropertiesCount; i++) {
		// Add the next property to the buffer.
		currentPos += snprintf(
			currentPos,
			(int)(endPos - currentPos),
			"%s,%s\n",
			*(_requiredPropertiesNames + i),
			getValueFromDevice(device, *(_requiredProperties + i)));

		// Check to see if buffer is filled in which case return -1.
		if (currentPos >= endPos)
			return -1;
	}

	// Return the length of the string buffer used.
	return (int)(currentPos - result);
}

// Process device properties into a JSON string.
int fiftyoneDegreesProcessDeviceJSON(int32_t deviceOffset, char* result, int resultLength) {
	const char* deviceValue;
	int32_t deviceValueLength, deviceValueIndex;
	char* currentPos = result;
	char* endPos = result + resultLength;
	int32_t i;
	int32_t* device = _devices + deviceOffset;

	// If no properties return empty JSON.
	if (_requiredPropertiesCount == 0) {
        currentPos += snprintf(currentPos, endPos - currentPos, "{ }");
		return (int)(currentPos - result);
	}

	currentPos += snprintf(currentPos, endPos - currentPos, "{\n");

	// Process each line of data using the relevant value separator. In this case, a pipe.
	for(i = 0; i < _requiredPropertiesCount; i++) {

		// Add the next property to the buffer.
		currentPos += snprintf(
			currentPos,
			(int)(endPos - currentPos),
			"\"%s\": \"",
			*(_requiredPropertiesNames + i));

			deviceValue = getValueFromDevice(device, *(_requiredProperties + i));
			deviceValueLength = (int32_t)strlen(deviceValue);
			for(deviceValueIndex = 0; deviceValueIndex < deviceValueLength; deviceValueIndex++) {
				if(deviceValue[deviceValueIndex] == 0){
					break;
				}
				else if(deviceValue[deviceValueIndex] == '"'){
					currentPos += snprintf(
						currentPos,
						(int)(endPos - currentPos),
						"\\");
				}
				currentPos += snprintf(
					currentPos,
					(int)(endPos - currentPos),
					"%c",
					deviceValue[deviceValueIndex]);
			}
			currentPos += snprintf(
				currentPos,
				(int)(endPos - currentPos),
				"\"");
		if(i + 1 != _requiredPropertiesCount) {
			currentPos += snprintf(currentPos, endPos - currentPos, ",\n");
		}
		// Check to see if buffer is filled in which case return -1.
		if (currentPos >= endPos)
			return -1;
	}
	currentPos += snprintf(currentPos, endPos - currentPos, "\n}");
	return (int)(currentPos - result);
}
