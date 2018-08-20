#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "51Degrees.h"

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
 * PROBLEM METHODS
 */

/* Change snprintf to the Microsoft version */
#ifdef _MSC_FULL_VER
#define snprintf _snprintf
#endif

// Define NDEBUG if needed, to ensure asserts are not called in release builds.
#if !defined(DEBUG) && !defined(_DEBUG) && !defined(NDEBUG)
#define NDEBUG
#endif
#include <assert.h>

/* The expected version of the data file */
#define TARGET_VERSION 34

/* Used for boolean results */
#define TRUE 1
#define FALSE 0

/**
 * Memory allocation and deallocation functions.
 */
void *(FIFTYONEDEGREES_CALL_CONV *fiftyoneDegreesMalloc)(size_t __size) = 
	malloc;
void (FIFTYONEDEGREES_CALL_CONV *fiftyoneDegreesFree)(void *__ptr) = free;


/**
* DATASET MEMORY ALLOCATION SIZE MACROS
*/
#define SIZE_OF_REQUIRED_PROPERTIES(count) count * sizeof(int32_t)
#define SIZE_OF_REQUIRED_PROPERTIES_ARRAY(count) count * sizeof(char *)
#define SIZE_OF_FILE_NAME(fileName) sizeof(char) * (strlen(fileName) + 1)
#define SIZE_OF_HTTP_HEADERS(count) count * sizeof(int32_t)

#define FIFTYONEDEGREES_MAX_STRING 500

/**
 * Define the direct memory getters. These are used in both memory and stream
 * mode.
 */
#define HTTPHEADER(d,k) d->httpHeaders.firstElement[k]
#define COMPONENT(d,k) d->componenets.firstElement[k]
#define PROPERTY(d,k) (d->allProperties.firstElement + k)

/**
 * Set the rest of the entity getters depending on whether the data set is
 * memory based or indirect. If FIFTYONEDEGREES_INDIRECT is defined, the data
 * data set will not be loaded into memory, but instead will be lazy loaded
 * from file.
 */
#ifdef FIFTYONEDEGREES_INDIRECT
/* Define entity getters for stream mode. */
#define STRING(d,k) ((char*)fiftyoneDegreesLoadingCacheGet(\
d->strings.cache, (const int64_t)k))
#define NODE(d,k) \
((fiftyoneDegreesSignatureNode*)fiftyoneDegreesLoadingCacheGet(\
d->nodes.cache, (const int64_t)k))
#define DEVICE(d,k) ((int32_t*)fiftyoneDegreesLoadingCacheGet(\
d->devices.cache, (const int64_t)k))
#define PROFILE(d,k) ((int32_t*)fiftyoneDegreesLoadingCacheGet(\
d->profiles.cache, (const int64_t)k))

/**
* Used to read data from file when initialising the data set.
*/
typedef struct file_reader_t {
	FILE *file; // Handle to the file being read.
	long length; // Length of the file in bytes.
} fileReader;

#else

/* Define entity getters for memory mode. */
#define STRING(d,k) ((char*)d->strings.firstByte + k + sizeof(int16_t))
#define NODE(d,k) \
(fiftyoneDegreesSignatureNode*)((byte*)d->nodes.firstByte + k)
#define DEVICE(d,k) ((int32_t*)d->devices.firstByte + k)
#define PROFILE(d,k) (int32_t*)(d->profiles.firstByte + k)

/**
* Used to read data from memory when initialising the data set.
*/
typedef struct memory_reader_t {
	byte *current; // The current byte being read from.
	byte *lastByte; // The maximum byte that can be read from.
	long length; // Length of the file in bytes.
} memoryReader;

#endif

/* Reader macros used to load common fixed size data structures. */
#define READ_INTEGERS(s,i) readListToMemory( \
s, (fiftyoneDegreesList*)i, sizeof(int32_t))
#define READ_PROPERTIES(s,i) readListToMemory( \
s, (fiftyoneDegreesList*)i, sizeof(fiftyoneDegreesProperty))

/**
 * The prime number used by the Rabin-Karp rolling hash method. 
 * https://en.wikipedia.org/wiki/Rabin%E2%80%93Karp_algorithm
 */
#define RK_PRIME 997

/**
 * The rolling hash data structure.
 */
typedef struct {
	const fiftyoneDegreesDataSet *dataSet;
	const char *userAgent;
	int userAgentLength;
	char *matchedUserAgent;
	fiftyoneDegreesSignatureNode *node;
	uint32_t power;
	uint32_t hash;
	int drift;
	int difference;
	int currentIndex;
	int firstIndex;
	int lastIndex;
	int deviceIndex;
} match_t;

/**
 * Array of powers for the RK_PRIME. 
 */
static unsigned int POWERS[129] = {
	0,
	997,
	994009,
	991026973,
	211414001,
	326361493,
	3259861321,
	3086461261,
	2005293281,
	2117608517,
	2426749113,
	1402278013,
	2206807761,
	1164082165,
	948748585,
	1009534125,
	1483175361,
	1257085093,
	3478354585,
	1880913373,
	2664891825,
	2607360597,
	1083301129,
	2014434317,
	2641286817,
	548004101,
	899242105,
	3191181117,
	3331774609,
	1769565365,
	3320077545,
	2992494445,
	2809658241,
	910589285,
	1619417689,
	3946699933,
	669790065,
	2060763925,
	1587265737,
	1955974861,
	191784033,
	2230119877,
	2931425337,
	2053299709,
	2735376977,
	4161580405,
	157255849,
	2165258797,
	2689438017,
	1310110245,
	509856281,
	1520571229,
	4181027121,
	2365762517,
	728183945,
	149920141,
	3441492513,
	3784133253,
	1799567353,
	3167288509,
	985680913,
	3471326773,
	3464119401,
	573336813,
	386152193,
	2741647077,
	1822935513,
	695540253,
	1963897585,
	3795772565,
	519059529,
	2106274893,
	4012027873,
	1377236805,
	3010527161,
	3608406909,
	2694061521,
	1624776437,
	699437097,
	1554083757,
	3233279169,
	2353859493,
	1745770905,
	1071837405,
	3470003377,
	2144693589,
	3660762121,
	3352600333,
	1057975713,
	2534798341,
	1753175929,
	4159679037,
	2556559249,
	1973964725,
	947809257,
	73024109,
	4085559937,
	1674260581,
	2790488409,
	3273103261,
	3403773553,
	538068501,
	3878350793,
	1245174221,
	193149793,
	3591782597,
	3299491641,
	3943184637,
	1460007249,
	3928281205,
	3781154729,
	3124946221,
	1720092737,
	1240507685,
	4130547993,
	3577679453,
	2123558961,
	4064374485,
	2027201417,
	2485183629,
	3826915617,
	1503911301,
	455980793,
	3641284541,
	1113322257,
	1880727861,
	2479936361,
	2890356717,
	4057558529,
};

#ifdef FIFTYONEDEGREES_INDIRECT
/**
* Releases the file handle stored in the cache loader parameters when the
* cache has been finished with.
* @param params reference to the loading cache parameters.
*/
static void freeCacheParams(const void *params) {
	fclose(((fiftyoneDegreesCollection*)params)->handle);
}
#endif

/**
 * \cond
 * Frees the memory used by the dataset.
 * @param dataSet a pointer to the dataset to be freed.
 * \endcond
 */
void fiftyoneDegreesDataSetFree(fiftyoneDegreesDataSet *dataSet) {
	unsigned int index;
	if (dataSet->requiredPropertiesNames != NULL) {
#ifdef FIFTYONEDEGREES_INDIRECT
		// In stream mode, the required properties are allocated as they cannot
		// point to an item in the cache.
		for (index = 0;
			index < (int)dataSet->requiredProperties.count;
			index++) {
			if (dataSet->requiredPropertiesNames[index] != NULL) {
				fiftyoneDegreesFree(
					(void*)dataSet->requiredPropertiesNames[index]);
				dataSet->requiredPropertiesNames[index] = NULL;
			}
		}
#endif
		fiftyoneDegreesFree((void*)dataSet->requiredPropertiesNames);
		dataSet->requiredPropertiesNames = NULL;
	}
	if (dataSet->requiredProperties.firstElement != NULL) {
		fiftyoneDegreesFree((void*)dataSet->requiredProperties.firstElement);
		dataSet->requiredProperties.firstElement = NULL;
	}
	if (dataSet->prefixedUpperHttpHeaders != NULL) {
		for (index = 0; index < dataSet->uniqueHttpHeaders.count; index++) {
			if (dataSet->prefixedUpperHttpHeaders[index] != NULL) {
				fiftyoneDegreesFree(
					(void*)dataSet->prefixedUpperHttpHeaders[index]);
				dataSet->prefixedUpperHttpHeaders[index] = NULL;
			}
		}
		dataSet->prefixedUpperHttpHeaders = NULL;
	}
	if (dataSet->uniqueHttpHeaders.firstElement != NULL) {
		fiftyoneDegreesFree((void*)dataSet->uniqueHttpHeaders.firstElement);
		dataSet->uniqueHttpHeaders.firstElement = NULL;
	}
	if (dataSet->fileName != NULL) {
		fiftyoneDegreesFree((void*)dataSet->fileName);
		dataSet->fileName = NULL;
	}
	if (dataSet->httpHeaders.firstElement != NULL &&
		dataSet->httpHeaders.freeMemory) {
		fiftyoneDegreesFree((void*)dataSet->httpHeaders.firstElement);
		dataSet->httpHeaders.firstElement = NULL;
	}
	if (dataSet->components.firstElement != NULL &&
		dataSet->components.freeMemory) {
		fiftyoneDegreesFree((void*)dataSet->components.firstElement);
		dataSet->components.firstElement = NULL;
	}
	if (dataSet->allProperties.firstElement != NULL &&
		dataSet->allProperties.freeMemory) {
		fiftyoneDegreesFree((void*)dataSet->allProperties.firstElement);
		dataSet->allProperties.firstElement = NULL;
	}
#ifdef FIFTYONEDEGREES_INDIRECT
	// Free memory from all the caches.
	freeCacheParams(dataSet->devices.cache->params);
	fiftyoneDegreesCacheFree(dataSet->devices.cache);
	freeCacheParams(dataSet->profiles.cache->params);
	fiftyoneDegreesCacheFree(dataSet->profiles.cache);
	freeCacheParams(dataSet->strings.cache->params);
	fiftyoneDegreesCacheFree(dataSet->strings.cache);
	freeCacheParams(dataSet->nodes.cache->params);
	fiftyoneDegreesCacheFree(dataSet->nodes.cache);
#else
	// Free memory used to load the file into memory.
	if (dataSet->memoryToFree != NULL) {
		fiftyoneDegreesFree((void*)dataSet->memoryToFree);
		dataSet->memoryToFree = NULL;
	}
#endif
}

