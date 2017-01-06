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

typedef enum { false, true } bool;

// Global provider available to the module.
fiftyoneDegreesProvider *provider;

const char **importantHeaders;

static void
addValue(const char *delimiter, char *buffer, const char *str)
{
	if (buffer[0] != '\0')
	{
		strcat(buffer, delimiter);
	}
	strcat(buffer, str);
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

void
vmod_start(const struct vrt_ctx *ctx, VCL_STRING filePath)
{
    // Allocate and initialise the provider.
	provider = (fiftyoneDegreesProvider*)malloc(sizeof(fiftyoneDegreesProvider));
	fiftyoneDegreesInitProviderWithPropertyString(
        filePath,
        provider,
        "",
        0,
        0);

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

void
getValue(fiftyoneDegreesWorkset *fodws, const char **valueName, const char *requiredPropertyName)
{
	int i, j;
    // TODO set max buffer length properly.
    char buffer[1000];
	bool found = false;
	char *currentPropertyName;
	    // Get the requested property value from the match.
    if (strcmp(requiredPropertyName, "Method") == 0)
    {
		switch(fodws->method) {
			case EXACT: *valueName = "Exact"; break;
			case NUMERIC: *valueName = "Numeric"; break;
			case NEAREST: *valueName = "Nearest"; break;
			case CLOSEST: *valueName = "Closest"; break;
			default:
			case NONE: *valueName = "None"; break;
		}
    }
    else if (strcmp(requiredPropertyName, "Difference") == 0)
    {
        sprintf(buffer, "%d", fodws->difference);
        *valueName = buffer;
    }
    else if (strcmp(requiredPropertyName, "DeviceId") == 0)
    {
        fiftyoneDegreesGetDeviceId(fodws, buffer, 24);
        *valueName = buffer;
    }
    else if (strcmp(requiredPropertyName, "Rank") == 0)
    {
        sprintf(buffer, "%d", fiftyoneDegreesGetSignatureRank(fodws));
        *valueName = buffer;
    }
	else {
        // Property is not a match metric, so search the required properties.
        for (i = 0; i < fodws->dataSet->requiredPropertyCount; i++)
        {
            currentPropertyName = (char*)fiftyoneDegreesGetPropertyName(fodws->dataSet, fodws->dataSet->requiredProperties[i]);
            if (strcmp(currentPropertyName, requiredPropertyName) == 0)
            {
            	buffer[0] = '\0';
                fiftyoneDegreesSetValues(fodws, i);
                for (j = 0; j < fodws->valuesCount; j++)
                {
                	addValue("|", buffer, fiftyoneDegreesGetValueName(fodws->dataSet, fodws->values[j]));
                }
                *valueName = buffer;
                found = true;
                break;
            }
        }
        if (!found)
            // Property was not found, so set value accordingly.
            *valueName = "N/A";
    }
}

VCL_STRING
vmod_match_single(const struct vrt_ctx *ctx, VCL_STRING userAgent, VCL_STRING propertyInputString)
{

	char *p;
	unsigned u, v;
	const char *valueName;

    // Create a workset to use for the match.
	fiftyoneDegreesWorkset *fodws = fiftyoneDegreesWorksetCreate(provider->activePool->dataSet, NULL);

	fiftyoneDegreesMatch(fodws, userAgent);

	getValue(fodws, &valueName, propertyInputString);

	u = WS_Reserve(ctx->ws, 0); /* Reserve some work space */
	p = ctx->ws->f;	            /* Front of workspace area */
	v = snprintf(p, u, "%s", valueName);

	v++;

    // Free the workset.
	fiftyoneDegreesWorksetFree(fodws);

	if (v > u) {
		/* No space, reset and leave */
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	/* Update work space with what we've used */
	WS_Release(ctx->ws, v);
	return (p);
}

VCL_STRING
vmod_match_all(const struct vrt_ctx *ctx, VCL_STRING propertyInputString)
{
	char *p;
	unsigned u, v;
	const char *valueName;

    // Create a workset to use for the match.
	fiftyoneDegreesWorkset *fodws = fiftyoneDegreesWorksetCreate(provider->activePool->dataSet, NULL);

	int headerIndex;
	char *searchResult;
    // Get a match from the HTTP headers.
	fodws->importantHeadersCount = 0;
	for (headerIndex = 0; headerIndex < fodws->dataSet->httpHeadersCount; headerIndex++) {
		searchResult = searchHeaders(ctx, fodws->dataSet->httpHeaders[headerIndex].headerName);
		if (searchResult) {
			fodws->importantHeaders[fodws->importantHeadersCount].header = fodws->dataSet->httpHeaders + headerIndex;
			fodws->importantHeaders[fodws->importantHeadersCount].headerValue = (const char*)searchResult;
			fodws->importantHeaders[fodws->importantHeadersCount].headerValueLength = strlen(searchResult);
			fodws->importantHeadersCount++;
		}
		fiftyoneDegreesMatchForHttpHeaders(fodws);
	}

	getValue(fodws, &valueName, propertyInputString);

	u = WS_Reserve(ctx->ws, 0); /* Reserve some work space */
	p = ctx->ws->f;	            /* Front of workspace area */
	v = snprintf(p, u, "%s", valueName);

	v++;

    // Free the workset.
	fiftyoneDegreesWorksetFree(fodws);

	if (v > u) {
		/* No space, reset and leave */
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	/* Update work space with what we've used */
	WS_Release(ctx->ws, v);
	return (p);
}
