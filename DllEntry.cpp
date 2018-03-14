/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements. The ASF licenses this
file to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.   
*/

#pragma once

#include "BerconCommon.h"


extern ClassDesc2* GetBerconNoiseDesc();
extern ClassDesc2* GetBerconWoodDesc();
extern ClassDesc2* GetBerconTileDesc();
extern ClassDesc2* GetBerconDistortionDesc();
extern ClassDesc2* GetBerconGradientDesc();
extern void InitGradientControls();

HINSTANCE hInstance;
int controlsInit = FALSE;


// This function is called by Windows when the DLL is loaded.  This 
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.

BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID /*lpvReserved*/)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		MaxSDK::Util::UseLanguagePackLocale();
		// Hang on to this DLL's instance handle.
		hInstance = hinstDLL;
		DisableThreadLibraryCalls(hInstance);
		// DO NOT do any initialization here. Use LibInitialize() instead.
	}
	return(TRUE);
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) int LibNumberClasses() {
	return 5;
}
// This function returns the names of plug-in classes this DLL
__declspec( dllexport ) ClassDesc* LibClassDesc(int i) {
	switch(i) {
		case 0: return GetBerconNoiseDesc();
		case 1: return GetBerconWoodDesc();
		case 2: return GetBerconTileDesc();
		case 3: return GetBerconDistortionDesc();
		case 4: return GetBerconGradientDesc();
	default: return GetBerconNoiseDesc();
	}
}

// This function returns a string that describes the DLL and where the user
// could purchase the DLL if they don't have it.
__declspec(dllexport) const TCHAR* LibDescription()
{
	return GetString(IDS_LIBDESCRIPTION);
}

__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

__declspec( dllexport ) int LibInitialize(void) {
	return TRUE;
}

__declspec( dllexport ) int LibShutdown(void) {
	return TRUE;	
}

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadStringW(hInstance, id, buf, _countof(buf)) ? buf : NULL;
	return NULL;
}