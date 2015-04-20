#include <stdio.h>
#include <stdarg.h>
#include "../snprintf/snprintf.h"
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
 * PROBLEM MEHODS
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

// The size of the copyright notice at the top of the data file.
static int32_t _copyrightSize;

// Pointer to the copyright notice held in the data file.
static char* _copyright;

// The size of the strings data array.
static int32_t _stringsSize;

// Pointer to the start of the strings data array.
static char* _strings;

// The number of properties contained in the system.
static int32_t _propertiesCount;

// The size of the profiles data array.
static int32_t _propertiesSize;

// Pointer to the start of the pointers data array.
static int32_t* _properties;

// Pointer to the start of the devices data array.
static int32_t* _devices;

// The size of the deviecs data array.
static int32_t _devicesSize;

// The size of the memory reserved for lookup lists.
static int32_t _lookupListSize;

// Pointer to the start of the lookup lists.
static LOOKUP_HEADER* _lookupList;

// Offset in the device data file for the root node.
static int32_t* _rootNode;

// The size of the data array containing the nodes.
static int64_t _nodesSize;

// The number of properties to be returned.
static int _requiredPropertiesCount;

// A list of the required property indexes.
static uint32_t* _requiredProperties;

// A list of pointers to the names of the properties.
static char** _requiredPropertiesNames;

// Reads the strings from the file.
fiftyoneDegreesDataSetInitStatus readStrings(FILE *inputFilePtr) {
	if (fread(&_stringsSize, sizeof(int32_t), 1, inputFilePtr) != 1)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	_strings = (char*)malloc(_stringsSize);
	if (_strings == NULL)
        return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(_strings, sizeof(BYTE), _stringsSize, inputFilePtr) != _stringsSize)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
    return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the profiles from the file.
fiftyoneDegreesDataSetInitStatus readProperties(FILE *inputFilePtr) {
	if(fread(&_propertiesSize, sizeof(int32_t), 1, inputFilePtr) != 1)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	_properties = (int32_t*)malloc(_propertiesSize);
	if (_properties == NULL)
        return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(_properties, sizeof(BYTE), _propertiesSize, inputFilePtr) != _propertiesSize)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	_propertiesCount = _propertiesSize / sizeof(int32_t);
	return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the profiles from the file.
fiftyoneDegreesDataSetInitStatus readDevices(FILE *inputFilePtr) {
	if (fread(&_devicesSize, sizeof(int32_t), 1, inputFilePtr) != 1)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	_devices = (int32_t*)malloc(_devicesSize);
	if (_devices == NULL)
        return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(_devices, sizeof(BYTE), _devicesSize, inputFilePtr) != _devicesSize)
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
	if (fread(_lookupList, sizeof(BYTE), _lookupListSize, inputFilePtr) != _lookupListSize)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
    return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the nodes byte array into memory.
fiftyoneDegreesDataSetInitStatus readNodes(FILE *inputFilePtr) {
	if (fread(&_nodesSize, sizeof(int64_t), 1, inputFilePtr) != 1)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	_rootNode = (int32_t*)malloc(_nodesSize);
    if (_rootNode == 0)
        return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (_rootNode > 0) {
        if (fread(_rootNode, sizeof(BYTE), _nodesSize, inputFilePtr) != _nodesSize) {
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
	if (fread(_copyright, sizeof(BYTE), _copyrightSize, inputFilePtr) != _copyrightSize)
        return DATA_SET_INIT_STATUS_CORRUPT_DATA;
    return DATA_SET_INIT_STATUS_SUCCESS;
}

// Fress the memory.
void fiftyoneDegreesDestroy(void) {
	if (_requiredProperties > 0) free(_requiredProperties);
	if (_rootNode > 0) free(_rootNode);
	if (_lookupList > 0) free(_lookupList);
	if (_devices > 0) free(_devices);
	if (_properties > 0) free(_properties);
	if (_strings > 0) free(_strings);
}

// Reads the version value from the start of the file and returns
// 0 if the file is in a format that can be read by this code.
fiftyoneDegreesDataSetInitStatus readVersion(FILE *inputFilePtr) {
	uint16_t version;
	fread(&version, sizeof(uint16_t), 1, inputFilePtr);
	if (version != 3)
        return DATA_SET_INIT_STATUS_INCORRECT_VERSION;
    return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the input file into memory returning 1 if it
// was read unsuccessfully, otherwise 0.
fiftyoneDegreesDataSetInitStatus readFile(char* fileName) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;

	FILE *inputFilePtr;

	// Open the file and hold on to the pointer.
	inputFilePtr = fopen(fileName, "rb");

	// If the file didn't open return -1.
	if (inputFilePtr == NULL) {
        return DATA_SET_INIT_STATUS_FILE_NOT_FOUND;
	}
	// Read the various data segments if the version is
	// one we can read.
    status = readVersion(inputFilePtr);
    if (status != DATA_SET_INIT_STATUS_SUCCESS) {
        fclose(inputFilePtr);
        return status;
    }
	status = readCopyright(inputFilePtr);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
        fclose(inputFilePtr);
        return status;
	}
	status = readStrings(inputFilePtr);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
        fclose(inputFilePtr);
        return status;
	}
    status = readProperties(inputFilePtr);
    if (status != DATA_SET_INIT_STATUS_SUCCESS) {
        fclose(inputFilePtr);
        return status;
    }
    status = readDevices(inputFilePtr);
    if (status != DATA_SET_INIT_STATUS_SUCCESS) {
        fclose(inputFilePtr);
        return status;
    }
    status = readLookupList(inputFilePtr);
    if (status != DATA_SET_INIT_STATUS_SUCCESS) {

        fclose(inputFilePtr);
        return status;
    }
	status = readNodes(inputFilePtr);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
        fclose(inputFilePtr);
        return status;
    }
	fclose(inputFilePtr);

	return status;
}

