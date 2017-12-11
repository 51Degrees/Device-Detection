#include "cache.h"

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

// Define NDEBUG if needed, to ensure asserts are not called in release builds.
#if !defined(DEBUG) && !defined(_DEBUG) && !defined(NDEBUG)
#define NDEBUG
#endif
#include <assert.h>

/**
 * WARNING: The loading cache should not be used in multi-threaded
 * environments. Pointers to entries returned may be changed by another
 * thread while in use. For this reason, always define
 * FIFTYONEDEGREES_NO_THREADING when compiling with
 * FIFTYONEDEGREES_INDIRECT.
 */

/**
 * Macros used in the binary tree.
 */

#define TREE_COLOUR_RED  1 // Indicates the node is black
#define TREE_COLOUR_BLACK 0 // Indicates the node is red

/* Gets the root of the tree. The TREE_FIRST macro gets the first node. */
#define TREE_ROOT(c) &c->root

/* Gets the empty node used to indicate no further data. */
#define TREE_EMPTY(c) (fiftyoneDegreesCacheNode*)&c->empty

/* Gets the first node under the root. */
#define TREE_FIRST(c) c->root.treeLeft 

/**
 * RED BLACK BINARY TREE METHODS
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
 *    the cache data itself. See structure fiftyoneDegreesCacheNode.
 * 4. The cache structure fiftyoneDegreesCacheNodeCache contains special
 *    fields "empty" and "root". "Empty" is used in place of NULL to indicate
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
 */

/**
 * \cond
 * Used by assert statements to validate the number of entries in the cache for
 * debugging should any changes be made to the logic. Should not be compiled in
 * release builds.
 * @param current pointer to the node being counted.
 * @returns the number of children plus 1 for this current node.
 * \endcond
 */
static int32_t cacheTreeCount(fiftyoneDegreesCacheNode *current) {
	int32_t count = 0;
	if (current != TREE_EMPTY(current->cache)) {
		count = 1;
		if (current->treeLeft != TREE_EMPTY(current->cache)) {
			count += cacheTreeCount(current->treeLeft);
			assert(current->hash > current->treeLeft->hash);
		}
		if (current->treeRight != TREE_EMPTY(current->cache)) {
			count += cacheTreeCount(current->treeRight);
			assert(current->hash < current->treeRight->hash);
		}
	}
	return count;
}

/**
 * \cond
 * Rotates the red black tree node to the left.
 * @param node pointer to the node being rotated.
 * \endcond
 */
static void cacheTreeRotateLeft(fiftyoneDegreesCacheNode *node) {
	fiftyoneDegreesCacheNode *child = node->treeRight;
	node->treeRight = child->treeLeft;

	if (child->treeLeft != TREE_EMPTY(node->cache)) {
		child->treeLeft->treeParent = node;
	}
	child->treeParent = node->treeParent;

	if (node == node->treeParent->treeLeft) {
		node->treeParent->treeLeft = child;
	}
	else {
		node->treeParent->treeRight = child;
	}

	child->treeLeft = node;
	node->treeParent = child;
}

/**
 * \cond
 * Rotates the red black tree node to the right.
 * @param node pointer to the node being rotated.
 * \endcond
 */
static void cacheTreeRotateRight(fiftyoneDegreesCacheNode *node) {
	fiftyoneDegreesCacheNode *child = node->treeLeft;
	node->treeLeft = child->treeRight;

	if (child->treeRight != TREE_EMPTY(node->cache)) {
		child->treeRight->treeParent = node;
	}
	child->treeParent = node->treeParent;

	if (node == node->treeParent->treeLeft) {
		node->treeParent->treeLeft = child;
	}
	else {
		node->treeParent->treeRight = child;
	}

	child->treeRight = node;
	node->treeParent = child;
}

/**
 * \cond
 * Maintains the properties of the binary tree following an insert.
 * @param node pointer to the node being repaired after insert.
 * \endcond
 */
