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
<li>Open an input file with a list of User-Agents, and an output file,
<p><code>
FILE* fin = fopen(inputFile, "r");<br>
FILE* fout = fopen(outputFile, "w");
</code></p>
<li>Write a header to the output file with the property names in '|'
separated CSV format ('|' sepparated because some User-Agents contain
commas)
<p><code>
fprintf(fout, "User-Agent");<br>
for (j=0;j<propertiesCount;j++) {<br>
	fprintf(fout, "|%s", propertiesArray[j]);<br>
}<br>
fprintf(fout, "\n");
</code></p>
<li>For the first 20 User-Agents in the input file, performa match then
write the User-Agent along with the values for chosen properties to
the CSV.
<p><code>
for (i=0;i<20;i++) {<br>
	fgets(userAgent, sizeof(userAgent), fin);<br>
	userAgent[strlen(userAgent)-1] = '\0';<br>
	fprintf(fout, "%s", userAgent);<br>
	fiftyoneDegreesMatch(ws, userAgent);<br>
	for (j=0;j<propertiesCount;j++) {<br>
		value = getValue(ws, propertiesArray[j]);<br>
		fprintf(fout, "|%s", value);<br>
	}<br>
	fprintf(fout, "\n");<br>
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
and city.c. This will happen automaticaly if you are compiling
as part of the Visual Studio solution. Additionaly, when running,
the location of a 51Degrees data file and an input file
(20000 User Agents.csv is included in the data folder) must be 
passed as command line arguments.
</tutorial>
*/

// Snippet Start
#include <stdio.h>
#include <string.h>
#include "../src/pattern/51Degrees.h"

fiftyoneDegreesWorkset *ws = NULL;
fiftyoneDegreesDataSet dataSet;
char *inputFile;
char *properties[3];

const char* getValue(fiftyoneDegreesWorkset* ws, char* propertyName);
void run(fiftyoneDegreesDataSet* dataSet, char* properties[], int propertiesCount, char *inputFile);

int main(int argc, char* argv[]) {
	properties[0] = "IsMobile";
	properties[1] = "PlatformName";
	properties[2] = "PlatformVersion";
	int propertiesCount = 3;
	if (argc > 1) {
		switch (fiftyoneDegreesInitWithPropertyArray(argv[1], &dataSet, properties, propertiesCount)) {
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
			inputFile = argv[2];
			run(&dataSet, properties, propertiesCount, inputFile);
			break;
		}
	}

	// Wait for a character to be pressed.
	fgetc(stdin);
}

void run(fiftyoneDegreesDataSet* dataSet, char* properties[], int propertiesCount, char *inputFile) {
    char userAgent[1000];
    const char* value;
    int i, j;

// Creates workset.
    ws = fiftyoneDegreesWorksetCreate(dataSet, NULL);

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

// Frees workset.
    fiftyoneDegreesWorksetFree(ws);

// Frees dataset.
    fiftyoneDegreesDataSetFree(dataSet);
}

/**
 * Returns a string representation of the value associated with the required
 * property name. If the property name is not valid an empty string is
 * returned.If the property relates to a list with more than one value then
 * values are seperated by | characters.
 * @param propertyName pointer to a string containing the property name
 * @returns a string representation of the value for the property
 */
const char* getValue(fiftyoneDegreesWorkset* ws, char* propertyName) {
    int requiredPropertyIndex;
    const char* result;
    const fiftyoneDegreesAsciiString* valueName;

    requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(ws->dataSet, propertyName);
    fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
    valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[0]->nameOffset);
    result = &(valueName->firstByte);
    return result;
}
// Snippet End
