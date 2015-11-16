#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../src/pattern/51Degrees.h"

fiftyoneDegreesDataSet dataSet;

bool getIsMobileBool(fiftyoneDegreesWorkset* ws);

int main(int argc, char* argv[]) {
    const char* fileName = "../data/51Degrees-LiteV3.2.dat";
    const char* properties = "IsMobile";
    bool isMobileBool;
    fiftyoneDegreesWorkset *ws = NULL;

    // User-Agent string of an iPhone mobile device.
    const char* mobileUserAgent = ("Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) "
    "AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 "
    "Safari/9537.53");

    // User-Agent string of Firefox Web browser version 41 on dektop.
    const char* desktopUserAgent = ("Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) "
    "Gecko/20100101 Firefox/41.0");

    // User-Agent string of a MediaHub device.
    const char* mediaHubUserAgent = ("Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core "
    "Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 "
    "Chrome/30.0.0.0 Safari/537.36");

    printf("Starting Getting Started Strongly Typed Example\n");

    fiftyoneDegreesInitWithPropertyString(fileName, &dataSet, properties);

    printf("\nUser-Agent: %s\n", mobileUserAgent);
    ws = fiftyoneDegreesWorksetCreate(&dataSet, NULL);
    fiftyoneDegreesMatch(ws, mobileUserAgent);
    isMobileBool = getIsMobileBool(ws);
    if (isMobileBool){
        printf("Mobile\n");
    }
    else {
        printf("Non-Mobile\n");
    }

    printf("\nUser-Agent: %s\n", desktopUserAgent);
    ws = fiftyoneDegreesWorksetCreate(&dataSet, NULL);
    fiftyoneDegreesMatch(ws, desktopUserAgent);
    isMobileBool = getIsMobileBool(ws);
    if (isMobileBool){
        printf("Mobile\n");
    }
    else {
        printf("Non-Mobile\n");
    }

    printf("\nUser-Agent: %s\n", mediaHubUserAgent);
    ws = fiftyoneDegreesWorksetCreate(&dataSet, NULL);
    fiftyoneDegreesMatch(ws, mediaHubUserAgent);
    isMobileBool = getIsMobileBool(ws);
    if (isMobileBool){
        printf("Mobile\n");
    }
    else {
        printf("Non-Mobile\n");
    }
}

bool getIsMobileBool(fiftyoneDegreesWorkset* ws) {
    int requiredPropertyIndex;
    const char* isMobile;
    const fiftyoneDegreesAsciiString* valueName;

    requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(ws->dataSet, "IsMobile");
    fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
    valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[0]->nameOffset);
    isMobile = &(valueName->firstByte);
    if (strcmp(isMobile, "True") == 0) {
        return true;
    }
    else {
    return false;
    }
}
