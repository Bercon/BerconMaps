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

#include "BerconTile.h"

#define NCOLS		3  // Number of colors

#define COORD_REF	0
#define PBLOCK_REF	1
#define MAP1_REF	2
#define MAP2_REF	3
#define MAP3_REF	4
#define MAP4_REF	5
#define MAP5_REF	6

#define BMAP1_REF	7
#define BMAP2_REF	8

#define BMAP1_NUM	5
#define BMAP2_NUM	6

#define OUTPUT_REF	9
#define PBMAP_REF	10

#define NAMELENGTH 64
typedef TCHAR TChBuffer[NAMELENGTH];

static BerconTileClassDesc BerconTileDesc;
ClassDesc2* GetBerconTileDesc() { return &BerconTileDesc; }

enum { xyz_params, BerconTile_params, BerconMap_params };

static XYZ_Desc xyz_blk(&BerconTileDesc, COORD_REF, xyz_params, 1, 0, 0, 0);

enum { 	
	tile_style,
	tile_size,
	tile_width, tile_height,
	edge_width, edge_height,

	tile_width2, tile_height2,
	edge_width2, edge_height2,

	soften, soften_rad,
	pb_round, round_rad,

	pb_map_uv, pb_uv_channel,
	pb_randX, pb_randY,
	pb_randSX, pb_randSY,
	pb_rotUV,
	pb_randRot,
	pb_flipX, pb_flipY,
	pb_randZ,
	pb_center, pb_center_channel,
	pb_lock,

	// Distortion
	distortion_map, distortion_map2,
	use_distortion, distortion_str,
	// Colors controls
	lock_edge,
	noise_color1, noise_color2, noise_color3,
	noise_map1, noise_map2,	noise_map3,	
	noise_map1_on, noise_map2_on, noise_map3_on,
	// UVW, Output
	pb_output,
	pb_pattern,
	pb_auto,
};

ParamDlg* BerconTile::texoutDlg;

