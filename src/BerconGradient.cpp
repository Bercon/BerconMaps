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

#include "BerconGradient.h"

#define PBLOCK_REF	1
#define COORD_REF	0
#define OUTPUT_REF	2
#define CURVE_REF	3
#define CURVEPB_REF	4

#define KEYTEX_REF	6
#define KEYTEX_NUM	1

#define MAPTEX_REF	5
#define MAPTEX_NUM	0

#define DISTEX_REF	7
#define DISTEX_NUM	2

#define REF_OFFSET	5
#define SUBMAPCOUNT 3

static BerconGradientClassDesc BerconGradientDesc;
ClassDesc2* GetBerconGradientDesc() { return &BerconGradientDesc; }

// #############################################################################################
// #################################/ User interface def       \################################
// #############################################################################################

enum { xyz_params, gradientmap_params, BerconCurve_params };

static XYZ_Desc xyz_blk(&BerconGradientDesc, COORD_REF, xyz_params, 0, 1, 1, 1);

enum { 
	pb_submaps,
	pb_colors,
	pb_positions,
	
	pb_keyTex,
	pb_keyCol,
	pb_keyPos,
	pb_keyNum,

	pb_type,
	pb_interpolation,
	pb_reverse,

	pb_seed,
	pb_rand_mat,
	pb_rand_obj,
	pb_rand_par,
	pb_rand_tile,

	pb_range_min,
	pb_range_max,
	pb_range_loop,

	pb_gradient_uvw,
	pb_gradient_normal,
	pb_gradient_normal2,

	pb_ior,

	pb_node,	
	pb_maptex,

	pb_distex,
	pb_disstr,
	pb_dison,
	
	pb_output,
};

enum {
	pb_curve_on,
	pb_curve,
};

ParamDlg* BerconGradient::texoutDlg;

/*
#define P_ANIMATABLE		0x00000001	//!< animatable param
#define P_TRANSIENT			0x00000002	//!< do not store actual value, PBAccessor-derived
#define P_NO_INIT			0x00000004	//!< do not initialize
#define P_COMPUTED_NAME		0x00000008	//!< call compute name fn to get name
#define P_INVISIBLE			0x00000010	//!< not visible in track view (if an animatable)
#define P_RESET_DEFAULT		0x00000020	//!< do not make create params sticky, reset to defaults always
#define P_SUBANIM			0x00000040	//!< non-animatable reference param is still a subanim (makes it visible in TV)
#define P_TV_SHOW_ALL		0x00000080	//!< for Tab<> animatables, show all entries even if no controller assigned
#define P_NO_REF			0x00000100	//!< for reftarg params do not maintain Reference automatically
#define P_OWNERS_REF		0x00000200	//!< reference param maintained by owner, specify owner's refno in a p_refno =>P_NO_REF
#define P_CAN_CONVERT		0x00000400	//!< indicates the p_classid validator is is in a CanConvertoTo() call, rather than as exact class
#define P_SUBTEX			0x00000800	//!< indicates texmap param is kept by owner using MtlBase::xSubTexmap protocol, give subtex # in p_subtexno
#define P_VARIABLE_SIZE		0x00001000	//!< Tab<> param is variable size allowing scripted changes
#define P_NO_AUTO_LABELS	0x00002000	//!< don't auto-set map & mtl names for associated button UI controls
#define P_SHORT_LABELS		0x00004000	//!< use short auto names for associated button UI controls
#define P_READ_ONLY			0x00008000	//!< this parameter is not assignable through MAXScript (allows try-and-buy 3rd-party plugins)
*/

