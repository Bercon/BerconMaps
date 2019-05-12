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

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

#define DIST_NSUBTEX		3  // Number of subtextures

class BerconDistortion;

class BerconDistortion : public Texmap {
	public:		
		// Distortion		
		BOOL useDistortion;
		float distortionStr;

		Point3 getDistVector(ShadeContext& sc);

		// Parameter block
		IParamBlock2	*pblock;	//ref 0
		Texmap			*subtex[DIST_NSUBTEX]; //array of sub-materials		
		Interval		ivalid;

		IParamBlock2	*pbXYZ;
		BerconXYZ berconXYZ;

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

		//TODO: Return the number of sub-textures
		int NumSubTexmaps() { return DIST_NSUBTEX; }
		//TODO: Return the pointer to the 'i-th' sub-texmap
		Texmap* GetSubTexmap(int i) { return subtex[i]; }
		void SetSubTexmap(int i, Texmap *m);
		TSTR GetSubTexmapSlotName(int i);
		
		//From Texmap
		RGBA EvalColor(ShadeContext& sc);
		float EvalMono(ShadeContext& sc);
		Point3 EvalNormalPerturb(ShadeContext& sc);

		//XYZGen *GetTheXYZGen() { return NULL; } 
		
		//TODO: Return anim index to reference index
		int SubNumToRefNum(int subNum) { return subNum; }
		
		void ReadSXPData(TCHAR *name, void *sxpdata) { }		

		//From Animatable
		Class_ID ClassID() {return BerconDistortion_CLASS_ID;}		
		SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_BERCON_DIST);}

		RefTargetHandle Clone( RemapDir &remap );
		RefResult NotifyRefChanged(NOTIFY_REF_CHANGED_ARGS);

		int NumSubs() { return 2+DIST_NSUBTEX; }
		Animatable* SubAnim(int i); 
		TSTR SubAnimName(int i);

		// TODO: Maintain the number or references here 
		int NumRefs() { return 5; }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		int	NumParamBlocks() { return 2; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return i ? pbXYZ : pblock; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : pbXYZ; } // return id'd ParamBlock

		void DeleteThis() { delete this; }		
		
		//Constructor/Destructor
		BerconDistortion();
		~BerconDistortion();		
};

class BerconDistortionClassDesc : public ClassDesc2 {
public:
	BerconDistortionClassDesc() {}
	virtual ~BerconDistortionClassDesc() {}
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 	{ return new BerconDistortion(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_BERCON_DIST); }
	virtual SClass_ID SuperClassID() 				{ return TEXMAP_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return BerconDistortion_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return TEXMAP_CAT_3D; }

	virtual const TCHAR* InternalName() 			{ return _T("BerconMapping"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle	
};