static ParamBlockDesc2 BerconTile_param_blk ( BerconTile_params, _T("params"),  0, &BerconTileDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 	
	IDD_PANEL_TILE, IDS_PARAMS, 0, 0, NULL,
	// params

	// Standard noise controls
	noise_color1,	 _T("color1"),	TYPE_RGBA,				P_ANIMATABLE,	IDS_DS_COLOR1,	
		p_default,		Color(1.f, 1.f, 1.f), 
		p_ui,			TYPE_COLORSWATCH, IDC_NOISE_COL1, 
		p_end,
	noise_color2,	 _T("color2"),	TYPE_RGBA,				P_ANIMATABLE,	IDS_DS_COLOR2,	
		p_default,		Color(0.f , 0.f, 0.f), 
		p_ui,			TYPE_COLORSWATCH, IDC_NOISE_COL2, 
		p_end,			 
	noise_color3,	 _T("color3"),	TYPE_RGBA,				P_ANIMATABLE,	IDS_DS_COLOR3,	
		p_default,		Color(.5f, .5f, .5f), 
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

	lock_edge,	_T("lockEdge"), TYPE_BOOL,			0,				IDS_TILE_LOCK,
		p_default,		TRUE,
		p_ui,			TYPE_CHECKBUTTON, IDC_LOCKEDGE,
		p_end,

	tile_style,	_T("style"),		TYPE_INT,		0,				IDS_TILE_STYLE,		
		p_default,		0,		
		p_ui, 			TYPE_INTLISTBOX, IDC_TYPE, 0, 
		p_end,
	tile_size,	_T("size"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TILE_SIZE,
		p_default,		25.f,
		p_range,		0.0, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_TILESIZE_EDIT, IDC_TILESIZE_SPIN, SPIN_AUTOSCALE, 
		p_end,
	tile_width,	_T("tileWidth"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TILE_WIDTH,
		p_default,		4.f,
		p_range,		0.0, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_TILEWIDTH_EDIT, IDC_TILEWIDTH_SPIN, SPIN_AUTOSCALE, 
		p_end,
	tile_height,	_T("tileheight"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TILE_HEIGHT,
		p_default,		2.f,
		p_range,		0.0, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_TILEHEIGHT_EDIT, IDC_TILEHEIGHT_SPIN, SPIN_AUTOSCALE, 
		p_end,
	edge_width,	_T("edgeWidth"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TILE_EWIDTH,
		p_default,		.1f,
		p_range,		0.0, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_EDGEWIDTH_EDIT, IDC_EDGEWIDTH_SPIN, SPIN_AUTOSCALE, 
		p_end,
	edge_height,	_T("edgeheight"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TILE_EHEIGHT,
		p_default,		.1f,
		p_range,		0.0, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_EDGEHEIGHT_EDIT, IDC_EDGEHEIGHT_SPIN, SPIN_AUTOSCALE, 
		p_end,


	tile_width2,	_T("tileWidth2"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TILE_WIDTH2,
		p_default,		0.f,
		p_range,		0.0, 100.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_TILEWIDTH_EDIT2, IDC_TILEWIDTH_SPIN2, SPIN_AUTOSCALE, 
		p_end,
	tile_height2,	_T("tileheight2"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TILE_HEIGHT2,
		p_default,		0.f,
		p_range,		0.0, 100.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_TILEHEIGHT_EDIT2, IDC_TILEHEIGHT_SPIN2, SPIN_AUTOSCALE, 
		p_end,
	edge_width2,	_T("edgeWidth2"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TILE_EWIDTH2,
		p_default,		.0f,
		p_range,		0.0, 100.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_EDGEWIDTH_EDIT2, IDC_EDGEWIDTH_SPIN2, SPIN_AUTOSCALE, 
		p_end,
	edge_height2,	_T("edgeheight2"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TILE_EHEIGHT2,
		p_default,		.0f,
		p_range,		0.0, 100.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_EDGEHEIGHT_EDIT2, IDC_EDGEHEIGHT_SPIN2, SPIN_AUTOSCALE, 
		p_end,


	soften,	_T("soften"),		TYPE_INT,		0,				IDS_TILE_SOFTEN,		
		p_default,		2,		
		p_ui, 			TYPE_INTLISTBOX, IDC_SOFTEN, 0, 
		p_end,	
	soften_rad,	_T("softenRad"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TILE_SOFTRAD,
		p_default,		.1f,
		p_range,		0.0, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SOFTENRAD_EDIT, IDC_SOFTENRAD_SPIN, SPIN_AUTOSCALE, 
		p_end,
	pb_round,	_T("pb_round"), TYPE_BOOL,			0,				IDS_TILE_ROUND,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_CORNERS,
		p_end,
	round_rad,	_T("roundRad"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TILE_ROUNDRAD,
		p_default,		.3f,
		p_range,		0.0, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_CORNER_EDIT, IDC_CORNER_SPIN, SPIN_AUTOSCALE, 
		p_end,

	// UV
	pb_auto,	_T("pb_auto"),		TYPE_INT,		0,				IDS_TILE_AUTO,		
		p_default,		5,		
		p_ui, 			TYPE_INTLISTBOX, IDC_AUTO, 0, 
		p_end,

	pb_map_uv,	_T("mapUV"), TYPE_BOOL,			0,				IDS_TILE_MAPUV,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MAPUV,
		p_end,
	pb_uv_channel,	_T("uvChannel"),   TYPE_INT,			P_ANIMATABLE,	IDS_UVWCHAN,
		p_default,		1,
		p_range,		1, 99,
		p_ui, 			TYPE_SPINNER, EDITTYPE_POS_INT, IDC_UVCHAN_EDIT, IDC_UVCHAN_SPIN, SPIN_AUTOSCALE,
		p_end,


	pb_randX,	_T("pb_randX"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_RANDOM_X_OFF,
		p_default,		.0f,
		p_range,		0.0, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_OFF_X2, IDC_OFF_X_SPIN2, SPIN_AUTOSCALE, 
		p_end,
	pb_randY,	_T("pb_randY"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_RANDOM_Y_OFF,
		p_default,		.0f,
		p_range,		0.0f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_OFF_Y2, IDC_OFF_Y_SPIN2, SPIN_AUTOSCALE, 
		p_end,
	pb_randSX,	_T("pb_randSX"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_RANDOM_X_SCA,
		p_default,		.0f,
		p_range,		0.0f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SIZ_X2, IDC_SIZ_X_SPIN2, SPIN_AUTOSCALE, 
		p_end,
	pb_randSY,	_T("pb_randSY"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_RANDOM_Y_SCA,
		p_default,		.0f,
		p_range,		0.0f, 1000000.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_SIZ_Y2, IDC_SIZ_Y_SPIN2, SPIN_AUTOSCALE, 
		p_end,

	pb_lock,	_T("pb_lock"), TYPE_BOOL,			0,				IDS_LOCK_SCALE,
		p_default,		TRUE,
		p_ui,			TYPE_CHECKBUTTON, IDC_LOCK,
		p_end,

	pb_rotUV,	_T("rotUV"),		TYPE_INT,		0,				IDS_TILE_ROTUV,		
		p_default,		2,		
		p_ui, 			TYPE_INTLISTBOX, IDC_ROTUV, 0, 
		p_end,
	pb_randRot,	_T("pb_randRot"),   TYPE_FLOAT,			P_ANIMATABLE,	IDS_TILE_ROT,
		p_default,		.0f,
		p_range,		0.0f, 180.f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_RANDROT, IDC_RANDROT_SPIN, SPIN_AUTOSCALE, 
		p_end,

	
	pb_flipX,	_T("pb_flipX"), TYPE_BOOL,			0,				IDS_FLIP_X,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_FLIPX,
		p_end,
	pb_flipY,	_T("pb_flipY"), TYPE_BOOL,			0,				IDS_FLIP_Y,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_FLIPY,
		p_end,

	pb_randZ,	_T("pb_randZ"), TYPE_BOOL,			0,				IDS_SUPPORT,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_RANDZ,
		p_end,	

	pb_center,	_T("pb_center"), TYPE_BOOL,			0,				IDS_CENTER_MAP,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MAPUV2,
		p_end,
	pb_center_channel,	_T("pb_center_channel"),   TYPE_INT,			P_ANIMATABLE,	IDS_UVWCHAN2,
		p_default,		1,
		p_range,		1, 99,
		p_ui, 			TYPE_SPINNER, EDITTYPE_POS_INT, IDC_UVCHAN_EDIT2, IDC_UVCHAN_SPIN2, SPIN_AUTOSCALE,
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
	pb_output,			_T("Output"),		TYPE_REFTARG,	P_OWNERS_REF,	IDS_OUTPUT,
		p_refno,		OUTPUT_REF, 
		p_end,

	pb_pattern,	_T("pattern"), TYPE_STRING,			0,				IDS_PATTERN_STRING,
		p_default,		"0,1,1/.5,.5,1",
		p_ui,			TYPE_EDITBOX, IDC_PATTERN,
		p_end,

	p_end
);

enum {
	map1, map2,
	mapOn1, mapOn2,
};

static ParamBlockDesc2 BerconMap_param_blk ( BerconMap_params, _T("params"),  0, &BerconTileDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBMAP_REF, 	
	IDD_PANEL_TILE_MAP, IDS_PBMAP, 0, 1, NULL,
	
	map1, _T("bmap1"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP1, p_refno, BMAP1_REF, p_subtexno, BMAP1_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP1, p_end, mapOn1, _T("bmap1Enabled"), TYPE_BOOL, 0, IDS_MAPON1, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON1, p_end,
	map2, _T("bmap2"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP2, p_refno, BMAP2_REF, p_subtexno, BMAP2_NUM, p_ui, TYPE_TEXMAPBUTTON, IDC_MAP2, p_end, mapOn2, _T("bmap2Enabled"), TYPE_BOOL, 0, IDS_MAPON2, p_default, TRUE, p_ui, TYPE_SINGLECHEKBOX, IDC_MAPON2, p_end,

	p_end
);

//dialog stuff to get the Set Ref button
class BerconTileDlgProc : public ParamMap2UserDlgProc {
	public:
		BerconTile *tile;		
		BerconTileDlgProc(BerconTile *m) {tile = m;}		
		INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);		
		void DeleteThis() {delete this;}
		void SetThing(ReferenceTarget *m) {
			tile = (BerconTile*)m;
			tile->EnableStuff(GetCOREInterface()->GetTime());
		}
};

INT_PTR BerconTileDlgProc::DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)	{	
	switch (msg) {
		case WM_INITDIALOG:	{			
			// Lock buttons			
			ICustButton* custButton = GetICustButton(GetDlgItem(hWnd, IDC_LOCKEDGE));
			custButton->SetText(_T("L"));
			ReleaseICustButton(custButton);
			custButton = GetICustButton(GetDlgItem(hWnd, IDC_LOCK));
			custButton->SetText(_T("L"));
			ReleaseICustButton(custButton); 

			// Style
			HWND hwndMap = GetDlgItem(hWnd, IDC_TYPE);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILETYPE_11)); // Custom
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILETYPE_10)); // Herringbone
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILETYPE_9)); // Stack
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILETYPE_0)); // Stretcher			
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILETYPE_1)); // Flemish Stretcher
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILETYPE_2)); // Common
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILETYPE_3)); // Flemish
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILETYPE_4)); // Monk
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILETYPE_5)); // Flemish Garden Wall
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILETYPE_6)); // English
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILETYPE_7)); // English Cross
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILETYPE_8)); // Double

			// Soften
			hwndMap = GetDlgItem(hWnd, IDC_SOFTEN);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILESOFTEN_0));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILESOFTEN_1));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILESOFTEN_2));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILESOFTEN_3));

			// UV rotation
			hwndMap = GetDlgItem(hWnd, IDC_ROTUV);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILEROT_0));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILEROT_1));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILEROT_2));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILEROT_3));	

			// Auto scale
			hwndMap = GetDlgItem(hWnd, IDC_AUTO);  
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILEUVW_0));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILEUVW_1));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILEUVW_2));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILEUVW_3));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILEUVW_4));
			SendMessage(hwndMap, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_TILEUVW_5));

			// Set correct dropdown value			
			int curIndex;
			map->GetParamBlock()->GetValue(tile_style, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_TYPE), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(soften, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_SOFTEN), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_rotUV, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_ROTUV), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_auto, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_AUTO), CB_SETCURSEL, (WPARAM)curIndex, 0);
			break;
		}		
		case WM_SHOWWINDOW:	{					
			// Set correct dropdown value
			int curIndex;
			map->GetParamBlock()->GetValue(tile_style, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_TYPE), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(soften, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_SOFTEN), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_rotUV, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_ROTUV), CB_SETCURSEL, (WPARAM)curIndex, 0);
			map->GetParamBlock()->GetValue(pb_auto, t, curIndex, FOREVER);
			SendMessage(GetDlgItem(hWnd, IDC_AUTO), CB_SETCURSEL, (WPARAM)curIndex, 0);
			break;
		}
		default: return FALSE;
	}
	return TRUE;
}

