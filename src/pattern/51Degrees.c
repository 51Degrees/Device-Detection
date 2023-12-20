#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include "../cityhash/city.h"
#include "51Degrees.h"
#if !defined(DEBUG) && !defined(_DEBUG) && !defined(NDEBUG)
#define NDEBUG
#endif
#include <assert.h>
/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 *
 * This Source Code Form is the subject of the following patents and patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent No. 2871816;
 * European Patent Application No. 17184134.9;
 * United States Patent Nos. 9,332,086 and 9,350,823; and
 * United States Patent Application No. 15/686,066.
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
 * \cond
 * PROBLEM METHODS
 * \endcond
 */

/* Change snprintf to the Microsoft version */
#ifdef _MSC_VER
#define snprintf _snprintf
#define strdup _strdup
#endif

/* Define INT32_MAX if not defined */
#ifndef INT32_MAX
#define INT32_MAX 2147483647L
#endif

 /**
 * \cond
 * DATA STRUCTURES USED ONLY BY FUNCTIONS IN THIS FILE
 * \endcond
 */

/* Used for boolean results */
#define TRUE 1
#define FALSE 0

/* Ranges used when performing a numeric match */
const fiftyoneDegreesRange RANGES[] = {
		{ 0, 10 },
		{ 10, 100 },
		{ 100, 1000 },
		{ 1000, 10000 },
		{ 10000, SHRT_MAX }
};

#define RANGES_COUNT sizeof(RANGES) / sizeof(fiftyoneDegreesRange)

const int16_t POWERS[] = { 1, 10, 100, 1000, 10000 };

#define POWERS_COUNT sizeof(POWERS) / sizeof(int16_t)

#define MIN_CACHE_SIZE 2

#define HTTP_PREFIX_UPPER "HTTP_"

/**
 * Memory allocation functions.
 */
void *(FIFTYONEDEGREES_CALL_CONV *fiftyoneDegreesMalloc)(size_t __size) = malloc;
void *(FIFTYONEDEGREES_CALL_CONV *fiftyoneDegreesCalloc)(size_t __nmemb, size_t __size) = calloc;
void (FIFTYONEDEGREES_CALL_CONV *fiftyoneDegreesFree)(void *__ptr) = free;

/**
 * DATASET MEMORY ALLOCATION SIZE MACROS
 */
#define SIZE_OF_ROOT_NODES(h) h.rootNodes.count * sizeof(fiftyoneDegreesNode*)
#define SIZE_OF_REQUIRED_PROPERTIES_ARRAY(count) count * sizeof(char*)
#define SIZE_OF_FILE_NAME(fileName) sizeof(char) * (strlen(fileName) + 1)
#define SIZE_OF_COMPONENTS(h) h.components.count * sizeof(fiftyoneDegreesComponent*)
#define SIZE_OF_HTTP_HEADERS(count) count * sizeof(fiftyoneDegreesHttpHeader)
#define SIZE_OF_REQUIRED_PROPERTIES(count) count * sizeof(fiftyoneDegreesProperty*)
#define SIZE_OF_PROFILES_STRUCT_ARRAY(h) h.properties.count * sizeof(fiftyoneDegreesProfilesStructArray)
#define SIZE_OF_PROFILE_INDEXES_STRUCT(count) count * sizeof(fiftyoneDegreesProfileIndexesStruct)

/**
* WORKSET MEMORY ALLOCATION SIZE MACROS
*/
#define SIZE_OF_WORKSET_POOL sizeof(fiftyoneDegreesWorksetPool)
#define SIZE_OF_POOL_WORKSETS(size) size * sizeof(fiftyoneDegreesWorkset*)
#define SIZE_OF_WORKSET sizeof(fiftyoneDegreesWorkset)
#define SIZE_OF_WORKSET_INPUT(h) (h.maxUserAgentLength + 1) * sizeof(char)
#define SIZE_OF_WORKSET_SIGLIST_ITEMS(h) h.maxSignaturesClosest * sizeof(fiftyoneDegreesLinkedSignatureListItem)
#define SIZE_OF_WORKSET_NODES(h) h.maxUserAgentLength * sizeof(const fiftyoneDegreesNode*)
#define SIZE_OF_WORKSET_USED_NODES(h) h.maxUserAgentLength + 1
#define SIZE_OF_WORKSET_SIG_AS_STRING(h) (h.maxUserAgentLength + 1) * sizeof(char)
#define SIZE_OF_WORKSET_VALUES(h) h.maxValues * sizeof(const fiftyoneDegreesValue*)
#define SIZE_OF_WORKSET_IMPORTANT_HEADERS(count) count * sizeof(fiftyoneDegreesHttpHeaderWorkset)
#define SIZE_OF_WORKSET_PROFILES(h) h.components.count * sizeof(const fiftyoneDegreesProfile*)
#define SIZE_OF_WORKSET_TARGET_USERAGENT_ARRAY(h) (h.maxUserAgentLength + 1) * sizeof(byte)

/**
* CACHE MEMORY ALLOCATION SIZE MACROS
*/
#define SIZE_OF_CACHE sizeof(fiftyoneDegreesResultsetCache)
#define SIZE_OF_CACHE_RESULTSETS(c) c * (sizeof(fiftyoneDegreesResultset))
#define SIZE_OF_CACHE_TARGET_USER_AGENTS(c, h) c * SIZE_OF_WORKSET_TARGET_USERAGENT_ARRAY(h)
#define SIZE_OF_CACHE_PROFILES(c, h) c * SIZE_OF_WORKSET_PROFILES(h)

/**
* METRIC INDEX MACROS
*/
#define FIFTYONEDEGREES_METRIC_COUNT 4
#define FIFTYONEDEGREES_METRIC_METHOD_INDEX 0
#define FIFTYONEDEGREES_METRIC_RANK_INDEX 1
#define FIFTYONEDEGREES_METRIC_DIFFERENCE_INDEX 2
#define FIFTYONEDEGREES_METRIC_ID_INDEX 3

/**
* MAX PROPERTY VALUE LENGTH MEMORY ALLOCATION MACROS
*/
#define SIZE_OF_MAX_PROPERTY_VALUES(h) (h.properties.count + FIFTYONEDEGREES_METRIC_COUNT) * sizeof(int32_t)


 /**
 * \cond
 * DATA SET FILE AND MEMORY METHODS
 * \endcond
 */

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
 * Returns the total size of signatures in bytes.
 * @param dataSet with headers initialised.
 * @return the total size of signatures in bytes.
 * \endcond
 */
static int32_t getSizeOfSignature(fiftyoneDegreesDataSet *dataSet) {
	return (dataSet->header.signatureProfilesCount * sizeof(int32_t)) +
		sizeof(byte) +
		sizeof(int32_t) +
		sizeof(int32_t) +
		sizeof(byte);
}

/**
 * \cond
 * Returns the start offset for the signature structures.
 * @param dataSet with headers initialised.
 ( @return the start offset for the signature structures.
 * \endcond
 */
static int32_t getSignatureStartOfStruct(fiftyoneDegreesDataSet *dataSet) {
	return (dataSet->header.signatureProfilesCount * sizeof(int32_t));
}

 /**
 * \cond
 * Returns true if the header already exists in the list of headers.
 * @param dataSet that already contains headers
 * @param headerOffset to the string containing the name of the header
 * @return true if the header exists, otherwise false
 * \endcond
 */
