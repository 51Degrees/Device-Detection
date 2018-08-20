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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// Set up headers for the correct version of Varnish.
#ifdef FIFTYONEDEGREES_VARNISH_VERSION
#if FIFTYONEDEGREES_VARNISH_VERSION < 060000
#include "vrt.h"
#include "vcl.h"
#endif
#else
#error "FIFTYONEDEGREES_VARNISH_VERSION was not defined. Either define in the format MMmmRR or build using configure which does this automatically."
#endif

#include "cache/cache.h"
#include "string.h"
#include "vmod_fiftyonedegrees_interface.h"

// Global structure available to the module.
vmodfod_global global = VMODFOD_DEFAULTGLOBAL;

/**
 * Get the number of separators contained in a string. Separators include
 * ',', '|', ' ', and '\t'.
 * @param input to search for separators
 * @return the number of separators found
 */
static int32_t getSeparatorCount(const char* input) {
	int32_t index = 0, count = 0;
	if (input != NULL && *input != 0) {
		while (*(input + index) != 0) {
			if (*(input + index) == ',' ||
				*(input + index) == '|' ||
				*(input + index) == ' ' ||
				*(input + index) == '\t')
				count++;
			index++;
		}
		return count + 1;
	}
	return 0;
}

/**
 * Allocate an store the names of the HTTP headers which will be used. This
 * prevents repeated fetching later on.
 */
static void initHttpHeaders()
{
	global.importantHeaders = (const char**)malloc(
		sizeof(char*) * vmodfod_getHeaderCount(global.provider));

	for (int httpHeaderIndex = 0;
		httpHeaderIndex < vmodfod_getHeaderCount(global.provider);
		httpHeaderIndex++) {
			global.importantHeaders[httpHeaderIndex] =
				vmodfod_getHeaderName(global.provider, httpHeaderIndex);
	}
}
/**
 * VMOD Function
 * Get the name of the dataset.
 * @param ctx the context
 * @return dataset name
 */
VCL_STRING vmod_get_dataset_name(const struct vrt_ctx *ctx)
{
	char *p;
	unsigned u, v;

	u = WS_Reserve(ctx->ws, 0);
	p = ctx->ws->f;

	v = snprintf(p, u, "%s", vmodfod_getString(
		global.provider,
		vmodfod_getDataSet(global.provider)->header.nameOffset));
	v++;
	if (v > u) {
		WS_Release(ctx->ws, 0);
		return(NULL);
	}
	WS_Release(ctx->ws, v);
	return (p);
}

/**
 * VMOD Function
 * Get the format of the dataset.
 * @param ctx the context
 * @return dataset format
 */
VCL_STRING vmod_get_dataset_format(const struct vrt_ctx *ctx)
{
	char *p;
	unsigned u, v;

	u = WS_Reserve(ctx->ws, 0);
	p = ctx->ws->f;

	v = snprintf(p, u, "%s", vmodfod_getString(
		global.provider,
		vmodfod_getDataSet(global.provider)->header.formatOffset));
	v++;
	if (v > u) {
		WS_Release(ctx->ws, 0);
		return(NULL);
	}
	WS_Release(ctx->ws, v);
	return (p);
}

/**
 * VMOD Function
 * Get the date the dataset was published.
 * @param ctx the context
 * @return the published date
 */
VCL_STRING vmod_get_dataset_published_date(const struct vrt_ctx *ctx)
{
	char *p;
	unsigned u, v;

	u = WS_Reserve(ctx->ws, 0);
	p = ctx->ws->f;

	v = snprintf(p, u, "%d-%d-%d",
		vmodfod_getDataSet(global.provider)->header.published.year,
		(int)vmodfod_getDataSet(global.provider)->header.published.month,
		(int)vmodfod_getDataSet(global.provider)->header.published.day);
	v++;
	if (v > u) {
		WS_Release(ctx->ws, 0);
		return(NULL);
	}
	WS_Release(ctx->ws, v);
	return (p);
}

/**
 * VMOD Function
 * Get the number of signatures contained in the dataset.
 * @param the context
 * @return number of signatures
 */
