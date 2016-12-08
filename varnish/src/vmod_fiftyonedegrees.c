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

static void
addValue(const char *delimiter, char *buffer, const char *str)
{
	if (buffer[0] != '\0')
	{
		strcat(buffer, delimiter);
	}
	strcat(buffer, str);
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
}

VCL_STRING
vmod_match(const struct vrt_ctx *ctx, VCL_STRING propertyInputString)
{
	char *p;
	unsigned u, v;
	const char *propertyName, *valueName;
	int i, j;
    char buffer[1000];
	const char *userAgent = "";
    bool found = false;

    // Create a workset to use for the match.
	fiftyoneDegreesWorkset *fodws = fiftyoneDegreesWorksetCreate(provider->activePool->dataSet, NULL);

    // Get the User-Agent from the request.
	for (i = 0; i < ctx->http_req->nhd; i++)
	{
		if (ctx->http_req->hd[i].b != NULL
			&& strncmp(ctx->http_req->hd[i].b, "User-Agent", 10) == 0)
		{
			userAgent = ctx->http_req->hd[i].b + 12;
			break;
		}
	}

    // Get a match from the User-Agent.
	fiftyoneDegreesMatch(fodws, userAgent);

    // Get the requested property value from the match.
    if (strcmp(propertyInputString, "Method") == 0)
    {
		switch(fodws->method) {
			case EXACT: valueName = "Exact"; break;
			case NUMERIC: valueName = "Numeric"; break;
			case NEAREST: valueName = "Nearest"; break;
			case CLOSEST: valueName = "Closest"; break;
			default:
			case NONE: valueName = "None"; break;
		}
    }
    else if (strcmp(propertyInputString, "Difference") == 0)
    {
        sprintf(buffer, "%d", fodws->difference);
        valueName = buffer;
    }
    else if (strcmp(propertyInputString, "DeviceId") == 0)
    {
        fiftyoneDegreesGetDeviceId(fodws, buffer, 24);
        valueName = buffer;
    }
    else if (strcmp(propertyInputString, "Rank") == 0)
    {
        sprintf(buffer, "%d", fiftyoneDegreesGetSignatureRank(fodws));
        valueName = buffer;
    }
	else {
        // Property is not a match metric, so search the required properties.
        for (i = 0; i < fodws->dataSet->requiredPropertyCount; i++)
        {
            propertyName = (char*)fiftyoneDegreesGetPropertyName(fodws->dataSet, fodws->dataSet->requiredProperties[i]);
            if (strcmp(propertyName, propertyInputString) == 0)
            {
            	buffer[0] = '\0';
                fiftyoneDegreesSetValues(fodws, i);
                for (j = 0; j < fodws->valuesCount; j++)
                {
                	addValue("|", buffer, fiftyoneDegreesGetValueName(fodws->dataSet, fodws->values[j]));
                }
                valueName = buffer;
                found = true;
                break;
            }
        }
        if (!found)
            // Property was not found, so set value accordingly.
            valueName = "N/A";
    }

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
