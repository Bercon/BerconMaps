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

/*
	This is the general include file, used by all maps.
*/

#pragma once

#include "BerconCommon.h"

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;


#define NOISE_NSUBTEX		18  // Number of subtextures

class BerconNoise;

class BerconNoise : public Texmap, public ResourceMakerCallback/*, public imrShaderTranslation*/ {
	public:		
		bool mappedParameters;
		NoiseParams EvalParameters(ShadeContext* sc);
		
		//void limitLevel(Point3 dp, NoiseParams &np, float nSize);

		//void calcAverage();
		//float average;

		// Noise variables	
		float size;
		float phase;
		float spread;
		float F1, F2, F3, F4;				

		int noiseFunction;
		int worleyFunction;
		int fractalFunction;

		// Fractal variables
		float levels;
		float low, high;
		float fractalH, fractalOffset, fractalGain, fractalLacunarity;		

		// Distortion		
		BOOL useDistortion;
		float distortionStr;
		void applyDistortion(ShadeContext& sc, Point3& p);
		int uvwDist;		

		// User Interface
		void EnableStuff();

		// Parameter block
		IParamBlock2	*pbXYZ;
		IParamBlock2	*pblock;	//ref 0
		IParamBlock2	*pbCurve;	//ref CURVEPB_REF
		IParamBlock2	*pbMap;		//ref PBMAP_REF

		BerconXYZ berconXYZ;

		Color			 col[2];
		Texmap			*subtex[NOISE_NSUBTEX]; //array of sub-materials
		BOOL			mapOn[NOISE_NSUBTEX];
		static ParamDlg* texoutDlg;
		TextureOutput   *texout;
		Interval		ivalid;
		
		// Curve
		ICurveCtl* curve;				
		BOOL useCurve;
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

		// Interactive Display
		TexHandle *texHandle;
		Interval texHandleValid;
		void DiscardTexHandle() { if (texHandle) { texHandle->DeleteThis(); texHandle = NULL; } }
		BOOL SupportTexDisplay() { return TRUE; }
		void ActivateTexDisplay(BOOL onoff) { if (!onoff) DiscardTexHandle(); }
		DWORD_PTR GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker);

		//From MtlBase
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		BOOL SetDlgThing(ParamDlg* dlg);
		void Update(TimeValue t, Interval& valid);
		void Reset();
		Interval Validity(TimeValue t);
		ULONG LocalRequirements(int subMtlNum) { return berconXYZ.req(); }
		void MappingsRequired(int subMtlNum, BitArray& mapreq, BitArray& bumpreq) { berconXYZ.map(subMtlNum, mapreq, bumpreq); }


		int NumSubTexmaps() { return NOISE_NSUBTEX; }
		Texmap* GetSubTexmap(int i) { return subtex[i]; }
		void SetSubTexmap(int i, Texmap *m);
		TSTR GetSubTexmapSlotName(int i);
		
		//From Texmap
		RGBA EvalColor(ShadeContext& sc);
		float EvalMono(ShadeContext& sc);
		Point3 EvalNormalPerturb(ShadeContext& sc);

		XYZGen *GetTheXYZGen() { return NULL; } 
		
		int SubNumToRefNum(int subNum) { return subNum; }
		
		void ReadSXPData(TCHAR *name, void *sxpdata) { }
		
		//From Animatable
		Class_ID ClassID() {return BerconNoise_CLASS_ID;}		
		SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

		RefTargetHandle Clone( RemapDir &remap );
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message);

		int NumSubs() { return 24; }
		Animatable* SubAnim(int i); 
		TSTR SubAnimName(int i);

		// TODO: Maintain the number or references here 
		int NumRefs() { return 24; }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		int	NumParamBlocks() { return 4; }
		IParamBlock2* GetParamBlock(int i) { switch (i) { case 0: return pblock; case 1: return pbCurve; case 2: return pbMap; case 3: return pbXYZ; } return NULL; }
		IParamBlock2* GetParamBlockByID(BlockID id) { 
			if (pblock->ID() == id) return pblock;
			if (pbCurve->ID() == id) return pbCurve;
			if (pbMap->ID() == id) return pbMap;
			if (pbXYZ->ID() == id) return pbXYZ;
			return NULL;			
		}

		void DeleteThis() { delete this; }		
		
		//Constructor/Destructor
		BerconNoise();
		~BerconNoise();		

};

class BerconNoiseClassDesc : public ClassDesc2 {
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL)				 		{ return new BerconNoise(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return TEXMAP_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return BerconNoise_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return TEXMAP_CAT_3D; }
	virtual const TCHAR* InternalName() 			{ return _T("BerconNoise"); } // returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }
};
