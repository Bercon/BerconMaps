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

#include "BerconDistortion.h"

#define PBLOCK_REF	0
#define MAP1_REF	1
#define MAP2_REF	2
#define MAP3_REF	3
#define COORD_REF	4

#define NAMELENGTH 64
typedef TCHAR TChBuffer[NAMELENGTH];

static BerconDistortionClassDesc BerconDistortionDesc;
ClassDesc2* GetBerconDistortionDesc() { return &BerconDistortionDesc; }

enum { BerconDistortion_params, xyz_params };

static XYZ_Desc xyz_blk(&BerconDistortionDesc, COORD_REF, xyz_params, 0, 1, 1, 1, 0);

enum { map1, distortion_map, distortion_map2, use_distortion, distortion_str, };

static ParamBlockDesc2 BerconDistortion_param_blk ( BerconDistortion_params, _T("params"),  0, &BerconDistortionDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 	
	IDD_PANEL_DISTORTION, IDS_PARAMS, 0, 0, NULL,
	// params
	// Distortion controls
	map1,		_T("map3"),		TYPE_TEXMAP,			P_OWNERS_REF,	IDS_MAP1,
		p_refno,		MAP3_REF,
		p_subtexno,		2,
		p_ui,			TYPE_TEXMAPBUTTON, IDC_MAP1,
		p_end,
	distortion_map,		_T("map1"),		TYPE_TEXMAP,			P_OWNERS_REF,	IDS_DISTORTION_MAP,
		p_refno,		MAP1_REF,
		p_subtexno,		0,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_DISTORTION_TEX,
		p_end,
	distortion_map2,	_T("map2"),		TYPE_TEXMAP,			P_OWNERS_REF,	IDS_DISTORTION_MAP2,
		p_refno,		MAP2_REF,
		p_subtexno,		1,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_DISTORTION_TEX2,
		p_end,
	distortion_str,	_T("distortionStr"),		 TYPE_FLOAT,	P_ANIMATABLE,	IDS_DISTORTION_STRENGTH,
		p_default,		0.1f,
		p_range,		0.0f, 1000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_DISTORTION_EDIT, IDC_DISTORTION_SPIN, 0.001f, 
		p_end,
	use_distortion,	_T("useDistortion"), TYPE_BOOL,			0,				IDS_USE_DISTORTION,
		p_default,		FALSE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_USE_DISTORTION,
		p_end,
	p_end
);

//--- BerconDistortion -------------------------------------------------------
BerconDistortion::BerconDistortion() {	
	for (int i=0; i<DIST_NSUBTEX; i++) subtex[i] = NULL;
	texHandle = NULL;
	pblock = NULL;
	pbXYZ = NULL;
	BerconDistortionDesc.MakeAutoParamBlocks(this);
	Reset();
}

BerconDistortion::~BerconDistortion() { DiscardTexHandle(); }
//From MtlBase
void BerconDistortion::Reset() {			   
	for (int i=0; i<DIST_NSUBTEX; i++) 
		DeleteReference(i+1);	
	ivalid.SetEmpty();
	berconXYZ.reset(pbXYZ, ivalid, 0, 1, 1, 1);
}

DWORD_PTR BerconDistortion::GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker) {
	if (texHandle) {
		if (texHandleValid.InInterval(t))
			return texHandle->GetHandle();
		else DiscardTexHandle();
		}
	texHandle = thmaker.MakeHandle(GetVPDisplayDIB(t,thmaker,texHandleValid));
	return texHandle->GetHandle();
}

void BerconDistortion::Update(TimeValue t, Interval& valid) {	
	if (pblock == NULL) return;

	if (!ivalid.InInterval(t)) {
		ivalid.SetInfinite();
		for (int i=0; i<DIST_NSUBTEX; i++)
			if (subtex[i]) 
				subtex[i]->Update(t,ivalid);
		pblock->GetValue( distortion_str, t, distortionStr, ivalid);
		pblock->GetValue( use_distortion, t, useDistortion, ivalid);		

		berconXYZ.update(pbXYZ, t, ivalid);
	}
	valid &= ivalid;
}

Interval BerconDistortion::Validity(TimeValue t) {
	Interval v;
	Update(t,v);
	return ivalid;
}

ParamDlg* BerconDistortion::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) {	
	IAutoMParamDlg* masterDlg = BerconDistortionDesc.CreateParamDlgs(hwMtlEdit, imp, this);	
	xyz_blk.SetUserDlgProc(new BerconXYZDlgProc(this));
	return masterDlg;	
}

BOOL BerconDistortion::SetDlgThing(ParamDlg* dlg) {	
	return FALSE;
}

