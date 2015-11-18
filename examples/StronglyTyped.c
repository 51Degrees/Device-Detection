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
Strongly Typed example of using 51Degrees device detection. 
The example shows how to:
<ol>
<li>Set the various settings for 51Degrees detector
<p><code>
const char* fileName = "../data/51Degrees-LiteV3.2.dat";<br>
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
<li>Extract the boolean value of the IsMobile property
<p><code>
requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex
(ws->dataSet, "IsMobile");<br>
fiftyoneDegreesSetValues(ws, requiredPropertyIndex);<br>
valueName = fiftyoneDegreesGetString(ws->dataSet, 
ws->values[0]->nameOffset);<br>
isMobile = &(valueName->firstByte);<br>
if (strcmp(isMobile, "True") == 0) {<br>
	return true;<br>
}<br>
else {<br>
	return false;<br>
}
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
and city.c.
</tutorial>
*/

// Snippet Start
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../src/pattern/51Degrees.h"

fiftyoneDegreesDataSet dataSet;
fiftyoneDegreesWorkset *ws = NULL;

void run(fiftyoneDegreesDataSet* dataSet);
bool getIsMobileBool(fiftyoneDegreesWorkset* ws);

int main(int argc, char* argv[]) {
    const char* properties = "IsMobile";
	if (argc > 1) {
		switch (fiftyoneDegreesInitWithPropertyString(argv[1], &dataSet, properties)) {
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
		case DATA_SET_INIT_STATUS_NOT_SET:
			printf("Device data file '%s' could not be loaded.", argv[1]);
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
	bool isMobileBool;

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

    printf("Starting Getting Started Strongly Typed Example\n");

// Created workset.
    ws = fiftyoneDegreesWorksetCreate(dataSet, NULL);

// Carries out a match with a mobile User-Agent.
    printf("\nUser-Agent: %s\n", mobileUserAgent);
    fiftyoneDegreesMatch(ws, mobileUserAgent);
    isMobileBool = getIsMobileBool(ws);
    if (isMobileBool){
        printf("Mobile\n");
    }
    else {
        printf("Non-Mobile\n");
    }

// Carries out a match with a desktop User-Agent.
    printf("\nUser-Agent: %s\n", desktopUserAgent);
    fiftyoneDegreesMatch(ws, desktopUserAgent);
    isMobileBool = getIsMobileBool(ws);
    if (isMobileBool){
        printf("Mobile\n");
    }
    else {
        printf("Non-Mobile\n");
    }

// Carries out a match with a MediaHub User-Agent.
    printf("\nUser-Agent: %s\n", mediaHubUserAgent);
    fiftyoneDegreesMatch(ws, mediaHubUserAgent);
    isMobileBool = getIsMobileBool(ws);
    if (isMobileBool){
        printf("Mobile\n");
    }
    else {
        printf("Non-Mobile\n");
    }

// Frees the workset.
    fiftyoneDegreesWorksetFree(ws);

// Frees the dataset.
    fiftyoneDegreesDataSetFree(dataSet);
}

/**
 * Returns a boolean representation of the value associated with the IsMobile
 * property.
 * @param initialised workset of type fiftyoneDegreesWorkset
 * @returns a boolean representation of the value for IsMobile
 */
bool getIsMobileBool(fiftyoneDegreesWorkset* ws) {
    int requiredPropertyIndex;
    const char* isMobile;
    const fiftyoneDegreesAsciiString* valueName;

    requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(ws->dataSet, "IsMobile");
    fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
    valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[0]->nameOffset);
    isMobile = &(valueName->firstByte);
    if (strcmp(isMobile, "True") == 0) {
        return true;
    }
    else {
    	return false;
    }
}

// Snippet End
