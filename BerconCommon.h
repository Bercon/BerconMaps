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

#include <max.h>
#include <3dsmaxdlport.h>
#include <maxscript/maxscript.h>
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <stdmat.h>
#include <imtl.h>
#include <macrorec.h>
#include <texutil.h>
#include <plugapi.h>
#include <bitmap.h>
#include "fractal.h"
#include "tchar.h"
#include "resource.h"


// #include "BerconRefMaker.h"	// This doesn't appear to do anything


extern TCHAR* GetString(int id);
  
#define NOTIFY_REF_CHANGED_ARGS const Interval &changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate


// Class ID's
/* Old class ID's, prior to BerconMaps 3.0
#define BerconGradient_CLASS_ID		Class_ID(0x1cf3071d, 0x6d41c735)
#define BerconWood_CLASS_ID			Class_ID(0x4bb4094d, 0x5f33ebf0)
#define BerconTile_CLASS_ID			Class_ID(0x5c0d714a, 0x6679c3f0)
#define BerconNoise_CLASS_ID		Class_ID(0x5ab8fef5, 0x9aaa1ec4)
#define BerconDistortion_CLASS_ID	Class_ID(0x6017b625, 0x2b7c52d6)
*/

#define BerconNoise_CLASS_ID			Class_ID(0x2a5e5975, 0x7bb5823e)


// Useful macros
#define pblockGetValue(from, to) (pblock->GetValue(from, t, to, ivalid))
#define pblockSetValue(from, to) (pblock->SetValue(from, t, to))

// Random number (0..1)
#define sfrand() ((double)rand() / (double)RAND_MAX)
// Random number (-1..1)
#define ufrand() ((double)rand() / (double)RAND_MAX * 2. - 1.)
#define URANDF() ((float)rand() / (float)RAND_MAX * 2.f - 1.f)

// Some macros for U/V/W/Range looping/mirroring etc.
#define D_LOOP(x) x = (x) - (float)((int)(x)); if ((x)<0) ((x)) = 1.f + (x);
#define D_MIRR(x) if ((x)<0) (x) = -(x); int ix = (int)(x); if (ix%2==0) (x) = (x) - ix; else (x) = 1.f - (x) + ix;
#define D_STRE(x) if ((x)<0) (x) = 0.f; else if ((x)>1) (x) = 1.f;

#pragma warning (push)
#pragma warning(disable: 4505)

static auto setSpinnerType(IParamMap2* map, TimeValue t, int pb_id, int edit_id, int spin_id, int spinnerTypeWorld = 1,
                           bool allowNegative = false) -> void
{
	HWND hWnd = map->GetHWnd();
	if (!hWnd) return;

	float val;
	map->GetParamBlock()->GetValue(pb_id, t, val, Interval(int(0x80000000), int(0x7fffffff)));

	float minVal = allowNegative ? -1000000.f : 0.f;

	ISpinnerControl* spin;
	if (spinnerTypeWorld)
		spin = SetupUniverseSpinner(hWnd, spin_id, edit_id, minVal, 1000000.f, val);
	else
		spin = SetupFloatSpinner(hWnd, spin_id, edit_id, minVal, 1000000.f, val);
	spin->SetAutoScale(TRUE);
	ReleaseISpinner(spin);
}
#pragma warning (pop)

enum {
	xyz_map, xyz_chan,

	xyz_offset_x, xyz_offset_y, xyz_offset_z,
	xyz_size_x, xyz_size_y, xyz_size_z,
	xyz_angle_x, xyz_angle_y, xyz_angle_z,

	xyz_tile_x, xyz_tile_y, xyz_tile_z,

	xyz_offset_x2, xyz_offset_y2, xyz_offset_z2,
	xyz_size_x2, xyz_size_y2, xyz_size_z2,
	xyz_angle_x2, xyz_angle_y2, xyz_angle_z2,

	xyz_seed, xyz_rand_mat, xyz_rand_obj, xyz_rand_par,

	xyz_lock,
	xyz_filtering,
};

//25 MAR 2018 -- REPLACED STRING LITERALS WITH RESOURCE IDs AS REQUIRED BY MAX 2019. REPLACED _T WITH _M.
class XYZ_Desc : public ParamBlockDesc2 {

public:

