#include <stdio.h>
#include <string.h>
#include "../src/pattern/51Degrees.h"

fiftyoneDegreesWorkset *ws = NULL;
fiftyoneDegreesDataSet dataSet;

const char* getValue(fiftyoneDegreesWorkset* ws, char* propertyName);

int main(int argc, char* argv[]) {

    const char* fileName = "../data/51Degrees-LiteV3.2.dat";
    char properties[] = "IsMobile,PlatformName,PlatformVersion";
    char *propertiesArray[1000];
    char userAgent[1000];
    const char* value;
    int i, j;

    fiftyoneDegreesInitWithPropertyString(fileName, &dataSet, properties);
    ws = fiftyoneDegreesWorksetCreate(&dataSet, NULL);

    printf("Starting Offline Processing Example.\n");

    char* inputFile = "../data/20000 User Agents.csv";
    char* outputFile = "offlineProcessingOutput.csv";

    FILE* fin = fopen(inputFile, "r");
    FILE* fout = fopen(outputFile, "w");

    char *tok = strtok(properties, ",");
    while (tok != NULL) {
        propertiesArray[i++] = tok;
        tok = strtok(NULL, ",");
    }
    int propertiesCount = i;

    fprintf(fout, "User-Agent");
    for (j=0;j<propertiesCount;j++) {
        fprintf(fout, "|%s", propertiesArray[j]);
    }
    fprintf(fout, "\n");

    for (i=0;i<20;i++) {
        fgets(userAgent, sizeof(userAgent), fin);
        userAgent[strlen(userAgent)-1] = '\0';
        fprintf(fout, "%s", userAgent);
        fiftyoneDegreesMatch(ws, userAgent);
        for (j=0;j<propertiesCount;j++) {
            value = getValue(ws, propertiesArray[j]);
            fprintf(fout, "|%s", value);
        }
        fprintf(fout, "\n");
    }

    printf("Output Written to %s\n", outputFile);
    fiftyoneDegreesWorksetFree(ws);
    fiftyoneDegreesDataSetFree(&dataSet);
}

const char* getValue(fiftyoneDegreesWorkset* ws, char* propertyName) {
    int requiredPropertyIndex;
    const char* result;
    const fiftyoneDegreesAsciiString* valueName;

    requiredPropertyIndex = fiftyoneDegreesGetRequiredPropertyIndex(ws->dataSet, propertyName);
    fiftyoneDegreesSetValues(ws, requiredPropertyIndex);
    valueName = fiftyoneDegreesGetString(ws->dataSet, ws->values[0]->nameOffset);
    result = &(valueName->firstByte);
    return result;
}