static ParamBlockDesc2 gradientmap_param_blk ( gradientmap_params, _T("params"),  0, &BerconGradientDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 	
	IDD_PANEL_COLOR, IDS_PARAMS, 0, 0, NULL,

	// #################### // Map / Node \\ ####################

	pb_maptex,		_T("mapTex"),		TYPE_TEXMAP,			P_OWNERS_REF,	IDS_MAP,
		p_refno,		MAPTEX_REF,
		p_subtexno,		MAPTEX_NUM,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_MAPTEX,
		p_end,

	pb_node, 			_T("node"), 		TYPE_INODE, 	0,		IDS_NODE,
		p_ui, 			TYPE_PICKNODEBUTTON, IDC_NODE, 
		p_end, 
	
	// #################### // Keys \\ ####################
	pb_submaps,		_T("submaps"),		TYPE_TEXMAP_TAB,	0, P_VARIABLE_SIZE|P_SUBANIM, IDS_SUBMAPS,						
		p_end,
	pb_colors,		_T("colors"),		TYPE_FRGBA_TAB,		0, P_VARIABLE_SIZE|P_ANIMATABLE, IDS_SUBCOLORS,						
		p_end,
	pb_positions,	_T("positions"),	TYPE_FLOAT_TAB,		0, P_VARIABLE_SIZE|P_ANIMATABLE, IDS_SUBPOSITIONS,						
		p_end,

	// #################### // Key controls \\ ####################

	pb_keyTex,		_T("keyTex"),		TYPE_TEXMAP,	P_OWNERS_REF|P_TRANSIENT|P_INVISIBLE,	IDS_KEYTEX,
		p_refno,		KEYTEX_REF,
		p_subtexno,		KEYTEX_NUM,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_KEYTEX,
		p_end,

	pb_keyNum,		_T("keyNum"),		TYPE_INT,		P_TRANSIENT,	IDS_KEYNUM,
		p_default,		0,
		p_range,		0, 100000,
		p_ui, 			TYPE_SPINNER,	EDITTYPE_INT, IDC_KEYNUM_EDIT, IDC_KEYNUM_SPIN, SPIN_AUTOSCALE, 
		p_end,

	pb_keyPos,		_T("keyPos"),		TYPE_FLOAT,		P_TRANSIENT,	IDS_KEYPOS,
		p_default,		0.f,
		p_range,		0.0, 1.0f,
		p_ui, 			TYPE_SPINNER,	EDITTYPE_FLOAT, IDC_KEYPOS_EDIT, IDC_KEYPOS_SPIN, SPIN_AUTOSCALE, 
		p_end,

	pb_keyCol,		_T("keyCol"),		TYPE_RGBA,		P_TRANSIENT,	IDS_KEYCOL,	
		p_default,		Color(0.0, 0.0, 0.0), 
		p_ui,			TYPE_COLORSWATCH, IDC_KEYCOL, 
		p_end,

	// #################### // General \\ ####################

	pb_type,		_T("type"),			TYPE_INT,		P_ANIMATABLE, IDS_GRADIENTTYPE,
		p_default,		0,		
		p_ui, 			TYPE_INTLISTBOX, IDC_GRADIENT_TYPE, 0, 
		p_end,
		
	pb_interpolation,_T("interpolation"),TYPE_INT,		P_ANIMATABLE, IDS_GRADIENTINTERPOLATION,
		p_default,		0,		
		p_ui, 			TYPE_INTLISTBOX, IDC_GRADIENT_INT, 0, 
		p_end,

	pb_reverse,	_T("reverse"),	TYPE_BOOL,		P_ANIMATABLE, IDS_REVERSE,
		p_default,		FALSE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_REVERSE,
		p_end,

	// #################### // Random \\ ####################

	pb_seed,		_T("seed"), 		TYPE_INT, 		P_ANIMATABLE, 	IDS_SEED, 
		p_default, 12345, p_range, 1, 99999,
		p_ui, 			TYPE_SPINNER,	EDITTYPE_INT,	IDC_SEED_EDIT,	IDC_SEED_SPIN, SPIN_AUTOSCALE, 
		p_end,

	pb_rand_mat,	_T("randByMat"),	TYPE_BOOL,		P_ANIMATABLE, IDS_RANDBYMAT,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_RAND_MAT,
		p_end,

	pb_rand_obj,	_T("randByObj"),	TYPE_BOOL,		P_ANIMATABLE, IDS_RANDBYOBJ,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_RAND_OBJ,
		p_end,

	pb_rand_par,	_T("randByPar"),	TYPE_BOOL,		P_ANIMATABLE, IDS_RANDBYPAR,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_RAND_PAR,
		p_end,

	pb_rand_tile,	_T("randByTile"),	TYPE_BOOL,		P_ANIMATABLE, IDS_RANDBYPAR,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_RAND_TILE,
		p_end,	

	// #################### // Range \\ ####################

	pb_range_min,		_T("rangeMin"),		TYPE_FLOAT,		P_ANIMATABLE,	IDS_RANGEMIN,
		p_default,		0.f,
		p_range,		-1000000.0f, 1000000.0f,
		p_ui, 			TYPE_SPINNER,	EDITTYPE_FLOAT, IDC_RANGEMIN_EDIT, IDC_RANGEMIN_SPIN, SPIN_AUTOSCALE, 
		p_end,

	pb_range_max,		_T("rangeMax"),		TYPE_FLOAT,		P_ANIMATABLE,	IDS_RANGEMAX,
		p_default,		1.f,
		p_range,		-1000000.0f, 1000000.0f,
		p_ui, 			TYPE_SPINNER,	EDITTYPE_FLOAT, IDC_RANGEMAX_EDIT, IDC_RANGEMAX_SPIN, SPIN_AUTOSCALE, 
		p_end,

	pb_range_loop,		_T("rangeLoop"),	TYPE_INT,		P_ANIMATABLE,				IDS_RANGELOOP,
		p_default,		0,
		p_ui, 			TYPE_INTLISTBOX, IDC_RANGELOOP, 0, 
		p_end,

	// #################### // UVW \\ ####################

	pb_gradient_uvw,	_T("gradientUVW"),TYPE_INT,		P_ANIMATABLE, IDS_GRADIENTUVW,
		p_default,		0,		
		p_ui, 			TYPE_INTLISTBOX, IDC_GRADIENT_UVW, 0, 
		p_end,

	// #################### // Normal \\ ####################

	pb_gradient_normal,	_T("normalType"),TYPE_INT,		P_ANIMATABLE, IDS_NORMALTYPE,
		p_default,		0,		
		p_ui, 			TYPE_INTLISTBOX, IDC_GRADIENT_NORMAL, 0, 
		p_end,

	pb_gradient_normal2,	_T("normalFunction"),TYPE_INT,		P_ANIMATABLE, IDS_NORMALFUNCTION,
		p_default,		0,		
		p_ui, 			TYPE_INTLISTBOX, IDC_GRADIENT_NORMAL2, 0, 
		p_end,	

	pb_ior,					_T("IOR"),		TYPE_FLOAT,		P_ANIMATABLE,	IDS_IOR,
		p_default,		1.6f,
		p_range,		1.0, 1000000.0f,
		p_ui, 			TYPE_SPINNER,	EDITTYPE_FLOAT, IDC_IOR_EDIT, IDC_IOR_SPIN, SPIN_AUTOSCALE, 
		p_end,

	// #################### // Distortion \\ ####################

	pb_distex,		_T("disTex"),		TYPE_TEXMAP,			P_OWNERS_REF,	IDS_DISTORTION_MAP,
		p_refno,		DISTEX_REF,
		p_subtexno,		DISTEX_NUM,		
		p_ui,			TYPE_TEXMAPBUTTON, IDC_DISTEX,
		p_end,

	pb_disstr,		_T("disStr"),		TYPE_FLOAT,		P_ANIMATABLE,	IDS_DISTORTION_STRENGTH,
		p_default,		0.f,
		p_range,		0.0, 1000000.0f,
		p_ui, 			TYPE_SPINNER,	EDITTYPE_FLOAT, IDC_DISTSTR_EDIT, IDC_DISTSTR_SPIN, SPIN_AUTOSCALE, 
		p_end,

	pb_dison,	_T("disOn"),	TYPE_BOOL,		P_ANIMATABLE, IDS_DISTON,
		p_default,		FALSE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_DISTON,
		p_end,
		
	// #################### // UVW, Output \\ ####################

	/*pb_coords,			_T("coords"),		TYPE_REFTARG,	P_OWNERS_REF,	IDS_COORDS,
		p_refno,		COORD_REF, 
		p_end,*/
	pb_output,			_T("output"),		TYPE_REFTARG,	P_OWNERS_REF,	IDS_OUTPUT,
		p_refno,		OUTPUT_REF, 
		p_end,

	p_end
);

static ParamBlockDesc2 BerconCurve_param_blk ( BerconCurve_params, _T("params"),  0, &BerconGradientDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, CURVEPB_REF, 	
	IDD_PANEL_CURVE, IDS_CURVEPARM, 0, 1, NULL,

	// #################### // Curve \\ ####################

	pb_curve_on,	_T("enableCurve"), TYPE_BOOL,			0,				IDS_OUTPUT_NOISE,
		p_default,		FALSE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_ENABLE,
		p_end,	
	pb_curve,			_T("curveControl"),	TYPE_REFTARG,	P_OWNERS_REF,	IDS_CURVE,
		p_refno,		CURVE_REF, 
		p_end,

	p_end
);	    

class BerconCurveDlgProcGRADIENT : public ParamMap2UserDlgProc {
	public:
		BerconGradient *parentMap;		
		BerconCurveDlgProcGRADIENT(BerconGradient *m) {parentMap = m;}		
		INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam) {
			if (parentMap->curve->GetHWND() != GetDlgItem(hWnd, IDC_CURVE))
				CurveCtrl::update(parentMap->curve, GetDlgItem(hWnd, IDC_CURVE), static_cast<ReferenceMaker*>(parentMap)); // Force update curve
			switch (msg) {
				case WM_INITDIALOG:
				case WM_SHOWWINDOW:
					CurveCtrl::update(parentMap->curve, GetDlgItem(hWnd, IDC_CURVE), static_cast<ReferenceMaker*>(parentMap));					
					break;
				case WM_DESTROY:
					CurveCtrl::disable(parentMap->curve);
					break;
				default: return FALSE;
			}
			return TRUE;
		}
		void DeleteThis() {delete this;}
		void SetThing(ReferenceTarget *m) { 
			CurveCtrl::disable(parentMap->curve); // Disable previously used curve
			parentMap = (BerconGradient*)m;
		}
};

