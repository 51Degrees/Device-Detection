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

/* Used to represent bytes */
typedef unsigned char byte;

/* Used to represent boolean */
typedef unsigned char fod_bool;

/* Used to return the match method */
typedef enum {NONE, EXACT, NUMERIC, NEAREST, CLOSEST} matchMethod;

/* Used to provide the status of the data set initialisation */
typedef enum e_DataSetInitStatus {
    DATA_SET_INIT_STATUS_SUCCESS,
    DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY,
    DATA_SET_INIT_STATUS_CORRUPT_DATA,
    DATA_SET_INIT_STATUS_INCORRECT_VERSION,
    DATA_SET_INIT_STATUS_FILE_NOT_FOUND
} DataSetInitStatus;

typedef struct range_t {
    const int16_t lower;
    const int16_t upper;
} RANGE;


#pragma pack(push, 1)
typedef struct ascii_string_t {
    const int16_t length;
    const byte firstByte;
} AsciiString;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct component_t {
    const byte componentId; /* The unique Id of the component. */
    const int32_t nameOffset; /* Offset in the strings data structure to the name */
    const int32_t defaultProfileOffset; /* Offset in the profiles data structure to the default profile */
} Component;
#pragma pack(pop)

#pragma pack(push, 4)
typedef struct map_t {
    const int32_t nameOffset; /* Offset in the strings data structure to the name */
} Map;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct node_numeric_index_t {
    const int16_t value; /* The numeric value of the index */
    const int32_t relatedNodeOffset; /* The node offset which the numeric value relates to */
} NodeNumericIndex;
#pragma pack(pop)

#pragma pack(push, 4)
typedef union node_index_value_t {
    const byte characters[4]; /* If not a string the characters to be used */
    const int32_t integer; /* If a string the offset in the strings data structure of the characters */
} NodeIndexValue;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct node_index_t {
    const byte isString; /* True if the value is an offset in the ascii string collection */
    union {
        const byte characters[4];
        const int32_t integer;
    } value; /* The value of the index as either an integer or character array */
    const int32_t relatedNodeOffset; /* The node offset which the value relates to */
} NodeIndex;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct string_t {
    byte* value;
    int16_t length;
} String;
#pragma pack(pop)

typedef struct strings_t {
    const String *firstString;
    const int32_t count;
} Strings;

#pragma pack(push, 2)
typedef struct node_t {
    const int16_t position;
    const int16_t nextCharacterPosition;
    const int32_t parentOffset;
    const int32_t characterStringOffset;
    const int16_t childrenCount;
    const int16_t numericChildrenCount;
    const int32_t signatureCount;
} Node;
#pragma pack(pop)

#pragma pack(push, 4)
typedef struct profile_offset_t {
    const int32_t profileId; /* The unique Id of the profile */
    const int32_t offset; /* Offset to the profile in the profiles structure */
} ProfileOffset;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct property_t {
    const byte componentIndex; /* Index of the component */
    const byte displayOrder; /* The order the property should be displayed in relative to other properties */
    const fod_bool isMandatory; /* True if the property is mandatory and must be provided */
    const fod_bool isList; /* True if the property is a list can can return multiple values */
    const fod_bool showValues; /* True if the values should be shown in GUIs */
    const fod_bool isObsolete; /* True if the property is obsolete and will be removed from future data sets */
    const fod_bool show; /* True if the property should be shown in GUIs */
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
} Property;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct value_t {
    const int16_t propertyIndex; /* Index of the property the value relates to */
    const int32_t nameOffset; /* The offset in the strings structure to the value name */
    const int32_t descriptionOffset; /* The offset in the strings structure to the value description */
    const int32_t urlOffset; /* The offset in the strings structure to the value url */
} Value;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct profile_t {
    const byte componentIndex;
    const int32_t profileId;
    const int32_t valueCount;
    const int32_t signatureCount;
} Profile;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct date_t {
    const int16_t year;
    const byte month;
    const byte day;
} Date;
#pragma pack(pop)

#pragma pack(push, 4)
typedef struct entity_header_t {
    const int32_t startPosition; /* Start position in the data file of the entities */
    const int32_t length; /* Length in bytes of the entities */
    const int32_t count; /* Number of entities in the collection */
} EntityHeader;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct numeric_node_state {
    int16_t target;
    const Node *node;
    const NodeNumericIndex *firstNodeNumericIndex;
    const RANGE *range;
    int32_t startIndex;
    int32_t lowIndex;
    int32_t highIndex;
    fod_bool lowInRange;
    fod_bool highInRange;
} NumericNodeState;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct dataset_header_t {
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
    const Date published;
    const Date nextUpdate;
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
    const EntityHeader strings;
    const EntityHeader components;
    const EntityHeader maps;
    const EntityHeader properties;
    const EntityHeader values;
    const EntityHeader profiles;
    const EntityHeader signatures;
    const EntityHeader rankedSignatureIndexes;
    const EntityHeader nodes;
    const EntityHeader rootNodes;
    const EntityHeader profileOffsets;
} DataSetHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct dataset_t {
    const DataSetHeader header;
    int32_t sizeOfSignature; /* The length in bytes of each signature record */
    int32_t signatureStartOfNodes; /* The number of bytes to ignore before the nodes start */
	const Property **requiredProperties; /* Pointer to properties to be returned */
	int32_t requiredPropertyCount; /* Number of properties to return */
    const byte *strings;
    const Component *components;
    const Map *maps;
    const Property *properties;
    const Value *values;
    const byte *profiles;
    const byte *signatures;
    const int32_t *rankedSignatureIndexes;
    const byte *nodes;
    const Node **rootNodes;
    const ProfileOffset *profileOffsets;
} DataSet;
#pragma pack(pop)

