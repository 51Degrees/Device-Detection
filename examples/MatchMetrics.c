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
Match metrics example of using 51Degrees device detection.
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
	<li>Obtain match method: provides information about the
	algorithm that was used to perform detection for a particular User-Agent.
	For more information on what each method means please see:
	<a href="https://51degrees.com/support/documentation/pattern">
	How device detection works</a>
	<p><pre class="prettyprint lang-c">ws->method</pre></p>
	<li>Obtain difference:  used when detection method is not Exact or None.
	This is an integer value and the larger the value the less confident the
	detector is in this result.
	<p>
	<pre class="prettyprint lang-c">
	ws->difference</pre>
	</p>
	<li>Obtain signature rank: an integer value that indicates how popular
	the device is. The lower the rank the more popular the signature.
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesGetSignatureRank();
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
void output_match_metrics(fiftyoneDegreesWorkset* ws);
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

    printf("Starting Getting Started Match Metrics Example\n");

	// Get a workset from the pool to perform this match.
	ws = fiftyoneDegreesProviderWorksetGet(provider);

    // Carries out a match with a mobile User-Agent.
    printf("\nUser-Agent: %s\n", mobileUserAgent);
    fiftyoneDegreesMatch(ws, mobileUserAgent);
    output_match_metrics(ws);

	// Release workset after match complete and workset no longer required.
	fiftyoneDegreesWorksetRelease(ws);

	// Get a workset from the pool to perform this match.
	ws = fiftyoneDegreesProviderWorksetGet(provider);

    // Carries out a match with a desktop User-Agent.
    printf("\nUser-Agent: %s\n", desktopUserAgent);
    fiftyoneDegreesMatch(ws, desktopUserAgent);
    output_match_metrics(ws);

	// Release workset after match complete and workset no longer required.
	fiftyoneDegreesWorksetRelease(ws);

	// Get a workset from the pool to perform this match.
	ws = fiftyoneDegreesProviderWorksetGet(provider);

    // Carries out a match with a MediaHub User-Agent.
    printf("\nUser-Agent: %s\n", mediaHubUserAgent);
    fiftyoneDegreesMatch(ws, mediaHubUserAgent);
    output_match_metrics(ws);

	// Release workset after match complete and workset no longer required.
	fiftyoneDegreesWorksetRelease(ws);
}

/**
 * Gets device id for given match and prints match metrics
 * associated with it.
 * @param initialised workset of type fiftyoneDegreesWorkset
 */
void output_match_metrics(fiftyoneDegreesWorkset* ws) {
    char deviceId[40];
    fiftyoneDegreesGetDeviceId(ws, deviceId, 40);

    int method = ws->method;
    int difference = ws->difference;
    int rank = fiftyoneDegreesGetSignatureRank(ws);

    printf("   Id: %s\n", deviceId);
    printf("   Method: %d\n", method);
    printf("   Difference: %d\n", difference);
    printf("   Rank: %d\n", rank);
    return;
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
