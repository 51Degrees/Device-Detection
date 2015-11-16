#include <stdio.h>
#include <string.h>
#include "../src/pattern/51Degrees.h"

fiftyoneDegreesDataSet dataSet;

void output_match_metrics(fiftyoneDegreesWorkset* ws);

int main(int argc, char* argv[]) {
    const char* fileName = "../data/51Degrees-LiteV3.2.dat";
    const char* properties = "IsMobile";
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

    printf("Starting Getting Started Match Metrics Example\n");

    fiftyoneDegreesInitWithPropertyString(fileName, &dataSet, properties);

    printf("\nUser-Agent: %s\n", mobileUserAgent);
    ws = fiftyoneDegreesWorksetCreate(&dataSet, NULL);
    fiftyoneDegreesMatch(ws, mobileUserAgent);
    output_match_metrics(ws);

    printf("\nUser-Agent: %s\n", desktopUserAgent);
    fiftyoneDegreesMatch(ws, desktopUserAgent);
    output_match_metrics(ws);

    printf("\nUser-Agent: %s\n", mediaHubUserAgent);
    fiftyoneDegreesMatch(ws, mediaHubUserAgent);
    output_match_metrics(ws);
    fiftyoneDegreesWorksetFree(ws);

    fiftyoneDegreesDataSetFree(&dataSet);
}


void output_match_metrics(fiftyoneDegreesWorkset* ws) {
    int deviceIdSize = ws->dataSet->header.components.count * 10;
    char deviceId[deviceIdSize];
    fiftyoneDegreesGetDeviceId(ws, deviceId, deviceIdSize);

    int method = ws->method;
    int difference = ws->difference;
    int rank = fiftyoneDegreesGetSignatureRank(ws);

    printf("   Id: %s\n", deviceId);
    printf("   Method: %d\n", method);
    printf("   Difference: %d\n", difference);
    printf("   Rank: %d\n", rank);
    return;
}
