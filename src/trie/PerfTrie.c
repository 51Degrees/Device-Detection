#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include "../threading.h"
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
* This Source Code Form is "Incompatible With Secondary Licenses", as
* defined by the Mozilla Public License, v. 2.0.
********************************************************************** */

#ifdef _DEBUG
#define PASSES 1
#else
#define PASSES 10
#endif

// Size of the character buffers
#define BUFFER 50000

// Number of marks to make when showing progress.
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
	int calibrate;
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX lock;
#endif
	int count;
	int progress;
	int max;
	int numberOfThreads;
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

void reportProgress(PERFORMANCE_STATE *perfState, int count, int device, int propertyIndex) {

#ifndef FIFTYONEDEGREES_NO_THREADING
	// Lock the state whilst the counters are updated.
	FIFTYONEDEGREES_MUTEX_LOCK(&perfState->lock);
#endif

	// Increase the count.
	perfState->count += count;

	// Update the user interface.
	printLoadBar(perfState);

	// If in real detection mode then print the id of the device found
	// to prove it's actually doing something!
	if (perfState->calibrate == 0) {
		printf(" %s  ", fiftyoneDegreesGetValue(device, propertyIndex));
	}

#ifndef FIFTYONEDEGREES_NO_THREADING
	// Unlock the signal now that the count has been updated.
	FIFTYONEDEGREES_MUTEX_UNLOCK(&perfState->lock);
#endif
}

void runPerformanceTest(void* state) {
	PERFORMANCE_STATE *perfState = (PERFORMANCE_STATE*)state;
	const char *result;
	char userAgent[BUFFER];
	int device = INT_MAX;
	int propertyIndex = fiftyoneDegreesGetPropertyIndex("Id");
	FILE *inputFilePtr = fopen(perfState->fileName, "r");
	int count = 0;

    // Get the next character from the input.
    result = fgets(userAgent, BUFFER, inputFilePtr);

	while(result != NULL && !feof(inputFilePtr)) {

		// If we're not calibrating then get the device for the
		// useragent that has just been read.
		if (strlen(userAgent) < 1024 && perfState->calibrate == 0) {
			device = fiftyoneDegreesGetDeviceOffset(userAgent);
		}

		// Increase the local counter.
		count++;

		// Print a progress marker.
		if (count == perfState->progress) {

			reportProgress(perfState, count, device, propertyIndex);

			// Reset the local counter.
			count = 0;
		}

		// Get the next character from the input.
        result = fgets(userAgent, BUFFER, inputFilePtr);
	}

	fclose(inputFilePtr);

	// Finally report progress.
	reportProgress(perfState, count, device, propertyIndex);

#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_THREAD_EXIT;
#endif
}

// Execute a performance test using a file of null terminated useragent strings
// as input. If calibrate is true then the file is read but no detections
// are performed.
void performanceTest(PERFORMANCE_STATE *state) {
#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_THREAD *threads = (FIFTYONEDEGREES_THREAD*)malloc(sizeof(FIFTYONEDEGREES_THREAD) * state->numberOfThreads);
	int thread;
	FIFTYONEDEGREES_MUTEX_CREATE(state->lock);
#endif
	state->count = 0;

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

#ifndef FIFTYONEDEGREES_NO_THREADING
	free(threads);
	FIFTYONEDEGREES_MUTEX_CLOSE(state->lock);
#endif
}

// Perform the test and return the average time.
double performTest(PERFORMANCE_STATE *state, int passes, char *test) {
	int pass;
	time_t start, end;
	fflush(stdout);

	// Set the progress indicator.
	state->progress = (state->max > 0 ? state->max : INT_MAX) / PROGRESS_MARKS;

	// Perform a number of passes of the test.
	time(&start);
	for (pass = 1; pass <= passes; pass++) {
		printf("%s pass %i of %i: \n\n", test, pass, passes);
		performanceTest(state);
	}
	time(&end);
	return difftime(end, start) / (double)passes;
}

// Performance test.
void performance(char *fileName) {
	PERFORMANCE_STATE state;
	double totalSec, calibration, test;

	state.max = 0;
	state.fileName = fileName;
	state.calibrate = 1;

	// Get the number of records in the data file and also
	// get it loaded into any available disk cache.
	state.numberOfThreads = 1;
	performTest(&state, 1, "Caching Data");
	state.numberOfThreads = THREAD_COUNT;
	state.max = state.count * state.numberOfThreads;

	// Process the data file doing all tasks except detecting
	// the device.
	calibration = performTest(&state, PASSES, "Calibrate"),

	// Process the data file doing the device detection.
	state.calibrate = 0;
	test = performTest(&state, PASSES, "Detection test");

	// Time to complete.
	totalSec = test - calibration;
	printf("Average detection time for total data set: %.2fs\n", totalSec);
	printf("Average number of detections per second: %.0f\n", (double)state.max / totalSec);
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
}

// Check that file exists.
int fileExists(char *fileName) {
	FILE *filePtr = fopen(fileName, "rb");
	if (filePtr != NULL)
		fclose(filePtr);
	return filePtr != NULL;
}

// The main method used by the command line test routine.
int main(int argc, char* argv[]) {
	printf("\n");
	printf("\t#############################################################\n");
	printf("\t#                                                           #\n");
	printf("\t#  This program can be used to test the performance of the  #\n");
	printf("\t#                 51Degrees 'Trie' C API.                   #\n");
	printf("\t#                                                           #\n");
	printf("\t#   The test will read a list of User Agents and calculate  #\n");
	printf("\t#            the number of detections per second.           #\n");
	printf("\t#                                                           #\n");
	printf("\t#    Command line arguments should be a tree format data    #\n");
	printf("\t#   file and a csv file containing a list of user agents.   #\n");
	printf("\t#      A test file of 1 million can be downloaded from      #\n");
	printf("\t#            http://51degrees.com/million.zip               #\n");
	printf("\t#                                                           #\n");
	printf("\t#############################################################\n");

	if (argc > 2) {

		char *fileName = argc > 1 ? argv[1] : NULL;
		char *requiredProperties = argc > 3 ? argv[3] : NULL;

		fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_SUCCESS;
		status = fiftyoneDegreesInitWithPropertyString(fileName, requiredProperties);
		switch (status) {
		case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
			printf("Insufficient memory to load '%s'.", argv[1]);
			break;
		case DATA_SET_INIT_STATUS_CORRUPT_DATA:
			printf("Device data file '%s' is corrupted.", argv[1]);
			break;
		case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
			printf("Device data file '%s' is not correct version.", argv[1]);
			break;
		case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
			printf("Device data file '%s' not found.", argv[1]);
			break;
		default:
			printf("\n\nUseragents file is: %s\n\nData file is: %s\n\n",
				findFileNames(argv[2]),
				findFileNames(argv[1]));

			// Wait for a character to be pressed.
			printf("\nPress enter to start performance tests.\n");
			fgetc(stdin);

			// Run the performance tests.
			performance(argv[2]);

			break;
		}

		// Free the memory used by the trie detector.
		fiftyoneDegreesDestroy();

		// Wait for a character to be pressed.
		fgetc(stdin);
	}
	else {
		printf("Not enough arguments supplied. Expecting: path/to/trie_file path/to/test_file property1,property2(optional)\n");
	}

	return 0;
}