class BerconGradientDlgProc : public ParamMap2UserDlgProc {
	public:
		BerconGradient *parentMap;		
		IGradient* igrad;
		BerconGradientDlgProc(BerconGradient *m) {parentMap = m; igrad = NULL;}		
		INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);		
		void DeleteThis() {delete this;}
		void SetThing(ReferenceTarget *m) {
			IGradient* gradUI = parentMap->gradientUI; // Take old control
			parentMap->gradientUI = NULL; // Remove control from old map
			parentMap->gradient->setHWND(NULL); // Remove control from old maps gradient

			parentMap = (BerconGradient*)m; // Switch maps			

			parentMap->gradientUI = gradUI; // Replace new maps control
			parentMap->gradient->setHWND(parentMap->gradientUI->GetHwnd()); // Set new maps gradients HWND
			parentMap->gradientUI->setGradient(parentMap->gradient); // Set new maps controls gradient
			parentMap->gradient->invalidate(); // Redraw

			parentMap->EnableStuff();
		}
};

INT_PTR BerconGradientDlgProc::DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam) {	
	switch (msg) {	
		case WM_INITDIALOG: {			
			// Initialize gradient			
			parentMap->gradient->setHWND(GetDlgItem(hWnd,IDC_GRADIENT));
			igrad = new IGradient(GetDlgItem(hWnd,IDC_GRADIENT), parentMap->gradient);
			parentMap->gradientUI = igrad;
			
			// Dropdown values
			HWND hwndMap = GetDlgItem(hWnd, IDC_GRADIENT_TYPE);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADTYPE_UVW));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADTYPE_NORMAL));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADTYPE_DISTANCE));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADTYPE_LIGHT));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADTYPE_MAP));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADTYPE_RANDOM));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADTYPE_AGE));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADTYPE_SPEED));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADTYPE_SIZE));			

			hwndMap = GetDlgItem(hWnd, IDC_GRADIENT_INT);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADINT_LINEAR));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADINT_SMOOTH));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADINT_SOLIDNEAR));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADINT_SOLIDLEFT));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADINT_SOLIDRIGHT));

			hwndMap = GetDlgItem(hWnd, IDC_GRADIENT_UVW);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADUVW_LINEAR));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADUVW_RADIAL));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADUVW_RECTANGLE));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADUVW_DIAGONAL));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADUVW_PONG));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADUVW_SPIRAL));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADUVW_SWEEP));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADUVW_TARTAN));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADUVW_SPHERICAL));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_GRADUVW_BOX));

			hwndMap = GetDlgItem(hWnd, IDC_GRADIENT_NORMAL);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_VIEW));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_LOCALX));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_LOCALY));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_LOCALZ));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_WORLDX));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_WORLDY));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_WORLDZ));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_CAMERAX));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_CAMERAY));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_CAMERAZ));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_TOOBJECT));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_OBJECTZ));

			hwndMap = GetDlgItem(hWnd, IDC_GRADIENT_NORMAL2);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_PARALLEL));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_TOWARDS));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_NORMAL_FRESNEL));
						
			hwndMap = GetDlgItem(hWnd, IDC_RANGELOOP);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_STRETCH));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_TILE));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_MIRROR));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_NONE));

			// Set correct dropdown value
			int curIndex;
			map->GetParamBlock()->GetValue(pb_type, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_GRADIENT_TYPE), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_interpolation, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_GRADIENT_INT), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_gradient_uvw, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_GRADIENT_UVW), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_gradient_normal, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_GRADIENT_NORMAL), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_gradient_normal2, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_GRADIENT_NORMAL2), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_range_loop, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_RANGELOOP), CB_SETCURSEL, (WPARAM)curIndex, 0);
			break;
		}		
		case WM_SHOWWINDOW:	{
			// Set correct dropdown value
			int curIndex;
			map->GetParamBlock()->GetValue(pb_type, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_GRADIENT_TYPE), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_interpolation, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_GRADIENT_INT), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_gradient_uvw, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_GRADIENT_UVW), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_gradient_normal, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_GRADIENT_NORMAL), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_gradient_normal2, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_GRADIENT_NORMAL2), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_range_loop, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_RANGELOOP), CB_SETCURSEL, (WPARAM)curIndex, 0);
			break;
		}
		case WM_DESTROY: {						
			// Delete gradient
			igrad->DeleteMe();
			break;
		}
		case CC_COLOR_CHANGE: {
			switch (LOWORD(wParam)) {
				case IDC_KEYCOL:					
					parentMap->keyColorChanged(((IColorSwatch*)lParam)->GetAColor());
				default:
					break;
			}
			break;
		}
		case WM_CUSTEDIT_ENTER: {
			// Unlike the other messages here lParam is the HWND of the custom edit and not a pointer to it
			ICustEdit* custEdit=GetICustEdit(reinterpret_cast<HWND>(lParam));
			if (custEdit) {
				switch (LOWORD(wParam)) {
					case IDC_KEYNUM_EDIT:
						parentMap->keyNumChanged(custEdit->GetInt()); break;
					case IDC_KEYPOS_EDIT:
						parentMap->keyPosChanged(custEdit->GetFloat()); break;
					default: break;
				}
				ReleaseICustEdit(custEdit);
			}
			break;
		}
		case CC_SPINNER_CHANGE: {
			switch (LOWORD(wParam)) {
				case IDC_KEYNUM_SPIN:
					parentMap->keyNumChanged(((ISpinnerControl*)lParam)->GetIVal()); break;
				case IDC_KEYPOS_SPIN:
					parentMap->keyPosChanged(((ISpinnerControl*)lParam)->GetFVal()); break;
				default: break;
			}
			break;
		}
		default: return FALSE;
	}
	return TRUE;
}

BerconGradient::BerconGradient() {
	texHandle = NULL;
	previewMatIDMode = TRUE;	
	pblock = NULL;	
	pbXYZ = NULL;
	//xyzGen = NULL;
	texout = NULL;	
	pbCurve = NULL;
	curve = NULL;
	p_maptex = NULL;
	p_distex = NULL;
	p_node = NULL;
	ivalid.SetEmpty();
	gradient = new GradientRamp(this);	
	BerconGradientDesc.MakeAutoParamBlocks(this);
	berconXYZ.mode2D = TRUE;
	Reset();	
}

BerconGradient::~BerconGradient() {
	if (gradient) delete gradient;
	DiscardTexHandle();
}

void BerconGradient::Reset() {
	TimeValue t = GetCOREInterface()->GetTime();
		
	//if (xyzGen) xyzGen->Reset();
	//else ReplaceReference( COORD_REF, GetNewDefaultXYZGen());	
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
	pbCurve->SetValue( pb_curve_on, t, FALSE);	

	pblock->SetValue( pb_type,				t, 0);
	pblock->SetValue( pb_interpolation,		t, 0);
	pblock->SetValue( pb_seed,				t, 12345);
	pblock->SetValue( pb_rand_mat,			t, 1);
	pblock->SetValue( pb_rand_obj,			t, 1);
	pblock->SetValue( pb_rand_par,			t, 1);
	pblock->SetValue( pb_rand_tile,			t, 1);
	pblock->SetValue( pb_range_min,			t, 0.f);
	pblock->SetValue( pb_range_max,			t, 1.f);
	pblock->SetValue( p_rangeLoop,			t, 0);
	pblock->SetValue( p_reverse,			t, 0);
	pblock->SetValue( pb_dison,				t, 0);
	pblock->SetValue( pb_disstr,			t, 0.05f);
	pblock->SetValue( pb_gradient_uvw,		t, 0);	
	pblock->SetValue( pb_gradient_normal,	t, 0);	
	pblock->SetValue( pb_gradient_normal2,	t, 0);	
	pblock->SetValue( pb_ior,				t, 1.6f);		

	p_maptex = NULL;
	p_distex = NULL;
	p_node = NULL;

	gradient->reset();
	resetKeys();

	berconXYZ.reset(pbXYZ, ivalid, 0, 1, 1, 1);

	ivalid.SetEmpty();
}

