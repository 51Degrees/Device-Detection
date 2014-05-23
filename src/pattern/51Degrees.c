#include <stdlib.h>
#include <stdio.h>
#include "../snprintf/snprintf.h"
#include <string.h>
#include <limits.h>
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

/**
 * DATA STRUCTURES USED ONLY BY FUNCTIONS IN THIS FILE
 */

/* Ranges used when performing a numeric match */
const RANGE RANGES[] = {
    { 0, 10 },
    { 10, 100 },
    { 100, 1000 },
    { 1000, 10000 },
    { 10000, SHRT_MAX }
};

#define RANGES_COUNT sizeof(RANGES) / sizeof(RANGE)

const int16_t POWERS[] = { 1, 10, 100, 1000, 10000 };

#define POWERS_COUNT sizeof(POWERS) / sizeof(int32_t)

/**
 * DATA FILE READ METHODS
 */

void readDate(FILE *inputFilePtr, const Date *date) {
    fread((void*)&(date->year), sizeof(const int16_t), 1, inputFilePtr);
    fread((void*)&(date->month), sizeof(const byte), 1, inputFilePtr);
    fread((void*)&(date->day), sizeof(const byte), 1, inputFilePtr);
}

void readEntitiesHeader(FILE *inputFilePtr, const EntityHeader *enitiesHeader) {
    fread((void*)&(enitiesHeader->startPosition), sizeof(int32_t), 1, inputFilePtr);
    fread((void*)&(enitiesHeader->length), sizeof(int32_t), 1, inputFilePtr);
    fread((void*)&(enitiesHeader->count), sizeof(int32_t), 1, inputFilePtr);
}

void readStrings(FILE *inputFilePtr, DataSet *dataSet) {
    dataSet->strings = (const byte*)malloc(dataSet->header.strings.length + 1);
    fread((void*)(dataSet->strings), dataSet->header.strings.length, 1, inputFilePtr);
}

void readComponents(FILE *inputFilePtr, DataSet *dataSet) {
    dataSet->components = (const Component*)malloc(dataSet->header.components.length);
    fread((void*)(dataSet->components), dataSet->header.components.length, 1, inputFilePtr);
}

void readMaps(FILE *inputFilePtr, DataSet *dataSet) {
    dataSet->maps = (const Map*)malloc(dataSet->header.maps.length);
    fread((void*)(dataSet->maps), dataSet->header.maps.length, 1, inputFilePtr);
}

void readProperties(FILE *inputFilePtr, DataSet *dataSet) {
    dataSet->properties = (const Property*)malloc(dataSet->header.properties.length);
    fread((void*)(dataSet->properties), dataSet->header.properties.length, 1, inputFilePtr);
}

void readValues(FILE *inputFilePtr, DataSet *dataSet) {
    dataSet->values = (const Value*)malloc(dataSet->header.values.length);
    fread((void*)(dataSet->values), dataSet->header.values.length, 1, inputFilePtr);
}

void readProfiles(FILE *inputFilePtr, DataSet *dataSet) {
    dataSet->profiles = (const byte*)malloc(dataSet->header.profiles.length);
    fread((void*)(dataSet->profiles), dataSet->header.profiles.length, 1, inputFilePtr);
}

void readSignatures(FILE *inputFilePtr, DataSet *dataSet) {
    dataSet->signatures = (const byte*)malloc(dataSet->header.signatures.length);
    fread((void*)(dataSet->signatures), dataSet->header.signatures.length, 1, inputFilePtr);
}

void readRankedSignatureIndexes(FILE *inputFilePtr, DataSet *dataSet) {
    dataSet->rankedSignatureIndexes = (const int32_t*)malloc(dataSet->header.rankedSignatureIndexes.length);
    fread((void*)(dataSet->rankedSignatureIndexes), dataSet->header.rankedSignatureIndexes.length, 1, inputFilePtr);
}

void readNodes(FILE *inputFilePtr, DataSet *dataSet) {
    dataSet->nodes = (const byte*)malloc(dataSet->header.nodes.length);
    fread((void*)(dataSet->nodes), dataSet->header.nodes.length, 1, inputFilePtr);
}

void readRootNodes(FILE *inputFilePtr, DataSet *dataSet) {
    int32_t index;
    int32_t* rootNodeOffsets;
    rootNodeOffsets = (int32_t*)malloc(dataSet->header.rootNodes.length);
    dataSet->rootNodes = (const Node**)malloc(dataSet->header.rootNodes.count * sizeof(Node*));
    fread((void*)rootNodeOffsets, dataSet->header.rootNodes.length, 1, inputFilePtr);
    for(index = 0; index < dataSet->header.rootNodes.count; index++) {
        *(dataSet->rootNodes + index) = (Node*)(dataSet->nodes + *(rootNodeOffsets + index));
    }
    free(rootNodeOffsets);
}

void readProfileOffsets(FILE *inputFilePtr, DataSet *dataSet) {
    dataSet->profileOffsets = (const ProfileOffset*)malloc(dataSet->header.profileOffsets.length);
    fread((void*)(dataSet->profileOffsets), dataSet->header.profileOffsets.length, 1, inputFilePtr);
}

int32_t readDataSet(FILE *inputFilePtr, DataSet *dataSet) {

    /* Read the data set header */
    fread((void*)&(dataSet->header), sizeof(DataSetHeader), 1, inputFilePtr);

    /* Check the version of the data file */
    if (dataSet->header.versionMajor != 3 ||
        dataSet->header.versionMinor != 1)
        return 0;

    /* Read the entity lists */
    readStrings(inputFilePtr, dataSet);
    readComponents(inputFilePtr, dataSet);
    readMaps(inputFilePtr, dataSet);
    readProperties(inputFilePtr, dataSet);
    readValues(inputFilePtr, dataSet);
    readProfiles(inputFilePtr, dataSet);
    readSignatures(inputFilePtr, dataSet);
    readRankedSignatureIndexes(inputFilePtr, dataSet);
    readNodes(inputFilePtr, dataSet);
    readRootNodes(inputFilePtr, dataSet);
    readProfileOffsets(inputFilePtr, dataSet);

    /* Set some of the constant fields */
    ((DataSet*)dataSet)->sizeOfSignature =
        ((dataSet->header.signatureNodesCount + dataSet->header.signatureProfilesCount) * sizeof(int32_t));
    ((DataSet*)dataSet)->signatureStartOfNodes =
        (dataSet->header.signatureProfilesCount * sizeof(int32_t));

    return 1;
}

/**
 * METHODS TO RETURN ELEMENTS OF THE DATA SET
 */

/**
 * Returns a component pointer from the index provided
 * @param dataSet pointer to the data set
 * @return pointer to the component
 */
const Component* getComponent(DataSet *dataSet, int32_t componentIndex) {
    return dataSet->components + componentIndex;
}