//--- BerconTile -------------------------------------------------------
BerconTile::BerconTile() {	
	for (int i=0; i<TILE_NSUBTEX; i++) subtex[i] = NULL;
	texHandle = NULL;
	pblock = NULL;
	pbMap = NULL;
	pbXYZ = NULL;
	BerconTileDesc.MakeAutoParamBlocks(this);	
	texout = NULL;				
	berconXYZ.mode2D = TRUE;
	Reset();
}

BerconTile::~BerconTile() { DiscardTexHandle(); }
//From MtlBase
void BerconTile::Reset() {
	TimeValue t = GetCOREInterface()->GetTime();

	if (texout) texout->Reset();
	else ReplaceReference( OUTPUT_REF, GetNewDefaultTextureOutput());
									   
	for (int i=0; i<TILE_NSUBTEX; i++) 
		DeleteReference(i+2);	

	pblock->SetValue( noise_color1, t, Color(1.f, 1.f, 1.f));
	pblock->SetValue( noise_color2, t, Color(0.f, 0.f, 0.f));
	pblock->SetValue( noise_color3, t, Color(.5f, .5f, .5f));
	pblock->SetValue( noise_map1_on, t, TRUE);
	pblock->SetValue( noise_map2_on, t,	TRUE);
	pblock->SetValue( noise_map2_on, t,	TRUE);
	pblock->SetValue( distortion_str, t, .1f);
	pblock->SetValue( use_distortion, t, FALSE);
	pblock->SetValue( lock_edge, t, TRUE);

	pblock->SetValue( tile_style,    t, 3);
	pblock->SetValue( tile_size,     t, 1.0f);
	pblock->SetValue( tile_width,    t, 4.f);
	pblock->SetValue( tile_height,   t, 2.f);
	pblock->SetValue( edge_width,    t, .1f);
	pblock->SetValue( edge_height,   t, .1f);
	pblock->SetValue( tile_width2,   t, 0.f);
	pblock->SetValue( tile_height2,  t, 0.f);
	pblock->SetValue( edge_width2,   t, 0.f);
	pblock->SetValue( edge_height2,  t, 0.f);

	pblock->SetValue( soften, t, 2);
	pblock->SetValue( soften_rad, t, .1f);
	pblock->SetValue( pb_round, t,	TRUE);
	pblock->SetValue( round_rad, t, .1f);

	pblock->SetValue( pb_map_uv, t, FALSE);	
	pblock->SetValue( pb_uv_channel, t, 1);
	pblock->SetValue( pb_auto, t, 2);
	
	pblock->SetValue( pb_randX,   t, 0.f);
	pblock->SetValue( pb_randY,   t, 0.f);
	pblock->SetValue( pb_randSX,  t, 0.f);
	pblock->SetValue( pb_randSY,  t, 0.f);
	pblock->SetValue( pb_lock,  t, 1);	

	pblock->SetValue( pb_rotUV,  t, 0);	
	pblock->SetValue( pb_randRot,  t, 180.f);

	pblock->SetValue( pb_center,  t, 0);
	pblock->SetValue( pb_center_channel,  t, 2);

	pblock->SetValue( pb_flipX,  t, 1);
	pblock->SetValue( pb_flipY,  t, 1);

	pblock->SetValue( pb_randZ,  t, 1);	

	pblock->SetValue(pb_pattern, t, L"0,1,1/.5,.5,1");

	// Maps
	for (int i=2; i<4; i++)
		pbMap->SetValue(i, t, TRUE);	

	berconXYZ.reset(pbXYZ, ivalid, 1, 0, 0, 0);

	ivalid.SetEmpty();
}

