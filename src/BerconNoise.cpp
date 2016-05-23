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

#include "BerconNoise.h"

#define NCOLS		2  // Number of colors

#define COORD_REF	0
#define PBLOCK_REF	1
#define MAP1_REF	2
#define MAP2_REF	3
#define MAP3_REF	4
#define MAP4_REF	5

#define BMAP1_REF	6
#define BMAP2_REF	7
#define BMAP3_REF	8
#define BMAP4_REF	9
#define BMAP5_REF	10
#define BMAP6_REF	11
#define BMAP7_REF	12
#define BMAP8_REF	13
#define BMAP9_REF	14
#define BMAP10_REF	15
#define BMAP11_REF	16
#define BMAP12_REF	17
#define BMAP13_REF	18
#define BMAP14_REF	19

#define BMAP1_NUM	4
#define BMAP2_NUM	5
#define BMAP3_NUM	6
#define BMAP4_NUM	7
#define BMAP5_NUM	8
#define BMAP6_NUM	9
#define BMAP7_NUM	10
#define BMAP8_NUM	11
#define BMAP9_NUM	12
#define BMAP10_NUM	13
#define BMAP11_NUM	14
#define BMAP12_NUM	15
#define BMAP13_NUM	16
#define BMAP14_NUM	17

#define OUTPUT_REF	20
#define CURVEPB_REF	21
#define CURVE_REF	22
#define PBMAP_REF	23

#define NAMELENGTH 64
typedef TCHAR TChBuffer[NAMELENGTH];

static BerconNoiseClassDesc BerconNoiseDesc;
ClassDesc2* GetBerconNoiseDesc() { return &BerconNoiseDesc; }

enum { xyz_params, berconnoise_params, BerconCurve_params, BerconMap_params };

static XYZ_Desc xyz_blk(&BerconNoiseDesc, COORD_REF, xyz_params, 2, 0, 0, 0);

enum { 
	// Fractal
	fractal_type,
	fractal_gain, fractal_offset, fractal_h, fractal_lacunarity, fractal_levels,
	// Treshold
	noise_lowthresh, noise_hithresh,
	// Distortion
	distortion_map, distortion_map2,
	use_distortion, distortion_str, uvw_dist,
	// Worley	
	noise_function_type,	
	worley_distance,	
	worley_spread,
	worley_F1, worley_F2, worley_F3, worley_F4,		
	// Standard noise controls
	noise_color1, noise_color2,
	noise_map1, noise_map2,		
	noise_map1_on, noise_map2_on, 
	noise_size, noise_phase,	
	// Output
	pb_output,
};

ParamDlg* BerconNoise::texoutDlg;