void BerconGradient::Update(TimeValue t, Interval& valid) {	
//CharStream *out = thread_local(current_stdout);			

	if (pblock == NULL || gradient == NULL) return;	
	if (!ivalid.InInterval(t)) {
				
		// Set key information		
		if (gradient->selected >= 0 && gradient->selected < countKeys() && pblock->GetMap() != NULL) {			
			AColor currentKeyColor = pblock->GetAColor(pb_colors, t, gradient->selected);						
			pblock->SetValue(pb_keyCol, t, Color(currentKeyColor.r, currentKeyColor.g, currentKeyColor.b));	
			pblock->SetValue(pb_keyNum, t, gradient->selected);	
			pblock->SetValue(pb_keyPos, t, pblock->GetFloat(pb_positions, t, gradient->selected));				
			pblock->GetDesc()->InvalidateUI(pb_keyCol);
			pblock->GetDesc()->InvalidateUI(pb_keyNum);
			pblock->GetDesc()->InvalidateUI(pb_keyPos);			
		}

		ivalid.SetInfinite(); // Start from infinite interval
				
		if (p_maptex) p_maptex->Update(t,ivalid);	
		if (p_distex) p_distex->Update(t,ivalid);			

		//if (xyzGen != NULL)
			//xyzGen->Update(t,ivalid);
		if (texout != NULL)
			texout->Update(t,ivalid);

		pbCurve->GetValue(pb_curve_on, t, p_curveOn, ivalid);

		// Load gradient					
		int keys = countKeys();
		gradient->reset();	
		pblockGetValue(pb_interpolation,gradient->interpolation);	
		for (int i=0;i<keys;i++) {
			float pos; AColor col; Texmap* tex;
			pblock->GetValue(pb_positions, t, pos, ivalid, i);
			pblock->GetValue(pb_submaps, t, tex, ivalid, i);
			pblock->GetValue(pb_colors, t, col, ivalid, i);
			if (i == 0)
				pos = 0.f;
			else if (i == 1)
				pos = 1.f;
			gradient->addKey(i,pos,col,tex);
			if (gradient->getSubtex(i))
				gradient->getSubtex(i)->Update(t,ivalid);
		}	
		gradient->sort();
		gradient->invalidate();	

		// General stuff
		pblockGetValue(pb_seed,				p_seed);
		pblockGetValue(pb_type,				p_type);		
		pblockGetValue(pb_rand_obj,			p_randObj);
		pblockGetValue(pb_rand_mat,			p_randMat);
		pblockGetValue(pb_rand_par,			p_randPar);
		pblockGetValue(pb_rand_tile,		p_randTile);		
		pblockGetValue(pb_range_min,		p_rangeMin);
		pblockGetValue(pb_range_max,		p_rangeMax);
		pblockGetValue(pb_gradient_uvw,		p_uvwType);
		pblockGetValue(pb_gradient_normal,	p_normalType);
		pblockGetValue(pb_gradient_normal2,	p_normalFunction);
		pblockGetValue(pb_ior,				p_ior);

		pblockGetValue(pb_dison,			p_disOn);
		pblockGetValue(pb_disstr,			p_disStr);

		pblockGetValue(pb_reverse,			p_reverse);
		pblockGetValue(pb_range_loop,		p_rangeLoop);
		
		pblockGetValue(pb_node,				p_node);		

		if (p_rangeMin > p_rangeMax) // Switch so rangeMax is always bigger
			{ float f = p_rangeMin; p_rangeMax = p_rangeMin; p_rangeMin = f; }	
		
		EnableStuff();

		berconXYZ.update(pbXYZ, t, ivalid);
	}
	
	valid &= ivalid;
}

void BerconGradient::EnableStuff() {			
	if (!pblock) return;
	IParamMap2 *map = pblock->GetMap();
	if (map) {
		// Update values
		TimeValue t = GetCOREInterface()->GetTime();
		pblockGetValue(pb_type, p_type);
		pblockGetValue(pb_dison, p_disOn);
		pblockGetValue(pb_gradient_normal,	p_normalType);
		pblockGetValue(pb_gradient_normal2,	p_normalFunction);
		// Key params
		map->Enable(pb_keyTex,		gradient->selected==-1?FALSE:TRUE);
		map->Enable(pb_keyPos,		gradient->selected==-1?FALSE:TRUE);
		map->Enable(pb_keyCol,		gradient->selected==-1?FALSE:TRUE);
		// Random
		map->Enable(pb_seed,		p_type==5?TRUE:FALSE);
		map->Enable(pb_rand_obj,	p_type==5?TRUE:FALSE);
		map->Enable(pb_rand_mat,	p_type==5?TRUE:FALSE);
		map->Enable(pb_rand_par,	p_type==5?TRUE:FALSE);
		map->Enable(pb_rand_tile,	p_type==5?TRUE:FALSE);
		// Mapped
		map->Enable(pb_maptex,		p_type==4?TRUE:FALSE);		
		// Distortion
		map->Enable(pb_disstr,		p_disOn?TRUE:FALSE);
		map->Enable(pb_distex,		p_disOn?TRUE:FALSE);
		// Normal / Distance
		map->Enable(pb_gradient_normal,		p_type==1||p_type==2?TRUE:FALSE);
		map->Enable(pb_gradient_normal2,	p_type==1?TRUE:FALSE);
		map->Enable(pb_ior,					p_type==1&&p_normalFunction==2?TRUE:FALSE);
		map->Enable(pb_node,				(p_type==1||p_type==2)&&(p_normalType==10||p_normalType==11)?TRUE:FALSE);
		// UVW
		map->Enable(pb_gradient_uvw,	p_type==0?TRUE:FALSE);
	}		
}

Interval BerconGradient::Validity(TimeValue t) {
	Interval v;
	Update(t,v);
	return ivalid;
}

DWORD_PTR BerconGradient::GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker) {
	if (texHandle) {
		if (texHandleValid.InInterval(t))
			return texHandle->GetHandle();
		else DiscardTexHandle();
	}
	texHandle = thmaker.MakeHandle(GetVPDisplayDIB(t,thmaker,texHandleValid));
	return texHandle->GetHandle();
}

// #############################################################################################
// #################################/ Gradient interface       \################################
// #############################################################################################

void BerconGradient::keyColorChanged(AColor col) {
	if (!pblock) return;
	if (gradient->selected >= 0 && gradient->selected < gradient->numKeys()) {
		TimeValue t = GetCOREInterface()->GetTime();
		pblock->SetValue(pb_colors, t, col, gradient->selected);
		Update(GetCOREInterface()->GetTime(),Interval());
	}
}

void BerconGradient::keyNumChanged(int num) {	
	if (!pblock) return;
	TimeValue t = GetCOREInterface()->GetTime();	
	if (num >= gradient->numKeys())
		gradient->selected = gradient->numKeys() - 1;
	else if (num < 0)
		gradient->selected = 0;
	else
		gradient->selected = num;
	Update(GetCOREInterface()->GetTime(),Interval());
}