VCL_STRING vmod_get_dataset_signature_count(const struct vrt_ctx *ctx)
{
	char *p;
	unsigned u, v;

	u = WS_Reserve(ctx->ws, 0);
	p = ctx->ws->f;

	int signatures = vmodfod_getDeviceCombinations(global.provider);
	if (signatures >= 0) {
		v = snprintf(p, u, "%d", signatures);
	}
	else {
		v = snprintf(p, u, "%s", FIFTYONEDEGREES_PROPERTY_NOT_FOUND);
	}
	v++;

	if (v > u) {
		WS_Release(ctx->ws, 0);
		return(NULL);
	}
	WS_Release(ctx->ws, v);
	return (p);
}

/**
 * VMOD Function
 * Get the number of device combinations in the dataset.
 * @param ctx the context
 * @return number of device combinations
 */
VCL_STRING vmod_get_dataset_device_combinations(const struct vrt_ctx *ctx)
{
	char *p;
	unsigned u, v;

	u = WS_Reserve(ctx->ws, 0);
	p = ctx->ws->f;

	v = snprintf(p, u, "%d", vmodfod_getDeviceCombinations(global.provider));
	v++;
	if (v > u) {
		WS_Release(ctx->ws, 0);
		return(NULL);
	}
	WS_Release(ctx->ws, v);
	return (p);
}

/**
 * VMOD Function
 * Set the size of the User-Agent cache.
 * @param ctx the context
 * @param size the size of the cache
 */
void vmod_set_cache(const struct vrt_ctx *ctx, VCL_INT size)
{
	global.cacheSize = size;
}

/**
 * VMOD Function
 * Set the properties which should be initialised in the dataset.
 * @param ctx the context
 * @param properties the list of required properties
 */
void vmod_set_properties(const struct vrt_ctx *ctx, VCL_STRING properties)
{
	global.requiredProperties = properties;
}

/**
 * VMOD Function
 * Set the size of the workset pool in the dataset.
 * @param ctx the context
 * @param size the size of the pool
 */
void vmod_set_pool(const struct vrt_ctx *ctx, VCL_INT size)
{
	global.poolSize = size;
}

/**
 * VMOD Function
 * Set the delimiter to use when returning values for multiple properties.
 * @param ctx the context
 * @param delimiter the delimiter string to use
 */
void vmod_set_delimiter(const struct vrt_ctx *ctx, VCL_STRING delimiter)
{
	global.propertyDelimiter = delimiter;
}

/**
 * Method used on shutdown to free the 51Degrees provider.
 * @param ptr pointer to the provider to free
 */
void privProviderFree(void *ptr)
{
	if (global.status == DATA_SET_INIT_STATUS_SUCCESS)
		fiftyoneDegreesProviderFree((fiftyoneDegreesProvider*)ptr);
	free(ptr);
}

#if FIFTYONEDEGREES_VARNISH_VERSION < 040100

/**
 * VMOD Init
 * Initialise the 51Degrees provider by allocating memory ready to be
 * populated.
 * @param priv the private memory pointer for the module
 * @param cfg the configuration
 * @return return code
 */
int init_function(struct vmod_priv *priv, const struct VCL_conf *cfg)
{
	(void)cfg;

	priv->priv = malloc(sizeof(fiftyoneDegreesProvider));
	global.provider = (fiftyoneDegreesProvider*) priv->priv;
	priv->free = privProviderFree;
	return (0);
}

#else

/**
 * VMOD Event
 * Initialise the 51Degrees provider by allocating memory ready to be
 * populated.
 * @param ctx the context
 * @param priv the private memory pointer for the module
 * @param e the event identifier
 * @return return code
 */
int init_function(
	const struct vrt_ctx *ctx,
	struct vmod_priv *priv,
	enum vcl_event_e e)
{
    switch (e) {
        case VCL_EVENT_LOAD:
            priv->priv = malloc(sizeof(fiftyoneDegreesProvider));
            global.provider = (fiftyoneDegreesProvider*) priv->priv;
            priv->free = privProviderFree;
            return (0);
        default:
            return (0);
    }
    return (0);
}

#endif

/**
 * VMOD Function
 * Start the 51Degrees module using the data file provided.
 * @param ctx the context
 * @param filePath the path to the data file to use
 */
