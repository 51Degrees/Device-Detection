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

#ifndef FIFTYONEDEGREES_H_INCLUDED
#define FIFTYONEDEGREES_H_INCLUDED

#ifndef FIFTYONEDEGREES_H_PATTERN_INCLUDED
#define FIFTYONEDEGREES_H_PATTERN_INCLUDED
#endif

#ifdef _MSC_VER
#define _INTPTR 0
#endif

#ifdef __cplusplus
#define EXTERNAL extern "C"
#else
#define EXTERNAL
#endif

#include <stdint.h>
#include <limits.h>
#include <time.h>
#include "../threading.h"

/* Used to represent bytes */
typedef unsigned char byte;

/* Used to return the match method */
typedef enum e_fiftyoneDegrees_MatchMethod {
	NONE,
	EXACT,
	NUMERIC,
	NEAREST,
	CLOSEST
} fiftyoneDegreesMatchMethod;

/* Used to indicate what state the result set is in the cache */
typedef enum e_fiftyoneDegrees_Resultset_CacheState {
	ACTIVE_CACHE_LIST_ONLY,
	BOTH_CACHE_LISTS
} fiftyoneDegreesResultsetCacheState;

/* Used to provide the status of the data set initialisation */
typedef enum e_fiftyoneDegrees_DataSetInitStatus {
	DATA_SET_INIT_STATUS_SUCCESS,
	DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY,
	DATA_SET_INIT_STATUS_CORRUPT_DATA,
	DATA_SET_INIT_STATUS_INCORRECT_VERSION,
	DATA_SET_INIT_STATUS_FILE_NOT_FOUND,
	DATA_SET_INIT_STATUS_NOT_SET
} fiftyoneDegreesDataSetInitStatus;

typedef struct fiftyoneDegreesRange_t {
	const int16_t lower;
	const int16_t upper;
} fiftyoneDegreesRange;

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_ascii_string_t {
	const int16_t length;
	const char firstByte;
} fiftyoneDegreesAsciiString;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_component_t {
	const byte componentId; /* The unique Id of the component. */
	const int32_t nameOffset; /* Offset in the strings data structure to the name */
	const int32_t defaultProfileOffset; /* Offset in the profiles data structure to the default profile */
	const uint16_t httpHeaderCount; /* The number of http header offsets at httpHeaderFirstOffset */
} fiftyoneDegreesComponent;
#pragma pack(pop)

#pragma pack(push, 4)
typedef struct fiftyoneDegrees_map_t {
	const int32_t nameOffset; /* Offset in the strings data structure to the name */
} fiftyoneDegreesMap;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct fiftyoneDegrees_node_numeric_index_t {
	const int16_t value; /* The numeric value of the index */
	const int32_t relatedNodeOffset; /* The node offset which the numeric value relates to */
} fiftyoneDegreesNodeNumericIndex;
#pragma pack(pop)

#pragma pack(push, 4)
typedef union fiftyoneDegrees_node_index_value_t {
	const byte characters[4]; /* If not a string the characters to be used */
	const int32_t integer; /* If a string the offset in the strings data structure of the characters */
} fiftyoneDegreesNodeIndexValue;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_node_index_t {
	const int32_t relatedNodeOffset; /* The node offset which the value relates to. Must be converted to absolute value. */
	union {
		const byte characters[4];
		const int32_t integer;
	} value; /* The value of the index as either an integer or character array */
} fiftyoneDegreesNodeIndex;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct fiftyoneDegrees_string_t {
	byte* value;
	int16_t length;
} fiftyoneDegreesString;
#pragma pack(pop)

typedef struct fiftyoneDegrees_strings_t {
	const fiftyoneDegreesString *firstString;
	const int32_t count;
} fiftyoneDegreesStrings;

#pragma pack(push, 2)
typedef struct fiftyoneDegrees_node_t {
	const int16_t position;
	const int16_t nextCharacterPosition;
	const int32_t parentOffset;
	const int32_t characterStringOffset;
	const int16_t childrenCount;
	const int16_t numericChildrenCount;
	const uint16_t signatureCount;
} fiftyoneDegreesNode;
#pragma pack(pop)