void BerconGradient::keyPosChanged(float pos) {
	if (!pblock) return;
	if (gradient->selected >= 0 && gradient->selected < gradient->numKeys()) {
		TimeValue t = GetCOREInterface()->GetTime();		
		pblock->SetValue(pb_positions, t, pos, gradient->selected);
		Update(GetCOREInterface()->GetTime(),Interval());
	}
}

int BerconGradient::countKeys() {
	if (!pblock) return 0;
	int smallest = pblock->Count(pb_submaps);
	if (smallest > pblock->Count(pb_colors)) smallest = pblock->Count(pb_colors);
	if (smallest > pblock->Count(pb_positions)) smallest = pblock->Count(pb_positions);	
	return smallest;
}

/*int BerconGradient::synchroniseKeys() {
	if (!pblock) return 0;
	int smallest = pblock->Count(pb_submaps);
	if (smallest > pblock->Count(pb_colors)) smallest = pblock->Count(pb_colors);
	if (smallest > pblock->Count(pb_positions)) smallest = pblock->Count(pb_positions);	
	pblock->SetCount(pb_submaps, smallest);
	pblock->SetCount(pb_colors, smallest);
	pblock->SetCount(pb_positions, smallest);	
	return smallest;
}*/

void BerconGradient::resetKeys() {	
	if (!pblock) return;

	//pblock->SetCount(pb_submaps, 2);
	//pblock->SetCount(pb_colors, 2);
	//pblock->SetCount(pb_positions, 2);
	/*
	pblock->SetValue(pb_submaps, t, NULL, 0);
	pblock->SetValue(pb_submaps, t, NULL, 1);
	pblock->SetValue(pb_colors, t, AColor(0.f,0.f,0.f,1.f), 0);
	pblock->SetValue(pb_colors, t, AColor(1.f,1.f,1.f,1.f), 1);
	pblock->SetValue(pb_positions, t, 0.f, 0);
	pblock->SetValue(pb_positions, t, 1.f, 1);
*/
	Texmap* nullMaps[2] = {NULL, NULL};
	pblock->Append(pb_submaps, 2, nullMaps);
	AColor col1 = AColor(0.f,0.f,0.f,1.f); AColor col2 = AColor(1.f,1.f,1.f,1.f);
	AColor* colors[2] = {&col1, &col2};
	pblock->Append(pb_colors, 2, colors);
	float floats[2] = {0.f, 1.f};
	pblock->Append(pb_positions, 2, floats);

	ivalid.SetEmpty();
	//Update(GetCOREInterface()->GetTime(),Interval());
}

void BerconGradient::gradMoveKey(int n, float pos) {
	TimeValue t = GetCOREInterface()->GetTime();
	if (n < countKeys())
		pblock->SetValue(pb_positions, t, pos, n);
	ivalid.SetEmpty();
	Update(GetCOREInterface()->GetTime(),Interval());
}


void BerconGradient::gradAddKey(float pos) {
	if (!pblock) return;
	countKeys();
	AColor* col = &gradient->getColor(pos);
	Texmap* sub = NULL;
	pblock->Append(pb_submaps, 1, &sub);
	pblock->Append(pb_colors, 1, &col);
	pblock->Append(pb_positions, 1, &pos);
	ivalid.SetEmpty();
	Update(GetCOREInterface()->GetTime(),Interval());
}

void BerconGradient::gradDelKey(int n) {
	if (!pblock) return;
	if (n <= 1) return; // Don't delete keys 0 and 1
	if (n < countKeys()) {
		pblock->Delete(pb_submaps, n, 1);
		pblock->Delete(pb_colors, n, 1);
		pblock->Delete(pb_positions, n, 1);
	}
	ivalid.SetEmpty();
	Update(GetCOREInterface()->GetTime(),Interval());
}

void BerconGradient::setKeyTex(Texmap* m) {		
	if (gradient->selected >= 0 && gradient->selected < gradient->numKeys()) {
		TimeValue t = GetCOREInterface()->GetTime();
		gradient->setSubtex(m);		
		if (pblock) // Max deletes pblock and starts assigining NULL materials, to prevent crash check if pblock still exists
			pblock->SetValue(pb_submaps, t, m, gradient->selected);
	}
}

Texmap* BerconGradient::getKeyTex() {	
	return gradient->getSubtex();
}

void BerconGradient::gradSelKey() {
	if (!pblock) return;
	ivalid.SetEmpty();
	Update(GetCOREInterface()->GetTime(),Interval());		
	pblock->GetDesc()->InvalidateUI(pb_keyTex);	
}

// #############################################################################################
// #################################/ General stuff            \################################
// #############################################################################################

ParamDlg* BerconGradient::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) {
	//xyzGenDlg = xyzGen->CreateParamDlg(hwMtlEdit, imp);
	IAutoMParamDlg* masterDlg = BerconGradientDesc.CreateParamDlgs(hwMtlEdit, imp, this);
	texoutDlg = texout->CreateParamDlg(hwMtlEdit, imp);
	//masterDlg->AddDlg(xyzGenDlg);	
	masterDlg->AddDlg(texoutDlg);
	gradientmap_param_blk.SetUserDlgProc(new BerconGradientDlgProc(this));
	BerconCurve_param_blk.SetUserDlgProc(new BerconCurveDlgProcGRADIENT(this));
	xyz_blk.SetUserDlgProc(new BerconXYZDlgProc(this));
	EnableStuff();
	return masterDlg;
}						

BOOL BerconGradient::SetDlgThing(ParamDlg* dlg) {	
	/*if (dlg == xyzGenDlg)
		xyzGenDlg->SetThing(xyzGen);
	else*/ if (dlg == texoutDlg)
		texoutDlg->SetThing(texout);
	else 
		return FALSE;
	return TRUE;
}

Texmap* BerconGradient::GetSubTexmap(int i) {	
	if (i == MAPTEX_NUM)
		return p_maptex;
	else if (i == DISTEX_NUM)
		return p_distex;	
	else if (i == KEYTEX_NUM)
		return getKeyTex();	
	else
		return gradient->getSubtex(i-SUBMAPCOUNT);
}

void BerconGradient::SetSubTexmap(int i, Texmap *m) {	
	if (i >= SUBMAPCOUNT+gradient->numKeys()) return;
	ReplaceReference(i+REF_OFFSET, m);		
	if (i == MAPTEX_NUM)
		gradientmap_param_blk.InvalidateUI(pb_maptex);
	else if (i == KEYTEX_NUM)
		gradientmap_param_blk.InvalidateUI(pb_keyTex);
	else if (i == DISTEX_NUM)
		gradientmap_param_blk.InvalidateUI(pb_distex);
	ivalid.SetEmpty();
}

