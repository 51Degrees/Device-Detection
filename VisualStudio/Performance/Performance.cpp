#include "stdafx.h"

// The number of threads to start.
#define NUM_THREADS 4

// Number of lines to read from file before reporting progress.
#define PROGRESS 200000

// Number of passes to included.
#define PASSES 5

// The timing results from each pass.
typedef struct t_result {
	int detections;
	int clocks;
} Result;

// The data used by each thread.
typedef struct t_threaddata {
	char* userAgentsFile;
	int detections;
	int calibrate;
	int device;
} ThreadData;

// Dataset used for matching.
static fiftyoneDegreesDataSet dataSet;

// Array of thread handles used to join the
// treads after each pass.
static HANDLE _threads[NUM_THREADS];

// The Ids of each of the threads.
static DWORD _threadIds[NUM_THREADS];

// Reads the input file of user agents and records number of detections
// and the time taken to perform them.
DWORD WINAPI test(LPVOID myThreadData) {
	ThreadData *data = (ThreadData*)myThreadData;
	FILE *inputFilePtr;
	char userAgent[1024];
	char* result;
	int device = -1;
	int counter = 0;

	while (counter < 1000000) {
		if (fopen_s(&inputFilePtr, data->userAgentsFile, "r") == 0) {

			// Get the next character from the input.
			result = fgets(userAgent, 1024, inputFilePtr);

			while (result != NULL || !feof(inputFilePtr)) {

				counter++;

				// If we're not calibrating then get the device for the
				// User-Agent that has just been read.
				if (strlen(userAgent) < 1024 && data->calibrate == 0)
					device = fiftyoneDegreesGetDeviceOffset(&dataSet, userAgent);

				// Increase the counter.
				data->detections++;

				// Print a progress marker.
				if (data->detections % PROGRESS == 0) {
					printf("=");
				}

				// Get the next character from the input.
				result = fgets(userAgent, 1024, inputFilePtr);
			};

			// Do this to ensure the compiler does not remove the test
			// we're trying to perform.
			data->device = device;

			fclose(inputFilePtr);
		}
	}
	return 0;
}

// Copies the TCHAR string to a char* string.
char* copyFileName(_TCHAR* source) {
	size_t wlen = wcslen(source) + 1;
	char *file = (char*)malloc(wlen * sizeof(char));
	wcstombs(file, source, wlen);
	return file;
}

// Performs a pass of the user agents file recording results
// in the results structure passed to the method.
void performPass(char *userAgentsFile, int calibrate, Result* result) {
	int thread = 0;
	ThreadData *data[NUM_THREADS];
	clock_t start = clock();

	// Start the threads for calibration.
	while(thread < NUM_THREADS) {
		data[thread] = (ThreadData*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ThreadData));
		data[thread]->userAgentsFile = userAgentsFile;
		data[thread]->calibrate = calibrate;
		_threads[thread] = CreateThread(NULL, 0, test, data[thread], 0, &_threadIds[thread]);
		thread++;
	}

	// Join the threads.
	WaitForMultipleObjects(NUM_THREADS, _threads, TRUE, INFINITE);

	// Record the total time taken to process the data.
	result->clocks = clock() - start + 1;

	// Record the number of detections performed.
	result->detections = 0;
	for(thread = 0; thread < NUM_THREADS; thread++) {
		result->detections += data[thread]->detections;
		HeapFree(GetProcessHeap(), 0, data[thread]);
	}
}

// Main method used to initiate the performance test.
int _tmain(int argc, _TCHAR* argv[]) {
	char *userAgentsFile, *dataFile;
	Result calibrate, test;
	int pass = 0, clocks = 0, detections = 0;

	printf("\n");
	printf("\t#############################################################\n");
    printf("\t#                                                           #\n");
    printf("\t#  This program can be used to test the performance of the  #\n");
    printf("\t#        51Degrees 'Trie' C API in a *multi threaded*       #\n");
	printf("\t#                      environment.                         #\n");
    printf("\t#                                                           #\n");
    printf("\t#   The test will read a list of User Agents and calculate  #\n");
    printf("\t#            the number of detections per second.           #\n");
    printf("\t#                                                           #\n");
	printf("\t#    Command line arguments should be a trie format data    #\n");
	printf("\t#   file and a csv file containing a list of user agents.   #\n");
	printf("\t#      A test file of 1 million can be downloaded from      #\n");
	printf("\t#            https://51degrees.com/million.zip              #\n");
	printf("\t#                                                           #\n");
    printf("\t#############################################################");

	if (argc >= 3) {

		// Get the useragents input file.
		dataFile = copyFileName(argv[1]);
		userAgentsFile = copyFileName(argv[2]);

		// Initialise the trie matcher.
		if (fiftyoneDegreesInitWithPropertyString(dataFile, &dataSet, "Id") != 0) {
			printf("\nData file '%s' could not be loaded.\n", dataFile);
		} else {

			for(pass = 0; pass < PASSES; pass++) {
				printf("\n\nCalibrating %d:\t\t", pass + 1);
				performPass(userAgentsFile, 1, &calibrate);
				printf("\n\nDetection test %d:\t", pass + 1);
				performPass(userAgentsFile, 0, &test);

				// Record the number of detections completed and the
				// difference in time between the calibration test
				// and the actual detection test.
				clocks += test.clocks - calibrate.clocks;
				detections += test.detections;
			}

			// Destroy the trie dataset.
			fiftyoneDegreesDataSetFree(&dataSet);

			// Display the results in detections per second.
			printf(
				"\n\nResult:\t\t\t%.0f detections per second\n\n",
				(float)detections / ((float)clocks / (float)CLOCKS_PER_SEC));
		}

		// Release the memory.
		free(dataFile);
		free(userAgentsFile);

		// Wait for a key press.
		fgetc(stdin);
	}
	return 0;
}