void BerconTile::Update(TimeValue t, Interval& valid) {	
	if (pblock == NULL) return;

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

		for (int i=0; i<TILE_NSUBTEX; i++)
			if (subtex[i]) 
				subtex[i]->Update(t,ivalid);

		pblock->GetValue( noise_map1_on, t, mapOn[0], ivalid);
		pblock->GetValue( noise_map2_on, t, mapOn[1], ivalid);		
		pblock->GetValue( noise_map3_on, t, mapOn[2], ivalid);					

		pblock->GetValue( tile_size, t, tileSize, ivalid);

		// Tile params
		pblock->GetValue( tile_style, t, tileParam.tilingType, ivalid);

		pblock->GetValue( tile_width, t, tileParam.tileWidth, ivalid);
		pblock->GetValue( tile_height, t, tileParam.tileHeight, ivalid);
		pblock->GetValue( edge_width, t, tileParam.edgeWidth, ivalid);
		pblock->GetValue( edge_height, t, tileParam.edgeHeight, ivalid);

		pblock->GetValue( tile_width2, t, tileParam.tileWidthVar, ivalid);
		pblock->GetValue( tile_height2, t, tileParam.tileHeightVar, ivalid);
		pblock->GetValue( edge_width2, t, tileParam.edgeWidthVar, ivalid);
		pblock->GetValue( edge_height2, t, tileParam.edgeHeightVar, ivalid);

		pblock->GetValue( soften, t, tileParam.tileBlur, ivalid);
		pblock->GetValue( soften_rad, t, tileParam.tileBlurRad, ivalid);
		pblock->GetValue( pb_round, t, tileParam.tileRound, ivalid);
		pblock->GetValue( round_rad, t, tileParam.tileCrnrRad, ivalid);		
		
		pblock->GetValue( lock_edge, t, lockEdge, ivalid);		

		pblock->GetValue( pb_auto, t, tileParam.autoScale, ivalid);

		pblock->GetValue( pb_map_uv, t, tileParam.mapUV, ivalid);
		pblock->GetValue( pb_rotUV, t, tileParam.rotUV, ivalid);		
		pblock->GetValue( pb_uv_channel, t, uvChan, ivalid);		
		pblock->GetValue( pb_randRot, t, tileParam.randRot, ivalid);

		pblock->GetValue( pb_randX, t, tileParam.randX, ivalid);
		pblock->GetValue( pb_randY, t, tileParam.randY, ivalid);

		pblock->GetValue( pb_randSX, t, tileParam.randSX, ivalid);
		pblock->GetValue( pb_randSY, t, tileParam.randSY, ivalid);
		
		pblock->GetValue( pb_lock, t, tileParam.lock, ivalid);		
	
		pblock->GetValue( pb_flipX, t, tileParam.flipX, ivalid);
		pblock->GetValue( pb_flipY, t, tileParam.flipY, ivalid);
		pblock->GetValue( pb_randZ, t, tileParam.tileID, ivalid);
	
	
		pblock->GetValue( pb_center, t, tileParam.center, ivalid);	
		pblock->GetValue( pb_center_channel, t, uvChan2, ivalid);

		// Dist params
		pblock->GetValue( distortion_str, t, distortionStr, ivalid);
		pblock->GetValue( use_distortion, t, useDistortion, ivalid);			

		// Update maps
		for (int i = 0; i<2; i++)
			pbMap->GetValue((i+2), t, mapOn[i+5], ivalid);		

		// Slight optimization
		mappedParameters = false;
		for (int i=5;i<=6;i++)
			if (mapOn[i] && subtex[i])
				mappedParameters = true;	

		berconXYZ.update(pbXYZ, t, ivalid);

/*#if MAX_RELEASE >= 12000 //MAX_RELEASE_R12
		const char *s;
#else
		char *s;
#endif*/
		const wchar_t *s;


		pblock->GetValue(pb_pattern, t, s, ivalid);			
		
		if (tileParam.tilingType == 0)
			pattern.setPattern(std::wstring(s));
		else if (tileParam.tilingType != 1)
			pattern.setPreset(tileParam.tilingType-2);
		//pattern.setPattern("0, 1, 1 / .5, 0.5, 1");

		tileParam.tileHeightVar /= 200.f;
		tileParam.tileWidthVar /= 200.f;
		tileParam.edgeHeightVar /= 100.f;
		tileParam.edgeWidthVar /= 100.f;
		tileParam.randRot *= DEG2RAD;
		tileParam.checkValues();

		EnableStuff(t);		
	}
	valid &= ivalid;
}

