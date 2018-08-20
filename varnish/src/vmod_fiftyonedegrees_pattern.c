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
#include "vmod_fiftyonedegrees_interface.h"
#include "src/pattern/51Degrees.h"

/**
 * This file contains the Pattern API implementation of the detection interface
 * for the 51Degrees VMOD.
 */

fiftyoneDegreesDataSet* vmodfod_getDataSet(fiftyoneDegreesProvider *provider) {
	return (fiftyoneDegreesDataSet*)provider->activePool->dataSet;
}
int vmodfod_getRequiredPropertyCount(fiftyoneDegreesProvider *provider) {
	return (int)vmodfod_getDataSet(provider)->requiredPropertyCount;
}
const char* vmodfod_getRequiredPropertyName(
	fiftyoneDegreesProvider *provider,
	int index) {
	return fiftyoneDegreesGetPropertyName(
		vmodfod_getDataSet(provider),
		vmodfod_getDataSet(provider)->requiredProperties[index]);
}
int vmodfod_getHeaderCount(fiftyoneDegreesProvider *provider) {
	return (int)vmodfod_getDataSet(provider)->httpHeadersCount;
}
const char* vmodfod_getHeaderName(
	fiftyoneDegreesProvider *provider,
	int headerIndex) {
	return vmodfod_getDataSet(provider)->httpHeaders[headerIndex].headerName;
}
const char* vmodfod_getString(
	fiftyoneDegreesProvider *provider,
	uint32_t offset) {
	return &fiftyoneDegreesGetString(
		vmodfod_getDataSet(provider),
		offset)->firstByte;
}
int vmodfod_getSignatureCount(fiftyoneDegreesProvider *provider) {
	return vmodfod_getDataSet(provider)->header.signatures.count;
}
int vmodfod_getDeviceCombinations(fiftyoneDegreesProvider *provider) {
	return vmodfod_getDataSet(provider)->header.deviceCombinations;
}
fiftyoneDegreesDataSetInitStatus vmodfod_initProvider(
	const char *filePath,
	fiftyoneDegreesProvider *provider,
	const char *requiredProperties,
	int poolSize,
	int cacheSize) {
	return fiftyoneDegreesInitProviderWithPropertyString(
		filePath,
		provider,
		requiredProperties,
		poolSize,
		cacheSize);
}
unsigned vmodfod_writeValue(
	VMODFOD_DEVICE device,
	int requiredPropertyIndex,
	char *p,
	unsigned u,
	unsigned v) {
	int i;
	// This if the property we want, so set the values and go
	// through them all.
	fiftyoneDegreesSetValues(device, requiredPropertyIndex);
	for (i = 0; i < device->valuesCount; i++)
	{
		if (i != 0) {
			// Print a separator between values.
			v += snprintf(p + v, u, "|");
		}
		// Print the value to the values string.
		v += snprintf(p + v, u, "%s", fiftyoneDegreesGetValueName(
			device->dataSet,
			device->values[i]));
	}
	return v;
}
void vmodfod_addHeaderValue(
	VMODFOD_DEVICE device,
	int headerIndex,
	char *headerValue) {
    device->importantHeaders[device->importantHeadersCount].header =
        device->dataSet->httpHeaders + headerIndex;
    device->importantHeaders[device->importantHeadersCount].headerValue =
        (const char*)headerValue;
    device->importantHeaders[device->importantHeadersCount].headerValueLength =
        strlen(headerValue);
    device->importantHeadersCount++;
}

VMODFOD_DEVICE vmodfod_createDevice(fiftyoneDegreesProvider *provider) {
    VMODFOD_DEVICE device = fiftyoneDegreesProviderWorksetGet(provider);
    // Reset the headers count before adding any to the workset.
	device->importantHeadersCount = 0;
	return device;
}

void vmodfod_matchDeviceAll(VMODFOD_DEVICE device) {
	fiftyoneDegreesMatchForHttpHeaders(device);
}

void vmodfod_matchDeviceSingle(VMODFOD_DEVICE device, const char* userAgent) {
	fiftyoneDegreesMatch(device, userAgent);
}

void vmodfod_releaseDevice(VMODFOD_DEVICE device) {
    fiftyoneDegreesWorksetRelease(device);
}
