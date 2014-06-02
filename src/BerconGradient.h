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
#include "windows.h"
#include "IGradient.h"
#include "chkmtlapi.h"

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

//#define GRADIENT3D

class BerconGradient : public Texmap, public GradientMap, public ResourceMakerCallback {
	public:
		// Parameter block
		IParamBlock2	*pblock;
		IParamBlock2	*pbCurve; //ref CURVEPB_REF
		IParamBlock2	*pbXYZ; //ref COORDS

		BerconXYZ berconXYZ;
		static ParamDlg* texoutDlg;
		TextureOutput   *texout;

		Interval		ivalid;	

		// Values		
		GradientRamp* gradient;
		IGradient* gradientUI;
		int dirty;
		
		int p_seed;
		int p_type;
		int p_randObj; int p_randMat; int p_randPar; int p_randTile;
		float p_rangeMin; float p_rangeMax;
		int p_rangeLoop;
		int p_uvwType;
		int p_reverse;		
		int p_normalType;
		int p_normalFunction;
		float p_ior;
		int p_disOn;
		float p_disStr;

		// Helpers
		void seedRandomGen(ShadeContext& sc);
		int limitRange(float& d);
		void EnableStuff();		

		// Shading
		float getGradientValueUVW(Point3 p);
		float getGradientValueDist(ShadeContext& sc);
		float getGradientValueNormal(ShadeContext& sc);
		float getGradientValue(ShadeContext& sc);		

		// UI				
		int previewMatIDMode;
		
		// Submaps		
		Texmap* p_maptex;
		Texmap* p_distex;
		INode* p_node;
		
		void setKeyTex(Texmap* m);
		Texmap* getKeyTex();

		// Editing keys
		void keyColorChanged(AColor col);
		void keyNumChanged(int num);
		void keyPosChanged(float pos);
		
		// GradientRampInterface
		int countKeys();
		void resetKeys();
		
		// From GradientMap
		void gradAddKey(float pos);
		void gradMoveKey(int n, float pos);
		void gradDelKey(int n);
		void gradSelKey();
		void gradReset() { resetKeys(); }

		// Curve
		ICurveCtl* curve;				
		BOOL p_curveOn;
		// From ResourceMakerCallback		
		BOOL SetCustomImageList(HIMAGELIST &hCTools,ICurveCtl *pCCtl) { return TRUE; };
		BOOL GetToolTip(int iButton, TSTR &ToolTip,ICurveCtl *pCCtl) { return TRUE; };
		void ResetCallback(int curvenum, ICurveCtl *pCCtl) { ICurve *pCurve = NULL; pCurve = pCCtl->GetControlCurve(curvenum); if(pCurve) { pCurve->SetNumPts(2); NewCurveCreatedCallback(curvenum, pCCtl); }}
		void NewCurveCreatedCallback(int curvenum, ICurveCtl *pCCtl) {
			ICurve *pCurve = NULL; pCurve = pCCtl->GetControlCurve(curvenum); TimeValue t = GetCOREInterface()->GetTime();
			CurvePoint pt = pCurve->GetPoint(t,0); pt.p.y = 0.f; pCurve->SetPoint(t,0,&pt);
			pCurve->SetPenProperty( RGB(0,0,0)); pCurve->SetDisabledPenProperty( RGB(128,128,128));		
			pt = pCurve->GetPoint(t,1); pt.p.y = 1.f; pCurve->SetPoint(t,1,&pt);
		}

		//From MtlBase
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		BOOL SetDlgThing(ParamDlg* dlg);
		void Update(TimeValue t, Interval& valid);
		void Reset();
		Interval Validity(TimeValue t);
		
		ULONG LocalRequirements(int subMtlNum) { return berconXYZ.req(); }
		void MappingsRequired(int subMtlNum, BitArray& mapreq, BitArray& bumpreq) { berconXYZ.map(subMtlNum, mapreq, bumpreq); }

		int NumSubTexmaps() { return 3 + gradient->numKeys(); } // !! Update submap count !!
		Texmap* GetSubTexmap(int i);
		void SetSubTexmap(int i, Texmap *m);
		TSTR GetSubTexmapSlotName(int i);
		
		//From Texmap
		RGBA EvalColor(ShadeContext& sc);
		float EvalMono(ShadeContext& sc);
		Point3 EvalNormalPerturb(ShadeContext& sc);						
		int SubNumToRefNum(int subNum) { return subNum; }		

		void ReadSXPData(TCHAR *name, void *sxpdata) {}

		// Interactive Display
		TexHandle *texHandle;
		Interval texHandleValid;
		void DiscardTexHandle() { if (texHandle) { texHandle->DeleteThis(); texHandle = NULL; } }
		BOOL SupportTexDisplay() { return TRUE; }
		void ActivateTexDisplay(BOOL onoff) { if (!onoff) DiscardTexHandle(); }
		DWORD_PTR GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker);

		//From Animatable
		Class_ID ClassID() {return BerconGradient_CLASS_ID;}		
		SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_BERCON_COLOR);}

		RefTargetHandle Clone( RemapDir &remap );
		RefResult NotifyRefChanged(NOTIFY_REF_CHANGED_ARGS);

		int NumSubs() { return 8+gradient->numKeys(); } // !! Update submap count !!
		Animatable* SubAnim(int i); 
		TSTR SubAnimName(int i);
		
		int NumRefs() { return 8+gradient->numKeys(); } // !! Update submap count !!
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		int	NumParamBlocks() { return 3; }
		IParamBlock2* GetParamBlock(int i) { switch (i) { case 0: return pblock; case 1: return pbCurve; case 2: return pbXYZ; } return NULL; }
		IParamBlock2* GetParamBlockByID(BlockID id) { 
			if (pblock->ID() == id) return pblock;
			if (pbCurve->ID() == id) return pbCurve;
			if (pbXYZ->ID() == id) return pbXYZ;
			return NULL;
		}
		
		int RenderBegin(TimeValue t, ULONG flags = 0) { if (!flags) previewMatIDMode = FALSE; return 1; }
		int RenderEnd(TimeValue t) { previewMatIDMode = TRUE; return 1; }				

		void DeleteThis() { delete this; }		
				
		BerconGradient();
		~BerconGradient();		

		void* GetInterface(ULONG id) {
			if(id == I_RESMAKER_INTERFACE)
				return (void *) (ResourceMakerCallback*) this;
			else
				return Texmap::GetInterface(id);
		}
};

class BerconGradientClassDesc : public ClassDesc2 {
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 	{ return new BerconGradient(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_BERCON_COLOR); }
	virtual SClass_ID SuperClassID() 				{ return TEXMAP_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return BerconGradient_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return TEXMAP_CAT_3D; }
	virtual const TCHAR* InternalName() 			{ return _T("BerconGradient"); } // scripter-visible name
	virtual HINSTANCE HInstance() 					{ return hInstance; }
};
