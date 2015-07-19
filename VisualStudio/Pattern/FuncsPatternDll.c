/* *********************************************************************
* This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
* Copyright 2014 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
********************************************************************** */

#include "..\..\src\pattern\51Degrees.h"
#include "Windows.h"

#define EXTERN_DLL_EXPORT __declspec(dllexport)

EXTERN_DLL_EXPORT LP __cdecl InitWithPropertyString(LPCTSTR fileName, LPCTSTR properties)
{
	fiftyoneDegreesDataSet *ds = (fiftyoneDegreesDataSet*)malloc(sizeof(fiftyoneDegreesDataSet));
	fiftyoneDegreesDataSetInitStatus status = fiftyoneDegreesInitWithPropertyString((char*)fileName, ds, (char*)properties);
	switch (status) {
		case DATA_SET_INIT_STATUS_SUCCESS:
			break;
		case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
		case DATA_SET_INIT_STATUS_CORRUPT_DATA:
		case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
		case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
		case DATA_SET_INIT_STATUS_NOT_SET:
		default:
			SetLastError((DWORD)status);
			free(ds);
			ds = NULL;
			break;
	}
	return (LP)ds;
}

EXTERN_DLL_EXPORT void __cdecl DataSetFree(LP dataSet)
{
	fiftyoneDegreesDataSetFree((fiftyoneDegreesDataSet*)dataSet);
}

EXTERN_DLL_EXPORT LP __cdecl ResultsetCacheCreate(LP dataSet, int size)
{
	fiftyoneDegreesResultsetCache *cache = NULL;
	cache = fiftyoneDegreesResultsetCacheCreate((fiftyoneDegreesDataSet*)dataSet, size);
	return (LP)cache;
}

EXTERN_DLL_EXPORT void __cdecl ResultsetCacheFree(LP cache)
{
	fiftyoneDegreesResultsetCacheFree((fiftyoneDegreesResultsetCache*)cache);
}

EXTERN_DLL_EXPORT LP __cdecl WorksetPoolCreate(LP dataSet, LP cache, int size)
{
	fiftyoneDegreesWorksetPool *pool = NULL;
	pool = fiftyoneDegreesWorksetPoolCreate((fiftyoneDegreesDataSet*)dataSet, (fiftyoneDegreesResultsetCache*)cache, size);
	return (LP)pool;
}

EXTERN_DLL_EXPORT void __cdecl WorksetPoolFree(LP pool)
{
	fiftyoneDegreesWorksetPoolFree((fiftyoneDegreesWorksetPool*)pool);
}

EXTERN_DLL_EXPORT LP __cdecl WorksetPoolGet(LP pool)
{
	return (LP)fiftyoneDegreesWorksetPoolGet((fiftyoneDegreesWorksetPool*)pool);
}

EXTERN_DLL_EXPORT void __cdecl WorksetPoolRelease(LP pool, LP ws)
{
	fiftyoneDegreesWorksetPoolRelease((fiftyoneDegreesWorksetPool*)pool, (fiftyoneDegreesWorkset*)ws);
}

EXTERN_DLL_EXPORT void __cdecl MatchFromUserAgent(LP workSet, LPCTSTR userAgent)
{
	fiftyoneDegreesMatch((fiftyoneDegreesWorkset*)workSet, (char*)userAgent);
}

EXTERN_DLL_EXPORT void __cdecl MatchFromHeaders(LP workSet, LPTSTR httpHeaders)
{
	fiftyoneDegreesMatchWithHeadersString((fiftyoneDegreesWorkset*)workSet, (char*)httpHeaders);
}

EXTERN_DLL_EXPORT int __cdecl GetHttpHeaderName(LP dataSet, INT httpHeaderIndex, LPTSTR httpHeader, INT size)
{
	return fiftyoneDegreesGetHttpHeaderName(
		(fiftyoneDegreesDataSet*)dataSet,
		httpHeaderIndex,
		(char*)httpHeader,
		size);
}

EXTERN_DLL_EXPORT int __cdecl GetRequiredPropertyName(LP dataSet, INT requiredPropertyIndex, LPTSTR propertyName, INT size)
{
	return fiftyoneDegreesGetRequiredPropertyName(
		(fiftyoneDegreesDataSet*)dataSet,
		requiredPropertyIndex,
		(char*)propertyName,
		size);
}

EXTERN_DLL_EXPORT int __cdecl GetRequiredPropertyIndex(LP dataSet, LPCTSTR propertyName) 
{
	return fiftyoneDegreesGetRequiredPropertyIndex(
		(fiftyoneDegreesDataSet*)dataSet,
		(char*)propertyName);
}

EXTERN_DLL_EXPORT int __cdecl GetPropertyValues(LP workSet, INT requiredPropertyIndex, LPTSTR values, INT size)
{
	return fiftyoneDegreesGetValues(
		(fiftyoneDegreesWorkset*)workSet,
		requiredPropertyIndex,
		(char*)values,
		size);
}