static byte doesHeaderExist(fiftyoneDegreesDataSet *dataSet, int32_t headerOffset) {
	int index;
	for (index = 0; index < dataSet->httpHeadersCount; index++) {
		if (headerOffset == dataSet->httpHeaders[index].headerNameOffset) {
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * \cond
 * Returns the string offset for the HTTP header index of the component.
 * @param component pointer to the compoent whose header is needed
 * @param index of the header name needed
 * @return the offset in the strings structure to the header namer
 * \endcond
 */
static int32_t getComponentHeaderOffset(const fiftyoneDegreesComponent *component, int index) {
	int32_t *first = (int32_t*)((byte*)component + sizeof(fiftyoneDegreesComponent));
	return first[index];
}

/**
 * \cond
 * Reads the root nodes into the dataset from memory.
 *
 * @param current source pointer to continuous memory space containing decompressed
 *		  51Degrees pattern data file.
 * @param dataSet to be initialised with data from the provided pointer to
 *		  memory location.
 * return dataset initialisation status.
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus readRootNodesFromMemory(
	int32_t *rootNodeOffsets,
	fiftyoneDegreesDataSet *dataSet) {
	int32_t index;
	dataSet->rootNodes = (const fiftyoneDegreesNode**)
		fiftyoneDegreesMalloc(SIZE_OF_ROOT_NODES(dataSet->header));
	if (dataSet->rootNodes == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	for (index = 0; index < dataSet->header.rootNodes.count; index++) {
		dataSet->rootNodes[index] = (fiftyoneDegreesNode*)(dataSet->nodes + rootNodeOffsets[index]);
	}
	return DATA_SET_INIT_STATUS_SUCCESS;
}

/**
 * \cond
 * Creates a list of unique HTTP header components. This is necessary because
 * each component can have several HTTP headers associated with it that are
 * useful for the purposes of device detection.
 *
 * This function populates the list of unique HTTP headers that are important
 * for device detection. Function advances pointer to the current position.
 *
 * Function is shared between functions that initialise data file from memory
 * and function that reds in data from file.
 *
 * @param current modifiable pointer to the current position within the
 *		  continuous memory space containing decompressed 51Degrees pattern
 *		  data file.
 * @param dataSet to store the header list in.
 * @param currentPosition of the pointer in bytes.
 * @param maxPosition maximum allowed position of the pointer in bytes.
 *		 Corresponds to the size in bytes that the adat file loaded into
 *		 memory occupies.
 * @return dataset initialisation status.
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus readComponents(
	fiftyoneDegreesDataSet *dataSet) {
	byte *current = (byte*)dataSet->componentsData;
	int index, httpHeaderIndex, httpHeadersCount = 0;
	int32_t httpHeaderOffset;

	// Allocate the memory needed for the components array.
	dataSet->components = (const fiftyoneDegreesComponent**)fiftyoneDegreesMalloc(
		SIZE_OF_COMPONENTS(dataSet->header));
	if (dataSet->components == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Set pointers to each of the components in the array.
	for (index = 0; index < dataSet->header.components.count; index++) {
		dataSet->components[index] = (const fiftyoneDegreesComponent*)current;
		current += sizeof(fiftyoneDegreesComponent) + (size_t)(dataSet->components[index]->httpHeaderCount * sizeof(int32_t));
		httpHeadersCount += dataSet->components[index]->httpHeaderCount;
	}

	// Now create a list of unique HTTP headers which will be used to identify
	// which headers should be checked when an array of multiple headers is
	// passed for detection.
	dataSet->httpHeaders = (fiftyoneDegreesHttpHeader*)fiftyoneDegreesMalloc(SIZE_OF_HTTP_HEADERS(httpHeadersCount));
	if (dataSet->httpHeaders == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	dataSet->httpHeadersCount = 0;
	for (index = 0; index < dataSet->header.components.count; index++) {
		for (httpHeaderIndex = 0; httpHeaderIndex < dataSet->components[index]->httpHeaderCount; httpHeaderIndex++) {
			httpHeaderOffset = getComponentHeaderOffset(dataSet->components[index], httpHeaderIndex);
			if (doesHeaderExist(dataSet, httpHeaderOffset) == FALSE) {
				(dataSet->httpHeaders + dataSet->httpHeadersCount)->headerNameOffset = httpHeaderOffset;
				(dataSet->httpHeaders + dataSet->httpHeadersCount)->headerName =
					(char*)&(fiftyoneDegreesGetString(dataSet, httpHeaderOffset)->firstByte);
				dataSet->httpHeadersCount++;
			}
		}
	}

	return DATA_SET_INIT_STATUS_SUCCESS;
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
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus setPropertiesFromExistingDataset(
	const fiftyoneDegreesDataSet *oldDataSet,
	fiftyoneDegreesDataSet *newDataSet) {
	int32_t index, propertyIndex, count;
	int16_t requiredPropertyLength;
	const char *requiredPropertyName;
	const fiftyoneDegreesAsciiString *propertyName;
	const fiftyoneDegreesProperty *requiredProperty;

	count = oldDataSet->requiredPropertyCount;
	newDataSet->requiredPropertyCount = 0;
	newDataSet->requiredProperties =
		(const fiftyoneDegreesProperty**)fiftyoneDegreesMalloc(count * sizeof(const fiftyoneDegreesProperty*));
	if (newDataSet->requiredProperties == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// For each property in the old dataset:
	for (propertyIndex = 0; propertyIndex < count; propertyIndex++) {

		// Get current property, property length and property name.
		requiredProperty = oldDataSet->requiredProperties[propertyIndex];
		requiredPropertyName = fiftyoneDegreesGetPropertyName(oldDataSet, requiredProperty);
		requiredPropertyLength = (int16_t)strlen(requiredPropertyName);

		// For each of the available properties in the new dataset:
		for (index = 0; index < newDataSet->header.properties.count; index++) {

			// Get name of the current property of the new data set.
			propertyName =
				fiftyoneDegreesGetString(newDataSet, (newDataSet->properties + index)->nameOffset);

			// Compare the two properties byte values and lengths.
			if (requiredPropertyLength == propertyName->length - 1 &&
				memcmp(requiredPropertyName, &propertyName->firstByte, requiredPropertyLength) == 0) {
				*(newDataSet->requiredProperties + newDataSet->requiredPropertyCount) =
					(newDataSet->properties + index);
				newDataSet->requiredPropertyCount++;
				break;
			}
		}
	}

	return DATA_SET_INIT_STATUS_SUCCESS;
}

/**
 * \cond
 * Initialises the provider with the provided dataset using the given
 * cache and pool sizes.
 * @param provider to initialise.
 * @param dataSet to create cache and pool from.
 * @param poolSize of the new pool.
 * @param cacheSize of the new cache.
 * @returns fiftyoneDegreesDataSetInitStatus indicates whether the init
 * was successful.
 * \endcond
 */
fiftyoneDegreesDataSetInitStatus initProvider(
	fiftyoneDegreesProvider *provider,
	fiftyoneDegreesDataSet *dataSet,
	int poolSize,
	int cacheSize) {
	fiftyoneDegreesWorksetPool *newPool;
	fiftyoneDegreesResultsetCache *cache;

	// Create a new cache for the pool to use if a value was provided.
	if (cacheSize > 0) {
		cache = fiftyoneDegreesResultsetCacheCreate(dataSet, cacheSize);
		if (cache == NULL) {
			fiftyoneDegreesDataSetFree(dataSet);
			return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
		}
	}
	else {
		cache = NULL;
	}

	// Create a new active pool for the provider.
	newPool = (fiftyoneDegreesWorksetPool*)fiftyoneDegreesWorksetPoolCreate(
		dataSet, cache, poolSize);
	if (newPool == NULL) {
		fiftyoneDegreesResultsetCacheFree(cache);
		fiftyoneDegreesDataSetFree(dataSet);
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Set a link between the new pool and the provider. Used to check if the
	// pool can be freed when the last work set is handed back.
	newPool->provider = provider;

#ifndef FIFTYONEDEGREES_NO_THREADING
	// Replace the lock and signal pointers with the ones from the provider.
	newPool->lockPtr = &provider->lock;
	newPool->signalPtr = &provider->signal;
#endif

	// Switch the active pool for the provider to the newly created one.
	provider->activePool = newPool;

	return DATA_SET_INIT_STATUS_SUCCESS;
}

static fiftyoneDegreesDataSetInitStatus reloadCommon(
	fiftyoneDegreesProvider *provider,
	fiftyoneDegreesDataSet *newDataSet) {
	fiftyoneDegreesDataSetInitStatus status;

	// Maintain a reference to the current pool in case it can be freed.
	const fiftyoneDegreesWorksetPool *oldPool =
		(const fiftyoneDegreesWorksetPool*)provider->activePool;

	// Initialise the new dataset with the same properties as the old one.
	status = setPropertiesFromExistingDataset(oldPool->dataSet, newDataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesDataSetFree(newDataSet);
		return status;
	}

#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_LOCK(&provider->lock);
#endif

	// Initialise the new provider with a pool and cache.
	status = initProvider(provider, newDataSet,
		oldPool->size, oldPool->cache != NULL ? oldPool->cache->total : 0);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesDataSetFree(newDataSet);
	}

	// If the old pool is ready to be freed then do so.
	else if (oldPool->available == oldPool->size) {
		fiftyoneDegreesWorksetPoolCacheDataSetFree(
			(fiftyoneDegreesWorksetPool*)oldPool);
	}

#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_UNLOCK(&provider->lock);
#endif

	return status;
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

	// Copy the bytes that form the header from the start of the file to the
	// data set pointer provided.
	if (memcpy((void*)(&dataSet->header), current, sizeof(fiftyoneDegreesDataSetHeader)) != dataSet) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}

	status = advancePointer(&current, lastByte, sizeof(fiftyoneDegreesDataSetHeader));

	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	/* Check the version of the data file */
	if (dataSet->header.versionMajor != 3 ||
		dataSet->header.versionMinor != 2) {
		return DATA_SET_INIT_STATUS_INCORRECT_VERSION;
	}

	dataSet->strings = (const byte*)current;
	status = advancePointer(&current, lastByte, dataSet->header.strings.length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	dataSet->componentsData = (const byte*)current;
	status = readComponents(dataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	status = advancePointer(&current, lastByte, dataSet->header.components.length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	dataSet->maps = (const fiftyoneDegreesMap*)current;
	status = advancePointer(&current, lastByte, dataSet->header.maps.length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	dataSet->properties = (const fiftyoneDegreesProperty*)current;
	status = advancePointer(&current, lastByte, dataSet->header.properties.length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	
	dataSet->values = (const fiftyoneDegreesValue*)current;
	status = advancePointer(&current, lastByte, dataSet->header.values.length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	dataSet->profiles = (const byte*)current;
	status = advancePointer(&current, lastByte, dataSet->header.profiles.length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	dataSet->signatures = (const byte*)current;
	status = advancePointer(&current, lastByte, dataSet->header.signatures.length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	dataSet->signatureNodeOffsets = (const int32_t*)current;
	status = advancePointer(&current, lastByte, dataSet->header.signatureNodeOffsets.length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	dataSet->nodeRankedSignatureIndexes = (const int32_t*)current;
	status = advancePointer(&current, lastByte, dataSet->header.nodeRankedSignatureIndexes.length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	dataSet->rankedSignatureIndexes = (const int32_t*)current;
	status = advancePointer(&current, lastByte, dataSet->header.rankedSignatureIndexes.length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	dataSet->nodes = (const byte*)current;
	status = advancePointer(&current, lastByte, dataSet->header.nodes.length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	status = readRootNodesFromMemory((int32_t*)current, dataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;
	status = advancePointer(&current, lastByte, dataSet->header.rootNodes.length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	dataSet->profileOffsets = (const fiftyoneDegreesProfileOffset*)current;
	status = advancePointer(&current, lastByte, dataSet->header.profileOffsets.length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) return status;

	/* Check that the current pointer equals the last byte */
	if (current != lastByte) {
		return DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS;
	}

	/* Set some of the constant fields */
	dataSet->sizeOfSignature = getSizeOfSignature(dataSet);
	dataSet->signatureStartOfStruct = getSignatureStartOfStruct(dataSet);

	return DATA_SET_INIT_STATUS_SUCCESS;
}

/**
 * \cond
 * Initialises an array with the size equal to the number of properties, each
 * one containing a pointer to an empty array with the size equal to the
 * number of values for the corresponding property.
 * @param dataSet pointer to a 51Degrees data set.
 * @return dataSet initialisation status.
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus ensureValueProfilesSet(fiftyoneDegreesDataSet *dataSet) {
	fiftyoneDegreesProperty *property;
	int propertyIndex, valuesCount;

	// Allocate an array element for each property.
	dataSet->valuePointersArray =
		(fiftyoneDegreesProfilesStructArray*)fiftyoneDegreesMalloc(SIZE_OF_PROFILES_STRUCT_ARRAY(dataSet->header));
	if (dataSet->valuePointersArray == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	for (propertyIndex = 0; propertyIndex < dataSet->header.properties.count; propertyIndex++) {
		property = (fiftyoneDegreesProperty*)(dataSet->properties + (int32_t)propertyIndex);
		valuesCount = property->lastValueIndex - property->firstValueIndex + 1;

		// Set the initialised flag to 0;
		dataSet->valuePointersArray[propertyIndex].initialised = 0;

		// Allocate an array element for each value of the current property.
		dataSet->valuePointersArray[propertyIndex].profilesStructs =
			(fiftyoneDegreesProfileIndexesStruct*)fiftyoneDegreesMalloc(SIZE_OF_PROFILE_INDEXES_STRUCT(valuesCount));
		if (dataSet->valuePointersArray[propertyIndex].profilesStructs == NULL) {
			return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
		}

#ifndef FIFTYONEDEGREES_NO_THREADING
		FIFTYONEDEGREES_MUTEX_CREATE(dataSet->valuePointersArray[propertyIndex].lock);
#endif
	}

	return DATA_SET_INIT_STATUS_SUCCESS;
}

// Define these here as they are needed in the below function.
static fiftyoneDegreesProfile* getProfileByIndex(const fiftyoneDegreesDataSet *dataSet, int32_t index);
static int32_t getPropertyIndex(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesProperty *property);
static const fiftyoneDegreesProperty* getPropertyByName(const fiftyoneDegreesDataSet *dataSet, char *name);

/**
* \cond
* Finds the maximum string length of the values associated with the given
* property name.
* @param dataSet pointer to a fiftyoneDegreesDataSet.
* @param propertyName the name of the property to find the value length for.
* @return int32_t the maximum string length of the values associated with the
* given property or -1 if the property was not found.
* \endcond
*/
EXTERNAL int32_t fiftyoneDegreesGetMaxPropertyValueLength(const fiftyoneDegreesDataSet *dataSet, char *propertyName)
{
	const fiftyoneDegreesProperty *property;
	int32_t propertyIndex;

	// Check for any match metrics.
	if (strcmp(propertyName, "Method") == 0) {
		return dataSet->maxPropertyValueLength[dataSet->header.properties.count
			+ FIFTYONEDEGREES_METRIC_METHOD_INDEX];
	}
	if (strcmp(propertyName, "Rank") == 0) {
		return dataSet->maxPropertyValueLength[dataSet->header.properties.count
			+ FIFTYONEDEGREES_METRIC_RANK_INDEX];
	}
	if (strcmp(propertyName, "Difference") == 0) {
		return dataSet->maxPropertyValueLength[dataSet->header.properties.count
			+ FIFTYONEDEGREES_METRIC_DIFFERENCE_INDEX];
	}
	if (strcmp(propertyName, "DeviceId") == 0) {
		return dataSet->maxPropertyValueLength[dataSet->header.properties.count
			+ FIFTYONEDEGREES_METRIC_ID_INDEX];
	}

	// Property is not a match metric, so get the property.
	property = getPropertyByName(dataSet, propertyName);
	if (property == NULL) {
		return -1;
	}

	// The property exists, so get the index and return the max value length.
	propertyIndex = getPropertyIndex(dataSet, property);
	return dataSet->maxPropertyValueLength[propertyIndex];
}
/*
 * \cond
 * Go through all the properties in the dataset, calculate and set the maximum
 * length of value each one can return. These values are stored in the
 * dataSet->maxPropertyValueLength array with the coresponding property index.
 * @param dataSet pointer to 51Degrees dataset.
 * @return fiftyoneDegreesDataSetInitStatus indicating the result.
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus setMaxPropertyValueLength(fiftyoneDegreesDataSet *dataSet)
{
	int32_t profileIndex,
		*profileValueIndexes,
		valueIndex,
		propertyIndex,
		lengthNeeded;
	fiftyoneDegreesProfile *profile;
	const fiftyoneDegreesValue *value;
	const char *valueName;

	// Allocate the array. This is the number of properties, plus 4 for the match metrics.
	dataSet->maxPropertyValueLength =
		(int32_t*)fiftyoneDegreesMalloc(SIZE_OF_MAX_PROPERTY_VALUES(dataSet->header));

	if (dataSet->maxPropertyValueLength == NULL)
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;

	// Initialize the array by setting all values to zero.
	for (propertyIndex = 0; propertyIndex < dataSet->header.properties.count; propertyIndex++)
	{
		dataSet->maxPropertyValueLength[propertyIndex] = 0;
	}

	// Loop through all profiles in the data set.
	for (profileIndex = 0; profileIndex < dataSet->header.profiles.count; profileIndex++)
	{
		// Set the profile.
		profile = getProfileByIndex(dataSet, profileIndex);

		// Get the array of value indexes for the profile.
		profileValueIndexes = (int32_t*)((byte*)profile + sizeof(fiftyoneDegreesProfile));

		// Start with the first value.
		valueIndex = 0;
		value = dataSet->values + profileValueIndexes[valueIndex];

		// Loop over the rest.
		while (valueIndex < profile->valueCount)
		{
			// Get the property index which relates to the current value.
			propertyIndex = value->propertyIndex;

			// Reset the length.
			lengthNeeded = 0;

			// Multiple values can relate to one property, so keep adding the length
			// of values until we reach a value for a new property.
			while (value->propertyIndex == propertyIndex)
			{
				// Get the value string.
				valueName = fiftyoneDegreesGetValueName(dataSet, value);
				// Account for a delimiter if needed.
				if (lengthNeeded != 0)
					lengthNeeded++;
				// Add the length of the current value to the total for this
				// property
				lengthNeeded += (int32_t)strlen(valueName);

				// Get the next value.
				valueIndex++;
				value = dataSet->values + profileValueIndexes[valueIndex];

				// Check we haven't reached the end of the values for this profile.
				// (remember, we are inside a second while loop).
				if (valueIndex >= profile->valueCount)
					break;
			}

			// If the total length for this profile + property combination is
			// greater that the current max length, then update it.
			if (lengthNeeded > dataSet->maxPropertyValueLength[propertyIndex])
				dataSet->maxPropertyValueLength[propertyIndex] = lengthNeeded;
		}
	}

	// Now do the match metrics.
	// Fist the Method, this is hard coded as it is not likely to change.
	dataSet->maxPropertyValueLength[dataSet->header.properties.count
		+ FIFTYONEDEGREES_METRIC_METHOD_INDEX] = 7;

	// Get the maximum length of an integer.
	if (INT_MAX < 10000) lengthNeeded = 4;
	else if (INT_MAX < 100000) lengthNeeded = 5;
	else if (INT_MAX < 1000000) lengthNeeded = 6;
	else if (INT_MAX < 10000000) lengthNeeded = 7;
	else if (INT_MAX < 100000000) lengthNeeded = 8;
	else if (INT_MAX < 1000000000) lengthNeeded = 9;
	else if (INT_MAX < 10000000000) lengthNeeded = 10;
	else if (INT_MAX < 100000000000) lengthNeeded = 11;
	else if (INT_MAX < 1000000000000) lengthNeeded = 12;
	else if (INT_MAX < 10000000000000) lengthNeeded = 13;
	else lengthNeeded = 20;

	// The Rank, this is the maximum number string
	dataSet->maxPropertyValueLength[dataSet->header.properties.count
		+ FIFTYONEDEGREES_METRIC_RANK_INDEX] = lengthNeeded;
	// The Difference, this is also the maximum number string.
	dataSet->maxPropertyValueLength[dataSet->header.properties.count
		+ FIFTYONEDEGREES_METRIC_DIFFERENCE_INDEX] = lengthNeeded;
	// The Device-Id, using max 5 digits per profile id,
	// and a separator per component.
	dataSet->maxPropertyValueLength[dataSet->header.properties.count
		+ FIFTYONEDEGREES_METRIC_ID_INDEX]
		=  dataSet->header.components.count * (5 + 1);

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

	// Determine the maximum string length that the getValues function
	// can return for each property in the data set.
	status = setMaxPropertyValueLength(dataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}

	// Initialise the memory for the properties and values structures
	// which point to profiles structures.
	status = ensureValueProfilesSet(dataSet);

	return status;
}

/**
 * \cond
 * Sets the data set file name by copying the file name string provided into
 * newly allocated memory in the data set.
 *
 * @param dataSet whose file name field needs to be set.
 * @param fileName string to use as the file name.
 * @return dataset initialisation status.
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
* Creates a new dataset, pool and cache using the same configuration options
* as the current data set, pool and cache associated with the provider. The
* data file which the provider was initialised with  is used to create the 
* new data set. The exisitng data set, pool and cache are marked to be freed
* if worksets are being used by other threads, or if no work sets are in use
* they are freed immediately.
* @param provider pointer to the provider whose data set should be reloaded
* @return fiftyoneDegreesDataSetInitStatus indicating the result of the reload
* 	   operation.
* \endcond
*/
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesProviderReloadFromFile(
	fiftyoneDegreesProvider *provider) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_NOT_SET;
	fiftyoneDegreesDataSet *newDataSet;

	// Allocate memory for a new data set.
	newDataSet = (fiftyoneDegreesDataSet*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDataSet));
	if (newDataSet == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Initialise the new data set with the properties of the current one.
	status = initFromFile(newDataSet, provider->activePool->dataSet->fileName);
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
 * Creates a new dataset, pool and cache using the same configuration options
 * as the current data set, pool and cache associated with the provider. The
 * memory located at the source pointer is used to create the new data set.
 * The exisitng data set, pool and cache are marked to be freed if worksets are
 * being used by other threads, or if no work sets are in use they are freed
 * immediately.
 * Important: The memory pointed to by source will NOT be freed by 51Degrees
 * when the associated data set is freed. The caller is responsible for
 * releasing the memory. If 51Degrees should release the memory then the
 * caller should set the memoryToFree field of the data set associated with
 * the returned pool to source. 51Degrees will then free this memory when the
 * pool, data set and cache are freed after the last work set is returned to
 * the pool.
 * @param provider pointer to the provider whose data set should be reloaded
 * @param source pointer to the dataset held in memory.
 * @param length number of bytes that the file occupies in memory.
 * @return fiftyoneDegreesDataSetInitStatus indicating the result of the reload
 * 	   operation.
 * \endcond
 */
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesProviderReloadFromMemory(
	fiftyoneDegreesProvider *provider,
	void *source,
	long length) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_NOT_SET;
	fiftyoneDegreesDataSet *newDataSet = NULL;

	// Allocate memory for a new data set.
	newDataSet = (fiftyoneDegreesDataSet*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDataSet));
	if (newDataSet == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Initialise the new data set with the data pointed to by source.
	status = initFromMemory(newDataSet, source, length);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesFree((void*)newDataSet);
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
 * METHODS TO RETURN ELEMENTS OF THE DATA SET
 * \endcond
 */

 /**
 * \cond
 * Returns a pointer to the ascii string at the byte offset provided
 * @param dataSet pointer to the data set
 * @param offset to the ascii string required
 * @return a pointer to the AsciiString at the offset
 * \endcond
 */
const fiftyoneDegreesAsciiString* fiftyoneDegreesGetString(const fiftyoneDegreesDataSet *dataSet, int32_t offset) {
	return (const fiftyoneDegreesAsciiString*)(dataSet->strings + offset);
}

 /**
 * \cond
 * Returns a pointer to the profile at the index provided
 * @param dataSet pointer to the data set
 * @param index of the profile required
 * @return pointer to the profile at the index
 * \endcond
 */
static fiftyoneDegreesProfile* getProfileByIndex(const fiftyoneDegreesDataSet *dataSet, int32_t index) {
	return (fiftyoneDegreesProfile*)(dataSet->profiles + (dataSet->profileOffsets + index)->offset);
}

 /**
 * \cond
 * Gets the index of the property provided from the dataset
 * @param dataSet pointer to the data set containing the property
 * @param property pointer to the property
 * @return the index of the property
 * \endcond
 */
static int32_t getPropertyIndex(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesProperty *property) {
	return (int32_t)(property - dataSet->properties);
}

/**
 * \cond
 * Gets the http header name at the index provided.
 * @param dataset pointer to an initialised dataset
 * @param index of the http header required
 * @param httpHeader pointer to memory to place the http header name
 * @param size of the memory allocated for the name
 * @return the number of bytes written for the http header
 * \endcond
 */
int32_t fiftyoneDegreesGetHttpHeaderName(const fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex, char *httpHeader, int size) {
	const fiftyoneDegreesHttpHeader *uniqueHttpHeader;
	const fiftyoneDegreesAsciiString *name;
	int written = 0;
	if (httpHeaderIndex >= 0 &&
		httpHeaderIndex < dataSet->httpHeadersCount) {
		uniqueHttpHeader = dataSet->httpHeaders + httpHeaderIndex;
		name = fiftyoneDegreesGetString(dataSet, uniqueHttpHeader->headerNameOffset);
		if (name->length <= size) {
			memcpy(
				httpHeader,
				(char*)(&name->firstByte),
				name->length);
			written = name->length;
		}
	}
	return written;
}

/**
 * \cond
 * Gets the required property name at the index provided.
 * @param dataset pointer to an initialised dataset
 * @param index of the property required
 * @param propertyName pointer to memory to place the property name
 * @param size of the memory allocated for the name
 * @return the number of bytes written for the property, zero if the property
 *		  does not exist at the index
 * \endcond
 */
int32_t fiftyoneDegreesGetRequiredPropertyName(const fiftyoneDegreesDataSet *dataSet, int requiredPropertyIndex, char *propertyName, int size) {
	const fiftyoneDegreesProperty *property;
	const fiftyoneDegreesAsciiString *name;
	int written = 0;
	if (requiredPropertyIndex >= 0 &&
		requiredPropertyIndex < dataSet->requiredPropertyCount) {
		property = dataSet->requiredProperties[requiredPropertyIndex];
		name = fiftyoneDegreesGetString(dataSet, property->nameOffset);
		if (name->length <= size) {
			memcpy(
				propertyName,
				(char*)(&name->firstByte),
				name->length);
			written = name->length;
		}
	}
	return written;
}

 /**
 * \cond
 * Gets the required property index of the property provided, or -1 if the
 * property is not available in the dataset.
 * @param dataset pointer to an initialised dataset
 * @param propertyName pointer to the name of the property required
 * @return the index of the property, or -1 if the property does not exist
 * \endcond
 */
int32_t fiftyoneDegreesGetRequiredPropertyIndex(const fiftyoneDegreesDataSet *dataSet, const char *propertyName) {
	int index;
	const char *currentPropertyName;
	for (index = 0; index < dataSet->requiredPropertyCount; index++) {
		currentPropertyName = fiftyoneDegreesGetPropertyName(
			dataSet,
			dataSet->requiredProperties[index]);
		if (strcmp(currentPropertyName, propertyName) == 0) {
			return index;
		}
	}
	return -1;
}

 /**
 * \cond
 * Sets the values character array to the values of the required property
 * provided. If the values character array is too small then only the values
 * that can be fitted in are added.
 * @param ws pointer to a workset configured with the match results
 * @param requiredPropertyIndex index of the required property
 * @param values pointer to allocated memory to store the values
 * @param size the size of the values memory
 * @return the number of characters written to the values memory
 * \endcond
 */
int32_t fiftyoneDegreesGetValues(fiftyoneDegreesWorkset *ws, int32_t requiredPropertyIndex, char *values, int32_t size) {
	int valueIndex;
	int sizeNeeded = 0;
	char *currentPosition = (char*)values;
	const fiftyoneDegreesAsciiString *value;
	fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
	for (valueIndex = 0; valueIndex < ws->valuesCount; valueIndex++) {
		value = fiftyoneDegreesGetString(ws->dataSet, ws->values[valueIndex]->nameOffset);
		sizeNeeded += value->length;
		if (sizeNeeded <= size) {
			// Add a value seperator if this isn't the first value in the list.
			if (valueIndex > 0) {
				*currentPosition = '|';
				currentPosition++;
			}
			// Copy the value string to the values memory at the current position
			// appending a seperator or a end of string byte depending on the
			// remaining characters. Take one from the length because we don't
			// need the 0 string terminator.
			memcpy(currentPosition, (char*)&(value->firstByte), value->length - 1);
			// Move to the next position to either write the next value and
			// if space remaining the next value string. -1 is used to skip
			// back from the trailing 0.
			currentPosition += value->length - 1;
		}
	}

	if (sizeNeeded <= size) {
		// Write the null terminator.
		*currentPosition = 0;
		// Return the number of bytes written.
		return sizeNeeded;
	}
	else {
		// Writing the value to the values memory space will result
		// in an overrun so we can't add any more values. Stop processing
		// the remaining values.
		return -sizeNeeded;
	}
}

 /**
 * \cond
 * Returns the property associated with the name.
 * @param dataSet pointer containing the property required
 * @param name string of the property required
 * @return pointer to the property, or NULL if not found.
 * \endcond
 */
static const fiftyoneDegreesProperty* getPropertyByName(const fiftyoneDegreesDataSet *dataSet, char* name) {
	int32_t index;
	const fiftyoneDegreesProperty *property;
	for (index = 0; index < dataSet->header.properties.count; index++) {
		property = dataSet->properties + index;
		if (strcmp(fiftyoneDegreesGetPropertyName(dataSet, property),
			name) == 0)
			return property;
	}
	return NULL;
}

 /**
 * \cond
 * Returns the name of the value provided.
 * @param dataSet pointer to the data set containing the value
 * @param value pointer whose name is required
 * @return pointer to the char string of the name
 * \endcond
 */
const char* fiftyoneDegreesGetValueName(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesValue *value) {
	return (char*)(&fiftyoneDegreesGetString(dataSet, value->nameOffset)->firstByte);
}

 /**
 * \cond
 * Returns the name of the property provided.
 * @param dataSet pointer to the data set containing the property
 * @param property pointer whose name is required
 * @return pointer to the char string of the name
 * \endcond
 */
const char* fiftyoneDegreesGetPropertyName(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesProperty *property) {
	return (const char*)&(fiftyoneDegreesGetString(dataSet, property->nameOffset)->firstByte);
}

/**
* \cond
* Returns whether or not the property is a list property or not as an integer
* i.e. 1=true 0=false, or -1 if the property cannot be found.
* @param dataSet pointer to an initialised dataset.
* @param propertyName pointer to the name of the property required.
* @return 1 if the property can return a list, 0 if not, or -1 if the property
*         does not exist.
* \endcond
*/
int32_t fiftyoneDegreesGetPropertyIsList(const fiftyoneDegreesDataSet *dataSet, char *propertyName) {
	const fiftyoneDegreesProperty *property;

	property = getPropertyByName(dataSet, propertyName);
	if (property == NULL)
		return -1;
	else
		return (int32_t)property->isList;
}

 /**
 * \cond
 * Returns the first numeric index for the node provided.
 * @param node pointer to the node whose numeric indexes are required
 * @return pointer to the first numeric index for the node
 * \endcond
 */
static const fiftyoneDegreesNodeNumericIndex* getFirstNumericIndexForNode(const fiftyoneDegreesNode *node) {
	return (const fiftyoneDegreesNodeNumericIndex*)(((byte*)node)
		+ (sizeof(fiftyoneDegreesNode)
		+ (node->childrenCount * sizeof(fiftyoneDegreesNodeIndex))));
}

 /**
 * \cond
 * Returns the node associated with the node index
 * @param dataSet pointer to the data set
 * @param nodeIndex pointer associated with the node required
 * \endcond
 */
static const fiftyoneDegreesNode* getNodeFromNodeIndex(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesNodeIndex *nodeIndex) {
	return (const fiftyoneDegreesNode*)(dataSet->nodes + abs(nodeIndex->relatedNodeOffset));
}

 /**
 * \cond
 * Returns a pointer to the first node index associated with the node provided
 * @param node whose first node index is needed
 * @return the first node index of the node
 * \endcond
 */
static const fiftyoneDegreesNodeIndex* getNodeIndexesForNode(const fiftyoneDegreesNode* node) {
	return (fiftyoneDegreesNodeIndex*)(((byte*)node) + sizeof(fiftyoneDegreesNode));
}

 /**
 * \cond
 * Returns true if the node is a complete one
 * @param node pointer to be checked
 * @return true if the node is complete, otherwise false
 * \endcond
 */
static byte getIsNodeComplete(const fiftyoneDegreesNode* node) {
	return node->nextCharacterPosition != SHRT_MIN;
}

 /**
 * \cond
 * Returns the node pointer at the offset provided.
 * @param dataSet pointer to the data set
 * @param offset to the node required
 * @return pointer to the node at the offset
 * \endcond
 */
static const fiftyoneDegreesNode* getNodeByOffset(const fiftyoneDegreesDataSet *dataSet, int32_t offset) {
	return (const fiftyoneDegreesNode*)(dataSet->nodes + offset);
}

 /**
 * \cond
 * Returns the root node associated with the node provided
 * @param dataSet pointer to the data set
 * @param node pointer whose root node is required
 * @return node pointer to the root node
 * \endcond
 */
static const fiftyoneDegreesNode* getRootNode(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesNode *node) {
	if (node->parentOffset >= 0) {
		return getRootNode(dataSet, getNodeByOffset(dataSet, node->parentOffset));
	}
	return node;
}

 /**
 * \cond
 * Returns the length of the signature based on the node offsets
 * associated with the signature.
 * @param dataSet pointer to the data set
 * @param nodeOffsets pointer to the first node offset for the signature
 * @return the number of characters the signature contains
 * \endcond
 */
static int32_t getSignatureLengthFromNodeOffsets(const fiftyoneDegreesDataSet *dataSet, int32_t nodeOffset) {
	const fiftyoneDegreesNode *node = getNodeByOffset(dataSet, nodeOffset);
	return getRootNode(dataSet, node)->position + 1;
}

 /**
 * \cond
 * Returns the characters associated with the node by looking them up in the
 * strings table.
 * @param dataSet pointer to the data set
 * @param node pointer for the node whose characters are required
 * @return pointer to the ascii string associated with the node
 * \endcond
 */
static const fiftyoneDegreesAsciiString* getNodeCharacters(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesNode *node) {
	return fiftyoneDegreesGetString(dataSet, node->characterStringOffset);
}

 /**
 * \cond
 * Returns the characters associated with a node index. This is either
 * performed using the strings table, or if short by converting the value
 * of the node index to a character array. The results are returned in the
 * string structure passed into the method.
 * @param ws pointer to the workset being used for matching
 * @param nodeIndex pointer of the node index being tested
 * @param string pointer to return the string
 * \endcond
 */
static void getCharactersForNodeIndex(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNodeIndex *nodeIndex, fiftyoneDegreesString *string) {
	int16_t index;
	const fiftyoneDegreesAsciiString *asciiString;
	if (nodeIndex->relatedNodeOffset < 0) {

		asciiString = fiftyoneDegreesGetString(ws->dataSet, nodeIndex->value.integer);
		/* Set the length of the byte array removing the null terminator */
		string->length = (int16_t)(asciiString->length - 1);
		string->value = (byte*)&(asciiString->firstByte);
	}
	else {
		for (index = 0; index < 4; index++) {
			if (nodeIndex->value.characters[index] == 0)
				break;
		}
		string->length = index;
		string->value = (byte*)&(nodeIndex->value.characters);
	}
}

/**
 * \cond
 * Returns a pointer to the signatures structure containing fixed
 * fields.
 * @param dataSet pointer to the data set
 * @param signature pointer to the start of the signature
 * @return pointer to the signatures structure
 * \endcond
 */
static fiftyoneDegreesSignature* getSignatureStruct(const fiftyoneDegreesDataSet *dataSet, const byte *signature) {
	return (fiftyoneDegreesSignature*)(signature + dataSet->signatureStartOfStruct);
}

 /**
 * \cond
 * Returns the signature at the index provided.
 * @param dataSet pointer to the data set
 * @param index of the signature required
 * @return pointer to the signature at the index
 * \endcond
 */
static const byte* getSignatureByIndex(const fiftyoneDegreesDataSet *dataSet, int32_t index) {
	return dataSet->signatures + (dataSet->sizeOfSignature * index);
}

 /**
 * \cond
 * Returns the signature at the ranked index provided.
 * @param dataSet pointer to the data set
 * @param ranked index of the signature required
 * @return pointer to the signature at the ranked index
 * \endcond
 */
static const byte* getSignatureByRankedIndex(const fiftyoneDegreesDataSet *dataSet, int32_t index) {
	return getSignatureByIndex(dataSet, dataSet->rankedSignatureIndexes[index]);
}

 /**
 * \cond
 * Returns the number of node offsets associated with the signature.
 * @param dataSet pointer to the data set
 * @param signature pointer to the start of the signature
 * @return the number of nodes associated with the signature
 * \endcond
 */
static const int32_t getSignatureNodeOffsetsCount(const fiftyoneDegreesDataSet *dataSet, const byte *signature) {
	return (const int32_t)(getSignatureStruct(dataSet, signature)->nodeCount);
}

 /**
 * \cond
 * Returns the offset associated with the node pointer provided.
 * @param dataSet pointer to the data set
 * @return the integer offset to the node in the data structure
 * \endcond
 */
static int32_t getNodeOffsetFromNode(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesNode *node) {
	return (int32_t)((byte*)node - (byte*)dataSet->nodes);
}

 /**
 * \cond
 * Returns an integer pointer to the node offsets associated with the
 * signature.
 * @param dataSet pointer to the data set
 * @param signature pointer to the signature whose node offsets are required
 * @return pointer to the first integer in the node offsets associated with
 *         the signature
 * \endcond
 */
static const int32_t* getNodeOffsetsFromSignature(const fiftyoneDegreesDataSet *dataSet, const byte *signature) {
	return dataSet->signatureNodeOffsets +
		getSignatureStruct(dataSet, signature)->firstNodeOffsetIndex;
}

 /**
 * \cond
 * Loops through the signature ranks until this one is found.
 * @param dataSet pointer to the data set
 * @param signature pointer to the start of a signature structure
 * @returns the rank of the signature if available, or INT_MAX
 * \endcond
 */
static const int32_t getRankFromSignature(const fiftyoneDegreesDataSet *dataSet, const byte *signature) {
	return signature != NULL ?
		getSignatureStruct(dataSet, signature)->rank :
		INT_MAX;
}

 /**
 * \cond
 * Returns an integer pointer to the profile offsets associated with the
 * signature.
 * @param signature pointer to the signature whose profile offsets are required
 * @return pointer to the first integer in the profile offsets associated with
 *         the signature
 * \endcond
 */
static int32_t* getProfileOffsetsFromSignature(const byte *signature) {
	return (int32_t*)signature;
}

 /**
 * \cond
 * Returns a pointer to the first signature index of the node
 * @param node pointer whose first signature index is required
 * @return a pointer to the first signature index
 * \endcond
 */
static const int32_t* getFirstRankedSignatureIndexForNode(const fiftyoneDegreesNode *node) {
	return (int32_t*)(((byte*)node) + sizeof(fiftyoneDegreesNode) +
		(node->childrenCount * sizeof(fiftyoneDegreesNodeIndex)) +
		(node->numericChildrenCount * sizeof(fiftyoneDegreesNodeNumericIndex)));
}

 /**
 * \cond
 * LINKED LIST METHODS
 * \endcond
 */

 /**
 * \cond
 * Adds the signature index to the linked list with a frequency of 1.
 * @param linkedList pointer to the linked list
 * @param signatureIndex to be added to the end of the list
 * \endcond
 */
static void linkedListAdd(fiftyoneDegreesLinkedSignatureList *linkedList, int32_t rankedSignatureIndex) {
	fiftyoneDegreesLinkedSignatureListItem *newSignature = (fiftyoneDegreesLinkedSignatureListItem*)(linkedList->items) + linkedList->count;
	newSignature->rankedSignatureIndex = rankedSignatureIndex;
	newSignature->frequency = 1;
	newSignature->next = NULL;
	newSignature->previous = linkedList->last;
	if (linkedList->first == NULL)
		linkedList->first = newSignature;
	if (linkedList->last != NULL)
		linkedList->last->next = newSignature;
	linkedList->last = newSignature;
	(linkedList->count)++;
}

 /**
 * \cond
 * Builds the initial linked list using a single node.
 * @param ws pointer to the workset used for the match
 * @param node pointer to the node whose signature indexes will be used to
 *        build the initial linked list.
 * \endcond
 */
static void buildInitialList(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNode *node) {
	int32_t index;
	const int32_t *rankedSignatureIndex = getFirstRankedSignatureIndexForNode(node);
	if (node->signatureCount == 1) {
		// Only one signature so the first ranked signature index
		// is the value for the ranked signature index.
		linkedListAdd(&(ws->linkedSignatureList), *rankedSignatureIndex);
	}
	else {
		// Multiple signatures so the first ranked signature index
		// is the index of the first value in the list.
		rankedSignatureIndex = ws->dataSet->nodeRankedSignatureIndexes + *rankedSignatureIndex;
		for (index = 0; index < node->signatureCount; index++) {
			linkedListAdd(&(ws->linkedSignatureList), *(rankedSignatureIndex + index));
		}
	}
}

 /**
 * \cond
 * Adds the signature index before the item provided.
 * @param linkedList pointer to the linked list to be altered
 * @param item that the signature index should be added before
 * \endcond
 */
static void linkedListAddBefore(fiftyoneDegreesLinkedSignatureList *linkedList, fiftyoneDegreesLinkedSignatureListItem *item, int32_t rankedSignatureIndex) {
	fiftyoneDegreesLinkedSignatureListItem *newSignature = (fiftyoneDegreesLinkedSignatureListItem*)(linkedList->items + linkedList->count);
	newSignature->rankedSignatureIndex = rankedSignatureIndex;
	newSignature->frequency = 1;
	newSignature->next = item;
	newSignature->previous = item->previous;
	if (newSignature->previous != NULL) {
		newSignature->previous->next = newSignature;
	}
	item->previous = newSignature;
	if (item == linkedList->first)
		linkedList->first = newSignature;
	linkedList->count++;
}

 /**
 * \cond
 * Removes the item specified from the linked list.
 * @param linkedList pointer to the linked list to be altered
 * @param item to be removed from the list
 * \endcond
 */
static void linkedListRemove(fiftyoneDegreesLinkedSignatureList *linkedList, fiftyoneDegreesLinkedSignatureListItem *item) {
	if (item->previous != NULL)
		item->previous->next = item->next;
	if (item->next != NULL)
		item->next->previous = item->previous;
	if (item == linkedList->first)
		linkedList->first = item->next;
	if (item == linkedList->last)
		linkedList->last = item->previous;
	linkedList->count--;
}

 /**
 * \cond
 * DATASET SETUP
 * \endcond
 */

static void freeProfilesStructs(const fiftyoneDegreesDataSet *dataSet) {
	int propertyIndex, valueIndex, propertyValueCount;
	fiftyoneDegreesProperty *property;
	for (propertyIndex = 0; propertyIndex < dataSet->header.properties.count; propertyIndex++) {
		if (dataSet->valuePointersArray[propertyIndex].initialised == 1) {
			property = (fiftyoneDegreesProperty*)(dataSet->properties + (int32_t)propertyIndex);
			propertyValueCount = property->lastValueIndex - property->firstValueIndex + 1;
			for (valueIndex = 0; valueIndex < propertyValueCount; valueIndex++) {
				fiftyoneDegreesFree((void*)dataSet->valuePointersArray[propertyIndex].profilesStructs[valueIndex].indexes);
			}
		}
		if (dataSet->valuePointersArray[propertyIndex].profilesStructs != NULL) {
			fiftyoneDegreesFree((void*)dataSet->valuePointersArray[propertyIndex].profilesStructs);
		}
	}
	fiftyoneDegreesFree((void*)dataSet->valuePointersArray);
}

/**
 * \cond
 * Destroys the data set releasing all memory available. Ensure all worksets
 * cache and pool structs are freed prior to calling this method.
 * @param dataSet pointer to the data set being destroyed
 * \endcond
 */
void fiftyoneDegreesDataSetFree(const fiftyoneDegreesDataSet *dataSet) {
	int index;

	freeProfilesStructs(dataSet);

	if (dataSet->prefixedUpperHttpHeaders != NULL) {
		for (index = 0; index < dataSet->httpHeadersCount; index++) {
			if (dataSet->prefixedUpperHttpHeaders[index] != NULL) {
				fiftyoneDegreesFree((void*)dataSet->prefixedUpperHttpHeaders[index]);
			}
		}
		fiftyoneDegreesFree((void*)dataSet->prefixedUpperHttpHeaders);
	}

	if (dataSet->maxPropertyValueLength != NULL) {
		fiftyoneDegreesFree(dataSet->maxPropertyValueLength);
	}

	if (dataSet->fileName != NULL) {
		fiftyoneDegreesFree((void*)dataSet->fileName);
	}

	if (dataSet->httpHeaders != NULL) {
		fiftyoneDegreesFree((void*)dataSet->httpHeaders);
	}

	if (dataSet->components != NULL) {
		fiftyoneDegreesFree((void*)dataSet->components);
	}

	if (dataSet->rootNodes != NULL) {
		fiftyoneDegreesFree((void*)dataSet->rootNodes);
	}

	if (dataSet->requiredProperties != NULL) {
		fiftyoneDegreesFree((void*)dataSet->requiredProperties);
	}

	if (dataSet->memoryToFree != NULL) {
		fiftyoneDegreesFree((void*)dataSet->memoryToFree);
	}
}

 /**
 * \cond
 * Adds all properties in the data set to the required properties list.
 * @param dataSet pointer to the data set
 * \endcond
 */
static void setAllProperties(fiftyoneDegreesDataSet *dataSet) {
	int32_t index;
	dataSet->requiredPropertyCount = dataSet->header.properties.count;
	dataSet->requiredProperties =
		(const fiftyoneDegreesProperty**)fiftyoneDegreesMalloc(SIZE_OF_REQUIRED_PROPERTIES(dataSet->requiredPropertyCount));
	if (dataSet->requiredProperties != NULL) {
		for (index = 0; index < dataSet->requiredPropertyCount; index++) {
			*(dataSet->requiredProperties + index) = dataSet->properties + index;
		}
	}
}

 /**
 * \cond
 * Adds the properties in the array of properties to the list
 * of required properties from a match.
 * @param dataSet pointer to the data set
 * @param properties array of properties to be returned
 * @param count number of elements in the properties array
 * \endcond
 */
static void setProperties(fiftyoneDegreesDataSet *dataSet, const char** properties, int32_t count) {
	int32_t index, propertyIndex;
	int16_t requiredPropertyLength;
	const char *requiredPropertyName;
	const fiftyoneDegreesAsciiString *propertyName;

	// Allocate memory for this number of properties.
	dataSet->requiredPropertyCount = 0;
	dataSet->requiredProperties = (const fiftyoneDegreesProperty**)fiftyoneDegreesMalloc(SIZE_OF_REQUIRED_PROPERTIES(count));
	// Add the properties to the list of required properties.
	if (dataSet->requiredProperties != NULL) {
		for (propertyIndex = 0; propertyIndex < count; propertyIndex++) {
			// Get next property name
			requiredPropertyName = *(properties + propertyIndex);
			requiredPropertyLength = (int16_t)strlen(requiredPropertyName);

			for (index = 0; index < dataSet->header.properties.count; index++) {
				// Go through each property in the properties dataset was initialised with.
				// Get name of current property of the dataset's properties
				propertyName = fiftyoneDegreesGetString(dataSet, (dataSet->properties + index)->nameOffset);
				// Compare the two properties byte values.
				if (requiredPropertyLength == propertyName->length - 1 &&
					memcmp(requiredPropertyName, &propertyName->firstByte, requiredPropertyLength) == 0) {

					*(dataSet->requiredProperties + dataSet->requiredPropertyCount) = (dataSet->properties + index);
					dataSet->requiredPropertyCount++;
					break;
				}
			}

		}
	}
}

/**
 * \cond
 * Gets the number of separators in the char array
 * @param input char array containing separated values
 * @return number of separators
 * \endcond
 */
static int32_t getSeparatorCount(const char* input) {
	int32_t index = 0, count = 0;
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
 * Initialises the data set passed to the method with the data from
 * the file provided. If required properties is provided the data set
 * will only return those listed and separated by comma, pipe, space
 * or tab.
 * @param fileName of the data source to use for initialisation
 * @param dataSet pointer to the data set
 * @param requiredProperties char array to the separated list of properties
 *        the dataSet can return
 * @return the number of bytes read from the file
 * \endcond
 */
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyString(
									const char *fileName,
									fiftyoneDegreesDataSet *dataSet,
									const char* requiredProperties) {
	int32_t index, count = 0;
	const char **requiredPropertiesArray = NULL;
	char *currentProperty, *copyRequiredProperties = NULL;
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_NOT_SET;
	int32_t requiredPropertyCount = getSeparatorCount(requiredProperties);

	if (fileName == NULL || fileName[0] == '\0') {
		return DATA_SET_INIT_STATUS_FILE_NOT_FOUND;
	}

	// Determine if properties were provided.
	if (requiredPropertyCount > 0) {
		// Copy the properties as we'll be null terminating at the seperators.
		copyRequiredProperties = strdup(requiredProperties);
		if (copyRequiredProperties != NULL) {
			// Allocate pointers for each of the properties.
			requiredPropertiesArray = (const char**)fiftyoneDegreesMalloc(
				SIZE_OF_REQUIRED_PROPERTIES_ARRAY(requiredPropertyCount));
			currentProperty = copyRequiredProperties;
			if (requiredPropertiesArray != NULL) {
				// Change the input string so that the separators are changed to nulls.
				for (index = 0; count < requiredPropertyCount; index++) {
					if (*(copyRequiredProperties + index) == ',' ||
						*(copyRequiredProperties + index) == '|' ||
						*(copyRequiredProperties + index) == ' ' ||
						*(copyRequiredProperties + index) == '\t' ||
						*(copyRequiredProperties + index) == 0) {
						*(copyRequiredProperties + index) = 0;
						requiredPropertiesArray[count] = currentProperty;
						currentProperty = copyRequiredProperties + index + 1;
						count++;
					}
				}
			}
		}
		else {
			// Memory could not be allocated for the properties
			// to be turned into an array.
			status = DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
		}
	}

	// Initialise the data set with the properties extracted.
	status = fiftyoneDegreesInitWithPropertyArray(
		fileName,
		dataSet,
		requiredPropertiesArray,
		requiredPropertyCount);

	// Free the memory used to process the properties string.
	if (requiredPropertiesArray != NULL) {
		fiftyoneDegreesFree((void*)requiredPropertiesArray);
	}
	if (copyRequiredProperties != NULL) {
		fiftyoneDegreesFree(copyRequiredProperties);
	}

	return status;
}

/**
 * \cond
 * Initialises the provider passed to the method with a data set initialised
 * from the file provided. If required properties is provided the associated
 * data set will only return properties contained between separators.
 * @param fileName of the data source to use for initialisation
 * @param provider pointer to the pool to be initialised
 * @param properties char array to the separated list of properties
 *        the dataSet can return
 * @param poolSize number of work sets to hold in the pool
 * @param cacheSize maximum number of items that the cache should store
 * @return fiftyoneDegreesDataSetInitStatus indicating the result of creating
 * 	   the new dataset, pool and cache. If status is anything other than
 * 	   DATA_SET_INIT_STATUS_SUCCESS, then the initialization has failed.
 * \endcond
 */
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitProviderWithPropertyString(
		const char *fileName,
		fiftyoneDegreesProvider *provider,
		const char* properties,
		int poolSize,
		int cacheSize) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_NOT_SET;
	fiftyoneDegreesDataSet *dataSet;
	dataSet = (fiftyoneDegreesDataSet *)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDataSet));
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
	FIFTYONEDEGREES_SIGNAL_CREATE(provider->signal);
#endif
	return initProvider(provider, dataSet, poolSize, cacheSize);
}

 /**
 * \cond
 * Initialises the data set passed to the method with the data from
 * the file provided. If required properties is provided the data set
 * will only return those contained in the array.
 * or tab.
 * @param fileName of the data source to use for initialisation
 * @param dataSet pointer to the data set
 * @param requiredProperties array of strings containing the property names
 * @param count the number of elements in the requiredProperties array
 * @return the number of bytes read from the file
 * \endcond
 */
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyArray(
		const char *fileName,
		fiftyoneDegreesDataSet *dataSet,
		const char** requiredProperties, int32_t count) {
	// Initialise the data set from the file provided.
	fiftyoneDegreesDataSetInitStatus status = initFromFile(dataSet, fileName);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}

	// Set the properties that are returned by the data set.
	if (requiredProperties == NULL || count == 0) {
		setAllProperties(dataSet);
	}
	else {
		setProperties(dataSet, requiredProperties, count);
	}
	
	return status;
}

/**
 * \cond
 * Initialises the provider passed to the method with a data set initialised
 * from the file provided. If required properties is provided the associated
 * data set will only return properties contained in the array.
 * @param fileName of the data source to use for initialisation
 * @param provider pointer to the pool to be initialised
 * @param properties array of strings containing the property names
 * @param count the number of elements in the requiredProperties array
 * @param poolSize number of work sets to hold in the pool
 * @param cacheSize maximum number of items that the cache should store
 * @return fiftyoneDegreesDataSetInitStatus indicating the result of creating
 * 	   the new dataset, pool and cache. If status is anything other than
 * 	   DATA_SET_INIT_STATUS_SUCCESS, then the initialization has failed.
 * \endcond
 */
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitProviderWithPropertyArray(
		const char *fileName,
		fiftyoneDegreesProvider *provider,
		const char** properties,
		int32_t count,
		int poolSize,
		int cacheSize) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_NOT_SET;
	fiftyoneDegreesDataSet *dataSet;
	dataSet = (fiftyoneDegreesDataSet *)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDataSet));
	if (dataSet == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	status = fiftyoneDegreesInitWithPropertyArray(fileName, dataSet, properties, count);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesFree(dataSet);
		return status;
	}
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_CREATE(provider->lock);
	FIFTYONEDEGREES_SIGNAL_CREATE(provider->signal);
#endif
	return initProvider(provider, dataSet, poolSize, cacheSize);
}

/**
* \cond
* Uses the memory allocation size macros to calculate the amount of memory
* needed by the provider. The size of the file and filename must be set prior
* to this function. The returned value overestimates by about 10 bytes to
* accommodate for extra http headers being introduced.
* @param sizeOfFile the size of the file and filename.
* @param header the data set header read from the file.
* @param requiredPropertyCount the number of properties the provider
* will be initialised with.
* @param poolSize the number of worksets in the pool.
* @param cacheSize the size of the resultset cache.
* @returns int the total size in bytes that the provider will need.
* \endcond
*/
static size_t getProviderSizeWithPropertyCount(size_t sizeOfFile, fiftyoneDegreesDataSetHeader header, int requiredPropertyCount, int poolSize, int cacheSize)
{
	int httpHeadersCount;
	size_t size;
	// Start with the size taken by the file and the filename.
	size = sizeOfFile;

	// Set maximum http headers per component. This is a slight overestimate
	// but saves reading in the data set.
	httpHeadersCount = 5;

	// Add provider.
	size += sizeof(fiftyoneDegreesProvider);
	// Add dataset.
	size += sizeof(fiftyoneDegreesDataSet);
	// Add components.
	size += (SIZE_OF_COMPONENTS(header));
	// Add http headers.
	size += (SIZE_OF_HTTP_HEADERS(httpHeadersCount * header.components.count));
	// Add root nodes.
	size += (SIZE_OF_ROOT_NODES(header));
	// Add required properties.
	size += (SIZE_OF_REQUIRED_PROPERTIES(requiredPropertyCount));

	// Add profile structures.
	size += (SIZE_OF_PROFILES_STRUCT_ARRAY(header));
	size += (SIZE_OF_PROFILE_INDEXES_STRUCT(header.values.count));

	// Add max property value length values.
	size += (SIZE_OF_MAX_PROPERTY_VALUES(header));

	// Add cache.
	if (cacheSize > 0) {
		size += (SIZE_OF_CACHE);
		size += (SIZE_OF_CACHE_RESULTSETS(cacheSize));
		size += (SIZE_OF_CACHE_TARGET_USER_AGENTS(cacheSize, header));
		size += (SIZE_OF_CACHE_PROFILES(cacheSize, header));
	}

	// Add workset pool.
	size += (SIZE_OF_WORKSET_POOL);
	size += (SIZE_OF_POOL_WORKSETS(poolSize));

	// Add worksets.
	size += ((SIZE_OF_WORKSET)* poolSize);
	size += ((SIZE_OF_WORKSET_INPUT(header)) * poolSize);
	size += ((SIZE_OF_WORKSET_SIGLIST_ITEMS(header)) * poolSize);
	size += ((SIZE_OF_WORKSET_NODES(header)) * poolSize * 2);
	size += ((SIZE_OF_WORKSET_USED_NODES(header)) * poolSize * 2);
	size += ((SIZE_OF_WORKSET_SIG_AS_STRING(header)) * poolSize);
	size += ((SIZE_OF_WORKSET_VALUES(header)) * poolSize);
	size += ((SIZE_OF_WORKSET_PROFILES(header)) * poolSize * 2);
	size += ((SIZE_OF_WORKSET_TARGET_USERAGENT_ARRAY(header)) * poolSize);
	size += ((SIZE_OF_WORKSET_IMPORTANT_HEADERS(httpHeadersCount)) * poolSize);

	assert(size > sizeOfFile);

	// Return the total size needed for the provider.
	return size;
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
* Calculates the amount of memory that the provider will need to allocate for
* the given data file and initialisation parameters. This should be used with
* the fiftyoneDegreesInitProviderWithPropertyString function.
* NOTE: This function will over estimate by about 10 bytes to account for a
* possible increase in http headers.
* @param fileName the file path of the data file.
* @param properties the comma separated string of properties that will be
* initialised.
* @param poolSize the number of worksets the pool will contain.
* @param cacheSize the size of the resultset cache.
* @return int the total size in bytes that is needed to initilaise the
* provider with the given parameters.
* \endcond
*/
size_t fiftyoneDegreesGetProviderSizeWithPropertyString(const char *fileName, const char *properties, int poolSize, int cacheSize)
{
	int requiredPropertyCount;
	size_t size;
	FILE *inputFilePtr;
	fiftyoneDegreesDataSetHeader *header = (fiftyoneDegreesDataSetHeader*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDataSetHeader));

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

	// Read in header.
	if (fread(header, sizeof(fiftyoneDegreesDataSetHeader), 1, inputFilePtr) != 1) {
		return -1;
	}

	size = getSizeOfFile(fileName);

	if ((int)size > 0) {
		// Add file name.
		size += (SIZE_OF_FILE_NAME(fileName));

		// Get property count.
		requiredPropertyCount = getSeparatorCount(properties);

		// If property string is empty, all properties will be used.
		if (requiredPropertyCount == 0) {
			requiredPropertyCount = header->properties.count;
		}

		// Add required properties array.
		size += (SIZE_OF_REQUIRED_PROPERTIES_ARRAY(requiredPropertyCount));

		// Return the total size needed for the provider.
		size = getProviderSizeWithPropertyCount(size, *header, requiredPropertyCount, poolSize, cacheSize);
	}
	return size;
}

/**
* \cond
* Calculates the amount of memory that the provider will need to allocate for
* the given data file and initialisation parameters. This should be used with
* the fiftyoneDegreesInitProviderWithPropertyArray function.
* NOTE: This function will over estimate by about 10 bytes to account for a
* possible increase in http headers.
* @param fileName the file path of the data file.
* @param propertyCount the number of properties in the properties array.
* @param poolSize the number of worksets the pool will contain.
* @param cacheSize the size of the resultset cache.
* @return int the total size in bytes that is needed to initilaise the
* provider with the given parameters.
* \endcond
*/
size_t fiftyoneDegreesGetProviderSizeWithPropertyCount(const char *fileName, int propertyCount, int poolSize, int cacheSize)
{
	size_t size;
	FILE *inputFilePtr;
	fiftyoneDegreesDataSetHeader *header = (fiftyoneDegreesDataSetHeader*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDataSetHeader));

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

	// Read in header.
	if (fread(header, sizeof(fiftyoneDegreesDataSetHeader), 1, inputFilePtr) != 1) {
		return -1;
	}

	size = getSizeOfFile(fileName);

	if ((int)size > 0) {
		// Add file name.
		size += (SIZE_OF_FILE_NAME(fileName));

		// Return the total size needed for the provider.
		size = getProviderSizeWithPropertyCount(size, *header, propertyCount, poolSize, cacheSize);
	}
	return size;
}

/**
 * FIND PROFILES METHODS
 */

/**
 * \cond
 * Sets the profile indexes relating to all values of the given property
 * allocating space for them first.
 * @param dataSet a pointer to the data set.
 * @param property the property to set the profiles for.
 * @param valuesProfileCount and array where each entry indicates the number of
 * profiles that relate to the value of corresponding index.
 * \endcond
 */
static void setProfileStructs(const fiftyoneDegreesDataSet *dataSet,
	const fiftyoneDegreesProperty *property,
	int32_t *valuesProfileCount) {
	int profileIndex, profileValueIndex, propertyValueIndex, propertyIndex;
	fiftyoneDegreesProfile *profile;
	int32_t *profileValueIndexes;
	int profileIndexElement, propertyValuesCount;

	propertyIndex = getPropertyIndex(dataSet, property);
	propertyValuesCount = property->lastValueIndex - property->firstValueIndex + 1;

	// Allocate memory for the profile indexes.
	for (propertyValueIndex = 0; propertyValueIndex < propertyValuesCount; propertyValueIndex++) {
		dataSet->valuePointersArray[propertyIndex].profilesStructs[propertyValueIndex].count =
			valuesProfileCount[propertyValueIndex];
		dataSet->valuePointersArray[propertyIndex].profilesStructs[propertyValueIndex].indexes =
			(int32_t*)fiftyoneDegreesMalloc(valuesProfileCount[propertyValueIndex] * sizeof(int32_t));
	}

	// Add the profile indexes to the values that they relate to.
	for (profileIndex = 0; profileIndex < dataSet->header.profiles.count; profileIndex++) {
		profile = getProfileByIndex(dataSet, profileIndex);
		// If the profile has a different component, then skip it as
		// nothing will be found.
		if (profile->componentIndex == property->componentIndex) {
			profileValueIndexes = (int32_t*)((byte*)profile + sizeof(fiftyoneDegreesProfile));
			// Loop through the value indexes for the current profile.
			for (profileValueIndex = 0; profileValueIndex < profile->valueCount; profileValueIndex++) {
				// If the value is one that relates to the property, add the profile.
				if (property->firstValueIndex <= profileValueIndexes[profileValueIndex]
					&& profileValueIndexes[profileValueIndex] <= property->lastValueIndex) {
					profileIndexElement =
						dataSet->valuePointersArray[propertyIndex].profilesStructs[profileValueIndexes[profileValueIndex] -
						property->firstValueIndex].count - valuesProfileCount[profileValueIndexes[profileValueIndex] -
						property->firstValueIndex];
					dataSet->valuePointersArray[propertyIndex].profilesStructs[profileValueIndexes[profileValueIndex] -
						property->firstValueIndex].indexes[profileIndexElement] = profileIndex;
					valuesProfileCount[profileValueIndexes[profileValueIndex] - property->firstValueIndex]--;
				}
			}
		}
	}
}

/**
 * \cond
 * Initialise the values for the given property with the indexes and count
 * of the profiles relating to each value.
 * @param dataSet pointer to a 51Degrees data set.
 * @param property pointer to the 51Degrees property to be initialised.
 * \endcond
 */
static void initFindProfiles(const fiftyoneDegreesDataSet *dataSet,
							 const fiftyoneDegreesProperty *property) {
	int profileIndex;
	int valueIndex;
	int propertyIndex = getPropertyIndex(dataSet, property);
	fiftyoneDegreesProfile *profile;
	int32_t *profileValueIndexes, propertyValuesCount, *valuesProfileCount;

	// Lock the structure being set, and check again if it has been
	// set by another process.
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_LOCK(&dataSet->valuePointersArray[propertyIndex].lock);
	if (dataSet->valuePointersArray[propertyIndex].initialised == 0) {
#endif
		// Get the amount of values for the property.
		propertyValuesCount = property->lastValueIndex - property->firstValueIndex + 1;
		// Initialise the array with the profiles count for each of the property's values.
		valuesProfileCount = (int32_t*)fiftyoneDegreesCalloc(propertyValuesCount, sizeof(int32_t));

		// Loop through all profiles incrementing the profile count for the
		// for the values they relate to.
		profileIndex = 0;
		while (profileIndex < dataSet->header.profiles.count) {
			profile = getProfileByIndex(dataSet, profileIndex);
			// If the profile has a different component, then skip it as
			// nothing will be found.
			if (profile->componentIndex == property->componentIndex) {
				profileValueIndexes = (int32_t*)((byte*)profile + sizeof(fiftyoneDegreesProfile));
				// Loop through the value indexes for the current profile.
				for (valueIndex = 0; valueIndex < profile->valueCount; valueIndex++) {
					// If the value relates to this property then increment the
					// profile count for the value.
					if (property->firstValueIndex <= profileValueIndexes[valueIndex]
						&& profileValueIndexes[valueIndex] <= property->lastValueIndex) {
						valuesProfileCount[profileValueIndexes[valueIndex] - property->firstValueIndex]++;
					}
				}
			}
			profileIndex++;
		}

		// Now allocate the memory and do a second pass adding the profiles.
		setProfileStructs(dataSet, property, valuesProfileCount);
		// Indicate this property has been initialised.
		dataSet->valuePointersArray[propertyIndex].initialised = 1;
		// Free the array of profile counts.
		fiftyoneDegreesFree(valuesProfileCount);

#ifndef FIFTYONEDEGREES_NO_THREADING
	}
	FIFTYONEDEGREES_MUTEX_UNLOCK(&dataSet->valuePointersArray[propertyIndex].lock);
#endif
}

/**
 * \cond
 * fiftyoneDegreesFindProfiles function initialises all the profiles for
 * the property if it has not been, then returns a profiles structure relating
 * to the given property and value pair containing the count, an array of profile
 * pointers, and an array of profile indexes.
 * @param dataSet pointer to a 51Degrees data set.
 * @param propertyName the name of the property to match as a string.
 * @param valueName the name of the property's value to match as a string.
 * @returns fiftyoneDegreesProfilesStruct* pointer to a profiles structure.
 * \endcond
 */
fiftyoneDegreesProfilesStruct *fiftyoneDegreesFindProfiles(
	const fiftyoneDegreesDataSet *dataSet,
	const char *propertyName,
	const char* valueName) {
	int32_t valueIndex, propertyIndex;
	const char *currentValueName;
	const fiftyoneDegreesValue *value;
	const fiftyoneDegreesProperty *property;
	int i;
	fiftyoneDegreesProfilesStruct *profilesList;
	property = getPropertyByName(dataSet, (char*)propertyName);

	// Only proceed if the property exists.
	if (property != NULL) {
		propertyIndex = getPropertyIndex(dataSet, property);

		// Find the value from the value name.
		for (valueIndex = property->firstValueIndex; valueIndex <= property->lastValueIndex; valueIndex++) {
			value = dataSet->values + valueIndex;
			currentValueName = fiftyoneDegreesGetValueName(dataSet, value);
			if (strcmp(valueName, currentValueName) == 0) {
				// Found the value. Now check if the property is initialised.
				if (dataSet->valuePointersArray[propertyIndex].initialised != 1) {
					initFindProfiles(dataSet, property);
				}
				// Set the profiles list to be returned, copying the profile
				// indexes and getting pointers to the profiles.
				profilesList = (fiftyoneDegreesProfilesStruct*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesProfilesStruct));
				profilesList->count = (int32_t)dataSet->valuePointersArray[propertyIndex].profilesStructs[valueIndex - property->firstValueIndex].count;
				profilesList->profiles = (fiftyoneDegreesProfile**)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesProfile*) * profilesList->count);
				profilesList->indexes = (int32_t*)fiftyoneDegreesMalloc(sizeof(int32_t) * profilesList->count);
				memcpy(profilesList->indexes, dataSet->valuePointersArray[propertyIndex].profilesStructs[valueIndex - property->firstValueIndex].indexes,
					sizeof(int32_t) * dataSet->valuePointersArray[propertyIndex].profilesStructs[valueIndex - property->firstValueIndex].count);

				for (i = 0; i < profilesList->count; i++) {
					profilesList->profiles[i] = getProfileByIndex(dataSet, profilesList->indexes[i]);
				}
				return profilesList;
			}
		}
	}
	// The property or the value could not be found, return an empty list.
	profilesList = (fiftyoneDegreesProfilesStruct*)fiftyoneDegreesMalloc(sizeof(int));
	profilesList->count = 0;
	return profilesList;
}

/**
 * \cond
 * Frees a 51Degrees profiles structure returned from the
 * fiftyoneDegreesFindProfiles function.
 * @param profiles pointer to a 51Degrees profiles structure.
 * \endcond
 */
void fiftyoneDegreesFreeProfilesStruct(fiftyoneDegreesProfilesStruct *profiles) {
	fiftyoneDegreesFree((void*)profiles->indexes);
	fiftyoneDegreesFree((void*)profiles->profiles);
	fiftyoneDegreesFree((void*)profiles);
}

/**
 * \cond
 * Function to compare two integers, this is used in the bsearch function.
 * @param a pointer to an integer.
 * @param b pointer to an integer.
 * \endcond
 */
static int FIFTYONEDEGREES_CALL_CONV intcmp(const void *a, const void *b) {
	return (*(int32_t*)a - *(int32_t*)b);
}

/**
 * \cond
 * fiftyoneDegreesFindProfilesInProfiles function initialises all the profiles for
 * the property if it has not been, then returns a profiles structure relating
 * to the given property and value pair containing the count, an array of profile
 * pointers, and an array of profile indexes.
 * @param dataSet pointer to a 51Degrees data set.
 * @param propertyName the name of the property to match as a string.
 * @param valueName the name of the property's value to match as a string.
 * @param profilesList a pointer to the profiles structure to filter.
 * @returns fiftyoneDegreesProfilesStruct* pointer to a profiles structure.
 * \endcond
 */
fiftyoneDegreesProfilesStruct *fiftyoneDegreesFindProfilesInProfiles(
	const fiftyoneDegreesDataSet *dataSet,
	const char *propertyName,
	const char* valueName,
	fiftyoneDegreesProfilesStruct *profilesList) {
	int32_t valueIndex, profileStructElement, propertyIndex;
	const char *currentValueName;
	const fiftyoneDegreesValue *value;
	const fiftyoneDegreesProperty *property;
	fiftyoneDegreesProfilesStruct *matchingProfiles;
	fiftyoneDegreesProfile *firstProfile;
	int i, *matchingIndex, matchingProfilesCount;

	matchingProfilesCount = 0;
	profileStructElement = 0;

	matchingProfiles = (fiftyoneDegreesProfilesStruct*)fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesProfilesStruct));

	property = getPropertyByName(dataSet, (char*)propertyName);

	// Only proceed if the property exists in the data set.
	if (property != NULL) {
		propertyIndex = getPropertyIndex(dataSet, property);
		firstProfile = profilesList->profiles[0];
		// Check that the profiles being filtered have the same component
		// as the property being matched. If they don't, nothing will be
		// found.
		if (firstProfile->componentIndex == property->componentIndex) {
			for (valueIndex = property->firstValueIndex; valueIndex <= property->lastValueIndex; valueIndex++) {
				value = dataSet->values + valueIndex;
				currentValueName = fiftyoneDegreesGetValueName(dataSet, value);
				if (strcmp(valueName, currentValueName) == 0) {
					// Found the value, now check the property is initialised.
					if (dataSet->valuePointersArray[propertyIndex].initialised != 1) {
						initFindProfiles(dataSet, property);
					}

					// Count the number of profiles that will be returned.
					for (i = 0; i < profilesList->count; i++) {
						matchingIndex = (int32_t*)bsearch(&profilesList->indexes[i],
							dataSet->valuePointersArray[propertyIndex].profilesStructs[valueIndex - property->firstValueIndex].indexes,
							dataSet->valuePointersArray[propertyIndex].profilesStructs[valueIndex - property->firstValueIndex].count,
							sizeof(int32_t), intcmp);
						if (matchingIndex != NULL) {
							matchingProfilesCount++;
						}
					}

					// Set the profiles count and allocate the space for the
					// indexes and pointers.
					matchingProfiles->count = matchingProfilesCount;
					matchingProfiles->indexes = (int32_t*)fiftyoneDegreesCalloc(matchingProfilesCount, sizeof(int32_t));
					matchingProfiles->profiles = (fiftyoneDegreesProfile**)fiftyoneDegreesCalloc(matchingProfilesCount, sizeof(fiftyoneDegreesProfile*));

					// Do a second pass of the seach adding the indexes and
					// pointers to the structure to be returned.
					for (i = 0; i < profilesList->count; i++) {
						matchingIndex = (int32_t*)bsearch(&profilesList->indexes[i],
							dataSet->valuePointersArray[propertyIndex].profilesStructs[valueIndex - property->firstValueIndex].indexes,
							dataSet->valuePointersArray[propertyIndex].profilesStructs[valueIndex - property->firstValueIndex].count,
							sizeof(int32_t), intcmp);
						if (matchingIndex != NULL) {
							matchingProfiles->indexes[profileStructElement] = *matchingIndex;
							matchingProfiles->profiles[profileStructElement] = getProfileByIndex(dataSet, *matchingIndex);
							profileStructElement++;
						}
					}
					return matchingProfiles;
				}
			}
		}
	}

	// No profiles found, so return an empty profiles structure.
	matchingProfiles->count = 0;
	return matchingProfiles;
}

 /**
 * \cond
 * WORKSET POOL METHODS
 * \endcond
 */

 /**
 * \cond
 * Creates a new workset pool for the data set and cache provided.
 * @param dataset pointer to a data set structure
 * @param cache pointer to a cache, or NULL if no cache to be used
 * @return a pointer to a new work set pool, or NULL if the pool can't be
 *		   created.
 * \endcond
 */
fiftyoneDegreesWorksetPool *fiftyoneDegreesWorksetPoolCreate(
								fiftyoneDegreesDataSet *dataSet,
								fiftyoneDegreesResultsetCache *cache,
								int32_t size) {
	int worksetIndex;
	fiftyoneDegreesWorksetPool *pool =
		(fiftyoneDegreesWorksetPool*)fiftyoneDegreesMalloc(SIZE_OF_WORKSET_POOL);
	if (pool != NULL) {
		pool->dataSet = dataSet;
		pool->cache = cache;
		pool->size = size;
		pool->provider = NULL;
#ifndef FIFTYONEDEGREES_NO_THREADING
		FIFTYONEDEGREES_MUTEX_CREATE(pool->lock);
		FIFTYONEDEGREES_SIGNAL_CREATE(pool->signal);
#endif
		pool->worksets = (fiftyoneDegreesWorkset**)fiftyoneDegreesMalloc(SIZE_OF_POOL_WORKSETS(size));
		if (pool->worksets == NULL
#ifndef FIFTYONEDEGREES_NO_THREADING
			||
			FIFTYONEDEGREES_MUTEX_VALID(&pool->lock) == 0 ||
			FIFTYONEDEGREES_SIGNAL_VALID(&pool->signal) == 0
#endif
			// Release the resources created so far.
			) {
			if (pool->worksets != NULL) { fiftyoneDegreesFree((void*)pool->worksets); }
#ifndef FIFTYONEDEGREES_NO_THREADING
			if (FIFTYONEDEGREES_MUTEX_VALID(&pool->lock) == 0) { FIFTYONEDEGREES_MUTEX_CLOSE(pool->lock); }
			if (FIFTYONEDEGREES_SIGNAL_VALID(&pool->signal) == 0) { FIFTYONEDEGREES_SIGNAL_CLOSE(pool->signal); }
#endif
			fiftyoneDegreesFree((void*)pool);
			pool = NULL;
		}
		else {
			// Fill the pool with worksets. If there is a problem
			// creating one then reduce the size of the pool to the
			// number that can be created.
			worksetIndex = 0;
			while (worksetIndex < pool->size) {
				pool->worksets[worksetIndex] = fiftyoneDegreesWorksetCreate(pool->dataSet, pool->cache);
				if (pool->worksets[worksetIndex] != NULL) {
					pool->worksets[worksetIndex]->associatedPool = pool;
				}
				else {
					pool->size = worksetIndex;
				}
				worksetIndex++;
			}
			pool->available = pool->size;

#ifndef FIFTYONEDEGREES_NO_THREADING
			// Set the lock and signal pointers to the ones held in the pool structure.
			pool->lockPtr = &pool->lock;
			pool->signalPtr = &pool->signal;
#endif
		}
	}
	return pool;
}

/**
 * \cond
 * Returns the workset back to the pool it was created from.
 * Worksets created without a pool should be freed using the method
 * fiftyoneDegreesWorksetFree.
 * @param ws a workset that was created from a pool or provider.
 * \endcond
 */
void fiftyoneDegreesWorksetRelease(fiftyoneDegreesWorkset *ws) {
	fiftyoneDegreesWorksetPool *pool = (fiftyoneDegreesWorksetPool*)ws->associatedPool;
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX *lockPtr = pool->lockPtr;
	FIFTYONEDEGREES_MUTEX_LOCK(lockPtr);
#endif
	if (pool->available >= pool->size) {
		// The pool is already full, so destroy the workset.
		fiftyoneDegreesWorksetFree(ws);
	}
	else {
		// Place the workset at the next available space in the
		// array of worksets.
		pool->worksets[pool->available] = ws;
		pool->available++;
#ifndef FIFTYONEDEGREES_NO_THREADING
		FIFTYONEDEGREES_SIGNAL_SET(pool->signalPtr);
#endif
	}

	// If the pool the work set is associated with is not the active pool and
	// all the work sets have been handed back then free the pool and its
	// related resources.
	if (pool->provider != NULL &&
		pool->provider->activePool != pool &&
		pool->available == pool->size) {
		fiftyoneDegreesWorksetPoolCacheDataSetFree(pool);
	}

#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_UNLOCK(lockPtr);
#endif
}

/**
 * \cond
 * Releases all the resources used by the provider. The provider can not be
 * used without being reinitialised after calling this method.
 * @param provider pointer to the provider to be freed
 * \endcond
 */
void fiftyoneDegreesProviderFree(fiftyoneDegreesProvider *provider) {
	fiftyoneDegreesWorksetPoolCacheDataSetFree((fiftyoneDegreesWorksetPool*)provider->activePool);
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_CLOSE(provider->lock);
	FIFTYONEDEGREES_SIGNAL_CLOSE(provider->signal);
#endif
}

/**
 * \cond
 * Returns a work set from the pool if one is available.
 * @param pool pointer to the pool to return the work set from
 * @return a work set ready for device detection, or NULL if no work sets are
 *		   available
 * \endcond
 */
static fiftyoneDegreesWorkset* worksetPoolGet(fiftyoneDegreesWorksetPool *pool) {
	fiftyoneDegreesWorkset *ws = NULL;
	if (pool->available > 0) {
		// Worksets are available. Take one from the end of the array.
		ws = pool->worksets[pool->available - 1];
		pool->available--;
	}
	return ws;
}

/**
 * \cond
 * Gets a workset from the pool, or creates a new one if none are available
 * @param pool pointer to a pool structure
 * @returns pointer to a workset that is free and ready for use, or NULL if
 *          none are available.
 * \endcond
 */
fiftyoneDegreesWorkset *fiftyoneDegreesWorksetPoolGet(
	fiftyoneDegreesWorksetPool *pool) {
	fiftyoneDegreesWorkset *ws = NULL;
#ifndef FIFTYONEDEGREES_NO_THREADING
	// Loop until a work set is available from the pool.
	while (ws == NULL) {
		FIFTYONEDEGREES_MUTEX_LOCK(pool->lockPtr);
		ws = worksetPoolGet(pool);
		FIFTYONEDEGREES_MUTEX_UNLOCK(pool->lockPtr);
		if (ws == NULL) {
			// Wait for a work set to return to the pool.
			FIFTYONEDEGREES_SIGNAL_WAIT(pool->signalPtr);
		}
	}
#else
	ws = worksetPoolGet(pool);
#endif
	return ws;
}

/**
 * \cond
 * Retrieves a work set from the pool associated with the provider. In multi
 * threaded operation will always return a work set. In single threaded
 * operation may return NULL if no work sets are available in the pool.
 * The work set returned must be released back to the provider by calling
 * fiftyoneDegreesWorksetRelease when finished with.
 * @param provider pointer to the provider to return the work set from
 * @return pointer to a work set ready to be used for device detection
 * \endcond
 */
fiftyoneDegreesWorkset* fiftyoneDegreesProviderWorksetGet(
	fiftyoneDegreesProvider *provider) {
	fiftyoneDegreesWorkset *ws = NULL;
#ifndef FIFTYONEDEGREES_NO_THREADING
	// Loop until a work set is available from the active pool.
	// The providers lock and signal are used rather than the
	// pool as it's possible the pool will be replaced in another
	// thread during the execution of this method.
	while (ws == NULL) {
		FIFTYONEDEGREES_MUTEX_LOCK(&provider->lock);
		ws = worksetPoolGet((fiftyoneDegreesWorksetPool*)provider->activePool);
		FIFTYONEDEGREES_MUTEX_UNLOCK(&provider->lock);
		if (ws == NULL) {
			// Wait for a work set to return to the pool.
			FIFTYONEDEGREES_SIGNAL_WAIT(&provider->signal);
		}
	}
#else
	ws = worksetPoolGet((fiftyoneDegreesWorksetPool*)provider->activePool);
#endif
	return ws;
}

/**
 * \cond
 * Frees all worksets in the pool and releases all memory. Ensure all worksets
 * have been released back to the pool before calling this method.
 * @param pool pointer to the pool created by fiftyoneDegreesWorksetPoolCreate
 * \endcond
 */
void fiftyoneDegreesWorksetPoolFree(const fiftyoneDegreesWorksetPool *pool) {
	int i;
	for (i = 0; i < pool->available; i++) {
		fiftyoneDegreesWorksetFree(pool->worksets[i]);
	}
	fiftyoneDegreesFree((void*)pool->worksets);
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_SIGNAL_CLOSE(pool->signal);
	FIFTYONEDEGREES_MUTEX_CLOSE(pool->lock);
#endif
	fiftyoneDegreesFree((void*)pool);
}

/**
 * \cond
 * Frees the pool and it's associated cache and dataset if provided.
 * @param pool pointer to the pool created by fiftyoneDegreesWorksetPoolCreate
 * \endcond
 */
void fiftyoneDegreesWorksetPoolCacheDataSetFree(const fiftyoneDegreesWorksetPool *pool) {
	if (pool->cache != NULL) {
		fiftyoneDegreesResultsetCacheFree(pool->cache);
	}
	if (pool->dataSet != NULL) {
		fiftyoneDegreesDataSetFree(pool->dataSet);
		fiftyoneDegreesFree((void*)pool->dataSet);
	}
	fiftyoneDegreesWorksetPoolFree(pool);
}

 /**
 * \cond
 * RED BLACK BINARY TREE METHODS - USED WITH CACHE
 * \endcond
 */

/**
 * Implementation of a classic red black binary tree adapted to support the 
 * resultset structure used in the LRU cache. Several important considerations
 * should be noted with this implementation.
 * 
 * 1. The maximum number of entries in the tree is known when the tree is 
 *    created. All memory allocation is performed at initialisation.
 * 2. Once the tree is full it will remain full and never shrinks. The memory
 *    used is freed when the cache is freed.
 * 3. The node in the tree also contains other data such as the linked list
 *    pointers used to identify the first and last entry in the cache, and 
 *    the cache data itself. See structure fiftyoneDegreesResultset.
 * 4. The cache structure fiftyoneDegreesResultsetCache contains special 
 *    fields "empty" and "root". "Empty" is used inplace of NULL to indicate 
 *    that the left, right or parent pointer of the node has no data. The use 
 *    of "empty" makes the algorithm more efficient as the data structure used
 *    to indicate no data is the same as a valid data structure and therefore 
 *    does not require custom logic. The "root" fields left pointer is used as
 *    the start of the tree. Similarly the parent element being a valid data
 *    structure simplifies the algorithm.
 *
 * Developers modifying this section of code should be familiar with the red 
 * black tree design template. Code comments assume an understanding of the 
 * principles involved. For further information see:
 * https://en.wikipedia.org/wiki/Red%E2%80%93black_tree
 *
 * In future major versions of 51Degrees which can support breaking changes 
 * this code should be split out into separate files for easier maintenance. 
 */

 /**
 * Macros used in the binary tree.
 */

#define TREE_COLOUR_RED  1 // Indicates the resultset is black
#define TREE_COLOUR_BLACK 0 // Indicates the resultset is red

// Gets the root of the tree. The TREE_FIRST macro gets the first resultset.
#define TREE_ROOT(c) (fiftyoneDegreesResultset*)(&c->root) 

// Gets the empry resultset used to indicate no further data.
#define TREE_EMPTY(c) (fiftyoneDegreesResultset*)(&c->empty)

// Gets the first resultset under the root.
#define TREE_FIRST(c) ((fiftyoneDegreesResultset*)TREE_ROOT(c))->treeLeft 

 /**
 * \cond
 * Used by assert statements to validate the number of entries in the cache for
 * debugging should any changes be made to the logic. Should not be compiled in
 * release builds.
 * @param current pointer to the resultset being counted.
 * @returns the number of children plus 1 for this current resulset.
 * \endcond
 */
static int32_t resultsetCacheTreeCount(fiftyoneDegreesResultset *current) {
	int32_t count = 0;
	if (current != TREE_EMPTY(current->cache)) {
		count = 1;
		if (current->treeLeft != TREE_EMPTY(current->cache)) {
			count += resultsetCacheTreeCount(current->treeLeft);
			assert(current->targetUserAgentHashCode > current->treeLeft->targetUserAgentHashCode);
		}
		if (current->treeRight != TREE_EMPTY(current->cache)) {
			count += resultsetCacheTreeCount(current->treeRight);
			assert(current->targetUserAgentHashCode < current->treeRight->targetUserAgentHashCode);
		}
	}
	return count;
}

/**
* \cond
* Rotates the red black tree resultset to the left.
* @param rs pointer to the resultset being rotated.
* \endcond
*/
static void resultsetCacheTreeRotateLeft(fiftyoneDegreesResultset *rs) {
	fiftyoneDegreesResultset *child = rs->treeRight;
	rs->treeRight = child->treeLeft;

	if (child->treeLeft != TREE_EMPTY(rs->cache)) {
		child->treeLeft->treeParent = rs;
	}
	child->treeParent = rs->treeParent;

	if (rs == rs->treeParent->treeLeft) {
		rs->treeParent->treeLeft = child;
	}
	else {
		rs->treeParent->treeRight = child;
	}

	child->treeLeft = rs;
	rs->treeParent = child;
}

/**
* \cond
* Rotates the red black tree resultset to the right.
* @param rs pointer to the resultset being rotated.
* \endcond
*/
static void resultsetCacheTreeRotateRight(fiftyoneDegreesResultset *rs) {
	fiftyoneDegreesResultset *child = rs->treeLeft;
	rs->treeLeft = child->treeRight;

	if (child->treeRight != TREE_EMPTY(rs->cache)) {
		child->treeRight->treeParent = rs;
	}
	child->treeParent = rs->treeParent;

	if (rs == rs->treeParent->treeLeft) {
		rs->treeParent->treeLeft = child;
	}
	else {
		rs->treeParent->treeRight = child;
	}

	child->treeRight = rs;
	rs->treeParent = child;
}

/**
* \cond
* Maintains the properties of the binary tree following an insert.
* @param rs pointer to the resultset being repaired after insert.
* \endcond
*/
static void resultsetCacheTreeInsertRepair(fiftyoneDegreesResultset *rs) {
	fiftyoneDegreesResultset *uncle;

	while (rs->treeParent->colour == TREE_COLOUR_RED) {
		if (rs->treeParent == rs->treeParent->treeParent->treeLeft) {
			uncle = rs->treeParent->treeParent->treeRight;
			if (uncle->colour == TREE_COLOUR_RED) {
				rs->treeParent->colour = TREE_COLOUR_BLACK;
				uncle->colour = TREE_COLOUR_BLACK;
				rs->treeParent->treeParent->colour = TREE_COLOUR_RED;
				rs = rs->treeParent->treeParent;
			}
			else {
				if (rs == rs->treeParent->treeRight) {
					rs = rs->treeParent;
					resultsetCacheTreeRotateLeft(rs);
				}
				rs->treeParent->colour = TREE_COLOUR_BLACK;
				rs->treeParent->treeParent->colour = TREE_COLOUR_RED;
				resultsetCacheTreeRotateRight(rs->treeParent->treeParent);
			}
		}
		else {
			uncle = rs->treeParent->treeParent->treeLeft;
			if (uncle->colour == TREE_COLOUR_RED) {
				rs->treeParent->colour = TREE_COLOUR_BLACK;
				uncle->colour = TREE_COLOUR_BLACK;
				rs->treeParent->treeParent->colour = TREE_COLOUR_RED;
				rs = rs->treeParent->treeParent;
			}
			else {
				if (rs == rs->treeParent->treeLeft) {
					rs = rs->treeParent;
					resultsetCacheTreeRotateRight(rs);
				}
				rs->treeParent->colour = TREE_COLOUR_BLACK;
				rs->treeParent->treeParent->colour = TREE_COLOUR_RED;
				resultsetCacheTreeRotateLeft(rs->treeParent->treeParent);
			}
		}
	}
}

/**
* \cond
* Inserts the resultset into the red black tree.
* @param rs pointer to the resultset being inserted.
* \endcond
*/
static void resultsetCacheTreeInsert(const fiftyoneDegreesResultset *rs) {
	fiftyoneDegreesResultsetCache *cache = 
		(fiftyoneDegreesResultsetCache*)rs->cache;
	fiftyoneDegreesResultset *current = TREE_FIRST(cache);
	fiftyoneDegreesResultset *parent = TREE_ROOT(cache);
	

	// Work out the correct point to insert the resultset.
	while (current != TREE_EMPTY(cache)) {
		parent = current;
		assert(rs->targetUserAgentHashCode != current->targetUserAgentHashCode);
		current = rs->targetUserAgentHashCode < current->targetUserAgentHashCode
			? current->treeLeft
			: current->treeRight;
	}

	// Set up the resultset being inserted in the tree.
	current = (fiftyoneDegreesResultset*)rs;
	current->treeLeft = TREE_EMPTY(cache);
	current->treeRight = TREE_EMPTY(cache);
	current->treeParent = parent;
	if (parent == TREE_ROOT(cache) ||
		current->targetUserAgentHashCode < parent->targetUserAgentHashCode) {
		parent->treeLeft = current;
	}
	else {
		parent->treeRight = current;
	}
	current->colour = TREE_COLOUR_RED;

	resultsetCacheTreeInsertRepair(current);

	TREE_FIRST(cache)->colour = TREE_COLOUR_BLACK;
}

/**
* \cond
* Returns the resulset that matches the hash code of the workset provided.
* @param ws pointer initialised with the hash code required.
* @returns the corresponding resultset if it exists in the cache, otherwise 
* null.
* \endcond
*/
static fiftyoneDegreesResultset* resultsetCacheTreeFind(
	const fiftyoneDegreesWorkset *ws) {
	int32_t iterations = 0;
	fiftyoneDegreesResultset *current = TREE_ROOT(ws->cache)->treeLeft;

	while (current != TREE_EMPTY(ws->cache)) {
		iterations++;
		if (ws->targetUserAgentHashCode == current->targetUserAgentHashCode) {
			return current;
		}
		current = ws->targetUserAgentHashCode < current->targetUserAgentHashCode
			? current->treeLeft
			: current->treeRight;
	}

	if (iterations > ws->cache->maxIterations) {
		((fiftyoneDegreesResultsetCache*)ws->cache)->maxIterations = iterations;
	}

	return NULL;
}

/**
* \cond
* Finds the sucessor for the resultset provided.
* @param rs pointer to the resultset whose successor is required.
* @returns the successor for the resultset which may be empty.
* \endcond
*/
static fiftyoneDegreesResultset* resultsetCacheTreeSuccessor(
	fiftyoneDegreesResultset *rs) {
	const fiftyoneDegreesResultsetCache *cache = rs->cache;
	fiftyoneDegreesResultset *successor = rs->treeRight;
	if (successor != TREE_EMPTY(cache)) {
		while (successor->treeLeft != TREE_EMPTY(cache)) {
			successor = successor->treeLeft;
		}
	}
	else {
		for (successor = rs->treeParent;
			rs == successor->treeRight;
			successor = successor->treeParent) {
			rs = successor;
		}
		if (successor == TREE_ROOT(cache)) {
			successor = TREE_EMPTY(cache);
		}
	}
	return successor;
}

/**
* \cond
* Following a deletion repairs the section of the tree impacted.
* @param rs pointer to the resultset below the one deleted.
* \endcond
*/
static void resultsetCacheTreeDeleteRepair(fiftyoneDegreesResultset *rs) {
	const fiftyoneDegreesResultsetCache *cache = rs->cache;
	fiftyoneDegreesResultset *sibling;

	while (rs->colour == TREE_COLOUR_BLACK && rs != TREE_FIRST(cache)) {
		if (rs == rs->treeParent->treeLeft) {
			sibling = rs->treeParent->treeRight;
			if (sibling->colour == TREE_COLOUR_RED) {
				sibling->colour = TREE_COLOUR_BLACK;
				rs->treeParent->colour = TREE_COLOUR_RED;
				resultsetCacheTreeRotateLeft(rs->treeParent);
				sibling = rs->treeParent->treeRight;
			}
			if (sibling->treeRight->colour == TREE_COLOUR_BLACK &&
				sibling->treeLeft->colour == TREE_COLOUR_BLACK) {
				sibling->colour = TREE_COLOUR_RED;
				rs = rs->treeParent;
			}
			else {
				if (sibling->treeRight->colour == TREE_COLOUR_BLACK) {
					sibling->treeLeft->colour = TREE_COLOUR_BLACK;
					sibling->colour = TREE_COLOUR_RED;
					resultsetCacheTreeRotateRight(sibling);
					sibling = rs->treeParent->treeRight;
				}
				sibling->colour = rs->treeParent->colour;
				rs->treeParent->colour = TREE_COLOUR_BLACK;
				sibling->treeRight->colour = TREE_COLOUR_BLACK;
				resultsetCacheTreeRotateLeft(rs->treeParent);
				rs = TREE_FIRST(cache);
			}
		}
		else {
			sibling = rs->treeParent->treeLeft;
			if (sibling->colour == TREE_COLOUR_RED) {
				sibling->colour = TREE_COLOUR_BLACK;
				rs->treeParent->colour = TREE_COLOUR_RED;
				resultsetCacheTreeRotateRight(rs->treeParent);
				sibling = rs->treeParent->treeLeft;
			}
			if (sibling->treeRight->colour == TREE_COLOUR_BLACK &&
				sibling->treeLeft->colour == TREE_COLOUR_BLACK) {
				sibling->colour = TREE_COLOUR_RED;
				rs = rs->treeParent;
			}
			else {
				if (sibling->treeLeft->colour == TREE_COLOUR_BLACK) {
					sibling->treeRight->colour = TREE_COLOUR_BLACK;
					sibling->colour = TREE_COLOUR_RED;
					resultsetCacheTreeRotateLeft(sibling);
					sibling = rs->treeParent->treeLeft;
				}
				sibling->colour = rs->treeParent->colour;
				rs->treeParent->colour = TREE_COLOUR_BLACK;
				sibling->treeLeft->colour = TREE_COLOUR_BLACK;
				resultsetCacheTreeRotateRight(rs->treeParent);
				rs = TREE_FIRST(cache);
			}
		}
	}
	rs->colour = TREE_COLOUR_BLACK;
}

/**
* \cond
* Removes the resultset from the tree so that it can be used again to store
* another result. The resultset will come from the last item in the cache's
* linked list.
* @param rs pointer to be deleted.
* \endcond
*/
static void resultsetCacheTreeDelete(fiftyoneDegreesResultset *rs) {
	fiftyoneDegreesResultsetCache *cache = 
		(fiftyoneDegreesResultsetCache*)rs->cache;
	fiftyoneDegreesResultset *x, *y;

	if (rs->treeLeft == TREE_EMPTY(cache) ||
		rs->treeRight == TREE_EMPTY(cache)) {
		y = rs;
	}
	else {
		y = resultsetCacheTreeSuccessor(rs);
	}
	x = y->treeLeft == TREE_EMPTY(cache) ? y->treeRight : y->treeLeft;

	x->treeParent = y->treeParent;
	if (x->treeParent == TREE_ROOT(cache)) {
		TREE_FIRST(cache) = x;
	}
	else {
		if (y == y->treeParent->treeLeft) {
			y->treeParent->treeLeft = x;
		}
		else {
			y->treeParent->treeRight = x;
		}
	}

	if (y->colour == TREE_COLOUR_BLACK) {
		resultsetCacheTreeDeleteRepair(x);
	}
	if (y != rs) {
		y->treeLeft = rs->treeLeft;
		y->treeRight = rs->treeRight;
		y->treeParent = rs->treeParent;
		y->colour = rs->colour;
		rs->treeLeft->treeParent = y;
		rs->treeRight->treeParent = y;
		if (rs == rs->treeParent->treeLeft) {
			rs->treeParent->treeLeft = y;
		}
		else {
			rs->treeParent->treeRight = y;
		}
	}
}

/**
* \cond
* CACHE METHODS
* \endcond
*/

/**
* \cond
* Validates the cache by checking the number of entries in the linked list and 
* the tree. Used by assert statements to validate the integrity of the cache 
* during development. Should not be compiled in release builds.
* @param cache pointer to the cache being validated.
* @returns always return 0 as the purpose is to execute assets in debug builds.
* \endcond
*/
static int resultsetCacheValidate(const fiftyoneDegreesResultsetCache *cache) {
	int linkedListEntriesForward = 0;
	int linkedListEntriesBackwards = 0;
	int binaryTreeEntries = 0;
	fiftyoneDegreesResultset *node;
	
	// Check the list from first to last.
	node = cache->listFirst;
	while (node != NULL &&
		linkedListEntriesForward <= cache->allocated) {
		linkedListEntriesForward++;
		node = node->listNext;
	}
	assert(linkedListEntriesForward == cache->allocated ||
		   linkedListEntriesForward == cache->allocated - 1);

	// Check the list from last to first.
	node = cache->listLast;
	while (node != NULL &&
		linkedListEntriesBackwards <= cache->allocated) {
		linkedListEntriesBackwards++;
		node = node->listPrevious;
	}
	assert(linkedListEntriesBackwards == cache->allocated ||
		linkedListEntriesBackwards == cache->allocated - 1);

	// Check the binary tree. We need to remove one because the root
	// resultset doesn't contain any data.
	binaryTreeEntries = resultsetCacheTreeCount(TREE_FIRST(cache));
	assert(binaryTreeEntries == cache->allocated ||
		binaryTreeEntries == cache->allocated - 1);

	return 0;
}

/**
* \cond
* Copies the data associated with the source resultset to the destination.
* @param src source result set
* @param dst destination result set
* \endcond
*/
static void resultsetCacheCopy(fiftyoneDegreesResultset *dst, const fiftyoneDegreesResultset *src) {
	int profileIndex;
	dst->closestSignatures = src->closestSignatures;
	dst->difference = src->difference;
	dst->hashCodeSet = src->hashCodeSet;
	dst->method = src->method;
	dst->nodesEvaluated = src->nodesEvaluated;
	dst->rootNodesEvaluated = src->rootNodesEvaluated;
	dst->signaturesCompared = src->signaturesCompared;
	dst->signaturesRead = src->signaturesRead;
	dst->stringsRead = src->stringsRead;
	dst->targetUserAgentArrayLength = src->targetUserAgentArrayLength;
	dst->targetUserAgentHashCode = src->targetUserAgentHashCode;
	dst->signature = src->signature;
	memcpy((void*)dst->targetUserAgentArray, (void*)src->targetUserAgentArray, src->targetUserAgentArrayLength + 1);
	for (profileIndex = 0; profileIndex < src->profileCount; profileIndex++) {
		dst->profiles[profileIndex] = src->profiles[profileIndex];
	}
	dst->profileCount = src->profileCount;
}

/**
* \cond
* Sets the hash code for the result set provided if the hash code has not
* already been calculated. This only works because we know that the target
* user agent never changes once set.
* @param rs resultset whose target user agent hash code needs to be set.
* \endcond
*/
static void setResultsetHashCode(fiftyoneDegreesResultset *rs) {
	if (rs->hashCodeSet == 0) {
		rs->targetUserAgentHashCode = CityHash64((const char*)rs->targetUserAgentArray,
			rs->targetUserAgentArrayLength);
		rs->hashCodeSet = 1;
	}
}

 /**
 * \cond
 * Releases the memory used by the cache.
 * @param pointer to the cache created previously
 * \endcond
 */
void fiftyoneDegreesResultsetCacheFree(const fiftyoneDegreesResultsetCache *rsc) {
	if (rsc != NULL) {
#ifndef FIFTYONEDEGREES_NO_THREADING
		FIFTYONEDEGREES_MUTEX_CLOSE(rsc->lock);
#endif
		fiftyoneDegreesFree((void*)rsc->targetUserAgentArrays);
		fiftyoneDegreesFree((void*)rsc->profiles);
		fiftyoneDegreesFree((void*)rsc->resultSets);
		fiftyoneDegreesFree((void*)rsc);
	}
}

 /**
 * \cond
 * Initialises the cache by setting pointers for the linked list and binary 
 * tree.
 * @param rsc pointer to the cache to be initialised.
 * \endcond
 */
static void resultsetCacheInit(fiftyoneDegreesResultsetCache *cache) {
	int i;
	int targetUserAgentArraySize = 
		SIZE_OF_WORKSET_TARGET_USERAGENT_ARRAY(cache->dataSet->header);
	fiftyoneDegreesResultset *current = NULL;

	// Configure the empty not.
	current = TREE_EMPTY(cache);
	current->treeLeft = TREE_EMPTY(cache);
	current->treeRight = TREE_EMPTY(cache);
	current->treeParent = TREE_EMPTY(cache);
	current->colour = TREE_COLOUR_BLACK;
	current->targetUserAgentHashCode = 0;
	current->dataSet = cache->dataSet;
	current->cache = cache;

	// Configure the fake root node to avoid spliting the root.
	current = TREE_ROOT(cache);
	current->treeLeft = TREE_EMPTY(cache);
	current->treeRight = TREE_EMPTY(cache);
	current->treeParent = TREE_EMPTY(cache);
	current->colour = TREE_COLOUR_BLACK;
	current->targetUserAgentHashCode = 0;
	current->dataSet = cache->dataSet;
	current->cache = cache;

	// Set the pointers for storing target User-Agents and Profiles. The pointer
	// field is cast to remove the const. Setting the pointer is the only time
	// the operation is required as the memory space pointed to does not change
	// after initialisation.
	for (i = 0; i < cache->total; i++) {
		current = &((fiftyoneDegreesResultset*)cache->resultSets)[i];

		// Point to the data set and resultset cache which the item is part of.
		current->dataSet = cache->dataSet;
		current->cache = cache;

		// Point to the nth block of profiles memory.
		current->profiles = cache->profiles + (
				i * cache->dataSet->header.components.count);

		// Point to the nth block of target User-Agent memory.
		current->targetUserAgentArray = (byte*)(cache->targetUserAgentArrays + (
				i * targetUserAgentArraySize));
	}

	// Set the default values for an empty cache.
	cache->allocated = 0;
	cache->maxIterations = 0;
	cache->listFirst = NULL;
	cache->listLast = NULL;
}

/**
 * \cond
 * Creates a new cache used to speed up duplicate detections.
 * The cache must be destroyed with the fiftyoneDegreesFreeCache method.
 * If the cache size is lower then 2 then no cache is created.
 * @param dataSet pointer to the data set
 * @param size maximum number of items that the cache should store
 * @returns a pointer to the resultset cache created, or NULL
 * \endcond
 */
fiftyoneDegreesResultsetCache *fiftyoneDegreesResultsetCacheCreate(const fiftyoneDegreesDataSet *dataSet, int32_t size) {
	fiftyoneDegreesResultsetCache *cache = size >= MIN_CACHE_SIZE
		? (fiftyoneDegreesResultsetCache*)fiftyoneDegreesMalloc(SIZE_OF_CACHE)
		: NULL;

	if (cache != NULL) {
		cache->dataSet = dataSet;
		cache->hits = 0;
		cache->misses = 0;
		cache->switches = 0;
		cache->total = size;
		cache->allocated = 0;

		// Initialise the memory used for the list of result sets, profiles and 
		// target User-Agents.
		cache->profiles = (const fiftyoneDegreesProfile**)fiftyoneDegreesMalloc(SIZE_OF_CACHE_PROFILES(size, dataSet->header));
		cache->targetUserAgentArrays = (const byte*)fiftyoneDegreesMalloc(SIZE_OF_CACHE_TARGET_USER_AGENTS(size, dataSet->header));
		cache->resultSets = (const fiftyoneDegreesResultset*)fiftyoneDegreesMalloc(SIZE_OF_CACHE_RESULTSETS(size));

#ifndef FIFTYONEDEGREES_NO_THREADING
		FIFTYONEDEGREES_MUTEX_CREATE(cache->lock);
#endif

		// Check memory was allocated and if there was a problem free that which
		// was allocated.
		if (cache->resultSets == NULL ||
			cache->profiles == NULL ||
			cache->targetUserAgentArrays == NULL) {
			if (cache->resultSets != NULL) { fiftyoneDegreesFree((void*)cache->resultSets); }
			if (cache->profiles != NULL) { fiftyoneDegreesFree((void*)cache->profiles); }
			if (cache->targetUserAgentArrays != NULL) { fiftyoneDegreesFree((void*)cache->targetUserAgentArrays); }
			fiftyoneDegreesFree((void*)cache);
			cache = NULL;
		}
		else {
			// Initialise the linked lists and binary tree.
			resultsetCacheInit(cache);
		}
	}
	return cache;
}

/**
* \cond
* Returns the next free resultset from the cache which can be used to add a 
* new entry to. Once the cache is full then the resultset returned is the one
* at the end of the linked list which will contain the least recently used
* data.
* @param cache cache to return the next free resultset from.
* @returns a pointer to a free resultset.
* \endcond
*/
static fiftyoneDegreesResultset *resultsetCacheGetNextFree(
	fiftyoneDegreesResultsetCache *cache) {
	int countBefore, countAfter;
	fiftyoneDegreesResultset *last; // The oldest resultset in the cache.

	if (cache->allocated < cache->total) {
		// Return the free element at the end of the cache and update
		// the number of allocated elements.
		last = (fiftyoneDegreesResultset*)&cache->resultSets[cache->allocated++];
	}
	else {
		// Remove the last resultset from the linked list.
		last = cache->listLast;
		last->listPrevious->listNext = NULL;
		cache->listLast = last->listPrevious;

		// Remove the last result from the binary tree.
		countBefore = resultsetCacheTreeCount(TREE_FIRST(cache));
		resultsetCacheTreeDelete(last);
		countAfter = resultsetCacheTreeCount(TREE_FIRST(cache));
		assert(countBefore - 1 == countAfter);
	}

	// Set the pointers of the resultset to null indicating that the
	// entry is not part of the cache anymore.
	last->listNext = NULL;
	last->listPrevious = NULL;
	last->treeLeft = NULL;
	last->treeRight = NULL;
	last->treeParent = NULL;
	last->colour = 0;
	return last;
}

/**
* \cond
* Moves the resultset to the head of the linked list. Used when a resultset 
* which is already in the cache is being returned and the cache needs to know
* that is has recently been used and shouldn't be considered for removal. Also
* used when a new entry is added to the cache.
* @param rs pointer to the resultset to move to the head of the linked list.
* \endcond
*/
static void resultsetCacheMoveToHead(fiftyoneDegreesResultset *rs) {
	fiftyoneDegreesResultsetCache *cache = 
		(fiftyoneDegreesResultsetCache*)rs->cache;
	
	// Only consider moving if not already the first entry.
	if (cache->listFirst != rs) {

		// If the entry is the last one in the list then set the last pointer
		// to the entry before the last one.
		if (rs == cache->listLast) {
			cache->listLast = cache->listLast->listPrevious;
			cache->listLast->listNext = NULL;
		}

		// Remove the entry from the linked list.
		if (rs->listPrevious != NULL) {
			rs->listPrevious->listNext = rs->listNext;
		}
		if (rs->listNext != NULL) {
			rs->listNext->listPrevious = rs->listPrevious;
		}

		// Move to become the first entry. 
		if (rs->cache->listFirst != NULL) {
			rs->cache->listFirst->listPrevious = rs;
		}
		rs->listNext = cache->listFirst;
		cache->listFirst = rs;
		cache->listFirst->listPrevious = NULL;
	}

	// Validate the state of the list.
	assert(cache->listFirst == rs);
	assert(cache->listFirst->listPrevious == NULL);
}

/**
* \cond
* Updates the binary tree and the linked list with the provided resultset.
* @param rs pointer to the resultset to add to the cache.
* \endcond
*/
static void resultsetCacheAdd(fiftyoneDegreesResultset *rs) {
	fiftyoneDegreesResultsetCache *cache = 
		(fiftyoneDegreesResultsetCache*)rs->cache;

	assert(resultsetCacheValidate(cache) == 0);

	resultsetCacheTreeInsert(rs);

	assert(resultsetCacheValidate(cache) == 0);

	if (cache->listFirst == NULL) {
		cache->listLast = rs;
		cache->listFirst = rs;
	}
	else {
		resultsetCacheMoveToHead(rs);
	}

	assert(resultsetCacheValidate(cache) == 0);
}

/**
* \cond
* If the cache contains a resultset that matches the provided workset the 
* entry is returned. In addition to returning the entry the linked list in the
* cache is updated to indicate that the item returned has recently been 
* retrieved.
* @param ws pointer initialised with the hash code required.
* \endcond
*/
static fiftyoneDegreesResultset *resultsetCacheFetch(const fiftyoneDegreesWorkset *ws) {
	fiftyoneDegreesResultsetCache *cache = (fiftyoneDegreesResultsetCache*)ws->cache;
	fiftyoneDegreesResultset *found;

	assert(resultsetCacheValidate(cache) == 0);

	// Find the resultset that matches the value requested.
	found = resultsetCacheTreeFind(ws);

	// If the target is already in the cache move the entry to the head of the
	// linked list. No need to change the binary tree. Update the cache 
	// statistics for both misses and hits.
	if (found != NULL) {
		resultsetCacheMoveToHead(found);
		cache->hits++;
	}
	else {
		cache->misses++;
	}

	assert(resultsetCacheValidate(cache) == 0);
	
	// Return the found item even if there was a miss. This is the entry that the
	// results of the device detection that will follow if a miss occured will be
	// set in before the result set is added back into the cache.
	return found;
}

 /**
 * \cond
 * WORKSET METHODS
 * \endcond
 */

 /**
 * \cond
 * Creates a new workset to perform matches using the dataset provided.
 * The workset must be destroyed using the freeWorkset method when it's
 * finished with to release memory.
 * @param dataSet pointer to the data set
 * @param cache pointer or NULL if not used
 * @returns a pointer to the workset created
 * \endcond
 */
fiftyoneDegreesWorkset *fiftyoneDegreesWorksetCreate(
						const fiftyoneDegreesDataSet *dataSet,
						const fiftyoneDegreesResultsetCache *cache) {
	fiftyoneDegreesWorkset *ws =
		(fiftyoneDegreesWorkset*)fiftyoneDegreesMalloc(SIZE_OF_WORKSET);
	if (ws != NULL) {
		// Initialise all the parameters of the workset.
		ws->dataSet = dataSet;
		ws->cache = (fiftyoneDegreesResultsetCache*)cache;
		ws->associatedPool = NULL;

		// Allocate all the memory needed to the workset.
		ws->input = (char*)fiftyoneDegreesMalloc(SIZE_OF_WORKSET_INPUT(ws->dataSet->header));
		ws->linkedSignatureList.items = (fiftyoneDegreesLinkedSignatureListItem*)fiftyoneDegreesMalloc(SIZE_OF_WORKSET_SIGLIST_ITEMS(dataSet->header));
		ws->nodes = (const fiftyoneDegreesNode**)fiftyoneDegreesMalloc(SIZE_OF_WORKSET_NODES(dataSet->header));
		ws->orderedNodes = (const fiftyoneDegreesNode**)fiftyoneDegreesMalloc(SIZE_OF_WORKSET_NODES(dataSet->header));
		ws->relevantNodes = (char*)fiftyoneDegreesMalloc(SIZE_OF_WORKSET_USED_NODES(dataSet->header));
		ws->closestNodes = (char*)fiftyoneDegreesMalloc(SIZE_OF_WORKSET_USED_NODES(dataSet->header));
		ws->signatureAsString = (char*)fiftyoneDegreesMalloc(SIZE_OF_WORKSET_SIG_AS_STRING(dataSet->header));
		ws->values = (const fiftyoneDegreesValue**)fiftyoneDegreesMalloc(SIZE_OF_WORKSET_VALUES(dataSet->header));
		ws->profiles = (const fiftyoneDegreesProfile**)fiftyoneDegreesMalloc(SIZE_OF_WORKSET_PROFILES(dataSet->header));
		ws->tempProfiles = (const fiftyoneDegreesProfile**)fiftyoneDegreesMalloc(SIZE_OF_WORKSET_PROFILES(dataSet->header));
		ws->targetUserAgentArray = (byte*)fiftyoneDegreesMalloc(SIZE_OF_WORKSET_TARGET_USERAGENT_ARRAY(dataSet->header));
		ws->importantHeaders = (fiftyoneDegreesHttpHeaderWorkset*)fiftyoneDegreesMalloc(SIZE_OF_WORKSET_IMPORTANT_HEADERS(ws->dataSet->httpHeadersCount));

		// Check all the memory was allocated correctly and also
		// allocate using the result set method.
		if (ws->input == NULL ||
			ws->linkedSignatureList.items == NULL ||
			ws->nodes == NULL ||
			ws->orderedNodes == NULL ||
			ws->relevantNodes == NULL ||
			ws->closestNodes == NULL ||
			ws->signatureAsString == NULL ||
			ws->values == NULL ||
			ws->profiles == NULL ||
			ws->tempProfiles == NULL ||
			ws->targetUserAgentArray == NULL ||
			ws->importantHeaders == NULL) {

			// One or more of the workset memory allocations failed.
			// Free any that worked and return NULL.
			if (ws->input != NULL) { fiftyoneDegreesFree((void*)ws->input); }
			if (ws->linkedSignatureList.items != NULL) { fiftyoneDegreesFree((void*)ws->linkedSignatureList.items); }
			if (ws->nodes != NULL) { fiftyoneDegreesFree((void*)ws->nodes); }
			if (ws->orderedNodes != NULL) { fiftyoneDegreesFree((void*)ws->orderedNodes); }
			if (ws->relevantNodes != NULL) { fiftyoneDegreesFree((void*)ws->relevantNodes); }
			if (ws->closestNodes != NULL) { fiftyoneDegreesFree((void*)ws->closestNodes); }
			if (ws->signatureAsString != NULL) { fiftyoneDegreesFree((void*)ws->signatureAsString); }
			if (ws->values != NULL) { fiftyoneDegreesFree((void*)ws->values); }
			if (ws->profiles != NULL) { fiftyoneDegreesFree((void*)ws->profiles); }
			if (ws->tempProfiles != NULL) { fiftyoneDegreesFree((void*)ws->tempProfiles); }
			if (ws->targetUserAgentArray != NULL) { fiftyoneDegreesFree((void*)ws->targetUserAgentArray); }
			if (ws->importantHeaders != NULL) { fiftyoneDegreesFree((void*)ws->importantHeaders); }

			// Free the workset which worked earlier and return NULL.
			fiftyoneDegreesFree(ws);
			ws = NULL;
		}
		else {
			// Null terminate the strings used to return the relevant and closest nodes.
			ws->relevantNodes[dataSet->header.maxUserAgentLength] = 0;
			ws->closestNodes[dataSet->header.maxUserAgentLength] = 0;
		}
	}
	return ws;
}

 /**
 * \cond
 * Releases the memory used by the workset.
 *
 * If the workset is associated with a pool then the
 * fiftyOneDegreesWorksetRelease method should be used to return the workset
 * to the pool its associated with.
 *
 * @param pointer to the workset created previously
 * \endcond
 */
void fiftyoneDegreesWorksetFree(const fiftyoneDegreesWorkset *ws) {
	fiftyoneDegreesFree((void*)ws->input);
	fiftyoneDegreesFree((void*)ws->linkedSignatureList.items);
	fiftyoneDegreesFree((void*)ws->nodes);
	fiftyoneDegreesFree((void*)ws->orderedNodes);
	fiftyoneDegreesFree((void*)ws->relevantNodes);
	fiftyoneDegreesFree((void*)ws->closestNodes);
	fiftyoneDegreesFree((void*)ws->signatureAsString);
	fiftyoneDegreesFree((void*)ws->values);
	fiftyoneDegreesFree((void*)ws->profiles);
	fiftyoneDegreesFree((void*)ws->tempProfiles);
	fiftyoneDegreesFree((void*)ws->targetUserAgentArray);
	fiftyoneDegreesFree((void*)ws->importantHeaders);
	fiftyoneDegreesFree((void*)ws);
}

 /**
 * \cond
 * Adds the node of the signature into the signature string of the work set
 * @param ws pointer to the work set used for the match
 * @param node from the signature to be added to the string
 * @return the right most character returned
 * \endcond
 */
static int addSignatureNodeToString(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNode *node) {
	int nodeIndex, signatureIndex;
	const fiftyoneDegreesAsciiString *characters = fiftyoneDegreesGetString(ws->dataSet, node->characterStringOffset);
	for (nodeIndex = 0, signatureIndex = node->position + 1;
		nodeIndex < characters->length - 1;
		nodeIndex++, signatureIndex++) {
		ws->signatureAsString[signatureIndex] = (&(characters->firstByte))[nodeIndex];
	}
	return signatureIndex;
}

 /**
 * \cond
 * Sets the signature provided as the string returned by the match.
 * @param ws pointer to the work set used for the match
 * @param signature pointer to be used as the string for the result
 * \endcond
 */
static void setSignatureAsString(fiftyoneDegreesWorkset *ws, const byte *signature) {
	const int32_t *nodeOffsets = getNodeOffsetsFromSignature(ws->dataSet, signature);
	const int32_t nodeOffsetCount = getSignatureNodeOffsetsCount(ws->dataSet, signature);
	int index,
		nullPosition = 0,
		lastCharacter = 0;
	for (index = 0; index < ws->dataSet->header.maxUserAgentLength; index++) {
		ws->signatureAsString[index] = ' ';
	}
	for (index = 0; index < nodeOffsetCount; index++) {
		lastCharacter = addSignatureNodeToString(ws, getNodeByOffset(ws->dataSet, *(nodeOffsets + index)));
		if (lastCharacter > nullPosition) {
			nullPosition = lastCharacter;
		}
	}
	ws->signatureAsString[nullPosition] = 0;
}

 /**
 * \cond
 * Adds the node to the end of the workset.
 * @param ws pointer to the work set used for the match
 * @param node pointer to be added to the work set
 * @param index the should be added at
 * \endcond
 */
static void addNodeIntoWorkset(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNode *node, int32_t index) {
	ws->nodes[index] = node;
	ws->nodeCount++;
}

 /**
 * \cond
 * Inserts the node at the position provided moving all other nodes down.
 * @param ws pointer to the work set used for the match
 * @param node pointer to the node being added to the workset
 * @param insertIndex the index to insert the node at
 * \endcond
 */
static void insertNodeIntoWorksetAtIndex(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNode *node, int32_t insertIndex) {
	int32_t index;
	for (index = ws->nodeCount - 1; index >= insertIndex; index--) {
		*(ws->nodes + index + 1) = *(ws->nodes + index);
	}
	addNodeIntoWorkset(ws, node, insertIndex);
}

 /**
 * \cond
 * Inserts the node into the workset considering its position relative to other
 * nodes already set.
 * @param ws pointer to the work set used for the match
 * @param node pointer to be added to the work set
 * @return the index the node as added at
 * \endcond
 */
static int32_t insertNodeIntoWorkSet(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNode *node) {
	int32_t index;
	for (index = 0; index < ws->nodeCount; index++) {
		if (node > *(ws->nodes + index)) {
			// The node to be inserted is greater than the current node.
			// Insert the node before this one.
			insertNodeIntoWorksetAtIndex(ws, node, index);
			return index;
		}
	}
	addNodeIntoWorkset(ws, node, index);
	return index;
}

 /**
 * \cond
 * MATCH METHODS
 * \endcond
 */

 /**
 * \cond
 * Reset the counters for the workset.
 * @param ws pointer to the workset to be used for the match
 * \endcond
 */
static void resetCounters(fiftyoneDegreesWorkset *ws) {
	ws->difference = 0;
	ws->stringsRead = 0;
	ws->nodesEvaluated = 0;
	ws->rootNodesEvaluated = 0;
	ws->signaturesCompared = 0;
	ws->signaturesRead = 0;
}

 /**
 * \cond
 * Sets the target user agent and resets any other fields to initial values.
 * Must be called before a match commences.
 * @param ws pointer to the workset to be used for the match
 * @param userAgent char pointer to the user agent. Trimmed if longer than
 *        the maximum allowed for matching
 * \endcond
 */
static void setTargetUserAgentArray(fiftyoneDegreesWorkset *ws, const char* userAgent, int userAgentLength) {
	uint16_t index = 0;
	ws->hashCodeSet = 0;

	// If the user agent length is not positive then set it
	// to the maximum length a user agent can be.
	if (userAgentLength <= 0) {
		userAgentLength = ws->dataSet->header.maxUserAgentLength;
	}

	// Copy the characters of the user agent provided into the
	// target.
	if (userAgent != NULL) {

		while (userAgent[index] != 0 &&
			index < userAgentLength) {
			if (userAgent[index] != '\r' &&
				userAgent[index] != '\n') {
				// This is a valid character so add it to the target user agent
				// array.
				ws->targetUserAgentArray[index] = userAgent[index];
			}
			else {
				// Not a valid character so use space instead. Will only happen
				// when the user agent length is provided
				ws->targetUserAgentArray[index] = ' ';
			}
			index++;
		}
	}

	// Set the target user agent to the target user agent array in
	// order to deal with user agents that are not null terminated
	// and the length is provided.
	ws->targetUserAgent = (char*)ws->targetUserAgentArray;

	// Set the last index position to null incase the target user agent
	// is longer than the maximum user agent length. The routine to
	// allocate the targetUserAgentArray adds an extra byte for this
	// purpose.
	ws->targetUserAgentArray[index] = 0;
	ws->targetUserAgentArrayLength = index;

	/* Work out the starting character position */
	ws->nextCharacterPositionIndex = (int16_t)(ws->targetUserAgentArrayLength - 1);

	/* Check to ensure the length of the user agent does not exceed
	   the number of root nodes. */
	if (ws->nextCharacterPositionIndex >= ws->dataSet->header.rootNodes.count) {
		ws->nextCharacterPositionIndex = (int16_t)(ws->dataSet->header.rootNodes.count - 1);
	}

	/* Reset the nodes to zero ready for the new match */
	ws->nodeCount = 0;
	for (index = 0; index < ws->dataSet->header.signatureNodesCount; index++) {
		*(ws->nodes + index) = NULL;
		*(ws->orderedNodes + index) = NULL;
	}

	/* Reset the profiles to space ready for the new match */
	ws->profileCount = 0;
	for (index = 0; index < ws->dataSet->header.components.count; index++) {
		*(ws->profiles + index) = NULL;
	}

	/* Reset the closest and relevant strings */
	for (index = 0; index < ws->dataSet->header.maxUserAgentLength; index++) {
		ws->relevantNodes[index] = '_';
		ws->closestNodes[index] = ' ';
	}

	/* Reset the linked lists */
	ws->linkedSignatureList.count = 0;
	ws->linkedSignatureList.first = NULL;
	ws->linkedSignatureList.last = NULL;

	/* Reset the profiles and signatures */
	ws->profileCount = 0;
	ws->closestSignatures = 0;
	ws->signature = NULL;
}

 /**
 * \cond
 * Compares the characters which start at the start index of the target user
 * agent with the string provided and returns 0 if they're equal, -1 if
 * lower or 1 if higher.
 * @param targetUserAgentArray pointer to the target user agent
 * @param startIndex character position in the array to start comparing
 * @param string pointer to be compared with the target user agent
 * @return the difference between the characters, or 0 if equal
 * \endcond
 */
static int32_t compareTo(const byte* targetUserAgentArray, int32_t startIndex, fiftyoneDegreesString *string) {
	int32_t i, o, difference;
	for (i = string->length - 1, o = startIndex + string->length - 1; i >= 0; i--, o--)
	{
		difference = *(string->value + i) - *(targetUserAgentArray + o);
		if (difference != 0)
			return difference;
	}
	return 0;
}

 /**
 * \cond
 * Returns the next node after the one provided for the target user agent.
 * @param ws pointer being used for the match
 * @param node pointer of the current node
 * @return pointer to the next node to evaluate, or NULL if none match the
 *         target user agent
 * \endcond
 */
static const fiftyoneDegreesNode* getNextNode(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNode *node) {
	int32_t upper = node->childrenCount - 1, lower, middle, length, startIndex, comparisonResult;
	const fiftyoneDegreesNodeIndex *children;
	fiftyoneDegreesString string;

	if (upper >= 0)
	{
		children = getNodeIndexesForNode(node);
		lower = 0;
		middle = lower + (upper - lower) / 2;
		getCharactersForNodeIndex(ws, children + middle, &string);
		length = string.length;
		startIndex = node->position - length + 1;

		while (lower <= upper)
		{
			middle = lower + (upper - lower) / 2;

			/* Increase the number of strings checked. */
			if ((children + middle)->relatedNodeOffset < 0)
				ws->stringsRead++;

			/* Increase the number of nodes checked. */
			ws->nodesEvaluated++;

			getCharactersForNodeIndex(ws, children + middle, &string);
			comparisonResult = compareTo(
				ws->targetUserAgentArray,
				startIndex,
				&string);
			if (comparisonResult == 0)
				return getNodeFromNodeIndex(ws->dataSet, children + middle);
			else if (comparisonResult > 0)
				upper = middle - 1;
			else
				lower = middle + 1;
		}
	}

	return NULL;
}

 /**
 * \cond
 * Returns a leaf node if one is available, otherwise NULL.
 * @param ws pointer the workset used for the match
 * @param node pointer to the node whose children should be checked
 * @return a pointer the complete leaf node if one is available
 * \endcond
 */
static const fiftyoneDegreesNode* getCompleteNode(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNode *node) {
	const fiftyoneDegreesNode *nextNode = getNextNode(ws, node), *foundNode = NULL;
	if (nextNode != NULL) {
		foundNode = getCompleteNode(ws, nextNode);

	}
	if (foundNode == NULL && getIsNodeComplete(node))
		foundNode = node;
	return foundNode;
}

 /**
 * \cond
 * Compares the signature to the nodes of the match in the workset and if
 * they match exactly returns 0. If not -1 or 1 are returned depending on
 * whether the signature is lower or higher in the list.
 * @param ws pointer to the workset used for the match
 * @param signature pointer to the signature being tested
 * @return the difference between the signature and the matched nodes
 * \endcond
 */
static int32_t compareExact(const byte *signature, fiftyoneDegreesWorkset *ws) {
	int32_t index, nodeIndex, difference;
	const int32_t *nodeOffsets = getNodeOffsetsFromSignature(ws->dataSet, signature);
	const int32_t signatureNodeOffsetsCount = getSignatureNodeOffsetsCount(ws->dataSet, signature);
	int32_t length = signatureNodeOffsetsCount > ws->nodeCount ? ws->nodeCount : signatureNodeOffsetsCount;

	for (index = 0, nodeIndex = ws->nodeCount - 1; index < length; index++, nodeIndex--) {
		difference = *(nodeOffsets + index) -
			getNodeOffsetFromNode(ws->dataSet, *(ws->nodes + nodeIndex));
		if (difference != 0)
			return difference;
	}

	if (signatureNodeOffsetsCount < ws->nodeCount)
		return -1;
	if (signatureNodeOffsetsCount > ws->nodeCount)
		return 1;

	return 0;
}

 /**
 * \cond
 * Returns the index of the signature exactly associated with the matched
 * nodes or -1 if no such signature exists.
 * @param ws pointer to the workset used for the match
 * @return index of the signature matching the nodes or -1
 * \endcond
 */
static int32_t getExactSignatureIndex(fiftyoneDegreesWorkset *ws) {
	int32_t comparisonResult, middle, lower = 0, upper = ws->dataSet->header.signatures.count - 1;
	const byte *signature;

	while (lower <= upper)
	{
		middle = lower + (upper - lower) / 2;
		signature = getSignatureByIndex(ws->dataSet, middle);
		ws->signaturesRead++;
		comparisonResult = compareExact(signature, ws);
		if (comparisonResult == 0)
			return middle;
		else if (comparisonResult > 0)
			upper = middle - 1;
		else
			lower = middle + 1;
	}

	return -1;
}

 /**
 * \cond
 * Sets the characters of the node at the correct position in the output
 * array provided.
 * @param dataSet pointer to the dataset
 * @param node pointer to the node being added to the output
 * @return the position of the right most character set
 * \endcond
 */
static int32_t setNodeString(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesNode *node, char* output) {
	int32_t n, c, last = 0;
	const fiftyoneDegreesAsciiString *string = fiftyoneDegreesGetString(dataSet, node->characterStringOffset);
	for (n = 0, c = node->position + 1; n < string->length - 1; c++, n++) {
		*(output + c) = *(&string->firstByte + n);
		if (c > last)
			last = c;
	}
	return last;
}

 /**
 * \cond
 * Sets the characters of the nodes in the worksets relevant nodes pointer.
 * @param ws pointer to the workset being used for the match
 * \endcond
 */
static void setRelevantNodes(fiftyoneDegreesWorkset *ws) {
	int32_t index, lastPosition, last = -1;
	for (index = 0; index < ws->nodeCount; index++) {
		lastPosition = setNodeString(ws->dataSet,
			*(ws->nodes + index),
			ws->relevantNodes);
		if (lastPosition > last)
			last = lastPosition;
	}
	/* Null terminate the string */
	*(ws->relevantNodes + last + 1) = 0;
}

 /**
 * \cond
 * Sets the characters of the signature returned from the match.
 * @param ws pointer to the workset being used for the match
 * @param signature pointer to the signature being set for the match
 * \endcond
 */
static void setMatchSignature(fiftyoneDegreesWorkset *ws, const byte *signature) {
	int32_t index, lastPosition, last = 0, profileIndex;
	const int32_t *signatureNodeOffsets = getNodeOffsetsFromSignature(ws->dataSet, signature);
	const int32_t signatureNodeCount = getSignatureNodeOffsetsCount(ws->dataSet, signature);
	int32_t *signatureProfiles = getProfileOffsetsFromSignature(signature);

	ws->signature = (byte*)signature;
	ws->profileCount = 0;
	for (index = 0; index < ws->dataSet->header.signatureProfilesCount; index++) {
		profileIndex = *(signatureProfiles + index);
		if (profileIndex >= 0) {
			*(ws->profiles + index) = (fiftyoneDegreesProfile*)(ws->dataSet->profiles + profileIndex);
			ws->profileCount++;
		}
	}

	/* Set the closest nodes string from the signature found */
	for (index = 0; index < signatureNodeCount; index++) {
		lastPosition = setNodeString(ws->dataSet,
			(fiftyoneDegreesNode*)(ws->dataSet->nodes + *(signatureNodeOffsets + index)),
			ws->closestNodes);
		if (lastPosition > last)
			last = lastPosition;
	}
	*(ws->closestNodes + last + 1) = 0;
}

 /**
 * \cond
 * Sets the signature returned for the match.
 * @param ws pointer to the work set
 * @param signatureIndex of the signature being set for the match result
 * \endcond
 */
static void setMatchSignatureIndex(fiftyoneDegreesWorkset *ws, int32_t signatureIndex) {
	setMatchSignature(ws, getSignatureByIndex(ws->dataSet, signatureIndex));
}

 /**
 * \cond
 * If a match can't be found this function sets the default profiles for each
 * component type.
 * @param ws pointer to the work set
 * \endcond
 */
static void setMatchDefault(fiftyoneDegreesWorkset *ws) {
	int32_t index;
	int32_t profileOffset;

	ws->profileCount = 0;
	for (index = 0; index < ws->dataSet->header.components.count; index++) {
		profileOffset = ws->dataSet->components[index]->defaultProfileOffset;
		*(ws->profiles + index) = (fiftyoneDegreesProfile*)(ws->dataSet->profiles + profileOffset);
		ws->profileCount++;
	}

	/* Default the other values as no data available */
	ws->signature = NULL;
	*ws->signatureAsString = 0;
	*ws->closestNodes = 0;
}

 /**
 * \cond
 * Moves the next character position index to the right most character
 * position for evaluation.
 * @param ws pointer to the workset used for the match
 * \endcond
 */
static void resetNextCharacterPositionIndex(fiftyoneDegreesWorkset *ws) {
	ws->nextCharacterPositionIndex =
		ws->targetUserAgentArrayLength < ws->dataSet->header.rootNodes.count ?
		(int16_t)ws->targetUserAgentArrayLength - 1 :
		(int16_t)ws->dataSet->header.rootNodes.count - 1;
}

 /**
 * \cond
 * Returns the integer number at the start index of the array provided. The
 * characters at that position must have already been checked to ensure
 * they're numeric.
 * @param array pointer to the start of a byte array of characters
 * @param start index of the first character to convert to a number
 * @param length of the characters from start to conver to a number
 * @return the numeric integer of the characters specified
 * \endcond
 */
static int32_t getNumber(const byte *array, int32_t start, int32_t length) {
	int32_t i, p;
	int32_t value = 0;
	for (i = start + length - 1, p = 0; i >= start && p < (int32_t)(POWERS_COUNT); i--, p++)
	{
		value += POWERS[p] * (array[i] - (byte)'0');
	}
	return value;
}

 /**
 * \cond
 * Sets the stats target field to the numeric value of the current position in
 * the target user agent. If no numeric value is available the field remains
 * unaltered.
 * @param ws pointer to the work set used for the match
 * @param node pointer to the node being evaluated
 * @param state of the numeric node evaluation
 * @return the number of bytes from the target user agent needed to form a number
 * \endcond
 */
static int32_t getCurrentPositionAsNumeric(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNode *node, fiftyoneDegreesNumericNodeState *state) {
	// Find the left most numeric character from the current position.
	int32_t index = node->position;
	while (index >= 0 &&
		*(ws->targetUserAgentArray + index) >= (byte)'0' &&
		*(ws->targetUserAgentArray + index) <= (byte)'9')
		index--;

	// If numeric characters were found then return the number.
	if (index < node->position) {
		state->target = getNumber(
			ws->targetUserAgentArray,
			index + 1,
			node->position - index);
	}
	else {
		state->target = SHRT_MIN;
	}
	return node->position - index;
}

 /**
 * \cond
 * Gets the integer ranges for the target integer.
 * @param target whose range needs to be returned
 * @returns the range of values that relate to the target
 * \endcond
 */
static const fiftyoneDegreesRange* getRange(int target) {
	int32_t index;
	for (index = 0; index < (int32_t)(RANGES_COUNT); index++) {
		if (target >= RANGES[index].lower &&
			target < RANGES[index].upper) {
			return &RANGES[index];
		}
	}
	return &RANGES[RANGES_COUNT - 1];
}

 /**
 * \cond
 * Searches the numeric children using a binary search for the states target
 * provided. If a match can't be found the position to insert the target is
 * returned.
 * @param node pointer whose numeric children are to searched
 * @param state pointer for the numeric evaluation
 * @return the index of the target or twos compliment of insertion point
 * \endcond
 */
static int32_t binarySearchNumericChildren(const fiftyoneDegreesNode *node, fiftyoneDegreesNumericNodeState *state) {
	int16_t lower = 0, upper = node->numericChildrenCount - 1, middle, comparisonResult;

	while (lower <= upper)
	{
		middle = lower + (upper - lower) / 2;
		comparisonResult = (int16_t)((state->firstNodeNumericIndex + middle)->value - state->target);
		if (comparisonResult == 0)
			return middle;
		else if (comparisonResult > 0)
			upper = middle - 1;
		else
			lower = middle + 1;
	}

	return ~lower;
}

 /**
 * \cond
 * Configures the state ready to evaluate the numeric nodes in order of
 * difference from the target.
 * @param node pointer whose numeric indexes are to be evaluated
 * @param data pointer to the state settings used for the evaluation
 * \endcond
 */
static void setNumericNodeState(const fiftyoneDegreesNode *node, fiftyoneDegreesNumericNodeState *state) {
	if (state->target >= 0 && state->target <= SHRT_MAX) {
		state->range = getRange(state->target);
		state->node = node;
		state->firstNodeNumericIndex = getFirstNumericIndexForNode(node);

		// Get the index in the ordered list to start at.
		state->startIndex = binarySearchNumericChildren(node, state);
		if (state->startIndex < 0)
			state->startIndex = ~state->startIndex - 1;
		state->lowIndex = state->startIndex;
		state->highIndex = state->startIndex + 1;

		// Determine if the low and high indexes are in range.
		state->lowInRange = state->lowIndex >= 0 && state->lowIndex < node->numericChildrenCount &&
			(state->firstNodeNumericIndex + state->lowIndex)->value >= state->range->lower &&
			(state->firstNodeNumericIndex + state->lowIndex)->value < state->range->upper;
		state->highInRange = state->highIndex < node->numericChildrenCount && state->highIndex >= 0 &&
			(state->firstNodeNumericIndex + state->highIndex)->value >= state->range->lower &&
			(state->firstNodeNumericIndex + state->highIndex)->value < state->range->upper;
	}
	else
	{
		// As the target is out of valid ranges then
		// set the low and high in range to false
		// to ensure no iteration happens.
		state->lowInRange = 0;
		state->highInRange = 0;
	}
}

 /**
 * \cond
 * The iterator function used to return the next numeric node index for
 * the evaluation for the state provided.
 * @param state settings for the evalution
 * @return NULL if no more indexes are available, or the next index
 * \endcond
 */
static const fiftyoneDegreesNodeNumericIndex* getNextNumericNode(fiftyoneDegreesNumericNodeState *state) {
	int32_t lowDifference, highDifference;
	const fiftyoneDegreesNodeNumericIndex *nodeNumericIndex = NULL;

	if (state->lowInRange && state->highInRange)
	{
		// Get the differences between the two values.
		lowDifference = abs((state->firstNodeNumericIndex + state->lowIndex)->value - state->target);
		highDifference = abs((state->firstNodeNumericIndex + state->highIndex)->value - state->target);

		// Favour the lowest value where the differences are equal.
		if (lowDifference <= highDifference)
		{
			nodeNumericIndex = (state->firstNodeNumericIndex + state->lowIndex);

			// Move to the next low index.
			state->lowIndex--;
			state->lowInRange = state->lowIndex >= 0 &&
				(state->firstNodeNumericIndex + state->lowIndex)->value >= state->range->lower &&
				(state->firstNodeNumericIndex + state->lowIndex)->value < state->range->upper;
		}
		else
		{
			nodeNumericIndex = (state->firstNodeNumericIndex + state->highIndex);

			// Move to the next high index.
			state->highIndex++;
			state->highInRange = state->highIndex < state->node->numericChildrenCount &&
				(state->firstNodeNumericIndex + state->highIndex)->value >= state->range->lower &&
				(state->firstNodeNumericIndex + state->highIndex)->value < state->range->upper;
		}
	}
	else if (state->lowInRange)
	{
		nodeNumericIndex = (state->firstNodeNumericIndex + state->lowIndex);

		// Move to the next low index.
		state->lowIndex--;
		state->lowInRange = state->lowIndex >= 0 &&
			(state->firstNodeNumericIndex + state->lowIndex)->value >= state->range->lower &&
			(state->firstNodeNumericIndex + state->lowIndex)->value < state->range->upper;
	}
	else if (state->highInRange)
	{
		nodeNumericIndex = (state->firstNodeNumericIndex + state->highIndex);

		// Move to the next high index.
		state->highIndex++;
		state->highInRange = state->highIndex < state->node->numericChildrenCount &&
			(state->firstNodeNumericIndex + state->highIndex)->value >= state->range->lower &&
			(state->firstNodeNumericIndex + state->highIndex)->value < state->range->upper;
	}

	return nodeNumericIndex;
}

 /**
 * \cond
 * Gets a complete numeric leaf node for the node provided if one exists.
 * @param ws pointer to the work set used for the match
 * @param node pointer to be evaluated against the current position
 * @return pointer to the complete node found, or NULL if no node exists
 * \endcond
 */
static const fiftyoneDegreesNode* getCompleteNumericNode(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNode *node) {
	const fiftyoneDegreesNode *foundNode = NULL, *numericNode, *nextNode = getNextNode(ws, node);
	const fiftyoneDegreesNodeNumericIndex *nodeNumericIndex;
	int32_t difference;
	fiftyoneDegreesNumericNodeState state;

	// Get the next child node that matches the target User-Agent
	// exactly.
	if (nextNode != NULL) {
		// An exact matching child node was found. Evaluate it for
		// a numeric child. 
		foundNode = getCompleteNumericNode(ws, nextNode);
    }

	if (foundNode == NULL && node->numericChildrenCount > 0)
	{
		// There is either no exact matching normal child or there are
		// no children that generated a numeric match. This node does 
		// have numeric children that should be evaluated. Get the 
		// numeric value of the current position from the target 
		// User-Agent.
		if (getCurrentPositionAsNumeric(ws, node, &state) > 0 &&
			state.target >= 0)
		{
			// Return the numeric nodes in the ascending order from 
			// the target value. i.e. if the target is 10 and the 
			// numeric nodes are ordered 5, 8, 11, 15 they would be
			// provided in the order 11, 8, 5, 15 by the enumerator.
			setNumericNodeState(node, &state);
			nodeNumericIndex = getNextNumericNode(&state);
			while (nodeNumericIndex != NULL)
			{
                numericNode = getNodeByOffset(
					ws->dataSet, 
					abs(nodeNumericIndex->relatedNodeOffset));

				// Evaluate the node from the enumerator if it has
				// not already been evaluated earlier in the method
				// when it was found as the nextNode.
				if (nextNode == NULL || numericNode != nextNode) {

					// Check if there is a complete numeric node under
					// this node. If there is then calculate and record
					// the difference value before returning it.
					foundNode = getCompleteNumericNode(ws, numericNode);
					if (foundNode != NULL)
					{
						difference = abs(state.target - nodeNumericIndex->value);
						ws->difference += difference;
						break;
					}
				}
				nodeNumericIndex = getNextNumericNode(&state);
			}
		}
	}

	// If no suitable child node could be found and this node is a 
	// complete node then return this node.
	if (foundNode == NULL && getIsNodeComplete(node)) {
		foundNode = node;
	}
	return foundNode;
}

 /**
 * \cond
 * Passed two nodes and determines if their character positions overlap
 * @param a the first node to test
 * @param b the second node to test
 * @return true if they overlap, otherwise false
 * \endcond
 */
static byte areNodesOverlapped(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesNode *a, const fiftyoneDegreesNode *b) {
	const fiftyoneDegreesNode *lower = a->position < b->position ? a : b,
		*higher = lower == b ? a : b,
		*rootNode = getRootNode(dataSet, lower);

	if (lower->position == higher->position ||
		rootNode->position > higher->position) {
		return 1;
	}

	return 0;
}

 /**
 * \cond
 * Determines if the node overlaps any nodes already in the workset.
 * @param node pointer to be checked
 * @param ws pointer to the work set used for the match
 * @return 1 if the node overlaps, otherwise 0
 * \endcond
 */
static byte isNodeOverlapped(const fiftyoneDegreesNode *node, fiftyoneDegreesWorkset *ws) {
	const fiftyoneDegreesNode  *currentNode;
	int index;
	for (index = ws->nodeCount - 1; index >= 0; index--) {
		currentNode = *(ws->nodes + index);
		if (currentNode == node ||
			areNodesOverlapped(ws->dataSet, node, currentNode) == 1) {
			return 1;
		}
	}
	return 0;
}

 /**
 * \cond
 * Evaluates the target user agent for a numeric match.
 * @param ws pointer to the work set being used for the match
 * \endcond
 */
static void evaluateNumeric(fiftyoneDegreesWorkset *ws) {
	int32_t existingNodeIndex = 0;
	const fiftyoneDegreesNode *node;
	resetNextCharacterPositionIndex(ws);
	ws->difference = 0;
	while (ws->nextCharacterPositionIndex > 0 &&
		ws->nodeCount < ws->dataSet->header.signatureNodesCount)
	{
		if (existingNodeIndex >= ws->nodeCount ||
			getRootNode(ws->dataSet, ws->nodes[existingNodeIndex])->position < ws->nextCharacterPositionIndex)
		{
			ws->rootNodesEvaluated++;
			node = getCompleteNumericNode(ws, ws->dataSet->rootNodes[ws->nextCharacterPositionIndex]);
			if (node != NULL &&
				isNodeOverlapped(node, ws) == 0)
			{
				// Insert the node and update the existing index so that
				// it's the node to the left of this one.
				existingNodeIndex = insertNodeIntoWorkSet(ws, node) + 1;

				// Move to the position of the node found as
				// we can't use the next node incase there's another
				// not part of the same signatures closer.
				ws->nextCharacterPositionIndex = node->position;
			}
			else {
				ws->nextCharacterPositionIndex--;
			}
		}
		else
		{
			// The next position to evaluate should be to the left
			// of the existing node already in the list.
			ws->nextCharacterPositionIndex = ws->nodes[existingNodeIndex]->position;

			// Swap the existing node for the next one in the list.
			existingNodeIndex++;
		}
	}
}

 /**
 * \cond
 * Enumerates the nodes signature indexes updating the frequency counts in the
 * linked list for any that exist already, adding those that still stand a chance
 * of making the count, or removing those that aren't necessary.
 * @param ws pointer to the work set used for the match
 * @param node pointer to the node whose signatures are to be added
 * @param count the current count that is needed to be considered
 * @param iteration the iteration that we're currently in
 * @return the count after the node has been evaluated
 * \endcond
 */
static int32_t setClosestSignaturesForNode(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNode *node, int32_t count, int32_t iteration) {
	byte thresholdReached = (ws->nodeCount - iteration) < count;
	fiftyoneDegreesLinkedSignatureList *linkedList = &(ws->linkedSignatureList);
	fiftyoneDegreesLinkedSignatureListItem *current = linkedList->first,
		*next;
	int32_t index = 0;
	// If there is only 1 signature then the value is the
	// ranked signature index, otherwise it's the index of the first
	// ranked index in the nodeRankedSignatureIndexes array.
	const int32_t *firstRankedSignatureIndex =
		node->signatureCount == 1 ?
		getFirstRankedSignatureIndexForNode(node) :
		ws->dataSet->nodeRankedSignatureIndexes +
		*getFirstRankedSignatureIndexForNode(node);
	const int32_t *currentRankedSignatureIndex;

	while (index < node->signatureCount &&
		current != NULL)
	{
		currentRankedSignatureIndex = firstRankedSignatureIndex + index;
		if (current->rankedSignatureIndex > *currentRankedSignatureIndex)
		{
			// The base list is higher than the target list. Add the element
			// from the target list and move to the next element in each.
			if (thresholdReached == 0)
				linkedListAddBefore(linkedList, current, *currentRankedSignatureIndex);
			index++;
		}
		else if (current->rankedSignatureIndex < *currentRankedSignatureIndex)
		{
			if (thresholdReached)
			{
				// Threshold reached so we can removed this item
				// from the list as it's not relevant.
				next = current->next;
				if (current->frequency < count)
					linkedListRemove(linkedList, current);
				current = next;
			}
			else
			{
				current = current->next;
			}
		}
		else
		{
			// They're the same so increase the frequency and move to the next
			// element in each.
			current->frequency++;
			if (current->frequency > count)
				count = current->frequency;
			index++;
			current = current->next;
		}
	}
	if (thresholdReached == 0)
	{
		// Add any signature indexes higher than the base list to the base list.
		while (index < node->signatureCount)
		{
			linkedListAdd(linkedList, *(firstRankedSignatureIndex + index));
			index++;
		}
	}
	return count;
}

 /**
 * \cond
 * Orders the nodes so that those with the lowest rank are are the top of the
 * list.
 * @param ws pointer to the work set being used for the match
 * @param count frequency below which signatures should be removed
 * \endcond
 */
static void setClosestSignaturesFinal(fiftyoneDegreesWorkset *ws, int32_t count) {
	fiftyoneDegreesLinkedSignatureList *linkedList = &(ws->linkedSignatureList);
	fiftyoneDegreesLinkedSignatureListItem *current = linkedList->first;

	/* First iteration to remove any items with lower than the maxcount. */
	while (current != NULL) {
		if (current->frequency < count) {
			linkedListRemove(linkedList, current);
		}
		current = current->next;
	}
}

 /**
 * \cond
 * Compares the signature counts of two nodes for the purposes of ordering
 * them in ascending order of count.
 * @param a pointer to the first node
 * @param b pointer to the second node
 * @return the difference between the nodes
 * \endcond
 */
static int FIFTYONEDEGREES_CALL_CONV nodeSignatureCountCompare(const void *a, const void *b) {
	fiftyoneDegreesNode* c1 = (*(fiftyoneDegreesNode**)a);
	fiftyoneDegreesNode* c2 = (*(fiftyoneDegreesNode**)b);
	int difference = c1->signatureCount - c2->signatureCount;
	if (difference == 0) {
		difference = c1->position - c2->position;
	}
	return difference;
}

 /**
 * \cond
 * Orders the nodes found in ascending order of signature count.
 * @param ws pointer to the work set being used for the match
 * \endcond
 */
static void orderNodesOnSignatureCount(fiftyoneDegreesWorkset *ws) {
	int32_t nodeIndex;
	for (nodeIndex = 0; nodeIndex < ws->nodeCount; nodeIndex++) {
		ws->orderedNodes[nodeIndex] = ws->nodes[nodeIndex];
	}
	qsort((void*)ws->orderedNodes,
		(size_t)ws->nodeCount,
		sizeof(fiftyoneDegreesNode*),
		nodeSignatureCountCompare);
}

 /**
 * \cond
 * Fills the workset with those signatures that are closest to the target user
 * agent. This is required before the NEAREST and CLOSEST methods are used to
 * determine which signature is in fact closest.
 * @param ws pointer to the work set being used for the match
 * \endcond
 */
static void fillClosestSignatures(fiftyoneDegreesWorkset *ws) {
	int32_t iteration = 2, maxCount = 1, nodeIndex = 1;
	const fiftyoneDegreesNode  *node;

	if (ws->nodeCount == 1) {
		// No need to create a linked list as only 1 node.
		ws->closestSignatures = ws->nodes[0]->signatureCount;
	}
	else {
		// Order the nodes in ascending order of signature index length.
		orderNodesOnSignatureCount(ws);

		// Get the first node and add all the signature indexes.
		node = *(ws->orderedNodes);
		buildInitialList(ws, node);

		// Count the number of times each signature index occurs.
		while (nodeIndex < ws->nodeCount)
		{
			node = *(ws->orderedNodes + nodeIndex);
			maxCount = setClosestSignaturesForNode(
				ws, node, maxCount, iteration);
			iteration++;
			nodeIndex++;
		}

		// Remove any signatures under the max count and order the
		// remainder in ascending order of Rank.
		setClosestSignaturesFinal(ws, maxCount);

		ws->closestSignatures = ws->linkedSignatureList.count;
	}
}

 /**
 * \cond
 * Determines if a byte value is a numeric character.
 * @param value to be checked
 * @return 1 if the value is numeric, otherwise 0
 * \endcond
 */
static byte getIsNumeric(const byte *value) {
	return (*value >= (char)'0' && *value <= (char)'9');
}

 /**
 * \cond
 * Works out the numeric difference between the current position and the target.
 * @param characters pointer to the ascii string of the node
 * @param ws pointer to the work set used for the match
 * @param nodeIndex pointer to the index in the node being evaluated
 * @param targetIndex pointer to the target user agent index
 * @return the absolute difference between the characters or 0 if not numeric
 * \endcond
 */
static int32_t calculateNumericDifference(const fiftyoneDegreesAsciiString *characters, fiftyoneDegreesWorkset *ws, int32_t *nodeIndex, int32_t *targetIndex) {
	// Move right when the characters are numeric to ensure
	// the full number is considered in the difference comparison.
	int32_t newNodeIndex = *nodeIndex + 1;
	int32_t newTargetIndex = *targetIndex + 1;
	int16_t count = 0;
	while (newNodeIndex < characters->length &&
		newTargetIndex < ws->targetUserAgentArrayLength &&
		getIsNumeric(ws->targetUserAgentArray + newTargetIndex) &&
		getIsNumeric((byte*)&(characters->firstByte) + newNodeIndex))
	{
		newNodeIndex++;
		newTargetIndex++;
	}
	(*nodeIndex) = newNodeIndex - 1;
	(*targetIndex) = newTargetIndex - 1;

	// Find when the characters stop being numbers.
	while (
		(int64_t) nodeIndex >= 0 &&
		getIsNumeric(ws->targetUserAgentArray + *targetIndex) &&
		getIsNumeric((byte*)&(characters->firstByte) + *nodeIndex))
	{
		(*nodeIndex)--;
		(*targetIndex)--;
		count++;
	}

	// If there is more than one character that isn't a number then
	// compare the numeric values.
	if (count > 1)
	{
		return abs(
			getNumber(ws->targetUserAgentArray, *targetIndex + 1, count) -
			getNumber((byte*)&(characters->firstByte), *nodeIndex + 1, count));
	}

	return 0;
}

 /**
 * \cond
 * The index of the nodes characters in the target user agent.
 * @param ws pointer to the work set used for the match
 * @param node pointer to the node to be checked in the target
 * @return index in the target user agent of the nodes characters, or -1 if
 *         not present
 * \endcond
 */
static int16_t matchIndexOf(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNode *node)
{
	const fiftyoneDegreesAsciiString *characters = fiftyoneDegreesGetString(ws->dataSet, node->characterStringOffset);
	int16_t index,
		nodeIndex,
		targetIndex,
		finalNodeIndex = characters->length - 2,
		charactersLength = characters->length - 1;

	for (index = 0; index < ws->targetUserAgentArrayLength - charactersLength; index++)
	{
		for (nodeIndex = 0, targetIndex = index;
			nodeIndex < charactersLength && targetIndex < ws->targetUserAgentArrayLength;
			nodeIndex++, targetIndex++)
		{
			if (*(&(characters->firstByte) + nodeIndex) != *(ws->targetUserAgent + targetIndex)) {
				break;
			}
			else if (nodeIndex == finalNodeIndex) {
				return index;
			}
		}
	}
	return -1;
}

 /**
 * \cond
 * Used to determine if the node is in the target user agent and if so how
 * many character positions different it is.
 * @param ws pointer to the workset used for the match
 * @param node pointer to be checked against the target user agent
 * @return the difference in character positions or -1 if not present.
 * \endcond
 */
static int32_t getScoreNearest(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNode *node) {
	int16_t index = matchIndexOf(ws, node);
	if (index >= 0) {
		return abs(node->position + 1 - index);
	}

	// Return -1 to indicate that a score could not be calculated.
	return -1;
}

 /**
 * \cond
 * Compares the node against the target user agent on a character by characters
 * basis.
 * @param ws pointer to the work set used for the current match
 * @param node pointer to the node being compared
 * @return the difference based on character comparisons between the two
 * \endcond
 */
static int32_t getScoreClosest(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesNode *node) {
	const fiftyoneDegreesAsciiString *characters = getNodeCharacters(ws->dataSet, node);
	int32_t score = 0;
	int32_t difference;
	int32_t numericDifference;
	int32_t nodeIndex = characters->length - 2;
	int32_t targetIndex = node->position + characters->length - 1;

	// Adjust the score and indexes if the node is too long.
	if (targetIndex >= ws->targetUserAgentArrayLength) {
		score = targetIndex - ws->targetUserAgentArrayLength;
		nodeIndex -= score;
		targetIndex = ws->targetUserAgentArrayLength - 1;
	}

	while (nodeIndex >= 0 && score < ws->difference) {
		difference = abs(
			*(ws->targetUserAgentArray + targetIndex) -
			*(&(characters->firstByte) + nodeIndex));
		if (difference != 0) {
			numericDifference = calculateNumericDifference(
				characters,
				ws,
				&nodeIndex,
				&targetIndex);
			if (numericDifference != 0)
				score += numericDifference;
			else
				score += (difference * 10);
		}
		nodeIndex--;
		targetIndex--;
	}

	return score;
}

 /**
 * \cond
 * Calculates the score for difference score for the signature.
 * @param ws pointer to the workset used for the match
 * @param signature pointer to the signature to be compared
 * @param lastNodeCharacter position of the right most character in the
 *        signature
 * @return the difference score using the active scoring method between the
 *         signature and the target user agent
 * \endcond
 */
static int32_t getScore(fiftyoneDegreesWorkset *ws,
	const byte *signature,
	int16_t lastNodeCharacter) {
	const int32_t *nodeOffsets = getNodeOffsetsFromSignature(ws->dataSet, signature);
	const int32_t count = getSignatureNodeOffsetsCount(ws->dataSet, signature);
	int32_t runningScore = ws->startWithInitialScore == 1 ?
		abs(lastNodeCharacter + 1 - getSignatureLengthFromNodeOffsets(ws->dataSet, *(nodeOffsets + count - 1))) :
		0,
		matchNodeIndex = ws->nodeCount - 1,
		signatureNodeIndex = 0,
		matchNodeOffset,
		signatureNodeOffset,
		score;

	while (signatureNodeIndex < count &&
		runningScore < ws->difference)
	{
		matchNodeOffset = matchNodeIndex < 0 ?
		INT32_MAX :
				  getNodeOffsetFromNode(ws->dataSet, *(ws->nodes + matchNodeIndex));
		signatureNodeOffset = *(nodeOffsets + signatureNodeIndex);
		if (matchNodeOffset > signatureNodeOffset)
		{
			// The matched node is either not available, or is higher than
			// the current signature node. The signature node is not contained
			// in the match so we must score it.
			score = ws->functionPtrGetScore(ws, getNodeByOffset(ws->dataSet, signatureNodeOffset));

			// If the score is less than zero then a score could not be
			// determined and the signature can't be compared to the target
			// user agent. Exit with a high score.
			if (score < 0)
				return INT32_MAX;
			runningScore += score;
			signatureNodeIndex++;
		}
		else if (matchNodeOffset == signatureNodeOffset)
		{
			// They both are the same so move to the next node in each.
			matchNodeIndex--;
			signatureNodeIndex++;
		}
		else if (matchNodeOffset < signatureNodeOffset)
		{
			// The match node is lower so move to the next one and see if
			// it's higher or equal to the current signature node.
			matchNodeIndex--;
		}
	}

	return runningScore;
}

 /**
 * \cond
 * Evaluates a signature using the active scoring method.
 * @param ws pointer to the work set used for the match
 * @param signature pointer to be compared to the target user agent
 * @param lastNodeCharacter position of the right most node character in the
 *        signature
 * \endcond
 */
static void evaluateSignature(fiftyoneDegreesWorkset *ws,
	const byte *signature,
	int16_t lastNodeCharacter) {

	// Get the score between the target and the signature stopping if it's
	// going to be larger than the lowest score already found.
	int32_t score = getScore(ws, signature, lastNodeCharacter);

	ws->signaturesCompared++;

	// If the score is lower than the current lowest then use this signature.
	if (score < ws->difference)
	{
		ws->difference = score;
		ws->signature = (byte*)signature;
	}
}

 /**
 * \cond
 * Gets the next signature to be examined for the single node found.
 * @param ws the current work set for the match.
 * @return null if there are no more signature pointers.
 * \endcond
 */
static const byte* getNextClosestSignatureForSingleNode(fiftyoneDegreesWorkset *ws) {
	const byte *signature;
	int32_t rankedSignatureIndex;
	if (ws->closestNodeRankedSignatureIndex < ws->nodes[0]->signatureCount) {
		rankedSignatureIndex = *getFirstRankedSignatureIndexForNode(ws->nodes[0]);
		// If there is a count greater than 1 then the value relates
		// to the first index in the list of node ranked signature
		// indexes. If the count is 1 then the value is the index.
		if (ws->nodes[0]->signatureCount > 1) {
			rankedSignatureIndex = *(ws->dataSet->nodeRankedSignatureIndexes +
				rankedSignatureIndex + ws->closestNodeRankedSignatureIndex);
		}
		signature = getSignatureByRankedIndex(
			ws->dataSet,
			rankedSignatureIndex);
		ws->closestNodeRankedSignatureIndex++;
	}
	else {
		signature = NULL;
	}
	return signature;
}

 /**
 * \cond
 * Gets the next signature to be examined from the linked list.
 * @param ws the current work set for the match.
 * @return null if there are no more signature pointers.
 * \endcond
 */
static const byte* getNextClosestSignatureForLinkedList(fiftyoneDegreesWorkset *ws) {
	const byte *signature;
	if (ws->linkedSignatureList.current != NULL) {
		signature = getSignatureByRankedIndex(ws->dataSet,
			ws->linkedSignatureList.current->rankedSignatureIndex);
		ws->linkedSignatureList.current = ws->linkedSignatureList.current->next;
	}
	else {
		signature = NULL;
	}
	return signature;
}

 /**
 * \cond
 * Resets the work set ready to return the closest signatures.
 * @param ws the current work set for the match.
 * \endcond
 */
static void resetClosestSignatureEnumeration(fiftyoneDegreesWorkset *ws) {
	if (ws->nodeCount == 1) {
		/* There's only 1 node so just iterate through it's indexes */
		ws->closestNodeRankedSignatureIndex = 0;
		ws->functionPtrNextClosestSignature = &getNextClosestSignatureForSingleNode;
	}
	else if (ws->nodeCount > 1) {
		/* We'll need to walk the linked list to get the signatures */
		ws->linkedSignatureList.current = ws->linkedSignatureList.first;
		ws->functionPtrNextClosestSignature = &getNextClosestSignatureForLinkedList;
	}
}

 /**
 * \cond
 * Evaluates all the signatures that are possible candidates for the result
 * against the target user agent using the active method.
 * @param ws pointer to the work set being used for the match
 * \endcond
 */
static void evaluateSignatures(fiftyoneDegreesWorkset *ws) {
	int16_t lastNodeCharacter = getRootNode(ws->dataSet, *(ws->nodes))->position;
	int32_t count = 0;
	const byte *currentSignature;

	/* Setup the linked list to be navigated */
	resetClosestSignatureEnumeration(ws);
	currentSignature = ws->functionPtrNextClosestSignature(ws);
	ws->difference = INT_MAX;

	while (currentSignature != NULL &&
		count < ws->dataSet->header.maxSignatures) {
		setSignatureAsString(ws, currentSignature);
		evaluateSignature(ws, currentSignature, lastNodeCharacter);
		currentSignature = ws->functionPtrNextClosestSignature(ws);
		count++;
	}
}

 /**
 * \cond
 * Evaluates the target user agent against root nodes to find those nodes
 * that match precisely.
 * @param ws pointer to the work set used for the match
 * \endcond
 */
static void evaluate(fiftyoneDegreesWorkset *ws) {
	const fiftyoneDegreesNode *node;
	while (ws->nextCharacterPositionIndex >= 0 &&
		ws->nodeCount < ws->dataSet->header.signatureNodesCount) {
		ws->rootNodesEvaluated++;
		node = getCompleteNode(ws, *(ws->dataSet->rootNodes + ws->nextCharacterPositionIndex));
		if (node != NULL) {
			*(ws->nodes + ws->nodeCount) = node;
			ws->nodeCount++;
			ws->nextCharacterPositionIndex = node->nextCharacterPosition;
		}
		else {
			ws->nextCharacterPositionIndex--;
		}
	}
}

 /**
 * \cond
 * Processes the data in the workset setting relevant fields to indicate
 * the result of the match.
 * @param ws workset configured with input data
 * \endcond
 */
static void setMatch(fiftyoneDegreesWorkset *ws) {
	int32_t signatureIndex;
	evaluate(ws);
	signatureIndex = getExactSignatureIndex(ws);
	if (signatureIndex >= 0) {
		setMatchSignatureIndex(ws, signatureIndex);
		ws->method = EXACT;
	}
	else {
		evaluateNumeric(ws);
		signatureIndex = getExactSignatureIndex(ws);
		if (signatureIndex >= 0) {
			setMatchSignatureIndex(ws, signatureIndex);
			ws->method = NUMERIC;
		}
		else if (ws->nodeCount > 0) {
			// Find the closest signatures and compare them
			// to the target looking at the smallest character
			// difference.
			fillClosestSignatures(ws);

			ws->startWithInitialScore = 0;
			ws->functionPtrGetScore = &getScoreNearest;
			evaluateSignatures(ws);
			if (ws->signature != NULL) {
				ws->method = NEAREST;
			}
			else {
				ws->startWithInitialScore = 1;
				ws->functionPtrGetScore = &getScoreClosest;
				evaluateSignatures(ws);
				ws->method = CLOSEST;
			}

			setMatchSignature(ws, ws->signature);
		}
	}
	if (ws->profileCount == 0) {
		setMatchDefault(ws);
		ws->method = NONE;
	}
	setRelevantNodes(ws);
}

/**
 * \cond
 * First the cache is checked to determine if the userAgent has already been
 * found. If not then detection is performed. The cache is then updated before
 * the resultset is returned.
 * @param ws pointer to a work set to be used for the match created via
 *        createWorkset function
 * @param userAgent pointer to the target user agent
 * @param userAgentLength of the user agent string
 * \endcond
 */
static void internalMatch(fiftyoneDegreesWorkset *ws, const char* userAgent, int userAgentLength) {
	fiftyoneDegreesResultset *rs = NULL;
	setTargetUserAgentArray(ws, userAgent, userAgentLength);
	if (ws->targetUserAgentArrayLength >= ws->dataSet->header.minUserAgentLength) {
		if (ws->cache != NULL) {

			// Calculate the hash code for the target user agent before
			// locking thing active list to improve performance over
			// performing the hashcode calculation in the lock.
			setResultsetHashCode((fiftyoneDegreesResultset*)ws);

#ifndef FIFTYONEDEGREES_NO_THREADING
			// Lock the cache to stop other threads altering the
			// cache whilst this thread is checking it.
			FIFTYONEDEGREES_MUTEX_LOCK(&ws->cache->lock);
#endif

			// Does the target exist in the cache?
			rs = resultsetCacheFetch(ws);

			// If the item couldn't be retrieved from the cache then perform
			// a match and then cache this result.
			if (rs == NULL) {

#ifndef FIFTYONEDEGREES_NO_THREADING
				// Unlock the cache whilst the detection is performed. The cache
				// will be checked again after detection and may has already
				// been altered by another thread.
				FIFTYONEDEGREES_MUTEX_UNLOCK(&ws->cache->lock);
#endif

				// Get the results of the detection process.
				setMatch(ws);

#ifndef FIFTYONEDEGREES_NO_THREADING
				// Lock the cache again whilst the result is added to the cache
				// list and the resulset from the cache copied into the result
				FIFTYONEDEGREES_MUTEX_LOCK(&ws->cache->lock);
#endif
				// Does the entry still not exist in the cache.
				rs = resultsetCacheFetch(ws);

				if (rs == NULL) {
					// Get an empty resultset from the cache, copy the workset into
					// the entry and then add the entry back into the cache.
					rs = resultsetCacheGetNextFree((fiftyoneDegreesResultsetCache*)ws->cache);
					resultsetCacheCopy(rs, (fiftyoneDegreesResultset*)ws);
					resultsetCacheAdd(rs);
				}
			}
			else {

				// The cache does have the resultset. Copy this to the work set.
				resultsetCacheCopy((fiftyoneDegreesResultset*)ws, rs);
			}

#ifndef FIFTYONEDEGREES_NO_THREADING
			// Unlock the cache.
			FIFTYONEDEGREES_MUTEX_UNLOCK(&ws->cache->lock);
#endif
		}
		else {
			// There is no cache. Just process the input data and
			// use the workset as the return pointer.
			setMatch(ws);
		}
	}
	else {
		setMatchDefault(ws);
		ws->method = NONE;
	}
}

/**
 * \cond
 * Main entry method used for perform a match. First the cache is checked to
 * determine if the userAgent has already been found. If not then detection
 * is performed. The cache is then updated before the resultset is returned.
 * @param ws pointer to a work set to be used for the match created via
 *        createWorkset function
 * @param userAgent pointer to the target user agent
 * \endcond
 */
void fiftyoneDegreesMatch(fiftyoneDegreesWorkset *ws, const char* userAgent) {
	resetCounters(ws);
	internalMatch(ws, userAgent, 0);
}

/**
 * \cond
 * Looks for for first HTTP header that matches the component. If found then
 * perform a standard match and returns.
 * @param ws pointer to a work set to be used for the match created via
 *        createWorkset function
 * @param component to find the profile for
 * @return true if the header was found and processed, otherwise false
 * \endcond
 */
static byte matchForHttpHeader(fiftyoneDegreesWorkset *ws, const fiftyoneDegreesComponent *component) {
	int httpHeaderIndex, importantHeaderIndex;
	int32_t httpHeaderOffset;
	for (httpHeaderIndex = 0; httpHeaderIndex < component->httpHeaderCount; httpHeaderIndex++) {
		httpHeaderOffset = getComponentHeaderOffset(component, httpHeaderIndex);
		for (importantHeaderIndex = 0; importantHeaderIndex < ws->importantHeadersCount; importantHeaderIndex++) {
			if (ws->importantHeaders[importantHeaderIndex].header->headerNameOffset == httpHeaderOffset) {
				internalMatch(ws,
					ws->importantHeaders[importantHeaderIndex].headerValue,
					ws->importantHeaders[importantHeaderIndex].headerValueLength);
				return TRUE;
			}
		}
	}
	return FALSE;
}

 /**
 * \cond
 * Sets the workset for the important headers included in the workset.
 * @param ws pointer to a work set to be used for the match created via
 *        createWorkset function
 * \endcond
 */
void fiftyoneDegreesMatchForHttpHeaders(fiftyoneDegreesWorkset *ws) {
	int componentIndex, profileIndex = 0;
	int32_t differenceTotal = 0;
	fiftyoneDegreesMatchMethod worstMethod = EXACT;
	if (ws->importantHeadersCount == 0) {
		// No important headers were found. Set the default match.
		setMatchDefault(ws);
		ws->method = NONE;
	}
	else if (ws->importantHeadersCount == 1) {
		// There is only one important header so no need to do anything complex.
		resetCounters(ws);
		internalMatch(ws,
			ws->importantHeaders[0].headerValue,
			ws->importantHeaders[0].headerValueLength);
	}
	else {
		// Loop through each component and get the results for the first header
		// that is also in the list of important headers.
		resetCounters(ws);
		for (componentIndex = 0; componentIndex < ws->dataSet->header.components.count; componentIndex++) {
			ws->difference = 0;
			if (matchForHttpHeader(ws, ws->dataSet->components[componentIndex])) {
				differenceTotal += ws->difference;
				if ((int)ws->method > (int)worstMethod) {
					worstMethod = ws->method;
				}
				ws->tempProfiles[componentIndex] = ws->profiles[componentIndex];
			}
			else {
				ws->tempProfiles[componentIndex] = NULL;
			}
		}

		// Now set the profiles being returned based on the temp profiles found
		// for each of the components.
		for (componentIndex = 0; componentIndex < ws->dataSet->header.components.count; componentIndex++) {
			if (ws->tempProfiles[componentIndex] != NULL) {
				ws->profiles[profileIndex] = (fiftyoneDegreesProfile*)ws->tempProfiles[componentIndex];
				profileIndex++;
			}
		}
		ws->profileCount = profileIndex;

		// Set the signature to NULL because there can be no signature when multi
		// headers are used.
		ws->signature = NULL;
		ws->targetUserAgent = NULL;

		// Use the worst method used in the resulting workset.
		ws->method = worstMethod;
		ws->difference = differenceTotal;
	}
}

 /**
 * \cond
 * Passed array of HTTP header names and values. Sets the workset to
 * the results for these headers.
 * @param ws pointer to a work set to be used for the match created via
 *        createWorkset function
 * @param httpHeaderNames array of HTTP header names i.e. User-Agent
 * @param httpHeaderValues array of HTTP header values
 * @param httpHeaderCount the number of entires in each array
 * \endcond
 */
void fiftyoneDegreesMatchWithHeadersArray(fiftyoneDegreesWorkset *ws, const char **httpHeaderNames, const char **httpHeaderValues, int httpHeaderCount) {
	int httpHeaderIndex, dataSetHeaderIndex, importantHeaderIndex = 0;
	for (httpHeaderIndex = 0;
		httpHeaderIndex < httpHeaderCount &&
		importantHeaderIndex < ws->dataSet->httpHeadersCount;
		httpHeaderIndex++) {
		for (dataSetHeaderIndex = 0; dataSetHeaderIndex < ws->dataSet->httpHeadersCount; dataSetHeaderIndex++) {
			if (strcmp(ws->dataSet->httpHeaders[dataSetHeaderIndex].headerName, httpHeaderNames[httpHeaderIndex]) == 0)
			{
				ws->importantHeaders[importantHeaderIndex].header = ws->dataSet->httpHeaders + dataSetHeaderIndex;
				ws->importantHeaders[importantHeaderIndex].headerValue = httpHeaderValues[httpHeaderIndex];
				ws->importantHeaders[importantHeaderIndex].headerValueLength = (int)strlen(httpHeaderValues[httpHeaderIndex]);
				importantHeaderIndex++;
				break;
			}
		}
	}
	ws->importantHeadersCount = importantHeaderIndex;
	fiftyoneDegreesMatchForHttpHeaders(ws);
}

 /**
 * \cond
 * Initialises the prefixed upper HTTP header names for use with Perl, Python
 * and PHP. These headers are in the form HTTP_XXXXXX_XXXX where User-Agent
 * would appear as HTTP_USER_AGENT. This method avoids needing to duplicate
 * the logic to format the header names in each API.
 * @param dataSet pointer to a data set instance with uninitialised prefixed
 * 		upper headers
 * \endcond
 */
static void initPrefixedUpperHttpHeaderNames(const fiftyoneDegreesDataSet *dataSet) {
	int index;
	int httpHeaderIndex;
	char *prefixedUpperHttpHeader;
	const fiftyoneDegreesAsciiString *httpHeaderName;
	((fiftyoneDegreesDataSet*)dataSet)->prefixedUpperHttpHeaders = (const char**)fiftyoneDegreesMalloc(dataSet->httpHeadersCount * sizeof(const char*));
	if (dataSet->prefixedUpperHttpHeaders != NULL) {
		for (httpHeaderIndex = 0; httpHeaderIndex < dataSet->httpHeadersCount; httpHeaderIndex++) {
			httpHeaderName = fiftyoneDegreesGetString(
				dataSet,
				(dataSet->httpHeaders + httpHeaderIndex)->headerNameOffset);
			dataSet->prefixedUpperHttpHeaders[httpHeaderIndex] = (char*)fiftyoneDegreesMalloc((httpHeaderName->length + sizeof(HTTP_PREFIX_UPPER) - 1) * sizeof(char));
			if (dataSet->prefixedUpperHttpHeaders[httpHeaderIndex] != NULL) {
				prefixedUpperHttpHeader = (char*)dataSet->prefixedUpperHttpHeaders[httpHeaderIndex];
				memcpy(prefixedUpperHttpHeader, HTTP_PREFIX_UPPER, sizeof(HTTP_PREFIX_UPPER) - 1);
				prefixedUpperHttpHeader += sizeof(HTTP_PREFIX_UPPER) - 1;
				for (index = 0; index < httpHeaderName->length; index++) {
					*prefixedUpperHttpHeader = (char)toupper(*(&httpHeaderName->firstByte + index));
					if (*prefixedUpperHttpHeader == '-') {
						*prefixedUpperHttpHeader = '_';
					}
					prefixedUpperHttpHeader++;
				}
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
const char* fiftyoneDegreesGetPrefixedUpperHttpHeaderName(const fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex) {
	const char *prefixedUpperHeaderName = NULL;
	if (dataSet->prefixedUpperHttpHeaders == NULL) {
		initPrefixedUpperHttpHeaderNames(dataSet);
	}
	if (httpHeaderIndex >= 0 &&
		httpHeaderIndex < dataSet->httpHeadersCount) {
		prefixedUpperHeaderName = dataSet->prefixedUpperHttpHeaders[httpHeaderIndex];
	}
	return prefixedUpperHeaderName;
}

 /**
 * \cond
 * Sets name to the start of the http header name and returns the length of
 * the string. A space or colon are used to identify the end of the header
 * name.
 * @param start of the string to be processed
 * @param end of the string to be processed
 * @param value to be set when returned
 * @returns the number of characters in the value
 * \endcond
 */
static int setNextHttpHeaderName(const char* start, const char* end, char** name) {
	char *current = (char*)start, *lastChar = (char*)start;
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
 * \cond
 * Sets the value pointer to the start of the next HTTP header value and
 * returns the length.
 * @param start of the string to be processed
 * @param end of the string to be processed
 * @param value to be set when returned
 * @returns the number of characters in the value
 * \endcond
 */
static int setNextHttpHeaderValue(char* start, const char *end, char** value) {
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
 * \cond
 * Compares two header strings for case insensitive equality and where -
 * are replaced with _. The http header name must be the same length
 * as the unique header.
 * @param httpHeaderName string to be checked for equality
 * @param uniqueHeader the unique HTTP header to be compared
 * @param length of the strings
 * @returns 0 if both strings are equal, otherwise the different between
 *          the first mismatched characters
 * \endcond
 */
static int headerCompare(char *httpHeaderName, const fiftyoneDegreesAsciiString *uniqueHeader, int length) {
	int index, difference;
	for (index = 0; index < length; index++) {
		if (httpHeaderName[index] == '_') {
			difference = '-' - (&uniqueHeader->firstByte)[index];
		}
		else {
			difference = tolower(httpHeaderName[index]) - tolower((&uniqueHeader->firstByte)[index]);
		}
		if (difference != 0) {
			return difference;
		}
	}
	return 0;
}

 /**
 * \cond
 * Return the index of the unique header, or -1 if the header is not important.
 * Check for headers from Perl and PHP with HTTP_ prefixes.
 * @param dataSet the header is being checked against
 * @param httpHeaderName of the header being checked
 * @param length of the header name
 * @returns the index in the datasets headers of this header or -1
 * \endcond
 */
static int getUniqueHttpHeaderIndex(const fiftyoneDegreesDataSet *dataSet, char* httpHeaderName, int length) {
	int uniqueHeaderIndex;
	static const int httpPrefixLength = sizeof(HTTP_PREFIX_UPPER) - 1;
	char *adjustedHttpHeaderName;
	const fiftyoneDegreesAsciiString *header;

	// Check if header is from a Perl or PHP wrapper in the form of HTTP_*
	// and if present skip these characters.
	if (strncmp(httpHeaderName, HTTP_PREFIX_UPPER, httpPrefixLength) == 0) {
		adjustedHttpHeaderName = httpHeaderName + httpPrefixLength;
		length -= httpPrefixLength;
	}
	else {
		adjustedHttpHeaderName = httpHeaderName;
	}

	for (uniqueHeaderIndex = 0; uniqueHeaderIndex < dataSet->httpHeadersCount; uniqueHeaderIndex++) {
		// Get the unique header string to compare length and equality.
		header = fiftyoneDegreesGetString(dataSet, (dataSet->httpHeaders + uniqueHeaderIndex)->headerNameOffset);

		// Compare the headers for length and then equality recognising that
		// the data set string length includes the null terminator. If they
		// match then a matching header is found and the unique index
		// should be used.
		if (header->length - 1 == length &&
			headerCompare(adjustedHttpHeaderName, header, length) == 0) {
			return uniqueHeaderIndex;
		}
	}

	return -1;
}

 /**
 * \cond
 * Passed a string where each line contains the HTTP header name and value.
 * The first space character and/or colon separates the HTTP header name
 * at the beginning of the line and the value. Does not perform a device
 * detection. Use fiftyoneDegreesMatchForHttpHeaders to complete a match.
 * @param ws pointer to a work set to have important headers set
 * @param httpHeaders is a list of HTTP headers and values on each line
 * @param size is the valid characters in the httpHeaders string
 * \endcond
 */
int32_t fiftyoneDegreesSetHttpHeaders(fiftyoneDegreesWorkset *ws, const char *httpHeaders, size_t size) {
	char *headerName, *headerValue;
	const char *endOfHeaders = httpHeaders + size;
	int headerNameLength, headerValueLength, uniqueHeaderIndex = 0;
	ws->importantHeadersCount = 0;
	headerNameLength = setNextHttpHeaderName(httpHeaders, endOfHeaders, &headerName);
	while (headerNameLength > 0 &&
		ws->importantHeadersCount < ws->dataSet->httpHeadersCount) {
		headerValueLength = setNextHttpHeaderValue(headerName + headerNameLength, endOfHeaders, &headerValue);
		uniqueHeaderIndex = getUniqueHttpHeaderIndex(ws->dataSet, headerName, headerNameLength);
		if (uniqueHeaderIndex >= 0) {
			ws->importantHeaders[ws->importantHeadersCount].header = ws->dataSet->httpHeaders + uniqueHeaderIndex;
			ws->importantHeaders[ws->importantHeadersCount].headerValue = headerValue;
			ws->importantHeaders[ws->importantHeadersCount].headerValueLength = headerValueLength;
			ws->importantHeadersCount++;
		}
		headerNameLength = setNextHttpHeaderName(headerValue + headerValueLength, endOfHeaders, &headerName);
	}
	return ws->importantHeadersCount;
}

 /**
 * \cond
 * Passed a string where each line contains the HTTP header name and value.
 * The first space character seperates the HTTP header name at the beginning of
 * the line and the value.
 * @param ws pointer to a work set to be used for the match created via
 *        createWorkset function
 * @param httpHeaders is a list of HTTP headers and values on each line
 * @param size is the valid characters in the httpHeaders string
 * \endcond
 */
void fiftyoneDegreesMatchWithHeadersString(fiftyoneDegreesWorkset *ws, const char *httpHeaders, size_t size) {
	fiftyoneDegreesSetHttpHeaders(ws, httpHeaders, size);
	fiftyoneDegreesMatchForHttpHeaders(ws);
}

 /**
 * \cond
 * Returns the rank of the signature set in the workset.
 * @param ws pointer to the work set associated with the match
 * @returns the rank of the signature if available, or INT_MAX
 * \endcond
 */
int32_t fiftyoneDegreesGetSignatureRank(fiftyoneDegreesWorkset *ws) {
	return getRankFromSignature(ws->dataSet, ws->signature);
}

/**
 * \cond
 * Gets the device id as a string.
 * @param ws pointer to the work set associated with the match
 * @param deviceId pointer to memory to place the device id
 * @param size of the memory allocated for the device id
 * @return the number of bytes written for the device id
 * \endcond
 */
int32_t fiftyoneDegreesGetDeviceId(fiftyoneDegreesWorkset *ws, char *deviceId, int size) {
	int32_t length = 0;
	char *current = deviceId;
	int profileIndex, profileId;

	// Get the length of the string needed to store the device id where each
	// profile id needs the number of digits plus one for the seperator or
	// string terminator.
	for (profileIndex = 0; profileIndex < ws->profileCount; profileIndex++) {
		profileId = ws->profiles[profileIndex]->profileId;
		length += profileId > 0 ? (int)(floor(log10((float)abs(profileId)))) + 2 : 1;
	}

	// Set the device id if enough space available.
	if (length <= size) {
		for (profileIndex = 0; profileIndex < ws->profileCount; profileIndex++) {
			if (profileIndex > 0) {
				*current = '-';
				current++;
			}
			current += snprintf(
				current,
				size - (int)(current - deviceId),
				"%d",
				ws->profiles[profileIndex]->profileId);
		}
	}
	return length <= size ? length : -length;
}

 /**
 * \cond
 * Gets the profile associated with the profileId or NULL if there is no
 * corresponding profile.
 * \endcond
 */
static fiftyoneDegreesProfile* findProfileForProfileId(
		const fiftyoneDegreesDataSet *dataSet, const int profileId) {
	int32_t upper = dataSet->header.profileOffsets.count - 1;
	int32_t lower = 0, middle;
	if (upper >= 0)
	{
		while (lower <= upper)
		{
			middle = lower + (upper - lower) / 2;
			if (dataSet->profileOffsets[middle].profileId == profileId) {
				return (fiftyoneDegreesProfile*)(dataSet->profiles +
					dataSet->profileOffsets[middle].offset);
			}
			else if (profileId < dataSet->profileOffsets[middle].profileId) {
				upper = middle - 1;
			}
			else {
				lower = middle + 1;
			}
		}
	}
	return NULL;
}

/**
 * \cond
 * Sets the workset for the device Id provided.
 * @param ws pointer to the work set associated with the match.
 * @param deviceId string representation of the device id to use for the match.
 * \endcond
 */
void fiftyoneDegreesMatchForDeviceId(fiftyoneDegreesWorkset *ws, const char *deviceId) {
	char *start = ws->input, *current = ws->input;
	int lastId = 0, profileId;
	fiftyoneDegreesProfile *profile;
	resetCounters(ws);
	ws->profileCount = 0;
	if (strncpy(ws->input, deviceId, strlen(deviceId) + 1) == ws->input) {
		while (lastId == 0 && ws->profileCount < ws->dataSet->header.components.count) {
			lastId = *current == 0;
			if (isdigit(*current) == 0 || *current == 0) {
				*current = 0;
				profileId = atoi(start);
				if (profileId != 0) {
					profile = findProfileForProfileId(ws->dataSet, profileId);
					if (profile != NULL) {
						assert(profile->profileId == profileId);
						ws->profiles[ws->profileCount] = profile;
						ws->profileCount++;
					}
				}
				start = current + 1;
			}
			current++;
		}
		ws->signature = NULL;
		ws->method = NONE;
	}
}

/**
 * \cond
 * Gets the required property name at the index provided.
 * @param ws pointer to the work set associated with the match
 * @param signatureAsString pointer to memory to place the signature
 * @param size of the memory allocated for the signature
 * @return the number of bytes written for the signature
 * \endcond
 */
int32_t fiftyoneDegreesGetSignatureAsString(fiftyoneDegreesWorkset *ws, char *signatureAsString, int size) {
	int32_t length;
	if (ws->signature != NULL) {
		setSignatureAsString(ws, ws->signature);
		length = (int32_t)strlen(ws->signatureAsString);
		if (length <= size) {
			// Copy the signature as a string.
			strcpy(signatureAsString, ws->signatureAsString);
		}
		else {
			// The memory for the string is too short. Return
			// negative length.
			length = -length;
		}
	}
	else {
		// There is no signature so return 0 length.
		*signatureAsString = 0;
		length = 0;
	}
	return length;
}

 /**
 * \cond
 * Sets the values associated with the require property index in the workset
 * so that an array of values can be read.
 * @param ws pointer to the work set associated with the match
 * @param requiredPropertyIndex index of the property required from the array of
 *        require properties
 * @return the number of values that were set.
 * \endcond
 */
int32_t fiftyoneDegreesSetValues(fiftyoneDegreesWorkset *ws, int32_t requiredPropertyIndex) {
	int32_t profileIndex = 0, valueIndex;
	const fiftyoneDegreesProfile *profile;
	const fiftyoneDegreesProperty *property = *(ws->dataSet->requiredProperties + requiredPropertyIndex);
	int32_t *profileValueIndexes;
	int32_t propertyIndex;
	const fiftyoneDegreesValue *value;
	ws->valuesCount = 0;
	if (property != NULL) {
		propertyIndex = getPropertyIndex(ws->dataSet, property);
		while (profileIndex < ws->profileCount &&
			ws->valuesCount == 0) {
			profile = ws->profiles[profileIndex];
			if (profile->componentIndex == property->componentIndex) {
				profileValueIndexes = (int32_t*)((byte*)profile + sizeof(fiftyoneDegreesProfile));
				valueIndex = 0;
				while(valueIndex < profile->valueCount &&
					ws->valuesCount == 0) {
					value = ws->dataSet->values + profileValueIndexes[valueIndex];
					while (value->propertyIndex == propertyIndex) {
						ws->values[ws->valuesCount] = value;
						ws->valuesCount++;
						valueIndex++;
						if (valueIndex == profile->valueCount) {
							break;
						}
						value = ws->dataSet->values + profileValueIndexes[valueIndex];
					}
					valueIndex++;
				}
			}
			profileIndex++;
		}
	}
	return ws->valuesCount;
}

 /**
 * \cond
 * Allocates memory sufficiently large to store CSV results.
 * @param ws pointer to a workset with the results to return in CSV
 * @returns pointer to memory space to store CSV results
 * \endcond
 */
char* fiftyoneDegreesCSVCreate(fiftyoneDegreesWorkset *ws) {
	return (char*)fiftyoneDegreesMalloc(ws->dataSet->header.csvBufferLength * sizeof(char));
}

 /**
 * \cond
 * Frees the memory space previously allocated by fiftyoneDegreesCSVCreate.
 * @param csv pointer to the memory space to be freed
 * \endcond
 */
void fiftyoneDegreesCSVFree(void* csv) {
	fiftyoneDegreesFree(csv);
}

 /**
 * \cond
 * Process the workset results into a CSV string.
 * @param ws pointer to a workset with the results to return in CSV
 * @param csv pointer to memory allocated with fiftyoneDegreesCSVCreate
 * \endcond
 */
int32_t fiftyoneDegreesProcessDeviceCSV(fiftyoneDegreesWorkset *ws, char* csv) {
	int32_t propertyIndex, valueIndex, profileIndex;
	char* currentPos = csv;
	char* endPos = csv + ws->dataSet->header.csvBufferLength;

	if (ws->profileCount > 0) {
		currentPos += snprintf(
			currentPos,
			(int32_t)(endPos - currentPos),
			"Id,");
		for (profileIndex = 0; profileIndex < ws->profileCount; profileIndex++) {
			currentPos += snprintf(
				currentPos,
				(int32_t)(endPos - currentPos),
				"%d",
				ws->profiles[profileIndex]->profileId);
			if (profileIndex < ws->profileCount - 1) {
				currentPos += snprintf(
					currentPos,
					(int32_t)(endPos - currentPos),
					"-");
			}
		}
		currentPos += snprintf(
			currentPos,
			(int32_t)(endPos - currentPos),
			"\n");

		for (propertyIndex = 0; propertyIndex < ws->dataSet->requiredPropertyCount; propertyIndex++) {
			if (fiftyoneDegreesSetValues(ws, propertyIndex) > 0) {
				currentPos += snprintf(
					currentPos,
					(int32_t)(endPos - currentPos),
					"%s,",
					fiftyoneDegreesGetPropertyName(ws->dataSet, *(ws->dataSet->requiredProperties + propertyIndex)));
				for (valueIndex = 0; valueIndex < ws->valuesCount; valueIndex++) {
					currentPos += snprintf(
						currentPos,
						(int32_t)(endPos - currentPos),
						"%s",
						fiftyoneDegreesGetValueName(ws->dataSet, *(ws->values + valueIndex)));
					if (valueIndex < ws->valuesCount - 1) {
						currentPos += snprintf(
							currentPos,
							(int32_t)(endPos - currentPos),
							"|");
					}
				}
				currentPos += snprintf(
					currentPos,
					(int32_t)(endPos - currentPos),
					"\n");
			}
		}
	}
	return (int32_t)(currentPos - csv);
}

 /**
 * \cond
 * Allocates memory sufficiently large to store JSON results.
 * @param ws pointer to a workset with the results to return in JSON
 * @returns pointer to memory space to store JSON results
 * \endcond
 */
char* fiftyoneDegreesJSONCreate(fiftyoneDegreesWorkset *ws) {
	return (char*)fiftyoneDegreesMalloc(ws->dataSet->header.jsonBufferLength * sizeof(char));
}

 /**
 * \cond
 * Frees the memory space previously allocated by fiftyoneDegreesJSONCreate.
 * @param json pointer to the memory space to be freed
 * \endcond
 */
void fiftyoneDegreesJSONFree(void* json) {
	fiftyoneDegreesFree(json);
}

 /**
 * \cond
 * Escapes a range of characters in a JSON string value.
 * @param start the first character to be considered
 * @param next the character after the last one to be considered
 * @param max the last allocated pointer
 * @return the number of characters that were escaped
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
 * Process the workset results into a JSON string.
 * @param ws pointer to a workset with the results to return in JSON
 * @param JSON pointer to memory allocated with fiftyoneDegreesJSONCreate
 * @return number of characters written to the JSON string
 * \endcond
 */
int32_t fiftyoneDegreesProcessDeviceJSON(fiftyoneDegreesWorkset *ws, char* json) {
	int32_t propertyIndex, valueIndex, profileIndex;
	const fiftyoneDegreesAsciiString* valueName;
	char* currentPos = json;
	char* endPos = json + ws->dataSet->header.jsonBufferLength;

	if (ws->profileCount > 0) {

		// Add the device ID to the JSON.
		currentPos += snprintf(
			currentPos,
			(int32_t)(endPos - currentPos),
			"{\"Id\": \"");
		for (profileIndex = 0; profileIndex < ws->profileCount; profileIndex++) {
			currentPos += snprintf(
				currentPos,
				(int32_t)(endPos - currentPos),
				"%d",
				ws->profiles[profileIndex]->profileId);
			if (profileIndex < ws->profileCount - 1) {
				*currentPos = '-';
				currentPos++;
			}
		}
		currentPos += snprintf(
			currentPos,
			(int32_t)(endPos - currentPos),
			"\",\n");

		// Add each of the required properties.
		for (propertyIndex = 0; propertyIndex < ws->dataSet->requiredPropertyCount; propertyIndex++) {
			if (fiftyoneDegreesSetValues(ws, propertyIndex) > 0) {
				currentPos += snprintf(
					currentPos,
					(int32_t)(endPos - currentPos),
					"\"%s\": \"",
					fiftyoneDegreesGetPropertyName(ws->dataSet, *(ws->dataSet->requiredProperties + propertyIndex)));
				for (valueIndex = 0; valueIndex < ws->valuesCount; valueIndex++) {
					valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[valueIndex]->nameOffset);
					memcpy(currentPos, &valueName->firstByte, valueName->length - 1);
					currentPos += valueName->length - 1 +
						escapeJSON(currentPos, currentPos + valueName->length - 1, endPos);
					if (valueIndex < ws->valuesCount - 1) {
						*currentPos = '|';
						currentPos++;
					}
				}
				if (propertyIndex + 1 != ws->dataSet->requiredPropertyCount) {
					currentPos += snprintf(
						currentPos,
						(int32_t)(endPos - currentPos),
						"\",\n");
				}
			}
		}
		currentPos += snprintf(
			currentPos,
			(int32_t)(endPos - currentPos),
			"\"}");
	}
	return (int32_t)(currentPos - json);
}

/**
 * \cond
 * OBSOLETE METHODS - RETAINED FOR BACKWARDS COMPAITABILITY
 * \endcond
 */

/**
 * \cond
 * Releases the workset provided back to the pool making it available for future
 * use.
 *
 * The method is obsolete as all worksets now have a pool associated with them
 * which used when returning the work set to the pool.
 *
 * @param pool containing worksets
 * @param ws workset to be placed back on the queue
 * \endcond
 */
void fiftyoneDegreesWorksetPoolRelease(fiftyoneDegreesWorksetPool *pool, fiftyoneDegreesWorkset *ws) {
	// Just a precaution, associated pool is assigned when workset is created.
	if (ws->associatedPool == NULL) {
		ws->associatedPool = pool;
	}
	fiftyoneDegreesWorksetRelease(ws);
}

fiftyoneDegreesWorkset* fiftyoneDegreesCreateWorkset(const fiftyoneDegreesDataSet *dataSet) {
	return fiftyoneDegreesWorksetCreate(dataSet, NULL);
}

void fiftyoneDegreesFreeWorkset(const fiftyoneDegreesWorkset *ws) {
	fiftyoneDegreesWorksetFree(ws);
}

void fiftyoneDegreesDestroy(const fiftyoneDegreesDataSet *dataSet) {
	fiftyoneDegreesDataSetFree(dataSet);
}

/**
* \cond
* Determines if the compiled code supports multi threading.
* @return boolean where true means multi threading is supported.
* \endcond
*/
int fiftyoneDegreesGetIsThreadSafe() {
#if FIFTYONEDEGREES_NO_THREADING
	return FALSE;
#else
	return TRUE;
#endif
};
