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
 * \defgroup FiftyOneDegreesFunctions
 * These functions are the core that all 51Degrees C based API's build on. All the 
 * device detection happens here.
 */

#ifndef FIFTYONEDEGREES_H_INCLUDED
#define FIFTYONEDEGREES_H_INCLUDED

#ifndef FIFTYONEDEGREES_H_TRIE_INCLUDED
#define FIFTYONEDEGREES_H_TRIE_INCLUDED
#endif

#include <stdint.h>
#include <stdio.h>
#ifndef FIFTYONEDEGREES_NO_THREADING
#include "../threading.h"
#endif

#ifdef FIFTYONEDEGREES_INDIRECT
#include "../cache.h"
#endif

#ifdef __cplusplus
#define EXTERNAL extern "C"
#else
#define EXTERNAL
#endif

#ifdef _MSC_VER
#define FIFTYONEDEGREES_CALL_CONV __cdecl
#else
#define FIFTYONEDEGREES_CALL_CONV
#endif

#ifdef FIFTYONEDEGREES_INDIRECT
 /* Define default cache sizes if cache sizes have not already been defined. */
#ifndef FIFTYONEDEGREES_STRING_CACHE_SIZE
#define FIFTYONEDEGREES_STRING_CACHE_SIZE 1000
#endif
#ifndef FIFTYONEDEGREES_NODE_CACHE_SIZE
#define FIFTYONEDEGREES_NODE_CACHE_SIZE 100000
#endif
#ifndef FIFTYONEDEGREES_DEVICE_CACHE_SIZE
#define FIFTYONEDEGREES_DEVICE_CACHE_SIZE 1000
#endif
#ifndef FIFTYONEDEGREES_PROFILE_CACHE_SIZE
#define FIFTYONEDEGREES_PROFILE_CACHE_SIZE 5000
#endif
#endif

/* Used to provide the status of the data set initialisation */
typedef enum e_fiftyoneDegreesDataSetInitStatus {
	DATA_SET_INIT_STATUS_SUCCESS, /* All okay */
	DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY, /* Lack of memory */
	DATA_SET_INIT_STATUS_CORRUPT_DATA, /* Data structure not readable */
	DATA_SET_INIT_STATUS_INCORRECT_VERSION, /* Data not the required version */
	DATA_SET_INIT_STATUS_FILE_NOT_FOUND, /* The data file couldn't be found */
	DATA_SET_INIT_STATUS_NOT_SET, /* Should never be returned to the caller */
	/* Working pointer exceeded the amount of memory containing the data */
	DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS,
	DATA_SET_INIT_STATUS_NULL_POINTER, /* A key pointer was not set */
	DATA_SET_INIT_STATUS_TOO_MANY_OPEN_FILES /* Too many files are open */
} fiftyoneDegreesDataSetInitStatus;

/* Declaration of the active dataset wrapper. */
typedef struct fiftyoneDegrees_active_dataset_t fiftyoneDegreesActiveDataSet;

/* Relates a HTTP header index to to a device offset */
typedef struct fiftyoneDegrees_device_offset_t {
	int httpHeaderOffset; /* Offset to the HTTP header string */
	int deviceOffset; /* Offset to the device */
	size_t length; /* Number of characters in the matched User-Agent */
	char *userAgent; /* Pointer to the User-Agent */
	int difference; /* Difference in length between the target and matched
					   User-Agent */
} fiftyoneDegreesDeviceOffset;

/* Used to return results from a device detection operation */
typedef struct fiftyoneDegrees_device_offsets_t {
	int size; /* The number of records in the array */
	fiftyoneDegreesDeviceOffset *firstOffset; /* First item in the array of
											     offsets */
	fiftyoneDegreesActiveDataSet *active; /* Pointer to the current dataset */
} fiftyoneDegreesDeviceOffsets;

/* Used to map a byte from the data file. */
typedef unsigned char byte;

