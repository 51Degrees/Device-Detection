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

/*
<tutorial>
Reload from file example that shows how to:
<ol>
	<li>Only maintain a reference to the fiftyoneDegreesWorksetPool and use the
	reference to access dataset and cache.
	<li>Use the fiftyoneDegreesDatasetReloadFromFile function to reload the
	dataset, cache and workset pool from the same location and with the same
	set of properties.
	<li>Retrieve a workset from the pool and return it back into the pool when
	done with detecting current User-Agent.
	<li>Use the reload functionality in a single threaded environment.
	<li>Use the reload functionality in a multi threaded environment.
</ol>
<p>
	This example illustrates how to use a single reference to the provider 
	structure to use device detection and invoke the reload functionality 
	instead of maintaining a reference to the dataset, pool and cache 
	structures separately.
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesProvider provider;
	</pre></p>
</p>
<p>
	Example assumes that the initial pool, dataset and cache were created using
	either the fiftyoneDegreesInitProviderWithPropertyString function,
	or the fiftyoneDegreesInitProviderWithPropertyArray function.
</p>
<p>
	The fiftyoneDegreesProviderReloadFromFile function requires an existing 
	pool with the initialized dataset and cache. Function reloads the dataset 
	from the same location and with the same parameters as the original dataset. 
	New cache and pool is created.
</p>
<p>
	Please keep in mind that even if the current dataset was constructed with
	all available properties this does not guarantee that the new dataset will
	be initialized with the same set of properties. If the new data file
	contains properties that were not part of the original data file, the new
	extra property(ies) will not be initialized. If the new data file does not
	contain one or more property that were previously available, then these
	property(ies) will not be initialized.
</p>
<p>
	Each successful data file reload should be accompanied by the integrity
	check to verify that the properties you want have indeed been loaded. This
	can be achieved by simply comparing the number of properties before and
	after the reload as the number can not go up but it can go down.
</p>
<p>
	Example also demonstrates the concept of a workset pool. A workset pool is
	a thread safe collection of workset structures. To retrieve a workset use:
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesWorkset *ws = NULL;
	fiftyoneDegreesProviderWorksetGet(&provider);
	</pre></p>
	And to return a workset to the pool use:
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesWorksetRelease(ws);
	</pre></p>
</p>
<p>
	The benefit of the workset pool is that it eliminates the overheads of
	creating a new workset structure for every new request, instead an existing
	workset is used. Be sure to initialize the workset of the appropriate size
	as an insufficiently small workset could cause delay with processing the
	device detection requests as the thread is waiting for a the next available
	workset in the pool.
</p>
<p>
	The reload functionality works both with the single threaded as well as the
	multi threaded modes. To try the reload functionality in single threaded
	mode build with FIFTYONEDEGREES_NO_THREADING defined. Or build without
	FIFTYONEDEGREES_NO_THREADING for multi threaded example.
</p>
<p>
	In a single threaded environment the reload function is executed as part of
	the normal flow of the program execution and will prevent any other actions
	until the reload is complete. The reload itself takes less than half a
	second even for Enterprise dataset. For more information see:
	https://51degrees.com/Support/Documentation/APIs/C-V32/Benchmarks
</p>
</tutorial>
*/

#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#endif

#ifdef _DEBUG
#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#else
#include "dmalloc.h"
#endif
#endif

// Snippet Start
#include "../src/pattern/51Degrees.h"

// Global settings and properties.
static fiftyoneDegreesProvider provider;
#ifndef FIFTYONEDEGREES_NO_THREADING
static FIFTYONEDEGREES_THREAD *threads;
static const int numberOfThreads = 50;
static volatile int threadsFinished = 0;
FIFTYONEDEGREES_MUTEX lock;
#endif

// Function declarations.
static void reportDatasetInitStatus(
					fiftyoneDegreesDataSetInitStatus status,
					const char* fileName);
static unsigned long hash(unsigned char *value);
static unsigned long getHashCode(fiftyoneDegreesWorkset *ws);
#ifndef FIFTYONEDEGREES_NO_THREADING
static void startThreads(const char* inputFile);
static void stopThreads();
static void runRequests(void* inputFile);
#else
static int runRequest(const char *inputFile);
#endif