// Returns the index of the property requested, or -1 if not available.
int getPropertyIndexRange(char *start, char *end) {
	uint32_t i = 0;
	for(i = 0; i < _propertiesCount; i++) {
		if(strncmp(
			_strings + *(_properties + i),
			start,
			end - start) == 0) {
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
			if (getPropertyIndexRange(start, end) > 0)
				_requiredPropertiesCount++;
			start = end + 1;
		}
	} while (*end != '\0');

	// Create enough memory for the properties.
	_requiredProperties = (uint32_t*)malloc(_requiredPropertiesCount * sizeof(int));
	_requiredPropertiesNames = (char**)malloc(_requiredPropertiesCount * sizeof(char**));

	// Initialise the requiredProperties array.
	start = properties;
	end = properties - 1;
	do {
		end++;
		if (*end == '|' || *end == ',' || *end == '\0') {
			// If this is a valid property add it to the list.
			propertyIndex = getPropertyIndexRange(start, end);
			if (propertyIndex > 0) {
				*(_requiredProperties + currentIndex) = propertyIndex;
				*(_requiredPropertiesNames + currentIndex) = _strings + *(_properties + propertyIndex);
				currentIndex++;
			}
			start = end + 1;
		}

	} while (*end != '\0');
}

// Initialises all the available properties.
void initAllProperties() {
	uint32_t i;

	// Set to include all properties.
	_requiredPropertiesCount = _propertiesCount;

	// Create enough memory for the properties.
	_requiredProperties = (uint32_t*)malloc(_requiredPropertiesCount * sizeof(int));
	_requiredPropertiesNames = (char**)malloc(_requiredPropertiesCount * sizeof(char**));

	// Add all the available properties.
	for(i = 0; i < _propertiesCount; i++) {
		*(_requiredProperties + i) = i;
		*(_requiredPropertiesNames + i) = _strings + *(_properties + i);
	}
}

// Initialises the memory using the file provided.
fiftyoneDegreesDataSetInitStatus init(char* fileName, char* properties) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;
    status = readFile(fileName);
    if (status != DATA_SET_INIT_STATUS_SUCCESS) {
        return status;
    }

	// If no properties are provided then use all of them.
	if (properties == NULL || strlen(properties) == 0)
		initAllProperties();
	else
		initSpecificProperties(properties);

	return status;
}

// Returns the index of the property requested, or -1 if not available.
int fiftyoneDegreesGetPropertyIndex(char *value) {
	uint32_t i = 0;
	for(i = 0; i < _propertiesCount; i++) {
		if(strcmp(
			_strings + *(_properties + i),
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
int32_t getDeviceOffset(char* userAgent) {
    return getDeviceIndex(userAgent) * _propertiesCount;
}

char* getValueFromDevice(int32_t* device, int32_t propertyIndex) {
	return _strings + *(device + propertyIndex);
}

// Takes the results of getDeviceOffset and getPropertyIndex to return a value.
char* getValue(int deviceOffset, int propertyIndex) {
    return getValueFromDevice(_devices + deviceOffset, propertyIndex);
}

// Process device properties into a CSV string.
int processDeviceCSV(int32_t deviceOffset, char* result, int resultLength) {
	char* currentPos = result;
	char* endPos = result + resultLength;
	uint32_t i;
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
int processDeviceJSON(int32_t deviceOffset, char* result, int resultLength) {
	const char* deviceValue;
	int32_t deviceValueLength, deviceValueIndex;
	char* currentPos = result;
	char* endPos = result + resultLength;
	uint32_t i;
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
			deviceValueLength = strlen(deviceValue);
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