static fiftyoneDegreesDataSetInitStatus createFileHandle(
	const char* fileName,
	FILE** handle) {
	// Open the file and hold on to the pointer.
#ifndef _MSC_FULL_VER
	*handle = fopen(fileName, "rb");
#else
	/* If using Microsoft use the fopen_s method to avoid warning */
	errno_t error = fopen_s(handle, fileName, "rb");
	if (error != 0) {
		switch (error) {
		case ENFILE:
		case EMFILE:
			return DATA_SET_INIT_STATUS_TOO_MANY_OPEN_FILES;
		default:
			return DATA_SET_INIT_STATUS_FILE_NOT_FOUND;
		}
	}
#endif
	return DATA_SET_INIT_STATUS_SUCCESS;
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
	dataSet->fileName = (const char*)fiftyoneDegreesMalloc(
		SIZE_OF_FILE_NAME(fileName));
	if (dataSet->fileName == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	memcpy((char*)dataSet->fileName, (char*)fileName, strlen(fileName) + 1);
	return DATA_SET_INIT_STATUS_SUCCESS;
}

/**
 * \cond
 * Returns the index of the property requested, or -1 if not available.
 * @param dataSet a dataset that is already initialised.
 * @param property to look for in the dataset.
 * @param length the length of the property string provided.
 * @retrurns int the index of the property requested, or -1 if not
 * available.
 * \endcond
 */
static int getPropertyIndexRange(
		fiftyoneDegreesDataSet *dataSet,
		const char *property,
		size_t length) {
	unsigned int i = 0;
	const char *propertyString;
	for (i = 0; i < dataSet->allProperties.count; i++) {
		propertyString = STRING(dataSet, PROPERTY(dataSet, i)->stringOffset);
		if (strncmp(propertyString, property, length) == 0) {
			return i;
		}
	}
	return -1;
}

/**
 * \cond
 * Iterates over the properties specified in the string where properties are 
 * separated by commas or pipes. If the property name matches property names 
 * in the data set then the information about the matching property is passed 
 * to the callback method specified.
 * @param dataSet to find matching property names in
 * @param properties single string of separated property names required
 * @param matchedProperty callback method when a property is matched
 * \endcond
 */
static void iterateProperties(
	fiftyoneDegreesDataSet *dataSet,
	const char* properties, 
	void(*matchedProperty)(fiftyoneDegreesDataSet*, int, int)) {
	int propertyIndex, requiredIndex = 0;
	char *property = (char*)properties;
	const char *end = properties - 1;
	do {
		end++;
		if (*end == '|' || *end == ',' || *end == '\0') {
			// If the property name is one that is valid in the data set then
			// use the callback matchedProperty to provide the index.
			propertyIndex = getPropertyIndexRange(
				dataSet, 
				property,
				end - property);
			if (propertyIndex >= 0) {
				matchedProperty(dataSet, propertyIndex, requiredIndex++);
			}
			property = (char*)end + 1;
		}
	} while (*end != '\0');
}

/**
 * \cond
 * Iterates over the properties specified in the array. If they match property
 * names in the data set then the information about the matching property is
 * passed to the callback method specified.
 * @param dataSet to find matching property names in
 * @param properties array of strings containing required property names
 * @param count the number of elements in the properties parameter
 * @param matchedProperty callback method when a property is matched
 * \endcond
 */
static void iteratePropertiesFromArray(
	fiftyoneDegreesDataSet *dataSet,
	const char** properties,
	int count,
	void(*matchedProperty)(fiftyoneDegreesDataSet*, int32_t, int)) {
	const char *property;
	int i, propertyIndex, length = 0, requiredIndex = 0;
	for (i = 0; i < count; i++) {
		property = properties[i];
		length = (int)strlen(property);
		// If the property name is one that is valid in the data set then
		// use the callback matchedProperty to provide the index.
		propertyIndex = getPropertyIndexRange(dataSet, property, length);
		if (propertyIndex >= 0) {
			matchedProperty(dataSet, propertyIndex, requiredIndex++);
		}
	}
}

/**
 * \cond
 * Callback method used to count the number of required properties.
 * @param dataSet containing the required properties
 * @param propertyIndex the index of the property in the requiredProperties
 * @param index of the required property
 * \endcond
 */
static void increaseRequiredPropertiesCount(
	fiftyoneDegreesDataSet *dataSet,
	int propertyIndex,
	int index) {
	dataSet->requiredProperties.count++;
}

/**
 * \cond
 * Callback method used to set the property index of the required property.
 * @param dataSet containing the required properties
 * @param propertyIndex the index of the property in the requiredProperties
 * @param index of the required property
 * \endcond
 */
static void addRequiredProperty(
	fiftyoneDegreesDataSet *dataSet,
	int propertyIndex,
	int index) {
	dataSet->requiredProperties.firstElement[index] = (int32_t)propertyIndex;
}

/**
 * \cond
 * Used to order the property name strings in ascending order.
 * @param a pointer to a property name string
 * @param b pointer to a property name string
 * @return 0 if the names are the same, otherwise -1 or 1 to indicate order
 * \endcond
 */
static int comparePropertyNamesAscending(const void *a, const void *b) {
	return strcmp(*(const char**)a, *(const char**)b);
}

/**
 * \cond
 * Sorts the required property names in ascending order. Then resets the 
 * associated property index for the corresponding element index in the 
 * requiredProperties array. Used to improve performance when searching
 * for a property index by name.
 * @param dataSet to have the required properties sorted
 * \endcond
 */
static void initRequiredPropertiesSort(fiftyoneDegreesDataSet *dataSet) {
	unsigned int i;
	const char* propertyName;

	// Sort the required property names in ascending order.
	qsort((void*)dataSet->requiredPropertiesNames,
		  dataSet->requiredProperties.count,
		  sizeof(const char*),
		  comparePropertyNamesAscending);

	// Set the property indexes to match.
	for (i = 0; i < dataSet->requiredProperties.count; i++) {
		propertyName = dataSet->requiredPropertiesNames[i];
		dataSet->requiredProperties.firstElement[i] = getPropertyIndexRange(
			dataSet,
			propertyName,
			strlen(propertyName));
	}
}

/**
 * \cond
 * References the property name string in the strings data structure to the
 * required property index. Then sorts the required property names and indexes
 * in ascending order of property name. Where indirect operation is configured
 * memory is allocated to hold a copy of the property name string. This is 
 * needed because the strings are not stored in memory but in the cache and 
 * could be evicted during general operation.
 * @param dataSet to have it's required proeprty names set
 * @return the status of the initialise result
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus initRequiredPropertyNames(
		fiftyoneDegreesDataSet *dataSet) {
	unsigned int i;
	char *propertyString;

	// Allocate sufficient memory to store the pointers to the property
	// name strings.
	dataSet->requiredPropertiesNames = (const char**)fiftyoneDegreesMalloc(
		dataSet->requiredProperties.count * sizeof(const char*));
	if (dataSet->requiredPropertiesNames == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Loop through the required property indexes setting the string.
	for (i = 0; i < dataSet->requiredProperties.count; i++) {
		propertyString = STRING(dataSet, 
			PROPERTY(dataSet,
			dataSet->requiredProperties.firstElement[i])->stringOffset);
#ifdef FIFTYONEDEGREES_INDIRECT
		// The current property string value has come from the cache so memory
		// needs to be allocated to copy the value to as it may be removed 
		// from the cache in the future.
		dataSet->requiredPropertiesNames[i] =
			(const char*)fiftyoneDegreesMalloc(sizeof(char*) *
				(strlen(propertyString) + 1));
		strcpy(
			(char*)dataSet->requiredPropertiesNames[i],
			propertyString);
#else
		// Store the pointer directly to the property string.
		dataSet->requiredPropertiesNames[i] = propertyString;
#endif
	}

	// Sort the properties in ascending order of the property name.
	initRequiredPropertiesSort(dataSet);

	return DATA_SET_INIT_STATUS_SUCCESS;
}

/**
 * \cond
 * Initialises the properties string provided.
 * @param dataSet to initialise the required properties for.
 * @param properties a comma separated properties string.
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus initSpecificProperties(
		fiftyoneDegreesDataSet *dataSet, 
		const char* properties) {
	// Count the number of valid properties.
	dataSet->requiredProperties.count = 0;
	iterateProperties(dataSet, properties, increaseRequiredPropertiesCount);
	
	// If no properties are needed then set the memory to NULL and return.
	if (dataSet->requiredProperties.count == 0) {
		dataSet->requiredProperties.firstElement = NULL;
		return DATA_SET_INIT_STATUS_SUCCESS;
	}

	// Create enough memory for the required property indexes and the names.
	dataSet->requiredProperties.firstElement = (int32_t*)fiftyoneDegreesMalloc(
		dataSet->requiredProperties.count * sizeof(int32_t));
	if (dataSet->requiredProperties.firstElement == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Iterate again through the properties setting the indexes.
	iterateProperties(dataSet, properties, addRequiredProperty);

	// Set the require property name strings.
	return initRequiredPropertyNames(dataSet);
}

/**
 * \cond
 * Initialises the properties array provided.
 * @param dataSet to initialise the required properties for.
 * @param properties an array of property strings.
 * @param count the number of properties in the array.
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus initSpecificPropertiesFromArray(
		fiftyoneDegreesDataSet *dataSet, 
		const char** properties,
		int count) {

	// Count the number of valid properties.
	dataSet->requiredProperties.count = 0;
	iteratePropertiesFromArray(
		dataSet, 
		properties, 
		count, 
		increaseRequiredPropertiesCount);

	// If no properties are needed then set the memory to NULL and return.
	if (dataSet->requiredProperties.count == 0) {
		dataSet->requiredProperties.firstElement = NULL;
		return DATA_SET_INIT_STATUS_SUCCESS;
	}

	// Create enough memory for the required property indexes and the names.
	dataSet->requiredProperties.firstElement = (int32_t*)fiftyoneDegreesMalloc(
		dataSet->requiredProperties.count * sizeof(int32_t));
	if (dataSet->requiredProperties.firstElement == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Iterate again through the properties setting the indexes.
	iteratePropertiesFromArray(
		dataSet,
		properties,
		count,
		addRequiredProperty);

	// Set the require property name strings.
	return initRequiredPropertyNames(dataSet);
}

/**
 * \cond
 * Initialises all available properties.
 * @param dataSet to initialise the required properties for.
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus initAllProperties(
		fiftyoneDegreesDataSet *dataSet) {
	unsigned int i;

	// The required properties count is the same as all the available 
	// properties.
	dataSet->requiredProperties.count = dataSet->allProperties.count;

	// Create enough memory for the required property indexes and the names.
	dataSet->requiredProperties.firstElement = (int32_t*)fiftyoneDegreesMalloc(
		dataSet->requiredProperties.count * sizeof(int32_t));
	if (dataSet->requiredProperties.firstElement == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Set the required indexes to match the property in the all properties 
	// list.
	for (i = 0; i < dataSet->allProperties.count; i++) {
		dataSet->requiredProperties.firstElement[i] = (int32_t)i;
	}

	// Set the require property name strings.
	return initRequiredPropertyNames(dataSet);
}

/**
 * \cond
 * Initialises the unique HTTP headers in the dataset.
 * @param dataSet to initialise the headers in.
 * @returns fiftyoneDegreesDataSetInitStatus indicates whether the init was 
 * 	   successful.
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus initUniqueHttpHeaders(
		fiftyoneDegreesDataSet *dataSet) {
	unsigned int headerIndex, uniqueHeaderIndex;
	// Allocate more space than is necessary just in case.
	dataSet->uniqueHttpHeaders.firstElement = (int32_t*)fiftyoneDegreesMalloc(
		dataSet->httpHeaders.count * sizeof(int32_t));
	if (dataSet->uniqueHttpHeaders.firstElement == NULL) {
		fiftyoneDegreesFree(dataSet->uniqueHttpHeaders.firstElement);
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	dataSet->uniqueHttpHeaders.freeMemory = TRUE;

	// Set the unique HTTP header names;
	dataSet->uniqueHttpHeaders.count = 0;
	for (headerIndex = 0; 
		 headerIndex < dataSet->httpHeaders.count;
		 headerIndex++) {
		for (uniqueHeaderIndex = 0; 
			 uniqueHeaderIndex < dataSet->uniqueHttpHeaders.count;
			 uniqueHeaderIndex++) {
			if (dataSet->uniqueHttpHeaders.firstElement[uniqueHeaderIndex] ==
				HTTPHEADER(dataSet, headerIndex)) {
				break;
			}
		}
		if (uniqueHeaderIndex == dataSet->uniqueHttpHeaders.count) {
			dataSet->uniqueHttpHeaders.firstElement
				[dataSet->uniqueHttpHeaders.count] = 
				HTTPHEADER(dataSet, headerIndex);
			dataSet->uniqueHttpHeaders.count++;
		}
	}
	return DATA_SET_INIT_STATUS_SUCCESS;
}

#ifdef FIFTYONEDEGREES_INDIRECT

/**
 * Sets the node to indicate the load failed. This will happen very rarely. 
 */
static void loaderFailure(fiftyoneDegreesCacheNode *cacheNode) {
	if (cacheNode->data != NULL) {
		cacheNode->cache->freeCacheData(cacheNode->data);
	}
	cacheNode->data = NULL;
	cacheNode->size = 0;
}

/**
 * Loads chunks of data starting at the offset provided. The length of the data
 * loaded is dependent on the elementSize field of the loader parameters 
 * associated with the cache. Sets the cacheNode->data pointer to the chunk
 * of bytes loaded. Allocated memory if the cacheNode has not be used 
 * previously.
 */
static void byteOffsetLoader(
	fiftyoneDegreesCacheNode *cacheNode,
	const long offset) {
	fiftyoneDegreesCollection *params = 
		(fiftyoneDegreesCollection*)cacheNode->cache->params;

	// Move to the start of the record in the file.
	if (fseek(
		params->handle,
		params->offset + offset,
		SEEK_SET) != 0) {
		loaderFailure(cacheNode);
		return;
	}

	// Allocate memory if this is the first time the node is being used.
	if (cacheNode->data == NULL) {
		cacheNode->data = cacheNode->cache->mallocCacheData(
			params->elementSize);
		cacheNode->size = params->elementSize;
	}

	// Read the record from file to the cache node's data field which was
	// preallocated when the cache was created.
	if (fread(
		cacheNode->data,
		params->elementSize,
		1,
		params->handle) != 1) {
		loaderFailure(cacheNode);
		return;
	}
}

/**
* Fixed loader to load fixed size records from file to the cache, where the
* key is the record's index in the data structure.
*/
static void integerLoader(
		fiftyoneDegreesCacheNode *cacheNode,
		const long index) {
	byteOffsetLoader(cacheNode, index * sizeof(int32_t));
}

/**
 * Strings loader to load strings from file to the strings cache, where the key
 * is the offset in the strings structure where the record starts.
 */
static void stringsLoader(
	fiftyoneDegreesCacheNode *cacheNode,
	const long stringOffset) {
	int16_t length = 0;
	fiftyoneDegreesCollection *params =
		(fiftyoneDegreesCollection*)cacheNode->cache->params;

	// Move to the start of the string in the file.
	if (fseek(
		params->handle,
		params->offset + stringOffset,
		SEEK_SET) != 0) {
		loaderFailure(cacheNode);
		return;
	}

	// Get the length of the string. This is stored as the first two bytes 
	// after the offset.
	if (fread(&length, sizeof(int16_t), 1, params->handle) != 1) {
		loaderFailure(cacheNode);
		return;
	}

	// Force the allocation of more memory if the node does not contain 
	// sufficient memory.
	if (cacheNode->data != NULL && length > cacheNode->size) {
		cacheNode->cache->freeCacheData(cacheNode->data);
		cacheNode->data = NULL;
		cacheNode->size = 0;
	}

	// Allocate memory if needed.
	if (cacheNode->data == NULL) {
		cacheNode->data = cacheNode->cache->mallocCacheData(length);
		cacheNode->size = length;
	}

	// Read the string into the node's data field.
	if (fread(cacheNode->data, length, 1, params->handle) != 1) {
		loaderFailure(cacheNode);
		return;
	}
}

/**
 * Nodes loader to load nodes from file to the nodes cache.
 */
static void nodeLoader(
	fiftyoneDegreesCacheNode *cacheNode,
	const long nodeOffset) {
	fiftyoneDegreesSignatureNode nodeHeader;
	size_t nodeSize;
	fiftyoneDegreesCollection *params =
		(fiftyoneDegreesCollection*)cacheNode->cache->params;
	long startPos = params->offset + nodeOffset;
	
	// Move to the start of the node in the file.
	if (fseek(params->handle, startPos, SEEK_SET) != 0) {
		loaderFailure(cacheNode);
		return;
	}

	// Read the node header so the total size can be calculated.
	if (fread(
		&nodeHeader,
		sizeof(fiftyoneDegreesSignatureNode),
		1,
		params->handle) != 1) {
		loaderFailure(cacheNode);
		return;
	}
	
	nodeSize = sizeof(fiftyoneDegreesSignatureNode) +
		sizeof(fiftyoneDegreesSignatureNodeHash) * nodeHeader.hashesCount;
	
	// Force the allocation of more memory if needed.
	if (cacheNode->data != NULL && nodeSize > cacheNode->size) {
		cacheNode->cache->freeCacheData(cacheNode->data);
		cacheNode->data = NULL;
		cacheNode->size = 0;
	}

	// Allocate memory if needed.
	if (cacheNode->data == NULL) {
		cacheNode->data = cacheNode->cache->mallocCacheData(nodeSize);
		cacheNode->size = nodeSize;
	}
	
	// Read the whole node into the node's data field.
	if (fseek(params->handle, startPos, SEEK_SET) != 0 ||
		fread(cacheNode->data, nodeSize, 1, params->handle) != 1) {
		loaderFailure(cacheNode);
		return;
	}
}

#else

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
 *                number of bytes provided in advanceBy.
 * @param lastByte pointer to the last valid byte in the memory space. A
 *                 corrupt memory response is return if this is exceeded.
 * @param advanceBy number of bytes to advance the pointer by.
 * @return fiftyoneDegreesDataSetInitStatus stating the result of the
 * 	   current advance operation. Codes other than
 * 	   DATA_SET_INIT_STATUS_SUCCESS indicate there is a problem.
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus advancePointer(
		memoryReader *reader,
		size_t advanceBy) {
	if (reader == NULL || reader->current == NULL) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}
	reader->current += advanceBy;
	if (reader->current > reader->lastByte) {
		return DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS;
	}
	return DATA_SET_INIT_STATUS_SUCCESS;
}