#pragma pack(push, 4)
typedef struct fiftyoneDegrees_profile_offset_t {
	const int32_t profileId; /* The unique Id of the profile */
	const int32_t offset; /* Offset to the profile in the profiles structure */
} fiftyoneDegreesProfileOffset;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct property_t {
	const byte componentIndex; /* Index of the component */
	const byte displayOrder; /* The order the property should be displayed in relative to other properties */
	const byte isMandatory; /* True if the property is mandatory and must be provided */
	const byte isList; /* True if the property is a list can can return multiple values */
	const byte showValues; /* True if the values should be shown in GUIs */
	const byte isObsolete; /* True if the property is obsolete and will be removed from future data sets */
	const byte show; /* True if the property should be shown in GUIs */
	const byte valueType; /* The type of value the property represents */
	const int32_t defaultValueIndex; /* The default value index for the property */
	const int32_t nameOffset; /* The offset in the strings structure to the property name */
	const int32_t descriptionOffset; /* The offset in the strings structure to the property description */
	const int32_t categoryOffset; /* The offset in the strings structure to the property category */
	const int32_t urlOffset; /* The offset in the strings structure to the property url */
	const int32_t firstValueIndex; /* Index of the first possible value */
	const int32_t lastValueIndex; /* Index of the last possible value */
	const int32_t mapCount; /* Number of maps the property is associated with */
	const int32_t firstMapIndex; /* The first index in the list of maps the property is associated with */
} fiftyoneDegreesProperty;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct fiftyoneDegrees_value_t {
	const int16_t propertyIndex; /* Index of the property the value relates to */
	const int32_t nameOffset; /* The offset in the strings structure to the value name */
	const int32_t descriptionOffset; /* The offset in the strings structure to the value description */
	const int32_t urlOffset; /* The offset in the strings structure to the value url */
} fiftyoneDegreesValue;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_profile_t {
	const byte componentIndex;
	const int32_t profileId;
	const int32_t valueCount;
	const int32_t signatureCount;
} fiftyoneDegreesProfile;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct fiftyoneDegrees_date_t {
	const int16_t year;
	const byte month;
	const byte day;
} fiftyoneDegreesDate;
#pragma pack(pop)

#pragma pack(push, 4)
typedef struct fiftyoneDegrees_entity_header_t {
	const int32_t startPosition; /* Start position in the data file of the entities */
	const int32_t length; /* Length in bytes of the entities */
	const int32_t count; /* Number of entities in the collection */
} fiftyoneDegreesEntityHeader;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct fiftyoneDegrees_numeric_node_state {
	int32_t target;
	const fiftyoneDegreesNode *node;
	const fiftyoneDegreesNodeNumericIndex *firstNodeNumericIndex;
	const fiftyoneDegreesRange *range;
	int32_t startIndex;
	int32_t lowIndex;
	int32_t highIndex;
	byte lowInRange;
	byte highInRange;
} fiftyoneDegreesNumericNodeState;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_dataset_header_t {
	const int32_t versionMajor;
	const int32_t versionMinor;
	const int32_t versionBuild;
	const int32_t versionRevision;
	const byte tag[16];
	const byte exportTag[16];
	const int32_t copyrightOffset;
	const int16_t age;
	const int32_t minUserAgentCount;
	const int32_t nameOffset;
	const int32_t formatOffset;
	const fiftyoneDegreesDate published;
	const fiftyoneDegreesDate nextUpdate;
	const int32_t deviceCombinations;
	const int16_t maxUserAgentLength;
	const int16_t minUserAgentLength;
	const char lowestCharacter;
	const char highestCharacter;
	const int32_t maxSignatures;
	const int32_t signatureProfilesCount;
	const int32_t signatureNodesCount;
	const int16_t maxValues;
	const int32_t csvBufferLength;
	const int32_t jsonBufferLength;
	const int32_t xmlBufferLength;
	const int32_t maxSignaturesClosest;
	const int32_t maxRank;
	const fiftyoneDegreesEntityHeader strings;
	const fiftyoneDegreesEntityHeader components;
	const fiftyoneDegreesEntityHeader maps;
	const fiftyoneDegreesEntityHeader properties;
	const fiftyoneDegreesEntityHeader values;
	const fiftyoneDegreesEntityHeader profiles;
	const fiftyoneDegreesEntityHeader signatures;
	const fiftyoneDegreesEntityHeader signatureNodeOffsets;
	const fiftyoneDegreesEntityHeader nodeRankedSignatureIndexes;
	const fiftyoneDegreesEntityHeader rankedSignatureIndexes;
	const fiftyoneDegreesEntityHeader nodes;
	const fiftyoneDegreesEntityHeader rootNodes;
	const fiftyoneDegreesEntityHeader profileOffsets;
} fiftyoneDegreesDataSetHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_signature_t {
	const byte nodeCount;
	const int32_t firstNodeOffsetIndex;
	const int32_t rank;
	const byte flags;
} fiftyoneDegreesSignature;
#pragma pack(pop)

