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

#ifndef VMOD_FIFTYONEDEGREES_INTERFACE_H
#define VMOD_FIFTYONEDEGREES_INTERFACE_H

#include <stdio.h>
#include <string.h>

#ifndef FIFTYONEDEGREES_PROPERTY_NOT_FOUND
#define FIFTYONEDEGREES_PROPERTY_NOT_FOUND "N/A"
#endif

#if defined(FIFTYONEDEGREES_HASH) && defined(FIFTYONEDEGREES_PATTERN)
#error "FIFTYONEDEGREES_HASH and FIFTYONEDEGREES_PATTERN cannot both be defined."
#elif !defined(FIFTYONEDEGREES_HASH) && !defined(FIFTYONEDEGREES_PATTERN)
#error "Neither Pattern or Hash were defined as the detection algorithm to use. Define either FIFTYONEDEGREES_PATTERN or FIFTYONEDEGREES_HASH."
#endif

#ifdef FIFTYONEDEGREES_PATTERN
#include "src/pattern/51Degrees.h"
#define VMODFOD_DEVICE fiftyoneDegreesWorkset*
#endif

#ifdef FIFTYONEDEGREES_HASH
#include "src/trie/51Degrees.h"
#define VMODFOD_DEVICE fiftyoneDegreesDeviceOffsets*
#endif

/**
 * Global structure used by the module. This contains elements
 * which are required in multiple parts of the module.
 */
typedef struct vmodfod_global_t {
	fiftyoneDegreesProvider *provider; /* Single provider instance constructed at startup. */
	int cacheSize; /* Size of the cache to use in the provider. This only applies to Pattern. */
	int poolSize; /* Size of the workset pool to use in the provider. This only applies to Pattern.*/
	const char *requiredProperties; /* String containing the required properties. */
	const char *propertyDelimiter; /* The delimiter to use when returning value strings. */
	const char **importantHeaders; /* Array of important HTTP header names which should used for matching. */
	fiftyoneDegreesDataSetInitStatus status; /* Status returned when the provider is initialised. */
} vmodfod_global;

/* Default global, set before being updated by the VCL. */
#define VMODFOD_DEFAULTGLOBAL {NULL, 0, 20, "", ",", NULL, DATA_SET_INIT_STATUS_NOT_SET}

/**
 * Get the active dataset within the provider.
 * @param provider pointer to the provider to get the dataset from
 * @return pointer to the dataset currently in use by the provider
 */
fiftyoneDegreesDataSet* vmodfod_getDataSet(fiftyoneDegreesProvider *provider);

/**
 * Get the number of properties which have been initialised in the provider.
 * @param provider pointer to the provider to get the count from
 * @return the number of initialised properties
 */
int vmodfod_getRequiredPropertyCount(fiftyoneDegreesProvider *provider);

/**
 * Get the name of the property with the required property index provided. This
 * Will only return properties which have been initialised.
 * @param provider pointer to the provider to get the property name from
 * @param index the index of the property in the required properties array
 * @return name of the property
 */
const char* vmodfod_getRequiredPropertyName(
	fiftyoneDegreesProvider *provider,
	int index);

/**
 * Get the number of HTTP headers which should be provided when matching using
 * the provider.
 * @param provider pointer to the provider to get the count from
 * @return number of HTTP headers which are useful
 */
int vmodfod_getHeaderCount(fiftyoneDegreesProvider *provider);

/**
 * Get the name of the HTTP header with the index provided.
 * @param provider pointer to the provider to get the header name from
 * @oaram headerIndex the index of the header to get the name of
 * @return name of the HTTP header
 */
const char* vmodfod_getHeaderName(fiftyoneDegreesProvider *provider,
		int headerIndex);

/**
 * Get a string from the provider's string collection starting at the string
 * offset provided.
 * @param provider pointer to the provider to get the string from
 * @param offset the offset in the strings collection where the string starts
 * @return the string at the offset provided
 */
const char* vmodfod_getString(fiftyoneDegreesProvider *provider,
		uint32_t offset);

/**
 * Get the number of signatures contained in the provider's dataset. This is
 * only available in Pattern.
 * @param provider pointer to the provider to get the count from
 * @return number of signatures
 */
int vmodfod_getSignatureCount(fiftyoneDegreesProvider *provider);

/**
 * Get the number of device combinations contained in the provider's dataset.
 * @param provider pointer to the provider to get the count from
 * @return number of device combinations
 */
int vmodfod_getDeviceCombinations(fiftyoneDegreesProvider *provider);

/**
 * Initialise the provider with the settings provided.
 * @param filePath path to the data file to use
 * @param provider pointer to allocated memory to initialise
 * @param requiredProperties comma separated list of properties to initialise
 * @param poolSize size of the workset pool (only applies to Pattern)
 * @param cacheSize size of the User-Agent cache (only applies to Pattern)
 * @return status code indicating whether or not the provider was initialised
 * successfully
 */
fiftyoneDegreesDataSetInitStatus vmodfod_initProvider(
	const char *filePath,
	fiftyoneDegreesProvider *provider,
	const char *requiredProperties,
	int poolSize,
	int cacheSize);

/**
 * Write the value of the required property for the matched device. The value
 * is written to the workspace memory defined by p, u and v.
 * @param device pointer to the matched device to get the value from
 * @param requiredPropertyIndex the index of the property to get the value of
 * @param p pointer to the start of the memory to write to
 * @param u length of available memory
 * @param v number of bytes already written
 * @return the total number of bytes written i.e. the new value of v
 */
unsigned vmodfod_writeValue(
	VMODFOD_DEVICE device,
	int requiredPropertyIndex,
	char *p,
	unsigned u,
	unsigned v);

/**
 * Add the value of an HTTP header to the device ready to be matched.
 * @param device pointer to the unmatched device to add the header to
 * @param headerIndex the index of the HTTP which the value relates to
 * @param headerValue the of the HTTP header
 */
void vmodfod_addHeaderValue(
	VMODFOD_DEVICE device,
	int headerIndex,
	char *headerValue);

/**
 * Create a new instance of the device structure. Or return an instance from
 * a pool of preallocated instances.
 * @param provider pointer to the provider to create the device instance from
 * @return pointer to a device instance to use for matching
 */
VMODFOD_DEVICE vmodfod_createDevice(fiftyoneDegreesProvider *provider);

/**
 * Carry out the matching algorithm on the device instance containing the HTTP
 * header values.
 * @param device pointer to the device containing HTTP header values
 */
void vmodfod_matchDeviceAll(VMODFOD_DEVICE device);
/**
 * Carry out the matching algorithm on the device instance using the User-Agent
 * provided.
 * @param device pointer to the device to run the match on
 * @param userAgent User-Agent string to match
 */
void vmodfod_matchDeviceSingle(VMODFOD_DEVICE device, const char* userAgent);

/**
 * Release the resources used by the device instance. This could mean freeing
 * memory or returning a pointer to a pool
 * @param device pointer to the device to release
 */
void vmodfod_releaseDevice(VMODFOD_DEVICE device);

#endif /* VMOD_FIFTYONEDEGREES_INTERFACE_H */
