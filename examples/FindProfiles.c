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
<p><pre class="prettyprint lang-c">
const char* fileName = argv[1];
const char* properties = "IsMobile";
</pre></p>
<li>Instantiate the 51Degrees device detection provider with these
properties
<p><pre class="prettyprint lang-c">
fiftyoneDegreesInitWithPropertyString(fileName, &dataSet, properties);
</pre></p>
<li>Retrive all the profiles from the data set which match
a specified property value pair
<p><pre class="prettyprint lang-c">
fiftyoneDegreesProfilesStruct *profiles
= fiftyoneDegreesFindProfiles(dataSet, "IsMobile", "True");
</pre></p>
<li>Search within a list of profiles for another property value pair.
<p><pre class="prettyprint lang-c">
fiftyoneDegreesProfilesStruct *profiles1080
= fiftyoneDegreesFindProfilesInProfiles(dataSet, "ScreenPixelsWidth",
"1080", profiles);
</pre></p>
<li>Free the memory taken by the profiles structure
<p><pre class="prettyprint lang-c">
fiftyoneDegreesFreeProfilesStruct(profiles);
</pre><p>
<li>Finally release the memory taken by the dataset
<p><pre class="prettyprint lang-c">
fiftyoneDegreesDataSetFree(&dataSet);
</pre></p>
</ol>
This example assumes you have compiled with 51Degrees.c
and city.c. This will happen automatically if you are compiling
as part of the Visual Studio solution. Additionally, when running,
the location of a 51Degrees data file must be passed as a
command line argument if you wish to use Premium or Enterprise
data files.
</tutorial>
*/

// Snippet Start
#include <stdio.h>
#include <string.h>
#include "../src/pattern/51Degrees.h"

fiftyoneDegreesDataSet dataSet;

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
	printf("Starting Find Profiles Example.\n\n");

	// Retieve all the mobile profiles in the data set.
	fiftyoneDegreesProfilesStruct *mobileProfiles = fiftyoneDegreesFindProfiles(dataSet, "IsMobile", "True");
	printf("There are %d mobile profiles in the Lite data set.\n", mobileProfiles->count);
	// Find how many have a screen width of 1080 pixels.
	fiftyoneDegreesProfilesStruct *mobile1080Profiles = fiftyoneDegreesFindProfilesInProfiles(dataSet, "ScreenPixelsWidth", "1080", mobileProfiles);
	printf("%d of them have a screen width of 1080 pixels.\n", mobile1080Profiles->count);

	// Retrieve all the non-mobile profiles in the data set.
	fiftyoneDegreesProfilesStruct *nonMobileProfiles = fiftyoneDegreesFindProfiles(dataSet, "IsMobile", "False");
	printf("There are %d non-mobile profiles in the Lite data set.\n", nonMobileProfiles->count);
	// Find how many have a screen width of 1080 pixels.
	fiftyoneDegreesProfilesStruct *nonMobile1080Profiles = fiftyoneDegreesFindProfilesInProfiles(dataSet, "ScreenPixelsWidth", "1080", nonMobileProfiles);
	printf("%d of them have a screen width of 1080 pixels.\n", nonMobile1080Profiles->count);

	// Free the profiles structures.
	fiftyoneDegreesFreeProfilesStruct(mobileProfiles);
	fiftyoneDegreesFreeProfilesStruct(nonMobileProfiles);
	fiftyoneDegreesFreeProfilesStruct(mobile1080Profiles);
	fiftyoneDegreesFreeProfilesStruct(nonMobile1080Profiles);

	// Free the data set.
	fiftyoneDegreesDataSetFree(dataSet);
}

// Snippet End
