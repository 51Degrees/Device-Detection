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
and city.c.
</tutorial>
*/

// Snippet Start
#include <stdio.h>
#include <string.h>
#include "../src/pattern/51Degrees.h"

fiftyoneDegreesWorkset *ws = NULL;
fiftyoneDegreesDataSet dataSet;

const char* getValue(fiftyoneDegreesWorkset* ws, char* propertyName);

int main(int argc, char* argv[]) {

    const char* fileName = "../data/51Degrees-LiteV3.2.dat";
    char properties[] = "IsMobile,PlatformName,PlatformVersion";
    char *propertiesArray[1000];
    char userAgent[1000];
    const char* value;
    int i, j;

/**
 * Initialises the device detection dataset with the above settings. 
 * This uses the Lite data file For more info
 * see:
 * <a href="https://51degrees.com/compare-data-options">compare data options
 * </a>
 */
    fiftyoneDegreesInitWithPropertyString(fileName, &dataSet, properties);

// Creates workset.
    ws = fiftyoneDegreesWorksetCreate(&dataSet, NULL);

    printf("Starting Offline Processing Example.\n");

// Opens input and output files.
    char* inputFile = "../data/20000 User Agents.csv";
    char* outputFile = "offlineProcessingOutput.csv";
    FILE* fin = fopen(inputFile, "r");
    FILE* fout = fopen(outputFile, "w");

// Converts properties list to array.
    char *tok = strtok(properties, ",");
    while (tok != NULL) {
        propertiesArray[i++] = tok;
        tok = strtok(NULL, ",");
    }
    int propertiesCount = i;

// Print CSV headers to output file.
    fprintf(fout, "User-Agent");
    for (j=0;j<propertiesCount;j++) {
        fprintf(fout, "|%s", propertiesArray[j]);
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
            value = getValue(ws, propertiesArray[j]);
            fprintf(fout, "|%s", value);
        }
        fprintf(fout, "\n");
    }

    printf("Output Written to %s\n", outputFile);

// Frees workset.
    fiftyoneDegreesWorksetFree(ws);

// Frees dataset.
    fiftyoneDegreesDataSetFree(&dataSet);
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