TSTR BerconGradient::GetSubTexmapSlotName(int i) {		
	if (i == MAPTEX_NUM)
		return TSTR(GetString(IDS_MAP)); 
	if (i == DISTEX_NUM)
		return TSTR(GetString(IDS_DISTORTION_MAP)); 
	if (i == KEYTEX_NUM)
		return TSTR(GetString(IDS_KEYTEX)); 
	
	/*
	TSTR s;
	TCHAR* name = NULL;
	Interval iv;

	int j = FindSubMtl(i);
	if (j>=0) {
		pblock->GetValue(multi_names,0,name,iv,j);
	if (name) 
		s.printf("(%d) %s",i+1,name);
	else 
		s.printf("(%d)",i+1);
	}

	else 
	s.printf("(%d)",i+1);
	return s;
	}*/

	TSTR n;
//	n.printf("Key #%d", i-3);
	return n;	
}

RefTargetHandle BerconGradient::GetReference(int i) {
	switch (i) {		
		case COORD_REF: return pbXYZ;
		case PBLOCK_REF: return pblock;		
		case OUTPUT_REF: return texout;
		case CURVE_REF: return curve;		
		case CURVEPB_REF: return pbCurve;
		case MAPTEX_REF: return p_maptex;
		case DISTEX_REF: return p_distex;
		case KEYTEX_REF: return getKeyTex();		
		default:
			int k = i-REF_OFFSET-SUBMAPCOUNT;
			if (k >= gradient->numKeys()) return NULL;
			return gradient->getSubtex(k);
	}
	return NULL;
}

void BerconGradient::SetReference(int i, RefTargetHandle rtarg) {
	switch(i) {		
		case COORD_REF: pbXYZ = (IParamBlock2 *)rtarg; break;			
		case PBLOCK_REF: pblock = (IParamBlock2 *)rtarg; break;		
		case OUTPUT_REF: texout = (TextureOutput *)rtarg; break;
		case CURVE_REF: curve = (ICurveCtl *)rtarg; break;
		case CURVEPB_REF: pbCurve = (IParamBlock2 *)rtarg; break;
		case MAPTEX_REF: p_maptex = (Texmap *)rtarg; break;
		case DISTEX_REF: p_distex = (Texmap *)rtarg; break;		
		case KEYTEX_REF: setKeyTex((Texmap *)rtarg); break;		
		default:
			int k = i-REF_OFFSET-SUBMAPCOUNT;
			if (k >= gradient->numKeys()) return;
			gradient->setSubtex(k, (Texmap *)rtarg);
			pblock->SetValue(pb_submaps, 0, (Texmap *)rtarg, k);
			break;
	}
}

RefTargetHandle BerconGradient::Clone(RemapDir &remap) {	
	int keys = countKeys();

	BerconGradient *mnew = new BerconGradient();
	*((MtlBase*)mnew) = *((MtlBase*)this);	

	mnew->pblock->SetCount(pb_submaps, keys);		
	for (int i = 0; i<keys; i++) {		
		mnew->ReplaceReference(i+REF_OFFSET+SUBMAPCOUNT, remap.CloneRef(gradient->getSubtex(i)));
	}

	mnew->ReplaceReference(MAPTEX_REF, remap.CloneRef(p_maptex));
	mnew->ReplaceReference(DISTEX_REF, remap.CloneRef(p_distex));

	mnew->ivalid.SetEmpty();
	mnew->ReplaceReference(PBLOCK_REF,remap.CloneRef(pblock));
	mnew->ReplaceReference(COORD_REF,remap.CloneRef(pbXYZ));
	mnew->ReplaceReference(OUTPUT_REF,remap.CloneRef(texout));	
	mnew->ReplaceReference(CURVE_REF,remap.CloneRef(curve));
	mnew->ReplaceReference(CURVEPB_REF,remap.CloneRef(pbCurve));

	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
}

Animatable* BerconGradient::SubAnim(int i) {
	switch (i) {		
		case PBLOCK_REF: return pblock;
		case COORD_REF: return pbXYZ;		
		case OUTPUT_REF: return texout;
		case CURVE_REF: return curve;
		case CURVEPB_REF: return pbCurve;
		case MAPTEX_REF: return p_maptex;
		case DISTEX_REF: return p_distex;
		case KEYTEX_REF: return getKeyTex();		
		default:
			int k = i-REF_OFFSET-SUBMAPCOUNT;
			if (k >= gradient->numKeys()) return NULL;		
			return gradient->getSubtex(k);
	}
	return NULL;
}

TSTR BerconGradient::SubAnimName(int i) {
	switch (i) {
		case PBLOCK_REF: return GetString(IDS_PARAMS);
		case COORD_REF: return GetString(IDS_COORDS);				
		case OUTPUT_REF: return GetString(IDS_OUTPUT);	
		case CURVE_REF: return GetString(IDS_CURVE);
		case CURVEPB_REF: return GetString(IDS_CURVEPB);
		default: return GetSubTexmapTVName(i-REF_OFFSET);
	}									  
}