#pragma pack(push, 4)
typedef struct fiftyoneDegrees_http_header_t {
	int32_t headerNameOffset; /* Offset to the string with the header name */
	const char *headerName; /* Pointer to the header name string information */
} fiftyoneDegreesHttpHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_dataset_t {
	const fiftyoneDegreesDataSetHeader header;
	int32_t sizeOfSignature; /* The length in bytes of each signature record */
	int32_t signatureStartOfStruct; /* The number of bytes to ignore before the signature structure is found */
	const fiftyoneDegreesProperty **requiredProperties; /* Pointer to properties to be returned */
	int32_t requiredPropertyCount; /* Number of properties to return */
	const byte *strings;
	const byte *componentsData;
	const fiftyoneDegreesComponent **components;
	const fiftyoneDegreesMap *maps;
	const fiftyoneDegreesProperty *properties;
	const fiftyoneDegreesValue *values;
	const byte *profiles;
	const byte *signatures;
	const int32_t *signatureNodeOffsets;
	const int32_t *nodeRankedSignatureIndexes;
	const int32_t *rankedSignatureIndexes;
	const byte *nodes;
	const fiftyoneDegreesNode **rootNodes;
	const fiftyoneDegreesProfileOffset *profileOffsets;
	int32_t httpHeadersCount; /* Number of unique HTTP headers in the array */
	fiftyoneDegreesHttpHeader *httpHeaders; /* Array of HTTP headers the data set can process */
	const char **prefixedUpperHttpHeaders; /* Array of HTTP header strings in upper case form prefixed with HTTP_ */
} fiftyoneDegreesDataSet;
#pragma pack(pop)

typedef struct fiftyoneDegrees_linked_signature_list_item_t {
	struct fiftyoneDegrees_linked_signature_list_item_t *next;
	struct fiftyoneDegrees_linked_signature_list_item_t *previous;
	int32_t rankedSignatureIndex;
	int32_t frequency;
} fiftyoneDegreesLinkedSignatureListItem;

typedef struct fiftyoneDegrees_linked_signature_list_t {
	const fiftyoneDegreesLinkedSignatureListItem *items; /* List of signatures that are being evaluated by Closest match */
	fiftyoneDegreesLinkedSignatureListItem *first; /* Pointer to the first signature in the linked list */
	fiftyoneDegreesLinkedSignatureListItem *last; /* Pointer to the last signature in the linked list */
	int32_t count; /* The number of signatures pointed to by signatures */
	fiftyoneDegreesLinkedSignatureListItem *current; /* Pointer to the current item in the list when navigating */
} fiftyoneDegreesLinkedSignatureList;

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_resultset_t {
	const fiftyoneDegreesDataSet *dataSet; /* A pointer to the data set to use for the match */
	byte *targetUserAgentArray; /* An array of bytes representing the target user agent */
	uint16_t targetUserAgentArrayLength; /* The length of the target user agent */
	uint64_t targetUserAgentHashCode; /* The hash code of the target user agent */
	byte hashCodeSet; /* 0 if the hash code has not been calculated */
	fiftyoneDegreesMatchMethod method; /* The method used to provide the match result */
	int32_t difference; /* The difference score between the signature found and the target */
	int32_t rootNodesEvaluated; /* The number of root nodes evaluated */
	int32_t stringsRead; /* The number of strings read */
	int32_t nodesEvaluated; /* The number of nodes read during the detection */
	int32_t signaturesCompared; /* The number of signatures read in full and compared to the target */
	int32_t signaturesRead; /* The number of signatures read in full */
	int32_t closestSignatures; /* The total number of closest signatures available */
	const fiftyoneDegreesProfile *profiles; /* Pointer to a list of profiles returned for the match */
	int32_t profileCount; /* The number of profiles the match contains */
	byte *signature; /* The signature found if only one exists */
	struct fiftyoneDegrees_resultset_t *previous; /* The previous item in the linked list, or NULL if first */
	struct fiftyoneDegrees_resultset_t *next; /* The next item in the linked list, or NULL if last */
	fiftyoneDegreesResultsetCacheState state; /* Indicates if the result set is in the active, background or both lists */
} fiftyoneDegreesResultset;
#pragma pack(pop)

