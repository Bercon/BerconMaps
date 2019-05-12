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
#include "tile.h"

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

#define TILE_NSUBTEX		7  // Number of subtextures

class BerconTile;

class BerconTile : public Texmap, public ResourceMakerCallback {
	public:
		// Tiling parameters
		float tileSize;	
		
		TileParam tileParam;
		int lockEdge, uvChan, uvChan2;

		TilePattern pattern;

		// Tiling		
		bool mappedParameters;
		TileParam EvalParameters(ShadeContext& sc);

		// Distortion		
		BOOL useDistortion;
		float distortionStr;
		Point3 getDistVector(ShadeContext& sc);

		// User Interface
		void EnableStuff(TimeValue t);

		// Parameter block
		IParamBlock2	*pblock;	//ref 1
		IParamBlock2	*pbMap;	//ref PBMAP_REF
		IParamBlock2	*pbXYZ; //ref COORDS

		BerconXYZ berconXYZ;

		Color			 col[3];
		Texmap			*subtex[TILE_NSUBTEX]; //array of sub-materials
		BOOL			mapOn[TILE_NSUBTEX];
		static ParamDlg* texoutDlg;
		TextureOutput   *texout;
		Interval		ivalid;
		//bool viewportPreview;

		inline AColor BerconTile::getColor(ShadeContext &sc, int i)  { return mapOn[i]&&subtex[i] ? subtex[i]->EvalColor(sc): col[i]; }
		inline float  BerconTile::getFloat(ShadeContext &sc, int i)  { return mapOn[i]&&subtex[i] ? subtex[i]->EvalMono(sc): Intens(col[i]); }
		inline Point3 BerconTile::getNormal(ShadeContext &sc, int i) { return mapOn[i]&&subtex[i] ? subtex[i]->EvalNormalPerturb(sc): Point3(0,0,0); }

		// Methods for interactive display
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

		int NumSubTexmaps() { return TILE_NSUBTEX; }
		Texmap* GetSubTexmap(int i) { return subtex[i]; }
		void SetSubTexmap(int i, Texmap *m);
		TSTR GetSubTexmapSlotName(int i);
		
		//From Texmap
		RGBA EvalColor(ShadeContext& sc);
		float EvalMono(ShadeContext& sc);
		Point3 EvalNormalPerturb(ShadeContext& sc);
		
		int SubNumToRefNum(int subNum) { return subNum; }
		
		void ReadSXPData(TCHAR *name, void *sxpdata) { }

		//From Animatable
		Class_ID ClassID() {return BerconTile_CLASS_ID;}		
		SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_BERCON_TILE);}

		RefTargetHandle Clone( RemapDir &remap );
		RefResult NotifyRefChanged(NOTIFY_REF_CHANGED_ARGS);

		int NumSubs() { return 11; }
		Animatable* SubAnim(int i); 
		TSTR SubAnimName(int i);

		int NumRefs() { return 11; }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		int	NumParamBlocks() { return 3; } // return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { switch (i) { case 0: return pblock; case 1: return pbMap; case 2: return pbXYZ; } return NULL; }
		IParamBlock2* GetParamBlockByID(BlockID id) { 
			if (pblock->ID() == id) return pblock;			
			if (pbMap->ID() == id) return pbMap;
			if (pbXYZ->ID() == id) return pbXYZ;
			return NULL;			
		}
		void DeleteThis() { delete this; }		
		
		//Constructor/Destructor
		BerconTile();
		~BerconTile();		

		void* GetInterface(ULONG id) {
			if(id == I_RESMAKER_INTERFACE)
				return (void *) (ResourceMakerCallback*) this;
			else
				return Texmap::GetInterface(id);
		}
};

class BerconTileClassDesc : public ClassDesc2 {
public:
	BerconTileClassDesc() {}
	virtual ~BerconTileClassDesc() {}
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 	{ return new BerconTile(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_BERCON_TILE); }
	virtual SClass_ID SuperClassID() 				{ return TEXMAP_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return BerconTile_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return TEXMAP_CAT_3D; }
	virtual const TCHAR* InternalName() 			{ return _T("BerconTile"); } // returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }
};