static ParamBlockDesc2 berconnoise_param_blk ( berconnoise_params, _T("params"),  0, &BerconNoiseDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 	
	IDD_PANEL, IDS_PARAMS, 0, 0, NULL,	
	// Maps
	noise_color1,	 _T("color1"),	TYPE_RGBA,				P_ANIMATABLE,	IDS_DS_COLOR1,	
		p_default,		Color(0,0,0), 
		p_ui,			TYPE_COLORSWATCH, IDC_NOISE_COL1, 
		p_end,
	noise_color2,	 _T("color2"),	TYPE_RGBA,				P_ANIMATABLE,	IDS_DS_COLOR2,	
		p_default,		Color(1.0, 1.0, 1.0), 
		p_ui,			TYPE_COLORSWATCH, IDC_NOISE_COL2, 
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
	noise_map1_on,	_T("map1Enabled"), TYPE_BOOL,			0,				IDS_JW_MAP1ENABLE,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MAPON1,
		p_end,
	noise_map2_on,	_T("map2Enabled"), TYPE_BOOL,			0,				IDS_JW_MAP2ENABLE,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MAPON2,
		p_end,

	// General
	noise_size,	_T("size"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_DS_NOISESIZE,
		p_default,		25.f,
		p_range,		0.0, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_NOISESIZE_EDIT, IDC_NOISESIZE_SPIN, SPIN_AUTOSCALE, 
		p_end,
	noise_lowthresh, _T("thresholdLow"), TYPE_FLOAT,	P_ANIMATABLE,	IDS_RB_LOWTHRESH,
		p_default,		0.0f,
		p_range,		0.0f, 1000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_NOISE_LOWTHRESH, IDC_NOISE_LOWTHRESHSPIN, SPIN_AUTOSCALE, 
		p_end,
	noise_hithresh, _T("thresholdHigh"), TYPE_FLOAT,	P_ANIMATABLE,	IDS_RB_HIGHTHRESH,
		p_default,		1.0f,
		p_range,		0.0f, 1000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_NOISE_HIGHTHRESH, IDC_NOISE_HIGHTHRESHSPIN, SPIN_AUTOSCALE, 
		p_end,

	// Noise controls
	noise_phase,		_T("phase"),	TYPE_FLOAT,	P_ANIMATABLE,	IDS_DS_PHASE,
		p_default,		0.f,
		p_range,		-1000.0, 1000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_NOISEPHASE_EDIT, IDC_NOISEPHASE_SPIN, SPIN_AUTOSCALE, 
		p_end,
	noise_function_type, _T("noise_function"), TYPE_INT,	0,				IDS_NOISE_FUNCTION,
		p_default,		1,		
		p_ui, 			TYPE_INTLISTBOX, IDC_NOISE_FUNCTION, 0, 
		p_end,
	worley_distance, _T("worleyDistance"),	TYPE_INT,		0,				IDS_WORLEY_DISTANCE,
		p_default,		0,		
		p_ui, 			TYPE_INTLISTBOX, IDC_WORLEY_DISTANCE, 0, 
		p_end,
	worley_F1,	_T("wF1"),		 TYPE_FLOAT,	P_ANIMATABLE,	IDS_WORLEY_F1,
		p_default,		1.0f,
		p_range,		-1000.0f, 1000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_F1_EDIT, IDC_F1_SPIN, SPIN_AUTOSCALE, 
		p_end,
	worley_F2,	_T("wF2"),		 TYPE_FLOAT,	P_ANIMATABLE,	IDS_WORLEY_F2,
		p_default,		0.0f,
		p_range,		-1000.0f, 1000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_F2_EDIT, IDC_F2_SPIN, SPIN_AUTOSCALE, 
		p_end,
	worley_F3,	_T("wF3"),		 TYPE_FLOAT,	P_ANIMATABLE,	IDS_WORLEY_F3,
		p_default,		0.0f,
		p_range,		-1000.0f, 1000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_F3_EDIT, IDC_F3_SPIN, SPIN_AUTOSCALE, 
		p_end,
	worley_F4,	_T("wF4"),		 TYPE_FLOAT,	P_ANIMATABLE,	IDS_WORLEY_F4,
		p_default,		0.0f,
		p_range,		-1000.0f, 1000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_F4_EDIT, IDC_F4_SPIN, SPIN_AUTOSCALE, 
		p_end,
	worley_spread,	_T("worleySpread"),		 TYPE_FLOAT,	P_ANIMATABLE,	IDS_WORLEY_SPREAD,
		p_default,		3.0f,
		p_range,		0.0f, 1000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_WORLEY_EDIT, IDC_WORLEY_SPIN, SPIN_AUTOSCALE, 
		p_end,

	// Fractal controls
	fractal_type,	_T("fractalType"),		TYPE_INT,		0,				IDS_FRACTAL_TYPE,
		p_default,		0,		
		p_ui, 			TYPE_INTLISTBOX, IDC_FRACTAL_TYPE, 0, 
		p_end,
	fractal_levels,	_T("levels"), TYPE_FLOAT,			P_ANIMATABLE,	IDS_DS_NSLEV,
		p_default,		3.0f,
		p_range,		1.0f, 20.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_NOISELEV_EDIT, IDC_NOISELEV_SPIN, SPIN_AUTOSCALE, 
		p_end,
	fractal_gain,	_T("fractalGain"), TYPE_FLOAT,			P_ANIMATABLE,	IDS_FRACTAL_GAIN,
		p_default,		0.0f,
		p_range,		0.0f, 100.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_FRACTAL_GAIN_EDIT, IDC_FRACTAL_GAIN_SPIN, SPIN_AUTOSCALE, 
		p_end,
	fractal_offset,	_T("fractalOffset"), TYPE_FLOAT,			P_ANIMATABLE,	IDS_FRACTAL_OFFSET,
		p_default,		0.0f,
		p_range,		0.0f, 100.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_FRACTAL_OFFSET_EDIT, IDC_FRACTAL_OFFSET_SPIN, SPIN_AUTOSCALE, 
		p_end,
	fractal_h,	_T("fractalH"), TYPE_FLOAT,			P_ANIMATABLE,	IDS_FRACTAL_H,
		p_default,		0.0f,
		p_range,		0.0f, 100.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_FRACTAL_H_EDIT, IDC_FRACTAL_H_SPIN, SPIN_AUTOSCALE, 
		p_end,
	fractal_lacunarity,	_T("fractalLacunarity"), TYPE_FLOAT,			P_ANIMATABLE,	IDS_FRACTAL_LACUNARITY,
		p_default,		0.0f,
		p_range,		0.0f, 100.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_FRACTAL_LACUNARITY_EDIT, IDC_FRACTAL_LACUNARITY_SPIN, SPIN_AUTOSCALE, 
		p_end,

	// Distortion controls
	distortion_map,		_T("map3"),		TYPE_TEXMAP,			P_OWNERS_REF,	IDS_DISTORTION_MAP,
		p_refno,		MAP3_REF,
		p_subtexno,		2,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_DISTORTION_TEX,
		p_end,
	distortion_map2,	_T("map4"),		TYPE_TEXMAP,			P_OWNERS_REF,	IDS_DISTORTION_MAP2,
		p_refno,		MAP4_REF,
		p_subtexno,		3,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_DISTORTION_TE2,
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
	uvw_dist, _T("UVWdistortion"), TYPE_INT,	0,				IDS_DIST_UVW,
		p_default,		1,		
		p_ui, 			TYPE_INTLISTBOX, IDC_DIST_UVW, 0, 
		p_end,
		
	// UVW, Output controls
	pb_output,			_T("output"),		TYPE_REFTARG,	P_OWNERS_REF,	IDS_OUTPUT,
		p_refno,		OUTPUT_REF, 
		p_end,
	p_end
);

enum { enable_curve, curve_cont };

static ParamBlockDesc2 BerconCurve_param_blk ( BerconCurve_params, _T("params"),  0, &BerconNoiseDesc, 
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
	map1, map2, map3, map4, map5, map6, map7, map8, map9, map10, map11, map12, map13, map14,
	mapOn1, mapOn2, mapOn3, mapOn4, mapOn5, mapOn6, mapOn7, mapOn8, mapOn9, mapOn10, mapOn11, mapOn12, mapOn13, mapOn14,
};

static ParamBlockDesc2 BerconMap_param_blk ( BerconMap_params, _T("params"),  0, &BerconNoiseDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBMAP_REF, 	
	IDD_PANEL_NOISE_MAP, IDS_PBMAP, 0, 1, NULL,
	
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

	p_end
);

class BerconCurveDlgProcNOISE : public ParamMap2UserDlgProc {
	public:
		BerconNoise *berconNoise;		
		BerconCurveDlgProcNOISE(BerconNoise *m) {berconNoise = m;}		
		INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam) {
			if (berconNoise->curve->GetHWND() != GetDlgItem(hWnd, IDC_CURVE))
				CurveCtrl::update(berconNoise->curve, GetDlgItem(hWnd, IDC_CURVE), static_cast<ReferenceMaker*>(berconNoise)); // Force update curve
			switch (msg) {
				case WM_INITDIALOG:
				case WM_SHOWWINDOW:
					CurveCtrl::update(berconNoise->curve, GetDlgItem(hWnd, IDC_CURVE), static_cast<ReferenceMaker*>(berconNoise));					
					break;
				case WM_DESTROY:
					CurveCtrl::disable(berconNoise->curve);
					break;
				default: return FALSE;
			}
			return TRUE;
		}
		void DeleteThis() {delete this;}
		void SetThing(ReferenceTarget *m) { 
			CurveCtrl::disable(berconNoise->curve); // Disable previously used curve
			berconNoise = (BerconNoise*)m;
		}
};

