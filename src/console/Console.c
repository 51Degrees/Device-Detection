#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../pattern/51Degrees.h"

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

#ifndef NULL
#define NULL 0
#endif

/* Change snprintf to the Microsoft version */
#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#define RANDOM_INDEX(r) rand() % r

#define POOL_SIZE 4
#define CACHE_SIZE 5000

char *TARGET_USER_AGENTS[] = {
    // Internet explorer
    "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko",
    // Internet explorer (again to test the cache)
    "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko",
    // A set top box running Android, not a mobile device.
    "Mozilla/5.0 (Linux; U; Android 4.1.1; nl-nl; Rikomagic MK802IIIS Build/JRO03H) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Safari/534.30",
    // Galaxy Note from Samsung.
    "Mozilla/5.0 (Linux; U; Android 4.1.2; de-de; GT-N8020 Build/JZO54K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Safari/534.30",
    // A possible future version of the Galaxy Note to show how numeric fallback handling works.
    "Mozilla/5.0 (Linux; U; Android 4.1.2; de-de; GT-N8420 Build/JZO54K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Safari/534.30",
    // Crawler
    "Mozilla/5.0 (compatible; AhrefsBot/3.1; +http://ahrefs.com/robot/)",
    // Modern version of Chrome
    "Mozilla/5.0 (Windows NT 6.3; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/43.0.2357.81 Safari/537.36",
    // Internet explorer (again to test the cache)
    "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko",
	// Non Mozilla prefixed
	"Dalvik/2.1.0 (Linux; U; Android 5.0.1; HTC One_M8 Build/LRX22C)",
	// Unusual
	"DOSarrest Monitor/1.3 (Linux)"
};
int TARGET_USER_AGENTS_LENGTH = 9;

const char* PROPERTIES[] = {
    "IsMobile",
    "ScreenPixelsWidth",
    "ScreenPixelsHeight",
    "ScreenMMWidth",
    "ScreenMMHeight",
	"HardwareVendor",
	"HardwareFamily",
	"HardwareModel"
};
const int32_t PROPERTIES_COUNT = sizeof(PROPERTIES) / sizeof(char*);

void print50Columns(char* label, const char* value, int32_t length) {
    int32_t index = 0;
    for(index = 0; index < length; index++) {
        if (index % 50 == 0) {
            if (index != 0) {
                printf("\r\n\t\t\t");
            }
            else {
                printf("%s", label);
            }
        }
        printf("%c", *(value + index));
    }
    printf("\r\n");
}

void reportResults(fiftyoneDegreesWorkset *ws) {
	int32_t propertyIndex, valueIndex, profileIndex;

	printf("\r\n\t\t\t*** Detection Results ***\r\n");
	if (ws->targetUserAgent != NULL) {
		print50Columns("Target User Agent:\t", ws->targetUserAgent, (int32_t)strlen(ws->targetUserAgent));
	}
	print50Columns("Relevant Sub Strings:\t", ws->relevantNodes, (int32_t)strlen(ws->relevantNodes));
	print50Columns("Closest Sub Strings:\t", ws->closestNodes, (int32_t)strlen(ws->closestNodes));
	printf("Difference:\t\t%d\r\n", ws->difference);
	printf("Method:\t\t\t%s\r\n",
		ws->method == EXACT ? "Exact" :
		ws->method == NUMERIC ? "Numeric" :
		ws->method == CLOSEST ? "Closest" :
		ws->method == NEAREST ? "Nearest" :
		"None");
	printf("Signature Rank:\t\t%d\r\n", fiftyoneDegreesGetSignatureRank(ws));
	printf("Root Nodes Evaluated:\t%d\r\n", ws->rootNodesEvaluated);
	printf("Nodes Evaluated:\t%d\r\n", ws->nodesEvaluated);
	printf("Strings Read:\t\t%d\r\n", ws->stringsRead);
	printf("Signatures Read:\t%d\r\n", ws->signaturesRead);
	printf("Signatures Compared:\t%d\r\n", ws->signaturesCompared);
	printf("Profiles:\t\t");
	for (profileIndex = 0; profileIndex < ws->profileCount; profileIndex++) {
		printf("%d", ws->profiles[profileIndex]->profileId);
		if (profileIndex < ws->profileCount - 1) {
			printf("-");
		}
	}
	printf("\r\n");
	printf("\r\n");
	printf("\t\t\t*** Example Properties ***\r\n");

	for (propertyIndex = 0; propertyIndex < ws->dataSet->requiredPropertyCount; propertyIndex++) {
		printf("%s:\t", fiftyoneDegreesGetPropertyName(ws->dataSet, *(ws->dataSet->requiredProperties + propertyIndex)));
		if (fiftyoneDegreesSetValues(ws, propertyIndex) > 0) {
			for (valueIndex = 0; valueIndex < ws->valuesCount; valueIndex++) {
				printf("%s", fiftyoneDegreesGetValueName(ws->dataSet, *(ws->values + valueIndex)));
				if (valueIndex < ws->valuesCount - 1)
					printf(", ");
			}
		}
		printf("\r\n");
	}
}

