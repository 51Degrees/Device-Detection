#include <stdio.h>
#include <string.h>
#include "../src/pattern/51Degrees.h"

fiftyoneDegreesWorkset *ws = NULL;
fiftyoneDegreesDataSet dataSet;

const char* getIsMobile(fiftyoneDegreesWorkset* ws);

int main(int argc, char* argv[]) {

const char* fileName = "../data/51Degrees-LiteV3.2.dat";
const char* properties = "IsMobile";
const char* isMobile;

// Device id string of an iPhone mobile device.
const char* mobileDeviceId = "12280-48866-24305-18092";

// Device id string of Firefox Web browser version 41 on dektop.
const char* desktopDeviceId = "15364-21460-53251-18092";

// Device id string of a MediaHub device.
const char* mediaHubDeviceId = "41231-46303-24154-18092";


printf("Starting Getting Started Example.\n");

fiftyoneDegreesInitWithPropertyString(fileName, &dataSet, properties);

printf("\nMobileDeviceId: %s\n", mobileDeviceId);
ws = fiftyoneDegreesWorksetCreate(&dataSet, NULL);
fiftyoneDegreesMatchForDeviceId(ws, mobileDeviceId);
isMobile = getIsMobile(ws);
printf("   IsMobile: %s\n", isMobile);
//fiftyoneDegreesWorksetFree(ws);

printf("\nDesktopDeviceId: %s\n", desktopDeviceId);
//ws = fiftyoneDegreesWorksetCreate(&dataSet, NULL);
fiftyoneDegreesMatchForDeviceId(ws, desktopDeviceId);
isMobile = getIsMobile(ws);
printf("   IsMobile: %s\n", isMobile);
//fiftyoneDegreesWorksetFree(ws);

printf("\nMediHub Device Id: %s\n", mediaHubDeviceId);
//ws = fiftyoneDegreesWorksetCreate(&dataSet, NULL);
fiftyoneDegreesMatchForDeviceId(ws, mediaHubDeviceId);
isMobile = getIsMobile(ws);
printf("   IsMobile: %s\n", isMobile);
fiftyoneDegreesWorksetFree(ws);

fiftyoneDegreesDataSetFree(&dataSet);
}

const char* getIsMobile(fiftyoneDegreesWorkset* ws) {
    int requiredPropertyIndex;
    const char* isMobile;
    const fiftyoneDegreesAsciiString* valueName;

    requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(ws->dataSet, "IsMobile");
    fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
    valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[0]->nameOffset);
    isMobile = &(valueName->firstByte);
    return isMobile;
}
