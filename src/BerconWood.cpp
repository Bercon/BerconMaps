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

#include "BerconWood.h"

#define COORD_REF	0
#define PBLOCK_REF	1

#define MAP1_REF	2
#define MAP2_REF	3
#define MAP3_REF	4
#define MAP4_REF	5
#define MAP5_REF	6

#define BMAP1_REF	7
#define BMAP2_REF	8
#define BMAP3_REF	9
#define BMAP4_REF	10
#define BMAP5_REF	11
#define BMAP6_REF	12
#define BMAP7_REF	13
#define BMAP8_REF	14
#define BMAP9_REF	15
#define BMAP10_REF	16
#define BMAP11_REF	17
#define BMAP12_REF	18
#define BMAP13_REF	19
#define BMAP14_REF	20
#define BMAP15_REF	21
#define BMAP16_REF	22

#define BMAP1_NUM	5
#define BMAP2_NUM	6
#define BMAP3_NUM	7
#define BMAP4_NUM	8
#define BMAP5_NUM	9
#define BMAP6_NUM	10
#define BMAP7_NUM	11
#define BMAP8_NUM	12
#define BMAP9_NUM	13
#define BMAP10_NUM	14
#define BMAP11_NUM	15
#define BMAP12_NUM	16
#define BMAP13_NUM	17
#define BMAP14_NUM	18
#define BMAP15_NUM	19
#define BMAP16_NUM	20

#define OUTPUT_REF	23
#define CURVEPB_REF	24
#define CURVE_REF	25
#define PBMAP_REF	26

#define NAMELENGTH 64
typedef TCHAR TChBuffer[NAMELENGTH];

static BerconWoodClassDesc BerconWoodDesc;
ClassDesc2* GetBerconWoodDesc() { return &BerconWoodDesc; }

enum { xyz_params, BerconWood_params, BerconCurve_params, BerconMap_params };

static XYZ_Desc xyz_blk(&BerconWoodDesc, COORD_REF, xyz_params, 2, 0, 0, 0);

enum { 	
	wood_type,
	wood_size,
	wood_skew,
	trunk_str, trunk_freq,
	radial_str, radial_freq, radial_z,
	angle_str, angle_freq, angle_rad,
	grain_amount, grain_freq, lock_grain,
	width_var, gain_var,
	low_tresh, high_tresh,
	rand_seed,
	// Distortion
	distortion_map, distortion_map2,
	use_distortion, distortion_str,
	// Standard noise controls
	noise_color1, noise_color2, noise_color3,
	noise_map1, noise_map2,	noise_map3,	
	noise_map1_on, noise_map2_on, noise_map3_on,
	// UVW, Output
	pb_output,
	pb_samples,
};

//ParamDlg* BerconWood::xyzGenDlg;
ParamDlg* BerconWood::texoutDlg;