#endif

/**
 * Reads into memory the fixed size elements at the readers current position.
 * Moves the current file pointer over the memory consumed reading the data.
 * @param reader points to the data file pointer
 * @param list the list to read data into
 * @param sizeOfElement the length in bytes of each element in the list
 * @return the status following the read operation
 */
static fiftyoneDegreesDataSetInitStatus readListToMemory(
#ifdef FIFTYONEDEGREES_INDIRECT
	fileReader *reader,
#else
	memoryReader *reader,
#endif
	fiftyoneDegreesList *list,
	size_t sizeOfElement) {
	
#ifdef FIFTYONEDEGREES_INDIRECT
	size_t bytesToRead;

	// Get the number of elements the array will contain.
	if (fread((void*)&list->count, sizeof(uint32_t), 1, reader->file) != 1) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}

	// Allocate the memory for the elements.
	bytesToRead = list->count * sizeOfElement;
	list->firstElement = (byte*)fiftyoneDegreesMalloc(bytesToRead);

	// Check the memory was allocated correctly.
	if (list->firstElement == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Mark the memory to be freed when the data set is disposed of.
	list->freeMemory = TRUE;

	// Read the data if the memory was allocated. If the read fails then
	// free the memory allocated.
	if (fread(list->firstElement, bytesToRead, 1, reader->file) != 1) {
		fiftyoneDegreesFree(list->firstElement);
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}

	return DATA_SET_INIT_STATUS_SUCCESS;
#else
	// Gets the number of bytes that the returned pointer will contain.
	list->count = *(const uint32_t*)reader->current;

	// Point the structure to the first byte.
	list->firstElement = reader->current + sizeof(uint32_t);

	// Mark the memory not to be freed as it'll be disposed of when the memory
	// containing the data file is disposed of.
	list->freeMemory = FALSE;

	// Move over the structure and the size integer.
	return advancePointer(
		reader,
		(list->count * sizeOfElement) + sizeof(uint32_t));
#endif
}

/**
 * Reads into memory or the cache depending on the mode of operation the 
 * variable width elements at the current position of the reader.
 * Moves the current file pointer over the memory consumed reading the data.
 * @param reader points to the data file pointer
 * @param collection stores the entity information for future retrieval
 * @return the status following the read operation
 */
static fiftyoneDegreesDataSetInitStatus readVariableCollection(
#ifdef FIFTYONEDEGREES_INDIRECT
		fileReader *reader,
#else
		memoryReader *reader,
#endif
		fiftyoneDegreesCollection *collection) {
	int32_t dataSize;
#ifdef FIFTYONEDEGREES_INDIRECT
	// Check that the cache has been created correctly.
	if (collection->cache == NULL) {
		return DATA_SET_INIT_STATUS_NULL_POINTER;
	}

	// Get the size of the data structure in bytes.
	if (fread((void*)&dataSize, sizeof(int32_t), 1, reader->file) != 1) {
		return DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS;
	}

	// Record in the loader the start of the actual data structure.
	collection->offset = ftell(reader->file);

	// Move to the next data structure.
	if (fseek(reader->file, dataSize, SEEK_CUR) != 0) {
		return DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS;
	}
	return DATA_SET_INIT_STATUS_SUCCESS;
#else
	// Gets the number of bytes that the returned pointer will contain.
	dataSize = *(uint32_t*)reader->current;

	// Point the structure to the first byte.
	collection->firstByte = reader->current + sizeof(uint32_t);

	// Move over the structure and the size integer.
	return advancePointer(
		reader,
		dataSize + sizeof(uint32_t));
#endif
}

/**
 * Reads into memory or the cache depending on the mode of operation the
 * fixed width elements at the current position of the reader.
 * Moves the current file pointer over the memory consumed reading the data.
 * @param reader points to the data file pointer
 * @param collection stores the entity information for future retrieval
 * @param sizeOfElement the length in bytes of each element in the collection
 * @return the status following the read operation
 */
static fiftyoneDegreesDataSetInitStatus readFixedCollection(
#ifdef FIFTYONEDEGREES_INDIRECT
	fileReader *reader,
#else
	memoryReader *reader,
#endif
	fiftyoneDegreesCollection *collection,
	long elementSize) {
	int32_t dataSize;
#ifdef FIFTYONEDEGREES_INDIRECT

	// Check that the cache has been created correctly.
	if (collection->cache == NULL) {
		return DATA_SET_INIT_STATUS_NULL_POINTER;
	}

	// Get the number of elements the array will contain.
	if (fread((void*)&dataSize, sizeof(int32_t), 1, reader->file) != 1) {
		return DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS;
	}

	// Record in the loader the start of the actual data structure.
	collection->offset = ftell(reader->file);

	// Move to the next data structure.
	if (fseek(reader->file, dataSize, SEEK_CUR) != 0) {
		return DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS;
	}
	return DATA_SET_INIT_STATUS_SUCCESS;
#else
	// Gets the number of bytes that the returned pointer will contain.
	dataSize = *(int32_t*)reader->current;

	// Point the structure to the first byte.
	collection->firstByte = reader->current + sizeof(uint32_t);

	// Set the count so that the structure is complete.
	collection->count = dataSize / elementSize;

	// Move over the structure and the size integer.
	return advancePointer(
		reader,
		dataSize + sizeof(uint32_t));
#endif
}

/**
* Reads the data set header into the beginning of the data set.
* The file pointer must be at the beginning of the file,
* Last byte is not used in the indirect data set.
* Moves the current file pointer over the memory consumed reading the data.
* @param filePtr points to the data file pointer
* @param header set during the read operation
* @param lastByte not used in indirect data set
* @return the status following the read operation
*/
fiftyoneDegreesDataSetInitStatus readHeader(
#ifdef FIFTYONEDEGREES_INDIRECT
	fileReader *reader,
#else
	memoryReader *reader,
#endif
	const fiftyoneDegreesDataSetHeader *header) {

#ifdef FIFTYONEDEGREES_INDIRECT
	// Read the bytes that make up the dataset header.
	if (fread(
		(void*)header,
		sizeof(fiftyoneDegreesDataSetHeader),
		1,
		reader->file) != 1) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}

	return DATA_SET_INIT_STATUS_SUCCESS;
#else
	// Copy the bytes that make up the dataset header.
	if (memcpy(
		(void*)header,
		(const void*)reader->current,
		sizeof(fiftyoneDegreesDataSetHeader)) != header) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}

	// Move the current pointer to the next data structure.
	return advancePointer(
		reader,
		sizeof(fiftyoneDegreesDataSetHeader));
#endif
}

#ifdef FIFTYONEDEGREES_INDIRECT

static fiftyoneDegreesCache* createCache(
	int32_t size,
	void(*load)(fiftyoneDegreesCacheNode*, long),
	fiftyoneDegreesCollection *params) {
	// Set uninitialised values. These are set as the data set is read.
	params->elementSize = 0;
	params->handle = NULL;
	params->offset = 0;

	// Create the cache using the provided malloc and free methods.
	return fiftyoneDegreesCacheCreate(
		size,
		fiftyoneDegreesMalloc,
		fiftyoneDegreesFree,
		load,
		params);
}

static fiftyoneDegreesDataSetInitStatus initCache(
		fiftyoneDegreesDataSet *dataSet) {
	fiftyoneDegreesDataSetInitStatus status;

	// Create all the caches ready to have the start location in the file added
	// to them.
	dataSet->strings.cache = createCache(
		FIFTYONEDEGREES_STRING_CACHE_SIZE,
		stringsLoader,
		&dataSet->strings);
	dataSet->profiles.cache = createCache(
		FIFTYONEDEGREES_PROFILE_CACHE_SIZE,
		byteOffsetLoader,
		&dataSet->profiles);
	dataSet->devices.cache = createCache(
		FIFTYONEDEGREES_DEVICE_CACHE_SIZE,
		integerLoader,
		&dataSet->devices);
	dataSet->nodes.cache = createCache(
		FIFTYONEDEGREES_NODE_CACHE_SIZE,
		nodeLoader,
		&dataSet->nodes);

	// Set file pointers for each of the caches based on the data set filename.
	status = createFileHandle(dataSet->fileName, &dataSet->strings.handle);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}
	status = createFileHandle(dataSet->fileName, &dataSet->profiles.handle);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}
	status = createFileHandle(dataSet->fileName, &dataSet->devices.handle);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}
	return createFileHandle(dataSet->fileName, &dataSet->nodes.handle);
}
#endif

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
 * 	 51Degrees pattern data file. Or a file pointer if an indirect data set is
 *   being initialised. Can not be NULL.
 * @param dataSet to be initialised with data from the provided pointer to
 * 	 continuous memory space.
 * @param length number of bytes that the file occupies in memory.
 * 	 Also corresponds to the last byte within the continuous memory
 * 	 space.
 * @return dataset initialisation status.
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus readDataSet(
#ifndef FIFTYONEDEGREES_INDIRECT
	memoryReader *source,
#else
	fileReader *source,
#endif
	fiftyoneDegreesDataSet *dataSet) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;

#ifdef FIFTYONEDEGREES_INDIRECT
	// Initialise the caches in the data set ready to read the data set.
	status = initCache(dataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}
#endif
	
	// Copy the bytes that form the header from the start of the memory
	// location to the data set pointer provided.
	status = readHeader(source, &dataSet->header);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}

	/* Check the version of the data file */
	if (dataSet->header.version != TARGET_VERSION) {
		return DATA_SET_INIT_STATUS_INCORRECT_VERSION;
	}

	// Read the strings.
	status = readVariableCollection(source, &dataSet->strings);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}

	// Read the components.
	status = READ_INTEGERS(source, &dataSet->components);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return DATA_SET_INIT_STATUS_SUCCESS;
	}

	// Read the HTTP headers.
	status = READ_INTEGERS(source, &dataSet->httpHeaders);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}

	// Read the number of properties contained directly in a device.
