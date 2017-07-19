#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
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
 * This Source Code Form is “Incompatible With Secondary Licenses”, as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */

// Number of test passes to perform.
#define PASSES 5

// Number of detection to complete between reporting progress.
#define PROGRESS_MARKS 40

// Number of threads to start for performance analysis.
#ifndef FIFTYONEDEGREES_NO_THREADING
#define THREAD_COUNT 4
#else
#define THREAD_COUNT 1
#endif

// Used to control multi threaded performance.
typedef struct t_performance_state {
	char* fileName;
	fiftyoneDegreesWorksetPool *pool;
	int calibrate;
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX lock;
#endif
	int count;
	int progress;
	int max;
	int numberOfThreads;
	long valueCount;
	int passes;
	char *test;
} PERFORMANCE_STATE;

// Prints a progress bar
void printLoadBar(PERFORMANCE_STATE *state) {
	int i;
	int full = state->count / state->progress;
	int empty = (state->max - state->count) / state->progress;

	printf("\r\t[");
	for (i = 0; i < full; i++) {
		printf("=");
	}

	for (i = 0; i < empty; i++) {
		printf(" ");
	}
	printf("]");
}

void reportProgress(PERFORMANCE_STATE *state, int count) {

#ifndef FIFTYONEDEGREES_NO_THREADING
	// Lock the state whilst the counters are updated.
	FIFTYONEDEGREES_MUTEX_LOCK(&state->lock);
#endif

	// Increase the count.
	state->count += count;

	// Update the user interface.
	printLoadBar(state);

#ifndef FIFTYONEDEGREES_NO_THREADING
	// Unlock the signal now that the count has been updated.
	FIFTYONEDEGREES_MUTEX_UNLOCK(&state->lock);
#endif
}

// Execute a performance test using a file of null terminated useragent strings
// as input. If calibrate is true then the file is read but no detections
// are performed.
void runPerformanceTest(PERFORMANCE_STATE *state) {
	char *input = (char*)malloc(state->pool->dataSet->header.maxUserAgentLength + 1);
	char *result = NULL;
	long valueCount;
	FILE *inputFilePtr;
	int i, v, count = 0;
	fiftyoneDegreesWorkset *ws = NULL;

    // Open the file and check it's valid.
    inputFilePtr = fopen(state->fileName, "r");
	if (inputFilePtr == NULL) {
        printf("Failed to open file with null-terminating user agent strings to fiftyoneDegreesMatch against 51Degrees data file. \n");
        fclose(inputFilePtr);
        exit(0);
	}

    // Get the first entry from the file.
    result = fgets(input, state->pool->dataSet->header.maxUserAgentLength, inputFilePtr);

	while(result != NULL && !feof(inputFilePtr)) {

        // Split the string at carriage returns.
		strtok(result, "\n");

		// If detection should be performed get the result and the property values.
		if (state->calibrate == 0) {
            ws = fiftyoneDegreesWorksetPoolGet(state->pool);
			fiftyoneDegreesMatch(ws, input);
			valueCount = 0;
			for (i = 0; i < ws->dataSet->requiredPropertyCount; i++) {
				fiftyoneDegreesSetValues(ws, i);
				for (v = 0; v < ws->valuesCount; v++) {
					valueCount += (long)(ws->values[v]->nameOffset);
				}
			}
			fiftyoneDegreesWorksetPoolRelease(state->pool, ws);
#ifndef FIFTYONEDEGREES_NO_THREADING
			FIFTYONEDEGREES_MUTEX_LOCK(&state->lock);
#endif
			state->valueCount += valueCount;
#ifndef FIFTYONEDEGREES_NO_THREADING
			FIFTYONEDEGREES_MUTEX_UNLOCK(&state->lock);
#endif
		}

		count++;

		// Print a progress marker.
		if (count == state->progress) {
			reportProgress(state, count);
			count = 0;
		}

		// Get the next entry from the data file.
		result = fgets(input, state->pool->dataSet->header.maxUserAgentLength, inputFilePtr);
    }

    free(input);

	reportProgress(state, count);
	fclose(inputFilePtr);

#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_THREAD_EXIT;
#endif
}

// Perform the test and return the average time.
double performTest(PERFORMANCE_STATE *state) {
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_THREAD *threads = (FIFTYONEDEGREES_THREAD*)malloc(sizeof(FIFTYONEDEGREES_THREAD) * state->numberOfThreads);
    int thread;
#endif
	int pass;
	time_t start, end;
	fflush(stdout);

	state->progress = (state->max > 0 ? state->max : INT_MAX) / PROGRESS_MARKS;

	// Perform a number of passes of the test.
	time(&start);
	for(pass = 1; pass <= state->passes; pass++) {

		state->valueCount = 0;
		state->count = 0;

		printf("%s pass %i of %i: \n\n", state->test, pass, state->passes);

#ifndef FIFTYONEDEGREES_NO_THREADING
		// Create the threads.
		for (thread = 0; thread < state->numberOfThreads; thread++) {
			FIFTYONEDEGREES_THREAD_CREATE(threads[thread], (void*)&runPerformanceTest, state);
		}

		// Wait for them to finish.
		for (thread = 0; thread < state->numberOfThreads; thread++) {
			FIFTYONEDEGREES_THREAD_JOIN(threads[thread]);
		}
#else
        runPerformanceTest(state);
#endif

		printf("\n\n");

		// If the cache is being used then output the check value which
		// should be identical across multiple runs.
		if (state->calibrate == 0 && state->pool->cache != NULL) {
			printf("Cache check value = %ld\n\n", state->valueCount);
		}
	}

#ifndef FIFTYONEDEGREES_NO_THREADING
    free((void*)threads);
#endif

	time(&end);
	return difftime(end, start) / (double)state->passes;
}