/**
 * Returns a pointer to the ascii string at the byte offset provided
 * @param dataSet pointer to the data set
 * @param offset to the ascii string required
 * @return a pointer to the AsciiString at the offset
 */
const AsciiString* getString(const DataSet *dataSet, int32_t offset) {
    return (const AsciiString*)(dataSet->strings + offset);
}

/**
 * Returns a pointer to the profile at the index provided
 * @param dataSet pointer to the data set
 * @param index of the profile required
 * @return pointer to the profile at the index
 */
Profile* getProfileByIndex(DataSet *dataSet, int32_t index) {
    return (Profile*)dataSet->profiles + (dataSet->profileOffsets + index)->offset;
}

/**
 * Gets the index of the property provided from the dataset
 * @param dataSet pointer to the data set containing the property
 * @param property pointer to the property
 * @return the index of the property
 */
int32_t getPropertyIndex(const DataSet *dataSet, const Property *property) {
    return property - dataSet->properties;
}

/**
 * Returns the property associated with the name.
 * @param dataSet pointer containing the property required
 * @param name string of the property required
 * @return pointer to the property, or NULL if not found.
 */
const Property* getPropertyByName(DataSet *dataSet, char* name) {
    int32_t index;
    const Property *property;
    for(index = 0; index < dataSet->header.properties.count; index++) {
        property = dataSet->properties + index;
        if (strcmp(getPropertyName(dataSet, property),
                   name) == 0)
            return property;
    }
    return NULL;
}

/**
 * Returns the name of the value provided.
 * @param dataSet pointer to the data set containing the value
 * @param value pointer whose name is required
 * @return pointer to the char string of the name
 */
const char* getValueName(const DataSet *dataSet, const Value *value) {
    return (char*)(&getString(dataSet, value->nameOffset)->firstByte);
}

/**
 * Returns the name of the property provided.
 * @param dataSet pointer to the data set containing the property
 * @param property pointer whose name is required
 * @return pointer to the char string of the name
 */
const char* getPropertyName(const DataSet *dataSet, const Property *property) {
    return (const char*)&(getString(dataSet, property->nameOffset)->firstByte);
}

/**
 * Returns the first numeric index for the node provided.
 * @param node pointer to the node whose numeric indexes are required
 * @return pointer to the first numeric index for the node
 */
const NodeNumericIndex* getFirstNumericIndexForNode(const Node *node) {
    return (const NodeNumericIndex*)(((byte*)node) + (sizeof(Node) + (node->childrenCount * sizeof(NodeIndex))));
}

/**
 * Returns the node associated with the node index
 * @param dataSet pointer to the data set
 * @param nodeIndex pointer associated with the node required
 */
const Node* getNodeFromNodeIndex(const DataSet *dataSet, const NodeIndex *nodeIndex) {
    return (const Node*)(dataSet->nodes + nodeIndex->relatedNodeOffset);
}

/**
 * Returns a pointer to the first node index associated with the node provided
 * @param node whose first node index is needed
 * @return the first node index of the node
 */
const NodeIndex* getNodeIndexesForNode(const Node* node) {
    return (NodeIndex*)(((byte*)node) + sizeof(Node));
}

/**
 * Returns true if the node is a complete one
 * @param node pointer to be checked
 * @return true if the node is complete, otherwise false
 */
fod_bool getIsNodeComplete(const Node* node) {
    return node->nextCharacterPosition != SHRT_MIN;
}

/**
 * Returns the node pointer at the offset provided.
 * @param dataSet pointer to the data set
 * @param offset to the node required
 * @return pointer to the node at the offset
 */
const Node* getNodeByOffset(const DataSet *dataSet, int32_t offset) {
    return (const Node*)(dataSet->nodes + offset);
}

/**
 * Returns the root node associated with the node provided
 * @param dataSet pointer to the data set
 * @param node pointer whose root node is required
 * @return node pointer to the root node
 */
const Node* getRootNode(const DataSet *dataSet, const Node *node) {
    if (node->parentOffset >= 0) {
        return getRootNode(dataSet, getNodeByOffset(dataSet, node->parentOffset));
    }
    return node;
}

/**
 * Returns the length of the signature based on the node offsets
 * associated with the signature.
 * @param dataSet pointer to the data set
 * @param nodeOffsets pointer to the first node offset for the signature
 * @return the number of characters the signature contains
 */
int32_t getSignatureLengthFromNodeOffsets(const DataSet *dataSet, int32_t nodeOffset) {
    const Node *node = getNodeByOffset(dataSet, nodeOffset);
    return getRootNode(dataSet, node)->position + 1;
}

/**
 * Returns the characters associated with the node by looking them up in the
 * strings table.
 * @param dataSet pointer to the data set
 * @param node pointer for the node whose characters are required
 * @return pointer to the ascii string associated with the node
 */
const AsciiString* getNodeCharacters(const DataSet *dataSet, const Node *node) {
    return getString(dataSet, node->characterStringOffset);
}

/**
 * Returns the characters associated with a node index. This is either
 * performed using the strings table, or if short by converting the value
 * of the node index to a character array. The results are returned in the
 * string structure passed into the method.
 * @param ws pointer to the workset being used for matching
 * @param nodeIndex pointer of the node index being tested
 * @param string pointer to return the string
 */
void getCharactersForNodeIndex(Workset *ws, const NodeIndex *nodeIndex, String *string) {
    int16_t index;
    const AsciiString *asciiString;
    if (nodeIndex->isString != 0) {

        asciiString = getString(ws->dataSet, nodeIndex->value.integer);
        /* Set the length of the byte array removing the null terminator */
        string->length = (int16_t)(asciiString->length - 1);
        string->value = (byte*)&(asciiString->firstByte);
    }
    else {
        for(index = 0; index < 4; index++) {
            if (nodeIndex->value.characters[index] == 0)
                break;
        }
        string->length = index;
        string->value = (byte*)&(nodeIndex->value.characters);
    }
}

/**
 * Returns the signature at the index provided.
 * @param dataSet pointer to the data set
 * @param index of the signature required
 * @return pointer to the signature at the index
 */
const byte* getSignatureByIndex(const DataSet *dataSet, int32_t index) {
    return dataSet->signatures + (dataSet->sizeOfSignature * index);
}

/**
 * Returns the signature at the ranked index provided.
 * @param dataSet pointer to the data set
 * @param ranked index of the signature required
 * @return pointer to the signature at the ranked index
 */
const byte* getSignatureByRankedIndex(const DataSet *dataSet, int32_t index) {
    return getSignatureByIndex(dataSet, dataSet->rankedSignatureIndexes[index]);
}

/**
 * Returns the number of node offsets associated with the signature.
 * @param dataSet pointer to the data set
 * @param nodeOffsets pointer to the node offsets associated with the signature
 * @return the number of nodes associated with the signature
 */
int32_t getSignatureNodeOffsetsCount(const DataSet *dataSet, int32_t *nodeOffsets) {
    int32_t count = 0;
    while (*(nodeOffsets + count) >= 0 &&
           count < dataSet->header.signatureNodesCount)
        count++;
    return count;
}

