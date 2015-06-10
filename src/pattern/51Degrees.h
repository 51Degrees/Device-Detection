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

#ifdef _MSC_VER
#define FIFTYONEDEGREES_MUTEX HANDLE
#else
#define FIFTYONEDEGREES_MUTEX pthread_mutex_t
#endif

#include <stdint.h>
#include <limits.h>
#include <time.h>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <pthread.h>
#endif

/* Used to represent bytes */
typedef unsigned char byte;

/* Single byte boolean representation */
typedef unsigned char fiftyoneDegreesBool;

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
} fiftyoneDegreesRANGE;

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_ascii_string_t {
	const int16_t length;
	const byte firstByte;
} fiftyoneDegreesAsciiString;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_component_t {
	const byte componentId; /* The unique Id of the component. */
	const int32_t nameOffset; /* Offset in the strings data structure to the name */
	const int32_t defaultProfileOffset; /* Offset in the profiles data structure to the default profile */
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
	const byte isString; /* True if the value is an offset in the ascii string collection */
	union {
		const byte characters[4];
		const int32_t integer;
	} value; /* The value of the index as either an integer or character array */
	const int32_t relatedNodeOffset; /* The node offset which the value relates to */
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
	const int32_t signatureCount;
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
	const fiftyoneDegreesBool isMandatory; /* True if the property is mandatory and must be provided */
	const fiftyoneDegreesBool isList; /* True if the property is a list can can return multiple values */
	const fiftyoneDegreesBool showValues; /* True if the values should be shown in GUIs */
	const fiftyoneDegreesBool isObsolete; /* True if the property is obsolete and will be removed from future data sets */
	const fiftyoneDegreesBool show; /* True if the property should be shown in GUIs */
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
	int16_t target;
	const fiftyoneDegreesNode *node;
	const fiftyoneDegreesNodeNumericIndex *firstNodeNumericIndex;
	const fiftyoneDegreesRANGE *range;
	int32_t startIndex;
	int32_t lowIndex;
	int32_t highIndex;
	fiftyoneDegreesBool lowInRange;
	fiftyoneDegreesBool highInRange;
} fiftyoneDegreesNumericNodeState;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_dataset_header_t {
	const int32_t versionMajor;
	const int32_t versionMinor;
	const int32_t versionBuild;
	const int32_t versionRevision;
	const byte tag[16];
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
	const fiftyoneDegreesEntityHeader strings;
	const fiftyoneDegreesEntityHeader components;
	const fiftyoneDegreesEntityHeader maps;
	const fiftyoneDegreesEntityHeader properties;
	const fiftyoneDegreesEntityHeader values;
	const fiftyoneDegreesEntityHeader profiles;
	const fiftyoneDegreesEntityHeader signatures;
	const fiftyoneDegreesEntityHeader rankedSignatureIndexes;
	const fiftyoneDegreesEntityHeader nodes;
	const fiftyoneDegreesEntityHeader rootNodes;
	const fiftyoneDegreesEntityHeader profileOffsets;
} fiftyoneDegreesDataSetHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_dataset_t {
	const fiftyoneDegreesDataSetHeader header;
	int32_t sizeOfSignature; /* The length in bytes of each signature record */
	int32_t signatureStartOfNodes; /* The number of bytes to ignore before the nodes start */
	int32_t signatureStartOfRank; /* The number of bytes to ignore before the signature rank is found */
	const fiftyoneDegreesProperty **requiredProperties; /* Pointer to properties to be returned */
	int32_t requiredPropertyCount; /* Number of properties to return */
	const byte *strings;
	const fiftyoneDegreesComponent *components;
	const fiftyoneDegreesMap *maps;
	const fiftyoneDegreesProperty *properties;
	const fiftyoneDegreesValue *values;
	const byte *profiles;
	const byte *signatures;
	const int32_t *rankedSignatureIndexes;
	const byte *nodes;
	const fiftyoneDegreesNode **rootNodes;
	const fiftyoneDegreesProfileOffset *profileOffsets;
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
	fiftyoneDegreesBool hashCodeSet; /* 0 if the hash code has not been calculated */
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
	FIFTYONEDEGREES_MUTEX lock; /* Used to lock access to the cache list */
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
};
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fiftyoneDegrees_workset_t {
	const fiftyoneDegreesDataSet *dataSet; /* A pointer to the data set to use for the match */
	byte *targetUserAgentArray; /* An array of bytes representing the target user agent */
	uint16_t targetUserAgentArrayLength; /* The length of the target user agent */
	uint64_t targetUserAgentHashCode; /* The hash code of the target user agent */
	fiftyoneDegreesBool hashCodeSet; /* 0 if the hash code has not been calculated */
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
	const fiftyoneDegreesValue **values; /* Pointers to values associated with the property requested */
	int32_t valuesCount; /* Number of values available */
	char *input; /* An input buffer large enough to store the useragent to be matched */
	char *targetUserAgent; /* A pointer to the user agent string */
	char *relevantNodes; /* Pointer to a char array containing the relevant nodes */
	char *closestNodes; /* Pointer to a char array containing the closest nodes */
	byte *signature; /* The signature found if only one exists */
	char *signatureAsString; /* The signature as a string */
	const fiftyoneDegreesNode **nodes; /* Pointer to a list of nodes related to the match */
	const fiftyoneDegreesNode **orderedNodes; /* Pointer to a list of nodes in ascending order of signature count */
	int32_t nodeCount; /* The number of nodes referenced by **nodes */
	int32_t closestNodeRankedSignatureIndex; /* If a single node is returned the index of the ranked signature to be processed */
	fiftyoneDegreesLinkedSignatureList linkedSignatureList; /* Linked list of signatures used by Closest match */
	int16_t nextCharacterPositionIndex;
	fiftyoneDegreesBool startWithInitialScore; /* True if the NEAREST and CLOSEST methods should start with an initial score */
	int(*functionPtrGetScore)(struct fiftyoneDegrees_workset_t *ws, const fiftyoneDegreesNode *node); /* Returns scores for each different node between signature and match */
	const byte* (*functionPtrNextClosestSignature)(struct fiftyoneDegrees_workset_t *ws); /* Returns the next closest signature */
	const fiftyoneDegreesResultsetCache *cache; /* Pointer to the cache, or NULL if not available. */
} fiftyoneDegreesWorkset;
#pragma pack(pop)

