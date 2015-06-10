#include <stdio.h>
#include <Windows.h>

int count = 0;
const int attempts = 1000000;
#define threadCount 2

HANDLE ghMutex;



DWORD WINAPI increment(LPVOID lpParam)
{
	Sleep(1000);
	for (int i = 0; i < attempts; i++)
	{
		count++;
	}
	return 0;
}

DWORD WINAPI safe_increment(LPVOID lpParam)
{
	Sleep(1000);
	
	for (int i = 0; i < attempts; i++)
	{
		WaitForSingleObject(
			ghMutex,    // handle to mutex
			INFINITE);  // no time-out interval
		count++;
		ReleaseMutex(ghMutex);
	}
}

int main()
{
	DWORD   dwThreadIdArray[threadCount];
	HANDLE  hThreadArray[threadCount];

	ghMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

	// Create MAX_THREADS worker threads.

	for (int i = 0; i < threadCount; i++)
	{
		// Create the thread to begin execution on its own.

		hThreadArray[i] = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			safe_increment,       // thread function name
			NULL,          // argument to thread function 
			0,                      // use default creation flags 
			&dwThreadIdArray[i]);   // returns the thread identifier 
	} // End of main thread creation loop.

	printf("Starting threads.\n");

	printf("Awaiting threads...\n");

	WaitForMultipleObjects(threadCount, hThreadArray, TRUE, INFINITE);

	printf("Complete. %d threads incrementing %d times.\n", threadCount, attempts);
	printf("Excecting a count of %d. Actually returned %d.", threadCount * attempts, count);
	
	scanf_s("%d");
	
	for (int i = 0; i < threadCount; i++)
	{
		CloseHandle(hThreadArray[i]);
	}

	CloseHandle(ghMutex);

	/* exit */
	exit(0);
}