/* Used to store the date when the dataset was produced and next date 51Degrees 
 * expect to provide a new data file. */
#pragma pack(push, 1)
typedef struct fiftyoneDegrees_date_t {
	const int16_t year;
	const byte month;
	const byte day;
} fiftyoneDegreesDate;
#pragma pack(pop)

/* HTTP header prefix used when processing collections of parameters. */
#define FIFTYONEDEGREES_HTTP_PREFIX_UPPER "HTTP_"

/* A property including references to HTTP headers. */
#pragma pack(push, 4)
typedef struct fiftyoneDegrees_property_t {
	const int32_t stringOffset; /* Offset of the property name in the strings
								   structure. */
	const int32_t componentIndex; /* Component the property relates to. */
	const int32_t subIndex; /* Property index within the component. */
	const int32_t headerCount; /* Number of relevant HTTP headers. */
	const int32_t headerFirstIndex; /* First relevant HTTP header. */
} fiftyoneDegreesProperty;
#pragma pack(pop)

/* Hash record structure to compare to a substring hash. */
#pragma pack(push, 4)
typedef struct fiftyoneDegrees_signature_node_hash_t {
	uint32_t hashCode; /* Hash code to compare. */
	int32_t nodeOffset; /* Offset of the node to use if this hash code is a
						   match. */
} fiftyoneDegreesSignatureNodeHash;
#pragma pack(pop)

/**
 * Signature node structure used to construct the directed acyclic graph to
 * search.
 */
#pragma pack(push, 1)
typedef struct fiftyoneDegrees_signature_node_t {
	int32_t unmatchedNodeOffset; /* Offset of the node to use if there is no
								    matching hash record. */
	int16_t firstIndex; /* First character index to search for a matching hash
						   code. */
	int16_t lastIndex; /* Last character index to search for a matching hash
					      code. */
	byte length; /* Length of the substring to hash. */
	int32_t hashesCount; /* Number of hash records in the node. */
	int32_t modulo; /* Modulo to use when the hashes are a hash table. */
	fiftyoneDegreesSignatureNodeHash hashes; /* The first hash record. */
} fiftyoneDegreesSignatureNode;
#pragma pack(pop)

/* Dataset header containing information about the dataset. */
#pragma pack(push, 2)
typedef struct fiftyoneDegrees_dataset_header_t {
	const uint16_t version; /* The version of the data file. */
	const int32_t formatOffset; /* Offset of the dataset format in the strings
								   structure. */
	const int32_t nameOffset; /* Offset of the dataset name in strings
							     structure. */
	const byte tag[16]; /* Unique data file tag. */
	const fiftyoneDegreesDate published; /* Date the datafile was published. */
	const fiftyoneDegreesDate nextUpdate; /* Date of the next data file to be
										     published. */
	const int32_t copyrightOffset; /* Offset of the copyright in the strings
								      data structure. */
	const uint16_t maxStringLength; /* Maximum length of a string in the
									   strings data structure. */
} fiftyoneDegreesDataSetHeader;
#pragma pack(pop)

typedef struct fiftyoneDegrees_list_t {
	byte *firstElement; 
	unsigned int count;
	int freeMemory;
} fiftyoneDegreesList;

typedef struct fiftyoneDegrees_integers_t {
	int32_t *firstElement;
	unsigned int count;
	int freeMemory;
} fiftyoneDegreesIntegers;

typedef struct fiftyoneDegrees_properties_t {
	fiftyoneDegreesProperty *firstElement;
	unsigned int count;
	int freeMemory;
} fiftyoneDegreesProperties;

/**
* Reference information associated with entity types that can vary based on
* compiler directives.
*/
typedef struct fiftyoneDegrees_collection_t {
#ifdef FIFTYONEDEGREES_INDIRECT
	fiftyoneDegreesCache *cache; /* Pointer to the cache. */
	long offset; /* Offset to the data structure in the file. */
	FILE *handle; /* File handle to use to load entities into the cache. */
	int elementSize; /* If the entities are fixed size the size, otherwise 0 */
#else
	byte *firstByte; /* The first byte in memory of the data structure. */
	unsigned int count;
#endif
} fiftyoneDegreesCollection;