Interval BerconTile::Validity(TimeValue t) {
	Interval v;
	Update(t,v);
	return ivalid;
}

void BerconTile::EnableStuff(TimeValue t) {
	if (pblock) {
		IParamMap2 *map = pblock->GetMap();
		if (map) {
			// Update values we need to know
			
			//pblock->GetValue( tile_style, 0, tilingType, FOREVER);
			pblock->GetValue( use_distortion, 0, useDistortion, FOREVER);

			// Update parameters			
			map->Enable(soften_rad, tileParam.tileBlur!=0?TRUE:FALSE);
			map->Enable(round_rad, tileParam.tileRound);

			map->Enable(tile_height, tileParam.tilingType != 1);
			map->Enable(tile_width2, tileParam.tilingType != 1);
			map->Enable(tile_height2, tileParam.tilingType != 1);

			map->Enable(pb_uv_channel, tileParam.mapUV);
			map->Enable(pb_auto, tileParam.mapUV);
			map->Enable(pb_rotUV, tileParam.mapUV);
			map->Enable(pb_uv_channel, tileParam.mapUV);
			map->Enable(pb_randX, tileParam.mapUV);
			map->Enable(pb_randY, tileParam.mapUV);
			map->Enable(pb_randSX, tileParam.mapUV);			
			map->Enable(pb_randSY, tileParam.mapUV && (!tileParam.lock));			
			map->Enable(pb_lock, tileParam.mapUV);
			map->Enable(pb_flipX, tileParam.mapUV);
			map->Enable(pb_flipY, tileParam.mapUV);
			map->Enable(pb_randRot, tileParam.mapUV && (tileParam.rotUV==3));	
			map->Enable(pb_center_channel, tileParam.center);

			map->Enable(noise_color3, lockEdge?FALSE:TRUE);
			map->Enable(noise_map3, lockEdge?FALSE:TRUE);
			map->Enable(noise_map3_on, lockEdge?FALSE:TRUE);

			map->Enable(distortion_str, useDistortion);
			map->Enable(distortion_map, useDistortion);
			map->Enable(distortion_map2, useDistortion);

			int type = berconXYZ.isRealworld();

			setSpinnerType(map, t, tile_width,  IDC_TILEWIDTH_EDIT,  IDC_TILEWIDTH_SPIN,  type);	
			setSpinnerType(map, t, tile_height, IDC_TILEHEIGHT_EDIT, IDC_TILEHEIGHT_SPIN, type);
			setSpinnerType(map, t, edge_width,  IDC_EDGEWIDTH_EDIT,  IDC_EDGEWIDTH_SPIN,  type);
			setSpinnerType(map, t, edge_height, IDC_EDGEHEIGHT_EDIT, IDC_EDGEHEIGHT_SPIN, type);

			setSpinnerType(map, t, soften_rad, IDC_SOFTENRAD_EDIT, IDC_SOFTENRAD_SPIN, type);
			setSpinnerType(map, t, round_rad,  IDC_CORNER_EDIT,    IDC_CORNER_SPIN,    type);
		}
	}
}