#ifdef FIFTYONEDEGREES_INDIRECT
	if (fread(
		&dataSet->devicePropertiesCount,
		sizeof(int32_t),
		1,
		source->file) != 1) {
		status = DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS;
	}
#else
	dataSet->devicePropertiesCount = *(int32_t*)source->current;
	status = advancePointer(source, sizeof(int32_t));
#endif
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}

	// Read all the properties.
	status = READ_PROPERTIES(source, &dataSet->allProperties);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}
	
#ifdef FIFTYONEDEGREES_INDIRECT
	// Set the record sizes for profiles and devices now that the properties
	// count is known.
	dataSet->profiles.elementSize =
		dataSet->allProperties.count * sizeof(int32_t);
	dataSet->devices.elementSize = 
		(dataSet->components.count + dataSet->devicePropertiesCount) *
		sizeof(int32_t);
#endif

	// Set the number of integers that make a device record. This is the 
	// number of components plus the number of properties that are associated
	// specifically with a device. This could be 4 components plus one device
	// specific property such as the unique id of the device.
	dataSet->devicesIntegerCount =
		dataSet->components.count + dataSet->devicePropertiesCount;

	// Read the profiles.
	status = readFixedCollection(
		source, 
		&dataSet->profiles,
		sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}
	
	// Read the devices.
	status = readFixedCollection(
		source,
		&dataSet->devices,
		sizeof(int32_t));
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}
	
	// Read the nodes.
	status = readVariableCollection(source, &dataSet->nodes);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}
	
	/* Check that the current pointer equals the last byte */
#ifdef FIFTYONEDEGREES_INDIRECT
	if (ftell(source->file) != source->length) {
#else
	if (source->current != source->lastByte) {
#endif
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
 * 	  memory location.
 * @param source pointer to continuous memory space containing decompressed
 * 	 51Degrees pattern data file. Or a file pointer if an indirect data set is
 *   being initialised. Can not be NULL
 * @param length number of bytes that the file occupies in memory.
 * @return dataset initialisation status.
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus initDataSet(
	fiftyoneDegreesDataSet *dataSet,
#ifndef FIFTYONEDEGREES_INDIRECT
	memoryReader *reader
#else
	fileReader *reader
#endif
	) {
	fiftyoneDegreesDataSetInitStatus status;

	if (reader == NULL) {
		return DATA_SET_INIT_STATUS_NULL_POINTER;
	}

	// Set data set pointer fields to null so that only memory that is actually
	// allocated can be freed in the event of a problem.
	dataSet->requiredPropertiesNames = NULL;
	dataSet->requiredProperties.firstElement = NULL;
	dataSet->prefixedUpperHttpHeaders = NULL;
	dataSet->uniqueHttpHeaders.firstElement = NULL;
	dataSet->httpHeaders.firstElement = NULL;
	dataSet->components.firstElement = NULL;
	dataSet->allProperties.firstElement = NULL;

	// Read the data set from the source.
	status = readDataSet(reader, dataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesDataSetFree(dataSet);
		return status;
	}

	status = initUniqueHttpHeaders(dataSet);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesDataSetFree(dataSet);
		return status;
	}

	// Set the drift and difference to 0, these can be set later by the
	// fiftyoneDegreesSetDrift and fiftyoneDegreesSetDifference methods.
	dataSet->baseDrift = 0;
	dataSet->baseDifference = 0;

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
	fiftyoneDegreesDataSetInitStatus status;
#ifdef FIFTYONEDEGREES_INDIRECT
	fileReader reader;
#else
	memoryReader reader;
#endif
	// Set the file name of the data set.
	status = setDataSetFileName(dataSet, fileName);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}

	status = createFileHandle(fileName, &inputFilePtr);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		return status;
	}

	// If the file didn't open return not found.
	if (inputFilePtr == NULL) {
		return DATA_SET_INIT_STATUS_FILE_NOT_FOUND;
	}

	// Find the length of the file by moving to the end.
	if (fseek(inputFilePtr, 0, SEEK_END) != 0) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}
	reader.length = ftell(inputFilePtr);
	if (reader.length <= 0) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}
	if (fseek(inputFilePtr, 0, SEEK_SET) != 0) {
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}

#ifdef FIFTYONEDEGREES_INDIRECT
	// Set the file pointer and unset the memory to free as the dataset
	// will be staying in the file.
	reader.file = inputFilePtr;

	// Initialises the data set using the file pointer just opened.
	status = initDataSet(dataSet, &reader);
#else
	// Allocate the memory and read in the data file.
	dataSet->memoryToFree = reader.current = 
		(byte*)fiftyoneDegreesMalloc(reader.length);
	if (dataSet->memoryToFree == NULL)  {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	if (fread(reader.current, reader.length, 1, inputFilePtr) != 1) {
		fiftyoneDegreesFree((void*)dataSet->memoryToFree);
		return DATA_SET_INIT_STATUS_CORRUPT_DATA;
	}

	// Set the last byte to value which current should be when reading the
	// data set is complete.
	reader.lastByte = reader.current + reader.length;
	
	// Initialises the data set using the memory just allocated.
	status = initDataSet(dataSet, &reader);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesFree((void*)dataSet->memoryToFree);
		return status;
	}