/* External methods */
EXTERNAL fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyArray(const char *fileName, fiftyoneDegreesDataSet *dataSet, char** properties, int32_t count);
EXTERNAL fiftyoneDegreesDataSetInitStatus fiftyoneDegreesInitWithPropertyString(const char *fileName, fiftyoneDegreesDataSet *dataSet, char* properties);
EXTERNAL void fiftyoneDegreesDestroy(const fiftyoneDegreesDataSet *dataSet);

EXTERNAL fiftyoneDegreesResultsetCache *fiftyoneDegreesResultsetCacheCreate(const fiftyoneDegreesDataSet *dataSet, int32_t size);
EXTERNAL void fiftyoneDegreesResultsetCacheFree(const fiftyoneDegreesResultsetCache *rsc);

EXTERNAL fiftyoneDegreesWorkset* fiftyoneDegreesCreateWorkset(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesResultsetCache *cache);
EXTERNAL void fiftyoneDegreesFreeWorkset(const fiftyoneDegreesWorkset *ws);

EXTERNAL fiftyoneDegreesResultset* fiftyoneDegreesMatch(fiftyoneDegreesWorkset *ws, char* userAgent);
EXTERNAL int32_t fiftyoneDegreesSetValues(fiftyoneDegreesWorkset *ws, int32_t requiredPropertyIndex);
EXTERNAL const fiftyoneDegreesAsciiString* fiftyoneDegreesGetString(const fiftyoneDegreesDataSet *dataSet, int32_t offset);
EXTERNAL const char* fiftyoneDegreesGetValueName(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesValue *value);
EXTERNAL const char* fiftyoneDegreesGetPropertyName(const fiftyoneDegreesDataSet *dataSet, const fiftyoneDegreesProperty *property);
EXTERNAL int32_t fiftyoneDegreesProcessDeviceCSV(fiftyoneDegreesWorkset *ws, char* result, int32_t resultLength);
EXTERNAL int32_t fiftyoneDegreesProcessDeviceJSON(fiftyoneDegreesWorkset *ws, char* result, int32_t resultLength);
EXTERNAL int32_t fiftyoneDegreesGetSignatureRank(fiftyoneDegreesWorkset *ws);

#endif // 51DEGREES_H_INCLUDED