	XYZ_Desc(ClassDesc2* pointer, int ref, int blkID, int type, int x, int y, int z, int closed=1) : ParamBlockDesc2(blkID, _M("Mapping"),  0, pointer, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, ref, 	
	IDD_XYZ, IDS_XYZ, 0, closed, NULL,

	xyz_map, _M("xyz_map"), TYPE_INT, P_ANIMATABLE, IDS_XYZ, p_default, type, p_ui, TYPE_INTLISTBOX, IDC_TYPE, 0, p_end,

	xyz_chan,	_M("xyz_chan"), TYPE_INT, P_ANIMATABLE, IDS_SEED, 
		p_default, 1, p_range, 0, 100,
		p_ui, TYPE_SPINNER,	EDITTYPE_INT, IDC_CHAN, IDC_CHAN_SPIN, SPIN_AUTOSCALE, 
		p_end,

	xyz_offset_x, _M("xyz_offset_x"), TYPE_FLOAT, P_ANIMATABLE, IDC_OFF_X,
		p_default, 0.f, p_range, -1000000.0f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_OFF_X, IDC_OFF_X_SPIN, SPIN_AUTOSCALE,
		p_end,
	xyz_offset_y, _M("xyz_offset_y"), TYPE_FLOAT, P_ANIMATABLE, IDC_OFF_Y,
		p_default, 0.f, p_range, -1000000.0f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_OFF_Y, IDC_OFF_Y_SPIN, SPIN_AUTOSCALE,
		p_end,
	xyz_offset_z, _M("xyz_offset_z"), TYPE_FLOAT, P_ANIMATABLE, IDC_OFF_Z,
		p_default, 0.f, p_range, -1000000.0f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_OFF_Z, IDC_OFF_Z_SPIN, SPIN_AUTOSCALE,
		p_end,

	xyz_size_x, _M("xyz_size_x"), TYPE_FLOAT, P_ANIMATABLE, IDC_SIZ_X,
		p_default, 1.f, p_range, 0.0001f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SIZ_X, IDC_SIZ_X_SPIN, SPIN_AUTOSCALE,
		p_end,
	xyz_size_y, _M("xyz_size_y"), TYPE_FLOAT, P_ANIMATABLE, IDC_SIZ_Y,
		p_default, 1.f, p_range, 0.0001f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SIZ_Y, IDC_SIZ_Y_SPIN, SPIN_AUTOSCALE,
		p_end,
	xyz_size_z, _M("xyz_size_z"), TYPE_FLOAT, P_ANIMATABLE,IDC_SIZ_Z,
		p_default, 1.f, p_range, 0.0001f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SIZ_Z, IDC_SIZ_Z_SPIN, SPIN_AUTOSCALE,
		p_end,

	xyz_angle_x, _M("xyz_angle_x"), TYPE_FLOAT, P_ANIMATABLE, IDC_ANG_X,
		p_default, 0.f, p_range, -1000000.0f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_ANG_X, IDC_ANG_X_SPIN, SPIN_AUTOSCALE,
		p_end,
	xyz_angle_y, _M("xyz_angle_y"), TYPE_FLOAT, P_ANIMATABLE, IDC_ANG_Y,
		p_default, 0.f, p_range, -1000000.0f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_ANG_Y, IDC_ANG_Y_SPIN, SPIN_AUTOSCALE,
		p_end,
	xyz_angle_z, _M("xyz_angle_z"), TYPE_FLOAT, P_ANIMATABLE, IDC_ANG_Z,
		p_default, 0.f, p_range, -1000000.0f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_ANG_Z, IDC_ANG_Z_SPIN, SPIN_AUTOSCALE,
		p_end,

	xyz_tile_x, _M("xyz_tile_x"), TYPE_INT, P_ANIMATABLE, IDC_TIL_X, p_default, x, p_ui, TYPE_INTLISTBOX, IDC_TIL_X, 0, p_end,
	xyz_tile_y, _M("xyz_tile_y"), TYPE_INT, P_ANIMATABLE, IDC_TIL_Y, p_default, y, p_ui, TYPE_INTLISTBOX, IDC_TIL_Y, 0, p_end,
	xyz_tile_z, _M("xyz_tile_z"), TYPE_INT, P_ANIMATABLE, IDC_TIL_Z, p_default, z, p_ui, TYPE_INTLISTBOX, IDC_TIL_Z, 0, p_end,

	// #################### // Random \\ ####################

	xyz_seed,		_M("seed"), 		TYPE_INT, 		P_ANIMATABLE, 	IDS_SEED, 
		p_default, 12345, p_range, 1, 99999,
		p_ui, 			TYPE_SPINNER,	EDITTYPE_INT,	IDC_SEED_EDIT,	IDC_SEED_SPIN, SPIN_AUTOSCALE, 
		p_end,

	xyz_rand_mat,	_M("randByMat"),	TYPE_BOOL,		P_ANIMATABLE, IDS_RANDBYMAT,
		p_default,		FALSE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_RAND_MAT,
		p_end,

	xyz_rand_obj,	_M("randByObj"),	TYPE_BOOL,		P_ANIMATABLE, IDS_RANDBYOBJ,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_RAND_OBJ,
		p_end,

	xyz_rand_par,	_M("randByPar"),	TYPE_BOOL,		P_ANIMATABLE, IDS_RANDBYPAR,
		p_default,		FALSE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_RAND_PAR,
		p_end,	

	xyz_offset_x2, _M("xyz_offset_x2"), TYPE_FLOAT, P_ANIMATABLE, IDC_OFF_X2,
		p_default, 0.f, p_range, 0.0f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_OFF_X2, IDC_OFF_X_SPIN2, SPIN_AUTOSCALE,
		p_end,
	xyz_offset_y2, _M("xyz_offset_y2"), TYPE_FLOAT, P_ANIMATABLE, IDC_OFF_Y2,
		p_default, 0.f, p_range, 0.0f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_OFF_Y2, IDC_OFF_Y_SPIN2, SPIN_AUTOSCALE,
		p_end,
	xyz_offset_z2, _M("xyz_offset_z2"), TYPE_FLOAT, P_ANIMATABLE, IDC_OFF_Z2,
		p_default, 0.f, p_range, 0.0f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_OFF_Z2, IDC_OFF_Z_SPIN2, SPIN_AUTOSCALE,
		p_end,

	xyz_size_x2, _M("xyz_size_x2"), TYPE_FLOAT, P_ANIMATABLE, IDC_SIZ_X2,
		p_default, 0.f, p_range, 0.f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SIZ_X2, IDC_SIZ_X_SPIN2, SPIN_AUTOSCALE,
		p_end,
	xyz_size_y2, _M("xyz_size_y2"), TYPE_FLOAT, P_ANIMATABLE, IDC_SIZ_Y2,
		p_default, 0.f, p_range, 0.f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SIZ_Y2, IDC_SIZ_Y_SPIN2, SPIN_AUTOSCALE,
		p_end,
	xyz_size_z2, _M("xyz_size_z2"), TYPE_FLOAT, P_ANIMATABLE, IDC_SIZ_Z2,
		p_default, 0.f, p_range, 0.f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SIZ_Z2, IDC_SIZ_Z_SPIN2, SPIN_AUTOSCALE,
		p_end,

	xyz_angle_x2, _M("xyz_angle_x2"), TYPE_FLOAT, P_ANIMATABLE, IDC_ANG_X2,
		p_default, 0.f, p_range, 0.0f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_ANG_X2, IDC_ANG_X_SPIN2, SPIN_AUTOSCALE,
		p_end,
	xyz_angle_y2, _M("xyz_angle_y2"), TYPE_FLOAT, P_ANIMATABLE, IDC_ANG_Y2,
		p_default, 0.f, p_range, 0.0f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_ANG_Y2, IDC_ANG_Y_SPIN2, SPIN_AUTOSCALE,
		p_end,
	xyz_angle_z2, _M("xyz_angle_z2"), TYPE_FLOAT, P_ANIMATABLE, IDC_ANG_Z2,
		p_default, 0.f, p_range, 0.0f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_ANG_Z2, IDC_ANG_Z_SPIN2, SPIN_AUTOSCALE,
		p_end,

	xyz_lock,	_M("xyz_lock"), TYPE_BOOL,	P_ANIMATABLE,IDC_LOCK,
		p_default,	TRUE,
		p_ui,		TYPE_CHECKBUTTON, IDC_LOCK,
		p_end,

	xyz_filtering, _M("xyz_filtering"), TYPE_FLOAT, P_ANIMATABLE, IDC_FILTER,
		p_default, 0.f, p_range, 0.0f, 1000000.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_FILTER, IDC_FILTER_SPIN, SPIN_AUTOSCALE,
		p_end,	

	p_end) {}
};