// Performance test.
void performance(char *fileName, fiftyoneDegreesWorksetPool *pool) {
	double totalSec, calibration, test;
	PERFORMANCE_STATE state;

	state.pool = pool;
	state.fileName = fileName;
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_CREATE(state.lock);
#endif

	state.test = "Caching Data";
	state.calibrate = 1;
	state.max = 0;
	state.numberOfThreads = 1;
	state.passes = 1;
	performTest(&state);

	state.numberOfThreads = THREAD_COUNT;
	state.max = state.count * state.numberOfThreads;
	state.passes = PASSES;
	state.test = "Calibrate";
	calibration = performTest(&state);
	state.test = "Detection test";
	state.calibrate = 0;
	test = performTest(&state);

	// Time to complete.
	totalSec = test - calibration;
	printf("Number of records per iteration: %i s\n", state.count);
	printf("Average detection time for total data set: %.2f s\n", totalSec);
	printf("Average number of detections per second per thread: %.2f\n", (double)state.max / totalSec / (double)state.numberOfThreads);
	printf("Average milliseconds per detection: %.6f\n", (totalSec * (double)1000) / (double)state.max);
	if (pool->cache != NULL) {
		printf("Cache hits: %d\n", pool->cache->hits);
		printf("Cache misses: %d\n", pool->cache->misses);
		printf("Cache switches: %d\n", pool->cache->switches);
	}

#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_CLOSE(state.lock);
#endif

	// Wait for a character to be pressed.
	fgetc(stdin);
}

// Reduces a file path to file name only.
char *findFileNames(char *subject) {
	char *c = subject;
	char *hold = NULL;

	do {
		hold = strchr(c, '\\');
		if (hold == NULL) {
			return c;
		}
		else {
			c = hold + 1;
		}
	} while (1);

	return subject;
}

// The main method used by the command line test routine.
int main(int argc, char* argv[]) {
	fiftyoneDegreesDataSet dataSet;
	fiftyoneDegreesResultsetCache *cache;
	fiftyoneDegreesWorksetPool *pool;

	char *dataSetFileName = argc > 1 ? argv[1] : NULL;
	char *inputFileName = argc > 2 ? argv[2] : NULL;
    char *requiredProperties = argc > 3 ? argv[3] : NULL;
    int cacheSize = argc > 4 ? atoi(argv[4]) : 5000;

	printf("\n");
	printf("\t#############################################################\n");
    printf("\t#                                                           #\n");
    printf("\t#  This program can be used to test the performance of the  #\n");
    printf("\t#                51Degrees 'Pattern' C API.                 #\n");
    printf("\t#                                                           #\n");
    printf("\t#   The test will read a list of User Agents and calculate  #\n");
    printf("\t#            the number of detections per second.           #\n");
    printf("\t#                                                           #\n");
	printf("\t#  Command line arguments should be a csv file containing   #\n");
    printf("\t#  a list of user agents. A test file of 1 million can be   #\n");
	printf("\t#    downloaded from http://51degrees.com/million.zip       #\n");
	printf("\t#                                                           #\n");
    printf("\t#############################################################\n");

    if (dataSetFileName == NULL || inputFileName == NULL)
    {
        printf("Not enough arguments supplied. Expecting path/to/51Degrees.dat path/to/test_file.csv \n");
        return 0;
    }

    switch(fiftyoneDegreesInitWithPropertyString(dataSetFileName, &dataSet, requiredProperties)) {
        case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
            printf("Insufficient memory to load '%s'.", argv[1]);
            break;
		case DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS:
        case DATA_SET_INIT_STATUS_CORRUPT_DATA:
            printf("Device data file '%s' is corrupted.", argv[1]);
            break;
        case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
            printf("Device data file '%s' is not correct version.", argv[1]);
            break;
        case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
            printf("Device data file '%s' not found.", argv[1]);
            break;
        case DATA_SET_INIT_STATUS_NOT_SET:
            printf("Device data file '%s' could not be used to initialise.", argv[1]);
            break;
		case DATA_SET_INIT_STATUS_NULL_POINTER:
			printf("Null pointer prevented loading of '%s'.", argv[1]);
		default: {
			cache = fiftyoneDegreesResultsetCacheCreate(&dataSet, cacheSize);
            pool = fiftyoneDegreesWorksetPoolCreate(&dataSet, cache, THREAD_COUNT);
            if (pool != NULL) {
                printf("\n\nUseragents file is: %s\n", findFileNames(inputFileName));
                if (pool->cache != NULL) {
                    printf("Cache Size is: %d\n\n", pool->cache->total);
                } else {
                    printf("No Cache\n\n");
                }
                performance(inputFileName, pool);
                fiftyoneDegreesWorksetPoolFree(pool);
            }
			if (cache != NULL) {
				fiftyoneDegreesResultsetCacheFree(cache);
			}
            fiftyoneDegreesDataSetFree(&dataSet);
        }
        break;
    }
	return 0;
}
