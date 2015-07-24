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

#include "..\..\src\trie\51Degrees.h"
#include "Windows.h"

#define EXTERN_DLL_EXPORT __declspec(dllexport)

EXTERN_DLL_EXPORT int __cdecl InitWithPropertyString(LPCTSTR fileName, LPCTSTR properties)
{
	return fiftyoneDegreesInitWithPropertyString((char*)fileName, (char*)properties);
}

EXTERN_DLL_EXPORT void __cdecl Destroy()
{
	fiftyoneDegreesDestroy();
}

EXTERN_DLL_EXPORT void __cdecl FreeMatchResult(LP offsets)
{
	free((void*)offsets);
}

EXTERN_DLL_EXPORT LP __cdecl MatchFromUserAgent(LPCTSTR userAgent)
{
	fiftyoneDegreesDeviceOffsets* offsets = (fiftyoneDegreesDeviceOffsets*)malloc(sizeof(fiftyoneDegreesDeviceOffsets));
	offsets->size = 1;
	offsets->firstOffset.deviceOffset = fiftyoneDegreesGetDeviceOffset((char*)userAgent);
	return (LP)offsets;
}

EXTERN_DLL_EXPORT int __cdecl GetLastCharacterIndex(LPCTSTR userAgent)
{
	return fiftyoneDegreesGetLastCharacterIndex((char*)userAgent);
}

EXTERN_DLL_EXPORT LP __cdecl MatchFromHeaders(LPTSTR httpHeaders)
{
	return (LP)fiftyoneDegreesGetDeviceOffsetsWithHeadersString((char*)httpHeaders);
}

EXTERN_DLL_EXPORT int __cdecl GetHttpHeaderName(INT httpHeaderIndex, LPTSTR httpHeader, INT size)
{
	return fiftyoneDegreesGetHttpHeaderName(
		httpHeaderIndex,
		(char*)httpHeader,
		size);
}

EXTERN_DLL_EXPORT int __cdecl GetRequiredPropertyName(INT requiredPropertyIndex, LPTSTR propertyName, INT size)
{
	return fiftyoneDegreesGetRequiredPropertyName(
		requiredPropertyIndex,
		(char*)propertyName,
		size);
}

EXTERN_DLL_EXPORT int __cdecl GetPropertyValues(LP deviceOffsets, INT requiredPropertyIndex, LPTSTR values, INT size)
{
	return fiftyoneDegreesGetValueFromOffsets(
		(fiftyoneDegreesDeviceOffsets*)deviceOffsets,
		requiredPropertyIndex,
		(char*)values,
		size);
}