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

#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include "dmalloc.h"
#endif

#include "cache.h"
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

const char *strings[] = {
	"first",
	"second",
	"third",
	"fourth",
	"fifth",
	"sixth",
	"seventh",
	"eighth",
	"ninth",
	"tenth" };

/**
 * Used by the cache to load an element if it was not found in the cache.
 */
void elementLoader(fiftyoneDegreesCacheNode *node, long key) {
	char *element;

	// Free the memory associated with any existing data.
	if (node->data != NULL) {
		node->cache->freeCacheData(node->data);
	}

	// Copy the string into new memory allocated in the cache for it.
	node->data = element = (char*)node->cache->mallocCacheData(
		(strlen(strings[key - 1]) + 1) * sizeof(char));
	strcpy(element, strings[key - 1]);
}

/**
 * Check that the cache returns the correct value for all keys.
 */
void run(fiftyoneDegreesCache *cache, int first, int last) {
	int i;
	char *element;
	for (i = first; i <= last; i++) {
		element = (char*)fiftyoneDegreesLoadingCacheGet(cache, i);
		printf("%d: %s\n", i, element);
		assert(strcmp(element, strings[i - 1]) == 0);
	}
	printf("Allocated: %d/%d\n"
		"Hits: %d\n"
		"Misses: %d\n",
		cache->allocated, cache->capacity, cache->hits, cache->misses);
}

int main(int argc, char* argv[]) {

#ifdef _DEBUG
#ifndef _MSC_VER
	dmalloc_debug_setup("log-stats,log-non-free,check-fence,log=dmalloc.log");
#endif
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
#endif

	// Create the cache.
	fiftyoneDegreesCache *cache = fiftyoneDegreesCacheCreate(
		5, 
		&malloc,
		&free,
		&elementLoader,
		(void*)strings);
	assert(cache != NULL);

	// Check the cache is created with the correct size, and is empty.
	printf("Cache created with size %d\n", cache->capacity);
	printf("Allocated: %d/%d\n"
		"Hits: %d\n"
		"Misses: %d\n",
		cache->allocated, cache->capacity, cache->hits, cache->misses);
	assert(cache->hits == 0);
	assert(cache->misses == 0);
	assert(cache->allocated == 0);

	// Check all elements were added to the cache->
	printf("\nGetting elements not yet in the cache->\n");
	run(cache, 1, 5);
	assert(cache->hits == 0);
	assert(cache->misses == cache->capacity);
	assert(cache->allocated == cache->capacity);

	// Check all elements were retrieved from the cache->
	printf("\nGetting elements just added to the cache->\n");
	run(cache, 1, 5);
	assert(cache->hits == cache->capacity);
	assert(cache->misses == cache->capacity);
	assert(cache->allocated == cache->capacity);
	
	// Check new records are added to the cache->
	printf("\nGetting new elements not in the cache->\n");
	run(cache, 6, 10);
	assert(cache->hits == cache->capacity);
	assert(cache->misses == cache->capacity * 2);
	assert(cache->allocated == cache->capacity);
	
	fiftyoneDegreesCacheFree(cache);

#ifdef _DEBUG
#ifdef _MSC_VER
	_CrtDumpMemoryLeaks();
#else
	printf("Log file is %s\r\n", dmalloc_logpath);
#endif
#endif

	fgetc(stdin);
}