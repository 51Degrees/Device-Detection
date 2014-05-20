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

#include "..\..\src\trie\51Degrees.h"
#include "Windows.h"

#define EXTERN_DLL_EXPORT __declspec(dllexport)

EXTERN_DLL_EXPORT void __cdecl Init(LPCTSTR fileName, LPCTSTR properties)
{
	init((char*)fileName, (char*)properties);
}

EXTERN_DLL_EXPORT void __cdecl Destroy()
{
	destroy();
}

EXTERN_DLL_EXPORT int __cdecl GetPropertiesCSV(LPCTSTR userAgent, LPTSTR result, DWORD resultLength)
{
	return processDeviceCSV(getDeviceOffset((char*)userAgent), (char*)result, resultLength);
}