void vmod_start(const struct vrt_ctx *ctx, VCL_STRING filePath)
{
	global.status = vmodfod_initProvider(
        filePath,
		global.provider,
        global.requiredProperties,
        global.poolSize,
        global.cacheSize);

	if (global.status != DATA_SET_INIT_STATUS_SUCCESS) {
		// The initialisation was unsuccessful, so throw the correct error
		// message.
		switch (global.status) {
			case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
				fprintf(stderr,
					"vmod_fiftyonedegrees: Insufficient memory allocated.\n");
			break;
			case DATA_SET_INIT_STATUS_CORRUPT_DATA:
				fprintf(stderr,
					"vmod_fiftyonedegrees: The data was not in the correct "
					"format. Check the data file is uncompressed.\n");
			break;
			case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
				fprintf(stderr,
					"vmod_fiftyonedegrees: The data is an unsupported "
					"version. Check you have the latest data and API.\n");
			break;
			case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
				fprintf(stderr,
					"vmod_fiftyonedegrees: The data file '%s' could not be "
					"found. Check the file path and that the program has "
					"sufficient read permissions.\n",
					filePath);
			break;
			case DATA_SET_INIT_STATUS_NULL_POINTER:
				fprintf(stderr,
					"vmod_fiftyonedegrees: Null pointer to the existing "
					"dataset or memory location.\n");
			break;
			case DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS:
				fprintf(stderr,
					"vmod_fiftyonedegrees: Allocated continuous memory "
					"containing 51Degrees data file appears to be smaller "
					"than expected. Most likely because the data file was "
					"not fully loaded into the allocated memory.\n");
			break;
			case DATA_SET_INIT_STATUS_NOT_SET:
			default:
				fprintf(stderr,
					"vmod_fiftyonedegrees: Could not create data set from "
					"file.\n");
			break;
		}

		abort();
	}

	// Get the names of the important headers from the data set.
	initHttpHeaders();
}

/**
 * Get the value of the header for the request context.
 * @param ctx the context
 * @param headerName the name of the header to get the value for
 * @return the value of the header
 */
char* searchHeaders(const struct vrt_ctx *ctx, const char *headerName)
{
	char *currentHeader;
	int i;
	for (i = 0; i < ctx->http_req->nhd; i++)
	{
		currentHeader = (char*)ctx->http_req->hd[i].b;
		if (currentHeader != NULL
			&& strncmp(currentHeader, headerName, strlen(headerName)) == 0)
		{
			return currentHeader + strlen(headerName) + 2;
		}
	}

	return NULL;
}

/**
 * Populate an array of strings from a comma separated string. The comma
 * characters are replaced with null terminators to allow the array elements
 * to point to the strings directly.
 * @param propertiesString the string containing property names
 * @param propertiesArray memory allocated for the array
 * @param propertiesCount the number of properties contained in the string
 * @return a newly allocated array pointing to the property names
 */
void stringToArray(
	char *propertiesString,
	char **propertiesArray,
	int propertiesCount)
{
	int charPos, index;
	int propertiesStringLength = strlen(propertiesString);

	index = 0;
	propertiesArray[index++] = propertiesString;

	for (charPos = 0; charPos < propertiesStringLength; charPos++)
	{
		if (propertiesString[charPos] == ',') {
			propertiesString[charPos] = '\0';
			propertiesArray[index++] = propertiesString + charPos + 1;
		}
	}
}

/**
 * Write the values for the required property to the workspace memory.
 * @param device the matched device to get the values from
 * @param requiredPropertyName name of the required property
 * @param p pointer to the point in workspace memory to print the values
 * @param u the length of the available memory to write to
 * @return the number of bytes written
 */