/* Dataset structure containing all the components used for detections. */
typedef struct fiftyoneDegrees_dataset_t {
	const fiftyoneDegreesDataSetHeader header; /* Dataset header. */
#ifndef FIFTYONEDEGREES_INDIRECT
	const void *memoryToFree; /* A pointer to the memory where the dataset is
							     held. Not used in indirect operation. */
#endif
	const char *fileName; /* The location of the file the data set has been
						     loaded from. */
	int devicePropertiesCount; /* Number of properties referenced from the 
							   device rather than profiles. */
	int baseDrift; /* Number of characters to search either size of the
					   defined ranges. */
	int baseDifference; /* The difference in ASCII character value to
							allow. */
	int devicesIntegerCount; /* The number of integers associated with a device 
							 record. */
	const char **requiredPropertiesNames; /* A list of pointers to the names of
										     the required properties. */
	const char **prefixedUpperHttpHeaders; /* Pointer to an array of prefixed
										   upper HTTP headers. */
	fiftyoneDegreesIntegers components; /* Array of components. */
	fiftyoneDegreesProperties allProperties; /* Array of available properties. */
	fiftyoneDegreesIntegers httpHeaders; /* Array of HTTP headers. */
	fiftyoneDegreesIntegers uniqueHttpHeaders; /* Array of unique HTTP headers. */
	fiftyoneDegreesIntegers requiredProperties; /* Array of required property 
												   indexes. */
	/* Following collections can be either cached or held in memory depending on 
	the mode of operation. */
	fiftyoneDegreesCollection strings;
	fiftyoneDegreesCollection profiles;
	fiftyoneDegreesCollection devices;
	fiftyoneDegreesCollection nodes;
} fiftyoneDegreesDataSet;

/* Initial declaration of the provider structure. */
typedef struct fiftyoneDegrees_provider_t fiftyoneDegreesProvider;

/**
 * Active wrapper for the provider's dataset. This is used to make reloading
 * thread safe.
 */
struct fiftyoneDegrees_active_dataset_t {
	fiftyoneDegreesDataSet *dataSet; /* Pointer to an initialised data set. */
	fiftyoneDegreesProvider *provider; /* Pointer to the provider the active
									   wrapper relates to. */
	volatile long inUse; /* Counter indicating how many device offsets are still
						linked to this dataset. */
};

/* Provider structure containing the dataset used for detections. */
struct fiftyoneDegrees_provider_t {
#ifndef FIFTYONEDEGREES_NO_THREADING
	volatile fiftyoneDegreesActiveDataSet *active; /* Volatile wrapper for the
												      provider's data set. */
	FIFTYONEDEGREES_MUTEX lock; /* Used to lock critical regions where mutable
								   variables are written to */
#else
	fiftyoneDegreesActiveDataSet *active; /* Non volatile wrapper for the
										     provider's data set. */
#endif
};

/**
 * \ingroup FiftyOneDegreesFunctions
 * Initialises the dataset using the file provided and an array of properties.
 * @param fileName the path to a 51Degrees data file.
 * @param dataSet pointer to a dataset which has been allocated with the
 * correct size.
 * @param properties a string array containing the properties to be
 * initialised.
 * @param propertyCount the number of properties in the array
 * @returns fiftyoneDegreesDataSetInitStatus indicates whether or not the
 *  dataset has been initialised correctly.
 */