DWORD_PTR BerconTile::GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker) {
	if (texHandle) {
		if (texHandleValid.InInterval(t))
			return texHandle->GetHandle();
		else DiscardTexHandle();
		}
	texHandle = thmaker.MakeHandle(GetVPDisplayDIB(t,thmaker,texHandleValid));
	return texHandle->GetHandle();
}

ParamDlg* BerconTile::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) {	
	IAutoMParamDlg* masterDlg = BerconTileDesc.CreateParamDlgs(hwMtlEdit, imp, this);
	texoutDlg = texout->CreateParamDlg(hwMtlEdit, imp);
	masterDlg->AddDlg(texoutDlg);
	BerconTile_param_blk.SetUserDlgProc(new BerconTileDlgProc(this));
	xyz_blk.SetUserDlgProc(new BerconXYZDlgProc(this));
	EnableStuff(GetCOREInterface()->GetTime());
	return masterDlg;	
}

BOOL BerconTile::SetDlgThing(ParamDlg* dlg) {	
	if (dlg == texoutDlg)
		texoutDlg->SetThing(texout);
	else 
		return FALSE;
	return TRUE;
}

void BerconTile::SetSubTexmap(int i, Texmap *m)  {
	ReplaceReference(i+2,m);
	if (i==0) {
		BerconTile_param_blk.InvalidateUI(noise_map1);
		ivalid.SetEmpty();		
	} else if (i==1) {
		BerconTile_param_blk.InvalidateUI(noise_map2);
		ivalid.SetEmpty();		
	} else if (i==2) {
		BerconTile_param_blk.InvalidateUI(noise_map3);
		ivalid.SetEmpty();	
	} else if (i==3) {
		BerconTile_param_blk.InvalidateUI(distortion_map);
		ivalid.SetEmpty();	
	} else if (i==4) {
		BerconTile_param_blk.InvalidateUI(distortion_map2);
		ivalid.SetEmpty();
	}

	if (BMAP1_NUM <= i && i <= BMAP2_NUM) {  // TODO: Make range is correct
		BerconMap_param_blk.InvalidateUI(i-BMAP1_NUM);
		ivalid.SetEmpty();
	}
}