void BerconDistortion::SetSubTexmap(int i, Texmap *m)  {
	ReplaceReference(i+1,m);
	if (i==0) {
		BerconDistortion_param_blk.InvalidateUI(distortion_map);
		ivalid.SetEmpty();	
	} else if (i==1) {
		BerconDistortion_param_blk.InvalidateUI(distortion_map2);
		ivalid.SetEmpty();
	}  else if (i==2) {
		BerconDistortion_param_blk.InvalidateUI(map1);
		ivalid.SetEmpty();
	}
}

TSTR BerconDistortion::GetSubTexmapSlotName(int i) {	
	switch(i) {
		case 0:  return TSTR(GetString(IDS_DISTORTION_MAP)); 
		case 1:  return TSTR(GetString(IDS_DISTORTION_MAP2)); 
		case 2:  return TSTR(GetString(IDS_MAP1)); 
		default: return TSTR(_T(""));
	}
}

RefTargetHandle BerconDistortion::GetReference(int i)  {
	switch (i) {
		case PBLOCK_REF: return pblock;
		case COORD_REF: return pbXYZ;
		default: return subtex[i-1];
	}
}

void BerconDistortion::SetReference(int i, RefTargetHandle rtarg) {	
	switch(i) {
		case PBLOCK_REF: pblock = (IParamBlock2 *)rtarg; break;
		case COORD_REF: pbXYZ = (IParamBlock2 *)rtarg; break;
		default: subtex[i-1] = (Texmap *)rtarg; break;
	}
}

RefTargetHandle BerconDistortion::Clone(RemapDir &remap) {
	BerconDistortion *mnew = new BerconDistortion();
	*((MtlBase*)mnew) = *((MtlBase*)this); // copy superclass stuff
	mnew->ReplaceReference(PBLOCK_REF,remap.CloneRef(pblock));
	mnew->ReplaceReference(COORD_REF,remap.CloneRef(pbXYZ));
	mnew->ivalid.SetEmpty();		
	for (int i = 0; i<DIST_NSUBTEX; i++) {
		mnew->subtex[i] = NULL;
		if (subtex[i])
			mnew->ReplaceReference(i+1,remap.CloneRef(subtex[i]));		
	}
	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
}

Animatable* BerconDistortion::SubAnim(int i) {
	switch (i) {
		case PBLOCK_REF: return pblock;
		case COORD_REF: return pbXYZ;
		default: return subtex[i-1];
	}
}

TSTR BerconDistortion::SubAnimName(int i) {	
	switch (i) {
		case PBLOCK_REF: return GetString(IDS_PARAMS);
		case COORD_REF: return GetString(IDS_XYZ);
		default: return GetSubTexmapTVName(i-1);
	}
}

RefResult BerconDistortion::NotifyRefChanged(NOTIFY_REF_CHANGED_ARGS) {
	switch (message) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();			
			if (hTarget == pblock) {
				ParamID changing_param = pblock->LastNotifyParamID();
				BerconDistortion_param_blk.InvalidateUI(changing_param);								
				if (changing_param != -1) DiscardTexHandle();				
			}
			break;
		}
	return(REF_SUCCEED);
}

// Define some basic values
static AColor black(0.0f,0.0f,0.0f,1.0f);

Point3 BerconDistortion::getDistVector(ShadeContext& sc) {	
	if (subtex[1])
		return subtex[0]->EvalNormalPerturb(sc)*distortionStr*subtex[1]->EvalMono(sc);
	else					
		return subtex[0]->EvalNormalPerturb(sc)*distortionStr;		  
	return Point3(0.f, 0.f, 0.f);
}

AColor BerconDistortion::EvalColor(ShadeContext& sc) {
	if (!subtex[2]) return black;	
	Point3 p;
	berconXYZ.get(sc, p);
	BerconSC& bsc = BerconSC(&sc);
	if (useDistortion && subtex[0]) p += getDistVector(sc);
	bsc.setParams(p, berconXYZ.type(), berconXYZ.chan());
	return subtex[2]->EvalColor(bsc);
}

float BerconDistortion::EvalMono(ShadeContext& sc) {
	if (!subtex[2]) return 0.f;
	Point3 p;
	berconXYZ.get(sc, p);
	BerconSC& bsc = BerconSC(&sc);
	if (useDistortion && subtex[0]) p += getDistVector(sc);
	bsc.setParams(p, berconXYZ.type(), berconXYZ.chan());
	return subtex[2]->EvalMono(bsc);
}

Point3 BerconDistortion::EvalNormalPerturb(ShadeContext& sc) {	
	if (!subtex[2]) return Point3(0.f, 0.f, 0.f);
	Point3 p;
	berconXYZ.get(sc, p);
	BerconSC& bsc = BerconSC(&sc);
	if (useDistortion && subtex[0]) p += getDistVector(sc);
	bsc.setParams(p, berconXYZ.type(), berconXYZ.chan());
	return subtex[2]->EvalNormalPerturb(bsc);
}