EXTERNAL fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyArray(
	const char* fileName,
	fiftyoneDegreesDataSet *dataSet,
	const char** properties,
	int propertyCount);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Initialises the dataset using the file provided and a string of properties.
 * @param fileName the path to a 51Degrees data file.
 * @param dataSet pointer to a dataset which has been allocated with the
 * correct size.
 * @param properties a comma separated string containing the properties to be
 * initialised.
 * @returns fiftyoneDegreesDataSetInitStatus indicates whether or not the
 * dataset has been initialised correctly.
 */
EXTERNAL fiftyoneDegreesDataSetInitStatus
fiftyoneDegreesInitWithPropertyString(
	const char *fileName,
	fiftyoneDegreesDataSet *dataSet,
	const char *properties);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns the number of characters which matched in the Trie.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent User-Agent to use.
 * @returns int the number of matching characters from the User-Agent.
 */
EXTERNAL int fiftyoneDegreesGetDeviceOffset(
	fiftyoneDegreesDataSet *dataSet,
	const char *userAgent);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Sets the offsets structure passed to the method for the User-Agent provided.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent to match for.
 * @param httpHeaderIndex of the User-Agent.
 * @param offset to set.
 */
EXTERNAL void fiftyoneDegreesSetDeviceOffset(
	fiftyoneDegreesDataSet *dataSet,
	const char* userAgent,
	int httpHeaderIndex,
	fiftyoneDegreesDeviceOffset *offset);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Sets the offsets structure passed to the method for the User-Agent provided.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent to match for.
 * @param httpHeaderIndex of the User-Agent.
 * @param offset to set.
 * @param drift to extend the search range by.
 * @param difference to allow in hash values.
 */
EXTERNAL void fiftyoneDegreesSetDeviceOffsetWithTolerances(
	fiftyoneDegreesDataSet *dataSet,
	const char* userAgent,
	int httpHeaderIndex,
	fiftyoneDegreesDeviceOffset *offset,
	int drift,
	int difference);

	/**
 * \ingroup FiftyOneDegreesFunctions
 * Sets the offsets structure passed to the method for the User-Agent provided.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent to match for.
 * @param userAgentLength of the User-Agent.
 * @param httpHeaderIndex of the User-Agent.
 * @param offset to set.
 * @param drift to extend the search range by.
 * @param difference to allow in hash values.
 */
EXTERNAL void fiftyoneDegreesSetDeviceOffsetFromArrayWithTolerances(
	fiftyoneDegreesDataSet *dataSet,
	const char* userAgent,
	int userAgentLength,
	int httpHeaderIndex,
	fiftyoneDegreesDeviceOffset *offset,
	int drift,
	int difference);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns the offsets to a matching devices based on the HTTP headers provided.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaders to match for.
 * @param size of the HTTP headers string.
 * @returns fiftyoneDegreesDeviceOffsets* pointer to newly created device
 * offsets from the match.
 */
EXTERNAL fiftyoneDegreesDeviceOffsets*
fiftyoneDegreesGetDeviceOffsetsWithHeadersString(
	fiftyoneDegreesDataSet *dataSet,
	char *httpHeaders,
	size_t length);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Creates a new device offsets structure with memory allocated.
 * @param dataSet pointer to an initialised dataset.
 * @returns fiftyoneDegreesDeviceOffsets* newly created device offsets.
 */
EXTERNAL fiftyoneDegreesDeviceOffsets* fiftyoneDegreesCreateDeviceOffsets(
	fiftyoneDegreesDataSet *dataSet);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Resets the device offsets to the state they were in when they were created
 * with fiftyoneDegreesCreateDeviceOffsets.
 * @param offsets to reset.
 */
EXTERNAL void fiftyoneDegreesResetDeviceOffsets(
	fiftyoneDegreesDeviceOffsets* offsets);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Frees the memory used by the offsets.
 * @param offsets to free.
 */