#pragma pack(push, 4)
typedef struct fiftyoneDegrees_resultset_cache_t fiftyoneDegreesResultsetCache;
#pragma pack(pop)

#pragma pack(push, 4)
typedef struct fiftyoneDegrees_resultset_cache_list_t {
	struct fiftyoneDegrees_resultset_cache_t *cache; /* Pointer to the cache the list is a part of */
	fiftyoneDegreesResultset **resultSets; /* Hashcode ordered list of pointers to resultsets in the cache list */
	int32_t allocated; /* The number of resultsets currently allocated in the list */
} fiftyoneDegreesResultsetCacheList;
#pragma pack(pop)

#pragma pack(push, 4)
typedef struct fiftyoneDegrees_resultset_cache_link_list_t {
	fiftyoneDegreesResultset *first; /* Pointer to the first item in the linked list */
	fiftyoneDegreesResultset *last; /* Pointer to the last item in the linked list */
	int32_t count; /* Number of items in the linked list */
} fiftyoneDegreesResultsetCacheLinkedList;
#pragma pack(pop)

#pragma pack(push, 4)
struct fiftyoneDegrees_resultset_cache_t {
	const fiftyoneDegreesDataSet *dataSet; /* A pointer to the data set to use with the cache */
	const fiftyoneDegreesResultset *resultSets; /* The start of the list of resultsets in the cache */
	int32_t sizeOfResultset; /* The number of bytes used for each resultset */
	int32_t total; /* The number of resultset items in the cache */
	fiftyoneDegreesResultsetCacheLinkedList free; /* Linked list of pointers to free resultsets */
	fiftyoneDegreesResultsetCacheLinkedList allocated; /* Linked list of pointers to allocated resultsets */
	fiftyoneDegreesResultsetCacheList *active; /* List of cache items that are actively being checked */
	fiftyoneDegreesResultsetCacheList *background; /* List of cache items that are being recorded as recently accessed */
	int32_t switchLimit; /* The number of items that can be allocated before the caches are switched */
	int32_t hits; /* The number of times an item was found in the cache */
	int32_t misses; /* The number of times an item was not found in the cache */
	int32_t switches; /* The number of times the cache has been switched */
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX activeLock; /* Used to lock access to the active cache list */
	FIFTYONEDEGREES_MUTEX backgroundLock; /* Used to lock access to the background cache list */
#endif
};
#pragma pack(pop)