TSTR BerconTile::GetSubTexmapSlotName(int i)  {	
	switch(i) {
		case 0:  return TSTR(GetString(IDS_DS_COLOR1)); 
		case 1:  return TSTR(GetString(IDS_DS_COLOR2)); 
		case 2:  return TSTR(GetString(IDS_DS_COLOR3)); 
		case 3:  return TSTR(GetString(IDS_DISTORTION_MAP)); 
		case 4:  return TSTR(GetString(IDS_DISTORTION_MAP2)); 

		case BMAP1_NUM: return TSTR(GetString(IDS_MAP1)); 
		case BMAP2_NUM: return TSTR(GetString(IDS_MAP2)); 

		default: return TSTR(_T(""));
	}
}

RefTargetHandle BerconTile::GetReference(int i)  {
	switch (i) {
		case COORD_REF: return pbXYZ;
		case PBLOCK_REF: return pblock;
		case OUTPUT_REF: return texout;		
		case PBMAP_REF: return pbMap;
		default: return subtex[i-2];
	}
}

void BerconTile::SetReference(int i, RefTargetHandle rtarg) {	
	switch(i) {
		case COORD_REF: pbXYZ = (IParamBlock2 *)rtarg; break;
		case PBLOCK_REF: pblock = (IParamBlock2 *)rtarg; break;
		case OUTPUT_REF: texout = (TextureOutput *)rtarg; break;		
		case PBMAP_REF: pbMap = (IParamBlock2 *)rtarg; break;
		default: subtex[i-2] = (Texmap *)rtarg; break;
	}
}

RefTargetHandle BerconTile::Clone(RemapDir &remap) {
	BerconTile *mnew = new BerconTile();
	*((MtlBase*)mnew) = *((MtlBase*)this); // copy superclass stuff
	mnew->ReplaceReference(COORD_REF,remap.CloneRef(pbXYZ));
	mnew->ReplaceReference(OUTPUT_REF,remap.CloneRef(texout));
	mnew->ReplaceReference(PBLOCK_REF,remap.CloneRef(pblock));	
	mnew->ReplaceReference(PBMAP_REF,remap.CloneRef(pbMap));
	mnew->ivalid.SetEmpty();		
	for (int i = 0; i<TILE_NSUBTEX; i++) {
		mnew->subtex[i] = NULL;
		if (subtex[i])
			mnew->ReplaceReference(i+2,remap.CloneRef(subtex[i]));		
	}
	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
}

Animatable* BerconTile::SubAnim(int i) {
	switch (i) {
		case COORD_REF: return pbXYZ;
		case PBLOCK_REF: return pblock;	
		case OUTPUT_REF: return texout;
		case PBMAP_REF: return pbMap;
		default: return subtex[i-2];
	}
}

TSTR BerconTile::SubAnimName(int i) {	
	switch (i) {
		case COORD_REF: return GetString(IDS_COORDS);		
		case PBLOCK_REF: return GetString(IDS_PARAMS);		
		case OUTPUT_REF: return GetString(IDS_OUTPUT);
		case PBMAP_REF: return GetString(IDS_PBMAP);
		default: return GetSubTexmapTVName(i-2);
	}
}