EXTERNAL void fiftyoneDegreesFreeDeviceOffsets(
	fiftyoneDegreesDeviceOffsets* offsets);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Creates a new device offsets structure with memory allocated and
 * increments the inUse counter in the provider so the dataset will
 * not be freed until this is. A corresponding call to 
 * fiftyoneDegreesProviderFreeDeviceOffsets must be made when these
 * offsets are finished with.
 * @param provider pointer to an initialised provider.
 * @returns fiftyoneDegreesDeviceOffsets* newly created device offsets.
 */
EXTERNAL fiftyoneDegreesDeviceOffsets*
fiftyoneDegreesProviderCreateDeviceOffsets(
	fiftyoneDegreesProvider *provider);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Frees the memory used by the offsets created by
 * fiftyoneDegreesProviderCreateDeviceOffsets and decrements the inUse counter
 * for the associated dataset.
 * @param offsets to free.
 */
EXTERNAL void fiftyoneDegreesProviderFreeDeviceOffsets(
	fiftyoneDegreesDeviceOffsets* offsets);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns the offsets to a matching devices based on the HTTP headers provided.
 * @param dataSet pointer to an initialised dataset.
 * @param offsets to set.
 * @param httpHeaders to match for.
 * @param size of the headers string.
 */
EXTERNAL void fiftyoneDegreesSetDeviceOffsetsWithHeadersString(
	fiftyoneDegreesDataSet *dataSet,
	fiftyoneDegreesDeviceOffsets *offsets,
	char *httpHeaders,
	size_t size);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns the index of the property requested, or -1 if not available.
 * @param dataSet pointer to an initialised dataset.
 * @param value name of the property to find.
 * @returns int the index of the property requested, or -1 if not available.
 */
EXTERNAL int fiftyoneDegreesGetPropertyIndex(
	fiftyoneDegreesDataSet *dataSet,
	const char *value);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Takes the results of getDeviceOffset and getPropertyIndex to return a value.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffset to get the property from.
 * @param propertyIndex of the requested property.
 * @return const char* the value of the requested property
 */
EXTERNAL const char* fiftyoneDegreesGetValue(
	fiftyoneDegreesDataSet *dataSet,
	int deviceOffset,
	int propertyIndex);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns the number of properties that have been loaded in the dataset.
 * @param dataSet pointer to an initialised dataset,
 * @returns int32_t number of initialised properties in the dataset.
 */
EXTERNAL int fiftyoneDegreesGetRequiredPropertiesCount(
	fiftyoneDegreesDataSet *dataSet);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns the names of the properties loaded in the dataset.
 * @param dataSet pointer to an initialised dataset.
 * @retuens const char** pointer to the array of initialised properties.
 */
EXTERNAL const char** fiftyoneDegreesGetRequiredPropertiesNames(
	fiftyoneDegreesDataSet *dataSet);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns the index in the array of required properties for this name, or -1 if not found.
 * @param dataSet pointer to an initialised dataset.
 * @param propertyName name of the property to get.
 * @returns int index in the dataset's requiredProperties array, or -1 if not found.
 */
EXTERNAL int fiftyoneDegreesGetRequiredPropertyIndex(
	fiftyoneDegreesDataSet *dataSet,
	const char *propertyName);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Frees the memory used by the dataset.
 * @param dataSet a pointer to the dataset to be freed.
 */
EXTERNAL void fiftyoneDegreesDataSetFree(fiftyoneDegreesDataSet *dataSet);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns the number of HTTP headers relevant to device detection.
 * @param dataSet pointer to an initialised dataset.
 * @returns int the number of relevant HTTP headers.
 */
EXTERNAL int fiftyoneDegreesGetHttpHeaderCount(
	fiftyoneDegreesDataSet *dataSet);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns the HTTP header name offset at the index provided.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaderIndex index of the header to get.
 * @returns int the name offset of the HTTP header.
 */
EXTERNAL int fiftyoneDegreesGetHttpHeaderNameOffset(
	fiftyoneDegreesDataSet *dataSet,
	int httpHeaderIndex);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns a pointer to the HTTP header name at the index provided.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaderIndex index of the header to get.
 * @returns const char* the name of the HTTP header.
 */
