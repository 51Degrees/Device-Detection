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
<li>Set the various settings for 51Degrees detector
<p><code>
const char* fileName = argv[1];<br>
const char* properties = "IsMobile";
</code></p>
<li>Instantiate the 51Degrees device detection provider with these
properties
<p><code>
fiftyoneDegreesInitWithPropertyString(fileName, &dataSet, properties);
</code></p>
<li>Create a workset with which to find a match
<p><code>
ws = fiftyoneDegreesWorksetCreate(&dataSet, NULL);
</code></p>
<li>Produce a match for a single HTTP User-Agent
<p><code>
fiftyoneDegreesMatch(ws, userAgent);
</code></p>
<li>Extract the value of the IsMobile property
<p><code>
requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(ws->dataSet,
 "IsMobile");<br>
fiftyoneDegreesSetValues(ws, requiredPropertyIndex);<br>
valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[0]->nameOffset);
<br>
isMobile = &(valueName->firstByte);
</code></p>
<li>Release the memory taken by the workset
<p><code>
fiftyoneDegreesWorksetFree(ws);
</code></p>
<li>Finaly release the memory taken by the dataset
<p><code>
fiftyoneDegreesDataSetFree(&dataSet);
</code></p>
</ol>
This example assumes you have compiled with 51Degrees.c
and city.c. This will happen automaticaly if you are compiling
as part of the Visual Studio solution. Additionaly, when running,
the location of a 51Degrees data file must be passed as a 
command line argument if you wish to use premium or enterprise
data files.
</tutorial>
*/

// Snippet Start
#include <stdio.h>
#include <string.h>
#include "../src/pattern/51Degrees.h"

fiftyoneDegreesWorkset *ws = NULL;
fiftyoneDegreesDataSet dataSet;

const char* getIsMobile(fiftyoneDegreesWorkset* ws);
void run(fiftyoneDegreesDataSet* dataSet);

int main(int argc, char* argv[]) {
	const char* properties = "IsMobile";
	const char* fileName = argc > 1 ? argv[1] : "../../../data/51Degrees-LiteV3.2.dat";

	/**
	* Initialises the device detection dataset with the above settings.
	* This uses the Lite data file For more info
	* see:
	* <a href="https://51degrees.com/compare-data-options">compare data options
	* </a>
	*/
	if (fileName != NULL) {
		switch (fiftyoneDegreesInitWithPropertyString(fileName, &dataSet, properties)) {
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
		case DATA_SET_INIT_STATUS_NOT_SET:
			printf("Device data file '%s' could not be loaded.", fileName);
			break;
		default:
			run(&dataSet);
			break;
		}
	}

	// Wait for a character to be pressed.
	fgetc(stdin);
}

void run(fiftyoneDegreesDataSet* dataSet) {
	const char* isMobile;

	// User-Agent string of an iPhone mobile device.
	const char* mobileUserAgent = ("Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) "
		"AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 "
		"Safari/9537.53");

	// User-Agent string of Firefox Web browser version 41 on dektop.
	const char* desktopUserAgent = ("Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) "
		"Gecko/20100101 Firefox/41.0");

	// User-Agent string of a MediaHub device.
	const char* mediaHubUserAgent = ("Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core "
		"Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 "
		"Chrome/30.0.0.0 Safari/537.36");

	printf("Starting Getting Started Example.\n");

	// Creates a workset.
	ws = fiftyoneDegreesWorksetCreate(dataSet, NULL);

	printf("\nUser-Agent: %s\n", mobileUserAgent);

	// Carries out a match for a mobile User-Agent.
	fiftyoneDegreesMatch(ws, mobileUserAgent);
	isMobile = getIsMobile(ws);
	printf("   IsMobile: %s\n", isMobile);

	// Carries out a match for a desktop User-Agent.
	printf("\nUser-Agent: %s\n", desktopUserAgent);
	fiftyoneDegreesMatch(ws, desktopUserAgent);
	isMobile = getIsMobile(ws);
	printf("   IsMobile: %s\n", isMobile);

	// Carries out a match for a MediaHub User-Agent.
	printf("\nUser-Agent: %s\n", mediaHubUserAgent);
	fiftyoneDegreesMatch(ws, mediaHubUserAgent);
	isMobile = getIsMobile(ws);
	printf("   IsMobile: %s\n", isMobile);

	// Frees workset.
	fiftyoneDegreesWorksetFree(ws);

	// Frees dataset.
	fiftyoneDegreesDataSetFree(dataSet);
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
// Snippet End