unsigned getValue(
	VMODFOD_DEVICE device,
	char *requiredPropertyName,
	char *p,
	unsigned u)
{
	int i;
	unsigned v;
	char *currentPropertyName;
	bool found = false;

#ifdef FIFTYONEDEGREES_PATTERN
    if (strcmp(requiredPropertyName, "Method") == 0)
    {
		switch(device->method) {
			case EXACT: v = snprintf(p, u, "Exact"); break;
			case NUMERIC: v = snprintf(p, u, "Numeric"); break;
			case NEAREST: v = snprintf(p, u, "Nearest"); break;
			case CLOSEST: v = snprintf(p, u, "Closest"); break;
			default:
			case NONE: v = snprintf(p, u, "None"); break;
		}
    }
    else if (strcmp(requiredPropertyName, "Difference") == 0)
    {
        v = snprintf(p, u, "%d", device->difference);
    }
    else if (strcmp(requiredPropertyName, "DeviceId") == 0)
    {
        v = fiftyoneDegreesGetDeviceId(device, p, 24);
    }
    else if (strcmp(requiredPropertyName, "Rank") == 0)
    {
        v = snprintf(p, u, "%d", fiftyoneDegreesGetSignatureRank(device));
    }
#endif
#ifdef FIFTYONEDEGREES_HASH
    if (strcmp(global.requiredProperties, "DeviceId") == 0) {
    	// The name is different in Hash, so redirect it.
        v = getValue(device, "Id", p, u);
    }
#endif
	else {
		v = 0;
        // Property is not a match metric, so search the required properties.
        for (i = 0; i < vmodfod_getRequiredPropertyCount(global.provider); i++)
        {
            currentPropertyName =
				(char*)vmodfod_getRequiredPropertyName(global.provider, i);
            if (strcmp(currentPropertyName, requiredPropertyName) == 0)
            {
            	// This if the property we want, so write the values to the
            	// workspace
            	v = vmodfod_writeValue(device, i, p, u, v);
            	// Found the property, so stop looking.
                found = true;
                break;
            }
        }
        if (!found)
            // Property was not found, so set value accordingly.
            v = snprintf(p, u, FIFTYONEDEGREES_PROPERTY_NOT_FOUND);
    }
    return v;
}

/**
 * Write the values for all the requested properties to the workspace memory.
 * @param p pointer to the point in workspace memory to print the values
 * @param u the length of the available memory to write to
 * @param device the matched device to get the values from
 * @param propertiesArray the array of property names to write the values of
 * @param propertiesCount the number of properties in the array
 * @return the number of bytes written
 */
unsigned printValuesToWorkspace(
	char *p,
	unsigned u,
	VMODFOD_DEVICE device,
	char **propertiesArray,
	int propertiesCount)
{
	int i;
	unsigned v = 0;
	for (i = 0; i < propertiesCount; i++)
	{
		if (i != 0) {
			v += snprintf(p + v, u, "%s", global.propertyDelimiter);
			if (v > u) {
				// Break now as we will only be printing to another workspace.
				return v;
			}
		}
		v += getValue(device, propertiesArray[i], p + v, u);
		if (v > u) {
			// Break now as we will only be printing to another workspace.
			return v;
		}
	}

	return v;
}

/**
 * Add the important headers and their values to the device ready for a match
 * to be carried out.
 * @param ctx the context to get the headers from
 * @param device the device to add the headers to
 */
void setImportantHeaders(const struct vrt_ctx *ctx, VMODFOD_DEVICE device)
{
	int headerIndex;
	char *headerValue;

	// Loop over all important headers.
	for (headerIndex = 0;
	     headerIndex < vmodfod_getHeaderCount(global.provider);
	     headerIndex++)
	{
		// Look for the current header in the request.
		headerValue = searchHeaders(
			ctx,
			vmodfod_getHeaderName(global.provider, headerIndex));
		if (headerValue) {
			// The request contains the header, so add it to the important
			// headers to be matched.
			vmodfod_addHeaderValue(device, headerIndex, headerValue);
		}
	}
}

/**
 * VMOD Function
 * Match the User-Agent provided, and return the values for the properties
 * requested for the matched device.
 * @param ctx the context (the User-Agent does not come from here in this
 * instance, it is instead provided as an argument)
 * @param userAgent the User-Agent string to match
 * @param propertyInputString the list of property names to return the values
 * or
 * @return the values of the requested properties for the device matched with
 * the supplied User-Agent
 */
