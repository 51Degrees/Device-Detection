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
Offline processing example of using 51Degrees device detection.
The example shows how to:
<ol>
	<li>Specify name of the data file and properties the dataset should be 
	initialised with.
	<p><pre class="prettyprint lang-c">
	const char* fileName = argv[1];
	const char* properties = "IsMobile";
	</pre></p>
	<li>Instantiate the 51Degrees device detection provider using an array of 
	properties as opposed to a string containing coma separated properties.
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesInitProviderWithPropertyArray(
		fileName, &provider, properties, propertiesCount, 4, 1000);
	</pre></p>
	<li>Retrieve a workset from the pool and use it for a single match.
	<p><pre class="prettyprint lang-c">
	fiftyoneDegreesWorkset *ws = NULL;
	ws = fiftyoneDegreesProviderWorksetGet(&provider);
	</pre></p>
	<li>Open an input file with a list of User-Agents, and an output file,
	<p><pre class="prettyprint lang-c">
	FILE* fin = fopen(inputFile, "r");
	FILE* fout = fopen(outputFile, "w");
	</pre></p>
	<li>Write a header to the output file with the property names in '|'
	separated CSV format ('|' separated because some User-Agents contain
	commas)
	<p><pre class="prettyprint lang-c">
	fprintf(fout, "User-Agent");
	for (j=0;j&lt;propertiesCount;j++) {
		fprintf(fout, "|%s", propertiesArray[j]);
	}
	fprintf(fout, "\n");
	</pre></p>
	<li>For the first 20 User-Agents in the input file, perform a match then
	write the User-Agent along with the values for chosen properties to
	the CSV.
	<p><pre class="prettyprint lang-c">
	for (i=0;i&lt;20;i++) {
		fgets(userAgent, sizeof(userAgent), fin);
		userAgent[strlen(userAgent)-1] = '\0';
		fprintf(fout, "%s", userAgent);
		fiftyoneDegreesMatch(ws, userAgent);
		for (j=0;j&lt;propertiesCount;j++) {
			value = getValue(ws, propertiesArray[j]);
			fprintf(fout, "|%s", value);
		}
		fprintf(fout, "\n");
	}
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
	This example demonstrates one possible use of the API and device data for 
	the offline data processing. It also demostrates that you can reuse the 
	retrieved workset for multiple uses and only then return it to the pool.
</p>
</tutorial>
*/

#include <stdio.h>
#include <string.h>
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
#include "../src/pattern/51Degrees.h"

// Global settings and properties.
static fiftyoneDegreesProvider provider;
char *properties[3];

// Function declarations.
static void reportDatasetInitStatus(
	fiftyoneDegreesDataSetInitStatus status,
	const char* fileName);
const char* getValue(fiftyoneDegreesWorkset* ws, char* propertyName);
void run(fiftyoneDegreesProvider* provider, char* properties[],
	int propertiesCount, const char *inputFile);

int main(int argc, char* argv[]) {
	properties[0] = "IsMobile";
	properties[1] = "PlatformName";
	properties[2] = "PlatformVersion";
	int propertiesCount = 3;
	const char* fileName = argc > 1 ? argv[1] : "../../../data/51Degrees-LiteV3.2.dat";
	const char* inputFile = argc > 2 ? argv[2] : "../../../data/20000 User Agents.csv";

#ifdef _DEBUG
#ifndef _MSC_VER
	dmalloc_debug_setup("log-stats,log-non-free,check-fence,log=dmalloc.log");
#endif
#endif

	// Create a pool of 4 worksets with a cache for 1000 items.
	fiftyoneDegreesDataSetInitStatus status =
		fiftyoneDegreesInitProviderWithPropertyArray(
		fileName, &provider, (const char**)properties, propertiesCount, 4, 1000);
	if (status != DATA_SET_INIT_STATUS_SUCCESS) {
		reportDatasetInitStatus(status, fileName);
		fgetc(stdin);
		return 1;
	}

	run(&provider, properties, propertiesCount, inputFile);

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

void run(fiftyoneDegreesProvider* provider, char* properties[],
         int propertiesCount, const char *inputFile) {
	char userAgent[1000];
	const char* value;
	int i, j;
	fiftyoneDegreesWorkset *ws = NULL;

	// Get a workset from the pool to perform this match.
	ws = fiftyoneDegreesProviderWorksetGet(provider);

	printf("Starting Offline Processing Example.\n");

	// Opens input and output files.
	char* outputFile = "offlineProcessingOutput.csv";
	FILE* fin = fopen(inputFile, "r");
	FILE* fout = fopen(outputFile, "w");

	// Print CSV headers to output file.
	fprintf(fout, "User-Agent");
	for (j=0;j<propertiesCount;j++) {
		fprintf(fout, "|%s", properties[j]);
	}
	fprintf(fout, "\n");

	// Carries out match for first 20 User-Agents and prints results to
	// output file.
	for (i=0;i<20;i++) {
		fgets(userAgent, sizeof(userAgent), fin);
		userAgent[strlen(userAgent)-1] = '\0';
		fprintf(fout, "%s", userAgent);
		fiftyoneDegreesMatch(ws, userAgent);
		for (j=0;j<propertiesCount;j++) {
			value = getValue(ws, properties[j]);
			fprintf(fout, "|%s", value);
		}
		fprintf(fout, "\n");
	}

	printf("Output Written to %s\n", outputFile);

	// Release workset after match complete and workset no longer required.
	fiftyoneDegreesWorksetRelease(ws);
}

/**
 * Returns a string representation of the value associated with the required
 * property name. If the property name is not valid an empty string is
 * returned.If the property relates to a list with more than one value then
 * values are separated by | characters.
 * @param propertyName pointer to a string containing the property name
 * @returns a string representation of the value for the property
 */
const char* getValue(fiftyoneDegreesWorkset* ws, char* propertyName) {
	int requiredPropertyIndex;
	const char* result;
	const fiftyoneDegreesAsciiString* valueName;

	requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(ws->dataSet, propertyName);
	if (requiredPropertyIndex != -1) {
		fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
		valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[0]->nameOffset);
		result = &(valueName->firstByte);
		return result;
	}
	else {
		return "";
	}
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