#pragma pack(push, 4)
typedef struct fiftyoneDegrees_http_header_workset_t {
	fiftyoneDegreesHttpHeader *header; /* Pointer to information about the header name and offset */
	const char *headerValue; /* Pointer to the header value */
	int headerValueLength; /* The length of the header value */
} fiftyoneDegreesHttpHeaderWorkset;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_workset_t {
	const fiftyoneDegreesDataSet *dataSet; /* A pointer to the data set to use for the match */
	char *targetUserAgentArray; /* An array of bytes representing the target user agent */
	uint16_t targetUserAgentArrayLength; /* The length of the target user agent */
	uint64_t targetUserAgentHashCode; /* The hash code of the target user agent */
	byte hashCodeSet; /* 0 if the hash code has not been calculated */
	fiftyoneDegreesMatchMethod method; /* The method used to provide the match result */
	int32_t difference; /* The difference score between the signature found and the target */
	int32_t rootNodesEvaluated; /* The number of root nodes evaluated */
	int32_t stringsRead; /* The number of strings read */
	int32_t nodesEvaluated; /* The number of nodes read during the detection */
	int32_t signaturesCompared; /* The number of signatures read in full and compared to the target */
	int32_t signaturesRead; /* The number of signatures read in full */
	int32_t closestSignatures; /* The total number of closest signatures available */
	const fiftyoneDegreesProfile **profiles; /* Pointer to a list of profiles returned for the match */
	int32_t profileCount; /* The number of profiles the match contains */
	byte *signature; /* The signature found if only one exists */
	const fiftyoneDegreesValue **values; /* Pointers to values associated with the property requested */
	int32_t valuesCount; /* Number of values available */
	char *input; /* An input buffer large enough to store the useragent to be matched */
	char *targetUserAgent; /* A pointer to the user agent string */
	char *relevantNodes; /* Pointer to a char array containing the relevant nodes */
	char *closestNodes; /* Pointer to a char array containing the closest nodes */
	char *signatureAsString; /* The signature as a string */
        char *tempheaderlowercase; /* temp variable to store http header name */
	const fiftyoneDegreesNode **nodes; /* Pointer to a list of nodes related to the match */
	const fiftyoneDegreesNode **orderedNodes; /* Pointer to a list of nodes in ascending order of signature count */
	int32_t nodeCount; /* The number of nodes referenced by **nodes */
	int32_t closestNodeRankedSignatureIndex; /* If a single node is returned the index of the ranked signature to be processed */
	fiftyoneDegreesLinkedSignatureList linkedSignatureList; /* Linked list of signatures used by Closest match */
	int16_t nextCharacterPositionIndex;
	byte startWithInitialScore; /* True if the NEAREST and CLOSEST methods should start with an initial score */
	int(*functionPtrGetScore)(struct fiftyoneDegrees_workset_t *ws, const fiftyoneDegreesNode *node); /* Returns scores for each different node between signature and match */
	const byte* (*functionPtrNextClosestSignature)(struct fiftyoneDegrees_workset_t *ws); /* Returns the next closest signature */
	const fiftyoneDegreesResultsetCache *cache; /* Pointer to the cache, or NULL if not available. */
	const fiftyoneDegreesProfile **tempProfiles; /* Pointer to a list of working profiles used during a multi header match */
	int32_t importantHeadersCount; /* Number of elements included in the important headers array */
	fiftyoneDegreesHttpHeaderWorkset *importantHeaders; /* Array of headers that are available and are important to detection */
} fiftyoneDegreesWorkset;
#pragma pack(pop)

typedef struct fiftyoneDegrees_workset_pool_t {
	fiftyoneDegreesDataSet *dataSet; /* Pointer to the dataset the pool relates to */
	fiftyoneDegreesResultsetCache *cache; /* Pointer to the cache to be used by the worksets */
	int32_t size; /* The maximum number of worksets the pool can contain */
	fiftyoneDegreesWorkset **worksets; /* Pointer to the array of work sets */
	int32_t available; /* The number of worksets that are available in the pool */
	int32_t created; /* The number of worksets created by the pool */
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX lock; /* Used to to limit access to the pool */
	FIFTYONEDEGREES_SIGNAL signal; /* Used to wait for a workset to be made available */
#endif
} fiftyoneDegreesWorksetPool;

/**
 * EXTERNAL METHODS
 */

/**
 * Initialises the data set passed to the method with the data from
 * the file provided. If required properties is provided the data set
 * will only return those contained in the array.
 * or tab.
 * @param fileName of the data source to use for initialisation
 * @param dataSet pointer to the data set
 * @param requiredProperties array of strings containing the property names
 * @param count the number of elements in the requiredProperties array
 * @return the number of bytes read from the file
 */
EXTERNAL fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyArray(const char *fileName, fiftyoneDegreesDataSet *dataSet, const char** properties, int32_t count);

/**
 * Initialises the data set passed to the method with the data from
 * the file provided. If required properties is provided the data set
 * will only return those listed and separated by comma, pipe, space
 * or tab.
 * @param fileName of the data source to use for initialisation
 * @param dataSet pointer to the data set
 * @param requiredProperties char array to the separated list of properties
 *        the dataSet can return
 * @return the number of bytes read from the file
 */
