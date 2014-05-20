#include <stdio.h>
#include <string.h>

#include "51Degrees.h"

int main(int argc, char* argv[]) {
	char input[50000], output[50000];

	if (argc > 1) {
		if (init(argv[1], argc > 2 ? argv[2] : "Id") == 0) {
			gets(input);
			while(strlen(input) > 0) {
				processDeviceCSV(getDeviceOffset(input), output, 50000);
				printf("%s", output);

				// Flush buffers.
				fflush(stdin);
				fflush(stdout);

				// Get the next useragent.
				gets(input);
			}
			destroy();
		}
	}

	return 0;
}
