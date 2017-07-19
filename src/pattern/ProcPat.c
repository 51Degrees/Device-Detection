#include <stdio.h>
#include <string.h>
#include "51Degrees.h"

/* *********************************************************************
* This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
* Copyright 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
********************************************************************** */

#ifdef _MSC_VER
#define _INTPTR 0
#endif

int main(int argc, char* argv[]) {
	fiftyoneDegreesWorkset *ws = NULL;
	fiftyoneDegreesDataSet dataSet;
    char *output;
	char *fileName = argc > 1 ? argv[1] : NULL;
	char *requiredProperties = argc > 2 ? argv[2] : NULL;
	char *result;

	switch (fiftyoneDegreesInitWithPropertyString(fileName, &dataSet, requiredProperties)) {
	case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
		printf("Insufficient memory to load '%s'.", argv[1]);
		break;
	case DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS:
	case DATA_SET_INIT_STATUS_CORRUPT_DATA:
		printf("Device data file '%s' is corrupted.", argv[1]);
		break;
	case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
		printf("Device data file '%s' is not correct version.", argv[1]);
		break;
	case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
		printf("Device data file '%s' not found.", argv[1]);
		break;
	case DATA_SET_INIT_STATUS_NULL_POINTER:
		printf("Null pointer prevented loading of '%s'.", argv[1]);
		break;
	default: {

		ws = fiftyoneDegreesWorksetCreate(&dataSet, NULL);
		output = fiftyoneDegreesCSVCreate(ws);

#ifdef _MSC_VER
		result = gets_s(ws->input, dataSet.header.maxUserAgentLength);
#else
		result = fgets(ws->input, ws->dataSet->header.maxUserAgentLength, stdin);
#endif
		while (result != NULL) {
			fiftyoneDegreesMatch(ws, ws->input);
			if (ws->profileCount > 0) {
				fiftyoneDegreesProcessDeviceCSV(ws, output);
				printf("%s", output);
				/* Diagnostics Info
				printf("\r\nDiagnostics Information\r\n\r\n");
				printf("  Difference:           %i\r\n", ws->difference);
				printf("  Method:               %i\r\n", ws->method);
				printf("  Root Nodes Evaluated: %i\r\n", ws->rootNodesEvaluated);
				printf("  Nodes Evaluated:      %i\r\n", ws->nodesEvaluated);
				printf("  Strings Read:         %i\r\n", ws->stringsRead);
				printf("  Signatures Read:      %i\r\n", ws->signaturesRead);
				printf("  Signatures Compared:  %i\r\n", ws->signaturesCompared);
				printf("  Closest Signatures:   %i\r\n", ws->closestSignatures);
				*/
			}
			else {
				printf("null\n");
			}

			// Flush buffers.
			fflush(stdin);
			fflush(stdout);

			// Get the next useragent.
#ifdef _MSC_VER
			gets_s(ws->input, dataSet.header.maxUserAgentLength);
#else
			result = fgets(ws->input, ws->dataSet->header.maxUserAgentLength, stdin);
#endif
		}

        fiftyoneDegreesCSVFree(output);
		fiftyoneDegreesWorksetFree(ws);
		fiftyoneDegreesDataSetFree(&dataSet);
	}//End default
		break;
	}//End Switch

	return 0;
}
