#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "51Degrees.h"

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

/* Define the size in memory of the file name */
#define SIZE_OF_FILE_NAME(fileName) sizeof(char) * (strlen(fileName) + 1)

/**
 * Memory allocation functions.
 */
void *(FIFTYONEDEGREES_CALL_CONV *fiftyoneDegreesMalloc)(size_t __size) = malloc;
void (FIFTYONEDEGREES_CALL_CONV *fiftyoneDegreesFree)(void *__ptr) = free;

// Reads the strings from the file.
fiftyoneDegreesDataSetInitStatus readStrings(fiftyoneDegreesDataSet *dataSet, FILE *inputFilePtr) {
	if (fread(&dataSet->stringsSize, sizeof(int32_t), 1, inputFilePtr) != 1)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	dataSet->strings = (char*)fiftyoneDegreesMalloc(dataSet->stringsSize);
	if (dataSet->strings == NULL)
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(dataSet->strings, sizeof(byte), (size_t)dataSet->stringsSize, inputFilePtr) != (size_t)dataSet->stringsSize)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the HTTP headers from the file.
fiftyoneDegreesDataSetInitStatus readHttpHeaders(fiftyoneDegreesDataSet *dataSet, FILE *inputFilePtr) {
	int headerIndex, uniqueHeaderIndex;
	if (fread(&dataSet->httpHeadersSize, sizeof(int32_t), 1, inputFilePtr) != 1)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	dataSet->httpHeaders = (int32_t*)fiftyoneDegreesMalloc(dataSet->httpHeadersSize);
	dataSet->uniqueHttpHeaders = (int32_t*)fiftyoneDegreesMalloc(dataSet->httpHeadersSize);
	if (dataSet->httpHeaders == NULL || dataSet->uniqueHttpHeaders == NULL) {
		if (dataSet->httpHeaders != NULL) { fiftyoneDegreesFree(dataSet->httpHeaders); }
		if (dataSet->uniqueHttpHeaders != NULL) { fiftyoneDegreesFree(dataSet->uniqueHttpHeaders); }
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	if (fread(dataSet->httpHeaders, sizeof(byte), (size_t)dataSet->httpHeadersSize, inputFilePtr) != (size_t)dataSet->httpHeadersSize)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;

	// Set the unique HTTP header names;
	dataSet->uniqueHttpHeaderCount = 0;
	for (headerIndex = 0; headerIndex < (int)(dataSet->httpHeadersSize / sizeof(int32_t)); headerIndex++) {
		for (uniqueHeaderIndex = 0; uniqueHeaderIndex < dataSet->uniqueHttpHeaderCount; uniqueHeaderIndex++) {
			if (*(dataSet->uniqueHttpHeaders + uniqueHeaderIndex) == *(dataSet->httpHeaders + headerIndex)) {
				break;
			}
		}
		if (uniqueHeaderIndex == dataSet->uniqueHttpHeaderCount) {
			*(dataSet->uniqueHttpHeaders + dataSet->uniqueHttpHeaderCount) = *(dataSet->httpHeaders + headerIndex);
			dataSet->uniqueHttpHeaderCount++;
		}
	}

	return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the properties from the file.
fiftyoneDegreesDataSetInitStatus readProperties(fiftyoneDegreesDataSet *dataSet, FILE *inputFilePtr) {
	if (fread(&dataSet->propertiesSize, sizeof(int32_t), 1, inputFilePtr) != 1)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	dataSet->properties = (fiftyoneDegreesProperty*)fiftyoneDegreesMalloc(dataSet->propertiesSize);
	if (dataSet->properties == NULL)
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(dataSet->properties, sizeof(byte), (size_t)dataSet->propertiesSize, inputFilePtr) != (size_t)dataSet->propertiesSize)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	dataSet->propertiesCount = dataSet->propertiesSize / sizeof(fiftyoneDegreesProperty);
	return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the profiles from the file.
fiftyoneDegreesDataSetInitStatus readDevices(fiftyoneDegreesDataSet *dataSet, FILE *inputFilePtr) {
	if (fread(&dataSet->devicesSize, sizeof(int32_t), 1, inputFilePtr) != 1)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	dataSet->devices = (int32_t*)fiftyoneDegreesMalloc(dataSet->devicesSize);
	if (dataSet->devices == NULL)
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(dataSet->devices, sizeof(byte), (size_t)dataSet->devicesSize, inputFilePtr) != (size_t)dataSet->devicesSize)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the lookups from the input file provided.
fiftyoneDegreesDataSetInitStatus readLookupList(fiftyoneDegreesDataSet *dataSet, FILE *inputFilePtr) {
	if (fread(&dataSet->lookupListSize, sizeof(int32_t), 1, inputFilePtr) != 1)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	dataSet->lookupList = (FIFTYONEDEGREES_LOOKUP_HEADER*)fiftyoneDegreesMalloc(dataSet->lookupListSize);
	if (dataSet->lookupList == NULL)
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(dataSet->lookupList, sizeof(byte), dataSet->lookupListSize, inputFilePtr) != (size_t)dataSet->lookupListSize)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the nodes byte array into memory.
fiftyoneDegreesDataSetInitStatus readNodes(fiftyoneDegreesDataSet *dataSet, FILE *inputFilePtr) {
	if (fread(&dataSet->nodesSize, sizeof(int64_t), 1, inputFilePtr) != 1)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	dataSet->rootNode = (int32_t*)fiftyoneDegreesMalloc((size_t)dataSet->nodesSize);
	if (dataSet->rootNode == 0)
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if ((int64_t) dataSet->rootNode > 0) {
		if (fread(dataSet->rootNode, sizeof(byte), (size_t)dataSet->nodesSize, inputFilePtr) != (size_t)dataSet->nodesSize) {
			return DATA_SET_INIT_STATUS_CORRUPT_DATA;
		}
	}

	return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the copyright message into memory.
fiftyoneDegreesDataSetInitStatus readCopyright(fiftyoneDegreesDataSet *dataSet, FILE *inputFilePtr) {
	if (fread(&dataSet->copyrightSize, sizeof(int32_t), 1, inputFilePtr) != 1)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	dataSet->copyright = (char*)fiftyoneDegreesMalloc(dataSet->copyrightSize);
	if (dataSet->copyright == NULL)
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	if (fread(dataSet->copyright, sizeof(byte), (size_t)dataSet->copyrightSize, inputFilePtr) != (size_t)dataSet->copyrightSize)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	return DATA_SET_INIT_STATUS_SUCCESS;
}

// Fress the memory.
void fiftyoneDegreesDestroy(fiftyoneDegreesDataSet *dataSet) {
	int index;
	if (dataSet->copyright != NULL) {
		fiftyoneDegreesFree(dataSet->copyright);
		dataSet->copyright = NULL;
	}
	if (dataSet->requiredProperties != NULL) {
		fiftyoneDegreesFree(dataSet->requiredProperties);
		dataSet->requiredProperties = NULL;
	}
	if (dataSet->requiredPropertiesNames != NULL) {
		fiftyoneDegreesFree((void*)dataSet->requiredPropertiesNames);
		dataSet->requiredPropertiesNames = NULL;
	}
	if (dataSet->rootNode != NULL) {
		fiftyoneDegreesFree(dataSet->rootNode);
		dataSet->rootNode = NULL;
	}
	if (dataSet->lookupList != NULL) {
		fiftyoneDegreesFree(dataSet->lookupList);
		dataSet->lookupList = NULL;
	}
	if (dataSet->devices != NULL) {
		fiftyoneDegreesFree(dataSet->devices);
		dataSet->devices = NULL;
	}
	if (dataSet->properties != NULL) {
		fiftyoneDegreesFree(dataSet->properties);
		dataSet->properties = NULL;
	}
	if (dataSet->prefixedUpperHttpHeaders != NULL) {
		for (index = 0; index < dataSet->uniqueHttpHeaderCount; index++) {
			if (dataSet->prefixedUpperHttpHeaders[index] != NULL) {
				fiftyoneDegreesFree((void*)dataSet->prefixedUpperHttpHeaders[index]);
				dataSet->prefixedUpperHttpHeaders[index] = NULL;
			}
		}
		dataSet->prefixedUpperHttpHeaders = NULL;
	}
	if (dataSet->uniqueHttpHeaders != NULL) {
		fiftyoneDegreesFree(dataSet->uniqueHttpHeaders);
		dataSet->uniqueHttpHeaders = NULL;
	}
	if (dataSet->httpHeaders != NULL) {
		fiftyoneDegreesFree(dataSet->httpHeaders);
		dataSet->httpHeaders = NULL;
	}
	if (dataSet->strings != NULL) {
		fiftyoneDegreesFree(dataSet->strings);
		dataSet->strings = NULL;
	}
	if (dataSet->fileName != NULL) {
		fiftyoneDegreesFree((void*)dataSet->fileName);
		dataSet->fileName = NULL;
	}
}

// Reads the version value from the start of the file and returns
// 0 if the file is in a format that can be read by this code.
fiftyoneDegreesDataSetInitStatus readVersion(fiftyoneDegreesDataSet *dataSet, FILE *inputFilePtr) {
	if ((int) fread(&dataSet->version, sizeof(uint16_t), 1, inputFilePtr) != -1) {
		if (dataSet->version != 32)
			return DATA_SET_INIT_STATUS_INCORRECT_VERSION;
		return DATA_SET_INIT_STATUS_SUCCESS;
	}
	return DATA_SET_INIT_STATUS_CORRUPT_DATA;
}

// Sets the data set file name by copying the file name string provided into
// newly allocated memory in the data set.
static fiftyoneDegreesDataSetInitStatus setDataSetFileName(
	fiftyoneDegreesDataSet *dataSet,
	const char *fileName) {
	dataSet->fileName = (const char*)fiftyoneDegreesMalloc(SIZE_OF_FILE_NAME(fileName));
	if (dataSet->fileName == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	memcpy((char*)dataSet->fileName, (char*)fileName, strlen(fileName) + 1);
	return DATA_SET_INIT_STATUS_SUCCESS;
}

// Reads the input file into memory returning 1 if it
// was read unsuccessfully, otherwise 0.
fiftyoneDegreesDataSetInitStatus readFile(char* fileName, fiftyoneDegreesDataSet *dataSet) {
#define READMETHODS 8
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;
	FILE *inputFilePtr;
	int readMethod;
	fiftyoneDegreesDataSetInitStatus(*m[READMETHODS]) (fiftyoneDegreesDataSet *dataSet, FILE *inputFilePtr);
	m[0] = readVersion;
	m[1] = readCopyright;
	m[2] = readStrings;
	m[3] = readHttpHeaders;
	m[4] = readProperties;
	m[5] = readDevices;
	m[6] = readLookupList;
	m[7] = readNodes;

	// Open the file and hold on to the pointer.
#ifndef _MSC_FULL_VER
	inputFilePtr = fopen(fileName, "rb");
#else
	/* If using Microsoft use the fopen_s method to avoid warning */
	if (fopen_s(&inputFilePtr, fileName, "rb") != 0) {
		return DATA_SET_INIT_STATUS_FILE_NOT_FOUND;
	}
#endif

	// If the file didn't open return -1.
	if (inputFilePtr == NULL) {
		return DATA_SET_INIT_STATUS_FILE_NOT_FOUND;
	}

	// Set the file name for future reloads.
	status = setDataSetFileName(dataSet, fileName);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}

	// Read the various data segments if the version is
	// one we can read.
	for (readMethod = 0; readMethod < READMETHODS; readMethod++) {
		status = m[readMethod](dataSet, inputFilePtr);
		if (status != DATA_SET_INIT_STATUS_SUCCESS) {
			fiftyoneDegreesDestroy(dataSet);
			break;
		}
	}
	fclose(inputFilePtr);

	// Initialise prefixed HTTP headers so they are
	// freed correctly in debug mode.
	dataSet->prefixedUpperHttpHeaders = NULL;

	dataSet->memoryToFree = NULL;

	return status;
}

// Returns the index of the property requested, or -1 if not available.
int getPropertyIndexRange(fiftyoneDegreesDataSet *dataSet, const char *property, size_t length) {
	int32_t i = 0;
	for (i = 0; i < dataSet->propertiesCount; i++) {
		if (strncmp(
			dataSet->strings + (dataSet->properties + i)->stringOffset,
			property,
			length) == 0) {
			return i;
		}
	}
	return -1;
}

// Initialises the properties provided.
void initSpecificProperties(fiftyoneDegreesDataSet *dataSet, const char* properties) {
	char *start;
	const char *end;
	int propertyIndex, currentIndex = 0;

	// Count the number of valid properties.
	dataSet->requiredPropertiesCount = 0;
	start = (char*)properties;
	end = properties - 1;
	do {
		end++;
		if (*end == '|' || *end == ',' || *end == '\0') {
			// Check the property we've just reached is valid and
			// if it is then increase the count.
			if (getPropertyIndexRange(dataSet, start, (end - start)) > 0)
				dataSet->requiredPropertiesCount++;
			start = (char*)end + 1;
		}
	} while (*end != '\0');

	if (dataSet->requiredPropertiesCount > 0) {
		// Create enough memory for the properties.
		dataSet->requiredProperties = (uint32_t*)fiftyoneDegreesMalloc(dataSet->requiredPropertiesCount * sizeof(int));
		dataSet->requiredPropertiesNames = (const char**)fiftyoneDegreesMalloc(dataSet->requiredPropertiesCount * sizeof(const char*));

		start = (char*)properties;
		end = properties - 1;
		do {
			end++;
			if (*end == '|' || *end == ',' || *end == '\0') {
				// If this is a valid property add it to the list.
				propertyIndex = getPropertyIndexRange(dataSet, start, (end - start));
				if (propertyIndex >= 0) {
					dataSet->requiredProperties[currentIndex] = propertyIndex;
					dataSet->requiredPropertiesNames[currentIndex] = dataSet->strings + dataSet->properties[propertyIndex].stringOffset;
					currentIndex++;
				}
				start = (char*)end + 1;
			}

		} while (*end != '\0');
	}
}

// Initialises the properties provided.
void initSpecificPropertiesFromArray(fiftyoneDegreesDataSet *dataSet, const char** properties, int count) {
	int i;
	int propertyIndex, currentIndex = 0;
	const char *currentProperty;
	int currentLength = 0;

	// Count the number of valid properties.
	dataSet->requiredPropertiesCount = 0;
	for (i = 0; i < count; i++) {
		currentProperty = properties[i];
		currentLength = (int)strlen(currentProperty);
		if (getPropertyIndexRange(dataSet, currentProperty, currentLength) > 0)
			dataSet->requiredPropertiesCount++;
	}

	if (dataSet->requiredPropertiesCount > 0) {
		// Create enough memory for the properties.
		dataSet->requiredProperties = (uint32_t*)fiftyoneDegreesMalloc(dataSet->requiredPropertiesCount * sizeof(int));
		dataSet->requiredPropertiesNames = (const char**)fiftyoneDegreesMalloc(dataSet->requiredPropertiesCount * sizeof(const char*));

		// Initialise the requiredProperties array.
		for (i = 0; i < count; i++) {
			currentProperty = properties[i];
			currentLength = (int)strlen(currentProperty);
			// If this is a valid property add it to the list.
			propertyIndex = getPropertyIndexRange(dataSet, currentProperty, currentLength);
			if (propertyIndex > 0) {
				dataSet->requiredProperties[currentIndex] = propertyIndex;
				dataSet->requiredPropertiesNames[currentIndex] = dataSet->strings + dataSet->properties[propertyIndex].stringOffset;
				currentIndex++;
			}
		}
	}
}

// Initialises all the available properties.
void initAllProperties(fiftyoneDegreesDataSet *dataSet) {
	int32_t i;

	// Set to include all properties.
	dataSet->requiredPropertiesCount = dataSet->propertiesCount;

	if (dataSet->requiredPropertiesCount > 0) {
		// Create enough memory for the properties.
		dataSet->requiredProperties = (uint32_t*)fiftyoneDegreesMalloc(dataSet->requiredPropertiesCount * sizeof(int));
		dataSet->requiredPropertiesNames = (const char**)fiftyoneDegreesMalloc(dataSet->requiredPropertiesCount * sizeof(const char*));

		// Add all the available properties.
		for (i = 0; i < dataSet->propertiesCount; i++) {
			dataSet->requiredProperties[i] = i;
			dataSet->requiredPropertiesNames[i] = dataSet->strings + dataSet->properties[i].stringOffset;
		}
	}
}

// Initialises the memory using the file provided and a string of properties.
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyString(const char* fileName, fiftyoneDegreesDataSet *dataSet, const char* properties) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;
	status = readFile((char*)fileName, dataSet);
	if (status == DATA_SET_INIT_STATUS_SUCCESS) {
		// If no properties are provided then use all of them.
		if (properties == NULL || strlen(properties) == 0)
			initAllProperties(dataSet);
		else
			initSpecificProperties(dataSet, properties);
	}
	return status;
}

// Initialises the memory using the file provided.
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyArray(const char* fileName, fiftyoneDegreesDataSet *dataSet, const char** properties, int propertyCount) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;
	status = readFile((char*)fileName, dataSet);
	if (status == DATA_SET_INIT_STATUS_SUCCESS) {
		initSpecificPropertiesFromArray(dataSet, properties, propertyCount);
	}
	return status;
}

fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitProviderWithPropertyString(const char* fileName, fiftyoneDegreesProvider* provider, const char* properties) {
	fiftyoneDegreesDataSetInitStatus status;
	provider->active = (fiftyoneDegreesActiveDataSet*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesActiveDataSet));
	provider->active->dataSet = (fiftyoneDegreesDataSet*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDataSet));
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_CREATE(provider->lock);
#endif
	status = fiftyoneDegreesInitWithPropertyString(fileName, (fiftyoneDegreesDataSet*) provider->active->dataSet, properties);
	return status;
}

fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitProviderWithPropertyArray(const char* filename, fiftyoneDegreesProvider *provider, const char ** properties, int propertyCount) {
	fiftyoneDegreesDataSetInitStatus status;
	provider->active = (fiftyoneDegreesActiveDataSet*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesActiveDataSet));
	provider->active->dataSet = (fiftyoneDegreesDataSet*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDataSet));
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_CREATE(provider->lock);
#endif
	status = fiftyoneDegreesInitWithPropertyArray(filename, (fiftyoneDegreesDataSet*) provider->active->dataSet, properties, propertyCount);
	return status;
}

/**
* \cond
* Provides a safe way to advance a pointer by the specified amount of bytes.
* Designed for a continuous allocated memory space where pointer is advanced
* consecutively. Requires the maximum number of bytes in the continuous
* memory space to be provided to carry out of bounds check.
*
* If -1 is supplied as the maximum number of bytes in the continuous memory
* space the check is ignored.
*
* Prior to calling this function for the first time make sure the pointer is
* set to the first byte of the allocated continuous memory space.
*
* @param pointer is the pointer to the current byte. Gets incremented by the
number of bytes provided in advanceBy.
* @param lastByte pointer to the last valid byte in the memory space. A
corrupt memory response is return if this is exceeded.
* @param advanceBy number of bytes to advance the pointer by.
* @return fiftyoneDegreesDataSetInitStatus stating the result of the
*		   current advance operation. Codes other than
*		   DATA_SET_INIT_STATUS_SUCCESS indicate there is a problem.
* \endcond
*/
static fiftyoneDegreesDataSetInitStatus advancePointer(byte **pointer,
	const byte *lastByte,
	long advanceBy) {
	if (pointer == NULL || *pointer == NULL) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}
	*pointer += advanceBy;
	if (*pointer > lastByte) {
		return DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS;
	}
	return DATA_SET_INIT_STATUS_SUCCESS;
}