EXTERNAL const char* fiftyoneDegreesGetHttpHeaderNamePointer(
	fiftyoneDegreesDataSet *dataSet,
	int httpHeaderIndex);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns the name of the header in prefixed upper case form at the index
 * provided, or NULL if the index is not valid.
 * @param dataSet pointer to an initialised dataset
 * @param httpHeaderIndex index of the HTTP header name required
 * @returns name of the header, or NULL if index not valid
 */
EXTERNAL const char* fiftyoneDegreesGetPrefixedUpperHttpHeaderName(
	fiftyoneDegreesDataSet *dataSet,
	int httpHeaderIndex);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Sets the HTTP header string to the header name at the index provided.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaderIndex index of the header to get.
 * @param httpHeader to set.
 * @param size allocated to httpHeader.
 * @returns int the length of the HTTP header, or the requred length as a
 * negative if size is not large enough.
 */
EXTERNAL int fiftyoneDegreesGetHttpHeaderName(
	fiftyoneDegreesDataSet *dataSet,
	int httpHeaderIndex,
	char* httpHeader,
	int size);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns the index of the unique header, or -1 if the header is not important.
 * @param dataSet pointer to an initialised dataset.
 * @param httpHeaderName name of the header to get the index of.
 * @param length of the header name.
 * @returns int index of the unique header, or -1 if the header is not
 * important.
 */
EXTERNAL int fiftyoneDegreesGetUniqueHttpHeaderIndex(
	fiftyoneDegreesDataSet *dataSet,
	char* httpHeaderName,
	int length);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Sets the propertyName string to the property name at the index provided.
 * @param dataSet pointer to an initialised dataset.
 * @param requiredPropertIndex index in the dataset's requiredProperties array.
 * @param propertyName to set.
 * @param size allocated to propertyName.
 * @returns int the length of the property name , or the requred length as a
 * negative if size is not large enough.
 */
EXTERNAL int fiftyoneDegreesGetRequiredPropertyName(
	fiftyoneDegreesDataSet *dataSet,
	int requiredPropertyIndex,
	char* propertyName,
	int size);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Sets the values string to the property values for the device offsets and
 * index provided.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffsets to get the property from.
 * @param requiredPropertyIndex index in the dataset's requiredProperties 
 *		  array.
 * @param values string to set.
 * @param size allocated to the values string.
 * @returns int the length of the values string, or the required length as a
 * negative if size is not large enough.
 */
EXTERNAL int fiftyoneDegreesGetValueFromOffsets(
	fiftyoneDegreesDataSet *dataSet,
	fiftyoneDegreesDeviceOffsets* deviceOffsets,
	int requiredPropertyIndex,
	char* values,
	int size);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns a pointer to the value for the property based on the device offsets
 * provided.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffsets to get the property from.
 * @param requiredPropertyIndex index in the dataset's requiredProperties array.
 * @returns const char* pointer to the value of the requested property, or NULL
 * if the property does not exist.
 */
EXTERNAL const char* fiftyoneDegreesGetValuePtrFromOffsets(
	fiftyoneDegreesDataSet *dataSet,
	fiftyoneDegreesDeviceOffsets* deviceOffsets,
	int requiredPropertyIndex);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Process device properties into a CSV string for the device offset provided.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffset from a match to process.
 * @param result buffer to store the CSV in.
 * @param resultLength allocated to result buffer.
 * @returns int the length used in the buffer, 0 if there are no properties, or
 * -1 if the buffer is not long enough.
 */
EXTERNAL int fiftyoneDegreesProcessDeviceCSV(
	fiftyoneDegreesDataSet *dataSet,
	int deviceOffset,
	char* result,
	int resultLength);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Process device properties into a JSON string for the device offset provided.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffset from a match to process.
 * @param result buffer to store the JSON in.
 * @param resultLength allocated to result buffer.
 * @returns int the length used in the buffer, 0 if there are no properties, or
 * -1 if the buffer is not long enough.
 */