EXTERNAL fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyString(const char *fileName, fiftyoneDegreesDataSet *dataSet, const char* properties);

/**
 * Destroys the data set releasing all memory available. Ensure all worksets
 * cache and pool structs are freed prior to calling this method.
 * @param dataSet pointer to the data set being destroyed
 */
EXTERNAL void fiftyoneDegreesDataSetFree(const fiftyoneDegreesDataSet *dataSet);

/**
 * Creates a new cache used to speed up duplicate detections.
 * The cache must be destroyed with the fiftyoneDegreesFreeCache method.
 * If the cache size is lower then 2 then no cache is created.
 * @param dataSet pointer to the data set
 * @param size maximum number of items that the cache should store
 * @returns a pointer to the resultset cache created, or NULL
 */
EXTERNAL fiftyoneDegreesResultsetCache *fiftyoneDegreesResultsetCacheCreate(const fiftyoneDegreesDataSet *dataSet, int32_t size);

/**
 * Releases the memory used by the cache.
 * @param pointer to the cache created previously
 */
EXTERNAL void fiftyoneDegreesResultsetCacheFree(const fiftyoneDegreesResultsetCache *rsc);

/**
 * Creates a new workset pool for the data set and cache provided.
 * @param dataset pointer to a data set structure
 * @param cache pointer to a cache, or NULL if no cache to be used
 * @return a pointer to a new work set pool
 */
EXTERNAL fiftyoneDegreesWorksetPool *fiftyoneDegreesWorksetPoolCreate(fiftyoneDegreesDataSet *dataSet, fiftyoneDegreesResultsetCache *cache, int32_t size);

/**
 * Frees all worksets in the pool and releases all memory. Ensure all worksets
 * have been released back to the pool before calling this method.
 * @param pool pointer to the pool created by fiftyoneDegreesWorksetPoolCreate
 */
EXTERNAL void fiftyoneDegreesWorksetPoolFree(fiftyoneDegreesWorksetPool *pool);

/**
 * Gets a workset from the pool, or creates a new one if none are available
 * @param pool pointer to a pool structure
 * @returns pointer to a workset that is free and ready for use
 */
EXTERNAL fiftyoneDegreesWorkset *fiftyoneDegreesWorksetPoolGet(fiftyoneDegreesWorksetPool *pool);

/**
 * Releases the workset provided back to the pool making it available for future
 * use.
 * @param pool containing worksets
 * @param ws workset to be placed back on the queue
 */
EXTERNAL void fiftyoneDegreesWorksetPoolRelease(fiftyoneDegreesWorksetPool *pool, fiftyoneDegreesWorkset *ws);

/**
 * Creates a new workset to perform matches using the dataset provided.
 * The workset must be destroyed using the freeWorkset method when it's
 * finished with to release memory.
 * @param dataSet pointer to the data set
 * @param cache pointer or NULL if not used
 * @returns a pointer to the workset created
 */
EXTERNAL fiftyoneDegreesWorkset* fiftyoneDegreesWorksetCreate(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesResultsetCache *cache);

/**
 * Releases the memory used by the workset.
 * @param pointer to the workset created previously
 */
EXTERNAL void fiftyoneDegreesWorksetFree(const fiftyoneDegreesWorkset *ws);

/**
 * Allocates memory sufficiently large to store JSON results.
 * @param ws pointer to a workset with the results to return in JSON
 * @returns pointer to memory space to store JSON results
 */
EXTERNAL char* fiftyoneDegreesJSONCreate(fiftyoneDegreesWorkset *ws);

/**
 * Frees the memory space previously allocated by fiftyoneDegreesJSONCreate.
 * @param json pointer to the memory space to be freed
 */
EXTERNAL void fiftyoneDegreesJSONFree(void* json);

/**
 * Allocates memory sufficiently large to store CSV results.
 * @param ws pointer to a workset with the results to return in CSV
 * @returns pointer to memory space to store CSV results
 */
EXTERNAL char* fiftyoneDegreesCSVCreate(fiftyoneDegreesWorkset *ws);