//dialog stuff to get the Set Ref button
class BerconNoiseDlgProc : public ParamMap2UserDlgProc {
public:
	BerconNoise *berconNoise;		
	BerconNoiseDlgProc(BerconNoise *m) {berconNoise = m;}		
	INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);		
	void DeleteThis() {delete this;}
	void SetThing(ReferenceTarget *m) {
		berconNoise = (BerconNoise*)m;
		berconNoise->EnableStuff();
	}
};

INT_PTR BerconNoiseDlgProc::DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)	{	
	switch (msg) {	
		case WM_INITDIALOG:	{			
			// UVW
			HWND hwndMap = GetDlgItem(hWnd, IDC_DIST_UVW);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_UVW_NORMAL));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_UVW_RADIAL));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_UVW_SPHERICAL));
			
			// Noise type
			hwndMap = GetDlgItem(hWnd, IDC_NOISE_FUNCTION);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NF_PERLIN2));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NF_PERLIN3));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NF_PERLIN4));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NF_SIMPLEX2));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NF_SIMPLEX3));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NF_SIMPLEX4));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NF_WORLEY));

			// Worley distance
			hwndMap = GetDlgItem(hWnd, IDC_WORLEY_DISTANCE);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_W_LIN));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_W_SQU));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_W_MAN));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_W_CHE));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_W_MI5));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_W_MI4));

			// Fractal type
			hwndMap = GetDlgItem(hWnd, IDC_FRACTAL_TYPE);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_F_REGULAR));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_F_FRACTAL));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_F_TURBULENCE));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_F_FBM));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_F_FBMTURB));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_F_HETEROTERRAIN));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_F_HYBRIDMULTIFRACTAL));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_F_RIDGEDMULTIFRACTAL));
			
			// Set correct dropdown value
			int curIndex;
			map->GetParamBlock()->GetValue(noise_function_type, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_NOISE_FUNCTION), CB_SETCURSEL, (WPARAM)curIndex, 0);						
			map->GetParamBlock()->GetValue(worley_distance, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_WORLEY_DISTANCE), CB_SETCURSEL, (WPARAM)curIndex, 0);			
			map->GetParamBlock()->GetValue(fractal_type, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_FRACTAL_TYPE), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(uvw_dist, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_DIST_UVW), CB_SETCURSEL, (WPARAM)curIndex, 0);
			break;
		}		
		case WM_SHOWWINDOW:	{
			// Set correct dropdown value
			int curIndex;
			map->GetParamBlock()->GetValue(noise_function_type, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_NOISE_FUNCTION), CB_SETCURSEL, (WPARAM)curIndex, 0);						
			map->GetParamBlock()->GetValue(worley_distance, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_WORLEY_DISTANCE), CB_SETCURSEL, (WPARAM)curIndex, 0);			
			map->GetParamBlock()->GetValue(fractal_type, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_FRACTAL_TYPE), CB_SETCURSEL, (WPARAM)curIndex, 0);		
			map->GetParamBlock()->GetValue(uvw_dist, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_DIST_UVW), CB_SETCURSEL, (WPARAM)curIndex, 0);
			break;
		}
		default: return FALSE;
	}
	return TRUE;
}

