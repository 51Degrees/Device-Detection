/*
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 *
 * This Source Code Form is the subject of the following patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent Application No. 13192291.6; and
 * United States Patent Application Nos. 14/085,223 and 14/085,301.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "vrt.h"
#include "cache/cache.h"
#include "src/pattern/51Degrees.h"

#include "vcc_if.h"

#ifndef FIFTYONEDEGREES_PROPERTY_NOT_FOUND
#define FIFTYONEDEGREES_PROPERTY_NOT_FOUND "N/A"
#endif // FIFTYONEDEGREES_PROPERTY_NOT_FOUND

typedef enum { false, true } bool;

// Global provider available to the module.
fiftyoneDegreesProvider *provider = NULL;

// Array of pointers to the important header name strings.
const char **importantHeaders;
/*
static void
addValue(
		const char *delimiter,
		char *buffer, const char *str)
{
	if (buffer[0] != '\0')
	{
		strcat(buffer, delimiter);
	}
	strcat(buffer, str);
}
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

static void
initHttpHeaders()
{
	importantHeaders = (const char**)malloc(sizeof(char*) * provider->activePool->dataSet->httpHeadersCount);
	for (int httpHeaderIndex = 0;
		httpHeaderIndex < provider->activePool->dataSet->httpHeadersCount;
		httpHeaderIndex++) {
			importantHeaders[httpHeaderIndex] = provider->activePool->dataSet->httpHeaders[httpHeaderIndex].headerName;
	}
}

int cacheSize = 0,
poolSize = 20;
const char *requiredProperties = "";
const char *propertyDelimiter = ",";
fiftyoneDegreesDataSetInitStatus status = DATA_SET_INIT_STATUS_NOT_SET;

VCL_STRING vmod_get_version(const struct vrt_ctx *ctx)
{
	char *p;
	unsigned u, v;

	// Reserve some work space.
	u = WS_Reserve(ctx->ws, 0);
	// Get pointer to the front of the work space.
	p = ctx->ws->f;
	// Print the value to memory that has been reserved.
	v = snprintf(p, u, "%d.%d.%d.%d", provider->activePool->dataSet->header.versionMajor,
				provider->activePool->dataSet->header.versionMinor,
				provider->activePool->dataSet->header.versionBuild,
				provider->activePool->dataSet->header.versionRevision);

	v++;

	if (v > u) {
		// No space, reset and leave.
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	// Update work space with what has been used.
	WS_Release(ctx->ws, v);
	return (p);
}

void vmod_set_cache(const struct vrt_ctx *ctx, VCL_INT size)
{
	cacheSize = size;
}

void vmod_set_properties(const struct vrt_ctx *ctx, VCL_STRING properties)
{
	requiredProperties = properties;
}

void vmod_set_pool(const struct vrt_ctx *ctx, VCL_INT size)
{
	poolSize = size;
}

void vmod_set_delimiter(const struct vrt_ctx *ctx, VCL_STRING delimiter)
{
	propertyDelimiter = delimiter;
}

void privProviderFree(void *ptr)
{
	if (status == DATA_SET_INIT_STATUS_SUCCESS)
		fiftyoneDegreesProviderFree((fiftyoneDegreesProvider*)ptr);
	free(ptr);
}

int init_function(struct vmod_priv *priv, const struct VCL_conf *cfg)
{
	(void)cfg;

	priv->priv = malloc(sizeof(fiftyoneDegreesProvider));
	provider = (fiftyoneDegreesProvider*) priv->priv;
	priv->free = privProviderFree;
	return (0);
}

void
vmod_start(
		const struct vrt_ctx *ctx,
		VCL_STRING filePath)
{
	status = fiftyoneDegreesInitProviderWithPropertyString(
        filePath,
        provider,
        requiredProperties,
        poolSize,
        cacheSize);

	// TODO log the status.

	// Get the names of the important headers from the data set.
	initHttpHeaders();
}

char*
searchHeaders(const struct vrt_ctx *ctx, const char *headerName)
{
	char *currentHeader;
	int i;
	for (i = 0; i < ctx->http_req->nhd; i++)
	{
		currentHeader = ctx->http_req->hd[i].b;
		if (currentHeader != NULL
			&& strncmp(currentHeader, headerName, strlen(headerName)) == 0)
		{
			return currentHeader + strlen(headerName) + 2;
		}
	}

	return NULL;
}

char** getProperties(char *propertiesString, int propertiesCount)
{
	int charPos, index;
	int propertiesStringLength = strlen(propertiesString);

	// TODO use workspace memory instead of malloc.
	char **propertiesArray = (char**)malloc(sizeof(char*) * propertiesCount);
	index = 0;
	propertiesArray[index++] = propertiesString;

	for (charPos = 0; charPos < propertiesStringLength; charPos++)
	{
		if (propertiesString[charPos] == ',') {
			propertiesString[charPos] = '\0';
			propertiesArray[index++] = propertiesString + charPos + 1;
		}
	}

	return propertiesArray;
}

unsigned
getValue(
		fiftyoneDegreesWorkset *fodws,
		char *requiredPropertyName,
		char *p,
		unsigned u)
{
	int i, j;
	unsigned v;
	char *currentPropertyName;
	bool found = false;

    if (strcmp(requiredPropertyName, "Method") == 0)
    {
		switch(fodws->method) {
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
        v = snprintf(p, u, "%d", fodws->difference);
    }
    else if (strcmp(requiredPropertyName, "DeviceId") == 0)
    {
        v = fiftyoneDegreesGetDeviceId(fodws, p, 24);
    }
    else if (strcmp(requiredPropertyName, "Rank") == 0)
    {
        v = snprintf(p, u, "%d", fiftyoneDegreesGetSignatureRank(fodws));
    }
	else {
		v = 0;
        // Property is not a match metric, so search the required properties.
        for (i = 0; i < fodws->dataSet->requiredPropertyCount; i++)
        {
            currentPropertyName
            = (char*)fiftyoneDegreesGetPropertyName(fodws->dataSet, fodws->dataSet->requiredProperties[i]);
            if (strcmp(currentPropertyName, requiredPropertyName) == 0)
            {
            	// This if the property we want, so set the values and go
            	// through them all.
                fiftyoneDegreesSetValues(fodws, i);
                for (j = 0; j < fodws->valuesCount; j++)
                {
                	if (j != 0) {
						// Print a separator between values.
						v += snprintf(p + v, u, "|");
                	}
                	// Print the value to the values string.
                	v += snprintf(p + v, u, "%s", fiftyoneDegreesGetValueName(fodws->dataSet, fodws->values[j]));

                }
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

unsigned printValuesToWorkspace(
								char *p,
								unsigned u,
								fiftyoneDegreesWorkset *fodws,
								char **propertiesArray,
								int propertiesCount)
{
	int i;
	unsigned v = 0;
	for (i = 0; i < propertiesCount; i++)
	{
		if (i != 0) {
			v += snprintf(p + v, u, "%s", propertyDelimiter);
			if (v > u) {
				// Break now as we will only be printing to another workspace.
				return v;
			}
		}
		v += getValue(fodws, propertiesArray[i], p + v, u);
		if (v > u) {
			// Break now as we will only be printing to another workspace.
			return v;
		}
	}

	return v;
}

void setImportantHeaders(const struct vrt_ctx *ctx, fiftyoneDegreesWorkset *fodws)
{
	int headerIndex;
	char *searchResult;

    // Reset the headers count before adding any to the workset.
	fodws->importantHeadersCount = 0;
	// Loop over all important headers.
	for (headerIndex = 0;
	     headerIndex < fodws->dataSet->httpHeadersCount;
	     headerIndex++)
	{
		// Look for the current header in the request.
		searchResult
			= searchHeaders(ctx, fodws->dataSet->httpHeaders[headerIndex].headerName);
		if (searchResult) {
			// The request contains the header, so add it to the important
			// headers in the workset.
			fodws->importantHeaders[fodws->importantHeadersCount].header
				= fodws->dataSet->httpHeaders + headerIndex;
			fodws->importantHeaders[fodws->importantHeadersCount].headerValue
				= (const char*)searchResult;
			fodws->importantHeaders[fodws->importantHeadersCount].headerValueLength
				= strlen(searchResult);
			fodws->importantHeadersCount++;
		}
	}
}

VCL_STRING
vmod_match_single(
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

	// Reserve some work space.
	u = WS_Reserve(ctx->ws, 0);
	// Get pointer to the front of the work space.
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
	returnString = p + v;

	// Get the number of properties in the properties string.
	propertiesCount = getSeparatorCount(propertiesString);
	// Create a properties array by using pointers to the correct point in the
	// properties string.
	propertiesArray = getProperties(propertiesString, propertiesCount);

    // Get a workset from the pool to use for the match.
	fiftyoneDegreesWorkset *fodws
		= fiftyoneDegreesProviderWorksetGet(provider);

	// Get a match for the User-Agent supplied and store in the workset.
	fiftyoneDegreesMatch(fodws, userAgent);

	// Print the values to the workspace memory that has been reserved.
	v += printValuesToWorkspace(p + v, u, fodws, propertiesArray, propertiesCount);
	// Skip over the null terminator.
	v++;

    // Return the workset to the pool.
    fiftyoneDegreesWorksetRelease(fodws);

    free(propertiesArray);

	if (v > u) {
		// No space, reset and leave.
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	// Update work space with what has been used.
	WS_Release(ctx->ws, v);
	return (returnString);
}

VCL_STRING
vmod_match_all(
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

	// Reserve some work space.
	u = WS_Reserve(ctx->ws, 0);
	// Pointer to the front of work space area.
	p = ctx->ws->f;

	// Copy the properties string the the workspace memory.
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
	returnString = p + v;

    // Fetch a workset to use for the match.
	fiftyoneDegreesWorkset *fodws
		= fiftyoneDegreesProviderWorksetGet(provider);

	// Set the headers in the workset ready for the match.
	setImportantHeaders(ctx, fodws);

	// Get a match for the headers that have just been added and store in
	// the workset.
	fiftyoneDegreesMatchForHttpHeaders(fodws);

	// Get the number of properties in the properties string.
	propertiesCount = getSeparatorCount(propertiesString);
	// Create a properties array by using pointers to the correct point in the
	// properties string.
	propertiesArray = getProperties(propertiesString, propertiesCount);

	// Print the values to the workspace memory that has been reserved.
	v += printValuesToWorkspace(p + v, u, fodws, propertiesArray, propertiesCount);

	// Skip over the null terminator.
	v++;

    // Return the workset to the pool.
    fiftyoneDegreesWorksetRelease(fodws);

    free(propertiesArray);

	if (v > u) {
		// No space, reset and leave.
		WS_Release(ctx->ws, 0);
		return (NULL);
	}

	// Update work space with what has been used.
	WS_Release(ctx->ws, v);
	return (returnString);
}
