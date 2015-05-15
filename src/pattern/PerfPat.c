#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "51Degrees.h"

/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright © 2014 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
 * This Source Code Form is “Incompatible With Secondary Licenses”, as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */

// Number of test passes to perform.
#define PASSES 5

// Number of detection to complete between reporting progress.
#define PROGRESS_MARKERS 40

// The number of items being processed.
long _max = 0;

// Prints a progress bar
void printLoadBar(long count, long max) {
	int i;
	int markers = count / (max / PROGRESS_MARKERS);

	printf("\r\t[");
	for (i = 0; i < PROGRESS_MARKERS; i++) {
		printf(i <= markers ? "=" : " ");
	}
	printf("]");
}

// Execute a performance test using a file of null terminated useragent strings
// as input. If calibrate is true then the file is read but no detections
// are performed.
int performanceTest(char* fileName, fiftyoneDegreesWorkset *ws, long max, int calibrate) {
	long count = 0;
	long marker = max / PROGRESS_MARKERS;
	const char *result;
	int profileCount = 0;
	FILE *inputFilePtr;
	long size = 0, current;
	inputFilePtr = fopen(fileName, "r");

	if (inputFilePtr == NULL) {
        printf("Failed to open file with null-terminating user agent strings to fiftyoneDegreesMatch against 51Degrees data file. \n");
        fclose(inputFilePtr);
        exit(0);
	}

    // Get the size of the file.
    if (max == 0) {
        fseek(inputFilePtr, 0, SEEK_END);
        size = ftell(inputFilePtr);
        fseek(inputFilePtr, 0, SEEK_SET);
        marker = size / PROGRESS_MARKERS;
    }

	do {
		// Get the next character from the input.
		result = fgets(ws->input, ws->dataSet->header.maxUserAgentLength, inputFilePtr);

		// Break for an empty string or end of file.
		if (result == NULL && feof(inputFilePtr))
			break;

		// If we're not calibrating then get the device for the
		// useragent that has just been read.
		if (calibrate == 0)
		{
			fiftyoneDegreesMatch(ws, ws->input); //fiftyoneDegreesWorkset, useragent
		}

		// Increase the counter and reset the offset to
		// read the next user agent from the input.
		count++;

		// Print a progress marker.
		if (max == 0) {
            current = ftell(inputFilePtr);
            if (current > marker) {
                printLoadBar(current, size);
                marker += (size / PROGRESS_MARKERS);
            }
		} else if (count % marker == 0) {
            printLoadBar(count, max);

            // We need to use the device parameter otherwise an
            // optimising compiler will ignore the call to
            // getDevice missing the point of our test!
            profileCount += ws->profileCount;
		}
	} while(1);
	fclose(inputFilePtr);
	printf("\n\n");
	return count;
}

// Perform the test and return the average time.
double performTest(char *fileName, fiftyoneDegreesWorkset *ws, int passes, int calibrate, char *test) {
	int pass;
	time_t start, end;
	fflush(stdout);

	// Perform a number of passes of the test.
	time(&start);
	for(pass = 1; pass <= passes; pass++) {
		printf("%s pass %i of %i: \n\n", test, pass, passes);
		_max = performanceTest(fileName, ws, _max, calibrate);
	}
	time(&end);
	return difftime(end, start) / (double)passes;
}

// Performance test.
void performance(char *fileName, fiftyoneDegreesWorkset *ws) {
	double totalSec, calibration, test;
	performTest(fileName, ws, 1, 1, "Caching Data");

	calibration = performTest(fileName, ws, PASSES, 1, "Calibrate");
	test = performTest(fileName, ws, PASSES, 0, "Detection test");

	// Time to complete.
	totalSec = test - calibration;
	printf("Average detection time for total data set: %.2f s\n", totalSec);
	printf("Average number of detections per second: %.2f\n", (double)_max / totalSec);

	// Wait for a character to be pressed.
	fgetc(stdin);
}

// Reduces a file path to file name only.
char *findFileNames(char *subject) {
	char *c = subject;
	char *hold = NULL;

	do {
		hold = strchr(c, '\\');
		if (hold == NULL) {
			return c;
		}
		else {
			c = hold + 1;
		}
	} while (1);

	return subject;
}

// The main method used by the command line test routine.
int main(int argc, char* argv[]) {
	fiftyoneDegreesDataSet dataSet;
	fiftyoneDegreesWorkset *ws = NULL;
	char *dataSetFileName = argc > 1 ? argv[1] : NULL;
	char *inputFileName = argc > 2 ? argv[2] : NULL;
  char *requiredProperties = argc > 3 ? argv[3] : NULL;

	printf("\n");
	printf("\t#############################################################\n");
  printf("\t#                                                           #\n");
  printf("\t#  This program can be used to test the performance of the  #\n");
  printf("\t#                51Degrees 'Pattern' C API.                 #\n");
  printf("\t#                                                           #\n");
  printf("\t#   The test will read a list of User Agents and calculate  #\n");
  printf("\t#            the number of detections per second.           #\n");
  printf("\t#                                                           #\n");
	printf("\t#  Command line arguments should be a csv file containing   #\n");
  printf("\t#  a list of user agents. A test file of 1 million can be   #\n");
	printf("\t#    downloaded from http://51degrees.com/million.zip       #\n");
	printf("\t#                                                           #\n");
  printf("\t#############################################################\n");

    if (dataSetFileName == NULL || inputFileName == NULL)
    {
        printf("Not enough arguments supplied. Expecting path/to/51Degrees.dat path/to/test_file.csv \n");
        return 0;
    }

    switch(fiftyoneDegreesInitWithPropertyString(dataSetFileName, &dataSet, requiredProperties)) {
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
        default: {
            ws = fiftyoneDegreesCreateWorkset(&dataSet);
            printf("\n\nUseragents file is: %s\n\n", findFileNames(inputFileName));
            performance(inputFileName, ws);
            fiftyoneDegreesFreeWorkset(ws);
            fiftyoneDegreesDestroy(&dataSet);
        }
                break;
    }
	return 0;
}
