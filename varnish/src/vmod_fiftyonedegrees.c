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

fiftyoneDegreesProvider *provider;

void
vmod_start(const struct vrt_ctx *ctx, VCL_STRING name)
{
	provider = (fiftyoneDegreesProvider*)malloc(sizeof(fiftyoneDegreesProvider));
	fiftyoneDegreesInitProviderWithPropertyString(name,
															provider,
															"",
															0,
															0);
}

VCL_STRING
vmod_match(const struct vrt_ctx *ctx, VCL_STRING name)
{
	char *p;
	unsigned u, v;
	const char *propertyName, *valueName;
	int i;

	fiftyoneDegreesWorkset *fodws = fiftyoneDegreesWorksetCreate(provider->activePool->dataSet, NULL);

	const char *userAgent = "";

	for (i = 0; i < ctx->http_req->nhd; i++)
	{
		if (ctx->http_req->hd[i].b != NULL
			&& strncmp(ctx->http_req->hd[i].b, "User-Agent", 10) == 0)
		{
			userAgent = ctx->http_req->hd[i].b + 12;
			break;
		}
	}

	fiftyoneDegreesMatch(fodws, userAgent);

    if (strcmp(name, "Method") == 0)
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
    else if (strcmp(name, "Difference") == 0)
    {
        char buffer[10];
        sprintf(buffer, "%d", fodws->difference);
        valueName = buffer;
    }
	else {
        for (i = 0; i < fodws->dataSet->requiredPropertyCount; i++)
        {
            propertyName = (char*)fiftyoneDegreesGetPropertyName(fodws->dataSet, fodws->dataSet->requiredProperties[i]);
            if (strcmp(propertyName, name) == 0)
            {
                fiftyoneDegreesSetValues(fodws, i);
                valueName = fiftyoneDegreesGetValueName(fodws->dataSet, *fodws->values);
                break;
            }
        }
    }

	u = WS_Reserve(ctx->ws, 0); /* Reserve some work space */
	p = ctx->ws->f;		/* Front of workspace area */
	v = snprintf(p, u, "%s", valueName);

	v++;
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