typedef struct linked_signature_list_item_t {
    struct linked_signature_list_item_t *next;
    struct linked_signature_list_item_t *previous;
    int32_t rankedSignatureIndex;
    int32_t frequency;
} LinkedSignatureListItem;

typedef struct linked_signature_list_t {
	const LinkedSignatureListItem *items; /* List of signatures that are being evaluated by Closest match */
	LinkedSignatureListItem *first; /* Pointer to the first signature in the linked list */
	LinkedSignatureListItem *last; /* Pointer to the last signature in the linked list */
	int32_t count; /* The number of signatures pointed to by signatures */
	LinkedSignatureListItem *current; /* Pointer to the current item in the list when navigating */
} LinkedSignatureList;

#pragma pack(push, 1)
typedef struct workset_t {
    const DataSet *dataSet; /* A pointer to the data set to use for the match */
    char *input; /* An input buffer large enough to store the useragent to be matched */
	char *targetUserAgent; /* A pointer to the user agent string */
	byte *targetUserAgentArray; /* An array of bytes representing the target user agent */
	int32_t targetUserAgentArrayLength; /* The length of the target user agent */
	char* relevantNodes; /* Pointer to a char array containing the relevant nodes */
	char* closestNodes; /* Pointer to a char array containing the closest nodes */
	const Profile **profiles; /* Pointer to a list of profiles returned for the match */
	int32_t profileCount; /* The number of profiles the match contains */
	const Node **nodes; /* Pointer to a list of nodes related to the match */
	const Node **orderedNodes; /* Pointer to a list of nodes in ascending order of signature count */
	int32_t nodeCount; /* The number of nodes referenced by **nodes */
    int32_t closestNodeRankedSignatureIndex; /* If a single node is returned the index of the ranked signature to be processed */
	LinkedSignatureList linkedSignatureList; /* Linked list of signatures used by Closest match */
	byte *signature; /* The signature found if only one exists */
	char *signatureAsString; /* The signature as a string */
	int16_t nextCharacterPositionIndex;
	matchMethod method; /* The method used to provide the match result */
	int32_t difference; /* The difference score between the signature found and the target */
	int32_t rootNodesEvaluated; /* The number of root nodes evaluated */
	int32_t stringsRead; /* The number of strings read */
	int32_t nodesEvaluated; /* The number of nodes read during the detection */
	int32_t signaturesCompared; /* The number of signatures read in full and compared to the target */
	int32_t signaturesRead; /* The number of signatures read in full */
	int32_t closestSignatures; /* The total number of closest signatures available */
	const Value **values; /* Pointers to values associated with the property requested */
	int32_t valuesCount; /* Number of values available */
	fod_bool startWithInitialScore; /* True if the NEAREST and CLOSEST methods should start with an initial score */
	int (*functionPtrGetScore)(struct workset_t *ws, const Node *node); /* Returns scores for each different node between signature and match */
	const byte* (*functionPtrNextClosestSignature)(struct workset_t *ws); /* Returns the next closest signature */
} Workset;
#pragma pack(pop)

/* External methods */
EXTERNAL DataSetInitStatus initWithPropertyArray(const char *fileName, DataSet *dataSet, char** properties, int32_t count);
EXTERNAL DataSetInitStatus initWithPropertyString(const char *fileName, DataSet *dataSet, char* properties);
EXTERNAL void destroy(const DataSet *dataSet);

EXTERNAL Workset* createWorkset(const DataSet *dataSet);
EXTERNAL void freeWorkset(const Workset *ws);
EXTERNAL void match(Workset *ws, char* userAgent);
EXTERNAL int32_t setValues(Workset *ws, int32_t requiredPropertyIndex);
EXTERNAL const AsciiString* getString(const DataSet *dataSet, int32_t offset);
EXTERNAL const char* getValueName(const DataSet *dataSet, const Value *value);
EXTERNAL const char* getPropertyName(const DataSet *dataSet, const Property *property);
EXTERNAL int32_t processDeviceCSV(Workset *ws, char* result, int32_t resultLength);
EXTERNAL int32_t processDeviceJSON(Workset *ws, char* result, int32_t resultLength);

#endif // 51DEGREES_H_INCLUDED
