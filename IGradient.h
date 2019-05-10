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

#include <max.h>
#include <3dsmaxport.h>
#include "resource.h"
#include "plugapi.h"


#include "GradientRamp.h"

class IGradient : public ICustomControl {
private:
	HWND hWndMain;
public:
	GradientRamp *gradient;		
	
	IGradient(HWND hwRamp, GradientRamp *r) {
		hWndMain = hwRamp;
		gradient = r;
		DLSetWindowLongPtr(hWndMain, this);
		Execute(I_EXEC_CB_NO_BORDER);
	}

	~IGradient() {
		DLSetWindowLongPtr(hWndMain, NULL);
		DestroyWindow(hWndMain);
		hWndMain = NULL;
		gradient = NULL;
	}

	void setGradient(GradientRamp *r) { gradient = r; } 

	static LRESULT CALLBACK GradientProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // control window callback,

	//AColor getRampColor(float x) { return gradient->GetColor(x); }

	void Disable() {}

	void DeleteMe() {delete this;}

	int IsEnabled(){ return TRUE; }

	void Enable(int cmd) {}

	void Enable2(int cmd) {}

	HWND GetHwnd() { return hWndMain; }

	//void SetTooltip(bool bEnable, MCHAR* text) {}
	void SetTooltip(bool bEnable, const MCHAR* text) {}

	INT_PTR Execute(int cmd, ULONG_PTR arg1=0, ULONG_PTR arg2=0, ULONG_PTR arg3=0) { return 0; }
};