EXTERNAL int fiftyoneDegreesProcessDeviceJSON(
	fiftyoneDegreesDataSet *dataSet,
	int deviceOffset,
	char* result,
	int resultLength);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Process device properties into a JSON string for the device offsets provided.
 * @param dataSet pointer to an initialised dataset.
 * @param deviceOffsets from a match to process.
 * @param result buffer to store the JSON in.
 * @param resultLength allocated to result buffer.
 * @returns int the length used in the buffer, 0 if there are no properties, or
 * -1 if the buffer is not long enough.
 */
EXTERNAL int fiftyoneDegreesProcessDeviceOffsetsJSON(
	fiftyoneDegreesDataSet *dataSet,
	fiftyoneDegreesDeviceOffsets *deviceOffsets,
	char* result,
	int resultLength);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Returns the number of characters which matched in the Trie.
 * @param dataSet pointer to an initialised dataset.
 * @param userAgent User-Agent to use.
 * @returns int the number of matching characters from the User-Agent.
 */
EXTERNAL int fiftyoneDegreesGetMatchedUserAgentLength(
	fiftyoneDegreesDataSet *dataSet,
	char *userAgent);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Pointer to malloc function. Defaults to malloc, but can be set externally.
 * @param __size to allocate.
 * @returns pointer to allocated memory, or NULL if allocation failed.
 */
EXTERNAL void *(FIFTYONEDEGREES_CALL_CONV *fiftyoneDegreesMalloc)(size_t __size);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Pointer to free function. Defaults to free, but can be set externally.
 * @param __ptr pointer to the memory to free
 */
EXTERNAL void (FIFTYONEDEGREES_CALL_CONV *fiftyoneDegreesFree)(void *__ptr);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Get the size the dataset will need in memory when initialised with the
 * provided properties. Returns -1 if the file could not be accessed.
 * @param fileName path to a valid data file.
 * @param properties comma separated list of property strings.
 * @returns size_t the size in memory needed to initialise the dataset,
 * or -1 if the file could not be accessed.
 */
EXTERNAL size_t fiftyoneDegreesGetProviderSizeWithPropertyString(
	const char* fileName,
	const char* properties);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Get the size the dataset will need in memory when initialised with the
 * provided properties. Returns -1 if the file could not be accessed.
 * @param fileName path to a valid data file.
 * @param propertyCount the number of properties to be initialised.
 * @returns size_t the size in memory needed to initialise the dataset,
 * or -1 if the file could not be accessed.
 */
EXTERNAL size_t fiftyoneDegreesGetProviderSizeWithPropertyCount(
	const char* fileName,
	int propertyCount);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Initialises the provider using the file provided and a string of properties.
 * @param fileName the path to a 51Degrees data file.
 * @param provider pointer to a provider which has been allocated with the
 * correct size.
 * @param properties a comma separated string containing the properties to be
 * initialised.
 * @returns fiftyoneDegreesDataSetInitStatus indicates whether or not the
 * provider has been initialised correctly.
 */
EXTERNAL fiftyoneDegreesDataSetInitStatus
fiftyoneDegreesInitProviderWithPropertyString(
	const char* fileName,
	fiftyoneDegreesProvider* provider,
	const char* properties);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Initialises the provider using the file provided and an array of properties.
 * @param fileName the path to a 51Degrees data file.
 * @param provider pointer to a provider which has been allocated with the
 * correct size.
 * @param properties a string array containing the properties to be
 * initialised.
 * @param propertyCount the number of properties in the array.
 * @returns fiftyoneDegreesDataSetInitStatus indicates whether or not the
 * provider has been initialised correctly.
 */