VCL_STRING vmod_match_single(
	const struct vrt_ctx *ctx,
	VCL_STRING userAgent,
	VCL_STRING propertyInputString)
{
	// The pointer in workspace memory to print to.
	char *p;
	// The length of memory reserved, and the length that has been
	// printed respectively.
	unsigned u, v = 0;

	int propertiesCount;
	char **propertiesArray;
	char *propertiesString, *returnString;

	// Reserve some workspace.
	u = WS_Reserve(ctx->ws, 0);
	// Get pointer to the front of the workspace.
	p = ctx->ws->f;

	// Copy the properties string to the workspace memory.
	v += snprintf(p, u, "%s", propertyInputString);
	if (v > u) {
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	// Skip over the null terminator.
	v++;

	// This string is modifiable unlike propertyInputString, so getProperties
	// can replace separators with null terminators.
	propertiesString = p;

	// Get the number of properties in the properties string.
	propertiesCount = getSeparatorCount(propertiesString);
	// Create a properties array by using pointers to the correct point in the
	// properties string.
	propertiesArray = (char**)p + v;
	v += sizeof(char**) * propertiesCount;
	if (v > u) {
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	stringToArray(propertiesString, propertiesArray, propertiesCount);
	// Move over the array memory.
	returnString = p + v;

    // Get a device instance to work on.
	VMODFOD_DEVICE device = vmodfod_createDevice(global.provider);

	// Get a match for the User-Agent supplied and store in the device
	// instance.
	vmodfod_matchDeviceSingle(device, userAgent);

	// Print the values to the workspace memory that has been reserved.
	v += printValuesToWorkspace(
		p + v,
		u,
		device,
		propertiesArray,
		propertiesCount);

	// Skip over the null terminator.
	v++;

    // Release any resources used by the device instance.
	vmodfod_releaseDevice(device);

	if (v > u) {
		// No space, reset and leave.
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	// Update workspace with what has been used.
	WS_Release(ctx->ws, v);
	return (returnString);
}

/**
 * VMOD Function
 * Match the HTTP request headers, and return the values for the properties
 * requested for the matched device.
 * @param ctx the context to get the HTTP headers from
 * @param propertyInputString the list of property names to return the values
 * or
 * @return the values of the requested properties for the device matched with
 * the request headers
 */
VCL_STRING vmod_match_all(
	const struct vrt_ctx *ctx,
	VCL_STRING propertyInputString)
{
	// The pointer in workspace memory to print to.
	char *p;
	// The length of memory reserved, and the length that has been
	// printed respectively.
	unsigned u, v = 0;

	char *propertiesString, *returnString;
	int propertiesCount;
	char **propertiesArray;

	// Reserve some workspace.
	u = WS_Reserve(ctx->ws, 0);
	// Pointer to the front of workspace area.
	p = ctx->ws->f;

	// Copy the properties string the the workspace memory.
	v += snprintf(p, u, "%s", propertyInputString);
	if (v > u) {
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	// Skip over the null terminator.
	v++;

    // Get a device instance to work on.
	VMODFOD_DEVICE device = vmodfod_createDevice(global.provider);

	// Set the headers in the device ready for the match.
	setImportantHeaders(ctx, device);

	// Get a match for the headers that have just been added and store in
	// the device instance.
	vmodfod_matchDeviceAll(device);

	// This string is modifiable unlike propertyInputString, so getProperties
	// can replace separators with null terminators.
	propertiesString = p;

	// Get the number of properties in the properties string.
	propertiesCount = getSeparatorCount(propertiesString);
	// Create a properties array by using pointers to the correct point in the
	// properties string.
	propertiesArray = (char**)p + v;
	v += sizeof(char**) * propertiesCount;
	if (v > u) {
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	stringToArray(propertiesString, propertiesArray, propertiesCount);
	// Move over the array memory.
	returnString = p + v;

	// Print the values to the workspace memory that has been reserved.
	v += printValuesToWorkspace(
		p + v,
		u,
		device,
		propertiesArray,
		propertiesCount);

	// Skip over the null terminator.
	v++;

    // Release any resources used by the device instance.
	vmodfod_releaseDevice(device);

	if (v > u) {
		// No space, reset and leave.
		WS_Release(ctx->ws, 0);
		return (NULL);
	}

	// Update work space with what has been used.
	WS_Release(ctx->ws, v);
	return (returnString);
}