class BerconXYZDlgProc : public ParamMap2UserDlgProc {
	public:
		ReferenceTarget *reftarg;		
		BerconXYZDlgProc(ReferenceTarget *m) {reftarg = m;}		
		INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM /*wParam*/,LPARAM /*lParam*/) override
		{
			HWND hwndMap;
			ICustButton* custButton;
			switch (msg) {
				case WM_INITDIALOG:
					custButton = GetICustButton(GetDlgItem(hWnd, IDC_LOCK));
					custButton->SetText(_M("L"));
					ReleaseICustButton(custButton);

					// UVW
					hwndMap = GetDlgItem(hWnd, IDC_TYPE);  
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_XYZ_UVW));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_XYZ_UVW2));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_XYZ_OBJ));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_XYZ_WOR));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_XYZ_SCR));

					hwndMap = GetDlgItem(hWnd, IDC_TIL_X);  
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_CONT));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_STRETCH));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_TILE));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_MIRROR));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_NONE));

					hwndMap = GetDlgItem(hWnd, IDC_TIL_Y);  
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_CONT));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_STRETCH));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_TILE));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_MIRROR));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_NONE));

					hwndMap = GetDlgItem(hWnd, IDC_TIL_Z);
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_CONT));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_STRETCH));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_TILE));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_MIRROR));
					SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_LOOP_NONE));

					// NO BREAK HERE, WE WANT TO UPDATE DROPDOWN VALUES TOO
				case WM_SHOWWINDOW: 
					// Set correct dropdown value
					int curIndex;
					map->GetParamBlock()->GetValue(xyz_map, t, curIndex, Interval(TimeValue(0x80000000), TimeValue(0x7fffffff)));
					SendMessage(GetDlgItem(hWnd, IDC_TYPE), CB_SETCURSEL, WPARAM(curIndex), 0);
					map->GetParamBlock()->GetValue(xyz_tile_x, t, curIndex, Interval(TimeValue(0x80000000), TimeValue(0x7fffffff)));
					SendMessage(GetDlgItem(hWnd, IDC_TIL_X), CB_SETCURSEL, WPARAM(curIndex), 0);
					map->GetParamBlock()->GetValue(xyz_tile_y, t, curIndex, Interval(TimeValue(0x80000000), TimeValue(0x7fffffff)));
					SendMessage(GetDlgItem(hWnd, IDC_TIL_Y), CB_SETCURSEL, WPARAM(curIndex), 0);
					map->GetParamBlock()->GetValue(xyz_tile_z, t, curIndex, Interval(TimeValue(0x80000000), TimeValue(0x7fffffff)));
					SendMessage(GetDlgItem(hWnd, IDC_TIL_Z), CB_SETCURSEL, WPARAM(curIndex), 0);
					break;
				case WM_DESTROY:			
				default: return FALSE;
			}
			return TRUE;
		}
		void DeleteThis() override {delete this;}
		void SetThing(ReferenceTarget *m) override { reftarg = m; }
};