/**
 * Returns the offset associated with the node pointer provided.
 * @param dataSet pointer to the data set
 * @return the integer offset to the node in the data structure
 */
int32_t getNodeOffsetFromNode(const DataSet *dataSet, const Node *node) {
    return (byte*)node - (byte*)(dataSet->nodes);
}

/**
 * Returns an integer pointer to the node offsets associated with the
 * signature.
 * @param dataSet pointer to the data set
 * @param signature pointer to the signature whose node offsets are required
 * @return pointer to the first integer in the node offsets associated with
 *         the signature
 */
int32_t* getNodeOffsetsFromSignature(const DataSet *dataSet, const byte *signature) {
    return (int32_t*)(signature + dataSet->signatureStartOfNodes);
}

/**
 * Returns an integer pointer to the profile offsets associated with the
 * signature.
 * @param signature pointer to the signature whose profile offsets are required
 * @return pointer to the first integer in the profile offsets associated with
 *         the signature
 */
int32_t* getProfileOffsetsFromSignature(const byte *signature) {
    return (int32_t*)signature;
}

/**
 * Returns a pointer to the first signature index of the node
 * @param node pointer whose first signature index is required
 * @return a pointer to the first signature index
 */
int32_t* getFirstRankedSignatureIndexForNode(const Node *node) {
    return (int32_t*)(((byte*)node) + sizeof(Node) +
           (node->childrenCount * sizeof(NodeIndex)) +
           (node->numericChildrenCount * sizeof(NodeNumericIndex)));
}

/**
 * Returns a pointer to the first signature index of the node
 * @param node pointer whose first signature index is required
 * @return a pointer to the first signature index
 */
int32_t* getFirstSignatureIndexForNode(const Node *node) {
    return (int32_t*)(((byte*)node) + sizeof(Node) +
           (node->childrenCount * sizeof(NodeIndex)) +
           (node->numericChildrenCount * sizeof(NodeNumericIndex)));
}

/**
 * LINKED LIST METHODS
 */

/**
 * Adds the signature index to the linked list with a frequency of 1.
 * @param linkedList pointer to the linked list
 * @param signatureIndex to be added to the end of the list
 */
