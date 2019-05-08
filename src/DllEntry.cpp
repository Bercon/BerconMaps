//
// Copyright [2015] Autodesk, Inc.  All rights reserved. 
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law.  They may
// not be disclosed to, copied or used by any third party without the prior
// written consent of Autodesk, Inc.
//

//#define COMPILE_MULTIMAP 1


#include <3dsmaxsdk_preinclude.h>
#include "BerconNoise.h"
#include "BerconCommon.h"

#ifndef COMPILE_MULTIMAP

// #include "BerconWood.h"	// why???

// Define Bercon Noise

extern ClassDesc2* GetBerconNoiseDesc();
/*extern ClassDesc2* GetBerconWoodDesc();
extern ClassDesc2* GetBerconTileDesc();
extern ClassDesc2* GetBerconDistortionDesc();
extern ClassDesc2* GetBerconGradientDesc();
*/
extern void InitGradientControls();

HINSTANCE hInstance;

// This function is called by Windows when the DLL is loaded.  This 
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.

int controlsInit = FALSE;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		MaxSDK::Util::UseLanguagePackLocale();
		hInstance = hinstDLL;            // Hang on to this DLL's instance handle.
		DisableThreadLibraryCalls(hInstance);
	}

	return (TRUE);
}
// This function returns the number of plug-in classes this DLL

__declspec( dllexport ) int LibNumberClasses() {
	return 1;
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) ClassDesc* LibClassDesc(int i) {
	switch(i) {
		case 0: return GetBerconNoiseDesc();
		/*case 1: return GetBerconWoodDesc();
		case 2: return GetBerconTileDesc();
		case 3: return GetBerconDistortionDesc();
		case 4: return GetBerconGradientDesc();
		*/
		default: return 0;
	}
}

#else

#include "MultiMap.h"

extern ClassDesc2* GetMultiMapDesc();
HINSTANCE hInstance;
int controlsInit = FALSE;
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID /*lpvReserved*/) {
	if( fdwReason == DLL_PROCESS_ATTACH ) {
		hInstance = hinstDLL;
		DisableThreadLibraryCalls(hInstance);
	}      
	return(TRUE);
}

__declspec( dllexport ) int LibNumberClasses() {
	return 1;
}

__declspec( dllexport ) ClassDesc* LibClassDesc(int i) {
	switch(i) {
		case 0: return GetMultiMapDesc();
		default: return 0;
	}
}

#endif

// This function returns a string that describes the DLL and where the user
// could purchase the DLL if they don't have it.

__declspec( dllexport ) const TCHAR* LibDescription()
{
	return GetString(IDS_LIBDESCRIPTION);
}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec(dllexport) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}


__declspec( dllexport ) int LibInitialize(void) {
	return TRUE;
}

__declspec( dllexport ) int LibShutdown(void) {
	return TRUE;	
}

extern TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, _countof(buf)) ? buf : NULL;
	return NULL;
}
