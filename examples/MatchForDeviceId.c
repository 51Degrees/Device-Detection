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
Match for device id example of using 51Degrees device detection. The example
shows how to:
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
<li>Produce a match for a single device id
<p><code>
fiftyoneDegreesMatchForDeviceId(ws, DeviceId);
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
and city.c.
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
	const char* isMobile;

// Device id string of an iPhone mobile device.
const char* mobileDeviceId = "12280-48866-24305-18092";

// Device id string of Firefox Web browser version 41 on dektop.
const char* desktopDeviceId = "15364-21460-53251-18092";

// Device id string of a MediaHub device.
const char* mediaHubDeviceId = "41231-46303-24154-18092";


printf("Starting Match For Device Id Example.\n");

// Creates a workset.
ws = fiftyoneDegreesWorksetCreate(dataSet, NULL);

// Carries out a match for a mobile device id.
printf("\nMobileDeviceId: %s\n", mobileDeviceId);
fiftyoneDegreesMatchForDeviceId(ws, mobileDeviceId);
isMobile = getIsMobile(ws);
printf("   IsMobile: %s\n", isMobile);

// Carries out a match for a desktop device id.
printf("\nDesktopDeviceId: %s\n", desktopDeviceId);
fiftyoneDegreesMatchForDeviceId(ws, desktopDeviceId);
isMobile = getIsMobile(ws);
printf("   IsMobile: %s\n", isMobile);

// Carries out a match for a MediaHub device id.
printf("\nMediHub Device Id: %s\n", mediaHubDeviceId);
fiftyoneDegreesMatchForDeviceId(ws, mediaHubDeviceId);
isMobile = getIsMobile(ws);
printf("   IsMobile: %s\n", isMobile);

// Frees the workset.
fiftyoneDegreesWorksetFree(ws);

// Frees the dataset.
fiftyoneDegreesDataSetFree(dataSet);
}

/**
 * Returns a string representation of the value associated with the IsMobile
 * property.
 * @param initialised workset of type fiftyoneDegreesWorkset
 * @returns a string representation of the value for IsMobile
 */
const char* getIsMobile(fiftyoneDegreesWorkset* ws) {
    int requiredPropertyIndex;
    const char* isMobile;
    const fiftyoneDegreesAsciiString* valueName;

    requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(ws->dataSet, "IsMobile");
    fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
    valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[0]->nameOffset);
    isMobile = &(valueName->firstByte);
    return isMobile;
}
// Snippet End
