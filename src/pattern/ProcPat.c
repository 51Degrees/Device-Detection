#include <stdio.h>
#include <string.h>
#include "51Degrees.h"

#ifdef _MSC_VER
#define _INTPTR 0
#endif

#define BUFFER_LENGTH 50000

int main(int argc, char* argv[]) {
	fiftyoneDegreesWorkset *ws = NULL;
	fiftyoneDegreesDataSet dataSet;
	char output[BUFFER_LENGTH];
	char *fileName = argc > 1 ? argv[1] : NULL;
	char *requiredProperties = argc > 2 ? argv[2] : NULL;

	switch (fiftyoneDegreesInitWithPropertyString(fileName, &dataSet, requiredProperties)) {
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

		ws = fiftyoneDegreesCreateWorkset(&dataSet, NULL);

#ifdef _MSC_VER
		gets_s(ws->input, dataSet.header.maxUserAgentLength);
#else
		gets(ws->input);
#endif
		while (strlen(ws->input) > 0) {
			fiftyoneDegreesMatch(ws, ws->input);
			if (ws->profileCount > 0) {
				fiftyoneDegreesProcessDeviceCSV(ws, output, BUFFER_LENGTH);
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
			gets(ws->input);
#endif
		}

		fiftyoneDegreesFreeWorkset(ws);
		fiftyoneDegreesDestroy(&dataSet);
	}//End default
		break;
	}//End Switch

	return 0;
}
