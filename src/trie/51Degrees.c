#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
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

/**
 * \cond
 * Frees the memory used by the dataset.
 * @param dataSet a pointer to the dataset to be freed.
 * \endcond
 */
void fiftyoneDegreesDataSetFree(fiftyoneDegreesDataSet *dataSet) {
	int index;
	if (dataSet->requiredPropertiesNames != NULL) {
		fiftyoneDegreesFree((void*)dataSet->requiredPropertiesNames);
		dataSet->requiredPropertiesNames = NULL;
	}
	if (dataSet->requiredProperties != NULL) {
		fiftyoneDegreesFree((void*)dataSet->requiredProperties);
		dataSet->requiredProperties = NULL;
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
	if (dataSet->fileName != NULL) {
		fiftyoneDegreesFree((void*)dataSet->fileName);
		dataSet->fileName = NULL;
	}
	if (dataSet->memoryToFree != NULL) {
		fiftyoneDegreesFree((void*)dataSet->memoryToFree);
		dataSet->memoryToFree = NULL;
	}
}

/**
 * \cond
 * Sets the data set file name by copying the file name string provided into
 * newly allocated memory in the data set.
 * @param dataSet a pointer to the dataset to set the file name in.
 * @param fileName the path to the data file the dataset was initialised from.
 * @returns fiftyoneDegreesDataSetInitStatus indicates whether or not the file
 * name was set correctly.
 * \endcond
 */
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

/**
 * \cond
 *  Returns the index of the property requested, or -1 if not available.
 * @param dataSet a dataset that is already initialised.
 * @param property to look for in the dataset.
 * @param length the length of the property string provided.
 * @retrurns int the index of the property requested, or -1 if not
 * available.
 * \endcond
 */
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

/**
 * \cond
 * Initialises the properties string provided.
 * @param dataSet to initialise the required properties for.
 * @param properties a comma separated properties string.
 * \endcond
 */
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

/**
* \cond
* Initialises the properties array provided.
* @param dataSet to initialise the required properties for.
* @param properties an array of property strings.
* @param count the number of properties in the array.
* \endcond
*/
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

/**
* \cond
* Initialises all available properties.
* @param dataSet to initialise the required properties for.
* \endcond
*/
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

/**
* \cond
* Reads the various entities from the provided continuous memory location into
* the provided dataset.
*
* For most entities within the dataset it is sufficient to set the address
* of the pointer to the first element to the corresponding place within the
* provided memory space. This allows to avoid most of the additional memory
* allocations used in the init from file methods as the space is already
* allocated and contains data in the right format.
*
* @param source pointer to continuous memory space containing decompressed
*		 51Degrees pattern data file.
* @param dataSet to be initialised with data from the provided pointer to
*		 continuous memory space.
* @param length number of bytes that the file occupies in memory.
*		 Also corresponds to the last byte within the continuous memory
*		 space.
* @return dataset initialisation status.
* \endcond
*/
static fiftyoneDegreesDataSetInitStatus readDataSetFromMemoryLocation(
	const void *source,
	fiftyoneDegreesDataSet *dataSet,
	long length) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;
	byte *current = (byte*)source;
	const byte *lastByte = (byte*)source + length;

	// Copy the dataset version.
	dataSet->version = (uint16_t*)current;
	status = advancePointer(&current, lastByte, sizeof(uint16_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	/* Check the version of the data file */
	if (*dataSet->version != 32) {
		return DATA_SET_INIT_STATUS_INCORRECT_VERSION;
	}

	// Read the copyright.
	dataSet->copyrightSize = (int32_t*)current;
	status = advancePointer(&current, lastByte, sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->copyright = (char*)current;
	status = advancePointer(&current, lastByte, *dataSet->copyrightSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	// Read the strings.
	dataSet->stringsSize = (int32_t*)current;
	status = advancePointer(&current, lastByte, sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->strings = (char*)current;
	status = advancePointer(&current, lastByte, *dataSet->stringsSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	// Read the HTTP headers.
	dataSet->httpHeadersSize = (int32_t*)current;
	status = advancePointer(&current, lastByte, sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->httpHeaders = (int32_t*)current;
	status = advancePointer(&current, lastByte, *dataSet->httpHeadersSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	// Read the properties.
	dataSet->propertiesSize = (int32_t*)current;
	status = advancePointer(&current, lastByte, sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->properties = (fiftyoneDegreesProperty*)current;
	status = advancePointer(&current, lastByte, *dataSet->propertiesSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->propertiesCount = *dataSet->propertiesSize / sizeof(fiftyoneDegreesProperty);

	// Read the devices.
	dataSet->devicesSize = (int32_t*)current;
	status = advancePointer(&current, lastByte, sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->devices = (int32_t*)current;
	status = advancePointer(&current, lastByte, *dataSet->devicesSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	// Read the lookup list.
	dataSet->lookupListSize = (int32_t*)current;
	status = advancePointer(&current, lastByte, sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->lookupList = (FIFTYONEDEGREES_LOOKUP_HEADER*)current;
	status = advancePointer(&current, lastByte, *dataSet->lookupListSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	// Read the nodes.
	dataSet->nodesSize = (int64_t*)current;
	status = advancePointer(&current, lastByte, sizeof(int64_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	dataSet->rootNode = (int32_t*)current;
	status = advancePointer(&current, lastByte, (long)*dataSet->nodesSize);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;


	/* Check that the current pointer equals the last byte */
	if (current != lastByte) {
		return DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS;
	}

	return DATA_SET_INIT_STATUS_SUCCESS;
}

/**
 * \cond
 * Initialises the unique HTTP headers in the dataset.
 * @param dataSet to initialise the headers in.
 * @returns fiftyoneDegreesDataSetInitStatus indicates whether the init
 * was successful.
 * \endcond
 */
fiftyoneDegreesDataSetInitStatus initUniqueHttpHeaders(fiftyoneDegreesDataSet *dataSet)
{
	int headerIndex, uniqueHeaderIndex;
	// Allocate more space than is necessary just in case.
	dataSet->uniqueHttpHeaders = (int32_t*)fiftyoneDegreesMalloc(*dataSet->httpHeadersSize);
	if (dataSet->uniqueHttpHeaders == NULL) {
		fiftyoneDegreesFree(dataSet->uniqueHttpHeaders);
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Set the unique HTTP header names;
	dataSet->uniqueHttpHeaderCount = 0;
	for (headerIndex = 0; headerIndex < (int)(*dataSet->httpHeadersSize / sizeof(int32_t)); headerIndex++) {
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

	status = initUniqueHttpHeaders(dataSet);
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
* @return fiftyoneDegreesDataSetInitStatus dataset initialisation status.
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
* Initialises the provider with the provided dataset.
* @param provider to initialise.
* @param dataSet to initialise the provider with.
* @returns fiftyoneDegreesDataSetInitStatus indicates whether the init
* was successful.
* \endcond
*/
fiftyoneDegreesDataSetInitStatus initProvider(
	fiftyoneDegreesProvider *provider,
	fiftyoneDegreesDataSet *dataSet) {
	fiftyoneDegreesActiveDataSet *active;

	// Create a new active wrapper for the provider.
	active = (fiftyoneDegreesActiveDataSet*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesActiveDataSet));
	if (active == NULL) {
		fiftyoneDegreesDataSetFree(dataSet);
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Set the number of offsets using the active dataset to zero.
	active->inUse = 0;

	// Set a link between the new active wrapper and the provider. Used to check if the
	// dataset can be freed when the last thread has finished using it.
	active->provider = provider;

	// Switch the active dataset for the provider to the newly created one.
	active->dataSet = dataSet;
	provider->active = active;

	return DATA_SET_INIT_STATUS_SUCCESS;
}

/**
 * \cond
 * Initialises the dataset using the file provided and a string of properties.
 * @param fileName the path to a 51Degrees data file.
 * @param dataSet pointer to a dataset which has been allocated with the
 * correct size.
 * @param properties a comma separated string containing the properties to be
 * initialised.
 * @returns fiftyoneDegreesDataSetInitStatus indicates whether or not the
 * dataset has been initialised correctly.
 * \endcond
 */
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyString(const char* fileName, fiftyoneDegreesDataSet *dataSet, const char* properties) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;
	status = initFromFile(dataSet, fileName);
	if (status == DATA_SET_INIT_STATUS_SUCCESS) {
		// If no properties are provided then use all of them.
		if (properties == NULL || strlen(properties) == 0)
			initAllProperties(dataSet);
		else
			initSpecificProperties(dataSet, properties);
	}
	return status;
}

/**
* \cond
* Initialises the dataset using the file provided and an array of properties.
* @param fileName the path to a 51Degrees data file.
* @param dataSet pointer to a dataset which has been allocated with the
* correct size.
* @param properties a string array containing the properties to be
* initialised.
* @param propertyCount the number of properties in the array
* @returns fiftyoneDegreesDataSetInitStatus indicates whether or not the
* dataset has been initialised correctly.
* \endcond
*/
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyArray(const char* fileName, fiftyoneDegreesDataSet *dataSet, const char** properties, int propertyCount) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;
	status = initFromFile(dataSet, fileName);
	if (status == DATA_SET_INIT_STATUS_SUCCESS) {
		initSpecificPropertiesFromArray(dataSet, properties, propertyCount);
	}
	return status;
}

/**
* \cond
* Initialises the provider using the file provided and a string of properties.
* @param fileName the path to a 51Degrees data file.
* @param provider pointer to a provider which has been allocated with the
* correct size.
* @param properties a comma separated string containing the properties to be
* initialised.
* @returns fiftyoneDegreesDataSetInitStatus indicates whether or not the
* provider has been initialised correctly.
* \endcond
*/
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitProviderWithPropertyString(const char* fileName, fiftyoneDegreesProvider* provider, const char* properties) {
	fiftyoneDegreesDataSetInitStatus status;
	fiftyoneDegreesDataSet *dataSet = (fiftyoneDegreesDataSet*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDataSet));
	if (dataSet == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	status = fiftyoneDegreesInitWithPropertyString(fileName, dataSet, properties);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesFree(dataSet);
		return status;
	}
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_CREATE(provider->lock);
#endif
	return initProvider(provider, dataSet);
}

/**
* \cond
* Initialises the provider using the file provided and an array of properties.
* @param fileName the path to a 51Degrees data file.
* @param provider pointer to a provider which has been allocated with the
* correct size.
* @param properties a string array containing the properties to be
* initialised.
* @param propertyCount the number of properties in the array.
* @returns fiftyoneDegreesDataSetInitStatus indicates whether or not the
* provider has been initialised correctly.
* \endcond
*/
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitProviderWithPropertyArray(const char* fileName, fiftyoneDegreesProvider *provider, const char ** properties, int propertyCount) {
	fiftyoneDegreesDataSetInitStatus status;
	fiftyoneDegreesDataSet *dataSet = (fiftyoneDegreesDataSet*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDataSet));
	if (dataSet == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	status = fiftyoneDegreesInitWithPropertyArray(fileName, dataSet, properties, propertyCount);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesFree(dataSet);
		return status;
	}
#ifndef FIFTYONEDEGREES_NO_THREADING
		FIFTYONEDEGREES_MUTEX_CREATE(provider->lock);
#endif
	return initProvider(provider, dataSet);
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
* If the new data file does not obtain one or more property(ies) that the old
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
* @return fiftyoneDegreesDataSetInitStatus dataset initialisation status.
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
		requiredPropertyLength = (int16_t)strlen(requiredPropertyName);

		// For each of the available properties in the new dataset:
		for (index = 0; index < newDataSet->propertiesCount; index++) {

			// Get name of the current property of the new data set.
			propertyName = newDataSet->strings + newDataSet->properties[index].stringOffset;

			// Compare the two properties byte values and lengths.
			if (requiredPropertyLength == (int16_t)strlen(propertyName) &&
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

/**
* \cond
* Destroys the data set releasing all memory available. Ensure all offsets
* pointing to this method are freed first.
* @param dataSet pointer to the data set being destroyed
* \endcond
*/
void fiftyoneDegreesActiveDataSetFree(fiftyoneDegreesActiveDataSet *active) {
	if (active->dataSet != NULL) {
		fiftyoneDegreesDataSetFree(active->dataSet);
		fiftyoneDegreesFree(active->dataSet);
	}
	fiftyoneDegreesFree(active);
}

/**
 * \cond
 * Reloads the provider with the new dataset provided. This is common to both
 * memory and file reloads.
 * @param provider to reload.
 * @param newDataSet to replace the providers current dataset.
 * @return fiftyoneDegreesDataSetInitStatus dataset initialisation status.
 */
fiftyoneDegreesDataSetInitStatus reloadCommon(fiftyoneDegreesProvider *provider, fiftyoneDegreesDataSet *newDataSet) {
	const fiftyoneDegreesActiveDataSet *oldActive;
	fiftyoneDegreesDataSetInitStatus status;

	// Maintain a reference to the current active wrapper in case it can be freed.
	oldActive = (const fiftyoneDegreesActiveDataSet*)provider->active;

	// Initialise the new dataset with the same properties as the old one.
	status = setPropertiesFromExistingDataset(oldActive->dataSet, newDataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesDataSetFree(newDataSet);
		return status;
	}

#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_LOCK(&provider->lock);
#endif
	// Initialise the new provider.
	status = initProvider(provider, newDataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesDataSetFree(newDataSet);
	}

	// If the old dataset is ready to be freed then do so.
	else if (oldActive->inUse == 0) {
		fiftyoneDegreesActiveDataSetFree((fiftyoneDegreesActiveDataSet*)oldActive);
	}
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_UNLOCK(&provider->lock);
#endif
	return DATA_SET_INIT_STATUS_SUCCESS;

}

/**
* \cond
* Creates a new dataset using the same configuration options
* as the current data set associated with the provider. The
* memory located at the source pointer is used to create the new data set.
* Important: The memory pointed to by source will NOT be freed by 51Degrees
* when the associated data set is freed. The caller is responsible for
* releasing the memory. If 51Degrees should release the memory then the
* caller should set the memoryToFree field of the data set associated with
* the returned provider to source. 51Degrees will then free this memory when
* the data set is freed.
* @param provider pointer to the provider whose data set should be reloaded
* @param source pointer to the dataset held in memory.
* @param length number of bytes that the file occupies in memory.
* @return fiftyoneDegreesDataSetInitStatus indicating the result of the reload
* 	   operation.
* \endcond
*/
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesProviderReloadFromMemory(fiftyoneDegreesProvider *provider, void *source, long length) {
	fiftyoneDegreesDataSetInitStatus status;
	fiftyoneDegreesDataSet *newDataSet = NULL;

	// Allocate memory for a new data set.
	newDataSet = (fiftyoneDegreesDataSet*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDataSet));
	if (newDataSet == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Initialise the new data set with the data pointed to by source.
	status = initFromMemory(newDataSet, source, length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesFree(newDataSet);
		return status;
	}

	// Set the full data set pointer to NULL to indicate that when this
	// new data set is release the memory shouldn't be freed by 51Degrees.
	newDataSet->memoryToFree = NULL;

	// Reload common components.
	status = reloadCommon(provider, newDataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesDataSetFree(newDataSet);
	}

	return status;

}

/**
* \cond
* Creates a new dataset using the same configuration options
* as the current data set associated with the provider. The data file
* which the provider was initialised with is used to create the new data set.
* @param provider pointer to the provider whose data set should be reloaded
* @return fiftyoneDegreesDataSetInitStatus indicating the result of the reload
* 	   operation.
* \endcond
*/
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
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesFree(newDataSet);
		return status;
	}

	// Reload common components.
	status = reloadCommon(provider, newDataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesDataSetFree(newDataSet);
	}

	return status;
}

/**
* \cond
* Releases all the resources used by the provider. The provider can not be
* used without being reinitialised after calling this method.
* @param provider pointer to the provider to be freed.
* \endcond
*/
void fiftyoneDegreesProviderFree(fiftyoneDegreesProvider* provider) {
	fiftyoneDegreesActiveDataSetFree((fiftyoneDegreesActiveDataSet*)provider->active);
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_CLOSE(provider->lock);
#endif
}

/**
* \cond
* Gets the number of separators in the char array
* @param input char array containing separated values
* @return number of separators
* \endcond
*/
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

/**
 * \cond
 * Get the size of the file provided.
 * @param fileName path to the file.
 * @returns size_t the size the file will need in memory, or -1 if the file
 * could not be opened.
 * \endcond
 */
size_t getSizeOfFile(const char* fileName) {

	size_t sizeOfFile;
	FILE *inputFilePtr;

	// Open the file and hold on to the pointer.
#ifndef _MSC_FULL_VER
	inputFilePtr = fopen(fileName, "rb");
	if (inputFilePtr == NULL) {
		return -1;
	}
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

	assert(sizeOfFile > 0);

	return sizeOfFile;
}

/**
* \cond
* Get the number of properties in the data set from the file without
* reading it all into memory. This function is used in the
* getProviderSizeWithPropertyString when the property string is empty.
* @param fileName path to a valid data file.
* @returns int the number of properties in the data file.
* \endcond
*/
int getPropertyCountFromFile(const char* fileName) {
	FILE *inputFilePtr;
	int size;

	// Open the file and hold on to the pointer.
#ifndef _MSC_FULL_VER
	inputFilePtr = fopen(fileName, "rb");
	if (inputFilePtr == NULL) {
		return -1;
	}
#else
	/* If using Microsoft use the fopen_s method to avoid warning */
	if (fopen_s(&inputFilePtr, fileName, "rb") != 0) {
		return -1;
	}
#endif

	// Skip over the version.
	fseek(inputFilePtr, sizeof(uint16_t), SEEK_SET);
	
	// Skip over the copyright.
	if (fread(&size, sizeof(int32_t), 1, inputFilePtr) != 1)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	fseek(inputFilePtr, size, SEEK_CUR);

	// Skip over the strings.
	if (fread(&size, sizeof(int32_t), 1, inputFilePtr) != 1)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	fseek(inputFilePtr, size, SEEK_CUR);

	// Skip over the HTTP headers.
	if (fread(&size, sizeof(int32_t), 1, inputFilePtr) != 1)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	fseek(inputFilePtr, size, SEEK_CUR);

	// Read the size of the properties.
	if (fread(&size, sizeof(int32_t), 1, inputFilePtr) != 1)
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;

	// Exteact the number of properties from the size.
	size = size / (int)sizeof(fiftyoneDegreesProperty);

	// Close the file pointer.
	fclose(inputFilePtr);

	// Return the number of properties in the data file.
	return size;
}

/**
 * \cond
 * Get the size the provider will need in memory when initialised with the
 * provided properties. Returns -1 if the file could not be accessed.
 * @param fileName path to a valid data file.
 * @param properties comma separated list of property strings.
 * @returns size_t the size in memory needed to initialise the dataset,
 * or -1 if the file could not be accessed.
 * \endcond
 */
size_t fiftyoneDegreesGetProviderSizeWithPropertyString(const char* fileName, const char* properties) {

	int requiredPropertyCount;
	size_t size;

	size = getSizeOfFile(fileName);

	if (size > 0) {

		// Add size of file name.
		size += SIZE_OF_FILE_NAME(fileName);

		// Add the size of the dataset.
		size += sizeof(fiftyoneDegreesProvider);
		size += sizeof(fiftyoneDegreesDataSet);
		size += sizeof(fiftyoneDegreesActiveDataSet);

		// Get property count.
		if (properties[0] == '\0') {
			requiredPropertyCount = getPropertyCountFromFile(fileName);
		}
		else {
			requiredPropertyCount = (getSeparatorCount(properties) + 1);
		}

		assert(requiredPropertyCount > 0);

		size += 2 * sizeof(void*) * requiredPropertyCount;

		// Add the unique HTTP headers.
		size += sizeof(int32_t) * 5;
		}

	return size;
}

/**
* \cond
* Get the size the dataset will need in memory when initialised with the
* provided properties. Returns -1 if the file could not be accessed.
* @param fileName path to a valid data file.
* @param propertyCount the number of properties to be initialised.
* @returns size_t the size in memory needed to initialise the dataset,
* or -1 if the file could not be accessed.
* \endcond
*/
size_t fiftyoneDegreesGetProviderSizeWithPropertyCount(const char* fileName, int propertyCount) {

	size_t size;

	size = getSizeOfFile(fileName);

	if (size > 0) {
		// Add size of file name.
		size += SIZE_OF_FILE_NAME(fileName);

		// Add the size of the dataset.
		size += sizeof(fiftyoneDegreesDataSet);
		size += sizeof(fiftyoneDegreesActiveDataSet);

		// Get property count.
		size += 2 * sizeof(void*) * propertyCount;

		// Add the unique HTTP headers.
		size += sizeof(int32_t) * 5;
	}
	return size;
}

/**
 * \cond
 * Returns the index of the property requested, or -1 if not available.
 * @param dataSet pointer to an initialised dataset.
 * @param value name of the property to find.
 * @returns int the index of the property requested, or -1 if not available.
 * \endcond
 */
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

/**
 * \cond
 * Returns the index of the child of the current node based on
 * the value of the current character being compared.
 * @param dataSet pointer to an initialised dataset.
 * @param value the character to be compared.
 * @param lookupListPosition the current position in the lookup list.
 * @returns byte the index of the child node.
 * \endcond
 */
byte getChildIndex(fiftyoneDegreesDataSet *dataSet, char value, int32_t lookupListPosition) {
	FIFTYONEDEGREES_LOOKUP_HEADER *lookup = (FIFTYONEDEGREES_LOOKUP_HEADER*)(((byte*)dataSet->lookupList) + lookupListPosition);
	if (value < lookup->lowest ||
		value > lookup->highest)
		return FIFTYONEDEGREES_BYTE_MAX;

	// Return the child index.
	return *(&lookup->start + value - lookup->lowest);
}

/**
 * \cond
 * Returns the size in bytes of the child offsets for the
 * child type provided.
 * @param children node children to get the offset size of.
 * @returns int the size in bytes of the child offsets.
 * \endcond
 */
int getSizeOfOffsets(FIFTYONEDEGREES_NODE_CHILDREN* children) {
	switch (children->offsetType)
	{
	case FIFTYONEDEGREES_BITS16: return sizeof(uint16_t);
	case FIFTYONEDEGREES_BITS32: return sizeof(uint32_t);
	default: return sizeof(int64_t);
	}
}
/**
 * \cond
 * Returns a pointer to the next node.
 * @param children the current node children.
 * @param childIndex the index of the current child node.
 * @returns int32_t* a pointer to the next node.
 * \endcond
 */
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

/**
 * \cond
 * Get the device index from the current nodes children.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent the User-Agent used for the match.
 * @param childIndex the child index to get the next node from.
 * @param children of the current node.
 * @param parentDeviceIndex the device index of the current node.
 * @returns int32_t the device index of the next node.
 * \endcond
 */
int32_t getDeviceIndexChildren(fiftyoneDegreesDataSet *dataSet, char** userAgent, byte childIndex, FIFTYONEDEGREES_NODE_CHILDREN *children, int parentDeviceIndex) {
	*userAgent = *userAgent + 1;
	return getDeviceIndexForNode(
		dataSet,
		userAgent,
		getNextNode(children, childIndex),
		parentDeviceIndex);
}

/**
 * \cond
 * Get the device index from a full node.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent the User-Agent used for the match.
 * @param node the current full node.
 * @returns int32_t the device index for the supplied node.
 * \endcond
 */
int32_t getDeviceIndexFullNode(fiftyoneDegreesDataSet *dataSet, char** userAgent, FIFTYONEDEGREES_NODE_FULL* node) {
	byte childIndex = getChildIndex(dataSet, **userAgent, node->lookupListOffset);

	// If the child index is invalid then return this device index.
	if (childIndex >= node->children.numberOfChildren)
		return node->deviceIndex;

	// Move to the next child.
	return getDeviceIndexChildren(dataSet, userAgent, childIndex, &(node->children), node->deviceIndex);
}

/**
 * \cond
 * Get device index for a node with no device index.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent the User-Agent used for the match.
 * @param node the node to get the device index for.
 * @param parentDeviceIndex the device index of the nodes parent.
 * @returns int32_t the device index for the current node.
 * \endcond
 */
int32_t getDeviceIndexNoDeviceNode(fiftyoneDegreesDataSet *dataSet, char** userAgent, FIFTYONEDEGREES_NODE_NO_DEVICE_INDEX* node, int32_t parentDeviceIndex) {
	byte childIndex = getChildIndex(dataSet, **userAgent, abs(node->lookupListOffset));

	// If the child index is invalid then return this device index.
	if (childIndex >= node->children.numberOfChildren)
		return parentDeviceIndex;

	// Move to the next child.
	return getDeviceIndexChildren(dataSet, userAgent, childIndex, &(node->children), parentDeviceIndex);
}

/**
 * \cond
 * Gets the index of the device associated with the User-Agent pointer
 * provided. The method moves right along the user agent by shifting
 * the pointer to the user agent left.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent the User-Agent used for the match.
 * @param node the current node.
 * @param parentDeviceIndex the device index of the parent node.
 * @returns int32_t the device index of the current node.
 * \endcond
 */
int32_t getDeviceIndexForNode(fiftyoneDegreesDataSet *dataSet, char** userAgent, int32_t* node, int32_t parentDeviceIndex) {
	if (*node >= 0)
		return getDeviceIndexFullNode(dataSet, userAgent, (FIFTYONEDEGREES_NODE_FULL*)node);
	return getDeviceIndexNoDeviceNode(dataSet, userAgent, (FIFTYONEDEGREES_NODE_NO_DEVICE_INDEX*)node, parentDeviceIndex);
}

/**
 * \cond
 * Returns the index to a matching device based on the User-Agent provided.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent the User-Agent to match with.
 * @returns int32_t the device index for the given User-Agent.
 * \endcond
 */
int32_t getDeviceIndex(fiftyoneDegreesDataSet *dataSet, const char* userAgent) {
	return getDeviceIndexForNode(dataSet, (char**)&userAgent, dataSet->rootNode, -1);
}

/**
 * \cond
 * Returns the number of characters which matched in the Trie.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent User-Agent to use.
 * @returns int the number of matching characters from the User-Agent.
 * \endcond
 */
int fiftyoneDegreesGetMatchedUserAgentLength(fiftyoneDegreesDataSet *dataSet, char *userAgent) {
	char *lastCharacter = userAgent;
	getDeviceIndexForNode(dataSet, &lastCharacter, dataSet->rootNode, -1);
	return (int)(lastCharacter - userAgent);
}

/**
 * \cond
 * Returns the offset in the properties list to the first value for the device.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent to match for.
 * @returns int the property offset for the matched device.
 * \endcond
 */
int fiftyoneDegreesGetDeviceOffset(fiftyoneDegreesDataSet *dataSet, const char* userAgent) {
	return getDeviceIndex(dataSet, userAgent) * dataSet->propertiesCount;
}

/**
 * \cond
 * Sets the offsets structure passed to the method for the User-Agent provided.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent to match for.
 * @param httpHeaderIndex of the User-Agent.
 * @param offset to set.
 * \endcond
 */
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

/**
 * \cond
 * Creates a new device offsets structure with memory allocated.
 * @param dataSet pointer to an initialised dataset.
 * @returns fiftyoneDegreesDeviceOffsets* newly created device offsets.
 * \endcond
 */
fiftyoneDegreesDeviceOffsets* fiftyoneDegreesCreateDeviceOffsets(fiftyoneDegreesDataSet *dataSet) {
	fiftyoneDegreesDeviceOffsets* offsets = (fiftyoneDegreesDeviceOffsets*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDeviceOffsets));
	offsets->size = 0;
	offsets->firstOffset = (fiftyoneDegreesDeviceOffset*)fiftyoneDegreesMalloc(dataSet->uniqueHttpHeaderCount * sizeof(fiftyoneDegreesDeviceOffset));
	offsets->active = NULL;
	return offsets;
}

/**
 * \cond
 * Resets the device offsets to the state they were in when they were created
 * with fiftyoneDegreesCreateDeviceOffsets.
 * @param offsets to reset.
 * \endcond
 */
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

/**
 * \cond
 * Frees the memory used by the offsets.
 * @param offsets to free.
 * \endcond
 */
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
* \cond
* Creates a new device offsets structure with memory allocated and
* increments the inUse counter in the provider so the dataset will
* not be freed until this is. A corresponding call to
* fiftyoneDegreesProviderFreeDeviceOffsets must be made when these
* offsets are finished with.
* @param provider pointer to an initialised provider.
* @returns fiftyoneDegreesDeviceOffsets* newly created device offsets.
* \endcond
*/
fiftyoneDegreesDeviceOffsets* fiftyoneDegreesProviderCreateDeviceOffsets(fiftyoneDegreesProvider *provider) {
	const fiftyoneDegreesActiveDataSet *active;
	fiftyoneDegreesDeviceOffsets *offsets;
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_LOCK(&provider->lock);
#endif
	// Create a link to the dataset which these offsets will be
	// created from.
	active = (fiftyoneDegreesActiveDataSet*)provider->active;

	// Increment the inUse counter so the dataset is not free'd
	// while these offsets are still in use.
	provider->active->inUse++;
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_UNLOCK(&provider->lock);
#endif
	// Create the offsets.
	offsets = fiftyoneDegreesCreateDeviceOffsets(active->dataSet);
	offsets->active = (fiftyoneDegreesActiveDataSet*)active;
	return offsets;
}

/**
* \cond
* Frees the memory used by the offsets created by
* fiftyoneDegreesProviderCreateDeviceOffsets and decrements the inUse counter
* for the associated dataset.
* @param offsets to free.
* \endcond
*/
void fiftyoneDegreesProviderFreeDeviceOffsets(fiftyoneDegreesDeviceOffsets *offsets) {
	fiftyoneDegreesProvider* provider= offsets->active->provider;
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_LOCK(&provider->lock);
#endif
	offsets->active->inUse--;
	// If the dataset the offsets are associated with is not the active
	// one and no other offsets are using it, then dispose of it.
	if (offsets->active != NULL &&
		provider->active != offsets->active &&
		offsets->active->inUse == 0) {
		fiftyoneDegreesActiveDataSetFree(offsets->active);
	}

#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_UNLOCK(&provider->lock);
#endif
	fiftyoneDegreesFreeDeviceOffsets(offsets);
}

/**
 * Sets name to the start of the HTTP header name and returns the length of
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
* are replaced with _. The HTTP header name must be the same length
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

/**
 * \cond
 * Returns the index of the unique header, or -1 if the header is not important.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaderName name of the header to get the index of.
 * @param length of the header name.
 * @returns int index of the unique header, or -1 if the header is not
 * important.
 * \endcond
 */
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

/**
 * \cond
 * Returns the offsets to a matching devices based on the HTTP headers provided.
 * @param dataSet pointer to an initialised dataset.
 * @param offsets to set.
 * @param httpHeaders to match for.
 * @param size of the headers string.
 * \endcond
 */
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

/**
 * \cond
 * Returns the offsets to a matching devices based on the HTTP headers provided.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaders to match for.
 * @param size of the HTTP headers string.
 * @returns fiftyoneDegreesDeviceOffsets* pointer to newly created device
 * offsets from the match.
 * \endcond
 */
fiftyoneDegreesDeviceOffsets* fiftyoneDegreesGetDeviceOffsetsWithHeadersString(fiftyoneDegreesDataSet *dataSet, char *httpHeaders, size_t size) {
	fiftyoneDegreesDeviceOffsets* offsets = fiftyoneDegreesCreateDeviceOffsets(dataSet);
	fiftyoneDegreesSetDeviceOffsetsWithHeadersString(dataSet, offsets, httpHeaders, size);
	return offsets;
}

/**
 * \cond
 * Get the value of a given property index from a device.
 * @param dataSet pointer to an initialised dataset.
 * @param device to get the value from.
 * @param propertyIndex of the requested property.
 * @returns char* the value of the requested property.
 * \endcond
 */
char* getValueFromDevice(fiftyoneDegreesDataSet *dataSet, int32_t* device, int32_t propertyIndex) {
	return dataSet->strings + *(device + propertyIndex);
}

/**
 * \cond
 * Takes the results of getDeviceOffset and getPropertyIndex to return a value.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffset to get the property from.
 * @param propertyIndex of the requested property.
 * @return const char* the value of the requested property
 * \endcond
 */
const char* fiftyoneDegreesGetValue(fiftyoneDegreesDataSet *dataSet, int deviceOffset, int propertyIndex) {
	return getValueFromDevice(dataSet, dataSet->devices + deviceOffset, propertyIndex);
}

/**
 * \cond
 * Returns the number of HTTP headers relevant to device detection.
 * @param dataSet pointer to an initialised dataset.
 * @returns int the number of relevant HTTP headers.
 * \endcond
 */
int fiftyoneDegreesGetHttpHeaderCount(fiftyoneDegreesDataSet *dataSet) {
	return dataSet->uniqueHttpHeaderCount;
}

/**
 * \cond
 * Returns a pointer to the HTTP header name at the index provided.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaderIndex index of the header to get.
 * @returns const char* the name of the HTTP header.
 * \endcond
 */
const char* fiftyoneDegreesGetHttpHeaderNamePointer(fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex) {
	return httpHeaderIndex >= 0 && httpHeaderIndex < dataSet->uniqueHttpHeaderCount ?
		dataSet->strings + dataSet->uniqueHttpHeaders[httpHeaderIndex] : NULL;
}

/**
 * \cond
 * Returns the HTTP header name offset at the index provided.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaderIndex index of the header to get.
 * @returns int the name offset of the HTTP header.
 * \endcond
 */
int fiftyoneDegreesGetHttpHeaderNameOffset(fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex) {
	return dataSet->uniqueHttpHeaders[httpHeaderIndex];
}

/**
 * \cond
 * Sets the HTTP header string to the header name at the index provided.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaderIndex index of the header to get.
 * @param httpHeader to set.
 * @param size allocated to httpHeader.
 * @returns int the length of the HTTP header, or the requred length as a
 * negative if size is not large enough.
 * \endcond
 */
int fiftyoneDegreesGetHttpHeaderName(fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex, char* httpHeader, int size) {
	int length = 0;
	if (httpHeaderIndex < dataSet->uniqueHttpHeaderCount) {
		length = (int)strlen(dataSet->strings + dataSet->uniqueHttpHeaders[httpHeaderIndex]);
		if (length <= size) {
			// Copy the string and return the length.
			strcpy(httpHeader, dataSet->strings + dataSet->uniqueHttpHeaders[httpHeaderIndex]);
		}
		else {
			// The HTTP header is not large enough. Return it's required length.
			// as a negative.
			length = -length;
		}
	}
	return length;
}

/**
 * \cond
 * Initialises the prefixed upper HTTP header names for use with Perl, Python
 * and PHP. These headers are in the form HTTP_XXXXXX_XXXX where User-Agent
 * would appear as HTTP_USER_AGENT. This method avoids needing to duplicate
 * the logic to format the header names in each API.
 * @param dataSet pointer to initialised dataset.
 * \endcond
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
 * \cond
 * Returns the name of the header in prefixed upper case form at the index
 * provided, or NULL if the index is not valid.
 * @param dataSet pointer to an initialised dataset
 * @param httpHeaderIndex index of the HTTP header name required
 * @returns name of the header, or NULL if index not valid
 * \endcond
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

/**
 * \cond
 * Sets the propertyName string to the property name at the index provided.
 * @param dataSet pointer to an initialised dataset.
 * @param requiredPropertIndex index in the dataset's requiredProperties array.
 * @param propertyName to set.
 * @param size allocated to propertyName.
 * @returns int the length of the property name , or the requred length as a
 * negative if size is not large enough.
 * \endcond
 */
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

/**
 * \cond
 * Sets the values string for the property index and device requested.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffset to get the property from.
 * @param propertyIndex of the requested property.
 * @param values to set.
 * @param size allocated to values.
 * @returns int the length of the HTTP header, or the requred length as a
 * negative if size is not large enough.
 * \endcond
 */
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

/**
 * \cond
 * Returns a pointer to the value for the property based on the device offsets
 * provided.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffsets to get the property from.
 * @param requiredPropertyIndex index in the dataset's requiredProperties array.
 * @returns const char* pointer to the value of the requested property, or NULL
 * if the property does not exist.
 * \endcond
 */
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

/**
 * \cond
 * Sets the values string to the property values for the device offsets and index provided.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffsets to get the property from.
 * @param requiredPropertyIndex index in the dataset's requiredProperties array.
 * @param values string to set.
 * @param size allocated to the values string.
 * @returns int the length of the values string, or the required length as a
 * negative if size is not large enough.
 * \endcond
 */
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

/**
 * \cond
 * Returns the number of properties that have been loaded in the dataset.
 * @param dataSet pointer to an initialised dataset,
 * @returns int32_t number of initialised properties in the dataset.
 * \endcond
 */
int32_t fiftyoneDegreesGetRequiredPropertiesCount(fiftyoneDegreesDataSet *dataSet) {
	return dataSet->requiredPropertiesCount;
}

/**
 * \cond
 * Returns the names of the properties loaded in the dataset.
 * @param dataSet pointer to an initialised dataset.
 * @retuens const char** pointer to the array of initialised properties.
 * \endcond
 */
const char ** fiftyoneDegreesGetRequiredPropertiesNames(fiftyoneDegreesDataSet *dataSet) {
	return dataSet->requiredPropertiesNames;
}

/**
 * \cond
 * Returns the index in the array of required properties for this name, or -1 if not found.
 * @param dataSet pointer to an initialised dataset.
 * @param propertyName name of the property to get.
 * @returns int index in the dataset's requiredProperties array, or -1 if not found.
 * \endcond
 */
int fiftyoneDegreesGetRequiredPropertyIndex(fiftyoneDegreesDataSet *dataSet, const char *propertyName) {
	int requiredPropertyIndex;
	for (requiredPropertyIndex = 0; requiredPropertyIndex < dataSet->requiredPropertiesCount; requiredPropertyIndex++) {
		if (strcmp(propertyName, dataSet->requiredPropertiesNames[requiredPropertyIndex]) == 0) {
			return requiredPropertyIndex;
		}
	}
	return -1;
}

/**
 * \cond
 * Process device properties into a CSV string for the device offsets provided.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffsets from a match to process.
 * @param result buffer to store the CSV in.
 * @param resultLength allocated to result buffer.
 * @returns int the length used in the buffer, 0 if there are no properties, or
 * -1 if the buffer is not long enough.
 * \endcond
 */
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

/**
 * \cond
 * Process device properties into a CSV string for the device offset provided.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffset from a match to process.
 * @param result buffer to store the CSV in.
 * @param resultLength allocated to result buffer.
 * @returns int the length used in the buffer, 0 if there are no properties, or
 * -1 if the buffer is not long enough.
 * \endcond
 */
int fiftyoneDegreesProcessDeviceCSV(fiftyoneDegreesDataSet *dataSet, int32_t deviceOffset, char* result, int resultLength) {
	fiftyoneDegreesDeviceOffsets deviceOffsets;
	fiftyoneDegreesDeviceOffset singleOffset;
	deviceOffsets.firstOffset = &singleOffset;
	singleOffset.deviceOffset = deviceOffset;
	deviceOffsets.size = 1;
	return fiftyoneDegreesProcessDeviceOffsetsCSV(dataSet, &deviceOffsets, result, resultLength);
}

/**
 * \cond
 * Escapes a range of characters in a JSON string value.
 * @param start the first character to be considered
 * @param next the character after the last one to be considered
 * @param max the last allocated pointer
 * @return int the number of characters that were escaped
 * \endcond
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

/**
 * \cond
 * Process device properties into a JSON string for the device offsets provided.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffsets from a match to process.
 * @param result buffer to store the JSON in.
 * @param resultLength allocated to result buffer.
 * @returns int the length used in the buffer, 0 if there are no properties, or
 * -1 if the buffer is not long enough.
 * \endcond
 */
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

/**
 * \cond
 * Process device properties into a JSON string for the device offset provided.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffset from a match to process.
 * @param result buffer to store the JSON in.
 * @param resultLength allocated to result buffer.
 * @returns int the length used in the buffer, 0 if there are no properties, or
 * -1 if the buffer is not long enough.
 * \endcond
 */
int fiftyoneDegreesProcessDeviceJSON(fiftyoneDegreesDataSet *dataSet, int32_t deviceOffset, char* result, int resultLength) {
	fiftyoneDegreesDeviceOffsets deviceOffsets;
	fiftyoneDegreesDeviceOffset singleOffset;
	deviceOffsets.firstOffset = &singleOffset;
	singleOffset.deviceOffset = deviceOffset;
	deviceOffsets.size = 1;
	return fiftyoneDegreesProcessDeviceOffsetsJSON(dataSet, &deviceOffsets, result, resultLength);
}
