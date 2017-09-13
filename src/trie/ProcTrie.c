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
#include <stdio.h>
#include <string.h>

#include "51Degrees.h"

int main(int argc, char* argv[]) {
	fiftyoneDegreesDataSet dataSet;
	char input[50000], output[50000];
	char *result;

    if (argc > 1) {
      char *fileName = argc > 1 ? argv[1] : "../../../data/51Degree-LiteV3.4.trie";
      char *requiredProperties = argc > 2 ? argv[2] : NULL;
      switch(fiftyoneDegreesInitWithPropertyString(fileName, &dataSet, requiredProperties)) {
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
          result = fgets(input, 50000, stdin);
          while(result != 0) {
            fiftyoneDegreesProcessDeviceCSV(&dataSet, fiftyoneDegreesGetDeviceOffset(&dataSet, input), output, 50000);
            printf("%s", output);

            // Flush buffers.
            fflush(stdin);
            fflush(stdout);

            // Get the next useragent.
            result = fgets(input, 50000, stdin);
          }
          fiftyoneDegreesDataSetFree(&dataSet);
          break;
        }
      }
    } else {
        printf("Not enough arguments supplied for program to run. \n");
    }

	return 0;
}