static void cacheTreeInsertRepair(fiftyoneDegreesCacheNode *node) {
	fiftyoneDegreesCacheNode *uncle;

	while (node->treeParent->colour == TREE_COLOUR_RED) {
		if (node->treeParent == node->treeParent->treeParent->treeLeft) {
			uncle = node->treeParent->treeParent->treeRight;
			if (uncle->colour == TREE_COLOUR_RED) {
				node->treeParent->colour = TREE_COLOUR_BLACK;
				uncle->colour = TREE_COLOUR_BLACK;
				node->treeParent->treeParent->colour = TREE_COLOUR_RED;
				node = node->treeParent->treeParent;
			}
			else {
				if (node == node->treeParent->treeRight) {
					node = node->treeParent;
					cacheTreeRotateLeft(node);
				}
				node->treeParent->colour = TREE_COLOUR_BLACK;
				node->treeParent->treeParent->colour = TREE_COLOUR_RED;
				cacheTreeRotateRight(node->treeParent->treeParent);
			}
		}
		else {
			uncle = node->treeParent->treeParent->treeLeft;
			if (uncle->colour == TREE_COLOUR_RED) {
				node->treeParent->colour = TREE_COLOUR_BLACK;
				uncle->colour = TREE_COLOUR_BLACK;
				node->treeParent->treeParent->colour = TREE_COLOUR_RED;
				node = node->treeParent->treeParent;
			}
			else {
				if (node == node->treeParent->treeLeft) {
					node = node->treeParent;
					cacheTreeRotateRight(node);
				}
				node->treeParent->colour = TREE_COLOUR_BLACK;
				node->treeParent->treeParent->colour = TREE_COLOUR_RED;
				cacheTreeRotateLeft(node->treeParent->treeParent);
			}
		}
	}
}

/**
 * \cond
 * Inserts the node into the red black tree.
 * @param node pointer to the node being inserted.
 * \endcond
 */
static void cacheTreeInsert(const fiftyoneDegreesCacheNode *node) {
	fiftyoneDegreesCache *cache = node->cache;
	fiftyoneDegreesCacheNode *current = TREE_FIRST(cache);
	fiftyoneDegreesCacheNode *parent = TREE_ROOT(cache);
	
	// Work out the correct point to insert the node.
	while (current != TREE_EMPTY(cache)) {
		parent = current;
		assert(node->hash != current->hash);
		current = node->hash < current->hash
			? current->treeLeft
			: current->treeRight;
	}

	// Set up the node being inserted in the tree.
	current = (fiftyoneDegreesCacheNode*)node;
	current->treeLeft = TREE_EMPTY(cache);
	current->treeRight = TREE_EMPTY(cache);
	current->treeParent = parent;
	if (parent == TREE_ROOT(cache) ||
		current->hash < parent->hash) {
		parent->treeLeft = current;
	}
	else {
		parent->treeRight = current;
	}
	current->colour = TREE_COLOUR_RED;

	cacheTreeInsertRepair(current);

	TREE_FIRST(cache)->colour = TREE_COLOUR_BLACK;
}

/**
 * \cond
 * Returns the node that matches the hash code provided.
 * @param cache to search in
 * @param hash key to get the item for
 * @returns the corresponding node if it exists in the cache, otherwise
 * null.
 * \endcond
 */
static fiftyoneDegreesCacheNode* cacheTreeFind(
		fiftyoneDegreesCache *cache,
		int64_t hash) {
	int32_t iterations = 0;
	fiftyoneDegreesCacheNode *current = TREE_FIRST(cache);

	while (current != TREE_EMPTY(cache)) {
		iterations++;
		if (hash == current->hash) {
			return current;
		}
		current = hash < current->hash
			? current->treeLeft
			: current->treeRight;
	}

	if (iterations > cache->maxIterations) {
		cache->maxIterations = iterations;
	}

	return NULL;
}

/**
 * \cond
 * Finds the successor for the node provided.
 * @param node pointer to the node whose successor is required.
 * @returns the successor for the node which may be empty.
 * \endcond
 */