void linkedListAdd(LinkedSignatureList *linkedList, int32_t rankedSignatureIndex) {
    LinkedSignatureListItem *newSignature = (LinkedSignatureListItem*)(linkedList->items) + linkedList->count;
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
 * Builds the initial linked list using a single node.
 * @param ws pointer to the workset used for the match
 * @param node pointer to the node whose signature indexes will be used to
 *        build the initial linked list.
 */
void buildInitialList(Workset *ws, const Node *node) {
    int32_t index;
    int32_t *firstSignatureIndex = getFirstRankedSignatureIndexForNode(node);
    for (index = 0; index < node->signatureCount; index++) {
        linkedListAdd(&(ws->linkedSignatureList), *(firstSignatureIndex + index));
    }
}

/**
 * Adds the signature index before the item provided.
 * @param linkedList pointer to the linked list to be altered
 * @param item that the signature index should be added before
 */
void linkedListAddBefore(LinkedSignatureList *linkedList, LinkedSignatureListItem *item, int32_t rankedSignatureIndex) {
    LinkedSignatureListItem *newSignature = (LinkedSignatureListItem*)(linkedList->items + linkedList->count);
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
 * Removes the item specified from the linked list.
 * @param linkedList pointer to the linked list to be altered
 * @param item to be removed from the list
 */
void linkedListRemove(LinkedSignatureList *linkedList, LinkedSignatureListItem *item) {
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
 * DATASET SETUP
 */

/**
 * Destroys the data set releasing all memory available.
 * @param dataSet pointer to the data set being destroyed
 */
void destroy(const DataSet *dataSet) {
    free((void*)(dataSet->requiredProperties));
    free((void*)(dataSet->strings));
    free((void*)(dataSet->components));
    free((void*)(dataSet->maps));
    free((void*)(dataSet->properties));
    free((void*)(dataSet->values));
    free((void*)(dataSet->profiles));
    free((void*)(dataSet->signatures));
    free((void*)(dataSet->nodes));
    free((void*)(dataSet->rootNodes));
    free((void*)(dataSet->profileOffsets));
}

/**
 * Adds all properties in the data set to the required properties list.
 * @param dataSet pointer to the data set
 */
void setAllProperties(DataSet *dataSet) {
    int32_t index;
    dataSet->requiredPropertyCount = dataSet->header.properties.count;
    dataSet->requiredProperties = (const Property**)malloc(dataSet->requiredPropertyCount * sizeof(Property*));
    for(index = 0; index < dataSet->requiredPropertyCount; index++) {
        *(dataSet->requiredProperties + index) = dataSet->properties + index;
    }
}

/**
 * Adds the properties in the array of properties to the list
 * of required properties from a match.
 * @param dataSet pointer to the data set
 * @param properties array of properties to be returned
 * @param count number of elements in the properties array
 */
void setProperties(DataSet *dataSet, char** properties, int32_t count) {
    int32_t index, propertyIndex, requiredPropertyLength;
    char *requiredPropertyName;
    const AsciiString *propertyName;

    // Allocate memory for this number of properties.
    dataSet->requiredPropertyCount = 0;
    dataSet->requiredProperties = (const Property**)malloc(count * sizeof(const Property*));

    // Add the properties to the list of required properties.
    for(propertyIndex = 0; propertyIndex < count; propertyIndex++) {
        requiredPropertyName = *(properties + propertyIndex);
        requiredPropertyLength = strlen(requiredPropertyName);
        for(index = 0; index < dataSet->header.properties.count; index++) {
            propertyName = getString(dataSet, (dataSet->properties + index)->nameOffset);
            if (requiredPropertyLength == propertyName->length - 1 &&
                memcmp(requiredPropertyName, &propertyName->firstByte, requiredPropertyLength) == 0) {
                *(dataSet->requiredProperties + dataSet->requiredPropertyCount) = (dataSet->properties + index);
                dataSet->requiredPropertyCount++;
                break;
            }
        }
    }
}

/**
 * Gets the number of separators in the char array
 * @param input char array containing separated values
 * @return number of separators
 */
int32_t getSeparatorCount(char* input) {
    int32_t index = 0, count = 0;
    if (input != NULL) {
        while(*(input + index) != 0) {
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
int32_t initWithPropertyString(const char *fileName, DataSet *dataSet, char* requiredProperties) {
    int32_t requiredPropertyCount = getSeparatorCount(requiredProperties);
    int32_t index, count = 0;
    char **requiredPropertiesArray = NULL;
    char *last = requiredProperties;
    int32_t bytesRead = 0;

    // Determine if properties were provided.
    if (requiredPropertyCount > 0) {
        // Allocate pointers for each of the properties.
        requiredPropertiesArray = (char**)malloc(requiredPropertyCount * sizeof(char*));

        // Change the input string so that the separators are changed to nulls.
        for(index = 0; count < requiredPropertyCount; index++) {
            if (*(requiredProperties + index) == ',' ||
                *(requiredProperties + index) == '|' ||
                *(requiredProperties + index) == ' ' ||
                *(requiredProperties + index) == '\t' ||
                *(requiredProperties + index) == 0) {
                *(requiredProperties + index) = 0;
                *(requiredPropertiesArray + count) = last;
                last = requiredProperties + index + 1;
                count++;
            }
        }
    }

    bytesRead = initWithPropertyArray(fileName, dataSet, requiredPropertiesArray, requiredPropertyCount);

    if (requiredPropertiesArray != NULL)
        free(requiredPropertiesArray);

    return bytesRead;
}

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
int32_t initWithPropertyArray(const char *fileName, DataSet *dataSet, char** requiredProperties, int32_t count) {
	FILE *inputFilePtr = NULL;
	int32_t bytesRead = 0;

    // Open the file and hold on to the pointer.
    #ifndef _MSC_FULL_VER
	inputFilePtr = fopen(fileName, "rb");
	#else
	/* If using Microsoft use the fopen_s method to avoid warning */
    if (fopen_s(&inputFilePtr, fileName, "rb") != 0) {
        return -1;
    }
	#endif

	// If the file didn't open return -1.
	if (inputFilePtr == NULL)
		return -1;

    // Read the data set into memory.
    bytesRead = readDataSet(inputFilePtr, dataSet);

    // Set the properties that are returned by the data set.
    if (requiredProperties == NULL || count == 0) {
        setAllProperties(dataSet);
    } else {
        setProperties(dataSet, requiredProperties, count);
    }

    return bytesRead;
}

/**
 * WORKSET METHODS
 */

/**
 * Creates a new workset to perform matches using the dataset provided.
 * The workset must be destroyed using the freeWorkset method when it's
 * finished with to release memory.
 * @param dataSet pointer to the data set
 * @returns a pointer to the workset created
 */
Workset *createWorkset(const DataSet *dataSet) {
	Workset *ws = (Workset*)malloc(sizeof(Workset));
	ws->dataSet = dataSet;
	ws->linkedSignatureList.items = (LinkedSignatureListItem*)malloc(dataSet->header.maxSignaturesClosest * sizeof(LinkedSignatureListItem));
	ws->input = (char*)malloc((dataSet->header.maxUserAgentLength + 1) * sizeof(char));
	ws->signatureAsString = (char*)malloc((dataSet->header.maxUserAgentLength + 1) * sizeof(char));
    ws->profiles = (const Profile**)malloc(dataSet->header.components.count * sizeof(const Profile*));
    ws->nodes = (const Node**)malloc(dataSet->header.maxUserAgentLength * sizeof(const Node*));
    ws->orderedNodes = (const Node**)malloc(dataSet->header.maxUserAgentLength * sizeof(const Node*));
    ws->targetUserAgentArray = (byte*)malloc(dataSet->header.maxUserAgentLength);
    ws->relevantNodes = (char*)malloc(dataSet->header.maxUserAgentLength + 1);
    ws->closestNodes = (char*)malloc(dataSet->header.maxUserAgentLength + 1);
    ws->values = (const Value**)malloc(dataSet->header.maxValues * sizeof(const Value*));

    // Null terminate the strings used to return the relevant and closest nodes.
    ws->relevantNodes[dataSet->header.maxUserAgentLength] = 0;
    ws->closestNodes[dataSet->header.maxUserAgentLength] = 0;
	return ws;
}

/**
 * Releases the memory used by the workset.
 * @param pointer to the workset created previously
 */
void freeWorkset(const Workset *ws) {
    free((void*)(ws->input));
    free((void*)(ws->signatureAsString));
    free((void*)ws->profiles);
    free((void*)ws->nodes);
    free((void*)ws->orderedNodes);
	free((void*)ws->targetUserAgentArray);
	free((void*)ws->relevantNodes);
	free((void*)ws->closestNodes);
    free((void*)ws->values);
    free((void*)ws->linkedSignatureList.items);
	free((void*)ws);
}

/**
 * Adds the node of the signature into the signature string of the work set
 * @param ws pointer to the work set used for the match
 * @param node from the signature to be added to the string
 * @return the right most character returned
 */
int addSignatureNodeToString(Workset *ws, const Node *node) {
    int nodeIndex, signatureIndex;
    const AsciiString *characters = getString(ws->dataSet, node->characterStringOffset);
    for(nodeIndex = 0, signatureIndex = node->position + 1;
        nodeIndex < characters->length - 1;
        nodeIndex++, signatureIndex++) {
        ws->signatureAsString[signatureIndex] = (&(characters->firstByte))[nodeIndex];
    }
    return signatureIndex;
}

/**
 * Sets the signature provided as the string returned by the match.
 * @param ws pointer to the work set used for the match
 * @param signature pointer to be used as the string for the result
 */
void setSignatureAsString(Workset *ws, const byte *signature) {
    int *nodeOffsets = getNodeOffsetsFromSignature(ws->dataSet, signature);
    int index,
        nullPosition = 0,
        lastCharacter = 0,
        nodeOffsetCount = getSignatureNodeOffsetsCount(ws->dataSet, nodeOffsets);
    for(index = 0; index < ws->dataSet->header.maxUserAgentLength; index++) {
        ws->signatureAsString[index] = '_';
    }
    for(index = 0; index < nodeOffsetCount; index++) {
        lastCharacter = addSignatureNodeToString(ws, getNodeByOffset(ws->dataSet, *(nodeOffsets + index)));
        if (lastCharacter > nullPosition) {
            nullPosition = lastCharacter;
        }
    }
    ws->signatureAsString[nullPosition] = 0;
}

/**
 * Adds the node to the end of the workset.
 * @param ws pointer to the work set used for the match
 * @param node pointer to be added to the work set
 * @param index the should be added at
 */
void addNodeIntoWorkset(Workset *ws, const Node *node, int32_t index) {
    *(ws->nodes + index) = node;
    ws->nodeCount++;
}

/**
 * Inserts the node at the position provided moving all other nodes down.
 * @param ws pointer to the work set used for the match
 * @param node pointer to the node being added to the workset
 * @param insertIndex the index to insert the node at
 */
void insertNodeIntoWorksetAtIndex(Workset *ws, const Node *node, int32_t insertIndex) {
    int32_t index;
    for (index = ws->nodeCount - 1; index >= insertIndex; index--) {
        *(ws->nodes + index + 1) = *(ws->nodes + index);
    }
    addNodeIntoWorkset(ws, node, insertIndex);
}

/**
 * Inserts the node into the workset considering its position relative to other
 * nodes already set.
 * @param ws pointer to the work set used for the match
 * @param node pointer to be added to the work set
 * @return the index the node as added at
 */
int32_t insertNodeIntoWorkSet(Workset *ws, const Node *node) {
    int32_t index;
    for(index = 0; index < ws->nodeCount; index++) {
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
 * MATCH METHODS
 */

/**
 * Sets the target user agent and resets any other fields to initial values.
 * Must be called before a match commences.
 * @param ws pointer to the workset to be used for the match
 * @param userAgent char pointer to the user agent. Trimmed if longer than
 *        the maximum allowed for matching
 */
void setTargetUserAgentArray(Workset *ws, char* userAgent) {
    int32_t index;
    ws->targetUserAgent = userAgent;
    ws->targetUserAgentArrayLength = strlen(userAgent);

    if (ws->targetUserAgentArrayLength > ws->dataSet->header.maxUserAgentLength)
        ws->targetUserAgentArrayLength = ws->dataSet->header.maxUserAgentLength;

    /* Work out the starting character position */
    for(index = 0; index < ws->targetUserAgentArrayLength; index++) {
        ws->targetUserAgentArray[index]= userAgent[index];
    }
    ws->nextCharacterPositionIndex = (int16_t)(ws->targetUserAgentArrayLength - 1);

    /* Check to ensure the length of the user agent does not exceed
       the number of root nodes. */
    if (ws->nextCharacterPositionIndex >= ws->dataSet->header.rootNodes.count) {
        ws->nextCharacterPositionIndex = (int16_t)(ws->dataSet->header.rootNodes.count - 1);
    }

    /* Reset the nodes to zero ready for the new match */
    ws->nodeCount = 0;
    for(index = 0; index < ws->dataSet->header.signatureNodesCount; index++) {
        *(ws->nodes + index) = NULL;
        *(ws->orderedNodes + index) = NULL;
    }

    /* Reset the profiles to space ready for the new match */
    ws->profileCount = 0;
    for(index = 0; index < ws->dataSet->header.components.count; index++) {
        *(ws->profiles + index) = NULL;
    }

    /* Reset the closest and relevant strings */
    for(index = 0; index < ws->dataSet->header.maxUserAgentLength; index++) {
        ws->relevantNodes[index] = '_';
        ws->closestNodes[index] = ' ';
    }

    /* Reset the linked lists */
    ws->linkedSignatureList.count = 0;
    ws->linkedSignatureList.first = NULL;
    ws->linkedSignatureList.last = NULL;

    /* Reset the counters */
    ws->difference = 0;
    ws->stringsRead = 0;
    ws->nodesEvaluated = 0;
    ws->rootNodesEvaluated = 0;
    ws->signaturesCompared = 0;
    ws->signaturesRead = 0;

    /* Reset the profiles and signatures */
    ws->profileCount = 0;
    ws->signature = NULL;
}

/**
 * Compares the characters which start at the start index of the target user
 * agent with the string provided and returns 0 if they're equal, -1 if
 * lower or 1 if higher.
 * @param targetUserAgentArray pointer to the target user agent
 * @param startIndex character position in the array to start comparing
 * @param string pointer to be compared with the target user agent
 * @return the difference between the characters, or 0 if equal
 */
int32_t compareTo(byte* targetUserAgentArray, int32_t startIndex, String *string) {
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
 * Returns the next node after the one provided for the target user agent.
 * @param ws pointer being used for the match
 * @param node pointer of the current node
 * @return pointer to the next node to evaluate, or NULL if none match the
 *        target
 */
const Node* getNextNode(Workset *ws, const Node *node) {
    int32_t upper = node->childrenCount - 1, lower, middle, length, startIndex, comparisonResult;
    const NodeIndex *children;
    String string;

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
            if ((children + middle)->isString)
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
 * Returns a leaf node if one is available, otherwise NULL.
 * @param ws pointer the workset used for the match
 * @param node pointer to the node whose children should be checked
 * @return a pointer the complete leaf node if one is available
 */
const Node* getCompleteNode(Workset *ws, const Node *node) {
    const Node *nextNode = getNextNode(ws, node), *foundNode = NULL;
    if (nextNode != NULL) {
        foundNode = getCompleteNode(ws, nextNode);
    }
    if (foundNode == NULL && getIsNodeComplete(node))
        foundNode = node;
    return foundNode;
}

/**
 * Compares the signature to the nodes of the match in the workset and if
 * they match exactly returns 0. If not -1 or 1 are returned depending on
 * whether the signature is lower or higher in the list.
 * @param ws pointer to the workset used for the match
 * @param signature pointer to the signature being tested
 * @return the difference between the signature and the matched nodes
 */
int32_t compareExact(const byte *signature, Workset *ws) {
    int32_t index, nodeIndex, difference;
    int32_t *nodeOffsets = getNodeOffsetsFromSignature(ws->dataSet, signature);
    int32_t signatureNodeOffsetsCount = getSignatureNodeOffsetsCount(ws->dataSet, nodeOffsets);
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
 * Returns the index of the signature exactly associated with the matched
 * nodes or -1 if no such signature exists.
 * @param ws pointer to the workset used for the match
 * @return index of the signature matching the nodes or -1
 */
int32_t getExactSignatureIndex(Workset *ws) {
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
 * Sets the characters of the node at the correct position in the output
 * array provided.
 * @param dataSet pointer to the dataset
 * @param node pointer to the node being added to the output
 * @return the position of the right most character set
 */
int32_t setNodeString(const DataSet *dataSet, const Node *node, char* output) {
    int32_t n, c, last = 0;
    const AsciiString *string = getString(dataSet, node->characterStringOffset);
    for(n = 0, c = node->position + 1; n < string->length - 1; c++, n++) {
        *(output + c) = *(&string->firstByte + n);
        if (c > last)
            last = c;
    }
    return last;
}

/**
 * Sets the characters of the nodes in the worksets relevant nodes pointer.
 * @param ws pointer to the workset being used for the match
 */
void setRelevantNodes(Workset *ws) {
    int32_t index, lastPosition, last = -1;
    for(index = 0; index < ws->nodeCount; index++) {
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
 * Sets the characters of the signature returned from the match.
 * @param ws pointer to the workset being used for the match
 * @param signature pointer to the signature being set for the match
 */
void setMatchSignature(Workset *ws, const byte *signature) {
    int32_t index, lastPosition, last = 0, profileIndex;
    int32_t *signatureNodeOffsets = getNodeOffsetsFromSignature(ws->dataSet, signature);
    int32_t *signatureProfiles = getProfileOffsetsFromSignature(signature);

    ws->signature = (byte*)signature;
    ws->profileCount = 0;
    for(index = 0; index < ws->dataSet->header.signatureProfilesCount; index++) {
        profileIndex = *(signatureProfiles + index);
        if (profileIndex >= 0) {
            *(ws->profiles + index) = (Profile*)(ws->dataSet->profiles + profileIndex);
            ws->profileCount++;
        }
    }

    /* Set the closest nodes string from the signature found */
    for(index = 0;
        index < ws->dataSet->header.signatureNodesCount && *(signatureNodeOffsets + index) >= 0;
        index++) {
        lastPosition = setNodeString(ws->dataSet,
                       (Node*)(ws->dataSet->nodes + *(signatureNodeOffsets + index)),
                       ws->closestNodes);
        if (lastPosition > last)
            last = lastPosition;
    }
    *(ws->closestNodes + last + 1) = 0;
}

/**
 * Sets the signature returned for the match.
 * @param ws pointer to the work set
 * @param signatureIndex of the signature being set for the match result
 */
void setMatchSignatureIndex(Workset *ws, int32_t signatureIndex) {
    setMatchSignature(ws, getSignatureByIndex(ws->dataSet, signatureIndex));
}

/**
 * If a match can't be found this function sets the default profiles for each
 * component type.
 * @param ws pointer to the work set
 */
void setMatchDefault(Workset *ws) {
    int32_t index;
    int32_t profileOffset;

    ws->profileCount = 0;
    for (index = 0; index < ws->dataSet->header.components.count; index++) {
        profileOffset = (ws->dataSet->components + index)->defaultProfileOffset;
        *(ws->profiles + index) = (Profile*)(ws->dataSet->profiles + profileOffset);
        ws->profileCount++;
    }

    /* Default the other values as no data available */
    ws->signature = NULL;
    *ws->closestNodes = 0;
}

/**
 * Moves the next character position index to the right most character
 * position for evaluation.
 * @param ws pointer to the workset used for the match
 */
void resetNextCharacterPositionIndex(Workset *ws) {
    ws->nextCharacterPositionIndex =
        ws->targetUserAgentArrayLength < ws->dataSet->header.rootNodes.count ?
            (int16_t)ws->targetUserAgentArrayLength - 1 :
            (int16_t)ws->dataSet->header.rootNodes.count -1;
}

/**
 * Returns the integer number at the start index of the array provided. The
 * characters at that position must have already been checked to ensure
 * they're numeric.
 * @param array pointer to the start of a byte array of characters
 * @param start index of the first character to convert to a number
 * @param length of the characters from start to conver to a number
 * @return the numeric integer of the characters specified
 */
int16_t getNumber(const byte *array, int32_t start, int32_t length) {
    int32_t i, p;
    int16_t value = 0;
    for (i = start + length - 1, p = 0; i >= start && p < POWERS_COUNT; i--, p++)
    {
        value += POWERS[p] * (array[i] - (byte)'0');
    }
    return value;
}

/**
 * Sets the stats target field to the numeric value of the current position in
 * the target user agent. If no numeric value is available the field remains
 * unaltered.
 * @param ws pointer to the work set used for the match
 * @param node pointer to the node being evaluated
 * @param state of the numeric node evaluation
 * @return the number of bytes from the target user agent needed to form a number
 */
int32_t getCurrentPositionAsNumeric(Workset *ws, const Node *node, NumericNodeState *state) {
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
    } else {
        state->target = SHRT_MIN;
    }
    return node->position - index;
}

/**
 * Determines the integer ranges that can be compared to the target.
 * @param state whose range field needs to be set
 */
void setRange(NumericNodeState *state) {
    int32_t index;
    for(index = 0; index < RANGES_COUNT; index++) {
        if (state->target >= RANGES[index].lower &&
            state->target < RANGES[index].upper) {
            state->range = &RANGES[index];
        }
    }
}

/**
 * Searches the numeric children using a binary search for the states target
 * provided. If a match can't be found the position to insert the target is
 * returned.
 * @param node pointer whose numeric children are to searched
 * @param state pointer for the numeric evaluation
 * @return the index of the target or twos compliment of insertion point
 */
int32_t binarySearchNumericChildren(const Node *node, NumericNodeState *state) {
    int16_t lower = 0, upper = node->numericChildrenCount - 1, middle, comparisonResult;

    while (lower <= upper)
    {
        middle = lower + (upper - lower) / 2;
        comparisonResult = (state->firstNodeNumericIndex + middle)->value - state->target;;
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
 * Configures the state ready to evaluate the numeric nodes in order of
 * difference from the target.
 * @param node pointer whose numeric indexes are to be evaluated
 * @param data pointer to the state settings used for the evaluation
 */
void setNumericNodeState(const Node *node, NumericNodeState *state) {
    if (state->target >= 0 && state->target <= SHRT_MAX) {
        setRange(state);
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
            (state->firstNodeNumericIndex + state->lowIndex)->value >= state->range->lower &&
            (state->firstNodeNumericIndex + state->lowIndex)->value < state->range->upper;
    }
}

/**
 * The iterator function used to return the next numeric node index for
 * the evaluation for the state provided.
 * @param state settings for the evalution
 * @return NULL if no more indexes are available, or the next index
 */
const NodeNumericIndex* getNextNumericNode(NumericNodeState *state) {
    int32_t lowDifference, highDifference;
    const NodeNumericIndex *nodeNumericIndex = NULL;

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
 * Gets a complete numeric leaf node for the node provided if one exists.
 * @param ws pointer to the work set used for the match
 * @param node pointer to be evaluated against the current position
 * @return pointer to the complete node found, or NULL if no node exists
 */
const Node* getCompleteNumericNode(Workset *ws, const Node *node) {
    const Node *foundNode = NULL, *nextNode = getNextNode(ws, node);
    const NodeNumericIndex *nodeNumericIndex;
    int32_t difference;
    NumericNodeState state;

    // Check to see if there's a next node which matches
    // exactly.
    if (nextNode != NULL)
        foundNode = getCompleteNumericNode(ws, nextNode);

    if (foundNode == NULL && node->numericChildrenCount > 0)
    {
        // No. So try each of the numeric matches in ascending order of
        // difference.
        if (getCurrentPositionAsNumeric(ws, node, &state) > 0 &&
            state.target >= 0)
        {
            setNumericNodeState(node, &state);
            nodeNumericIndex = getNextNumericNode(&state);
            while (nodeNumericIndex != NULL)
            {
                foundNode = getCompleteNumericNode(ws,
                    getNodeByOffset(ws->dataSet, nodeNumericIndex->relatedNodeOffset));
                if (foundNode != NULL)
                {
                    difference = abs(state.target - nodeNumericIndex->value);
                    ws->difference += difference;
                    break;
                }
                nodeNumericIndex = getNextNumericNode(&state);
            }
        }
    }
    if (foundNode == NULL && getIsNodeComplete(node))
        foundNode = node;
    return foundNode;
}


/**
 * Passed two nodes and determines if their character positions overlap
 * @param a the first node to test
 * @param b the second node to test
 * @return true if they overlap, otherwise false
 */
fod_bool areNodesOverlapped(const DataSet *dataSet, const Node *a, const Node *b) {
    const Node *lower = a->position < b->position ? a : b,
         *higher = lower == b ? a : b,
         *rootNode = getRootNode(dataSet, lower);

    if (lower->position == higher->position ||
        rootNode->position > higher->position) {
        return 1;
    }

    return 0;
}

/**
 * Determines if the node overlaps any nodes already in the workset.
 * @param node pointer to be checked
 * @param ws pointer to the work set used for the match
 * @return 1 if the node overlaps, otherwise 0
 */
fod_bool isNodeOverlapped(const Node *node, Workset *ws) {
    const Node  *currentNode;
    int         index;
    for(index = ws->nodeCount - 1; index >= 0; index--) {
        currentNode = *(ws->nodes + index);
        if (currentNode == node ||
            areNodesOverlapped(ws->dataSet, node, currentNode) == 1) {
            return 1;
        }
    }
    return 0;
}

/**
 * Evaluates the target user agent for a numeric match.
 * @param ws pointer to the work set being used for the match
 */
void evaluateNumeric(Workset *ws) {
    int32_t existingNodeIndex = 0;
    const Node *node;
    resetNextCharacterPositionIndex(ws);
    ws->difference = 0;
    while (ws->nextCharacterPositionIndex > 0 &&
           ws->nodeCount < ws->dataSet->header.signatureNodesCount)
    {
        if (existingNodeIndex >= ws->nodeCount ||
            getRootNode(ws->dataSet, *(ws->nodes + existingNodeIndex))->position < ws->nextCharacterPositionIndex)
        {
            ws->rootNodesEvaluated++;
            node = getCompleteNumericNode(ws, *((ws->dataSet->rootNodes) + ws->nextCharacterPositionIndex));
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
            ws->nextCharacterPositionIndex = (*(ws->nodes + existingNodeIndex))->position;

            // Swap the existing node for the next one in the list.
            existingNodeIndex++;
        }
    }
}

/**
 * Enumerates the nodes signature indexes updating the frequency counts in the
 * linked list for any that exist already, adding those that still stand a chance
 * of making the count, or removing those that aren't necessary.
 * @param ws pointer to the work set used for the match
 * @param node pointer to the node whose signatures are to be added
 * @param count the current count that is needed to be considered
 * @param iteration the iteration that we're currently in
 * @return the count after the node has been evaluated
 */
int32_t setClosestSignaturesForNode(Workset *ws, const Node *node, int32_t count, int32_t iteration) {
    fod_bool                thresholdReached = (ws->nodeCount - iteration) < count;
    LinkedSignatureList     *linkedList = &(ws->linkedSignatureList);
    LinkedSignatureListItem *current = linkedList->first,
                            *next;
    int32_t                 index = 0;
    int32_t                 *firstRankedSignatureIndex = getFirstRankedSignatureIndexForNode(node),
                            *currentRankedSignatureIndex;

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
 * Orders the nodes so that those with the lowest rank are are the top of the
 * list.
 * @param ws pointer to the work set being used for the match
 * @param count frequency below which signatures should be removed
 */
void setClosestSignaturesFinal(Workset *ws, int32_t count) {
    LinkedSignatureList *linkedList = &(ws->linkedSignatureList);
    LinkedSignatureListItem *current = linkedList->first;

    /* First iteration to remove any items with lower than the maxcount. */
    while (current != NULL) {
        if (current->frequency < count) {
            linkedListRemove(linkedList, current);
        }
        current = current->next;
    }
}

/**
 * Compares the signature counts of two nodes for the purposes of ordering
 * them in ascending order of count.
 * @param a pointer to the first node
 * @param b pointer to the second node
 * @return the difference between the nodes
 */
int nodeSignatureCountCompare (const void * a, const void * b)
{
   return (*(Node**)a)->signatureCount - (*(Node**)b)->signatureCount;
}

/**
 * Orders the nodes found in ascending order of signature count.
 * @param ws pointer to the work set being used for the match
 */
void orderNodesOnSignatureCount(Workset *ws) {
    memcpy((void*)ws->orderedNodes, ws->nodes, ws->nodeCount * sizeof(Node*));
    qsort((void*)ws->orderedNodes, ws->nodeCount, sizeof(Node*), nodeSignatureCountCompare);
}

/**
 * Fills the workset with those signatures that are closest to the target user
 * agent. This is required before the NEAREST and CLOSEST methods are used to
 * determine which signature is in fact closest.
 * @param ws pointer to the work set being used for the match
 */
void fillClosestSignatures(Workset *ws) {
    int32_t     iteration = 2,
                maxCount = 1,
                nodeIndex = 1;
    const Node  *node;

    if (ws->nodeCount == 1) {
        // No need to create a linked list as only 1 node.
        ws->closestSignatures = ws->nodes[0]->signatureCount;
    } else {
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
 * Determines if a byte value is a numeric character.
 * @param value to be checked
 * @return 1 if the value is numeric, otherwise 0
 */
fod_bool getIsNumeric(byte *value) {
    return (*value >= (byte)'0' && *value <= (byte)'9');
}

/**
 * Works out the numeric difference between the current position and the target.
 * @param characters pointer to the ascii string of the node
 * @param ws pointer to the work set used for the match
 * @param nodeIndex pointer to the index in the node being evaluated
 * @param targetIndex pointer to the target user agent index
 * @return the absolute difference between the characters or 0 if not numeric
 */
int32_t calculateNumericDifference(const AsciiString *characters, Workset *ws, int32_t *nodeIndex, int32_t *targetIndex) {
    // Move right when the characters are numeric to ensure
    // the full number is considered in the difference comparison.
    int32_t newNodeIndex = *nodeIndex + 1;
    int32_t newTargetIndex = *targetIndex + 1;
    int16_t count = 0;
    while (newNodeIndex < characters->length &&
        newTargetIndex < ws->targetUserAgentArrayLength &&
        getIsNumeric(ws->targetUserAgentArray + newTargetIndex) &&
        getIsNumeric((byte*)(&(characters->firstByte) + newNodeIndex)))
    {
        newNodeIndex++;
        newTargetIndex++;
    }
    (*nodeIndex) = newNodeIndex - 1;
    (*targetIndex) = newTargetIndex - 1;

    // Find when the characters stop being numbers.
    while (
        nodeIndex >= 0 &&
        getIsNumeric(ws->targetUserAgentArray + *targetIndex) &&
        getIsNumeric((byte*)(&(characters->firstByte) + *nodeIndex)))
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
            getNumber(&(characters->firstByte), *nodeIndex + 1, count));
    }

    return 0;
}

/**
 * The index of the nodes characters in the target user agent.
 * @param ws pointer to the work set used for the match
 * @param node pointer to the node to be checked in the target
 * @return index in the target user agent of the nodes characters, or -1 if
 *         not present
 */
int16_t matchIndexOf(Workset *ws, const Node *node)
{
    const AsciiString *characters = getString(ws->dataSet, node->characterStringOffset);
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
            } else if (nodeIndex == finalNodeIndex) {
                return index;
            }
        }
    }
    return -1;
}

/**
 * Used to determine if the node is in the target user agent and if so how
 * many character positions different it is.
 * @param ws pointer to the workset used for the match
 * @param node pointer to be checked against the target user agent
 * @return the difference in character positions or -1 if not present.
 */
int32_t getScoreNearest(Workset *ws, const Node *node) {
    int16_t index = matchIndexOf(ws, node);
    if (index >= 0) {
        return abs(node->position + 1 - index);
    }

    // Return -1 to indicate that a score could not be calculated.
    return -1;
}

/**
 * Compares the node against the target user agent on a character by characters
 * basis.
 * @param ws pointer to the work set used for the current match
 * @param node pointer to the node being compared
 * @return the difference based on character comparisons between the two
 */
int32_t getScoreClosest(Workset *ws, const Node *node) {
    const AsciiString *characters = getNodeCharacters(ws->dataSet, node);
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
 * Calculates the score for difference score for the signature.
 * @param ws pointer to the workset used for the match
 * @param signature pointer to the signature to be compared
 * @param lastNodeCharacter position of the right most character in the
 *        signature
 * @return the difference score using the active scoring method between the
 *         signature and the target user agent
 */
int32_t getScore(Workset *ws,
                 const byte *signature,
                 int16_t lastNodeCharacter) {
    int32_t *nodeOffsets = getNodeOffsetsFromSignature(ws->dataSet, signature);
    int32_t count = getSignatureNodeOffsetsCount(ws->dataSet, nodeOffsets),
            runningScore = ws->startWithInitialScore == 1 ?
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
 * Evaluates a signature using the active scoring method.
 * @param ws pointer to the work set used for the match
 * @param signature pointer to be compared to the target user agent
 * @param lastNodeCharacter position of the right most node character in the
 *        signature
 */
void evaluateSignature(Workset *ws,
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
 * Gets the next signature to be examined for the single node found.
 * @param ws the current work set for the match.
 * @return null if there are no more signature pointers.
 */
const byte* getNextClosestSignatureForSingleNode(Workset *ws) {
    const byte *signature;
    if (ws->closestNodeRankedSignatureIndex < ws->nodes[0]->signatureCount) {
        signature = getSignatureByRankedIndex(ws->dataSet,
                        *(getFirstSignatureIndexForNode(ws->nodes[0]) + ws->closestNodeRankedSignatureIndex));
        ws->closestNodeRankedSignatureIndex++;
    } else {
        signature = NULL;
    }
    return signature;
}

/**
 * Gets the next signature to be examined from the linked list.
 * @param ws the current work set for the match.
 * @return null if there are no more signature pointers.
 */
const byte* getNextClosestSignatureForLinkedList(Workset *ws) {
    const byte *signature;
    if (ws->linkedSignatureList.current != NULL) {
        signature = getSignatureByRankedIndex(ws->dataSet,
                        ws->linkedSignatureList.current->rankedSignatureIndex);
        ws->linkedSignatureList.current = ws->linkedSignatureList.current->next;
    } else {
        signature = NULL;
    }
    return signature;
}

/**
 * Resets the work set ready to return the closest signatures.
 * @param ws the current work set for the match.
 */
void resetClosestSignatureEnumeration(Workset *ws) {
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
 * Evaluates all the signatures that are possible candidates for the result
 * against the target user agent using the active method.
 * @param ws pointer to the work set being used for the match
 */
void evaluateSignatures(Workset *ws) {
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
 * Evaluates the target user agent against root nodes to find those nodes
 * that match precisely.
 * @param ws pointer to the work set used for the match
 */
void evaluate(Workset *ws) {
    const Node *node;
    while (ws->nextCharacterPositionIndex > 0 &&
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
 * Main entry method used for perform a match.
 * @param ws pointer to a work set to be used for the match created via
 *        createWorkset function
 * @param userAgent pointer to the target user agent
 */
void match(Workset *ws, char* userAgent) {
    int32_t signatureIndex;
    setTargetUserAgentArray(ws, userAgent);
    if (ws->targetUserAgentArrayLength >= ws->dataSet->header.minUserAgentLength) {
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
}

/**
 * Sets the values associated with the require property index in the workset
 * so that an array of values can be read.
 * @param ws pointer to the work set associated with the match
 * @param requiredPropertyIndex index of the property required from the array of
 *        require properties
 * @return the number of values that were set.
 */
int32_t setValues(Workset *ws, int32_t requiredPropertyIndex) {
    int32_t profileIndex, valueIndex;
    const Profile *profile;
    const Property *property = *(ws->dataSet->requiredProperties + requiredPropertyIndex);
    int32_t *firstValueIndex;
    int32_t propertyIndex;
    const Value *value;
    ws->valuesCount = 0;
    if (property != NULL) {
        propertyIndex = getPropertyIndex(ws->dataSet, property);
        for(profileIndex = 0; profileIndex < ws->profileCount; profileIndex++) {
            profile = *(ws->profiles + profileIndex);
            if (profile->componentIndex == property->componentIndex) {
                firstValueIndex = (int32_t*)((byte*)profile + sizeof(Profile));
                for(valueIndex = 0; valueIndex < profile->valueCount; valueIndex++) {
                    value = ws->dataSet->values + *(firstValueIndex + valueIndex);
                    if (value->propertyIndex == propertyIndex) {
                        *(ws->values + ws->valuesCount) = value;
                        ws->valuesCount++;
                    }
                }
            }
        }
    }
    return ws->valuesCount;
}

/**
 * Process device properties into a CSV string
 */
int32_t processDeviceCSV(Workset *ws, char* result, int32_t resultLength) {
    int32_t propertyIndex, valueIndex, profileIndex;
	char* currentPos = result;
	char* endPos = result + resultLength;

	if (ws->profileCount > 0) {
        currentPos += snprintf(
            currentPos,
            (int32_t)(endPos - currentPos),
            "Id,");
        for(profileIndex = 0; profileIndex < ws->profileCount; profileIndex++) {
            currentPos += snprintf(
                currentPos,
                (int32_t)(endPos - currentPos),
                "%d",
                (*(ws->profiles + profileIndex))->profileId);
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

        for(propertyIndex = 0; propertyIndex < ws->dataSet->requiredPropertyCount; propertyIndex++) {
            if (setValues(ws, propertyIndex) > 0) {
                currentPos += snprintf(
                    currentPos,
                    (int32_t)(endPos - currentPos),
                    "%s,",
                    getPropertyName(ws->dataSet, *(ws->dataSet->requiredProperties + propertyIndex)));
                for(valueIndex = 0; valueIndex < ws->valuesCount; valueIndex++) {
                    currentPos += snprintf(
                        currentPos,
                        (int32_t)(endPos - currentPos),
                        "%s",
                        getValueName(ws->dataSet, *(ws->values + valueIndex)));
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
    return (int32_t)(currentPos - result);
}

