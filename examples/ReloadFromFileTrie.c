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
Reload from file example that shows how to:
<ol>
<li>Use the fiftyoneDegreesDatasetReloadFromFile function to reload the
dataset, from the same location and with the same set of properties.
<li>Run detections on the current User-Agent.
<li>Use the reload functionality in a single threaded environment.
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
Example assumes that the initial dataset was created using
either the fiftyoneDegreesInitProviderWithPropertyString function,
or the fiftyoneDegreesInitProviderWithPropertyArray function.
</p>
<p>
The fiftyoneDegreesProviderReloadFromFile function requires an existing
initialized dataset. Function reloads the dataset from the same location
and with the same parameters as the original dataset.
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
#ifdef _DEBUG
#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#else
#include "dmalloc.h"
#endif
#endif

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#endif
// Snippet Start
#include "../src/trie/51Degrees.h"

// Global provider declaration.
static fiftyoneDegreesProvider provider;

// Function declarations.
static void reportDatasetInitStatus(
	fiftyoneDegreesDataSetInitStatus status,
	const char* fileName);
static unsigned long hash(unsigned char *value);
static unsigned long getHashCode(fiftyoneDegreesDeviceOffsets *offsets);
static int runRequest(const char *inputFile);

int main(int argc, char* argv[]) {

	// Required properties. Empty string initializes all properties.
	const char* requiredProperties = "IsMobile,BrowserName";

	// Path to 51Degrees data files. Or use default paths.
	const char* fileName = argc > 1 ? argv[1] :
		"../../../data/51Degrees-LiteV3.2.trie";
	// Path to file containing HTTP User-Agent strings.
	const char* inputFile = argc > 2 ? argv[2] :
		"../../../data/20000 User Agents.csv";

#ifdef _DEBUG
#ifndef _MSC_VER
	dmalloc_debug_setup("log-stats,log-non-free,check-fence,log=dmalloc.log");
#endif
#endif

	int numberOfReloads = 0;

	printf("** Single Threaded Reload Example **\r\n");


	// Initialise the provider.
	fiftyoneDegreesDataSetInitStatus status = fiftyoneDegreesInitProviderWithPropertyString(fileName, &provider, requiredProperties);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		reportDatasetInitStatus(status, fileName);
		fgetc(stdin);
		return 1;
	}

	numberOfReloads = runRequest(inputFile);

	// Free the provider.
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

/**
* Demonstrates the dataset reload functionality in a single
* threaded environment. Since only one thread is available the reload will
* be done as part of the program flow and detection will not be available for
* the very short time that the dataset is being reloaded.
*
* The reload happens every 500 requests. The total number of dataset reloads
* is then returned.
*
* @param inputFile containing HTTP User-Agent strings to use with device
*		  detection.
* @return number of times the dataset, pool and cache were reloaded.
*/
static int runRequest(const char *inputFile) {
	fiftyoneDegreesDeviceOffsets *offsets;
	unsigned long hashCode = 0;
	int count = 0, numberOfReloads = 0;
	char userAgent[1000];
	FILE* fin = fopen((const char*)inputFile, "r");

	while (fgets(userAgent, sizeof(userAgent), fin) != NULL) {
		offsets = fiftyoneDegreesCreateDeviceOffsets(provider.active->dataSet);
		offsets->size = 1;
		fiftyoneDegreesSetDeviceOffset(provider.active->dataSet, userAgent, 0, offsets->firstOffset);
		hashCode ^= getHashCode(offsets);
		fiftyoneDegreesFreeDeviceOffsets(offsets);
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
* Returns the hash code for the values of properties contained in the offsets.
* @param offsets containing the results of a match
*/
static unsigned long getHashCode(fiftyoneDegreesDeviceOffsets *offsets) {
	unsigned long hashCode = 0;
	int32_t requiredPropertyIndex;
	const char *valueName;
	for (requiredPropertyIndex = 0;
		requiredPropertyIndex < provider.active->dataSet->requiredPropertiesCount;
		requiredPropertyIndex++) {
		valueName = fiftyoneDegreesGetValuePtrFromOffsets(provider.active->dataSet, offsets, requiredPropertyIndex);
		hashCode ^= hash((unsigned char*)&(valueName));
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
	default:
		printf("Device data file '%s' could not be loaded.", fileName);
		break;
	}
}

// Snippet End