EXTERNAL fiftyoneDegreesDataSetInitStatus
fiftyoneDegreesInitProviderWithPropertyArray(
	const char* fileName,
	fiftyoneDegreesProvider* provider,
	const char** properties,
	int propertyCount);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Releases all the resources used by the provider. The provider can not be
 * used without being reinitialised after calling this method.
 * @param provider pointer to the provider to be freed.
 */
EXTERNAL void fiftyoneDegreesProviderFree(fiftyoneDegreesProvider* provider);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Creates a new dataset using the same configuration options
 * as the current data set associated with the provider. The data file
 * which the provider was initialised with  is used to create the new data set.
 * @param provider pointer to the provider whose data set should be reloaded
 * @return fiftyoneDegreesDataSetInitStatus indicating the result of the reload
 * 	   operation.
 */
EXTERNAL fiftyoneDegreesDataSetInitStatus fiftyoneDegreesProviderReloadFromFile(
	fiftyoneDegreesProvider* provider);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Creates a new dataset using the same configuration options
 * as the current data set associated with the provider. The
 * memory located at the source pointer is used to create the new data set.
 * Important: The memory pointed to by source will NOT be freed by 51Degrees
 * when the associated data set is freed. The caller is responsible for
 * releasing the memory. If 51Degrees should release the memory then the
 * caller should set the memoryToFree field of the data set associated with
 * the returned pool to source. 51Degrees will then free this memory when the
 * data set is freed.
 * @param provider pointer to the provider whose data set should be reloaded
 * @param source pointer to the dataset held in memory.
 * @param length number of bytes that the file occupies in memory.
 * @return fiftyoneDegreesDataSetInitStatus indicating the result of the reload
 * 	   operation.
 */
EXTERNAL fiftyoneDegreesDataSetInitStatus fiftyoneDegreesProviderReloadFromMemory(
	fiftyoneDegreesProvider *provider,
	void *source,
	long length);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Sets the drift parameter for detection. By default, the drift is set to
 * zero. The drift parameter indicates how much the range is extended when
 * searching for a substring in a User-Agent. For example, if the drift is
 * set to one, then in addition to searching in the range first to last,
 * the range is extended to (first - 1) and (last + 1).
 * @param provider pointer to the provider to set the drift in
 * @param drift value of drift to set.
 */
EXTERNAL void fiftyoneDegreesSetDrift(
	fiftyoneDegreesProvider *provider,
	int drift);

/**
 * \ingroup FiftyOneDegreesFunctions
 * Sets the difference parameter for detection. By default, the difference is
 * set to zero. The difference parameter indicates the allowed difference in
 * hash value. This is most useful for the last character of a sub string, as
 * the sub string's hash code will be changed only be the change in ASCII value
 * of the final character. For example, if "Chrome 51" has the hash code 1234,
 * then "Chrome 52" will have the hash code 1235.
 * @param provider pointer to the provider to set the difference in
 * @param difference value of difference to set.
 */
EXTERNAL void fiftyoneDegreesSetDifference(
	fiftyoneDegreesProvider *provider,
	int difference);

/**
* \ingroup FiftyOneDegreesFunctions
* Determines if the compiled code supports multi threading.
* @return boolean where true means multi threading is supported.
*/
EXTERNAL int fiftyoneDegreesGetIsThreadSafe();

/**
 * \ingroup FiftyOneDegreesFunctions
 * @param dataSet pointer to an initialised data set.
 * @returns the format of the data set used contained in the source file.
 */
EXTERNAL char* fiftyoneDegreesGetDataSetFormat(fiftyoneDegreesDataSet *dataSet);

/**
 * \ingroup FiftyOneDegreesFunctions
 * @param dataSet pointer to an initialised data set.
 * @returns the name of the data set used contained in the source file.
 */
EXTERNAL char* fiftyoneDegreesGetDataSetName(fiftyoneDegreesDataSet *dataSet);


#endif // 51DEGREES_H_INCLUDED