void run(fiftyoneDegreesProvider *provider) {
    fiftyoneDegreesWorkset *ws = NULL;
	const fiftyoneDegreesDataSet *dataSet = provider->activePool->dataSet;
	fiftyoneDegreesResultsetCache *cache = provider->activePool->cache;
	int32_t index, httpHeadersLength;
	size_t httpHeadersSize;
	const char *httpHeaderNames[2];
	const char *httpHeaderValues[2];
	char *httpHeaders;

    printf("Name:\t\t\t%s\r\n", &(fiftyoneDegreesGetString(dataSet, dataSet->header.nameOffset)->firstByte));
    printf("Published:\t\t%d/%d/%d\r\n",
           dataSet->header.published.day,
           dataSet->header.published.month,
           dataSet->header.published.year);
    printf("Next Update:\t\t%d/%d/%d\r\n",
           dataSet->header.nextUpdate.day,
           dataSet->header.nextUpdate.month,
           dataSet->header.nextUpdate.year);
    printf("Signatures:\t\t%d\r\n", dataSet->header.signatures.count);
    printf("Device Combinations:\t%d\r\n", dataSet->header.deviceCombinations);
    printf("Data set format:\t%s\r\n", &(fiftyoneDegreesGetString(dataSet, dataSet->header.formatOffset)->firstByte));
    printf("\r\n");

    for(index = 0; index < (sizeof(TARGET_USER_AGENTS) / sizeof(char*)); index++)
    {
		ws = fiftyoneDegreesProviderWorksetGet(provider);
		fiftyoneDegreesMatch(ws, TARGET_USER_AGENTS[index]);
		reportResults(ws);
		fiftyoneDegreesWorksetRelease(ws);
    }

	for (index = 0; index < 5; index++) {

		// Use multiple headers as arrays.
		httpHeaderNames[0] = (ws->dataSet->httpHeaders + RANDOM_INDEX(ws->dataSet->httpHeadersCount))->headerName;
		httpHeaderNames[1] = fiftyoneDegreesGetPrefixedUpperHttpHeaderName(ws->dataSet, RANDOM_INDEX(ws->dataSet->httpHeadersCount));
		httpHeaderValues[0] = TARGET_USER_AGENTS[RANDOM_INDEX(TARGET_USER_AGENTS_LENGTH)];
		httpHeaderValues[1] = TARGET_USER_AGENTS[RANDOM_INDEX(TARGET_USER_AGENTS_LENGTH)];
		ws = fiftyoneDegreesProviderWorksetGet(provider);
		fiftyoneDegreesMatchWithHeadersArray(ws, httpHeaderNames, httpHeaderValues, 2);
		printf("\r\n\t\t\t*** HTTP Headers Array ***\r\n");
		print50Columns("Header 1 Name:\t\t", httpHeaderNames[0], strlen(httpHeaderNames[0]));
		print50Columns("Header 1 Value:\t\t", httpHeaderValues[0], strlen(httpHeaderValues[0]));
		print50Columns("Header 2 Name:\t\t", httpHeaderNames[1], strlen(httpHeaderNames[1]));
		print50Columns("Header 2 Value:\t\t", httpHeaderValues[1], strlen(httpHeaderValues[1]));
		reportResults(ws);
		fiftyoneDegreesWorksetRelease(ws);

		// Use multiple headers as a single string.
		httpHeadersSize = strlen(httpHeaderNames[0]) + 1 +
			strlen(httpHeaderNames[1]) + 1 +
			strlen(httpHeaderValues[0]) + 2 +
			strlen(httpHeaderValues[1]) + 1;
		httpHeaders = (char*)malloc(httpHeadersSize);
		if (httpHeaders != NULL) {
			httpHeadersLength = snprintf(httpHeaders, httpHeadersSize,
				"%s %s\r\n%s %s",
				httpHeaderNames[0],
				httpHeaderValues[0],
				httpHeaderNames[1],
				httpHeaderValues[1]);
			ws = fiftyoneDegreesProviderWorksetGet(provider);
			fiftyoneDegreesMatchWithHeadersString(ws, httpHeaders, httpHeadersLength);
			printf("\r\n\t\t\t*** HTTP Headers String ***\r\n");
			printf("%s\r\n",httpHeaders);
			free((void*)httpHeaders);
		}
		reportResults(ws);
		fiftyoneDegreesWorksetRelease(ws);
	}

    if (cache != NULL) {
        printf("\r\n\t\t\t*** Cache Results ***\r\n");
		printf("Max Iterations:\t%d\r\n", cache->maxIterations);
        printf("Hits:\t\t%d\r\n", cache->hits);
        printf("Misses:\t\t%d\r\n", cache->misses);
    }
}

int32_t main(int32_t argc, char* argv[]) {
    fiftyoneDegreesProvider provider;

    if (argc > 1) {
		switch (fiftyoneDegreesInitProviderWithPropertyArray(argv[1], &provider, PROPERTIES, PROPERTIES_COUNT, POOL_SIZE, CACHE_SIZE)) {
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
			case DATA_SET_INIT_STATUS_NOT_SET:
				printf("Device data file '%s' could not be loaded.", argv[1]);
				break;
			case DATA_SET_INIT_STATUS_NULL_POINTER:
				printf("Null pointer prevented loading of '%s'.", argv[1]);
            default:
				run(&provider);
				fiftyoneDegreesProviderFree(&provider);
                break;
        }
    }

	// Wait for a character to be pressed.
	fgetc(stdin);

    return 0;
}
