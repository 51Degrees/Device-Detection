/* *********************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 * ********************************************************************* */

#include "..\..\src\pattern\51Degrees.h"
#include "Windows.h"

#define EXTERN_DLL_EXPORT __declspec(dllexport)

EXTERN_DLL_EXPORT void __cdecl Destroy(LP dataSet)
{
	fiftyoneDegreesDestroy((fiftyoneDegreesDataSet*)dataSet);
	free((fiftyoneDegreesDataSet*)dataSet);
}

EXTERN_DLL_EXPORT LP __cdecl CreateDataSet(LPCTSTR fileName, LPCTSTR properties)
{
	fiftyoneDegreesDataSet *ds = NULL;
	int bytesRead;
	ds = (fiftyoneDegreesDataSet*)malloc(sizeof(fiftyoneDegreesDataSet));
	bytesRead = fiftyoneDegreesInitWithPropertyString((char*)fileName, ds, (char*)properties);
	if (bytesRead <= 0)
	{
		free(ds);
		ds = NULL;
	}
	return (LP)ds;
}

EXTERN_DLL_EXPORT LP __cdecl CreateWorkSet(LP dataSet)
{
	fiftyoneDegreesWorkset *ws = NULL;
	ws = fiftyoneDegreesCreateWorkset((fiftyoneDegreesDataSet*)dataSet);
	return (LP)ws;
}

EXTERN_DLL_EXPORT void __cdecl FreeWorkSet(LP workSet)
{
	fiftyoneDegreesFreeWorkset((fiftyoneDegreesWorkset*)workSet);
}

EXTERN_DLL_EXPORT int __cdecl GetPropertiesCSV(LP workSet, LPCTSTR userAgent, LPTSTR result, DWORD resultLength)
{
	// Populate the workset with the results for the provided
	// user agent.
	fiftyoneDegreesMatch((fiftyoneDegreesWorkset*)workSet, (char*)userAgent);

	// Add the results in CSV form to the string provided.
	return (int)fiftyoneDegreesProcessDeviceCSV(
		(fiftyoneDegreesWorkset*)workSet,
		(char*)result,
		resultLength);
}