//--- BerconNoise -------------------------------------------------------
BerconNoise::BerconNoise() {	
	for (int i=0; i<NOISE_NSUBTEX; i++) subtex[i] = NULL;
	texHandle = NULL;
	pblock = NULL;
	pbCurve = NULL;
	pbMap = NULL;
	pbXYZ = NULL;
	BerconNoiseDesc.MakeAutoParamBlocks(this);
	texout = NULL;	
	curve = NULL;		
	Reset();
}

BerconNoise::~BerconNoise() { DiscardTexHandle(); }
//From MtlBase
void BerconNoise::Reset() {
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

	for (int i=0; i<NOISE_NSUBTEX; i++) 
		DeleteReference(i+2);	

	pblock->SetValue( noise_color1, t, Color(0.f, 0.f, 0.f));
	pblock->SetValue( noise_color2, t, Color(1.f, 1.f, 1.f));
	pblock->SetValue( noise_map1_on, t, TRUE);
	pblock->SetValue( noise_map2_on, t,	TRUE);		
	pblock->SetValue( distortion_str, t, .1f);
	pblock->SetValue( use_distortion, t, FALSE);
	pblock->SetValue( uvw_dist, t, 0);
	pblock->SetValue( noise_function_type, t, 1);
	pblock->SetValue( fractal_type, t, 0);
	pblock->SetValue( worley_distance, t, 0);
	pblock->SetValue( noise_lowthresh, t, 0.f);
	pblock->SetValue( noise_hithresh, t, 1.f);
	pblock->SetValue( worley_spread, t, 3.f);			
	pblock->SetValue( worley_F1, t,	1.f);
	pblock->SetValue( worley_F2, t,	0.f);
	pblock->SetValue( worley_F3, t,	0.f);
	pblock->SetValue( worley_F4, t,	0.f);
	pblock->SetValue( noise_phase, t, 0.f);
	pblock->SetValue( noise_size, t, 25.f);
	pblock->SetValue( fractal_gain, t, 5.f);
	pblock->SetValue( fractal_offset, t, 0.f);
	pblock->SetValue( fractal_h, t, .5f);
	pblock->SetValue( fractal_lacunarity, t, 2.f);
	pblock->SetValue( fractal_levels, t, 3.f);	

	// Maps
	for (int i=14; i<28; i++)
		pbMap->SetValue(i, t, TRUE);	

	berconXYZ.reset(pbXYZ, ivalid, 2, 0, 0, 0);

	ivalid.SetEmpty();	
}

