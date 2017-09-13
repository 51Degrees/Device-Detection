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
	<li>Retrieve all the profiles from the data set which match a specified 
	property value pair.
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesProfilesStruct *profiles
	= provider.findProfiles("IsMobile", "True");
	</pre></p>
	<li>Free the memory taken by the profiles structure
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesFreeProfilesStruct(profiles);
	</pre><p>
	<li>Finally release the memory taken by the provider
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesDataSetFree(&dataSet);
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
	fiftyoneDegreesProfilesStruct *mobileProfiles, *nonMobileProfiles;
	printf("Starting Find Profiles Example.\n\n");

	// Retieve all the mobile profiles in the data set.
	mobileProfiles = 
		fiftyoneDegreesFindProfiles(provider->activePool->dataSet, "IsMobile", "True");
	printf("There are '%d' mobile profiles in the '%s' data set.\n", 
		mobileProfiles->count, 
		&fiftyoneDegreesGetString(provider->activePool->dataSet, 
			provider->activePool->dataSet->header.nameOffset)->firstByte);

	// Retrieve all the non-mobile profiles in the data set.
	nonMobileProfiles = 
		fiftyoneDegreesFindProfiles(provider->activePool->dataSet, "IsMobile", "False");
	printf("There are '%d' non-mobile profiles in the '%s' data set.\n", 
		nonMobileProfiles->count,
		&fiftyoneDegreesGetString(provider->activePool->dataSet, 
			provider->activePool->dataSet->header.nameOffset)->firstByte);

	// Free the profiles structures.
	fiftyoneDegreesFreeProfilesStruct(mobileProfiles);
	fiftyoneDegreesFreeProfilesStruct(nonMobileProfiles);
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