static fiftyoneDegreesCacheNode* cacheTreeSuccessor(
		fiftyoneDegreesCacheNode *node) {
	const fiftyoneDegreesCache *cache = node->cache;
	fiftyoneDegreesCacheNode *successor = node->treeRight;
	if (successor != TREE_EMPTY(cache)) {
		while (successor->treeLeft != TREE_EMPTY(cache)) {
			successor = successor->treeLeft;
		}
	}
	else {
		for (successor = node->treeParent;
			node == successor->treeRight;
			successor = successor->treeParent) {
			node = successor;
		}
		if (successor == TREE_ROOT(cache)) {
			successor = TREE_EMPTY(cache);
		}
	}
	return successor;
}

/**
 * \cond
 * Following a deletion repair the section of the tree impacted.
 * @param node pointer to the node below the one deleted.
 * \endcond
 */
static void cacheTreeDeleteRepair(fiftyoneDegreesCacheNode *node) {
	const fiftyoneDegreesCache *cache = node->cache;
	fiftyoneDegreesCacheNode *sibling;

	while (node->colour == TREE_COLOUR_BLACK && node != TREE_FIRST(cache)) {
		if (node == node->treeParent->treeLeft) {
			sibling = node->treeParent->treeRight;
			if (sibling->colour == TREE_COLOUR_RED) {
				sibling->colour = TREE_COLOUR_BLACK;
				node->treeParent->colour = TREE_COLOUR_RED;
				cacheTreeRotateLeft(node->treeParent);
				sibling = node->treeParent->treeRight;
			}
			if (sibling->treeRight->colour == TREE_COLOUR_BLACK &&
				sibling->treeLeft->colour == TREE_COLOUR_BLACK) {
				sibling->colour = TREE_COLOUR_RED;
				node = node->treeParent;
			}
			else {
				if (sibling->treeRight->colour == TREE_COLOUR_BLACK) {
					sibling->treeLeft->colour = TREE_COLOUR_BLACK;
					sibling->colour = TREE_COLOUR_RED;
					cacheTreeRotateRight(sibling);
					sibling = node->treeParent->treeRight;
				}
				sibling->colour = node->treeParent->colour;
				node->treeParent->colour = TREE_COLOUR_BLACK;
				sibling->treeRight->colour = TREE_COLOUR_BLACK;
				cacheTreeRotateLeft(node->treeParent);
				node = TREE_FIRST(cache);
			}
		}
		else {
			sibling = node->treeParent->treeLeft;
			if (sibling->colour == TREE_COLOUR_RED) {
				sibling->colour = TREE_COLOUR_BLACK;
				node->treeParent->colour = TREE_COLOUR_RED;
				cacheTreeRotateRight(node->treeParent);
				sibling = node->treeParent->treeLeft;
			}
			if (sibling->treeRight->colour == TREE_COLOUR_BLACK &&
				sibling->treeLeft->colour == TREE_COLOUR_BLACK) {
				sibling->colour = TREE_COLOUR_RED;
				node = node->treeParent;
			}
			else {
				if (sibling->treeLeft->colour == TREE_COLOUR_BLACK) {
					sibling->treeRight->colour = TREE_COLOUR_BLACK;
					sibling->colour = TREE_COLOUR_RED;
					cacheTreeRotateLeft(sibling);
					sibling = node->treeParent->treeLeft;
				}
				sibling->colour = node->treeParent->colour;
				node->treeParent->colour = TREE_COLOUR_BLACK;
				sibling->treeLeft->colour = TREE_COLOUR_BLACK;
				cacheTreeRotateRight(node->treeParent);
				node = TREE_FIRST(cache);
			}
		}
	}
	node->colour = TREE_COLOUR_BLACK;
}

/**
 * \cond
 * Removes the node from the tree so that it can be used again to store
 * another result. The node will come from the last item in the cache's
 * linked list.
 * @param node pointer to be deleted.
 * \endcond
 */
