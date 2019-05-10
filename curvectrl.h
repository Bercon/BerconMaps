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

#include "icurvctl.h"
#include <iparamb2.h>

// Handles basic operations on cc_curve control

#define CURVECTRL_CLASS_ID Class_ID(0x495131c4, 0x1b7973db)

class CurveCtrl : public ReferenceTarget, public ResourceMakerCallback, public ClassDesc2
{
public:


	static void init(ICurveCtl* cc_curve)
	{
		// UI
		DWORD flags = CC_NONE;
		flags |= CC_DRAWBG;
		flags |= CC_DRAWGRID;
		flags |= CC_DRAWUTOOLBAR;
		flags |= 0; //CC_DRAWLTOOLBAR;
		flags |= CC_SHOWRESET;
		flags |= 0; //CC_DRAWSCROLLBARS;
		flags |= 0; //CC_AUTOSCROLL;
		flags |= CC_DRAWRULER;
		flags |= 0; //CC_ASPOPUP;
		flags |= CC_CONSTRAIN_Y;
		flags |= CC_HIDE_DISABLED_CURVES;

		// RC Menu
		flags |= CC_RCMENU_MOVE_XY;
		flags |= CC_RCMENU_MOVE_X;
		flags |= CC_RCMENU_MOVE_Y;
		flags |= CC_RCMENU_SCALE;
		flags |= CC_RCMENU_INSERT_CORNER;
		flags |= CC_RCMENU_INSERT_BEZIER;
		flags |= CC_RCMENU_DELETE;

		cc_curve->SetCCFlags(flags);

		// Range	
		cc_curve->SetXRange(0.0f, 1.0f);
		cc_curve->SetYRange(0.0f, 1.0f);

		// Setup cc_curve			
		cc_curve->SetNumCurves(1, TRUE);
		ICurve* pCurve = cc_curve->GetControlCurve(0);
		pCurve->SetNumPts(2);
		pCurve->SetPenProperty(RGB(0, 0, 0));
		pCurve->SetDisabledPenProperty(RGB(128, 128, 128));
		//pCurve->SetLookupTableSize(1000);
		TimeValue t = 0;

		// Point 0
		CurvePoint pt = pCurve->GetPoint(t, 0);
		pt.p.y = 0.f;
		pCurve->SetPoint(t, 0, &pt);

		// Point 1
		pt = pCurve->GetPoint(t, 1);
		pt.p.y = 1.f;
		pCurve->SetPoint(t, 1, &pt);
	}

	static void update(ICurveCtl* cc_curve, HWND hParent, ReferenceMaker* resMaker)
	{
		cc_curve->SetCustomParentWnd(hParent);
		cc_curve->RegisterResourceMaker(dynamic_cast<ReferenceTarget*>(resMaker));
		BitArray ba = BitArray(1);
		ba.SetAll();
		cc_curve->SetDisplayMode(ba);
		cc_curve->EnableDraw(TRUE);
		cc_curve->SetActive(TRUE);
		cc_curve->SetZoomValues(256.f, 128.f);
		cc_curve->SetScrollValues(-16, -45);
		cc_curve->Redraw();
	}

	static void disable(ICurveCtl* cc_curve)
	{
		cc_curve->SetActive(FALSE);
		cc_curve->EnableDraw(FALSE);
	}

	

	virtual Class_ID ClassID() { return CURVECTRL_CLASS_ID; }
};
