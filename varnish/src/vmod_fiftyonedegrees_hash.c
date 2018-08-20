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
#include "src/trie/51Degrees.h"

/**
 * This file contains the Hash API implementation of the detection interface
 * for the 51Degrees VMOD.
 */

fiftyoneDegreesDataSet* vmodfod_getDataSet(fiftyoneDegreesProvider *provider) {
	return provider->active->dataSet;
}
int vmodfod_getRequiredPropertyCount(fiftyoneDegreesProvider *provider) {
	return fiftyoneDegreesGetRequiredPropertiesCount(vmodfod_getDataSet(provider));
}
const char* vmodfod_getRequiredPropertyName(
	fiftyoneDegreesProvider *provider,
	int index) {
	return vmodfod_getDataSet(provider)->requiredPropertiesNames[index];
}
int vmodfod_getHeaderCount(fiftyoneDegreesProvider *provider) {
	return fiftyoneDegreesGetHttpHeaderCount(vmodfod_getDataSet(provider));
}
const char* vmodfod_getString(
	fiftyoneDegreesProvider *provider,
	uint32_t offset) {
	return ((char*)vmodfod_getDataSet(provider)->strings.firstByte +
		offset +
		sizeof(int16_t));
}
const char* vmodfod_getHeaderName(
	fiftyoneDegreesProvider *provider,
	int headerIndex) {
	return fiftyoneDegreesGetHttpHeaderNamePointer(
		vmodfod_getDataSet(provider),
		headerIndex);
}
int vmodfod_getSignatureCount(fiftyoneDegreesProvider *provider) {
	return -1;
}
int vmodfod_getDeviceCombinations(fiftyoneDegreesProvider *provider) {
	return vmodfod_getDataSet(provider)->devices.count;
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
		requiredProperties);
}
unsigned vmodfod_writeValue(
	VMODFOD_DEVICE device,
	int requiredPropertyIndex,
	char *p,
	unsigned u,
	unsigned v) {
	const char *value = fiftyoneDegreesGetValuePtrFromOffsets(
		device->active->dataSet,
		device,
		requiredPropertyIndex);
	v += snprintf(p + v, u, "%s", value);
	return v;
}
void vmodfod_addHeaderValue(
	VMODFOD_DEVICE device,
	int headerIndex,
	char *headerValue) {
    fiftyoneDegreesSetDeviceOffset(
    	device->active->dataSet,
		headerValue,
		headerIndex,
		(&device->firstOffset)[device->size]);
    device->size++;
}
VMODFOD_DEVICE vmodfod_createDevice(fiftyoneDegreesProvider *provider) {
    return fiftyoneDegreesProviderCreateDeviceOffsets(provider);
}
void vmodfod_matchDeviceAll(VMODFOD_DEVICE device) {
}
void vmodfod_matchDeviceSingle(VMODFOD_DEVICE device, const char* userAgent) {
	fiftyoneDegreesSetDeviceOffset(
		device->active->dataSet,
		userAgent,
		0,
		device->firstOffset);
	device->size = 1;
}
void vmodfod_releaseDevice(VMODFOD_DEVICE device) {
    fiftyoneDegreesFreeDeviceOffsets(device);
}