static void cacheTreeDelete(fiftyoneDegreesCacheNode *node) {
	fiftyoneDegreesCache *cache = node->cache;
	fiftyoneDegreesCacheNode *x, *y;

	if (node->treeLeft == TREE_EMPTY(cache) ||
		node->treeRight == TREE_EMPTY(cache)) {
		y = node;
	}
	else {
		y = cacheTreeSuccessor(node);
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
		cacheTreeDeleteRepair(x);
	}
	if (y != node) {
		y->treeLeft = node->treeLeft;
		y->treeRight = node->treeRight;
		y->treeParent = node->treeParent;
		y->colour = node->colour;
		node->treeLeft->treeParent = y;
		node->treeRight->treeParent = y;
		if (node == node->treeParent->treeLeft) {
			node->treeParent->treeLeft = y;
		}
		else {
			node->treeParent->treeRight = y;
		}
	}
}

/**
 * \cond
 * Validates the cache by checking the number of entries in the linked list and
 * the tree. Used by assert statements to validate the integrity of the cache
 * during development. Should not be compiled in release builds.
 * @param cache pointer to the cache being validated.
 * @returns always return 0 as the purpose is to execute asserts in debug builds.
 * \endcond
 */
#ifndef NDEBUG
static int cacheValidate(const fiftyoneDegreesCache *cache) {
	int linkedListEntriesForward = 0;
	int linkedListEntriesBackwards = 0;
	int binaryTreeEntries = 0;
	fiftyoneDegreesCacheNode *node;

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
	// node doesn't contain any data.
	binaryTreeEntries = cacheTreeCount(TREE_FIRST(cache));
	assert(binaryTreeEntries == cache->allocated ||
		binaryTreeEntries == cache->allocated - 1);

	return 0;
}
#endif

/**
 * \cond
 * Initialises the cache by setting pointers for the linked list and binary
 * tree.
 * @param cache pointer to the cache to be initialised
 * @param init optional external init function to call
 * \endcond
 */
static void cacheInit(
	fiftyoneDegreesCache *cache) {
	int i;
	fiftyoneDegreesCacheNode *current = NULL;

	// Configure the empty not.
	current = TREE_EMPTY(cache);
	current->treeLeft = TREE_EMPTY(cache);
	current->treeRight = TREE_EMPTY(cache);
	current->treeParent = TREE_EMPTY(cache);
	current->colour = TREE_COLOUR_BLACK;
	current->hash = 0;
	current->cache = cache;

	// Configure the fake root node to avoid splitting the root.
	current = TREE_ROOT(cache);
	current->treeLeft = TREE_EMPTY(cache);
	current->treeRight = TREE_EMPTY(cache);
	current->treeParent = TREE_EMPTY(cache);
	current->colour = TREE_COLOUR_BLACK;
	current->hash = 0;
	current->cache = cache;

	// Set the default values for an empty cache.
	cache->allocated = 0;
	cache->maxIterations = 0;
	cache->listFirst = NULL;
	cache->listLast = NULL;
	for (i = 0; i < cache->capacity; i++) {
		current = cache->list + i;
		current->cache = cache;
		current->data = (void*)NULL;
		current->size = 0;
	}
}

/**
 * CACHE METHODS
 */

 /**
 * \cond
 * Moves the node to the head of the linked list. Used when a node
 * which is already in the cache is being returned and the cache needs to know
 * that is has recently been used and shouldn't be considered for removal. Also
 * used when a new entry is added to the cache.
 * @param node pointer to the node to move to the head of the linked list.
 * \endcond
 */