void BerconNoise::Update(TimeValue t, Interval& valid) {	
	if (pblock == NULL) return;

	if (!ivalid.InInterval(t)) {
		ivalid.SetInfinite();

		if (texout != NULL)
			texout->Update(t,ivalid);

		pblock->GetValue( noise_color1, t, col[0], ivalid );
		col[0].ClampMinMax();
		pblock->GetValue( noise_color2, t, col[1], ivalid );
		col[1].ClampMinMax();		
		
		for (int i=0; i<NOISE_NSUBTEX; i++)
			if (subtex[i]) 
				subtex[i]->Update(t,ivalid);

		pblock->GetValue( noise_map1_on, t, mapOn[0], ivalid);
		pblock->GetValue( noise_map2_on, t, mapOn[1], ivalid);				

		// General
		pblock->GetValue( noise_hithresh, t, high, ivalid );
		pblock->GetValue( noise_lowthresh, t, low, ivalid );	
		pblock->GetValue( noise_size, t,   size, ivalid );		

		// Distortion
		pblock->GetValue( distortion_str, t, distortionStr, ivalid);
		pblock->GetValue( use_distortion, t, useDistortion, ivalid);
		pblock->GetValue( uvw_dist, t, uvwDist, ivalid);

		// Update noise parameters
		pblock->GetValue( noise_phase, t,			phase , ivalid);
		pblock->GetValue( worley_spread, t,			spread , ivalid);
		pblock->GetValue( worley_F1, t,				F1 , ivalid);
		pblock->GetValue( worley_F2, t,				F2 , ivalid);
		pblock->GetValue( worley_F3, t,				F3 , ivalid);
		pblock->GetValue( worley_F4, t,				F4 , ivalid);

		// Static noise params
		pblock->GetValue( noise_function_type, t,	noiseFunction , ivalid);
		pblock->GetValue( worley_distance, t,		worleyFunction , ivalid);		
		pblock->GetValue( fractal_type, t,			fractalFunction, ivalid);

		// Update fractal parameters
		pblock->GetValue( fractal_levels, t,  levels, ivalid );
		pblock->GetValue( fractal_h, t, fractalH, ivalid);
		pblock->GetValue( fractal_offset, t, fractalOffset, ivalid);
		pblock->GetValue( fractal_gain, t, fractalGain, ivalid);
		pblock->GetValue( fractal_lacunarity, t, fractalLacunarity, ivalid);
	
		if (high<low) {
			float tmp = low;
			low = high;
			high = tmp;
		}

		// Update maps
		for (int i = 0; i<14; i++)
			pbMap->GetValue((i+14), t, mapOn[i+4], ivalid);		

		// Curve
		pbCurve->GetValue(enable_curve, t, useCurve, ivalid);

		EnableStuff();		

		// Slight optimization
		mappedParameters = false;
		for (int i=5;i<=17;i++)
			if (mapOn[i] && subtex[i])
				mappedParameters = true;			

		//calcAverage(); // Update average value		
		berconXYZ.update(pbXYZ, t, ivalid);
	}
	
	valid &= ivalid;
}

Interval BerconNoise::Validity(TimeValue t) {
	Interval v;
	Update(t,v);
	return ivalid;
}

void BerconNoise::EnableStuff() {	
	if (pblock) {
		IParamMap2 *map = pblock->GetMap();		
		if (map) {
			// Update values we need to know
			pblock->GetValue( use_distortion, 0, useDistortion, FOREVER);
			pblock->GetValue( noise_function_type, 0, noiseFunction, FOREVER);
			pblock->GetValue( fractal_type, 0, fractalFunction, FOREVER);

			// Update parameters			
			map->Enable(noise_phase, noiseFunction==2||noiseFunction==5?TRUE:FALSE);
			
			map->Enable(worley_F1, noiseFunction==6?TRUE:FALSE);
			map->Enable(worley_F2, noiseFunction==6?TRUE:FALSE);
			map->Enable(worley_F3, noiseFunction==6?TRUE:FALSE);
			map->Enable(worley_F4, noiseFunction==6?TRUE:FALSE);
			map->Enable(worley_distance, noiseFunction==6?TRUE:FALSE);
			map->Enable(worley_spread, noiseFunction==6?TRUE:FALSE);			

			map->Enable(fractal_levels, fractalFunction!=0?TRUE:FALSE);			
			map->Enable(fractal_lacunarity, fractalFunction>2?TRUE:FALSE);			
			map->Enable(fractal_h, fractalFunction>2?TRUE:FALSE);			
			map->Enable(fractal_offset, fractalFunction>4?TRUE:FALSE);			
			map->Enable(fractal_gain, fractalFunction>5?TRUE:FALSE);

			map->Enable(distortion_str, useDistortion);
			map->Enable(distortion_map, useDistortion);
			map->Enable(distortion_map2, useDistortion);
		}
	}
}

