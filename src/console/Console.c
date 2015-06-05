#include <stdio.h>
#include <string.h>
#include "../pattern/51Degrees.h"

#ifndef NULL
#define NULL 0
#endif

char* TARGET_USER_AGENTS[] = {
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
    "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko"
};

char* PROPERTIES[] = {
    "IsMobile",
    "ScreenPixelsWidth",
    "ScreenPixelsHeight",
    "ScreenMMWidth",
    "ScreenMMHeight"
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

void run(fiftyoneDegreesDataSet *dataSet) {
    fiftyoneDegreesWorkset *ws = NULL;
    int32_t index, propertyIndex, valueIndex, profileIndex;

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
    printf("Data set version:\t%s\r\n", &(fiftyoneDegreesGetString(dataSet, dataSet->header.formatOffset)->firstByte));
    printf("\r\n");

    ws = fiftyoneDegreesCreateWorksetWithCache(dataSet, 3);
    if (ws != NULL) {
        for(index = 0; index < (sizeof(TARGET_USER_AGENTS) / sizeof(char*)); index++)
        {
            fiftyoneDegreesMatch(ws, TARGET_USER_AGENTS[index]);

            printf("\r\n\t\t\t*** Detection Results ***\r\n");
            print50Columns("Target User Agent:\t", ws->targetUserAgent, (int32_t)strlen(ws->targetUserAgent));
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
            for(profileIndex = 0; profileIndex < ws->profileCount; profileIndex++) {
                printf("%d", ws->profiles[profileIndex]->profileId);
                if (profileIndex < ws->profileCount - 1) {
                    printf("-");
                }
            }
            printf("\r\n");
            printf("\r\n");
            printf("\t\t\t*** Example Properties ***\r\n");

            for(propertyIndex = 0; propertyIndex < ws->dataSet->requiredPropertyCount; propertyIndex++) {
                printf("%s:\t", fiftyoneDegreesGetPropertyName(ws->dataSet, *(ws->dataSet->requiredProperties + propertyIndex)));
                if (fiftyoneDegreesSetValues(ws, propertyIndex) > 0) {
                    for(valueIndex = 0; valueIndex < ws->valuesCount; valueIndex++) {
                        printf("%s", fiftyoneDegreesGetValueName(ws->dataSet, *(ws->values + valueIndex)));
                        if (valueIndex < ws->valuesCount - 1)
                            printf(", ");
                    }
                }
                printf("\r\n");
            }
        }

        if (ws->cache != NULL) {
            printf("\r\n\t\t\t*** Cache Results ***\r\n");
            printf("Switches:\t%d\r\n", ws->cache->switches);
            printf("Hits:\t\t%d\r\n", ws->cache->hits);
            printf("Misses:\t\t%d\r\n", ws->cache->misses);
        }

        fiftyoneDegreesFreeWorkset(ws);
    }
    fiftyoneDegreesDestroy(dataSet);
}

int32_t main(int32_t argc, char* argv[]) {
    fiftyoneDegreesDataSet dataSet;

    if (argc > 1) {
        switch(fiftyoneDegreesInitWithPropertyArray(argv[1], &dataSet, PROPERTIES, PROPERTIES_COUNT)) {
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
            default:
                run(&dataSet);
                break;
        }
    }

    return 0;
}