RefResult BerconGradient::NotifyRefChanged(NOTIFY_REF_CHANGED_ARGS) {	
	switch (message) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();			
			if (hTarget == pblock) {
				ParamID changing_param = pblock->LastNotifyParamID();
				gradientmap_param_blk.InvalidateUI(changing_param);
				if (changing_param != -1) DiscardTexHandle();
			}  else if (hTarget == pbCurve) {
				ParamID changing_param = pbCurve->LastNotifyParamID();
				BerconCurve_param_blk.InvalidateUI(changing_param);
				if (changing_param != -1) DiscardTexHandle();
			} else if (hTarget == pbXYZ) {
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

// Input: occ(0..1), val(0..1)
// Output: -1..1
static float occCurve(float occ, float val) {
	if (occ < 0.00001) return -1.f;
	if (occ > 0.99999) return 1.f;	
	
	if (val < 1.f - occ)	
		return ((occ / (1.f - occ)) * val) * 2.f - 1.f;
	else		
		return ((1.f - occ) * (val - 1.f) / occ + 1.f) * 2.f - 1.f;
}

// Random number (0..1)
#define sfrand() ((double)rand() / (double)RAND_MAX)
// Random number (-1..1)
#define ufrand() ((double)rand() / (double)RAND_MAX * 2. - 1.)

#define SQRT2 1.41421356 // sqrt(2);

//Point3 BerconGradient::getUVWPoint(ShadeContext& sc) {
//}

float BerconGradient::getGradientValueUVW(Point3 p) {
	// NOTE! Asumes the values are scaled to [0..1, 0..1, 0..1] space
	switch (p_uvwType) {
		case 0: { // 1D Linear
			return p.x;
		}
		case 1: { // 2D Radial
			p.x -= .5f; p.y -= .5f;
			return 1.f - sqrt(p.x*p.x + p.y*p.y) * 2.f;			
		}
		case 2: { // 2D Rectangle
			p.x -= .5f; p.y -= .5f;
			p.x = fabs(p.x); p.y = fabs(p.y);
			return 1.f - (p.x>p.y?p.x:p.y) * 2.f;		
		}
		case 3: { // 2D Diagonal
			return 1.f - fabs(p.x - p.y); // / SQRT2 / 2.f;					
		}
		case 4: { // 2D Pong
			p.z = 0;
			p = Normalize(p);
			float a = acos(p.y);
			if (a > pi / 4.f) a = (float)pi / 2.f - a;
			return a / (float)pi * 4.f;
		}	
		case 5: { // 2D Spiral
			p.x -= .5f; p.y -= .5f;	p.z = 0;
			p = Normalize(p);
			if (p.x > 0)
				return acos(p.y) / (float)pi / 2.f;
			else
				return 1.f - acos(p.y) / (float)pi / 2.f;
		}	
		case 6: { // 2D Sweep
			p.z = 0;
			p = Normalize(p);
			return acos(p.y) / (float)pi * 2.f;
		}	
		case 7: { // 2D Tartan
			p.x -= .5f; p.y -= .5f;
			p.x = fabs(p.x); p.y = fabs(p.y);
			return 1.f - (p.x<p.y?p.x:p.y)*2.f;		
		}
		case 8: { // 3D Spherical
			p.x -= .5f; p.y -= .5f; p.z -= .5f;
			return 1.f - sqrt(p.x*p.x + p.y*p.y + p.z*p.z) * 2.f;
		}
		case 9: { // 3D Box
			p.x -= .5f; p.y -= .5f; p.z -= .5f;
			p.x = fabs(p.x); p.y = fabs(p.y); p.z = fabs(p.z);
			float largest = p.x;
			if (largest < p.y) largest = p.y;
			if (largest < p.z) largest = p.z;
			return 1.f - largest * 2.f;													
		}
		default:
			return 0.f;
	}
	return 0.f;
}

// Seed random number generator		
void BerconGradient::seedRandomGen(ShadeContext& sc) {
	int seed = 1;
	if (previewMatIDMode) {
		seed = sc.mtlNum;
	} else {
		if (p_randMat) { 		
			seed += sc.mtlNum;
		}
		if (p_randObj) {
			int hand = 0;
			if (sc.Node())
				hand = (int)sc.Node()->GetHandle();

			seed += hand*(hand*hand*15731 + 789221);
		}
		if (p_randPar) {
			Object *ob = sc.GetEvalObject();		
			if (ob && ob->IsParticleSystem()) {
				ParticleObject *obj = (ParticleObject*)ob;
				IChkMtlAPI* chkMtlAPI = static_cast<IChkMtlAPI*>(obj->GetInterface(I_NEWMTLINTERFACE));
				if ((chkMtlAPI && chkMtlAPI->SupportsParticleIDbyFace())) {
					int id = chkMtlAPI->GetParticleFromFace(sc.FaceNumber());
					seed += id*(id*id*571 + 789221);
				}			
			}
		}
		if (p_randTile) {
			seed += (int)(sc.UVW(99).z);
		}
	}

	seed *= p_seed;
	srand(seed*(seed*seed*15731 + 789221));	
}

float BerconGradient::getGradientValueDist(ShadeContext& sc) {
	switch (p_normalType) {	 
		case 0: { // View			 
			return -sc.P().z; //Length(sc.OrigView()); //(sc.PointTo(sc.P(), REF_CAMERA)).z;
		}
		case 1: { // Local X
			return (sc.PointTo(sc.P(), REF_OBJECT)).x;
		}
		case 2: { // Local Y
			return (sc.PointTo(sc.P(), REF_OBJECT)).y;
		}
		case 3: { // Local Z
			return (sc.PointTo(sc.P(), REF_OBJECT)).z;
		}
		case 4: { // World X
			return (sc.PointTo(sc.P(), REF_WORLD)).x;
		}
		case 5: { // World Y
			return (sc.PointTo(sc.P(), REF_WORLD)).y;
		}
		case 6: { // World Z
			return (sc.PointTo(sc.P(), REF_WORLD)).z;
		}
		case 7: { // Camera X
			return sc.P().x; //(sc.PointTo(sc.P(), REF_CAMERA)).x;
		}
		case 8: { // Camera Y
			return sc.P().y; //(sc.PointTo(sc.P(), REF_CAMERA)).y;
		}
		case 9: { // Camera Z
			return -sc.P().z; //-(sc.PointTo(sc.P(), REF_CAMERA)).z;
		}
		case 10: { // To Object
			if (sc.InMtlEditor() || !p_node)
				return -sc.P().z; //(sc.PointTo(sc.P(), REF_CAMERA)).z;			
			return Length((p_node->GetNodeTM(sc.CurTime())).GetTrans() - sc.PointTo(sc.P(), REF_WORLD));
		}
		case 11: { // Object Z			
			if (sc.InMtlEditor() || !p_node)
				return -sc.P().z; //(sc.PointTo(sc.P(), REF_CAMERA)).z;			
			Matrix3 tm = p_node->GetNodeTM(sc.CurTime());
			Point3 a = tm.GetTrans() - sc.PointTo(sc.P(), REF_WORLD);
			Point3 b = FNormalize(tm.GetRow(2));
			return (-DotProd(b, a) / Length(b));
		}
	}
	return 0.f;
}

float BerconGradient::getGradientValueNormal(ShadeContext& sc) {
	switch (p_normalType) {	 
		case 0: { // View			 
			return -DotProd(sc.Normal(), sc.V());
		}
		case 1: { // Local X
			return (sc.VectorTo(sc.Normal(), REF_OBJECT)).x;
		}
		case 2: { // Local Y
			return (sc.VectorTo(sc.Normal(), REF_OBJECT)).y;
		}
		case 3: { // Local Z
			return (sc.VectorTo(sc.Normal(), REF_OBJECT)).z;
		}
		case 4: { // World X
			return (sc.VectorTo(sc.Normal(), REF_WORLD)).x;
		}
		case 5: { // World Y
			return (sc.VectorTo(sc.Normal(), REF_WORLD)).y;
		}
		case 6: { // World Z
			return (sc.VectorTo(sc.Normal(), REF_WORLD)).z;
		}
		case 7: { // Camera X
			return sc.Normal().x; //(sc.VectorTo(sc.Normal(), REF_CAMERA)).x;
		}
		case 8: { // Camera Y
			return sc.Normal().y; //(sc.VectorTo(sc.Normal(), REF_CAMERA)).y;
		}
		case 9: { // Camera Z
			return sc.Normal().z; //(sc.VectorTo(sc.Normal(), REF_CAMERA)).z;
		}
		case 10: { // To Object
			if (sc.InMtlEditor() || !p_node)
				return -DotProd(sc.Normal(), sc.V());												
			return DotProd(sc.Normal(), FNormalize(sc.PointFrom((p_node->GetNodeTM(sc.CurTime())).GetTrans(),REF_WORLD) - sc.P()));							
		}
		case 11: { // Object Z			
			if (sc.InMtlEditor() || !p_node)
				return -DotProd(sc.Normal(), sc.V());				
			return DotProd(sc.Normal(), FNormalize(sc.VectorFrom(p_node->GetNodeTM(sc.CurTime()).GetRow(2),REF_WORLD)));			
		}
	}
	return 0.f;
}

// Calculates 0..1 value which is given to the gradient
float BerconGradient::getGradientValue(ShadeContext& sc) {
	switch (p_type) {
		case 0: { // UVW
			break; // Handled in main evaluation
		}
		case 1: { // Normal
			switch (p_normalFunction) {
				case 0: { // Perpendicular / Parallel
					return fabs(getGradientValueNormal(sc));
				}
				case 1: { // Towards / Away
					return (getGradientValueNormal(sc) + 1.f) / 2.f;	
				}
				case 2: { // Fresnel
					// NOTE: Should this get IOR from sc.GetIOR()?
					//		 I think not since its just a map, not material.
					//		 You get more predictable behaviour with constant 1.f.
					static float n1 = 1.0f;								
					float cti = fabs(getGradientValueNormal(sc));					
					float stt = (n1 / p_ior) * sqrt(1 - cti * cti);
					float ctt = sqrt(1 - stt * stt);
					float rs = (p_ior * ctt - n1 * cti ) / (p_ior * ctt + n1 * cti);
					rs = rs * rs;
					float rp = (n1 * ctt - p_ior * cti ) / (n1 * ctt + p_ior * cti);
					rp = rp * rp;
					return 1.f - 0.5f * (rs + rp);				
				}
			}
		}
		case 2: { // Distance
			return getGradientValueDist(sc);		
		}
		case 3: { // Light
			return Intens(sc.DiffuseIllum());
		}
		case 4: { // Map			
			return p_maptex?p_maptex->EvalMono(sc):0.f; // TODO: Evaluate submaps color, bump is tougher DELTA shift with BerconSC?
		}
		case 5: { // Random
			seedRandomGen(sc);
			return (float)sfrand();		
			break;
		}
		case 6: { // Particle age
			Object *ob = sc.GetEvalObject();		
			if (ob && ob->IsParticleSystem()) {				
				ParticleObject *obj = (ParticleObject*)ob;
				TimeValue t = sc.CurTime();
				TimeValue age  = obj->ParticleAge(t,sc.mtlNum);
				TimeValue life = obj->ParticleLife(t,sc.mtlNum);
				if (age>=0 && life>=0) 
					return float(age)/float(life);
			}
			break;
		}
		case 7: { // Particle speed
			Object *ob = sc.GetEvalObject();		
			if (ob && ob->IsParticleSystem()) {
				ParticleObject *obj = (ParticleObject*)ob; 
				/*IChkMtlAPI* chkMtlAPI = static_cast<IChkMtlAPI*>(obj->GetInterface(I_NEWMTLINTERFACE));
				if ((chkMtlAPI&&chkMtlAPI->SupportsParticleIDbyFace()))
					return (Length(obj->ParticleVelocity(sc.CurTime(),chkMtlAPI->GetParticleFromFace(sc.FaceNumber()))) - p_rangeMin) / (p_rangeMax - p_rangeMin);
				else*/
					return Length(obj->ParticleVelocity(sc.CurTime(),sc.mtlNum));
			}									
			break;
		}
		case 8: { // Particle size
			Object *ob = sc.GetEvalObject();		
			if (ob && ob->IsParticleSystem()) {
				ParticleObject *obj = (ParticleObject*)ob;
				return obj->ParticleSize(sc.CurTime(),sc.mtlNum);
			}									
			break;
		}
		default:
			break;
	}
	return 0.f;
}

int BerconGradient::limitRange(float& d) {
	d = (d - p_rangeMin) / (p_rangeMax - p_rangeMin);
	switch (p_rangeLoop) {
		case 0: return TRUE; // Gradient handles the stretching by itself
		case 1: {D_LOOP(d) return TRUE;}
		case 2: {D_MIRR(d) return TRUE;}
		case 3: {if (d<0||d>1) return FALSE;}		
	}
	return FALSE;
}

// #################### // Color \\ ####################
AColor BerconGradient::EvalColor(ShadeContext& sc) {
	// Initialize returned color
	AColor res(0.0f,0.0f,0.0f,0.0f);
	if (!sc.doMaps) return res;

	// Use cache
	if (sc.GetCache(this,res)) 
		return res; 	
	if (gbufID) sc.SetGBufferID(gbufID);		
	
	// Function type
	float d;
	if (p_type == 0) {// UVW
		Point3 p;
		if (!berconXYZ.get(sc, p)) return res;
		d = getGradientValueUVW(p);
	} else { // Others
		d = getGradientValue(sc);
	}		 

	// Distortion
	if (p_disOn && p_distex) d += (1.f - p_distex->EvalMono(sc) * 2.f) * p_disStr;

	// Limit range
	if (!limitRange(d)) return res;

	// Curve
	if (p_curveOn) d = curve->GetControlCurve(0)->GetValue(sc.CurTime(), d);		

	// Get color from gradient
	res = gradient->getColor(p_reverse?1.f-d:d, sc);

	// Output
	res = texout->Filter(res);

	// Shading ready, return results
	sc.PutCache(this,res);
	return res;
}

// #################### // Float \\ ####################

float BerconGradient::EvalMono(ShadeContext& sc) {
	return Intens(EvalColor(sc));
}

// #################### // Vector \\ ####################
#define DELTA 0.001f
Point3 BerconGradient::EvalNormalPerturb(ShadeContext& sc) {
	// Returned vector
	Point3 res(0.0f,0.0f,0.0f);
	if (p_type != 0) return res; // Bump only works for UVW, otherwise we don't really know the derivative of the gradient
	
	// Use cache
	if (sc.GetCache(this,res)) 
		return res;
	if (gbufID) sc.SetGBufferID(gbufID);

	// UVW
	Point3 p;
	Point3 M[3];

	if (!berconXYZ.get(sc, p, M)) return res;
	
	// Distortion
	float dist = 0.f;
	if (p_disOn && p_distex) dist = (1.f - p_distex->EvalMono(sc) * 2.f) * p_disStr;
	
	// Origin
	float d = getGradientValueUVW(p) + dist;	
	if (!limitRange(d)) return res;
	if (p_curveOn) d = curve->GetControlCurve(0)->GetValue(sc.CurTime(), d);		
	d = Intens(gradient->getColor(d, sc));

	// Deltas
	Point3 normal;

	/*if (berconXYZ.req()) {		
		Point3 MP[3];
		MP[0] = Point3(DELTA,0.f,0.f); MP[1] = Point3(0.f,DELTA,0.f); MP[2] = Point3(0.f,DELTA,0.f);
		for (int i=0; i<3; i++) {
			normal[i] = getGradientValueUVW(p+DELTA*M[i]) + dist;
			if (!limitRange(normal[i])) return res;
			if (p_curveOn) 
				normal[i] = curve->GetControlCurve(0)->GetValue(sc.CurTime(), normal[i]);
			normal[i] = (normal[i] - d) / DELTA;
		}
		normal = M[0]*normal.x + M[1]*normal.y + M[2]*normal.z;
	} else {*/
		Point3 MP[3];
		MP[0] = Point3(DELTA,0.f,0.f); MP[1] = Point3(0.f,DELTA,0.f); MP[2] = Point3(0.f,DELTA,0.f);
		for (int i=0; i<3; i++) {
			normal[i] = getGradientValueUVW(p+MP[i]) + dist;
			if (!limitRange(normal[i])) return res;
			if (p_curveOn) 
				normal[i] = curve->GetControlCurve(0)->GetValue(sc.CurTime(), normal[i]);
			normal[i] = Intens(gradient->getColor(normal[i], sc));
			normal[i] = (normal[i] - d) / DELTA;
		}		
		normal = M[0]*normal.x + M[1]*normal.y + M[2]*normal.z;
		
		//normal = sc.VectorFromNoScale(normal, REF_OBJECT);		
		
	//}

	// Compute maps and proper bump vector
	res = gradient->getBump(p_reverse?1.f-d:d, p_reverse?normal:-normal, sc);

	// Output
	res = texout->Filter(res);

	// Shading ready, return results	
	sc.PutCache(this,res);
	return res;
}