/**
 * Frees the memory space previously allocated by fiftyoneDegreesCSVCreate.
 * @param csv pointer to the memory space to be freed
 */
EXTERNAL void fiftyoneDegreesCSVFree(void* csv);

/**
* Main entry method used for perform a match. First the cache is checked to
* determine if the userAgent has already been found. If not then detection
* is performed. The cache is then updated before the resultset is returned.
* @param ws pointer to a work set to be used for the match created via
*        createWorkset function
* @param userAgent pointer to the target user agent
*/
EXTERNAL void fiftyoneDegreesMatch(fiftyoneDegreesWorkset *ws, const char* userAgent);

/**
 * Passed array of HTTP header names and values. Sets the workset to
 * the results for these headers.
 * @param ws pointer to a work set to be used for the match created via
 *        createWorkset function
 * @param httpHeaderNames array of HTTP header names i.e. User-Agent
 * @param httpHeaderValues array of HTTP header values
 * @param the number of entires in each array
 */
EXTERNAL void fiftyoneDegreesMatchWithHeadersArray(fiftyoneDegreesWorkset *ws, const char **httpHeaderNames, const char **httpHeaderValues, int httpHeaderCount);

/**
 * Passed a string where each line contains the HTTP header name and value.
 * The first space character seperates the HTTP header name at the beginning of
 * the line and the value.
 * @param ws pointer to a work set to be used for the match created via
 *        createWorkset function
 * @param httpHeaders is a list of HTTP headers and values on each line
 * @param length number of characters in the headers array to consider
 */
EXTERNAL void fiftyoneDegreesMatchWithHeadersString(fiftyoneDegreesWorkset *ws, const char *httpHeaders, size_t length);

/**
 * Passed a string where each line contains the HTTP header name and value.
 * The first space character and/or colon seperates the HTTP header name
 * at the beginning of the line and the value. Does not perform a device
 * detection. Use fiftyoneDegreesMatchForHttpHeaders to complete a match.
 * @param ws pointer to a work set to have important headers set
 * @param httpHeaders is a list of HTTP headers and values on each line
 * @param length number of characters in the headers array to consider
 */
EXTERNAL int32_t fiftyoneDegreesSetHttpHeaders(fiftyoneDegreesWorkset *ws, const char *httpHeaders, size_t length);

/**
 * Sets the workset for the important headers included in the workset.
 * @param ws pointer to a work set to be used for the match created via
 *        createWorkset function
 */
EXTERNAL void fiftyoneDegreesMatchForHttpHeaders(fiftyoneDegreesWorkset *ws);

/**
 * Sets the values associated with the require property index in the workset
 * so that an array of values can be read.
 * @param ws pointer to the work set associated with the match
 * @param requiredPropertyIndex index of the property required from the array of
 *        require properties
 * @return the number of values that were set.
 */
EXTERNAL int32_t fiftyoneDegreesSetValues(fiftyoneDegreesWorkset *ws, int32_t requiredPropertyIndex);

/**
 * Returns a pointer to the ascii string at the byte offset provided
 * @param dataSet pointer to the data set
 * @param offset to the ascii string required
 * @return a pointer to the AsciiString at the offset
 */
EXTERNAL const fiftyoneDegreesAsciiString* fiftyoneDegreesGetString(const fiftyoneDegreesDataSet *dataSet, int32_t offset);

/**
* Sets the values character array to the values of the required property
* provided. If the values character array is too small then only the values
* that can be fitted in are added.
* @param ws pointer to a workset configured with the match results
* @param requiredPropertyIndex index of the required property
* @param values pointer to allocated memory to store the values
* @param size the size of the values memory
* @return the number of characters written to the values memory
*/
EXTERNAL int32_t fiftyoneDegreesGetValues(fiftyoneDegreesWorkset *ws, int32_t requiredPropertyIndex, char *values, int32_t size);

/**
 * Returns the name of the value provided.
 * @param dataSet pointer to the data set containing the value
 * @param value pointer whose name is required
 * @return pointer to the char string of the name
 */
EXTERNAL const char* fiftyoneDegreesGetValueName(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesValue *value);