// #############################################################################################
// #################################/ General stuff            \################################
// #############################################################################################

DWORD_PTR BerconNoise::GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker) {
	if (texHandle) {
		if (texHandleValid.InInterval(t))
			return texHandle->GetHandle();
		else DiscardTexHandle();
		}
	texHandle = thmaker.MakeHandle(GetVPDisplayDIB(t,thmaker,texHandleValid));
	return texHandle->GetHandle();
}

ParamDlg* BerconNoise::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) {	
	IAutoMParamDlg* masterDlg = BerconNoiseDesc.CreateParamDlgs(hwMtlEdit, imp, this);
	texoutDlg = texout->CreateParamDlg(hwMtlEdit, imp);
	masterDlg->AddDlg(texoutDlg);
	berconnoise_param_blk.SetUserDlgProc(new BerconNoiseDlgProc(this));
	BerconCurve_param_blk.SetUserDlgProc(new BerconCurveDlgProcNOISE(this));
	xyz_blk.SetUserDlgProc(new BerconXYZDlgProc(this));	
	EnableStuff();
	return masterDlg;	
}

BOOL BerconNoise::SetDlgThing(ParamDlg* dlg) {	
	if (dlg == texoutDlg)
		texoutDlg->SetThing(texout);
	else 
		return FALSE;
	return TRUE;
}

void BerconNoise::SetSubTexmap(int i, Texmap *m)  {
	ReplaceReference(i+2,m);
	if (i==0) {
		berconnoise_param_blk.InvalidateUI(noise_map1);
		ivalid.SetEmpty();		
	} else if (i==1) {
		berconnoise_param_blk.InvalidateUI(noise_map2);
		ivalid.SetEmpty();		
	} else if (i==2) {
		berconnoise_param_blk.InvalidateUI(distortion_map);
		ivalid.SetEmpty();
	}  else if (i==3) {
		berconnoise_param_blk.InvalidateUI(distortion_map2);
		ivalid.SetEmpty();
	}

	if (BMAP1_NUM <= i && i <= BMAP14_NUM) {
		BerconMap_param_blk.InvalidateUI(i-BMAP1_NUM);
		ivalid.SetEmpty();
	}
}

TSTR BerconNoise::GetSubTexmapSlotName(int i) {	
	switch(i) {
		case 0:  return TSTR(GetString(IDS_DS_COLOR1)); 
		case 1:  return TSTR(GetString(IDS_DS_COLOR2)); 
		case 2:  return TSTR(GetString(IDS_DISTORTION_MAP)); 
		case 3:  return TSTR(GetString(IDS_DISTORTION_MAP2)); 

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

		default: return TSTR(_T(""));
	}
}

