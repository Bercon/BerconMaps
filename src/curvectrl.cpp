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

#include "curvectrl.h"	

void CurveCtrl::update(ICurveCtl *curve, HWND hParent, ReferenceMaker *resMaker) {
	curve->SetCustomParentWnd(hParent);
#if MAX_RELEASE >= 18900
	curve->RegisterResourceMaker(static_cast<ReferenceTarget*>(resMaker));
#else
	curve->RegisterResourceMaker(static_cast<ReferenceMaker*>(resMaker));
#endif
	BitArray ba = BitArray(1);			
	ba.SetAll();
	curve->SetDisplayMode(ba);
	curve->EnableDraw(TRUE);
	curve->SetActive(TRUE);
	curve->SetZoomValues(256.f, 128.f);
	curve->SetScrollValues(-16, -45);
	curve->Redraw();
}
void CurveCtrl::disable(ICurveCtl *curve) {
	curve->SetActive(FALSE);
	curve->EnableDraw(FALSE);	
}

void CurveCtrl::init(ICurveCtl *curve) {	
	curve->SetTitle(L"Function value curve");

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
	
	curve->SetCCFlags(flags);

	// Range	
	curve->SetXRange(0.0f,1.0f);
	curve->SetYRange(0.0f,1.0f);

	// Setup curve			
	curve->SetNumCurves(1, TRUE);
	ICurve *pCurve = curve->GetControlCurve(0);
	pCurve->SetNumPts(2);
	pCurve->SetPenProperty( RGB(0,0,0));
	pCurve->SetDisabledPenProperty( RGB(128,128,128));
	//pCurve->SetLookupTableSize(1000);
	TimeValue t = 0;
	
	// Point 0
	CurvePoint pt = pCurve->GetPoint(t,0);
	pt.p.y = 0.f;			
	pCurve->SetPoint(t,0,&pt);
	
	// Point 1
	pt = pCurve->GetPoint(t,1);
	pt.p.y = 1.f;			
	pCurve->SetPoint(t,1,&pt);	
}