class BerconXYZ {
private:		
	int mappingType{}, mappingChannel{};
	float offX, offY, offZ;
	float sizeX{}, sizeY{}, sizeZ{};
	float angX{}, angY{}, angZ{};
	int tileX{}, tileY{}, tileZ{};
	float offX2{}, offY2{}, offZ2{};
	float sizeX2{}, sizeY2{}, sizeZ2{};
	float angX2{}, angY2{}, angZ2{};
	int p_seed{}, p_randObj{}, p_randMat{}, p_randPar{};
	float filtering{};

	BOOL lock{};

	bool variance{};

	Point3 b[3];

public:

	BerconXYZ() { offX = 0; offY = 0; offZ = 0; mode2D = FALSE; update(); }
	~BerconXYZ() {}


	Matrix3 tm;
	Matrix3 invNoScaleTm;
	int mode2D;

	ULONG req() { if (mappingType==0 || mappingType==1) return MTLREQ_UV; return 0; };
	void map(int /*subMtlNum*/, BitArray& mapreq, BitArray& /*bumpreq*/) { if (mappingType==0 || mappingType==1) mapreq.Set(mappingChannel); }	

	void update(IParamBlock2* pblock, TimeValue t, Interval& ivalid);
	void update();	
	void reset(IParamBlock2* pblock, Interval& ivalid, int type, int x, int y, int z);	

	int isRealworld() { return mappingType == 0 || mappingType == 4 ? 0 : 1; }
	int type() { return mappingType; }
	int chan() { return mappingChannel; }

	int get(ShadeContext& sc, Point3& p, Point3& dpdx, Point3& dpdy);
	int get(ShadeContext& sc, Point3& p, Point3& dpdx, Point3& dpdy, Point3* basis);
	int get(ShadeContext& sc, Point3& dpdy);
	int get(ShadeContext& sc, Point3& p, Point3* basis);
private:	
	int get(ShadeContext& sc, Point3& p, Point3& dpdx, Point3& dpdy, Matrix3 transform, int* flips=0);
	int get(ShadeContext& sc, Point3& p, Matrix3 transform, int* flips=0);
	void seedRandomGen(ShadeContext& sc);
	Matrix3 random(ShadeContext& sc, Matrix3* inv = 0);
	void EnableStuff(IParamBlock2* pblock, TimeValue t);
	void getBasis(Matrix3 transform, Point3* b);

};

class bercon_xyz : public BerconXYZ
{
public:
};