int main(int argc, char* argv[]) {

	// Required properties. Empty string initializes all properties.
	const char* requiredProperties = "IsMobile,BrowserName";

	// Path to 51Degrees data files. Or use default paths.
	const char* fileName = argc > 1 ? argv[1] :
		"../../../data/51Degrees-LiteV3.2.dat";
	// Path to file containing HTTP User-Agent strings.
	const char* inputFile = argc > 2 ? argv[2] :
		"../../../data/20000 User Agents.csv";

#ifdef _DEBUG
#ifndef _MSC_VER
	dmalloc_debug_setup("log-stats,log-non-free,check-fence,log=dmalloc.log");
#else
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
#endif
#endif

	int numberOfReloads = 0;

#ifndef FIFTYONEDEGREES_NO_THREADING
	printf("** Multi Threaded Reload Example **\r\n");
#else
	printf("** Single Threaded Reload Example **\r\n");
#endif

	// Create a pool of 4 worksets with a cache for 1000 items.
	fiftyoneDegreesDataSetInitStatus status =
		fiftyoneDegreesInitProviderWithPropertyString(
			fileName, &provider, requiredProperties, 4, 1000);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		reportDatasetInitStatus(status, fileName);
		fgetc(stdin);
		return 1;
	}

#ifndef FIFTYONEDEGREES_NO_THREADING
	FIFTYONEDEGREES_MUTEX_CREATE(lock);
	if (FIFTYONEDEGREES_MUTEX_VALID(&lock)) {
		startThreads(inputFile);
		while (threadsFinished < numberOfThreads) {
			status = fiftyoneDegreesProviderReloadFromFile(&provider);
			numberOfReloads++;
#ifdef _MSC_VER
			Sleep(1000); // milliseconds
#else
			sleep(1); // seconds
#endif
		}
		stopThreads();
		FIFTYONEDEGREES_MUTEX_CLOSE(lock);
	}
#else
	numberOfReloads = runRequest(inputFile);
#endif

	// Free the pool, dataset and cache.
	fiftyoneDegreesProviderFree(&provider);

	// Finish execution.
	printf("Reloaded '%i' times.\r\n", numberOfReloads);

#ifdef _DEBUG
#ifdef _MSC_VER
	_CrtDumpMemoryLeaks();
#else
	printf("Log file is %s\r\n", dmalloc_logpath);
#endif
#endif

	printf("Program execution complete. Press any Return to exit.");
	fgetc(stdin);

	return 0;
}

#ifndef FIFTYONEDEGREES_NO_THREADING

/**
 * Starts threads that run device detection. Must be done after the dataset,
 * cache and workset pool have been initialized.
 */
static void startThreads(const char* inputFile) {
	threads = (FIFTYONEDEGREES_THREAD*)malloc(sizeof(FIFTYONEDEGREES_THREAD) * numberOfThreads);
	int thread;
	for (thread = 0; thread < numberOfThreads; thread++) {
		FIFTYONEDEGREES_THREAD_CREATE(threads[thread], (void*)&runRequests, (void*)inputFile);
	}
}

/**
 * Stops the threads and frees the memory occupied by the threads.
 */
static void stopThreads() {
	int thread;
	for (thread = 0; thread < numberOfThreads; thread++) {
		FIFTYONEDEGREES_THREAD_JOIN(threads[thread]);
	}
	free(threads);
}

/**
 * Demonstrates the dataset, pool and cache reload functionality in a multi
 * threaded environment. When a workset is returned to the pool of worksets
 * a check is carried out to see if the pool is now inactive and all of the
 * worksets have been returned. If both conditions are met the pool is
 * freed along with the underlying dataset and cache.
 *
 * @param inputFile containing HTTP User-Agent strings.
 */