static ParamBlockDesc2 BerconWood_param_blk ( BerconWood_params, _T("params"),  0, &BerconWoodDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 	
	IDD_PANEL_WOOD, IDS_PARAMS, 0, 0, NULL,
	// params

	// Standard noise controls
	noise_color1,	 _T("color1"),	TYPE_RGBA,				P_ANIMATABLE,	IDS_DS_COLOR1,	
		p_default,		Color(0.768f, 0.568f, 0.25f), 
		p_ui,			TYPE_COLORSWATCH, IDC_NOISE_COL1, 
		p_end,
	noise_color2,	 _T("color2"),	TYPE_RGBA,				P_ANIMATABLE,	IDS_DS_COLOR2,	
		p_default,		Color(0.392f , 0.243f, 0.0f), 
		p_ui,			TYPE_COLORSWATCH, IDC_NOISE_COL2, 
		p_end,			 
	noise_color3,	 _T("color3"),	TYPE_RGBA,				P_ANIMATABLE,	IDS_DS_COLOR3,	
		p_default,		Color(0.549f, 0.337f, 0.0f), 
		p_ui,			TYPE_COLORSWATCH, IDC_NOISE_COL3, 
		p_end,
	noise_map1,		_T("map1"),		TYPE_TEXMAP,			P_OWNERS_REF,	IDS_JW_MAP1,
		p_refno,		MAP1_REF,
		p_subtexno,		0,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_NOISE_TEX1,
		p_end,
	noise_map2,		_T("map2"),		TYPE_TEXMAP,			P_OWNERS_REF,	IDS_JW_MAP2,
		p_refno,		MAP2_REF,
		p_subtexno,		1,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_NOISE_TEX2,
		p_end,
	noise_map3,		_T("map3"),		TYPE_TEXMAP,			P_OWNERS_REF,	IDS_JW_MAP3,
		p_refno,		MAP3_REF,
		p_subtexno,		2,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_NOISE_TEX3,
		p_end,
	noise_map1_on,	_T("map1Enabled"), TYPE_BOOL,			0,				IDS_JW_MAP1ENABLE,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MAPON1,
		p_end,
	noise_map2_on,	_T("map2Enabled"), TYPE_BOOL,			0,				IDS_JW_MAP2ENABLE,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MAPON2,
		p_end,
	noise_map3_on,	_T("map3Enabled"), TYPE_BOOL,			0,				IDS_JW_MAP3ENABLE,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MAPON3,
		p_end,
	wood_size,	_T("size"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_WOODSIZE,
		p_default,		3.f,
		p_range,		0.001, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_WOODSIZE_EDIT, IDC_WOODSIZE_SPIN, SPIN_AUTOSCALE, 
		p_end,
	low_tresh, _T("treshLow"), TYPE_FLOAT,	P_ANIMATABLE,	IDS_RB_LOWTHRESH,
		p_default,		0.3f,
		p_range,		0.0f, 1.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_TRESHLOW_EDIT, IDC_TRESHLOW_SPIN, SPIN_AUTOSCALE, 
		p_end,
	high_tresh, _T("treshHigh"), TYPE_FLOAT,	P_ANIMATABLE,	IDS_RB_HIGHTHRESH,
		p_default,		1.0f,
		p_range,		0.0f, 1.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_TRESHHIGH_EDIT, IDC_TRESHHIGH_SPIN, SPIN_AUTOSCALE, 
		p_end,

	wood_type,	_T("woodType"),		TYPE_INT,		0,				IDS_WOOD_TYPE,		
		p_default,		0,		
		p_ui, 			TYPE_INTLISTBOX, IDC_WOOD_TYPE, 0, 
		p_end,	

	trunk_str,	_T("trunkStr"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TRUNKSTR,
		p_default,		1.f,
		p_range,		0.f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_TRUNKSTR_EDIT, IDC_TRUNKSTR_SPIN, SPIN_AUTOSCALE, 
		p_end,
	trunk_freq,	_T("trunkFreq"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TRUNKFREQ,
		p_default,		.04f,
		p_range,		0.001f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_TRUNKFREQ_EDIT, IDC_TRUNKFREQ_SPIN, SPIN_AUTOSCALE, 
		p_end,
	
		
	radial_str,	_T("radialStr"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_RADIALSTR,
		p_default,		.25f,
		p_range,		0.f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_RADIALSTR_EDIT, IDC_RADIALSTR_SPIN, SPIN_AUTOSCALE, 
		p_end,
	radial_freq,	_T("radialFreq"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_RADIALFREQ,
		p_default,		.1f,
		p_range,		0.001f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_RADIALFREQ_EDIT, IDC_RADIALFREQ_SPIN, SPIN_AUTOSCALE, 
		p_end,
	radial_z,	_T("radialZ"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_RADIALZ,
		p_default,		.01f,
		p_range,		0.0f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_RADIALZ_EDIT, IDC_RADIALZ_SPIN, SPIN_AUTOSCALE, 
		p_end,


	angle_str,	_T("angleStr"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_ANGLESTR,
		p_default,		.1f,
		p_range,		0.f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_ANGLESTR_EDIT, IDC_ANGLESTR_SPIN, SPIN_AUTOSCALE, 
		p_end,
	angle_freq,	_T("angleFreq"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_ANGLEFREQ,
		p_default,		1.f,
		p_range,		0.001f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_ANGLEFREQ_EDIT, IDC_ANGLEFREQ_SPIN, SPIN_AUTOSCALE, 
		p_end,
	angle_rad,	_T("angleRad"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_ANGLERAD,
		p_default,		15.f,
		p_range,		0.001f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_ANGLERAD_EDIT, IDC_ANGLERAD_SPIN, SPIN_AUTOSCALE, 
		p_end,


	grain_amount,	_T("grainStr"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_GRAINSTR,
		p_default,		.2f,
		p_range,		0.0f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_GRAINSTR_EDIT, IDC_GRAINSTR_SPIN, SPIN_AUTOSCALE, 
		p_end,
	grain_freq,	_T("grainFreq"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_GRAINFREQ,
		p_default,		5.f,
		p_range,		0.001f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_GRAINFREQ_EDIT, IDC_GRAINFREQ_SPIN, SPIN_AUTOSCALE, 
		p_end,
	lock_grain,	_T("grainLock"), TYPE_BOOL,			0,				IDS_LOCK_GRAIN,
		p_default,		FALSE,
		p_ui,			TYPE_CHECKBUTTON, IDC_LOCK_GRAIN,
		p_end,


	width_var,	_T("widthVariation"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_WIDTHVAR,
		p_default,		.5f,
		p_range,		0.0f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_WIDTHVAR_EDIT, IDC_WIDTHVAR_SPIN, SPIN_AUTOSCALE, 
		p_end,
	gain_var,	_T("gainVariation"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_GAINVAR,
		p_default,		.75f,
		p_range,		0.0f, 1.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_GAINVAR_EDIT, IDC_GAINVAR_SPIN, SPIN_AUTOSCALE, 
		p_end,
	rand_seed,	_T("randomSeed"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_SEED,
		p_default,		12.345f,
		p_range,		0.0f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SEED_EDIT, IDC_SEED_SPIN, SPIN_AUTOSCALE, 
		p_end,

	wood_skew,	_T("skew"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_SKEW,
		p_default,		0.75f,
		p_range,		0.0f, 1.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SKEW_EDIT, IDC_SKEW_SPIN, SPIN_AUTOSCALE, 
		p_end,

	// Sampling
	pb_samples,	_T("samples"),   TYPE_INT,			P_ANIMATABLE,	IDS_SAMPLES,
		p_default,		4,
		p_range,		1, 1000000,
		p_ui, 			TYPE_SPINNER, EDITTYPE_INT, IDC_SAMPLES_EDIT, IDC_SAMPLES_SPIN, SPIN_AUTOSCALE, 
		p_end,

	// Distortion controls
	distortion_map,		_T("map4"),		TYPE_TEXMAP,			P_OWNERS_REF,	IDS_DISTORTION_MAP,
		p_refno,		MAP4_REF,
		p_subtexno,		3,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_DISTORTION_TEX,
		p_end,
	distortion_map2,	_T("map5"),		TYPE_TEXMAP,			P_OWNERS_REF,	IDS_DISTORTION_MAP2,
		p_refno,		MAP5_REF,
		p_subtexno,		4,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_DISTORTION_TEX2,
		p_end,
	distortion_str,	_T("distortionStr"),		 TYPE_FLOAT,	P_ANIMATABLE,	IDS_DISTORTION_STRENGTH,
		p_default,		0.1f,
		p_range,		0.0f, 1000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_DISTORTION_EDIT, IDC_DISTORTION_SPIN, SPIN_AUTOSCALE, 
		p_end,
	use_distortion,	_T("useDistortion"), TYPE_BOOL,			0,				IDS_USE_DISTORTION,
		p_default,		FALSE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_USE_DISTORTION,
		p_end,

	// UVW, Output controls
	pb_output,			_T("output"),		TYPE_REFTARG,	P_OWNERS_REF,	IDS_OUTPUT,
		p_refno,		OUTPUT_REF, 
		p_end,
	p_end
);

enum { enable_curve, curve_cont };

static ParamBlockDesc2 BerconCurve_param_blk ( BerconCurve_params, _T("params"),  0, &BerconWoodDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, CURVEPB_REF, 	
	IDD_PANEL_CURVE, IDS_CURVEPARM, 0, 1, NULL,
	enable_curve,	_T("enableCurve"), TYPE_BOOL,			0,				IDS_OUTPUT_NOISE,
		p_default,		FALSE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_ENABLE,
		p_end,	
	curve_cont,			_T("curveControl"),	TYPE_REFTARG,	P_OWNERS_REF,	IDS_CURVE,
		p_refno,		CURVE_REF, 
		p_end,
	p_end
);

enum {
	map1, map2, map3, map4, map5, map6, map7, map8, map9, map10, map11, map12, map13, map14, map15, map16,
	mapOn1, mapOn2, mapOn3, mapOn4, mapOn5, mapOn6, mapOn7, mapOn8, mapOn9, mapOn10, mapOn11, mapOn12, mapOn13, mapOn14, mapOn15, mapOn16,
};

static ParamBlockDesc2 BerconMap_param_blk ( BerconMap_params, _T("params"),  0, &BerconWoodDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBMAP_REF, 	
	IDD_PANEL_WOOD_MAP, IDS_PBMAP, 0, 1, NULL,
	
	map1, _T("bmap1"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP1, p_refno, BMAP1_REF, p_subtexno, BMAP1_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP1, p_end, mapOn1, _T("bmap1Enabled"), TYPE_BOOL, 0, IDS_MAPON1, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON1, p_end,
	map2, _T("bmap2"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP2, p_refno, BMAP2_REF, p_subtexno, BMAP2_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP2, p_end, mapOn2, _T("bmap2Enabled"), TYPE_BOOL, 0, IDS_MAPON2, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON2, p_end,
	map3, _T("bmap3"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP3, p_refno, BMAP3_REF, p_subtexno, BMAP3_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP3, p_end, mapOn3, _T("bmap3Enabled"), TYPE_BOOL, 0, IDS_MAPON3, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON3, p_end,
	map4, _T("bmap4"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP4, p_refno, BMAP4_REF, p_subtexno, BMAP4_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP4, p_end, mapOn4, _T("bmap4Enabled"), TYPE_BOOL, 0, IDS_MAPON4, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON4, p_end,
	map5, _T("bmap5"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP5, p_refno, BMAP5_REF, p_subtexno, BMAP5_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP5, p_end, mapOn5, _T("bmap5Enabled"), TYPE_BOOL, 0, IDS_MAPON5, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON5, p_end,
	map6, _T("bmap6"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP6, p_refno, BMAP6_REF, p_subtexno, BMAP6_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP6, p_end, mapOn6, _T("bmap6Enabled"), TYPE_BOOL, 0, IDS_MAPON6, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON6, p_end,
	map7, _T("bmap7"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP7, p_refno, BMAP7_REF, p_subtexno, BMAP7_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP7, p_end, mapOn7, _T("bmap7Enabled"), TYPE_BOOL, 0, IDS_MAPON7, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON7, p_end,
	map8, _T("bmap8"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP8, p_refno, BMAP8_REF, p_subtexno, BMAP8_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP8, p_end, mapOn8, _T("bmap8Enabled"), TYPE_BOOL, 0, IDS_MAPON8, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON8, p_end,
	map9, _T("bmap9"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP9, p_refno, BMAP9_REF, p_subtexno, BMAP9_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP9, p_end, mapOn9, _T("bmap9Enabled"), TYPE_BOOL, 0, IDS_MAPON9, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON9, p_end,
	map10, _T("bmap10"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP10, p_refno, BMAP10_REF, p_subtexno, BMAP10_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP10, p_end, mapOn10, _T("bmap10Enabled"), TYPE_BOOL, 0, IDS_MAPON10, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON10, p_end,
	map11, _T("bmap11"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP11, p_refno, BMAP11_REF, p_subtexno, BMAP11_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP11, p_end, mapOn11, _T("bmap11Enabled"), TYPE_BOOL, 0, IDS_MAPON11, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON11, p_end,
	map12, _T("bmap12"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP12, p_refno, BMAP12_REF, p_subtexno, BMAP12_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP12, p_end, mapOn12, _T("bmap12Enabled"), TYPE_BOOL, 0, IDS_MAPON12, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON12, p_end,
	map13, _T("bmap13"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP13, p_refno, BMAP13_REF, p_subtexno, BMAP13_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP13, p_end, mapOn13, _T("bmap13Enabled"), TYPE_BOOL, 0, IDS_MAPON13, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON13, p_end,
	map14, _T("bmap14"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP14, p_refno, BMAP14_REF, p_subtexno, BMAP14_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP14, p_end, mapOn14, _T("bmap14Enabled"), TYPE_BOOL, 0, IDS_MAPON14, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON14, p_end,
	map15, _T("bmap15"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP15, p_refno, BMAP15_REF, p_subtexno, BMAP15_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP15, p_end, mapOn15, _T("bmap15Enabled"), TYPE_BOOL, 0, IDS_MAPON15, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON15, p_end,
	map16, _T("bmap16"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP16, p_refno, BMAP16_REF, p_subtexno, BMAP16_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP16, p_end, mapOn16, _T("bmap16Enabled"), TYPE_BOOL, 0, IDS_MAPON16, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON16, p_end,

	p_end
);

class BerconCurveDlgProc : public ParamMap2UserDlgProc {
	public:
		BerconWood *berconWood;		
		BerconCurveDlgProc(BerconWood *m) {berconWood = m;}		
		INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam) {
			if (berconWood->curve->GetHWND() != GetDlgItem(hWnd, IDC_CURVE))
				CurveCtrl::update(berconWood->curve, GetDlgItem(hWnd, IDC_CURVE), static_cast<ReferenceMaker*>(berconWood)); // Force update curve
			switch (msg) {
				case WM_INITDIALOG:
				case WM_SHOWWINDOW:
					CurveCtrl::update(berconWood->curve, GetDlgItem(hWnd, IDC_CURVE), static_cast<ReferenceMaker*>(berconWood));					
					break;
				case WM_DESTROY:
					CurveCtrl::disable(berconWood->curve);
					break;
				default: return FALSE;
			}
			return TRUE;
		}
		void DeleteThis() {delete this;}
		void SetThing(ReferenceTarget *m) { 
			CurveCtrl::disable(berconWood->curve); // Disable previously used curve
			berconWood = (BerconWood*)m;
		}
};

//dialog stuff to get the Set Ref button
class BerconWoodDlgProc : public ParamMap2UserDlgProc {
	public:
		BerconWood *berconWood;		
		BerconWoodDlgProc(BerconWood *m) {berconWood = m;}		
		INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);		
		void DeleteThis() {delete this;}
		void SetThing(ReferenceTarget *m) {
			berconWood = (BerconWood*)m;
			berconWood->EnableStuff();
		}
};

INT_PTR BerconWoodDlgProc::DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)	{	
	switch (msg) {
		case WM_INITDIALOG:	{			
			// Lock buttons
			ICustButton* custButton = GetICustButton(GetDlgItem(hWnd, IDC_LOCK_GRAIN));
			custButton->SetText(_T("L"));
			ReleaseICustButton(custButton); 

			// Wood type
			HWND hwndMap = GetDlgItem(hWnd, IDC_WOOD_TYPE);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_RADIAL_WOOD));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PERLIN_WOOD));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_SIMPLEX_WOOD));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LINEAR_WOOD));

			// Set correct dropdown value
			int curIndex;
			map->GetParamBlock()->GetValue(wood_type, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_WOOD_TYPE), CB_SETCURSEL, (WPARAM)curIndex, 0);						
			break;
		}		
		case WM_SHOWWINDOW:	{		
			// Set correct dropdown value
			int curIndex;
			map->GetParamBlock()->GetValue(wood_type, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_WOOD_TYPE), CB_SETCURSEL, (WPARAM)curIndex, 0);
			break;
		}
		default: return FALSE;
	}
	return TRUE;
}

//--- BerconWood -------------------------------------------------------
BerconWood::BerconWood() {	
	for (int i=0; i<NSUBTEX; i++) subtex[i] = NULL;	
	texHandle = NULL;
	pblock = NULL;
	pbCurve = NULL;
	pbMap = NULL;
	pbXYZ = NULL;
	BerconWoodDesc.MakeAutoParamBlocks(this);	
	texout = NULL;	
	curve = NULL;	
	Reset();
}

BerconWood::~BerconWood() { DiscardTexHandle(); }

//From MtlBase
void BerconWood::Reset() {
	TimeValue t = GetCOREInterface()->GetTime();

	if (texout) texout->Reset();
	else ReplaceReference( OUTPUT_REF, GetNewDefaultTextureOutput());
	
	if (curve) curve->DeleteMe();
	curve = (ICurveCtl *) CreateInstance(REF_MAKER_CLASS_ID,CURVE_CONTROL_CLASS_ID);
#if MAX_RELEASE >= 18900
	curve->RegisterResourceMaker(static_cast<ReferenceTarget*>(this));
#else
	curve->RegisterResourceMaker(static_cast<ReferenceMaker*>(this));
#endif
	CurveCtrl::init(curve);
	pbCurve->SetValue(enable_curve, t, FALSE);

	for (int i=0; i<NSUBTEX; i++) { 
		DeleteReference(i+2);			
	}

	pblock->SetValue( noise_color1, t,		Color(0.768f, 0.568f, 0.25f)	);
	pblock->SetValue( noise_color2, t,		Color(0.392f , 0.243f, 0.0f)	);
	pblock->SetValue( noise_color3, t,		Color(0.549f, 0.337f, 0.0f)		);
	pblock->SetValue( noise_map1_on, t,		TRUE	);
	pblock->SetValue( noise_map2_on, t,		TRUE	);		
	pblock->SetValue( noise_map3_on, t,		TRUE	);				
	pblock->SetValue( distortion_str, t,	.1f		);
	pblock->SetValue( use_distortion, t,	FALSE	);
	pblock->SetValue( grain_amount, t,		.2f		);
	pblock->SetValue( grain_freq, t,		5.f		);
	pblock->SetValue( lock_grain, t,		FALSE	);		
	pblock->SetValue( wood_size, t,			3.f		);			
	pblock->SetValue( wood_type, t,			0		);
	pblock->SetValue( trunk_str, t,			1.f		);
	pblock->SetValue( trunk_freq, t,		.04f	);
	pblock->SetValue( radial_str, t,		.25f	);
	pblock->SetValue( radial_freq, t,		.1f		);
	pblock->SetValue( radial_z, t,			.01f	);
	pblock->SetValue( angle_str, t,			.1f		);
	pblock->SetValue( angle_freq, t,		1.f		);
	pblock->SetValue( angle_rad, t,			15.f	);
	pblock->SetValue( width_var, t,			.5f		);
	pblock->SetValue( gain_var, t,			.75f	);
	pblock->SetValue( low_tresh, t,			.3f		);
	pblock->SetValue( high_tresh, t,		1.f		);
	pblock->SetValue( wood_skew, t,			.75f	);
	pblock->SetValue( rand_seed, t,			12.345f	);
	pblock->SetValue( pb_samples, t,			4	);

	// Maps
	for (int i=16; i<32; i++)
		pbMap->SetValue(i, t, TRUE);

	berconXYZ.reset(pbXYZ, ivalid, 2, 0, 0, 0);

	ivalid.SetEmpty();		
}

void BerconWood::Update(TimeValue t, Interval& valid) {	
	if (pblock == NULL || pbCurve == NULL) return;
	if (!ivalid.InInterval(t)) {
		ivalid.SetInfinite();

		if (texout != NULL)
			texout->Update(t,ivalid);
		pblock->GetValue( noise_color1, t, col[0], ivalid );
		col[0].ClampMinMax();
		pblock->GetValue( noise_color2, t, col[1], ivalid );
		col[1].ClampMinMax();
		pblock->GetValue( noise_color3, t, col[2], ivalid );
		col[2].ClampMinMax();

		for (int i=0; i<NSUBTEX; i++)
			if (subtex[i]) 
				subtex[i]->Update(t,ivalid);

		pblock->GetValue( noise_map1_on, t, mapOn[0], ivalid);
		pblock->GetValue( noise_map2_on, t, mapOn[1], ivalid);		
		pblock->GetValue( noise_map3_on, t, mapOn[2], ivalid);		
	
		pblock->GetValue( distortion_str, t, distortionStr, ivalid);
		pblock->GetValue( use_distortion, t, useDistortion, ivalid);

		pblock->GetValue( grain_amount, t, grainAmount, ivalid);
		pblock->GetValue( grain_freq, t, grainFreq, ivalid);
		pblock->GetValue( lock_grain, t, lockGrain, ivalid);		

		pblock->GetValue( wood_size, t, woodSize, ivalid);		

		// Update noise parameters
		pblock->GetValue( wood_type, t,			woodType, ivalid);
		pblock->GetValue( trunk_str, t,			trunkStr, ivalid);
		pblock->GetValue( trunk_freq, t,		trunkFreq, ivalid);
		pblock->GetValue( radial_str, t,		radialStr, ivalid);
		pblock->GetValue( radial_freq, t,		radialFreq, ivalid);
		pblock->GetValue( radial_z, t,			radialZ, ivalid);
		pblock->GetValue( angle_str, t,			angleStr, ivalid);
		pblock->GetValue( angle_freq, t,		angleFreq, ivalid);
		pblock->GetValue( angle_rad, t,			angleRad, ivalid);
		pblock->GetValue( width_var, t,			widthVar, ivalid);
		pblock->GetValue( gain_var, t,			gainVar, ivalid);
		pblock->GetValue( low_tresh, t,			lowTresh, ivalid);
		pblock->GetValue( high_tresh, t,		highTresh , ivalid);
		pblock->GetValue( wood_skew, t,			skew , ivalid);
		pblock->GetValue( rand_seed, t,			randSeed , ivalid);
		pblock->GetValue( pb_samples, t,		samples , ivalid);

		// Update maps
		for (int i = 0; i<16; i++)
			pbMap->GetValue((i+16), t, mapOn[i+5], ivalid);		

		if (highTresh<lowTresh) {
			float tmp = lowTresh;
			lowTresh = highTresh;
			highTresh = tmp;
		}

		// Slight optimization
		mappedParameters = false;
		for (int i=6;i<=18;i++)
			if (mapOn[i] && subtex[i])
				mappedParameters = true;	

		// Curve
		pbCurve->GetValue(enable_curve, t, useCurve, ivalid);

		EnableStuff();		
		berconXYZ.update(pbXYZ, t, ivalid);
	}
	valid &= ivalid;		
}

Interval BerconWood::Validity(TimeValue t) {
	Interval v;
	Update(t,v);
	return ivalid;
}

void BerconWood::EnableStuff() {
	if (pblock) {
		IParamMap2 *map = pblock->GetMap();
		if (map) {
			// Update values we need to know
			pblock->GetValue( use_distortion, 0, useDistortion, FOREVER);
			pblock->GetValue( lock_grain, 0, lockGrain, FOREVER);		

			// Update parameters			
			map->Enable(noise_color3, lockGrain?FALSE:TRUE);
			map->Enable(noise_map3, lockGrain?FALSE:TRUE);
			map->Enable(noise_map3_on, lockGrain?FALSE:TRUE);

			map->Enable(distortion_str, useDistortion);
			map->Enable(distortion_map, useDistortion);
			map->Enable(distortion_map2, useDistortion);
		}
	}
}

DWORD_PTR BerconWood::GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker) {
	if (texHandle) {
		if (texHandleValid.InInterval(t))
			return texHandle->GetHandle();
		else DiscardTexHandle();
		}
	texHandle = thmaker.MakeHandle(GetVPDisplayDIB(t,thmaker,texHandleValid));
	return texHandle->GetHandle();
}

ParamDlg* BerconWood::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) {	
	IAutoMParamDlg* masterDlg = BerconWoodDesc.CreateParamDlgs(hwMtlEdit, imp, this);
	texoutDlg = texout->CreateParamDlg(hwMtlEdit, imp);
	masterDlg->AddDlg(texoutDlg);
	BerconWood_param_blk.SetUserDlgProc(new BerconWoodDlgProc(this));
	BerconCurve_param_blk.SetUserDlgProc(new BerconCurveDlgProc(this));
	xyz_blk.SetUserDlgProc(new BerconXYZDlgProc(this));
	EnableStuff();
	return masterDlg;	
}

BOOL BerconWood::SetDlgThing(ParamDlg* dlg) {	
	if (dlg == texoutDlg)
		texoutDlg->SetThing(texout);
	else 
		return FALSE;
	return TRUE;
}

void BerconWood::SetSubTexmap(int i, Texmap *m)  {
	ReplaceReference(i+2,m);
	if (i==0) {
		BerconWood_param_blk.InvalidateUI(noise_map1);
		ivalid.SetEmpty();		
	} else if (i==1) {
		BerconWood_param_blk.InvalidateUI(noise_map2);
		ivalid.SetEmpty();		
	} else if (i==2) {
		BerconWood_param_blk.InvalidateUI(noise_map3);
		ivalid.SetEmpty();	
	} else if (i==3) {
		BerconWood_param_blk.InvalidateUI(distortion_map);
		ivalid.SetEmpty();	
	} else if (i==4) {
		BerconWood_param_blk.InvalidateUI(distortion_map2);
		ivalid.SetEmpty();
	}

	if (BMAP1_NUM <= i && i <= BMAP16_NUM) {  // TODO: Make range is correct
		BerconMap_param_blk.InvalidateUI(i-BMAP1_NUM);
		ivalid.SetEmpty();
	}
}

TSTR BerconWood::GetSubTexmapSlotName(int i){	
	switch(i) {
		case 0:  return TSTR(GetString(IDS_DS_COLOR1)); 
		case 1:  return TSTR(GetString(IDS_DS_COLOR2)); 
		case 2:  return TSTR(GetString(IDS_DS_COLOR3)); 
		case 3:  return TSTR(GetString(IDS_DISTORTION_MAP)); 
		case 4:  return TSTR(GetString(IDS_DISTORTION_MAP2)); 
		
		case BMAP1_NUM: return TSTR(GetString(IDS_MAP1)); 
		case BMAP2_NUM: return TSTR(GetString(IDS_MAP2)); 
		case BMAP3_NUM: return TSTR(GetString(IDS_MAP3)); 
		case BMAP4_NUM: return TSTR(GetString(IDS_MAP4)); 
		case BMAP5_NUM: return TSTR(GetString(IDS_MAP5)); 
		case BMAP6_NUM: return TSTR(GetString(IDS_MAP6)); 
		case BMAP7_NUM: return TSTR(GetString(IDS_MAP7)); 
		case BMAP8_NUM: return TSTR(GetString(IDS_MAP8)); 
		case BMAP9_NUM: return TSTR(GetString(IDS_MAP9)); 
		case BMAP10_NUM: return TSTR(GetString(IDS_MAP10)); 
		case BMAP11_NUM: return TSTR(GetString(IDS_MAP11)); 
		case BMAP12_NUM: return TSTR(GetString(IDS_MAP12)); 
		case BMAP13_NUM: return TSTR(GetString(IDS_MAP13)); 
		case BMAP14_NUM: return TSTR(GetString(IDS_MAP14)); 
		case BMAP15_NUM: return TSTR(GetString(IDS_MAP15)); 
		case BMAP16_NUM: return TSTR(GetString(IDS_MAP16)); 
		
		default: return TSTR(_T(""));
	}
}

RefTargetHandle BerconWood::GetReference(int i)  {
	switch (i) {
		case COORD_REF: return pbXYZ;
		case PBLOCK_REF: return pblock;
		case OUTPUT_REF: return texout;
		case CURVE_REF: return curve;
		case CURVEPB_REF: return pbCurve;
		case PBMAP_REF: return pbMap;
		default: return subtex[i-2];
	}
}

void BerconWood::SetReference(int i, RefTargetHandle rtarg) {	
	switch(i) {
		case COORD_REF: pbXYZ = (IParamBlock2 *)rtarg; break;
		case PBLOCK_REF: pblock = (IParamBlock2 *)rtarg; break;
		case OUTPUT_REF: texout = (TextureOutput *)rtarg; break;
		case CURVE_REF: curve = (ICurveCtl *)rtarg; break;
		case CURVEPB_REF: pbCurve = (IParamBlock2 *)rtarg; break;
		case PBMAP_REF: pbMap = (IParamBlock2 *)rtarg; break;
		default: subtex[i-2] = (Texmap *)rtarg; break;
	}
}

RefTargetHandle BerconWood::Clone(RemapDir &remap) {
	BerconWood *mnew = new BerconWood();
	*((MtlBase*)mnew) = *((MtlBase*)this); // copy superclass stuff
	mnew->ReplaceReference(COORD_REF,remap.CloneRef(pbXYZ));
	mnew->ReplaceReference(OUTPUT_REF,remap.CloneRef(texout));
	mnew->ReplaceReference(PBLOCK_REF,remap.CloneRef(pblock));
	mnew->ReplaceReference(CURVE_REF,remap.CloneRef(curve));
	mnew->ReplaceReference(CURVEPB_REF,remap.CloneRef(pbCurve));
	mnew->ReplaceReference(PBMAP_REF,remap.CloneRef(pbMap));
	mnew->ivalid.SetEmpty();		
	for (int i = 0; i<NSUBTEX; i++) {
		mnew->subtex[i] = NULL;
		if (subtex[i])
			mnew->ReplaceReference(i+2,remap.CloneRef(subtex[i]));		
	}

	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
}

Animatable* BerconWood::SubAnim(int i) {
	switch (i) {
		case COORD_REF: return pbXYZ;
		case PBLOCK_REF: return pblock;
		case CURVE_REF: return curve;
		case CURVEPB_REF: return pbCurve;
		case OUTPUT_REF: return texout;
		case PBMAP_REF: return pbMap;
		default: return subtex[i-2];
	}
}

TSTR BerconWood::SubAnimName(int i) {	
	switch (i) {
		case COORD_REF: return GetString(IDS_COORDS);		
		case PBLOCK_REF: return GetString(IDS_PARAMS);
		case CURVE_REF: return GetString(IDS_CURVE);
		case CURVEPB_REF: return GetString(IDS_CURVEPB);
		case OUTPUT_REF: return GetString(IDS_OUTPUT);
		case PBMAP_REF: return GetString(IDS_PBMAP);
		default: return GetSubTexmapTVName(i-2);
	}
}

RefResult BerconWood::NotifyRefChanged(NOTIFY_REF_CHANGED_ARGS) {
	switch (message) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();			
			if (hTarget == pblock) {
				ParamID changing_param = pblock->LastNotifyParamID();
				BerconWood_param_blk.InvalidateUI(changing_param);
				if (changing_param != -1) DiscardTexHandle();
			}  else if (hTarget == pbXYZ) {
				ParamID changing_param = pblock->LastNotifyParamID();
				xyz_blk.InvalidateUI(changing_param);
				if (changing_param != -1) DiscardTexHandle();
			}
			break;
		}
	return(REF_SUCCEED);
}

// Define some basic values
static AColor black(0.0f,0.0f,0.0f,0.0f);
static AColor white(1.0f,1.0f,1.0f,1.0f);

void BerconWood::applyDistortion(ShadeContext& sc, Point3& p) {
	if (subtex[3])
		if (subtex[4])
			p += subtex[3]->EvalNormalPerturb(sc)*distortionStr*subtex[4]->EvalMono(sc);
		else					
			p += subtex[3]->EvalNormalPerturb(sc)*distortionStr;
}

WoodParam BerconWood::EvalParameters(ShadeContext& sc) {
	WoodParam wp;
	wp.woodType = woodType;
	wp.randSeed = randSeed;
	wp.samples = samples;
	// Use maps
	if (mappedParameters) {	
		wp.lowTresh = mapOn[6]&&subtex[6]?subtex[6]->EvalMono(sc)*lowTresh:lowTresh;
		wp.highTresh = mapOn[7]&&subtex[7]?subtex[7]->EvalMono(sc)*highTresh:highTresh;
		wp.skew = mapOn[8]&&subtex[8]?subtex[8]->EvalMono(sc)*skew:skew;
		wp.widthVar = mapOn[9]&&subtex[9]?subtex[9]->EvalMono(sc)*widthVar:widthVar;
		wp.gainVar = mapOn[10]&&subtex[10]?subtex[10]->EvalMono(sc)*gainVar:gainVar;
		wp.trunkStr = mapOn[11]&&subtex[11]?subtex[11]->EvalMono(sc)*trunkStr:trunkStr;
		wp.trunkFreq = mapOn[12]&&subtex[12]?subtex[12]->EvalMono(sc)*trunkFreq:trunkFreq;
		wp.radialStr = mapOn[13]&&subtex[13]?subtex[13]->EvalMono(sc)*radialStr:radialStr;
		wp.radialFreq = mapOn[14]&&subtex[14]?subtex[14]->EvalMono(sc)*radialFreq:radialFreq;
		wp.radialZ = mapOn[15]&&subtex[15]?subtex[15]->EvalMono(sc)*radialZ:radialZ;
		wp.angleStr = mapOn[16]&&subtex[16]?subtex[16]->EvalMono(sc)*angleStr:angleStr;
		wp.angleFreq = mapOn[17]&&subtex[17]?subtex[17]->EvalMono(sc)*angleFreq:angleFreq;
		wp.angleRad = mapOn[18]&&subtex[18]?subtex[18]->EvalMono(sc)*angleRad:angleRad;
	} else {
		wp.lowTresh = lowTresh;
		wp.highTresh = highTresh;
		wp.skew = skew;
		wp.widthVar = widthVar;
		wp.gainVar = gainVar;
		wp.trunkStr = trunkStr;
		wp.trunkFreq = trunkFreq;
		wp.radialStr = radialStr;
		wp.radialFreq = radialFreq;
		wp.radialZ = radialZ;
		wp.angleStr = angleStr;
		wp.angleFreq = angleFreq;
		wp.angleRad = angleRad;	
	}
	return wp;
}

AColor BerconWood::EvalColor(ShadeContext& sc) {
	Point3 p,dpdx,dpdy;
	if (!sc.doMaps) return black;

	// If we've already evalutated the color at this point we'll use it and stop here
	AColor c;
	if (sc.GetCache(this,c)) 
		return c; 	
	if (gbufID) sc.SetGBufferID(gbufID);

	// Evaluate parameters
	WoodParam wp = EvalParameters(sc);
	float grainA = mapOn[19]&&subtex[19]?subtex[19]->EvalMono(sc)*grainAmount:grainAmount;
	float grainF = mapOn[20]&&subtex[20]?subtex[20]->EvalMono(sc)*grainFreq:grainFreq;

	// UVW, Distortion and size
	berconXYZ.get(sc,p,dpdx,dpdy);
	if (useDistortion)
		applyDistortion(sc,p);	
	float wSize = mapOn[5]&&subtex[5]?subtex[5]->EvalMono(sc)*woodSize:woodSize;
	p /= wSize; dpdx /= (wSize / 2.f); dpdy /= (wSize / 2.f);
		
	// Caluclate wood function and grain
	Point3 gP;
	
	float d = sc.filterMaps? Noise::wood(p, dpdx, dpdy, gP, wp) : Noise::wood(p, gP, wp);

	float g = (grainAmount > .001f) ? Fractal::grain(gP, grainA, grainF): 0.f;	

	// Get colors
	RGBA c0 = mapOn[0]&&subtex[0] ? subtex[0]->EvalColor(sc): col[0];
	RGBA c1 = mapOn[1]&&subtex[1] ? subtex[1]->EvalColor(sc): col[1];
	RGBA c2 = lockGrain ? c1: (mapOn[2]&&subtex[2] ? subtex[2]->EvalColor(sc): col[2]);
	
	// Apply curves
	if (useCurve)
		d = curve->GetControlCurve(0)->GetValue(sc.CurTime(), d);

	// Calculate color
	c = (1.0f-d)*c0 + d*c1;
	c = (1.0f-g)*c + g*c2;
	c = texout->Filter(c);

	// Cache
	sc.PutCache(this,c); 
	return c;	
}

float BerconWood::EvalMono(ShadeContext& sc) {
	return Intens(EvalColor(sc));
}

#define DELTA 0.001f
Point3 BerconWood::EvalNormalPerturb(ShadeContext& sc) {	
	Point3 p,dpdx,dpdy;
	
	if (!sc.doMaps) return Point3(0,0,0);
	if (gbufID) sc.SetGBufferID(gbufID);

	// Evaluate parameters
	WoodParam wp = EvalParameters(sc);
	float grainA = mapOn[19]&&subtex[19]?subtex[19]->EvalMono(sc)*grainAmount:grainAmount;
	float grainF = mapOn[20]&&subtex[20]?subtex[20]->EvalMono(sc)*grainFreq:grainFreq;

	// UVW, Distortion and size
	Point3 M[3];
	berconXYZ.get(sc, p, dpdx, dpdy, M);

	if (useDistortion)
		applyDistortion(sc,p);	
	float wSize = mapOn[5]&&subtex[5]?subtex[5]->EvalMono(sc)*woodSize:woodSize;
	p /= wSize; dpdx /= (wSize / 2.f); dpdy /= (wSize / 2.f);

	// Vectors
	bool grainON = (grainAmount > .001f); 
	Point3 np, nG, gP;
	
	float d = sc.filterMaps? Noise::wood(p, dpdx, dpdy, gP, wp) : Noise::wood(p, gP, wp);
	if (useCurve) d = curve->GetControlCurve(0)->GetValue(sc.CurTime(), d);
	
	float g = grainON ? Fractal::grain(gP, grainA, grainF): 0.f;	
	
	for (int i=0; i<3; i++) {		
		np[i] = sc.filterMaps? Noise::wood(p + DELTA * M[i], dpdx, dpdy, gP, wp) : Noise::wood(p + DELTA * M[i], gP, wp);
		if (useCurve) np[i] = curve->GetControlCurve(0)->GetValue(sc.CurTime(), np[i]);	
		np[i] = (np[i] - d) / DELTA;
		if (grainON) nG[i] = Fractal::grain(gP, grainA, grainF); // gP is updated by wood()
	}

	np = -sc.VectorFromNoScale(np, REF_OBJECT);
	nG = -sc.VectorFromNoScale(nG, REF_OBJECT);

	// Eval sub maps
	float f1, f2, f3;
	Point3 v1, v2, v3;
	bool maps = false;
	if (subtex[0]) {
		f1 = subtex[0]->EvalMono(sc);
		v1 = subtex[0]->EvalNormalPerturb(sc);
		maps = true;
	} else {
		f1 = Intens(col[0]);
		v1 = Point3(0.f, 0.f, 0.f);
	}
	if (subtex[1]) {
		f2 = subtex[1]->EvalMono(sc);
		v2 = subtex[1]->EvalNormalPerturb(sc);
		maps = true;
	} else {
		f2 = Intens(col[1]);
		v2 = Point3(0.f, 0.f, 0.f);
	}
	if (subtex[2]) {
		f3 = subtex[2]->EvalMono(sc);
		v3 = subtex[2]->EvalNormalPerturb(sc);
		maps = true;
	} else {
		f3 = Intens(col[2]);
		v3 = Point3(0.f, 0.f, 0.f);
	}
	
	// Calculate vector	
	if (maps) {
		np = (f2-f1)*np + d*v2 + (1.f-d)*v1;
		if (grainON) {
			float val = d*f1 + (1.f-d)*f2;
			np = (f3-val)*nG + g*v3 + (1.f-g)*np;
		}
	} else {
		np *= f2 - f1;
		if (grainON) {
			float val = d*f1 + (1.f-d)*f2;
			np = (f3-val)*nG + (1.f-g)*np;
		}
	}	

	return texout->Filter(np); // Does this filter actually do something?
}