RefTargetHandle BerconNoise::GetReference(int i)  {
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

void BerconNoise::SetReference(int i, RefTargetHandle rtarg) {	
	switch(i) {
		case COORD_REF:  pbXYZ = (IParamBlock2 *)rtarg; break;
		case PBLOCK_REF: pblock = (IParamBlock2 *)rtarg; break;
		case OUTPUT_REF: texout = (TextureOutput *)rtarg; break;
		case CURVE_REF: curve = (ICurveCtl *)rtarg; break;
		case CURVEPB_REF: pbCurve = (IParamBlock2 *)rtarg; break;
		case PBMAP_REF: pbMap = (IParamBlock2 *)rtarg; break;
		default: subtex[i-2] = (Texmap *)rtarg; break;
	}
}

RefTargetHandle BerconNoise::Clone(RemapDir &remap) {
	BerconNoise *mnew = new BerconNoise();
	*((MtlBase*)mnew) = *((MtlBase*)this); // copy superclass stuff
	mnew->ReplaceReference(COORD_REF,remap.CloneRef(pbXYZ));
	mnew->ReplaceReference(OUTPUT_REF,remap.CloneRef(texout));
	mnew->ReplaceReference(PBLOCK_REF,remap.CloneRef(pblock));
	mnew->ReplaceReference(CURVE_REF,remap.CloneRef(curve));
	mnew->ReplaceReference(CURVEPB_REF,remap.CloneRef(pbCurve));
	mnew->ReplaceReference(PBMAP_REF,remap.CloneRef(pbMap));
	mnew->ivalid.SetEmpty();		
	for (int i = 0; i<NOISE_NSUBTEX; i++) {
		mnew->subtex[i] = NULL;
		if (subtex[i])
			mnew->ReplaceReference(i+2,remap.CloneRef(subtex[i]));
		mnew->mapOn[i] = mapOn[i];
		}
	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
}

Animatable* BerconNoise::SubAnim(int i) {
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

TSTR BerconNoise::SubAnimName(int i) {	
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

RefResult BerconNoise::NotifyRefChanged(NOTIFY_REF_CHANGED_ARGS) {
	switch (message) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();			
			if (hTarget == pblock) {
				ParamID changing_param = pblock->LastNotifyParamID();
				berconnoise_param_blk.InvalidateUI(changing_param);
				if (changing_param != -1) DiscardTexHandle();
			}  else if (hTarget == pbCurve) {
				ParamID changing_param = pbCurve->LastNotifyParamID();
				BerconCurve_param_blk.InvalidateUI(changing_param);
				if (changing_param != -1) DiscardTexHandle();
			}  else if (hTarget == pbXYZ) {
				ParamID changing_param = pbXYZ->LastNotifyParamID();
				xyz_blk.InvalidateUI(changing_param);
				if (changing_param != -1) DiscardTexHandle();
			}
			break;
		}
	return(REF_SUCCEED);
}

// #############################################################################################
// #################################/ Shading starts here      \################################
// #############################################################################################

// Define some basic values
static AColor black(0.0f,0.0f,0.0f,0.0f);
static AColor white(1.0f,1.0f,1.0f,1.0f);

void BerconNoise::applyDistortion(ShadeContext& sc, Point3& p) {
	if (subtex[2])
		if (subtex[3])
			p += subtex[2]->EvalNormalPerturb(sc)*distortionStr*subtex[3]->EvalMono(sc);
		else					
			p += subtex[2]->EvalNormalPerturb(sc)*distortionStr;
}

NoiseParams BerconNoise::EvalParameters(ShadeContext* sc) {
	NoiseParams np;
	np.noiseFunction = noiseFunction;
	np.fractalFunction = fractalFunction;
	np.worleyFunction = worleyFunction;
	if (mappedParameters) {
		np.low			= (mapOn[5] && subtex[5])?subtex[5]->EvalMono(*sc)*low:low;	
		np.high			= (mapOn[6] && subtex[6])?subtex[6]->EvalMono(*sc)*high:high;
		np.phase		= (mapOn[7] && subtex[7])?subtex[7]->EvalMono(*sc)*phase:phase;
		np.spread		= (mapOn[8] && subtex[8])?subtex[8]->EvalMono(*sc)*spread:spread;
		np.F1			= (mapOn[9] && subtex[9])?subtex[9]->EvalMono(*sc)*F1:F1;
		np.F2			= (mapOn[10] && subtex[10])?subtex[10]->EvalMono(*sc)*F2:F2;
		np.F3			= (mapOn[11] && subtex[11])?subtex[11]->EvalMono(*sc)*F3:F3;
		np.F4			= (mapOn[12] && subtex[12])?subtex[12]->EvalMono(*sc)*F4:F4;
		np.levels		= (mapOn[13] && subtex[13])?subtex[13]->EvalMono(*sc)*levels:levels;
		np.offset		= (mapOn[14] && subtex[14])?subtex[14]->EvalMono(*sc)*fractalOffset:fractalOffset;
		np.gain			= (mapOn[15] && subtex[15])?subtex[15]->EvalMono(*sc)*fractalGain:fractalGain;
		np.lacunarity	= (mapOn[16] && subtex[16])?subtex[16]->EvalMono(*sc)*fractalLacunarity:fractalLacunarity;
		np.H			= (mapOn[17] && subtex[17])?subtex[17]->EvalMono(*sc)*fractalH:fractalH;
	} else {
		np.low			= low;	
		np.high			= high;
		np.phase		= phase;
		np.spread		= spread;
		np.F1			= F1;
		np.F2			= F2;
		np.F3			= F3;
		np.F4			= F4;
		np.levels		= levels;
		np.offset		= fractalOffset;
		np.gain			= fractalGain;
		np.lacunarity	= fractalLacunarity;
		np.H			= fractalH;
	}
	return np;
}		  

/*
#define AVGSAMPLES 10000 // Samples computed for the average value
#define AVGDELTA 10000.f // Range of samples computed for the average value
void BerconNoise::calcAverage() {
	if (mappedParameters) return; // Computing average is impossible
	average = 0.f;
	NoiseParams np = EvalParameters(NULL); // No mapped parameters, no need for shadecontext
	srand(3141);
	for (int i=0; i<AVGSAMPLES; i++)
		average += calcNoise(Point3(UFRAND()*AVGDELTA, UFRAND()*AVGDELTA, UFRAND()*AVGDELTA), np);
	average /= (float)AVGSAMPLES;
}*/

AColor BerconNoise::EvalColor(ShadeContext& sc) {
	if (!sc.doMaps) return black;
	
	AColor c;
	if (sc.GetCache(this,c)) 
		return c; 	
	if (gbufID) sc.SetGBufferID(gbufID);

	// UVW and Distortion
	Point3 p, dpdx, dpdy, dp;	
	if(!berconXYZ.get(sc, p, dpdx, dpdy)) return AColor(0,0,0,0);

	if (useDistortion)
		applyDistortion(sc,p);	
	float nSize = (mapOn[4] && subtex[4]) ? subtex[4]->EvalMono(sc)*size : size;
	p /= nSize; dpdx /= nSize; dpdy /= nSize;
	
	Noise::alterUVW(p, uvwDist);
	NoiseParams np = EvalParameters(&sc);
		
	// Caluclate noise function
	float d = sc.filterMaps ? Noise::limitedNoise(p, dpdx, dpdy, np) : Noise::limitedNoise(p, np);	
	if (useCurve)
		d = curve->GetControlCurve(0)->GetValue(sc.CurTime(), d);

	// Get colors
	RGBA c0 = mapOn[0]&&subtex[0] ? subtex[0]->EvalColor(sc): col[0];
	RGBA c1 = mapOn[1]&&subtex[1] ? subtex[1]->EvalColor(sc): col[1];				
	c = texout->Filter((1.f-d)*c0 + d*c1);

	// Cache
	sc.PutCache(this,c); 
	return c;	
}

float BerconNoise::EvalMono(ShadeContext& sc) {
	// TODO: Evaluate two maps with mono instead of color, slightly faster...
	return Intens(EvalColor(sc));
}

/*
	Notice we evaluate maps that affect the parameters only once, this means change in them
	doesn't contribute to the bump vector correctly. Evaluating these submaps multiple times
	would be very computationally heavy and usually their effect is very small so its better
	to leave them out and just accept the error.
*/
#define DELTA 0.001f
Point3 BerconNoise::EvalNormalPerturb(ShadeContext& sc) {		
	if (!sc.doMaps) return Point3(0,0,0);
	if (gbufID) sc.SetGBufferID(gbufID);

	// UVW and Distortion
	Point3 p, dpdx, dpdy;
	Point3 M[3];
	if (!berconXYZ.get(sc, p, dpdx, dpdy, M)) return Point3(0,0,0);

	if (useDistortion)
		applyDistortion(sc,p);	
	float nSize = (mapOn[4] && subtex[4]) ? subtex[4]->EvalMono(sc)*size : size;
	p /= nSize;
	Noise::alterUVW(p, uvwDist);

	NoiseParams np = EvalParameters(&sc);

	// Vector
	Point3 normal;
	float d = Noise::limitedNoise(p, np);
	if (useCurve) {		
		d = curve->GetControlCurve(0)->GetValue(sc.CurTime(), d);
		normal.x = (curve->GetControlCurve(0)->GetValue(sc.CurTime(), Noise::limitedNoise(p+DELTA*M[0], np)) - d) / DELTA;
		normal.y = (curve->GetControlCurve(0)->GetValue(sc.CurTime(), Noise::limitedNoise(p+DELTA*M[1], np)) - d) / DELTA;
		normal.z = (curve->GetControlCurve(0)->GetValue(sc.CurTime(), Noise::limitedNoise(p+DELTA*M[2], np)) - d) / DELTA;
	} else {
		normal.x = (Noise::limitedNoise(p+DELTA*M[0], np) - d) / DELTA;
		normal.y = (Noise::limitedNoise(p+DELTA*M[1], np) - d) / DELTA;
		normal.z = (Noise::limitedNoise(p+DELTA*M[2], np) - d) / DELTA;
	}	
	normal = -sc.VectorFromNoScale(normal, REF_OBJECT);

	// Eval sub maps
	float f1, f2;
	Point3 v1, v2;
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
	
	// Calculate vector
	if (maps)
		normal = (f2-f1)*normal + d*v2 + (1.f-d)*v1;
	else
		normal *= f2 - f1;

	return texout->Filter(normal); // Does this filter actually do something?
}