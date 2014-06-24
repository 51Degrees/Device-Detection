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

#ifdef __cplusplus
#define EXTERNAL extern "C"
#else
#define EXTERNAL
#endif

/* Used to provide the status of the data set initialisation */
typedef enum e_DataSetInitStatus {
    DATA_SET_INIT_STATUS_SUCCESS,
    DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY,
    DATA_SET_INIT_STATUS_CORRUPT_DATA,
    DATA_SET_INIT_STATUS_INCORRECT_VERSION,
    DATA_SET_INIT_STATUS_FILE_NOT_FOUND
} DataSetInitStatus;

// Initialises the memory using the file provided.
EXTERNAL DataSetInitStatus init(char *fileName, char *properties);

// Returns the offset to a matching device based on the useragent provided.
EXTERNAL int getDeviceOffset(char *userAgent);

// Returns the index of the property requested, or -1 if not available.
EXTERNAL int getPropertyIndex(char *value);

// Takes the results of getDeviceOffset and getPropertyIndex to return a value.
EXTERNAL char* getValue(int deviceOffset, int propertyIndex);

// Fress the memory.
EXTERNAL void destroy();

// Converts the device offset to a CSV string returning the number of
// characters used.
EXTERNAL int processDeviceCSV(int deviceOffset, char* result, int resultLength);

// Converts the device offset to a JSON string returning the number of
// characters used.
EXTERNAL int processDeviceJSON(int deviceOffset, char* result, int resultLength);