RefResult BerconTile::NotifyRefChanged(NOTIFY_REF_CHANGED_ARGS) {
	switch (message) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();			
			if (hTarget == pblock) {
				ParamID changing_param = pblock->LastNotifyParamID();
				BerconTile_param_blk.InvalidateUI(changing_param);				
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

// Define some basic values
static AColor black(0.0f,0.0f,0.0f,1.0f);
static AColor white(1.0f,1.0f,1.0f,1.0f);

Point3 BerconTile::getDistVector(ShadeContext& sc) {
	if (subtex[3])
		if (subtex[4])
			return subtex[3]->EvalNormalPerturb(sc)*distortionStr*subtex[4]->EvalMono(sc);
		else					
			return subtex[3]->EvalNormalPerturb(sc)*distortionStr;
	else		  
		return Point3(0.f, 0.f, 0.f);
}

TileParam BerconTile::EvalParameters(ShadeContext& sc) {
	TileParam t = tileParam;
	t.pattern = &pattern;

	if (!mappedParameters)
		return t;
			
	t.tileBlurRad = (mapOn[5] && subtex[5])?subtex[5]->EvalMono(sc)*tileParam.tileBlurRad:tileParam.tileBlurRad;
	t.tileCrnrRad = (mapOn[6] && subtex[6])?subtex[6]->EvalMono(sc)*tileParam.tileCrnrRad:tileParam.tileCrnrRad;
	
	return t;
}

AColor BerconTile::EvalColor(ShadeContext& sc) {	
	Point3 p;	
	if (!sc.doMaps) return black;

	// If we've already evalutated the color at this point we'll use it and stop here
	AColor c;
	if (sc.GetCache(this,c)) 
		return c; 	
	if (gbufID) sc.SetGBufferID(gbufID);

	// Evaulate maps and tiling parameters
	TileParam t	= EvalParameters(sc);

	// UVW, Distortion and size
	berconXYZ.get(sc,p);
	if (useDistortion) p += getDistVector(sc);
	p /= tileSize;

	// Caluclate tiling	
	TilePoint tp = Tile::draw(p, t);

	// Calculate color	
	if (tp.d < -.5f) // First check if we are on edge		
		c = mapOn[1]&&subtex[1]?subtex[1]->EvalColor(sc): col[1];
	else {
		RGBA c1, c2;

		if (tileParam.mapUV || tileParam.tileID || tileParam.center) { // Then if we map UV coordinates
			
			BerconSC bsc = BerconSC(&sc);			
			if (tileParam.mapUV)
				bsc.setUV1(tp.uvw, uvChan);
			if (tileParam.center)
				bsc.setUV2(tp.center, uvChan2);
			if (tileParam.tileID)
				bsc.setMultiTexture((float)tp.id);

			              c1 = getColor(bsc, 0);
			if (lockEdge) c2 = getColor(bsc, 1);
			else          c2 = getColor(bsc, 2);
		
		} else { // Normal eval
			              c1 = getColor(sc, 0);
			if (lockEdge) c2 = getColor(sc, 1);
			else		  c2 = getColor(sc, 2);
		}
		c = (1.0f-tp.d)*c2 + tp.d*c1;
	}

	c = texout->Filter(c);

	// Cache
	sc.PutCache(this,c); 
	return c;	
}

float BerconTile::EvalMono(ShadeContext& sc) {
	return Intens(EvalColor(sc));
}

#define DELTA 0.001f

inline static float noise(Point3 p) { return (Perlin::noise(p.x, p.y, p.z) + 1.f) * .5f; }
Point3 BerconTile::EvalNormalPerturb(ShadeContext& sc) {	
	
	if (!sc.doMaps) return Point3(0,0,0);
	if (gbufID) sc.SetGBufferID(gbufID);

	// Evaulate maps and tiling parameters
	TileParam t	= EvalParameters(sc);

	// UVW, Distortion and size
	Point3 p;
	Point3 M[3];
	berconXYZ.get(sc, p, M);	

	if (useDistortion) p += getDistVector(sc);
	p /= tileSize;

	TilePoint tp =  Tile::draw(p, t);
	if (tp.d < 0) return getNormal(sc, 1);

	Point3 np;
	if (berconXYZ.req()) {		
		TilePoint tp1 = Tile::draw(p + Point3(DELTA,0.f,0.f), t); if (tp1.d < 0) return getNormal(sc, 1);				
		TilePoint tp2 = Tile::draw(p + Point3(0.f,DELTA,0.f), t); if (tp2.d < 0) return getNormal(sc, 1);
		np.x = (tp1.d - tp.d) / DELTA;
		np.y = (tp2.d - tp.d) / DELTA;
		np = M[0]*np.x + M[1]*np.y;
	} else {
		TilePoint tp1 = Tile::draw(p + DELTA*M[0], t); if (tp1.d < 0) return getNormal(sc, 1);				
		TilePoint tp2 = Tile::draw(p + DELTA*M[1], t); if (tp2.d < 0) return getNormal(sc, 1);
		TilePoint tp3 = Tile::draw(p + DELTA*M[2], t); if (tp3.d < 0) return getNormal(sc, 1);	
		np.x = (tp1.d - tp.d) / DELTA;
		np.y = (tp2.d - tp.d) / DELTA;
		np.z = (tp3.d - tp.d) / DELTA;
		np = -sc.VectorFromNoScale(np, REF_OBJECT);
	}
	
	// Eval sub maps
	float f1, f2;
	Point3 v1, v2;

	if (tileParam.mapUV || tileParam.tileID || tileParam.center) { // Then if we map UV coordinates
			
		BerconSC bsc = BerconSC(&sc);			
		if (tileParam.mapUV)
			bsc.setUV1(tp.uvw, uvChan);
		if (tileParam.center)
			bsc.setUV2(tp.center, uvChan2);
		if (tileParam.tileID)
			bsc.setMultiTexture((float)tp.id);

		              f1 = getFloat(bsc, 0);
		if (lockEdge) f2 = getFloat(bsc, 1);
		else		  f2 = getFloat(bsc, 2);

		              v1 = getNormal(bsc, 0);
		if (lockEdge) v2 = getNormal(bsc, 1);
		else		  v2 = getNormal(bsc, 2);
	} else { // And finally just normal eval
		              f1 = getFloat(sc, 0);
		if (lockEdge) f2 = getFloat(sc, 1);
		else		  f2 = getFloat(sc, 2);

		              v1 = getNormal(sc, 0);
		if (lockEdge) v2 = getNormal(sc, 1);
		else		  v2 = getNormal(sc, 2);
	}

	np = (f2-f1)*np + tp.d*v2 + (1.f-tp.d)*v1;
	
	return texout->Filter(np); // Does this filter actually do something?*/
}