static void runRequests(void* inputFile) {
	fiftyoneDegreesWorkset *ws = NULL;
	unsigned long hashCode = 0;
	char userAgent[1000];
	FILE* fin = fopen((const char*)inputFile, "r");

	while (fgets(userAgent, sizeof(userAgent), fin) != NULL) {
		ws = fiftyoneDegreesProviderWorksetGet(&provider);
		fiftyoneDegreesMatch(ws, userAgent);
		hashCode ^= getHashCode(ws);
		fiftyoneDegreesWorksetRelease(ws);
	}

	fclose(fin);
	printf("Finished with hashcode '%lu'\r\n", hashCode);
	FIFTYONEDEGREES_MUTEX_LOCK(&lock);
	threadsFinished++;
	FIFTYONEDEGREES_MUTEX_UNLOCK(&lock);
}

#else

/**
 * Demonstrates the dataset, pool and cache reload functionality in a single
 * threaded environment. Since only one thread is available the reload will
 * be done as part of the program flow and detection will not be available for
 * the very short time that the dataset, pool and cache are being reloaded.
 *
 * The reload happens every 500 requests. The total number of dataset reloads
 * is then returned.
 *
 * @param inputFile containing HTTP User-Agent strings to use with device
 *		  detection.
 * @return number of times the dataset, pool and cache were reloaded.
 */
static int runRequest(const char *inputFile) {
	fiftyoneDegreesWorkset *ws = NULL;
	unsigned long hashCode = 0;
	int count = 0, numberOfReloads = 0;
	char userAgent[1000];
	FILE* fin = fopen((const char*)inputFile, "r");

	while (fgets(userAgent, sizeof(userAgent), fin) != NULL) {
		ws = fiftyoneDegreesProviderWorksetGet(&provider);
		fiftyoneDegreesMatch(ws, userAgent);
		hashCode ^= getHashCode(ws);
		fiftyoneDegreesWorksetRelease(ws);
		count++;
		if (count % 1000 == 0) {
			fiftyoneDegreesProviderReloadFromFile(&provider);
			numberOfReloads++;
		}
	}

	fclose(fin);
	printf("Finished with hashcode '%lu'\r\n", hashCode);
	return numberOfReloads;
}

#endif

/**
* Returns a basic hashcode for the string value provided.
* @param value string whose hashcode is required.
* @returns the hashcode for the string provided.
*/
unsigned long hash(unsigned char *value) {
	unsigned long hashCode = 5381;
	int i;
	while ((i = *value++)) {
		hashCode = ((hashCode << 5) + hashCode) + i;
	}
	return hashCode;
}


/**
 * Returns the hash code for the values of properties contained in the work
 * set.
 * @param ws work set containing the results of a match
 */
static unsigned long getHashCode(fiftyoneDegreesWorkset *ws) {
	unsigned long hashCode = 0;
	int32_t requiredPropertyIndex;
	const fiftyoneDegreesAsciiString *valueName;
	for (requiredPropertyIndex = 0;
		 requiredPropertyIndex < ws->dataSet->requiredPropertyCount;
		 requiredPropertyIndex++) {
		fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
		valueName = fiftyoneDegreesGetString(
			ws->dataSet,
			ws->values[0]->nameOffset);
		hashCode ^= hash((unsigned char*)&(valueName->firstByte));
	}
	return hashCode;
}

/**
 * Reports the status of the data file initialization.
 */
static void reportDatasetInitStatus(fiftyoneDegreesDataSetInitStatus status,
									const char* fileName) {
	switch (status) {
	case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
		printf("Insufficient memory to load '%s'.", fileName);
		break;
	case DATA_SET_INIT_STATUS_CORRUPT_DATA:
		printf("Device data file '%s' is corrupted.", fileName);
		break;
	case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
		printf("Device data file '%s' is not correct version.", fileName);
		break;
	case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
		printf("Device data file '%s' not found.", fileName);
		break;
	case DATA_SET_INIT_STATUS_NULL_POINTER:
		printf("Null pointer to the existing dataset or memory location.");
		break;
	case DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS:
		printf("Allocated continuous memory containing 51Degrees data file "
			"appears to be smaller than expected. Most likely because the"
			" data file was not fully loaded into the allocated memory.");
		break;
	default:
		printf("Device data file '%s' could not be loaded.", fileName);
		break;
	}
}

// Snippet End
