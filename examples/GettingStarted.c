/**
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright (c) 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
 */
/*
<tutorial>
Getting started example of using 51Degrees device detection.
The example shows how to:
<ol>
	<li>Specify name of the data file and properties the dataset should be 
	initialised with.
	<p><pre class="prettyprint lang-c">
	const char* fileName = argv[1];
	const char* properties = "IsMobile";
	</pre></p>
	<li>Instantiate the 51Degrees provider from the specified data file with 
	the required properties, number of worksets in the pool and cache of the 
	specific size.
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesInitProviderWithPropertyString(
	fileName, &provider, properties, 4, 1000);
	</pre></p>
	<li>Retrieve a workset from the pool and use it for a single match.
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesWorkset *ws = NULL;
	ws = fiftyoneDegreesProviderWorksetGet(&provider);
	</pre></p>
	<li>Match a single HTTP User-Agent string to retrieve the values 
	associated with the User-Agent for the selected properties.
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesMatch(ws, userAgent);
	</pre></p>
	<li>Extract the value of the IsMobile property.
	<p><pre class="prettyprint lang-c">
	requiredPropertyIndex = 
	fiftyoneDegreesGetRequiredPropertyIndex(ws->dataSet,"IsMobile");
	fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
	valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[0]->nameOffset);
	isMobile = &(valueName->firstByte);
	</pre></p>
	<li>Release the workset back into the pool of worksets to be reused in one 
	of the next matches.
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesWorksetRelease(ws);
	</pre></p>
	<li>Finally release the memory taken by the provider.
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesProviderFree(&provider);
	</pre></p>
</ol>
<p>
	This example assumes you have compiled with 51Degrees.c and city.c. 
	This will happen automatically if you are compiling as part of the 
	Visual Studio solution. Additionally, when running the program, the 
	location of a 51Degrees data file must be passed as a command line 
	argument if you wish to use Premium or Enterprise data files.
</p>
<p>
	The size of the worksets pool parameter should be set to the maximum 
	(expected) number of concurrent detections to avoid delays related to 
	waiting for free worksets. Workset pool is thread safe. Initially the 
	number of created worksets in the pool is zero. When a workset is 
	retrieved from the pool a new workset is created if no worksets are 
	currently free and the number of worksets already created is less than 
	the maximum size of the workset pool.
</p>
</tutorial>
*/

#ifdef _DEBUG
#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include "dmalloc.h"
#endif
#endif

// Snippet Start
#include <stdio.h>
#include "../src/pattern/51Degrees.h"

// Global settings and properties.
static fiftyoneDegreesProvider provider;

// Function declarations.
static void reportDatasetInitStatus(
	fiftyoneDegreesDataSetInitStatus status,
	const char* fileName);

const char* getIsMobile(fiftyoneDegreesWorkset* ws);
void run(fiftyoneDegreesProvider* provider);

int main(int argc, char* argv[]) {
	const char* properties = "IsMobile";
	const char* fileName = argc > 1 ? argv[1] : "../../../data/51Degrees-LiteV3.2.dat";
	
#ifdef _DEBUG
#ifndef _MSC_VER
	dmalloc_debug_setup("log-stats,log-non-free,check-fence,log=dmalloc.log");
#endif
#endif

	// Create a pool of 4 worksets with a cache for 1000 items.
	fiftyoneDegreesDataSetInitStatus status =
		fiftyoneDegreesInitProviderWithPropertyString(
		fileName, &provider, properties, 4, 1000);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		reportDatasetInitStatus(status, fileName);
		fgetc(stdin);
		return 1;
	}

	run(&provider);

	// Free the pool, dataset and cache.
	fiftyoneDegreesProviderFree(&provider);

#ifdef _DEBUG
#ifdef _MSC_VER
	_CrtDumpMemoryLeaks();
#else
	printf("Log file is %s\r\n", dmalloc_logpath);
#endif
#endif

	// Wait for a character to be pressed.
	fgetc(stdin);

	return 0;
}

void run(fiftyoneDegreesProvider* provider) {
	const char* isMobile;
	fiftyoneDegreesWorkset *ws = NULL;

	// User-Agent string of an iPhone mobile device.
	const char* mobileUserAgent = ("Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) "
		"AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 "
		"Safari/9537.53");

	// User-Agent string of Firefox Web browser version 41 on desktop.
	const char* desktopUserAgent = ("Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) "
		"Gecko/20100101 Firefox/41.0");

	// User-Agent string of a MediaHub device.
	const char* mediaHubUserAgent = ("Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core "
		"Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 "
		"Chrome/30.0.0.0 Safari/537.36");

	printf("Starting Getting Started Example.\n");

	// Get a workset from the pool to perform this match.
	ws = fiftyoneDegreesProviderWorksetGet(provider);

	printf("\nMobile User-Agent: %s\n", mobileUserAgent);
	// Carries out a match for a mobile User-Agent.
	fiftyoneDegreesMatch(ws, mobileUserAgent);
	isMobile = getIsMobile(ws);
	printf("   IsMobile: %s\n", isMobile);

	// Release workset after match complete and workset no longer required.
	fiftyoneDegreesWorksetRelease(ws);

	// Get a workset from the pool to perform this match.
	ws = fiftyoneDegreesProviderWorksetGet(provider);

	// Carries out a match for a desktop User-Agent.
	printf("\n Desktop User-Agent: %s\n", desktopUserAgent);
	fiftyoneDegreesMatch(ws, desktopUserAgent);
	isMobile = getIsMobile(ws);
	printf("   IsMobile: %s\n", isMobile);

	// Release workset after match complete and workset no longer required.
	fiftyoneDegreesWorksetRelease(ws);

	// Get a workset from the pool to perform this match.
	ws = fiftyoneDegreesProviderWorksetGet(provider);

	// Carries out a match for a MediaHub User-Agent.
	printf("\n Media hub User-Agent: %s\n", mediaHubUserAgent);
	fiftyoneDegreesMatch(ws, mediaHubUserAgent);
	isMobile = getIsMobile(ws);
	printf("   IsMobile: %s\n", isMobile);

	// Release workset after match complete and workset no longer required.
	fiftyoneDegreesWorksetRelease(ws);
}

/**
 * Returns a string representation of the value associated with the IsMobile
 * property.
 * @param initialised workset of type fiftyoneDegreesWorkset
 * @returns a string representation of the value for IsMobile
 */
const char* getIsMobile(fiftyoneDegreesWorkset* ws) {
    int32_t requiredPropertyIndex;
    const char* isMobile;
    const fiftyoneDegreesAsciiString* valueName;
	const char* property = "IsMobile";

    requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(ws->dataSet, property);
    fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
    valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[0]->nameOffset);
    isMobile = &(valueName->firstByte);
    return isMobile;
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