#endif

	// Close the handle to the file. Indirect mode caches will have their own
	// handles on the data file.
	fclose(inputFilePtr);

	return status;
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
static fiftyoneDegreesDataSetInitStatus initProvider(
	fiftyoneDegreesProvider *provider,
	fiftyoneDegreesDataSet *dataSet) {
	fiftyoneDegreesActiveDataSet *active;

	// Create a new active wrapper for the provider.
	active = (fiftyoneDegreesActiveDataSet*)fiftyoneDegreesMalloc(
		sizeof(fiftyoneDegreesActiveDataSet));
	if (active == NULL) {
		fiftyoneDegreesDataSetFree(dataSet);
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Set the number of offsets using the active dataset to zero.
	active->inUse = 0;

	// Set a link between the new active wrapper and the provider. Used to
	// check if the dataset can be freed when the last thread has finished
	// using it.
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
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyString(
		const char* fileName,
		fiftyoneDegreesDataSet *dataSet,
		const char* properties) {
	fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;
	status = initFromFile(dataSet, fileName);
	if (status == DATA_SET_INIT_STATUS_SUCCESS) {
		// If no properties are provided then use all of them.
		if (properties == NULL || strlen(properties) == 0) {
			initAllProperties(dataSet);
		}
		else {
			initSpecificProperties(dataSet, properties);
		}
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
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyArray(
		const char* fileName,
		fiftyoneDegreesDataSet *dataSet,
		const char** properties,
		int propertyCount) {
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
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitProviderWithPropertyString(
		const char* fileName,
		fiftyoneDegreesProvider* provider,
		const char* properties) {
	fiftyoneDegreesDataSetInitStatus status;
	fiftyoneDegreesDataSet *dataSet =
		(fiftyoneDegreesDataSet*)fiftyoneDegreesMalloc(
			sizeof(fiftyoneDegreesDataSet));
	if (dataSet == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	status = fiftyoneDegreesInitWithPropertyString(
		fileName,
		dataSet,
		properties);
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
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitProviderWithPropertyArray(
		const char* fileName,
		fiftyoneDegreesProvider *provider,
		const char ** properties,
		int propertyCount) {
	fiftyoneDegreesDataSetInitStatus status;
	fiftyoneDegreesDataSet *dataSet =
		(fiftyoneDegreesDataSet*)fiftyoneDegreesMalloc(
			sizeof(fiftyoneDegreesDataSet));
	if (dataSet == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}
	status = fiftyoneDegreesInitWithPropertyArray(
		fileName,
		dataSet,
		properties,
		propertyCount);
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
 * \endcond
 */
static fiftyoneDegreesDataSetInitStatus reloadCommon(
		fiftyoneDegreesProvider *provider, 
		fiftyoneDegreesDataSet *newDataSet) {
	const fiftyoneDegreesActiveDataSet *oldActive;
	fiftyoneDegreesDataSetInitStatus status;

	// Maintain a reference to the current active wrapper in case it can be
	// freed.
	oldActive = (const fiftyoneDegreesActiveDataSet*)provider->active;

	// Initialise the new dataset with the same properties as the old one.
	status = initSpecificPropertiesFromArray(
		newDataSet,
		oldActive->dataSet->requiredPropertiesNames,
		oldActive->dataSet->requiredProperties.count);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		fiftyoneDegreesDataSetFree(newDataSet);
		return status;
	}

	// Set the tolerances from the old dataset.
	newDataSet->baseDrift = oldActive->dataSet->baseDrift;
	newDataSet->baseDifference = oldActive->dataSet->baseDifference;

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
		fiftyoneDegreesActiveDataSetFree(
			(fiftyoneDegreesActiveDataSet*)oldActive);
	}
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_UNLOCK(&provider->lock);
#endif
	return DATA_SET_INIT_STATUS_SUCCESS;
}

#ifndef FIFTYONEDEGREES_INDIRECT

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
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesProviderReloadFromMemory(
		fiftyoneDegreesProvider *provider,
		void *source,
		long length) {
	fiftyoneDegreesDataSetInitStatus status;
	fiftyoneDegreesDataSet *newDataSet = NULL;
	memoryReader reader;

	// Allocate memory for a new data set.
	newDataSet = (fiftyoneDegreesDataSet*)fiftyoneDegreesMalloc(
		sizeof(fiftyoneDegreesDataSet));
	if (newDataSet == NULL) {
		return DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY;
	}

	// Set the file name of the data set to NULL as there is no longer a file
	// that the data set relates to. This is needed to prevent the free dataset
	// method from freeing memory that was not allocated.
	newDataSet->fileName = NULL;

	// Set the full data set pointer to NULL to indicate that when this
	// new data set is released the memory shouldn't be freed by 51Degrees but
	// by the consumer.
	newDataSet->memoryToFree = NULL;

	// Initialise the new data set with the data pointed to by source.
	reader.current = (byte*)source;
	reader.length = length;
	reader.lastByte = reader.current + length;
	status = initDataSet(newDataSet, &reader);
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

#endif

/**
 * \cond
 * Creates a new dataset using the same configuration options as the current
 * data set associated with the provider. The data file which the provider was
 * initialised with is used to create the new data set.
 * @param provider pointer to the provider whose data set should be reloaded
 * @return fiftyoneDegreesDataSetInitStatus indicating the result of the reload
 * 	   operation.
 * \endcond
 */
fiftyoneDegreesDataSetInitStatus fiftyoneDegreesProviderReloadFromFile(
		fiftyoneDegreesProvider* provider) {
	fiftyoneDegreesDataSetInitStatus status;
	fiftyoneDegreesDataSet *newDataSet = NULL;

	// Allocate memory for a new data set.
	newDataSet = (fiftyoneDegreesDataSet*)fiftyoneDegreesMalloc(
		sizeof(fiftyoneDegreesDataSet));
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
 * Releases all the resources used by the provider. The provider can not be used 
 * without being reinitialised after calling this method.
 * @param provider pointer to the provider to be freed.
 * \endcond
 */
void fiftyoneDegreesProviderFree(fiftyoneDegreesProvider* provider) {
	fiftyoneDegreesActiveDataSetFree(
		(fiftyoneDegreesActiveDataSet*)provider->active);
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
 * @returns size_t the size the file will need in memory, or 0 if the file
 * could not be opened.
 * \endcond
 */
static size_t getSizeOfFile(const char* fileName) {
	size_t sizeOfFile = 0;
	FILE *inputFilePtr;

	// Open the file and hold on to the pointer.
#ifndef _MSC_FULL_VER
	inputFilePtr = fopen(fileName, "rb");
	if (inputFilePtr == NULL) {
		return 0;
	}
#else
	/* If using Microsoft use the fopen_s method to avoid warning */
	if (fopen_s(&inputFilePtr, fileName, "rb") != 0) {
		return 0;
	}
#endif
	if (fseek(inputFilePtr, 0, SEEK_END) != 0) {
		return 0;
	}

	// Add file size.
	sizeOfFile = ftell(inputFilePtr);
	fclose(inputFilePtr);

	assert(sizeOfFile > 0);

	return sizeOfFile;
}

/**
 * \cond
 * Get the number of entries for a specific entity in the data set from the
 * file without reading it all into memory. This function is used in the
 * getProviderSizeWithPropertyString when the property string is empty, and to
 * get the number of HTTP headers.
 * @param fileName path to a valid data file.
 * @param entityName of the entity to get the count of.
 * @returns int the number of properties in the data file.
 * \endcond
 */
static int getEntityCountFromFile(
		const char* fileName,
		const char* entityName) {
	FILE *inputFilePtr;
	unsigned int size;

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

	// Skip over the header.
	fseek(inputFilePtr, sizeof(fiftyoneDegreesDataSetHeader), SEEK_SET);

	// Skip over the strings.
	if (fread(&size, sizeof(uint32_t), 1, inputFilePtr) != 1) {
		return -1;
	}
	if (fseek(inputFilePtr, size, SEEK_CUR) != 0) {
		return -1;
	}

	// Skip over the components.
	if (fread(&size, sizeof(uint32_t), 1, inputFilePtr) != 1) {
		return -1;
	}
	if (fseek(inputFilePtr, size * sizeof(uint32_t), SEEK_CUR) != 0) {
		return -1;
	}
	if (strcmp("components", entityName) == 0) {
		fclose(inputFilePtr);
		return size;
	}

	// Skip over the HTTP headers.
	if (fread(&size, sizeof(uint32_t), 1, inputFilePtr) != 1) {
		return -1;
	}
	if (fseek(inputFilePtr, size * sizeof(uint32_t), SEEK_CUR) != 0) {
		return -1;
	}
	if (strcmp("httpheaders", entityName) == 0) {
		fclose(inputFilePtr);
		return size;
	}

	// Skip over the number of device properties.
	if (fseek(inputFilePtr, sizeof(uint32_t), SEEK_CUR) != 0) {
		return -1;
	}
	if (strcmp("deviceproperties", entityName) == 0) {
		fclose(inputFilePtr);
		return size;
	}

	// Read the number of properties.
	if (fread(&size, sizeof(uint32_t), 1, inputFilePtr) != 1) {
		return -1;
	}
	if (strcmp("properties", entityName) == 0) {
		fclose(inputFilePtr);
		return size;
	}

	// Close the file pointer.
	fclose(inputFilePtr);

	// Return the number of properties in the data file.
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
size_t fiftyoneDegreesGetProviderSizeWithPropertyCount(
	const char* fileName,
	int propertyCount) {

	int httpHeaderCount = getEntityCountFromFile(fileName, "httpheaders");

	size_t size = 0;

#ifdef FIFTYONEDEGREES_INDIRECT
	int devicePropertiesCount =
		getEntityCountFromFile(fileName, "deviceproperties");
	int propertiesCount = getEntityCountFromFile(fileName, "properties");
	int componentsCount = getEntityCountFromFile(fileName, "components");

	// Add the size of the allocated arrays.
	size += sizeof(uint32_t) * componentsCount;
	size += sizeof(uint32_t) * httpHeaderCount;
	size += sizeof(fiftyoneDegreesProperty) * propertiesCount;

	// todo file handle?
	size += sizeof(FILE*);
	// Add the size of the strings cache.
	size += sizeof(fiftyoneDegreesCache) + (sizeof(fiftyoneDegreesCacheNode) +
		sizeof(FIFTYONEDEGREES_MAX_STRING)) * FIFTYONEDEGREES_STRING_CACHE_SIZE;
	// Add the size of the devices cache.
	size += sizeof(fiftyoneDegreesCache) + (sizeof(fiftyoneDegreesCacheNode) +
		(sizeof(int32_t) * (componentsCount + devicePropertiesCount)))
		* FIFTYONEDEGREES_DEVICE_CACHE_SIZE;
	// Add the size of the profiles cache.
	size += sizeof(fiftyoneDegreesCache) + (sizeof(fiftyoneDegreesCacheNode) +
		(sizeof(int32_t) * propertyCount)) * FIFTYONEDEGREES_PROFILE_CACHE_SIZE;
	// As the nodes are all different sizes, the memory used by the cache will
	// constantly changing, so this number is an estimate of the average number
	// of hashes in a node.
	size += sizeof(fiftyoneDegreesCache) + (sizeof(fiftyoneDegreesCacheNode) +
		sizeof(fiftyoneDegreesSignatureNode) +
		sizeof(fiftyoneDegreesSignatureNodeHash) * 1000) *
		FIFTYONEDEGREES_NODE_CACHE_SIZE;
#else
	size = getSizeOfFile(fileName);
#endif

	if (size > 0) {
		// Add size of file name.
		size += SIZE_OF_FILE_NAME(fileName);

		// Add the size of the dataset.
		size += sizeof(fiftyoneDegreesDataSet);
		size += sizeof(fiftyoneDegreesActiveDataSet);

		// Add the size of the unique HTTP headers.
		size += SIZE_OF_HTTP_HEADERS(httpHeaderCount);

		// Add the size of the required properties.
		size += SIZE_OF_REQUIRED_PROPERTIES(propertyCount);
		size += SIZE_OF_REQUIRED_PROPERTIES_ARRAY(propertyCount);
	}
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
size_t fiftyoneDegreesGetProviderSizeWithPropertyString(
		const char* fileName,
		const char* properties) {
	// Get property count.
	int requiredPropertyCount = getSeparatorCount(properties);

	// If property string is empty, all properties will be used.
	if (requiredPropertyCount == 0) {
		requiredPropertyCount = getEntityCountFromFile(fileName, "properties");
	}

	// Return the total size needed for the provider.
	return fiftyoneDegreesGetProviderSizeWithPropertyCount(
		fileName,
		requiredPropertyCount);
}

/**
 * \cond
 * Sets the drift parameter for detection. By default, the drift is set to
 * zero. The drift parameter indicates how much the range is extended when
 * searching for a substring in a User-Agent. For example, if the drift is
 * set to one, then if a match is not found between the first and last
 * indexes, then the extended range between (first - 1) and (last + 1) is
 * searched.
 * @param provider pointer to the provider to set the drift in
 * @param drift value of drift to set.
 * \endcond
 */
void fiftyoneDegreesSetDrift(
		fiftyoneDegreesProvider *provider,
		int drift) {
#ifndef FIFTYONEDEGREES_NO_THREADING
    FIFTYONEDEGREES_MUTEX_LOCK(&provider->lock);
#endif
    provider->active->dataSet->baseDrift = drift;
#ifndef FIFTYONEDEGREES_NO_THREADING
    FIFTYONEDEGREES_MUTEX_UNLOCK(&provider->lock);
#endif
}

/**
 * \cond
 * Sets the difference parameter for detection. By default, the difference is
 * set to zero. The difference parameter indicates the allowed difference in
 * hash value. This is most useful for the last character of a sub string, as
 * the sub string's hash code will be changed only be the change in ASCII value
 * of the final character. For example, if "Chrome 51" has the hash code 1234,
 * then "Chrome 52" will have the hash code 1235.
 * @param provider pointer to the provider to set the difference in
 * @param difference value of difference to set.
 * \endcond
 */
void fiftyoneDegreesSetDifference(
		fiftyoneDegreesProvider *provider,
		int difference) {
#ifndef FIFTYONEDEGREES_NO_THREADING
    FIFTYONEDEGREES_MUTEX_LOCK(&provider->lock);
#endif
    provider->active->dataSet->baseDifference = difference;
#ifndef FIFTYONEDEGREES_NO_THREADING
    FIFTYONEDEGREES_MUTEX_UNLOCK(&provider->lock);
#endif
}

/**
 * \cond
 * Gets a matching hash record from a node where the hash records are
 * structured as a hash table.
 * The value that index is set to can never be greater than the number of 
 * hashes. As such there is no need to perform a bounds check on index 
 * before using it with the array of hashes.
 * @param match structure containing the hash code to search for, and the node
 *              to search for it in.
 * @returns fiftyoneDegreesSignatureHash* pointer to a matching hash record, or
 *                                        null if none match.
 * \endcond
 */
static fiftyoneDegreesSignatureNodeHash* getMatchingHashFromListNodeTable(
		match_t *match) {
	fiftyoneDegreesSignatureNodeHash *foundHash = NULL;
	fiftyoneDegreesSignatureNodeHash *hashes = &match->node->hashes;

	int index = match->hash % match->node->modulo;
	if (match->hash == hashes[index].hashCode) {
		// There is a single record at this index and it matched, so return it.
		foundHash = &hashes[index];
	}
	else if (hashes[index].hashCode == 0 &&
		hashes[index].nodeOffset > 0) {
		// There are multiple records at this index, so go through them to find
		// a match.
		index = hashes[index].nodeOffset;
		while (hashes[index].hashCode != 0) {
			if (match->hash == hashes[index].hashCode) {
				// There was a match, so stop looking.
				foundHash = &hashes[index];
				break;
			}
			index++;
		}
	}
	return foundHash;
}

/**
 * \cond
 * Gets a matching hash record from a node where the hash records are stored
 * as an ordered list by performing a binary search.
 * @param match structure containing the hash code to search for, and the node
 *              to search for it in.
 * @returns fiftyoneDegreesSignatureHash* pointer to a matching hash record, or
 *                                        null if none match.
 * \endcond
 */
static fiftyoneDegreesSignatureNodeHash* getMatchingHashFromListNodeSearch(
		match_t *match) {
	fiftyoneDegreesSignatureNodeHash *foundHash = NULL;
	fiftyoneDegreesSignatureNodeHash *hashes = &match->node->hashes;
	int32_t lower = 0, upper = match->node->hashesCount - 1, middle;
	while (lower <= upper) {
		middle = lower + (upper - lower) / 2;
		if (hashes[middle].hashCode == match->hash) {
			foundHash = &hashes[middle];
			break;
		}
		else if (hashes[middle].hashCode > match->hash) {
			upper = middle - 1;
		}
		else {
			lower = middle + 1;
		}
	}
	return foundHash;
}

/**
 * \cond
 * Gets a matching hash record from a match where the node has multiple hash
 * records.
 * @param match structure containing the hash code to search for, and the node
 *              to search for it in.
 * @returns fiftyoneDegreesSignatureHash* pointer to a matching hash record, or
 *                                        null if none match.
 * \endcond
 */
static fiftyoneDegreesSignatureNodeHash* getMatchingHashFromListNode(
		match_t *match) {
	fiftyoneDegreesSignatureNodeHash *foundHash;
	if (match->node->modulo == 0) {
		foundHash = getMatchingHashFromListNodeSearch(match);
	}
	else {
		foundHash = getMatchingHashFromListNodeTable(match);
	}
	return foundHash;
}

/**
 * \cond
 * Gets a matching hash record from a match where the node has multiple hash
 * records, while allowing a difference in hash code as defined by
 * dataSet->difference.
 * @param match structure containing the hash code to search for, and the node
 *              to search for it in.
 * @returns fiftyoneDegreesSignatureHash* pointer to a matching hash record, or
 *                                        null if none match.
 * \endcond
 */
fiftyoneDegreesSignatureNodeHash* getMatchingHashFromListNodeWithinDifference(
		match_t *match) {
	fiftyoneDegreesSignatureNodeHash *nodeHash = NULL;
	uint32_t originalHashCode = match->hash;
	for (match->hash = originalHashCode + match->difference;
		match->hash >= originalHashCode - match->difference &&
		nodeHash == NULL;
		match->hash--) {
		nodeHash = getMatchingHashFromListNode(match);
	}
	match->hash = originalHashCode;

	return nodeHash;
}

/**
 * \cond
 * Copies the characters from the User-Agent that the node encapsulates to the
 * matched User-Agent so that developers can understand the character positions
 * that influenced the result. Checks that the matchedUserAgent field is set 
 * before copying as this could be an easy way of improving performance where
 * the matched User-Agent is not needed.
 * @param match
 * \endcond
 */
static void updateMatchedUserAgent(match_t *match) {
	int i;
	int nodeLength = match->currentIndex + match->node->length;
	int end = nodeLength < match->userAgentLength ? 
			  nodeLength : match->userAgentLength;
	if (match->matchedUserAgent != NULL) {
		for (i = match->currentIndex; i < end; i++) {
			match->matchedUserAgent[i] = match->userAgent[i];
		}
	}
}

/**
 * Checks to see if the offset represents a node or a device index.
 * If the offset is positive then it is a an offset from the root node in the
 * data array. If it's negative or zero then it's a device index.
 * @param match
 * @param offset
 */
static void setNextNode(match_t *match, int32_t offset) {
	if (offset > 0) {
		// There is another node to look at, so move on.
		match->node = (fiftyoneDegreesSignatureNode*)(
			NODE(match->dataSet, offset));
		// Set the first and last indexes.
		match->firstIndex += match->node->firstIndex;
		match->lastIndex += match->node->lastIndex;
	}
	else if (offset <= 0) {
		// This is a leaf node, so set the device index.
		match->deviceIndex = -offset;
		match->node = NULL;
	}
}

/**
 * \cond
 * Works out the initial hash for the first index position and sets the
 * current index to the first index.
 *
 * The hash formula for a substring of characters 'c' of length 'L' is:
 *   h[0] = (c[0]*p^(L-1)) + (c[1]*p^(L-2)) ... + (c[L-1]*p^(0))
 * where p is a prime number.
 * The hash of a substring shifted one character to the right would
 * then be:
 *   h[1] = (c[1]*p^(L-1)) + (c[2]*p^(L-2)) ... + (c[L]*p^(0))
 * This can then be rearranged as follows:
 *   h[1] = p*((c[1]*p^(L-2)) + c[2]*p^(L-3)) ... + (c[L]*p^(-1))
 *        = p*(h[0] - (c[0]*p^(L-1)) + (c[L]*p^(-1)))
 *        = p*(h[0] - (c[0]*p^(L-1))) + (c[L]*p^(0))
 *        = p*(h[0] - (c[0]*p^(L-1))) + c[L]
 *        = p*h[0] - c[0]*p^(L) + c[L]
 * which for the nth hash of an initial hash position 'i' is:
 *   h[n] = p*h[n-1] - c[n-1]*p^(L) + c[i+L]
 *
 * The prime used should be sufficiently large that the prime powers
 * have a random distribution. However, it should also be small enough
 * that the largest singular operations (p^2 and p * ASCII.max) do not
 * cause an overflow. This gives the constraints:
 *   p*2 < uint.max
 *   p * ASCII.max < uint.max
 * @param match
 * @return true if the hash can be calculated as there are characters remaining
 * otherwise false
 * \endcond
 */
static int setInitialHash(match_t *match) {
	int result = 0;
	int i;
	match->hash = 0;
	// Hash over the whole length using:
	// h[i] = (c[i]*p^(L-1)) + (c[i+1]*p^(L-2)) ... + (c[i+L]*p^(0))
	if (match->firstIndex + match->node->length <= match->userAgentLength) {
		match->power = POWERS[match->node->length];
		for (i = match->firstIndex;
			i < match->firstIndex + match->node->length;
			i++) {
			// Increment the powers of the prime coefficients.
			match->hash *= RK_PRIME;
			// Add the next character to the right.
			match->hash += match->userAgent[i];
		}
		match->currentIndex = match->firstIndex;
		result = 1;
	}
	return result;
}

/**
 * \cond
 * Advances the hash value and index.
 *
 * The hash formula for a substring of characters 'c' of length 'L' is:
 *   h[0] = (c[0]*p^(L-1)) + (c[1]*p^(L-2)) ... + (c[L-1]*p^(0))
 * where p is a prime number.
 * The hash of a substring shifted one character to the right would
 * then be:
 *   h[1] = (c[1]*p^(L-1)) + (c[2]*p^(L-2)) ... + (c[L]*p^(0))
 * This can then be rearranged as follows:
 *   h[1] = p*((c[1]*p^(L-2)) + c[2]*p^(L-3)) ... + (c[L]*p^(-1))
 *        = p*(h[0] - (c[0]*p^(L-1)) + (c[L]*p^(-1)))
 *        = p*(h[0] - (c[0]*p^(L-1))) + (c[L]*p^(0))
 *        = p*(h[0] - (c[0]*p^(L-1))) + c[L]
 *        = p*h[0] - c[0]*p^(L) + c[L]
 * which for the nth hash of an initial hash position 'i' is:
 *   h[n] = p*h[n-1] - c[n-1]*p^(L) + c[i+L]
 *
 * The prime used should be sufficiently large that the prime powers
 * have a random distribution. However, it should also be small enough
 * that the largest singular operations (p^2 and p * ASCII.max) do not
 * cause an overflow. This gives the constraints:
 *   p*2 < uint.max
 *   p * ASCII.max < uint.max
 * @param match
 * @return true if the hash and index were advanced, otherwise false
 * \endcond
 */
static int advanceHash(match_t *match) {
	int result = 0;
	int nextAddIndex;
	// Roll the hash on by one character using:
	// h[n] = p*h[n-1] - c[n-1]*p^(L) + c[i+L]
	if (match->currentIndex < match->lastIndex) {
		nextAddIndex = match->currentIndex + match->node->length;
		if (nextAddIndex < match->userAgentLength) {
			// Increment the powers of the prime coefficients.
			// p*h[n-1]
			match->hash *= RK_PRIME;
			// Add the next character to the right.
			// + c[i+L]
			match->hash += match->userAgent[nextAddIndex];
			// Remove the character that has dropped off the left.
			// - c[n-1]*p^(L)
			match->hash -= (match->power *
				match->userAgent[match->currentIndex]);
			// Increment the current index to the start index of the hash
			// which was just calculated.
			match->currentIndex++;
			result = 1;
		}
	}
	return result;
}

/**
 * \cond
 * Extend the search range by the size defined by the drift parameter.
 * @param match to extend the range in.
 * \endcond
 */
static void applyDrift(match_t *match) {
	match->firstIndex =
		match->firstIndex >= match->drift ?
		match->firstIndex - match->drift :
		0;
	match->lastIndex =
		match->lastIndex + match->drift < match->userAgentLength ?
		match->lastIndex + match->drift :
		match->userAgentLength - 1;
}

/**
 * \cond
 * Get the next node to evaluate from a node with multiple hash records, or 
 * the device index if a leaf node has has been reached. The current node and
 * device index are updated in the match structure.
 * @param match
 * \endcond
 */
static void evaluateListNode(match_t *match) {
	fiftyoneDegreesSignatureNodeHash *nodeHash = NULL;

	// Set the match structure with the initial hash value.
	if (setInitialHash(match)) {
		// Loop between the first and last indexes checking the hash values.
		do {
			nodeHash = getMatchingHashFromListNode(match);
		} while (nodeHash == NULL && advanceHash(match));

		if (nodeHash == NULL &&
			match->difference > 0) {
			// DIFFERENCE
			// A match was not found, and the difference feature is enabled, so
			// search again allowing for the difference tolerance.
			if (setInitialHash(match)) {
				do {
					nodeHash =
						getMatchingHashFromListNodeWithinDifference(match);
				} while (nodeHash == NULL && advanceHash(match));
			}
		}

		if (nodeHash == NULL &&
			match->drift > 0) {
			// DRIFT
			// A match was not found, and the drift feature is enabled, so
			// search again in the extended range defined by the drift.
			applyDrift(match);
			if (setInitialHash(match)) {
				do {
					nodeHash = getMatchingHashFromListNode(match);
				} while (nodeHash == NULL && advanceHash(match));
			}
		}

		if (nodeHash == NULL &&
			match->difference > 0 &&
			match->drift > 0) {
			// DIFFERENCE + DRIFT
			// A match was still not found, and both the drift and difference
			// features are enabled, so search again with both tolerances.
			// Note the drift has already been applied to the match structure.
			if (setInitialHash(match)) {
				do {
					nodeHash =
						getMatchingHashFromListNodeWithinDifference(match);
				} while (nodeHash == NULL && advanceHash(match));
			}
		}
	}

	if (nodeHash != NULL) {
		// A match occurred and the hash value was found. Use the offset
		// to either find another node to evaluate or the device index.
		updateMatchedUserAgent(match);
		setNextNode(match, nodeHash->nodeOffset);
	}
	else {
		// No matching hash value was found. Use the unmatched node offset
		// to find another node to evaluate or the device index.
		setNextNode(match, match->node->unmatchedNodeOffset);
	}
}

/**
 * \cond
 * Get the next node to evaluate from a node with a single hash record, or
 * the device index if a leaf node has has been reached. The current node and
 * device index are updated in the match structure.
 * @param match
 * \endcond
 */
static void evaluateBinaryNode(match_t *match) {
	int found = FALSE;
	fiftyoneDegreesSignatureNodeHash *hash = &match->node->hashes;
	if (setInitialHash(match)) {
		// Keep rolling the hash until the hash is found or the last index is
		// reached and there is no possibility of finding the hash value.
		while (match->hash != hash->hashCode && advanceHash(match)) {
		}
	}

	if (match->hash == hash->hashCode) {
		// A match was found without the need to resort to allowing for drift
		// or difference.
		found = TRUE;
	}

	if (found == FALSE &&
		match->difference > 0) {
		// DIFFERENCE
		// A match was not found, and the difference feature is enabled, so
		// search again allowing for the difference tolerance.
		if (setInitialHash(match)) {
			while (abs((int)(match->hash - hash->hashCode))
				<= match->difference &&
				advanceHash(match)) {
			}
			if (abs((int)(match->hash - hash->hashCode))
				<= match->difference) {
				// A match was found within the difference tolerance.
				found = TRUE;
			}
		}
	}
	if (found == FALSE &&
		match->drift > 0) {
		// DRIFT
		// A match was not found, and the drift feature is enabled, so
		// search again in the extended range defined by the drift.
		applyDrift(match);
		if (setInitialHash(match)) {
			while (match->hash != hash->hashCode && advanceHash(match)) {
			}
			if (match->hash == hash->hashCode) {
				// A match was found within the drift tolerance.
				found = TRUE;
			}
		}
	}
	if (found == FALSE &&
		match->drift > 0 &&
		match->difference > 0) {
		// DIFFERENCE + DRIFT
		// A match was still not found, and both the drift and difference
		// features are enabled, so search again with both tolerances.
		// Note the drift has already been applied to the match structure.
		if (setInitialHash(match)) {
			while (abs((int)(match->hash - hash->hashCode))
				<= match->difference &&
				advanceHash(match)) {
			}
			if (abs((int)(match->hash - hash->hashCode))
				<= match->difference) {
				// A match was found within the difference and drift
				// tolerances.
				found = TRUE;
			}
		}
	}

	if (found == TRUE) {
		// A match occurred and the hash value was found. Use the offset
		// to either find another node to evaluate or the device index.
		updateMatchedUserAgent(match);
		setNextNode(match, hash->nodeOffset);
	}
	else {
		// No matching hash value was found. Use the unmatched node offset
		// to find another node to evaluate or the device index.
		setNextNode(match, match->node->unmatchedNodeOffset);
	}
}

/**
 * \cond
 * Initialises the match structure ready to be used to search the graph.
 * @param dataSet to search for a match in
 * @param userAgent string to search for
 * @param drift to extend the search range by.
 * @param difference to allow in hash values.
 * @param match structure to be initialised
 * \endcond
 */
static void initialiseMatch(
	const fiftyoneDegreesDataSet *dataSet,
	const char *userAgent,
	int userAgentLength,
	int drift,
	int difference,
	match_t *match) {
	match->dataSet = dataSet;
	match->userAgent = userAgent;
	if (userAgentLength < 0) {
		match->userAgentLength = (int16_t)strlen(userAgent);		
	}
	else {
		match->userAgentLength = (int16_t)userAgentLength;
	}
	match->node = NODE(dataSet, 0);
	match->drift = dataSet->baseDrift + drift;
	match->difference = dataSet->baseDifference + difference;
	match->firstIndex = match->node->firstIndex;
	match->lastIndex = match->node->lastIndex;
	match->matchedUserAgent = NULL;
}

/**
 * \cond
 * Returns the index to a matching device based on the User-Agent provided.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent the User-Agent to match with.
 * @param matchedUserAgent initialised string buffer for the matched
 *                         characters.
 * @param drift to extend the search range by.
 * @param difference to allow in hash values.
 * @returns int32_t the device index for the given User-Agent.
 * \endcond
 */
static int32_t getDeviceIndex(
	const fiftyoneDegreesDataSet *dataSet,
	const char *userAgent,
	int userAgentLength,
	char *matchedUserAgent,
	int drift,
	int difference) {
	match_t match;
	initialiseMatch(dataSet, userAgent, userAgentLength, drift, difference, &match);
	
	// Set the matched User-Agent array to spaces and null terminate at the 
	// User-Agent length so that the string displays as expected.
	if (matchedUserAgent != NULL) {
		match.matchedUserAgent = matchedUserAgent;
		memset(matchedUserAgent, '_', match.userAgentLength);
		matchedUserAgent[match.userAgentLength] = 0;
	}

	// While there is a node to evaluate keep evaluating. match.node will be
	// set to NULL when the evaluate method can't progress any further and the 
	// match.deviceIndex that is set should be returned.
	while (match.node != NULL) {
		if (match.node->hashesCount == 1) {
			// If there is only 1 hash then it's a binary node.
			evaluateBinaryNode(&match);
		}
		else {
			// More than 1 hash indicates a list node with multiple children.
			evaluateListNode(&match);
		}
	}
	return match.deviceIndex;
}

/**
 * \cond
 * Returns the number of characters which matched in the Trie.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent User-Agent to use.
 * @param drift to extend the search range by.
 * @param difference to allow in hash values.
 * @returns int the number of matching characters from the User-Agent.
 * \endcond
 */
int fiftyoneDegreesGetMatchedUserAgentLengthWithTolerances(
	fiftyoneDegreesDataSet *dataSet,
	char *userAgent,
	int drift,
	int difference) {
	match_t match;
	int i = 0;
	char matchedUserAgent[500];
	initialiseMatch(dataSet, userAgent, -1, drift, difference, &match);
	memset(&matchedUserAgent, 0, match.userAgentLength + 1);
	match.matchedUserAgent = &matchedUserAgent[0];
	while (match.node != NULL) {
		if (match.node->hashesCount == 1)
		{
			evaluateBinaryNode(&match);
		}
		else
		{
			evaluateListNode(&match);
		}
	}
	for (i = match.userAgentLength - 1; i >= 0; i--) {
		if (matchedUserAgent[i] != 0) {
			return i;
		}
	}
	return match.userAgentLength;
}

/**
 * \cond
 * Returns the number of characters which matched in the Trie.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent User-Agent to use.
 * @returns int the number of matching characters from the User-Agent.
 * \endcond
 */
int fiftyoneDegreesGetMatchedUserAgentLength(
		fiftyoneDegreesDataSet *dataSet,
		char *userAgent) {
	return fiftyoneDegreesGetMatchedUserAgentLengthWithTolerances(
		dataSet,
		userAgent,
		0,
		0);
}

/**
 * \cond
 * Returns the offset in the properties list to the first value for the device.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent to match for.
 * @returns int the property offset for the matched device.
 * \endcond
 */
int32_t fiftyoneDegreesGetDeviceOffset(
	fiftyoneDegreesDataSet *dataSet,
	const char* userAgent) {
return getDeviceIndex(dataSet, userAgent, -1, NULL, 0, 0) * 
		dataSet->devicesIntegerCount;
}

/**
 * \cond
 * Sets the offsets structure passed to the method for the User-Agent provided.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent to match for.
 * @param userAgentLength of the User-Agent.
 * @param httpHeaderIndex of the User-Agent.
 * @param offset to set.
 * @param drift to extend the search range by.
 * @param difference to allow in hash values.
 * \endcond
 */
void fiftyoneDegreesSetDeviceOffsetFromArrayWithTolerances(
	fiftyoneDegreesDataSet *dataSet,
	const char* userAgent,
	int userAgentLength,
	int httpHeaderIndex,
	fiftyoneDegreesDeviceOffset *offset,
	int drift,
	int difference) {
	offset->httpHeaderOffset =
		dataSet->uniqueHttpHeaders.firstElement[httpHeaderIndex];
	offset->length = strlen(userAgent);
	offset->difference = 0;
#ifdef _DEBUG
	offset->userAgent = (char*)fiftyoneDegreesMalloc(
		offset->length + 1 * sizeof(char));
#else
	offset->userAgent = NULL;
#endif
	offset->deviceOffset = getDeviceIndex(
		dataSet, 
		userAgent, 
		userAgentLength,
		offset->userAgent, 
		drift, 
		difference) *
		(dataSet->devicePropertiesCount + dataSet->components.count);
}

/**
 * \cond
 * Sets the offsets structure passed to the method for the User-Agent provided.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent to match for.
 * @param httpHeaderIndex of the User-Agent.
 * @param offset to set.
 * @param drift to extend the search range by.
 * @param difference to allow in hash values.
 * \endcond
 */
void fiftyoneDegreesSetDeviceOffsetWithTolerances(
	fiftyoneDegreesDataSet *dataSet,
	const char* userAgent,
	int httpHeaderIndex,
	fiftyoneDegreesDeviceOffset *offset,
	int drift,
	int difference) {
	fiftyoneDegreesSetDeviceOffsetFromArrayWithTolerances(
			dataSet,
			userAgent,
			-1,
			httpHeaderIndex,
			offset,
			drift,
			difference);
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
void fiftyoneDegreesSetDeviceOffset(
		fiftyoneDegreesDataSet *dataSet, 
		const char* userAgent,
		int httpHeaderIndex,
		fiftyoneDegreesDeviceOffset *offset) {
	fiftyoneDegreesSetDeviceOffsetWithTolerances(
		dataSet,
		userAgent,
		httpHeaderIndex,
		offset, 
		0, 
		0);
}

/**
 * \cond
 * Creates a new device offsets structure with memory allocated.
 * @param dataSet pointer to an initialised dataset.
 * @returns fiftyoneDegreesDeviceOffsets* newly created device offsets.
 * \endcond
 */
fiftyoneDegreesDeviceOffsets* fiftyoneDegreesCreateDeviceOffsets(
		fiftyoneDegreesDataSet *dataSet) {
	fiftyoneDegreesDeviceOffsets* offsets = (fiftyoneDegreesDeviceOffsets*)
		fiftyoneDegreesMalloc(sizeof(fiftyoneDegreesDeviceOffsets));
	offsets->size = 0;
	offsets->firstOffset = (fiftyoneDegreesDeviceOffset*)fiftyoneDegreesMalloc(
		dataSet->uniqueHttpHeaders.count * sizeof(fiftyoneDegreesDeviceOffset));
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
					fiftyoneDegreesFree(
						(void*)(offsets->firstOffset +
							offsetIndex)->userAgent);
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
					fiftyoneDegreesFree((void*)(offsets->firstOffset +
						offsetIndex)->userAgent);
				}
			}
			fiftyoneDegreesFree(offsets->firstOffset);
		}
		fiftyoneDegreesFree(offsets);
	}
}

/**
 * \cond
 * Creates a new device offsets structure with memory allocated and increments
 * the inUse counter in the provider so the dataset will not be freed until 
 * this is. A corresponding call to fiftyoneDegreesProviderFreeDeviceOffsets
 * must be made when these offsets are finished with.
 * @param provider pointer to an initialised provider.
 * @returns fiftyoneDegreesDeviceOffsets* newly created device offsets.
 * \endcond
 */
fiftyoneDegreesDeviceOffsets* fiftyoneDegreesProviderCreateDeviceOffsets(
	fiftyoneDegreesProvider *provider) {
	fiftyoneDegreesDeviceOffsets *offsets = NULL;
	fiftyoneDegreesActiveDataSet *active = NULL;

#ifndef FIFTYONEDEGREES_NO_THREADING
	do {
		// If the we've an old active data set and it's no longer in use
		// after we've decremented the use counter then free it.
		if (active != NULL) {
			fiftyoneDegreesProviderFreeDeviceOffsets(offsets);
		}

		// Get the active data set.
		active = (fiftyoneDegreesActiveDataSet*)provider->active;

		// Increment the inUse counter for the active data set so that we can
		// track any offsets that are created.
		FIFTYONEDEGREES_INTERLOCK_INC(&active->inUse);

		// Create the offsets.
		offsets = fiftyoneDegreesCreateDeviceOffsets(active->dataSet);
		offsets->active = active;

		// If the current active data set is the same as the local one then 
		// continue.
	} while (active != provider->active);
#else
	// Get the active data set.
	active = (fiftyoneDegreesActiveDataSet*)provider->active;

	// Increment the in use counter.
	active->inUse++;

	// Create the offsets.
	offsets = fiftyoneDegreesCreateDeviceOffsets(active->dataSet);
	offsets->active = active; 
#endif

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
void fiftyoneDegreesProviderFreeDeviceOffsets(
		fiftyoneDegreesDeviceOffsets *offsets) {
	fiftyoneDegreesProvider* provider= offsets->active->provider;

	// If the dataset the offsets are associated with is not the active
	// one and no other offsets are using it, then dispose of it.
#ifndef FIFTYONEDEGREES_NO_THREADING
	if (FIFTYONEDEGREES_INTERLOCK_DEC(&offsets->active->inUse) == 0 &&
		provider->active != offsets->active) {
		fiftyoneDegreesActiveDataSetFree(offsets->active);
	}
#else
	offsets->active->inUse--;
	if (offsets->active->inUse == 0 &&
		provider->active != offsets->active) {
		fiftyoneDegreesActiveDataSetFree(offsets->active);
	}
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
static int setNextHttpHeaderName(char* start, char* end, char** name) {
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
static int setNextHttpHeaderValue(char* start, char *end, char** value) {
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
static int headerCompare(
		char *httpHeaderName,
		const char *uniqueHeader,
		int length) {
	int index, difference;
	for (index = 0; index < length; index++) {
		if (httpHeaderName[index] == '_') {
			difference = '-' - uniqueHeader[index];
		}
		else {
			difference =
				tolower(httpHeaderName[index]) - tolower(uniqueHeader[index]);
		}
		if (difference != 0) {
			return difference;
		}
	}
	return 0;
}

/**
 * \cond
 * Returns the index of the unique header, or -1 if the header is not
 * important.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaderName name of the header to get the index of.
 * @param length of the header name.
 * @returns int index of the unique header, or -1 if the header is not
 * important.
 * \endcond
 */
int fiftyoneDegreesGetUniqueHttpHeaderIndex(
		fiftyoneDegreesDataSet *dataSet, 
		char* httpHeaderName,
		int length) {
	unsigned int uniqueHeaderIndex;
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

	for (uniqueHeaderIndex = 0; 
		 uniqueHeaderIndex < dataSet->uniqueHttpHeaders.count;
		 uniqueHeaderIndex++) {
		if ((int)strlen(
				STRING(
					dataSet,
					dataSet->uniqueHttpHeaders.firstElement
					[uniqueHeaderIndex])) ==
			length &&
			headerCompare(
				adjustedHttpHeaderName,
				STRING(
					dataSet, 
					dataSet->uniqueHttpHeaders.firstElement
					[uniqueHeaderIndex]),
					length) == 0) {
			return uniqueHeaderIndex;
		}
	}
	return -1;
}

/**
 * \cond
 * Returns the offsets to a matching devices based on the HTTP headers
 * provided.
 * @param dataSet pointer to an initialised dataset.
 * @param offsets to set.
 * @param httpHeaders to match for.
 * @param size of the headers string.
 * \endcond
 */
void fiftyoneDegreesSetDeviceOffsetsWithHeadersString(
		fiftyoneDegreesDataSet *dataSet,
		fiftyoneDegreesDeviceOffsets *offsets,
		char *httpHeaders,
		size_t size) {
	char *headerName, *headerValue, *endOfHeaders = httpHeaders + size;
	unsigned int headerNameLength, headerValueLength;
	int uniqueHeaderIndex = 0;
	offsets->size = 0;
	headerNameLength =
		setNextHttpHeaderName(httpHeaders, endOfHeaders, &headerName);
	while (headerNameLength > 0 &&
		offsets->size < (int)dataSet->uniqueHttpHeaders.count) {
		headerValueLength = setNextHttpHeaderValue(
			headerName + headerNameLength,
			endOfHeaders,
			&headerValue);
		uniqueHeaderIndex = fiftyoneDegreesGetUniqueHttpHeaderIndex(
			dataSet,
			headerName,
			headerNameLength);
		if (uniqueHeaderIndex >= 0) {
			fiftyoneDegreesSetDeviceOffset(
				dataSet, 
				headerValue,
				uniqueHeaderIndex,
				(offsets->firstOffset + offsets->size));
			offsets->size++;
		}
		headerNameLength = setNextHttpHeaderName(
			headerValue + headerValueLength,
			endOfHeaders,
			&headerName);
	}
	if (offsets->size == 0) {
		(offsets->firstOffset + offsets->size)->deviceOffset = 0;
		offsets->size++;
	}
}

/**
 * \cond
 * Returns the offsets to a matching devices based on the HTTP headers
 * provided.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaders to match for.
 * @param size of the HTTP headers string.
 * @returns fiftyoneDegreesDeviceOffsets* pointer to newly created device
 * offsets from the match.
 * \endcond
 */
fiftyoneDegreesDeviceOffsets* fiftyoneDegreesGetDeviceOffsetsWithHeadersString(
		fiftyoneDegreesDataSet *dataSet,
		char *httpHeaders,
		size_t size) {
	fiftyoneDegreesDeviceOffsets* offsets =
		fiftyoneDegreesCreateDeviceOffsets(dataSet);
	fiftyoneDegreesSetDeviceOffsetsWithHeadersString(
		dataSet,
		offsets,
		httpHeaders,
		size);
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
static char* getValueFromDevice(
		fiftyoneDegreesDataSet *dataSet,
		const int32_t *device,
		int32_t propertyIndex) {
	const fiftyoneDegreesProperty *property = PROPERTY(dataSet, propertyIndex);
	if (propertyIndex < (int32_t)dataSet->devicePropertiesCount) {
		// The value in referenced directly from the device in the integers
		// which follow the component profiles.
		return STRING(
			dataSet, 
			*(device + dataSet->components.count + propertyIndex));
	}
	else {
		// The value is referenced from the profile for the properties
		// component.
		int32_t *profile = PROFILE(
			dataSet,
			*(device + property->componentIndex));
		return STRING(dataSet, *(profile + property->subIndex));
	}
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
const char* fiftyoneDegreesGetValue(
		fiftyoneDegreesDataSet *dataSet,
		int deviceOffset,
		int propertyIndex) {
	return getValueFromDevice(
		dataSet,
		DEVICE(dataSet, deviceOffset),
		propertyIndex);
}

/**
 * \cond
 * Returns the number of HTTP headers relevant to device detection.
 * @param dataSet pointer to an initialised dataset.
 * @returns int the number of relevant HTTP headers.
 * \endcond
 */
int fiftyoneDegreesGetHttpHeaderCount(fiftyoneDegreesDataSet *dataSet) {
	return dataSet->uniqueHttpHeaders.count;
}

/**
 * \cond
 * Returns a pointer to the HTTP header name at the index provided.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaderIndex index of the header to get.
 * @returns const char* the name of the HTTP header.
 * \endcond
 */
const char* fiftyoneDegreesGetHttpHeaderNamePointer(
		fiftyoneDegreesDataSet *dataSet,
		int httpHeaderIndex) {
	return httpHeaderIndex >= 0 &&
		httpHeaderIndex < (int)dataSet->uniqueHttpHeaders.count ?
		STRING(
			dataSet,
			dataSet->uniqueHttpHeaders.firstElement[httpHeaderIndex]) : NULL;
}

/**
 * \cond
 * Returns the HTTP header name offset at the index provided.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaderIndex index of the header to get.
 * @returns int the name offset of the HTTP header.
 * \endcond
 */
int fiftyoneDegreesGetHttpHeaderNameOffset(
		fiftyoneDegreesDataSet *dataSet,
		int httpHeaderIndex) {
	return dataSet->uniqueHttpHeaders.firstElement[httpHeaderIndex];
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
int fiftyoneDegreesGetHttpHeaderName(
		fiftyoneDegreesDataSet *dataSet,
		int httpHeaderIndex,
		char* httpHeader,
		int size) {
	int length = 0;
	if (httpHeaderIndex < (int)dataSet->uniqueHttpHeaders.count) {
		length = (int)strlen(STRING(
			dataSet, 
			dataSet->uniqueHttpHeaders.firstElement[httpHeaderIndex]));
		if (length <= size) {
			// Copy the string and return the length.
			strcpy(
				httpHeader, 
				STRING(
					dataSet, 
					dataSet->uniqueHttpHeaders.firstElement[httpHeaderIndex]));
		}
		else {
			// The HTTP header is not large enough. Return it's required length
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
	int index;
	unsigned int httpHeaderIndex;
	size_t length;
	char *prefixedUpperHttpHeader, *httpHeaderName;
	dataSet->prefixedUpperHttpHeaders = (const char**)fiftyoneDegreesMalloc(
		dataSet->uniqueHttpHeaders.count * sizeof(char*));
	if (dataSet->prefixedUpperHttpHeaders != NULL) {
		for (httpHeaderIndex = 0; 
			 httpHeaderIndex < dataSet->uniqueHttpHeaders.count; 
			 httpHeaderIndex++) {
			httpHeaderName = STRING(
				dataSet, 
				dataSet->uniqueHttpHeaders.firstElement[httpHeaderIndex]);
			length = strlen(httpHeaderName);
			prefixedUpperHttpHeader = (char*)fiftyoneDegreesMalloc(
				(length + sizeof(FIFTYONEDEGREES_HTTP_PREFIX_UPPER)) *
				sizeof(char));
			if (prefixedUpperHttpHeader != NULL) {
				dataSet->prefixedUpperHttpHeaders[httpHeaderIndex] =
					(const char*)prefixedUpperHttpHeader;
				memcpy(
					(void*)prefixedUpperHttpHeader, 
					FIFTYONEDEGREES_HTTP_PREFIX_UPPER, 
					sizeof(FIFTYONEDEGREES_HTTP_PREFIX_UPPER) - 1);
				prefixedUpperHttpHeader += 
					sizeof(FIFTYONEDEGREES_HTTP_PREFIX_UPPER) - 1;
				for (index = 0; index < (int) length; index++) {
					*prefixedUpperHttpHeader = (char)toupper(*httpHeaderName);
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
const char* fiftyoneDegreesGetPrefixedUpperHttpHeaderName(
		fiftyoneDegreesDataSet *dataSet, 
		int httpHeaderIndex) {
	const char *prefixedUpperHeaderName = NULL;
	if (dataSet->prefixedUpperHttpHeaders == NULL) {
		initPrefixedUpperHttpHeaderNames(dataSet);
	}
	if (httpHeaderIndex >= 0 &&
		httpHeaderIndex < (int)dataSet->uniqueHttpHeaders.count) {
		prefixedUpperHeaderName =
			dataSet->prefixedUpperHttpHeaders[httpHeaderIndex];
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
int fiftyoneDegreesGetRequiredPropertyName(
		fiftyoneDegreesDataSet *dataSet,
		int requiredPropertyIndex,
		char* propertyName,
		int size) {
	int length = 0;
	if (requiredPropertyIndex < (int)dataSet->requiredProperties.count) {
		length = (int)strlen(
			dataSet->requiredPropertiesNames[requiredPropertyIndex]);
		if (length <= size) {
			// Copy the string and return the length.
			strcpy(
				propertyName, 
				dataSet->requiredPropertiesNames[requiredPropertyIndex]);
		}
		else {
			// The property name is not large enough. Return it's required
			// length as a negative.
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
static int setValueFromDeviceOffset(
		fiftyoneDegreesDataSet *dataSet,
		int32_t deviceOffset,
		int32_t propertyIndex,
		char* values,
		int size) {
	const char *value =
		fiftyoneDegreesGetValue(dataSet, deviceOffset, propertyIndex);
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
 * @param requiredPropertyIndex index in the dataset's requiredProperties
 *                              array.
 * @returns const char* pointer to the value of the requested property, or NULL
 *                      if the property does not exist.
 * \endcond
 */
const char* fiftyoneDegreesGetValuePtrFromOffsets(
		fiftyoneDegreesDataSet *dataSet,
		fiftyoneDegreesDeviceOffsets* deviceOffsets,
		int requiredPropertyIndex) {
	int deviceHttpHeaderIndex, propertyHttpHeaderIndex;
	int32_t propertyHttpHeaderOffset;
	const fiftyoneDegreesProperty *property;
	if (deviceOffsets->size == 1) {
		return fiftyoneDegreesGetValue(
			dataSet,
			deviceOffsets->firstOffset->deviceOffset,
			dataSet->requiredProperties.firstElement[requiredPropertyIndex]);
	}
	else {
		property = PROPERTY(
			dataSet,
			dataSet->requiredProperties.firstElement[requiredPropertyIndex]);
		for (propertyHttpHeaderIndex = 0; 
			 propertyHttpHeaderIndex < property->headerCount; 
			 propertyHttpHeaderIndex++) {
			propertyHttpHeaderOffset = HTTPHEADER(dataSet,
				property->headerFirstIndex + 
				propertyHttpHeaderIndex);
			for (deviceHttpHeaderIndex = 0;
				 deviceHttpHeaderIndex < deviceOffsets->size;
				 deviceHttpHeaderIndex++) {
				if (propertyHttpHeaderOffset == (deviceOffsets->firstOffset + 
					deviceHttpHeaderIndex)->httpHeaderOffset) {
					return fiftyoneDegreesGetValue(
						dataSet,
						(deviceOffsets->firstOffset + 
							deviceHttpHeaderIndex)->deviceOffset,
						dataSet->requiredProperties.firstElement[
							requiredPropertyIndex]);
				}
			}
		}
	}
	return NULL;
}

/**
 * \cond
 * Sets the values string to the property values for the device offsets and
 * index provided.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffsets to get the property from.
 * @param requiredPropertyIndex index in the dataset's requiredProperties array.
 * @param values string to set.
 * @param size allocated to the values string.
 * @returns int the length of the values string, or the required length as a
 * negative if size is not large enough.
 * \endcond
 */
int fiftyoneDegreesGetValueFromOffsets(
		fiftyoneDegreesDataSet *dataSet,
		fiftyoneDegreesDeviceOffsets* deviceOffsets,
		int requiredPropertyIndex,
		char* values,
		int size) {
	int deviceHttpHeaderIndex, propertyHttpHeaderIndex;
	int32_t propertyHttpHeaderOffset;
	const fiftyoneDegreesProperty *property;
	if (deviceOffsets->size == 1) {
		return setValueFromDeviceOffset(dataSet,
			deviceOffsets->firstOffset->deviceOffset,
			*(dataSet->requiredProperties.firstElement + requiredPropertyIndex),
			values,
			size);
	}
	else {
		property = PROPERTY(
			dataSet, 
			dataSet->requiredProperties.firstElement[requiredPropertyIndex]);
		for (propertyHttpHeaderIndex = 0;
			 propertyHttpHeaderIndex < property->headerCount;
			 propertyHttpHeaderIndex++) {
			propertyHttpHeaderOffset = HTTPHEADER(dataSet, 
				property->headerFirstIndex + 
				propertyHttpHeaderIndex);
			for (deviceHttpHeaderIndex = 0;
				 deviceHttpHeaderIndex < deviceOffsets->size;
				 deviceHttpHeaderIndex++) {
				if (propertyHttpHeaderOffset == (
					deviceOffsets->firstOffset + 
					deviceHttpHeaderIndex)->httpHeaderOffset) {
					return setValueFromDeviceOffset(dataSet,
						(deviceOffsets->firstOffset + 
							deviceHttpHeaderIndex)->deviceOffset,
						dataSet->requiredProperties.firstElement[
							requiredPropertyIndex],
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
int32_t fiftyoneDegreesGetRequiredPropertiesCount(
		fiftyoneDegreesDataSet *dataSet) {
	return dataSet->requiredProperties.count;
}

/**
 * \cond
 * Returns the names of the properties loaded in the dataset.
 * @param dataSet pointer to an initialised dataset.
 * @retuens const char** pointer to the array of initialised properties.
 * \endcond
 */
const char ** fiftyoneDegreesGetRequiredPropertiesNames(
		fiftyoneDegreesDataSet *dataSet) {
	return dataSet->requiredPropertiesNames;
}

/**
 * \cond
 * Returns the index in the array of required properties for this name, or -1
 * if not found.
 * @param dataSet pointer to an initialised dataset.
 * @param propertyName name of the property to get.
 * @returns int index in the dataset's requiredProperties array, or -1 if not
 *              found.
 * \endcond
 */
int fiftyoneDegreesGetRequiredPropertyIndex(
		fiftyoneDegreesDataSet *dataSet,
		const char *propertyName) {
	const char** found = (const char**)bsearch(
		&propertyName,
		dataSet->requiredPropertiesNames,
		dataSet->requiredProperties.count,
		sizeof(const char*),
		comparePropertyNamesAscending);
	if (found == NULL) {
		return -1;
	}
	else {
		return (int)(found - dataSet->requiredPropertiesNames);
	}
}

/**
* \cond
* Returns the index of the property requested, or -1 if not available.
* @param dataSet pointer to an initialised dataset.
* @param value name of the property to find.
* @returns int the index of the property requested, or -1 if not available.
* \endcond
*/
int fiftyoneDegreesGetPropertyIndex(
	fiftyoneDegreesDataSet *dataSet,
	const char *propertyName) {
	int requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(
		dataSet,
		propertyName);
	if (requiredPropertyIndex >= 0) {
		return dataSet->requiredProperties.firstElement[requiredPropertyIndex];
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
int fiftyoneDegreesProcessDeviceOffsetsCSV(
		fiftyoneDegreesDataSet *dataSet,
		fiftyoneDegreesDeviceOffsets *deviceOffsets,
		char* result,
		int resultLength) {
	char* currentPos = result;
	char* endPos = result + resultLength;
	unsigned int requiredPropertyIndex;

	// If no properties return nothing.
	if (dataSet->requiredProperties.count == 0) {
		*currentPos = 0;
		return 0;
	}

	// Process each line of data using the pipe (|) value separator.
	for (requiredPropertyIndex = 0;
		 requiredPropertyIndex < dataSet->requiredProperties.count;
		 requiredPropertyIndex++) {
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
int fiftyoneDegreesProcessDeviceCSV(
		fiftyoneDegreesDataSet *dataSet,
		int deviceOffset,
		char* result,
		int resultLength) {
	fiftyoneDegreesDeviceOffsets deviceOffsets;
	fiftyoneDegreesDeviceOffset singleOffset;
	deviceOffsets.firstOffset = &singleOffset;
	singleOffset.deviceOffset = deviceOffset;
	deviceOffsets.size = 1;
	return fiftyoneDegreesProcessDeviceOffsetsCSV(
		dataSet, 
		&deviceOffsets, 
		result, 
		resultLength);
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
int fiftyoneDegreesProcessDeviceOffsetsJSON(
		fiftyoneDegreesDataSet *dataSet,
		fiftyoneDegreesDeviceOffsets *deviceOffsets,
		char* result,
		int resultLength) {
	char* valuePos;
	unsigned int requiredPropertyIndex;
	char* currentPos = result;
	char* endPos = result + resultLength;

	// If no properties return empty JSON.
	if (dataSet->requiredProperties.count == 0) {
		currentPos += snprintf(currentPos, endPos - currentPos, "{ }");
		return (int)(currentPos - result);
	}

	currentPos += snprintf(currentPos, endPos - currentPos, "{");

	// Process each line of data using the pipe (|) value separator.
	for (requiredPropertyIndex = 0;
		 requiredPropertyIndex < dataSet->requiredProperties.count;
		 requiredPropertyIndex++) {
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
		if (requiredPropertyIndex + 1 != dataSet->requiredProperties.count) {
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
int fiftyoneDegreesProcessDeviceJSON(
		fiftyoneDegreesDataSet *dataSet,
		int deviceOffset,
		char* result,
		int resultLength) {
	fiftyoneDegreesDeviceOffsets deviceOffsets;
	fiftyoneDegreesDeviceOffset singleOffset;
	deviceOffsets.firstOffset = &singleOffset;
	singleOffset.deviceOffset = deviceOffset;
	deviceOffsets.size = 1;
	return fiftyoneDegreesProcessDeviceOffsetsJSON(
		dataSet, 
		&deviceOffsets, 
		result, 
		resultLength);
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

/**
* \ingroup FiftyOneDegreesFunctions
* @param dataSet pointer to an initialised data set.
* @returns the fomat of the data set used contained in the source file.
*/
char* fiftyoneDegreesGetDataSetFormat(fiftyoneDegreesDataSet *dataSet) {
	return STRING(dataSet, dataSet->header.formatOffset);
}

/**
* \ingroup FiftyOneDegreesFunctions
* @param dataSet pointer to an initialised data set.
* @returns the name of the data set used contained in the source file.
*/
char* fiftyoneDegreesGetDataSetName(fiftyoneDegreesDataSet *dataSet) {
	return STRING(dataSet, dataSet->header.nameOffset);
}