static void cacheMoveToHead(fiftyoneDegreesCacheNode *node) {
	fiftyoneDegreesCache *cache = (fiftyoneDegreesCache*)node->cache;

	// Only consider moving if not already the first entry.
	if (cache->listFirst != node) {

		// If the entry is the last one in the list then set the last pointer
		// to the entry before the last one.
		if (node == cache->listLast) {
			cache->listLast = cache->listLast->listPrevious;
			cache->listLast->listNext = NULL;
		}

		// Remove the entry from the linked list.
		if (node->listPrevious != NULL) {
			node->listPrevious->listNext = node->listNext;
		}
		if (node->listNext != NULL) {
			node->listNext->listPrevious = node->listPrevious;
		}

		// Move to become the first entry. 
		if (node->cache->listFirst != NULL) {
			node->cache->listFirst->listPrevious = node;
		}
		node->listNext = cache->listFirst;
		cache->listFirst = node;
		cache->listFirst->listPrevious = NULL;
	}

	// Validate the state of the list.
	assert(cache->listFirst == node);
	assert(cache->listFirst->listPrevious == NULL);
}

/**
 * \cond
 * Returns the next free node from the cache which can be used to add a
 * new entry to. Once the cache is full then the node returned is the one
 * at the end of the linked list which will contain the least recently used
 * data.
 * @param cache cache to return the next free node from.
 * @returns a pointer to a free node.
 * \endcond
 */