static fiftyoneDegreesDataSetInitStatus readDataSetFromMemoryLocation(
	const void *source,
	fiftyoneDegreesDataSet *dataSet,
	long length) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;
	byte *current = (byte*)source;
	const byte *lastByte = (byte*)source + length;

	// Copy the bytes that form the header from the start of the file to the
	// data set pointer provided.
	if (memcpy((void*)(&dataSet->version), current, sizeof(uint16_t)) != dataSet) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}
	status = advancePointer(&current, lastByte, sizeof(uint16_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	/* Check the version of the data file */
	if (dataSet->version != 32 ) {
		return DATA_SET_INIT_STATUS_INCORRECT_VERSION;
	}

	// Read the copyright.
	dataSet->copyrightSize = (int32_t)current;
	status = advancePointer(&current, lastByte, sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->copyright = (char*)current;
	status = advancePointer(&current, lastByte, dataSet->copyrightSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	// Read the strings.
	dataSet->stringsSize = (int32_t)current;
	status = advancePointer(&current, lastByte, sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->strings = (char*)current;
	status = advancePointer(&current, lastByte, dataSet->stringsSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	// Read the HTTP headers.
	dataSet->httpHeadersSize = (int32_t)current;
	status = advancePointer(&current, lastByte, sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->httpHeaders = (int32_t*)current;
	status = advancePointer(&current, lastByte, dataSet->httpHeadersSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	// Read the properties.
	dataSet->propertiesSize = (int32_t)current;
	status = advancePointer(&current, lastByte, sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->properties = (fiftyoneDegreesProperty*)current;
	status = advancePointer(&current, lastByte, dataSet->propertiesSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->propertiesCount = dataSet->propertiesSize / sizeof(fiftyoneDegreesProperty);

	// Read the devices.
	dataSet->devicesSize = (int32_t)current;
	status = advancePointer(&current, lastByte, sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->devices = (int32_t*)current;
	status = advancePointer(&current, lastByte, dataSet->devicesSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	// Read the lookup list.
	dataSet->lookupListSize = (int32_t)current;
	status = advancePointer(&current, lastByte, sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->lookupList = (FIFTYONEDEGREES_LOOKUP_HEADER*)current;
	status = advancePointer(&current, lastByte, dataSet->lookupListSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	// Read the nodes.
	dataSet->nodesSize = (int32_t)current;
	status = advancePointer(&current, lastByte, sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->rootNode = (int32_t*)current;
	status = advancePointer(&current, lastByte, (long)dataSet->nodesSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	
	/* Check that the current pointer equals the last byte */
	if (current != lastByte) {
		return DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS;
	}

	return DATA_SET_INIT_STATUS_SUCCESS;
}

/**
* \cond
* Initialises the provided dataset with data from the provided pointer to the
* continuous memory space containing decompressed 51Degreees pattern device
* data.
*
* Remember to free dataset if status is not success.
*
* @param dataSet to be initialised with data from the provided pointer to
*		  memory location.
* @param source pointer to continuous memory space containing decompressed
*		  51Degrees pattern data file. Not NULL.
* @param length number of bytes that the file occupies in memory.
* @return dataset initialisation status.
* \endcond
*/
static fiftyoneDegreesDataSetInitStatus initFromMemory(
	fiftyoneDegreesDataSet *dataSet,
	const void *source,
	long length) {
	fiftyoneDegreesDataSetInitStatus status;

	if (source == NULL) {
		return DATA_SET_INIT_STATUS_NULL_POINTER;
	}

	// Read the data set from the memory source.
	status = readDataSetFromMemoryLocation(source, dataSet, length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}

	// For memory resident data files there is no path to file on disk.
	dataSet->fileName = NULL;

	// Set the prefixed upper headers to NULL as they may not be
	// needed. If they are initialised later then the memory can
	// be freed when the data set is destroyed.
	dataSet->prefixedUpperHttpHeaders = NULL;

	return status;
}


/**
* \cond
* Initialises the provided dataset from the file path supplied. The memory
* required is allocated by the method and is also marked to be released when
* the data set is freed.
*
* @param fileName path to data file that should be used for initialisation.
* @param dataSet pointer to the dataset structure to be initialised.
* @return dataset initialisation status.
* \endcond
*/
static fiftyoneDegreesDataSetInitStatus initFromFile(
	fiftyoneDegreesDataSet *dataSet,
	const char *fileName) {
	FILE *inputFilePtr;
	long fileSize;
	fiftyoneDegreesDataSetInitStatus status;

	// Open the file and hold on to the pointer.
#ifndef _MSC_FULL_VER
	inputFilePtr = fopen(fileName, "rb");
#else
	/* If using Microsoft use the fopen_s method to avoid warning */
	if (fopen_s(&inputFilePtr, fileName, "rb") != 0) {
		return DATA_SET_INIT_STATUS_FILE_NOT_FOUND;
	}
#endif

	// If the file didn't open return not found.
	if (inputFilePtr == NULL) {
		return DATA_SET_INIT_STATUS_FILE_NOT_FOUND;
	}

	// Find the length of the file by moving to the end.
	if (fseek(inputFilePtr, 0, SEEK_END) != 0) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}
	fileSize = ftell(inputFilePtr);
	if (fileSize <= 0) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}

	// Read the file into memory in a single continuous memory space.
	if (fseek(inputFilePtr, 0, SEEK_SET) != 0) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}
	dataSet->memoryToFree = (byte*)fiftyoneDegreesMalloc(fileSize);
	if (dataSet->memoryToFree == NULL)  {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	if (fread((byte*)dataSet->memoryToFree, fileSize, 1, inputFilePtr) != 1) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}
	fclose(inputFilePtr);

	// Initialises the data set using the memory just allocated.
	status = initFromMemory(dataSet, dataSet->memoryToFree, fileSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesFree((void*)dataSet->memoryToFree);
		return status;
	}

	// Set the file name of the data set.
	return setDataSetFileName(dataSet, fileName);
}

/**
* \cond
* Provides a safe way of initialising the new dataset with properties from the
* old dataset. Safeguards against future changes to the data file and
* data file structure.
*
* The new dataset is created with exactly the same set of properties as found
* within the old dataset.
*
* If the new data file does not ontain one or more property(ies) that the old
* dataset was initialised with, then these properties will not be
* initialised in the new dataset.
*
* Similarly, properties that are present in the new data file but are not
* in the old data file will not be initialised.
*
* It is up to the caller to to verify that all of the required properties have
* been initialised.
*
* @param oldDataSet the dataset to retrieve required property names. Not NULL.
* @param newDataSet the dataset to initialise properties in. Not NULL.
* \endcond
*/
static fiftyoneDegreesDataSetInitStatus setPropertiesFromExistingDataset(
	const fiftyoneDegreesDataSet *oldDataSet,
	fiftyoneDegreesDataSet *newDataSet) {
	int32_t index, propertyIndex, count;
	int16_t requiredPropertyLength;
	const char *requiredPropertyName;
	const char *propertyName;
	const fiftyoneDegreesProperty *requiredProperty;

	count = oldDataSet->requiredPropertiesCount;
	newDataSet->requiredPropertiesCount = 0;
	newDataSet->requiredProperties =
		(uint32_t*)fiftyoneDegreesMalloc(count * sizeof(uint32_t*));
	if (newDataSet->requiredProperties == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	newDataSet->requiredPropertiesNames =
		(const char**)fiftyoneDegreesMalloc(count * sizeof(const char*));
	if (newDataSet->requiredPropertiesNames == NULL) {
		fiftyoneDegreesFree(newDataSet->requiredProperties);
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// For each property in the old dataset:
	for (propertyIndex = 0; propertyIndex < count; propertyIndex++) {

		// Count the number of valid properties.
		requiredProperty = oldDataSet->properties + oldDataSet->requiredProperties[propertyIndex];
		requiredPropertyName = oldDataSet->strings + requiredProperty->stringOffset;
		requiredPropertyLength = strlen(requiredPropertyName);

		// For each of the available properties in the new dataset:
		for (index = 0; index < newDataSet->propertiesCount; index++) {

			// Get name of the current property of the new data set.
			propertyName = newDataSet->strings + newDataSet->properties[index].stringOffset;

			// Compare the two properties byte values and lengths.
			if (requiredPropertyLength == strlen(propertyName) &&
				memcmp(requiredPropertyName, propertyName, requiredPropertyLength) == 0) {
				newDataSet->requiredProperties[newDataSet->requiredPropertiesCount] = index;
				newDataSet->requiredPropertiesNames[newDataSet->requiredPropertiesCount] = newDataSet->strings + newDataSet->properties[index].stringOffset;
				newDataSet->requiredPropertiesCount++;
				break;
			}
		}
	}

	return DATA_SET_INIT_STATUS_SUCCESS;
}

fiftyoneDegreesDataSetInitStatus reloadCommon(fiftyoneDegreesProvider *provider, fiftyoneDegreesDataSet *newDataSet) {
	fiftyoneDegreesDataSet *oldDataSet;
	fiftyoneDegreesDataSetInitStatus status;
	int headerIndex, uniqueHeaderIndex;

	oldDataSet = provider->active->dataSet;

	// Initialise the new dataset with the same properties as the old one.
	status = setPropertiesFromExistingDataset(oldDataSet, newDataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesDestroy(newDataSet);
		return status;
	}

	newDataSet->uniqueHttpHeaders = (int32_t*)fiftyoneDegreesMalloc(newDataSet->httpHeadersSize);
	if (newDataSet->uniqueHttpHeaders == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Set the unique HTTP header names;
	newDataSet->uniqueHttpHeaderCount = 0;
	for (headerIndex = 0; headerIndex < (int)(newDataSet->httpHeadersSize / sizeof(int32_t)); headerIndex++) {
		for (uniqueHeaderIndex = 0; uniqueHeaderIndex < newDataSet->uniqueHttpHeaderCount; uniqueHeaderIndex++) {
			if (*(newDataSet->uniqueHttpHeaders + uniqueHeaderIndex) == *(newDataSet->httpHeaders + headerIndex)) {
				break;
			}
		}
		if (uniqueHeaderIndex == newDataSet->uniqueHttpHeaderCount) {
			*(newDataSet->uniqueHttpHeaders + newDataSet->uniqueHttpHeaderCount) = *(newDataSet->httpHeaders + headerIndex);
			newDataSet->uniqueHttpHeaderCount++;
		}
	}


	// Swap data sets.
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_LOCK(&provider->lock);
#endif
	provider->active->dataSet = newDataSet;
	fiftyoneDegreesDestroy(oldDataSet);
	fiftyoneDegreesFree(oldDataSet);
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_UNLOCK(&provider->lock);
#endif

	return DATA_SET_INIT_STATUS_SUCCESS;

}

fiftyoneDegreesDataSetInitStatus fiftyoneDegreesProviderReloadFromMemory(fiftyoneDegreesProvider *provider, void *source, long length) {
	fiftyoneDegreesDataSetInitStatus status;
	fiftyoneDegreesDataSet *newDataSet = NULL;

	newDataSet = (fiftyoneDegreesDataSet*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDataSet));
	if (newDataSet == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	status = initFromMemory(newDataSet, source, length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesFree(newDataSet);
		return status;
	}

	// Reload common components.
	status = reloadCommon(provider, newDataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesDestroy(newDataSet);
	}

	return status;

}

fiftyoneDegreesDataSetInitStatus fiftyoneDegreesProviderReloadFromFile(fiftyoneDegreesProvider* provider) {
	fiftyoneDegreesDataSetInitStatus status;
	fiftyoneDegreesDataSet *newDataSet = NULL;

	// Allocate memory for a new data set.
	newDataSet = (fiftyoneDegreesDataSet*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDataSet));
	if (newDataSet == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Initialise the new data set with the properties of the current one.
	status = initFromFile(newDataSet, provider->active->dataSet->fileName);
	//status = fiftyoneDegreesInitWithPropertyArray(provider->active->dataSet->fileName, newDataSet, provider->active->dataSet->requiredPropertiesNames, provider->active->dataSet->requiredPropertiesCount);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesFree(newDataSet);
		return status;
	}

	// Reload common components.
	status = reloadCommon(provider, newDataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesDestroy(newDataSet);
	}

	return status;
}

void fiftyoneDegreesProviderFree(fiftyoneDegreesProvider* provider) {
	fiftyoneDegreesDestroy((fiftyoneDegreesDataSet*) provider->active->dataSet);
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_CLOSE(provider->lock);
#endif
}

static int getSeparatorCount(const char* input) {
	int index = 0, count = 0;
	if (input != NULL && *input != 0) {
		while (*(input + index) != 0) {
			if (*(input + index) == ',' ||
				*(input + index) == '|' ||
				*(input + index) == ' ' ||
				*(input + index) == '\t')
				count++;
			index++;
		}
		return count + 1;
	}
	return 0;
}

size_t getSizeOfFile(const char* fileName) {

	size_t sizeOfFile;
	FILE *inputFilePtr;

	// Open the file and hold on to the pointer.
#ifndef _MSC_FULL_VER
	inputFilePtr = fopen(fileName, "rb");
#else
	/* If using Microsoft use the fopen_s method to avoid warning */
	if (fopen_s(&inputFilePtr, fileName, "rb") != 0) {
		return -1;
	}
#endif
	fseek(inputFilePtr, 0, SEEK_END);

	// Add file size.
	sizeOfFile = ftell(inputFilePtr);
	fclose(inputFilePtr);

	return sizeOfFile;
}

size_t fiftyoneDegreesGetDataSetSizeWithPropertyString(const char* fileName, const char* properties) {

	int requiredPropertyCount;
	size_t size;

	size = getSizeOfFile(fileName);

	// Add size of file name.
	size += SIZE_OF_FILE_NAME(fileName);

	if (size > 0) {
		size += 9 * sizeof(void*);
		// Get property count.
		requiredPropertyCount = getSeparatorCount(properties);
		size += 2 * sizeof(void*) * requiredPropertyCount;
	}
	return size;
}

size_t fiftyoneDegreesGetDataSetSizeWithPropertyCount(const char* fileName, int propertyCount) {

	size_t size;

	size = getSizeOfFile(fileName);
	// Add size of file name.
	size += SIZE_OF_FILE_NAME(fileName);

	if (size > 0) {
		size += 9 * sizeof(void*);
		size += 2 * sizeof(void*) * propertyCount;
	}
	return size;
}
// Returns the index of the property requested, or -1 if not available.
int fiftyoneDegreesGetPropertyIndex(fiftyoneDegreesDataSet *dataSet, const char *value) {
	int32_t i;
	for (i = 0; i < dataSet->propertiesCount; i++) {
		if (strcmp(
			dataSet->strings + (dataSet->properties + i)->stringOffset,
			value) == 0) {
			return i;
		}
	}
	return -1;
}

// Returns the index of the child of the current node based on
// the value of the current character being compared.
byte getChildIndex(fiftyoneDegreesDataSet *dataSet, char value, int32_t lookupListPosition) {
	FIFTYONEDEGREES_LOOKUP_HEADER *lookup = (FIFTYONEDEGREES_LOOKUP_HEADER*)(((byte*)dataSet->lookupList) + lookupListPosition);
	if (value < lookup->lowest ||
		value > lookup->highest)
		return FIFTYONEDEGREES_BYTE_MAX;

	// Return the child index.
	return *(&lookup->start + value - lookup->lowest);
}

// Returns the size in bytes of the child offsets for the
// child type provided.
int getSizeOfOffsets(FIFTYONEDEGREES_NODE_CHILDREN* children) {
	switch (children->offsetType)
	{
	case FIFTYONEDEGREES_BITS16: return sizeof(uint16_t);
	case FIFTYONEDEGREES_BITS32: return sizeof(uint32_t);
	default: return sizeof(int64_t);
	}
}

int32_t* getNextNode(FIFTYONEDEGREES_NODE_CHILDREN* children, byte childIndex) {
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
		return (int32_t*)(((byte*)&(children->childrenOffsets)) + (getSizeOfOffsets(children) * (children->numberOfChildren - 1)));
	}
	else {
		switch (children->offsetType) {
		case FIFTYONEDEGREES_BITS16:
			offset16 = &(children->childrenOffsets.b16) + childIndex - 1;
			return (int32_t*)(((byte*)offset16) + *offset16);
		case FIFTYONEDEGREES_BITS32:
			offset32 = &(children->childrenOffsets.b32) + childIndex - 1;
			return (int32_t*)(((byte*)offset32) + *offset32);
		default:
			offset64 = &(children->childrenOffsets.b64) + childIndex - 1;
			return (int32_t*)(((byte*)offset64) + *offset64);
		}
	}
}

// Declaration of main device index function.
int32_t getDeviceIndexForNode(fiftyoneDegreesDataSet *dataSet, char** userAgent, int32_t* node, int32_t parentDeviceIndex);

int32_t getDeviceIndexChildren(fiftyoneDegreesDataSet *dataSet, char** userAgent, byte childIndex, FIFTYONEDEGREES_NODE_CHILDREN *children, int parentDeviceIndex) {
	*userAgent = *userAgent + 1;
	return getDeviceIndexForNode(
		dataSet,
		userAgent,
		getNextNode(children, childIndex),
		parentDeviceIndex);
}

int32_t getDeviceIndexFullNode(fiftyoneDegreesDataSet *dataSet, char** userAgent, FIFTYONEDEGREES_NODE_FULL* node) {
	byte childIndex = getChildIndex(dataSet, **userAgent, node->lookupListOffset);

	// If the child index is invalid then return this device index.
	if (childIndex >= node->children.numberOfChildren)
		return node->deviceIndex;

	// Move to the next child.
	return getDeviceIndexChildren(dataSet, userAgent, childIndex, &(node->children), node->deviceIndex);
}

int32_t getDeviceIndexNoDeviceNode(fiftyoneDegreesDataSet *dataSet, char** userAgent, FIFTYONEDEGREES_NODE_NO_DEVICE_INDEX* node, int32_t parentDeviceIndex) {
	byte childIndex = getChildIndex(dataSet, **userAgent, abs(node->lookupListOffset));

	// If the child index is invalid then return this device index.
	if (childIndex >= node->children.numberOfChildren)
		return parentDeviceIndex;

	// Move to the next child.
	return getDeviceIndexChildren(dataSet, userAgent, childIndex, &(node->children), parentDeviceIndex);
}

// Gets the index of the device associated with the user agent pointer
// provided. The method moves right along the user agent by shifting
// the pointer to the user agent left.
int32_t getDeviceIndexForNode(fiftyoneDegreesDataSet *dataSet, char** userAgent, int32_t* node, int32_t parentDeviceIndex) {
	if (*node >= 0)
		return getDeviceIndexFullNode(dataSet, userAgent, (FIFTYONEDEGREES_NODE_FULL*)node);
	return getDeviceIndexNoDeviceNode(dataSet, userAgent, (FIFTYONEDEGREES_NODE_NO_DEVICE_INDEX*)node, parentDeviceIndex);
}

// Returns the index to a matching device based on the useragent provided.
int32_t getDeviceIndex(fiftyoneDegreesDataSet *dataSet, const char* userAgent) {
	return getDeviceIndexForNode(dataSet, (char**)&userAgent, dataSet->rootNode, -1);
}

// Returns the number of characters which matched in the trie.
int fiftyoneDegreesGetMatchedUserAgentLength(fiftyoneDegreesDataSet *dataSet, char *userAgent) {
	char *lastCharacter = userAgent;
	getDeviceIndexForNode(dataSet, &lastCharacter, dataSet->rootNode, -1);
	return (int)(lastCharacter - userAgent);
}

// Returns the offset in the properties list to the first value for the device.
int fiftyoneDegreesGetDeviceOffset(fiftyoneDegreesDataSet *dataSet, const char* userAgent) {
	return getDeviceIndex(dataSet, userAgent) * dataSet->propertiesCount;
}

// Sets the offsets structure passed to the method for the useragent provided.
void fiftyoneDegreesSetDeviceOffset(fiftyoneDegreesDataSet *dataSet, const char* userAgent, int httpHeaderIndex, fiftyoneDegreesDeviceOffset *offset) {
	char *lastCharacter = (char*)userAgent;
	offset->httpHeaderOffset = dataSet->uniqueHttpHeaders[httpHeaderIndex];
	offset->deviceOffset = getDeviceIndexForNode(dataSet, &lastCharacter, dataSet->rootNode, -1) * dataSet->propertiesCount;
	offset->length = lastCharacter - userAgent;
	offset->userAgent = (char*)fiftyoneDegreesMalloc(offset->length + 1 * sizeof(char));
	memcpy((void*)offset->userAgent, userAgent, offset->length);
	((char*)offset->userAgent)[offset->length] = 0;
	offset->difference = (int)(strlen(userAgent) - offset->length);
}

// Creates a new device offsets structure with memory allocated.
fiftyoneDegreesDeviceOffsets* fiftyoneDegreesCreateDeviceOffsets(fiftyoneDegreesDataSet *dataSet) {
	fiftyoneDegreesDeviceOffsets* offsets = (fiftyoneDegreesDeviceOffsets*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDeviceOffsets));
	offsets->size = 0;
	offsets->firstOffset = (fiftyoneDegreesDeviceOffset*)fiftyoneDegreesMalloc(dataSet->uniqueHttpHeaderCount * sizeof(fiftyoneDegreesDeviceOffset));
	return offsets;
}

void fiftyoneDegreesResetDeviceOffsets(fiftyoneDegreesDeviceOffsets* offsets) {
	int offsetIndex;
	if (offsets != NULL) {
		if (offsets->firstOffset != NULL) {
			for (offsetIndex = 0; offsetIndex < offsets->size; offsetIndex++) {
				if ((offsets->firstOffset + offsetIndex)->userAgent != NULL) {
					fiftyoneDegreesFree((void*)(offsets->firstOffset + offsetIndex)->userAgent);
				}
			}
		}
	}
	offsets->size = 0;
}

// Frees the memory used by the offsets.
void fiftyoneDegreesFreeDeviceOffsets(fiftyoneDegreesDeviceOffsets* offsets) {
	int offsetIndex;
	if (offsets != NULL) {
		if (offsets->firstOffset != NULL) {
			for (offsetIndex = 0; offsetIndex < offsets->size; offsetIndex++) {
				if ((offsets->firstOffset + offsetIndex)->userAgent != NULL) {
					fiftyoneDegreesFree((void*)(offsets->firstOffset + offsetIndex)->userAgent);
				}
			}
			fiftyoneDegreesFree(offsets->firstOffset);
		}
		fiftyoneDegreesFree(offsets);
	}
}

/**
 * Sets name to the start of the http header name and returns the length of
 * the string. A space or colon are used to identify the end of the header
 * name.
 * @param start of the string to be processed
 * @param end of the string to be processed
 * @param value to be set when returned
 * @returns the number of characters in the value
 */
int setNextHttpHeaderName(char* start, char* end, char** name) {
	char *current = start, *lastChar = start;
	while (current <= end) {
		if (*current == ' ' ||
			*current == ':') {
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

/**
 * Sets the value pointer to the start of the next HTTP header value and
 * returns the length.
 * @param start of the string to be processed
 * @param end of the string to be processed
 * @param value to be set when returned
 * @returns the number of characters in the value
 */
int setNextHttpHeaderValue(char* start, char *end, char** value) {
	char *lastChar = start, *current;

	// Move to the first non-space character.
	while (lastChar <= end && (
		*lastChar == ' ' ||
		*lastChar == ':')) {
		lastChar++;
	}

	// Set the value to the start character.
	*value = lastChar;
	current = lastChar;

	// Loop until end of line or end of string.
	while (current <= end) {
		if (*current == '\r' ||
			*current == '\n') {
			*value = lastChar;
			break;
		}
		current++;
	}
	return (int)(current - lastChar);
}


/**
* Compares two header strings for case insensitive equality and where -
* are replaced with _. The http header name must be the same length
* as the unique header.
* @param httpHeaderName string to be checked for equality
* @param uniqueHeader the unique HTTP header to be compared
* @param length of the strings
* @returns 0 if both strings are equal, otherwise the different between
*          the first mismatched characters
*/
int headerCompare(char *httpHeaderName, const char *uniqueHeader, int length) {
	int index, difference;
	for (index = 0; index < length; index++) {
		if (httpHeaderName[index] == '_') {
			difference = '-' - uniqueHeader[index];
		}
		else {
			difference = tolower(httpHeaderName[index]) - tolower(uniqueHeader[index]);
		}
		if (difference != 0) {
			return difference;
		}
	}
	return 0;
}

// Returns the index of the unique header, or -1 if the header is not important.
int fiftyoneDegreesGetUniqueHttpHeaderIndex(fiftyoneDegreesDataSet *dataSet, char* httpHeaderName, int length) {
	int uniqueHeaderIndex;
	static const char httpPrefix[] = "HTTP_";
	static const int httpPrefixLength = sizeof(httpPrefix) - 1;
	char *adjustedHttpHeaderName;

	// Check if header is from a Perl or PHP wrapper in the form of HTTP_*
	// and if present skip these characters.
	if (strncmp(httpHeaderName, httpPrefix, httpPrefixLength) == 0) {
		adjustedHttpHeaderName = httpHeaderName + httpPrefixLength;
		length -= httpPrefixLength;
	}
	else {
		adjustedHttpHeaderName = httpHeaderName;
	}

	for (uniqueHeaderIndex = 0; uniqueHeaderIndex < dataSet->uniqueHttpHeaderCount; uniqueHeaderIndex++) {
		if ((int) strlen(dataSet->strings + dataSet->uniqueHttpHeaders[uniqueHeaderIndex]) == length &&
			headerCompare(adjustedHttpHeaderName, dataSet->strings + dataSet->uniqueHttpHeaders[uniqueHeaderIndex], length) == 0) {
			return uniqueHeaderIndex;
		}
	}
	return -1;
}

// Returns the offsets to a matching devices based on the http headers provided.
void fiftyoneDegreesSetDeviceOffsetsWithHeadersString(fiftyoneDegreesDataSet *dataSet, fiftyoneDegreesDeviceOffsets *offsets, char *httpHeaders, size_t size) {
	char *headerName, *headerValue, *endOfHeaders = httpHeaders + size;
	int headerNameLength, headerValueLength, uniqueHeaderIndex = 0;
	offsets->size = 0;
	headerNameLength = setNextHttpHeaderName(httpHeaders, endOfHeaders, &headerName);
	while (headerNameLength > 0 &&
		offsets->size < dataSet->uniqueHttpHeaderCount) {
		headerValueLength = setNextHttpHeaderValue(headerName + headerNameLength, endOfHeaders, &headerValue);
		uniqueHeaderIndex = fiftyoneDegreesGetUniqueHttpHeaderIndex(dataSet, headerName, headerNameLength);
		if (uniqueHeaderIndex >= 0) {
			fiftyoneDegreesSetDeviceOffset(dataSet, headerValue, uniqueHeaderIndex, (offsets->firstOffset + offsets->size));
			offsets->size++;
		}
		headerNameLength = setNextHttpHeaderName(headerValue + headerValueLength, endOfHeaders, &headerName);
	}
	if (offsets->size == 0) {
		(offsets->firstOffset + offsets->size)->deviceOffset = 0;
		offsets->size++;
	}
}

// Returns the offsets to a matching devices based on the http headers provided.
fiftyoneDegreesDeviceOffsets* fiftyoneDegreesGetDeviceOffsetsWithHeadersString(fiftyoneDegreesDataSet *dataSet, char *httpHeaders, size_t size) {
	fiftyoneDegreesDeviceOffsets* offsets = fiftyoneDegreesCreateDeviceOffsets(dataSet);
	fiftyoneDegreesSetDeviceOffsetsWithHeadersString(dataSet, offsets, httpHeaders, size);
	return offsets;
}

char* getValueFromDevice(fiftyoneDegreesDataSet *dataSet, int32_t* device, int32_t propertyIndex) {
	return dataSet->strings + *(device + propertyIndex);
}

// Takes the results of getDeviceOffset and getPropertyIndex to return a value.
const char* fiftyoneDegreesGetValue(fiftyoneDegreesDataSet *dataSet, int deviceOffset, int propertyIndex) {
	return getValueFromDevice(dataSet, dataSet->devices + deviceOffset, propertyIndex);
}

// Returns the number of HTTP headers relevent to device detection.
int fiftyoneDegreesGetHttpHeaderCount(fiftyoneDegreesDataSet *dataSet) {
	return dataSet->uniqueHttpHeaderCount;
}

// Returns a pointer to the HTTP header name at the index provided.
const char* fiftyoneDegreesGetHttpHeaderNamePointer(fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex) {
	return httpHeaderIndex >= 0 && httpHeaderIndex < dataSet->uniqueHttpHeaderCount ?
		dataSet->strings + dataSet->uniqueHttpHeaders[httpHeaderIndex] : NULL;
}

// Returns the HTTP header name offset at the index provided.
int fiftyoneDegreesGetHttpHeaderNameOffset(fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex) {
	return dataSet->uniqueHttpHeaders[httpHeaderIndex];
}

// Sets the http header string to the header name at the index provided.
int fiftyoneDegreesGetHttpHeaderName(fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex, char* httpHeader, int size) {
	int length = 0;
	if (httpHeaderIndex < dataSet->uniqueHttpHeaderCount) {
		length = (int)strlen(dataSet->strings + dataSet->uniqueHttpHeaders[httpHeaderIndex]);
		if (length <= size) {
			// Copy the string and return the length.
			strcpy(httpHeader, dataSet->strings + dataSet->uniqueHttpHeaders[httpHeaderIndex]);
		}
		else {
			// The http header is not large enough. Return it's required length.
			// as a negative.
			length = -length;
		}
	}
	return length;
}

/**
 * Initialises the prefixed upper HTTP header names for use with Perl, Python
 * and PHP. These headers are in the form HTTP_XXXXXX_XXXX where User-Agent
 * would appear as HTTP_USER_AGENT. This method avoids needing to duplicate
 * the logic to format the header names in each API.
 */
static void initPrefixedUpperHttpHeaderNames(fiftyoneDegreesDataSet *dataSet) {
	int index, httpHeaderIndex;
	size_t length;
	char *prefixedUpperHttpHeader, *httpHeaderName;
	dataSet->prefixedUpperHttpHeaders = (const char**)fiftyoneDegreesMalloc(dataSet->uniqueHttpHeaderCount * sizeof(char*));
	if (dataSet->prefixedUpperHttpHeaders != NULL) {
		for (httpHeaderIndex = 0; httpHeaderIndex < dataSet->uniqueHttpHeaderCount; httpHeaderIndex++) {
			httpHeaderName = dataSet->strings + dataSet->uniqueHttpHeaders[httpHeaderIndex];
			length = strlen(httpHeaderName);
			prefixedUpperHttpHeader = (char*)fiftyoneDegreesMalloc(
				(length + sizeof(FIFTYONEDEGREES_HTTP_PREFIX_UPPER)) * sizeof(char));
			if (prefixedUpperHttpHeader != NULL) {
				dataSet->prefixedUpperHttpHeaders[httpHeaderIndex] = (const char*)prefixedUpperHttpHeader;
				memcpy((void*)prefixedUpperHttpHeader, FIFTYONEDEGREES_HTTP_PREFIX_UPPER, sizeof(FIFTYONEDEGREES_HTTP_PREFIX_UPPER) - 1);
				prefixedUpperHttpHeader += sizeof(FIFTYONEDEGREES_HTTP_PREFIX_UPPER) - 1;
				for (index = 0; index < (int) length; index++) {
					*prefixedUpperHttpHeader = toupper(*httpHeaderName);
					if (*prefixedUpperHttpHeader == '-') {
						*prefixedUpperHttpHeader = '_';
					}
					prefixedUpperHttpHeader++;
					httpHeaderName++;
				}
				*prefixedUpperHttpHeader = 0;
			}
		}
	}
}

/**
 * Returns the name of the header in prefixed upper case form at the index
 * provided, or NULL if the index is not valid.
 * @param dataSet pointer to an initialised dataset
 * @param httpHeaderIndex index of the HTTP header name required
 * @returns name of the header, or NULL if index not valid
 */
const char* fiftyoneDegreesGetPrefixedUpperHttpHeaderName(fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex) {
	const char *prefixedUpperHeaderName = NULL;
	if (dataSet->prefixedUpperHttpHeaders == NULL) {
		initPrefixedUpperHttpHeaderNames(dataSet);
	}
	if (httpHeaderIndex >= 0 &&
		httpHeaderIndex < dataSet->uniqueHttpHeaderCount) {
		prefixedUpperHeaderName = dataSet->prefixedUpperHttpHeaders[httpHeaderIndex];
	}
	return prefixedUpperHeaderName;
}

// Sets the propertyname string to the property name at the index provided.
int fiftyoneDegreesGetRequiredPropertyName(fiftyoneDegreesDataSet *dataSet, int requiredPropertyIndex, char* propertyName, int size) {
	int length = 0;
	if (requiredPropertyIndex < dataSet->requiredPropertiesCount) {
		length = (int)strlen(dataSet->requiredPropertiesNames[requiredPropertyIndex]);
		if (length <= size) {
			// Copy the string and return the length.
			strcpy(propertyName, dataSet->requiredPropertiesNames[requiredPropertyIndex]);
		}
		else {
			// The property name is not large enough. Return it's required length.
			// as a negative.
			length = -length;
		}
	}
	return length;
}

int setValueFromDeviceOffset(fiftyoneDegreesDataSet *dataSet, int32_t deviceOffset, int32_t propertyIndex, char* values, int size) {
	const char *value = fiftyoneDegreesGetValue(dataSet, deviceOffset, propertyIndex);
	int length = (int)strlen(value);
	if (length <= size) {
		strcpy(values, value);
	}
	else {
		length = -length;
	}
	return length;
}

// Returns a pointer to the value for the property based on the device offsets provided.
const char* fiftyoneDegreesGetValuePtrFromOffsets(fiftyoneDegreesDataSet *dataSet, fiftyoneDegreesDeviceOffsets* deviceOffsets, int requiredPropertyIndex) {
	int deviceHttpHeaderIndex, propertyHttpHeaderIndex;
	int32_t propertyHttpHeaderOffset;
	fiftyoneDegreesProperty *property;
	if (deviceOffsets->size == 1) {
		return fiftyoneDegreesGetValue(
			dataSet,
			deviceOffsets->firstOffset->deviceOffset,
			dataSet->requiredProperties[requiredPropertyIndex]);
	}
	else {
		property = dataSet->properties + dataSet->requiredProperties[requiredPropertyIndex];
		for (propertyHttpHeaderIndex = 0; propertyHttpHeaderIndex < property->headerCount; propertyHttpHeaderIndex++) {
			propertyHttpHeaderOffset = *(dataSet->httpHeaders + property->headerFirstIndex + propertyHttpHeaderIndex);
			for (deviceHttpHeaderIndex = 0; deviceHttpHeaderIndex < deviceOffsets->size; deviceHttpHeaderIndex++) {
				if (propertyHttpHeaderOffset == (deviceOffsets->firstOffset + deviceHttpHeaderIndex)->httpHeaderOffset) {
					return fiftyoneDegreesGetValue(dataSet,
						(deviceOffsets->firstOffset + deviceHttpHeaderIndex)->deviceOffset,
						dataSet->requiredProperties[requiredPropertyIndex]);
				}
			}
		}
	}
	return NULL;
}

// Sets the values string to the property values for the device offests and index provided.
int fiftyoneDegreesGetValueFromOffsets(fiftyoneDegreesDataSet *dataSet, fiftyoneDegreesDeviceOffsets* deviceOffsets, int requiredPropertyIndex, char* values, int size) {
	int deviceHttpHeaderIndex, propertyHttpHeaderIndex;
	int32_t propertyHttpHeaderOffset;
	fiftyoneDegreesProperty *property;
	if (deviceOffsets->size == 1) {
		return setValueFromDeviceOffset(dataSet,
			deviceOffsets->firstOffset->deviceOffset,
			*(dataSet->requiredProperties + requiredPropertyIndex),
			values,
			size);
	}
	else {
		property = dataSet->properties + dataSet->requiredProperties[requiredPropertyIndex];
		for (propertyHttpHeaderIndex = 0; propertyHttpHeaderIndex < property->headerCount; propertyHttpHeaderIndex++) {
			propertyHttpHeaderOffset = *(dataSet->httpHeaders + property->headerFirstIndex + propertyHttpHeaderIndex);
			for (deviceHttpHeaderIndex = 0; deviceHttpHeaderIndex < deviceOffsets->size; deviceHttpHeaderIndex++) {
				if (propertyHttpHeaderOffset == (deviceOffsets->firstOffset + deviceHttpHeaderIndex)->httpHeaderOffset) {
					return setValueFromDeviceOffset(dataSet,
						(deviceOffsets->firstOffset + deviceHttpHeaderIndex)->deviceOffset,
						dataSet->requiredProperties[requiredPropertyIndex],
						values,
						size);
				}
			}
		}
	}
	return 0;
}

// Returns how many properties have been loaded in the dataset.
int32_t fiftyoneDegreesGetRequiredPropertiesCount(fiftyoneDegreesDataSet *dataSet) {
	return dataSet->requiredPropertiesCount;
}

// Returns the names of the properties loaded in the dataset.
const char ** fiftyoneDegreesGetRequiredPropertiesNames(fiftyoneDegreesDataSet *dataSet) {
	return dataSet->requiredPropertiesNames;
}

// Returns the index in the array of required properties for this name, or -1 if not found.
int fiftyoneDegreesGetRequiredPropertyIndex(fiftyoneDegreesDataSet *dataSet, const char *propertyName) {
	int requiredPropertyIndex;
	for (requiredPropertyIndex = 0; requiredPropertyIndex < dataSet->requiredPropertiesCount; requiredPropertyIndex++) {
		if (strcmp(propertyName, dataSet->requiredPropertiesNames[requiredPropertyIndex]) == 0) {
			return requiredPropertyIndex;
		}
	}
	return -1;
}

// Process device properties into a CSV string for the device offsets provided.
int fiftyoneDegreesProcessDeviceOffsetsCSV(fiftyoneDegreesDataSet *dataSet, fiftyoneDegreesDeviceOffsets *deviceOffsets, char* result, int resultLength) {
	char* currentPos = result;
	char* endPos = result + resultLength;
	int32_t requiredPropertyIndex;

	// If no properties return nothing.
	if (dataSet->requiredPropertiesCount == 0) {
		*currentPos = 0;
		return 0;
	}

	// Process each line of data using the relevant value separator. In this case, a pipe.
	for (requiredPropertyIndex = 0; requiredPropertyIndex < dataSet->requiredPropertiesCount; requiredPropertyIndex++) {
		// Add the property name to the buffer.
		currentPos += snprintf(
			currentPos,
			(int)(endPos - currentPos),
			"%s,",
			*(dataSet->requiredPropertiesNames + requiredPropertyIndex));
		if (currentPos >= endPos) return -1;
		// Add the value(s) to the buffer.
		currentPos += abs(fiftyoneDegreesGetValueFromOffsets(
			dataSet,
			deviceOffsets,
			requiredPropertyIndex,
			currentPos,
			(int)(endPos - currentPos)));
		if (currentPos >= endPos) return -1;
		// Add a carriage return to terminate the line.
		currentPos += snprintf(
			currentPos,
			(int)(endPos - currentPos),
			"\n");
		if (currentPos >= endPos) return -1;
	}

	// Return the length of the string buffer used.
	return (int)(currentPos - result);
}

// Process device properties into a CSV string for the device offset provided.
int fiftyoneDegreesProcessDeviceCSV(fiftyoneDegreesDataSet *dataSet, int32_t deviceOffset, char* result, int resultLength) {
	fiftyoneDegreesDeviceOffsets deviceOffsets;
	fiftyoneDegreesDeviceOffset singleOffset;
	deviceOffsets.firstOffset = &singleOffset;
	singleOffset.deviceOffset = deviceOffset;
	deviceOffsets.size = 1;
	return fiftyoneDegreesProcessDeviceOffsetsCSV(dataSet, &deviceOffsets, result, resultLength);
}

/**
 * Escapes a range of characters in a JSON string value.
 * @param start the first character to be considered
 * @param next the character after the last one to be considered
 * @param max the last allocated pointer
 * @return the number of characters that were escaped
 */
static int escapeJSON(char *start, char *next, char *max) {
	static const char charactersToChange[] = "\\\"\r\n\t";
	char *current = next - 1;
	int changedCharacters = 0;
	int currentShift;
	int found = 0;

	// Count the number of characters to escape.
	while (current >= start) {
		if (strchr(charactersToChange, *current) != NULL) {
			changedCharacters++;
		}
		current--;
	}

	// Move characters to the right adding escape characters
	// when required.
	currentShift = changedCharacters;
	current = next + changedCharacters;
	if (current > max) {
		return -1;
	}
	while (currentShift > 0) {
		*current = *(current - currentShift);
		found = 0;
		if (*current == '\r') {
			*current = 'r';
			found = 1;
		}
		else if (*current == '\n') {
			*current = 'n';
			found = 1;
		}
		else if (*current == '\t') {
			*current = 't';
			found = 1;
		}
		else if (*current == '\\' || *current == '"') {
			found = 1;
		}
		if (found == 1) {
			current--;
			*current = '\\';
			currentShift--;
		}
		current--;
	}

	return changedCharacters;
}

// Process device properties into a JSON string for the device offsets provided.
int fiftyoneDegreesProcessDeviceOffsetsJSON(fiftyoneDegreesDataSet *dataSet, fiftyoneDegreesDeviceOffsets *deviceOffsets, char* result, int resultLength) {
	char* valuePos;
	int requiredPropertyIndex;
	char* currentPos = result;
	char* endPos = result + resultLength;

	// If no properties return empty JSON.
	if (dataSet->requiredPropertiesCount == 0) {
		currentPos += snprintf(currentPos, endPos - currentPos, "{ }");
		return (int)(currentPos - result);
	}

	currentPos += snprintf(currentPos, endPos - currentPos, "{");

	// Process each line of data using the relevant value separator. In this case, a pipe.
	for (requiredPropertyIndex = 0; requiredPropertyIndex < dataSet->requiredPropertiesCount; requiredPropertyIndex++) {
		// Add the next property to the buffer.
		currentPos += snprintf(
			currentPos,
			(int)(endPos - currentPos),
			"\"%s\": \"",
			*(dataSet->requiredPropertiesNames + requiredPropertyIndex));
		if (currentPos >= endPos) return -1;
		// Add the values to the buffer.
		valuePos = currentPos;
		currentPos += abs(fiftyoneDegreesGetValueFromOffsets(
			dataSet,
			deviceOffsets,
			requiredPropertyIndex,
			currentPos,
			(int)(endPos - currentPos)));
		if (currentPos >= endPos) return -1;
		currentPos += escapeJSON(valuePos, currentPos - 1, endPos);
		if (currentPos >= endPos) return -1;
		currentPos += snprintf(
			currentPos,
			(int)(endPos - currentPos),
			"\"");
		if (currentPos >= endPos) return -1;
		if (requiredPropertyIndex + 1 != dataSet->requiredPropertiesCount) {
			currentPos += snprintf(currentPos, endPos - currentPos, ",\n");
			if (currentPos >= endPos) return -1;
		}
		if (currentPos >= endPos) return -1;
	}
	currentPos += snprintf(currentPos, endPos - currentPos, "}");
	return (int)(currentPos - result);
}

// Process device properties into a JSON string for the device offset provided.
int fiftyoneDegreesProcessDeviceJSON(fiftyoneDegreesDataSet *dataSet, int32_t deviceOffset, char* result, int resultLength) {
	fiftyoneDegreesDeviceOffsets deviceOffsets;
	fiftyoneDegreesDeviceOffset singleOffset;
	deviceOffsets.firstOffset = &singleOffset;
	singleOffset.deviceOffset = deviceOffset;
	deviceOffsets.size = 1;
	return fiftyoneDegreesProcessDeviceOffsetsJSON(dataSet, &deviceOffsets, result, resultLength);
}