/**
 * Returns the name of the property provided.
 * @param dataSet pointer to the data set containing the property
 * @param property pointer whose name is required
 * @return pointer to the char string of the name
 */
EXTERNAL const char* fiftyoneDegreesGetPropertyName(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesProperty *property);

/**
* Gets the required property name at the index provided.
* @param dataset pointer to an initialised dataset
* @param index of the property required
* @param propertyName pointer to memory to place the property name
* @param size of the memory allocated for the name
* @return the number of bytes written for the property
*/
EXTERNAL int32_t fiftyoneDegreesGetRequiredPropertyName(const fiftyoneDegreesDataSet *dataSet, int requiredPropertyIndex, char *propertyName, int size);

/**
* Gets the http header name at the index provided.
* @param dataset pointer to an initialised dataset
* @param index of the http header required
* @param httpHeader pointer to memory to place the http header name
* @param size of the memory allocated for the name
* @return the number of bytes written for the http header
*/
EXTERNAL int32_t fiftyoneDegreesGetHttpHeaderName(const fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex, char *httpHeader, int size);

/**
 * Returns the name of the header in prefixed upper case form at the index
 * provided, or NULL if the index is not valid.
 * @param dataSet pointer to an initialised dataset
 * @param httpHeaderIndex index of the HTTP header name required
 * @returns name of the header, or NULL if index not valid
 */
EXTERNAL const char* fiftyoneDegreesGetPrefixedUpperHttpHeaderName(const fiftyoneDegreesDataSet *dataSet, int httpHeaderIndex);

/**
* Gets the required property index of the property provided, or -1 if the
* property is not available in the dataset.
* @param dataset pointer to an initialised dataset
* @param propertyName pointer to the name of the property required
* @return the index of the property, or -1 if the property does not exist
*/
EXTERNAL int32_t fiftyoneDegreesGetRequiredPropertyIndex(const fiftyoneDegreesDataSet *dataSet, const char *propertyName);

/**
 * Process the workset results into a CSV string.
 * @param ws pointer to a workset with the results to return in CSV
 * @param csv pointer to memory allocated with fiftyoneDegreesCSVCreate
 */
EXTERNAL int32_t fiftyoneDegreesProcessDeviceCSV(fiftyoneDegreesWorkset *ws, char* csv);

/**
 * Process the workset results into a JSON string.
 * @param ws pointer to a workset with the results to return in JSON
 * @param json pointer to memory allocated with fiftyoneDegreesJSONCreate
 */
EXTERNAL int32_t fiftyoneDegreesProcessDeviceJSON(fiftyoneDegreesWorkset *ws, char* json);

/**
 * Returns the rank of the signature set in the workset.
 * @param ws pointer to the work set associated with the match
 * @returns the rank of the signature if available, or INT_MAX
 */
EXTERNAL int32_t fiftyoneDegreesGetSignatureRank(fiftyoneDegreesWorkset *ws);

/**
* Gets the signature as a string representing relevent user agent characters.
* @param ws pointer to the work set associated with the match
* @param signatureAsString pointer to memory to place the signature
* @param size of the memory allocated for the signature
* @return the number of bytes written for the signature
*/
EXTERNAL int32_t fiftyoneDegreesGetSignatureAsString(fiftyoneDegreesWorkset *ws, char *signatureAsString, int size);

/**
* Gets the device id as a string.
* @param ws pointer to the work set associated with the match
* @param deviceId pointer to memory to place the device id
* @param size of the memory allocated for the device id
* @return the number of bytes written for the device id
*/
EXTERNAL int32_t fiftyoneDegreesGetDeviceId(fiftyoneDegreesWorkset *ws, char *deviceId, int size);

/**
 * OBSOLETE METHODS - RETAINED FOR BACKWARDS COMPAITABILITY
 */

EXTERNAL fiftyoneDegreesWorkset* fiftyoneDegreesCreateWorkset(const fiftyoneDegreesDataSet *dataSet);

EXTERNAL void fiftyoneDegreesFreeWorkset(const fiftyoneDegreesWorkset *ws);

EXTERNAL void fiftyoneDegreesDestroy(const fiftyoneDegreesDataSet *dataSet);

#endif // 51DEGREES_H_INCLUDED