static fiftyoneDegreesCacheNode *cacheGetNextFree(
		fiftyoneDegreesCache *cache) {
	int countBefore, countAfter;
	fiftyoneDegreesCacheNode *last; // The oldest node in the cache.

	if (cache->allocated < cache->capacity) {
		// Return the free element at the end of the cache and update
		// the number of allocated elements.
		last = &cache->list[cache->allocated++];
	}
	else {
		// Remove the last node from the linked list.
		last = cache->listLast;
		last->listPrevious->listNext = NULL;
		cache->listLast = last->listPrevious;

		// Remove the last result from the binary tree.
		countBefore = cacheTreeCount(TREE_FIRST(cache));
		cacheTreeDelete(last);
		countAfter = cacheTreeCount(TREE_FIRST(cache));
		assert(countBefore - 1 == countAfter);
	}

	// Set the pointers of the node to null indicating that the
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
 * Updates the binary tree and the linked list with the provided node.
 * @param node pointer to the node to add to the cache.
 * \endcond
 */
static void cacheAdd(fiftyoneDegreesCacheNode *node) {
	fiftyoneDegreesCache *cache = node->cache;

	assert(cacheValidate(cache) == 0);

	cacheTreeInsert(node);

	assert(cacheValidate(cache) == 0);

	if (cache->listFirst == NULL) {
		cache->listLast = node;
		cache->listFirst = node;
	}
	else {
		cacheMoveToHead(node);
	}

	assert(cacheValidate(cache) == 0);
}

/**
 * \cond
 * If the cache contains a node with a key that matches the provided hash the
 * entry is returned. In addition to returning the entry the linked list in the
 * cache is updated to indicate that the item returned has recently been
 * retrieved.
 * @param cache to search for the item in
 * @param hash key indicating the item to find
 * @returns fiftyoneDegreesCacheNode* matching node, or null if none matched
 * \endcond
 */
static fiftyoneDegreesCacheNode *cacheFetch(
		fiftyoneDegreesCache *cache, 
		const int64_t hash) {
	fiftyoneDegreesCacheNode *found;

	assert(cacheValidate(cache) == 0);

	// Find the node that matches the value requested.
	found = cacheTreeFind(cache, hash);

	// If the target is already in the cache move the entry to the head of the
	// linked list. No need to change the binary tree. Update the cache 
	// statistics for both misses and hits.
	if (found != NULL) {
		cacheMoveToHead(found);
		cache->hits++;
	}
	else {
		cache->misses++;
	}

	assert(cacheValidate(cache) == 0);

	// Return the found item even if there was a miss. This is the entry that
	// the results of the device detection that will follow if a miss occurred
	// will be set in before the result set is added back into the cache.
	return found;
}

/**
 * EXTERNAL CACHE METHODS
 */

/**
 * \cond
 * Creates a new cache.The cache must be destroyed with the
 * fiftyoneDegreesFreeCache method.
 * @param size maximum number of items that the cache should store
 * @returns a pointer to the cache created, or NULL if one was not created.
 * \endcond
 */
fiftyoneDegreesCache *fiftyoneDegreesCacheCreate(
	int32_t capacity,
	void*(*malloc)(size_t __size),
	void(*free)(void*),
	void(*load)(fiftyoneDegreesCacheNode*, long),
	void *params) {

	// The capacity of the cache must be 2 or greater.
	if (capacity < 2) {
		return NULL;
	}

	// Work out the total amount of memory used by the cache. Keep the list 
	// of nodes and header together so they're in the same continuous memory
	// space and are allocated in a single operation.
	size_t cacheSize = sizeof(fiftyoneDegreesCache) + 
		(sizeof(fiftyoneDegreesCacheNode) * capacity);
	fiftyoneDegreesCache *cache = (fiftyoneDegreesCache*)malloc(cacheSize);
	if (cache != NULL) {
		// The list is set to the byte after the header.
		cache->list = (fiftyoneDegreesCacheNode*)(cache + 1);

		// Set the parameters for the cache.
		cache->mallocCacheData = malloc;
		cache->freeCacheData = free;
		cache->loadCacheData = load;
		cache->params = params;
		cache->hits = 0;
		cache->misses = 0;
		cache->capacity = capacity;
		cache->allocated = 0;
		cache->listFirst = NULL;
		cache->listLast = NULL;
#ifndef FIFTYONEDEGREES_NO_THREADING
		FIFTYONEDEGREES_MUTEX_CREATE(cache->lock);
#endif

		// Initialise the linked lists and binary tree.
		cacheInit(cache);
	}
	return cache;
}

/**
 * \cond
 * Frees the cache structure, all allocated nodes and their data.
 * @param cache to be freed
 * \endcond
 */
void fiftyoneDegreesCacheFree(fiftyoneDegreesCache *cache) {
	int i;
	
	// Free any data items that are created and are marked to be freed by the
	// cache when it is disposed of.
	for (i = 0; i < cache->capacity; i++) {
		if (cache->list[i].data != NULL) {
			cache->freeCacheData(cache->list[i].data);
			cache->list[i].data = NULL;
		}
	}

#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_CLOSE(cache->lock);
#endif

	// Free the cache and the node list.
	cache->freeCacheData(cache);
}

/**
 * \cond
 * Gets an item from the cache. If an item is not in the cache, it is loaded
 * using the loader the cache was initialized with.
 *
 * The cache being used as a loading cache must have a load method defined
 * which returns a pointer to the data relating to the key used. This method
 * may, or may not, allocate memory or free memory previously allocated to
 * data in the cache node.
 *
 * Note that this cache is not entirely thread safe. Although all cache 
 * operations are thread safe, any pointers returned are not reserved
 * while they are in use, so another thread may remove the entry from the
 * cache, changing the entry being used by the first thread. For this
 * reason, when a loading cache is being used, only one thread should use it at
 * once. In practical terms, this means that when compiling 51Degrees.c with
 * FIFTYONEDEGREES_INDIRECT, FIFTYONEDEGREES_NO_THREADING must also be
 * defined.
 * @param cache to get the entry from
 * @param key that indicates the item to get
 * @returns void* pointer to the requested item
 * \endcond
 */
void* fiftyoneDegreesLoadingCacheGet(
		const fiftyoneDegreesCache *cache, 
		const long key) {
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_LOCK(&cache->lock);
#endif
	// Get the entry from the cache.
	fiftyoneDegreesCacheNode *node = cacheFetch(
		(fiftyoneDegreesCache*)cache, key);
	if (node == NULL) {
		// An entry with the requested key did not exist in the cache,
		// so load the entry and add it to the cache.
		node = cacheGetNextFree((fiftyoneDegreesCache*)cache);
		cache->loadCacheData(node, key);
		node->hash = key;
		cacheAdd(node);
	}
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_UNLOCK(&cache->lock);
#endif
	return node->data;
}