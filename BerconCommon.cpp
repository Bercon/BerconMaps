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


#include "BerconCommon.h"
#include "chkmtlapi.h"


void BerconXYZ::reset(IParamBlock2* pblock, Interval& ivalid, int type, int x, int y, int z) {
	if (!pblock) return;
	//if (!pblock->GetMap()) return;

	TimeValue t = GetCOREInterface()->GetTime();

	pblockSetValue(xyz_offset_x, 0.f);	
	pblockSetValue(xyz_offset_y, 0.f);	
	pblockSetValue(xyz_offset_z, 0.f);
	pblockSetValue(xyz_size_x, 1.f);	
	pblockSetValue(xyz_size_y, 1.f);	
	pblockSetValue(xyz_size_z, 1.f);	
	pblockSetValue(xyz_angle_x, 0.f);	
	pblockSetValue(xyz_angle_y, 0.f);	
	pblockSetValue(xyz_angle_z, 0.f);

	pblockSetValue(xyz_offset_x2, 0.f);	
	pblockSetValue(xyz_offset_y2, 0.f);	
	pblockSetValue(xyz_offset_z2, 0.f);
	pblockSetValue(xyz_size_x2, 0.f);	
	pblockSetValue(xyz_size_y2, 0.f);	
	pblockSetValue(xyz_size_z2, 0.f);	
	pblockSetValue(xyz_angle_x2, 0.f);	
	pblockSetValue(xyz_angle_y2, 0.f);	
	pblockSetValue(xyz_angle_z2, 0.f);

	pblockSetValue(xyz_tile_x, x);
	pblockSetValue(xyz_tile_y, y);
	pblockSetValue(xyz_tile_z, z);

	pblockSetValue(xyz_seed, 12345);
	pblockSetValue(xyz_rand_obj, TRUE);
	pblockSetValue(xyz_rand_mat, FALSE);
	pblockSetValue(xyz_rand_par, FALSE);

	pblockSetValue(xyz_map, type);
	pblockSetValue(xyz_chan, 1);

	pblockSetValue(xyz_lock, TRUE);

	pblockSetValue(xyz_filtering, 1.f);

	ivalid.SetEmpty();	
}

void BerconXYZ::update(IParamBlock2* pblock, TimeValue t, Interval& ivalid) {
	if (!pblock) return;
	//if (!pblock->GetMap()) return;

	pblockGetValue(xyz_offset_x, offX);	
	pblockGetValue(xyz_offset_y, offY);	
	pblockGetValue(xyz_offset_z, offZ);
	pblockGetValue(xyz_size_x, sizeX);	
	pblockGetValue(xyz_size_y, sizeY);	
	pblockGetValue(xyz_size_z, sizeZ);	
	pblockGetValue(xyz_angle_x, angX);	
	pblockGetValue(xyz_angle_y, angY);	
	pblockGetValue(xyz_angle_z, angZ);

	pblockGetValue(xyz_offset_x2, offX2);	
	pblockGetValue(xyz_offset_y2, offY2);	
	pblockGetValue(xyz_offset_z2, offZ2);
	pblockGetValue(xyz_size_x2, sizeX2);	
	pblockGetValue(xyz_size_y2, sizeY2);	
	pblockGetValue(xyz_size_z2, sizeZ2);	
	pblockGetValue(xyz_angle_x2, angX2);	
	pblockGetValue(xyz_angle_y2, angY2);	
	pblockGetValue(xyz_angle_z2, angZ2);

	pblockGetValue(xyz_tile_x, tileX);
	pblockGetValue(xyz_tile_y, tileY);
	pblockGetValue(xyz_tile_z, tileZ);

	pblockGetValue(xyz_seed, p_seed);
	pblockGetValue(xyz_rand_obj, p_randObj);
	pblockGetValue(xyz_rand_mat, p_randMat);
	pblockGetValue(xyz_rand_par, p_randPar);

	pblockGetValue(xyz_map, mappingType);
	pblockGetValue(xyz_chan, mappingChannel);

	pblockGetValue(xyz_lock, lock);

	pblockGetValue(xyz_filtering, filtering);
	

	angX *= DEG_TO_RAD; angY *= DEG_TO_RAD; angZ *= DEG_TO_RAD;
	angX2 *= DEG_TO_RAD; angY2 *= DEG_TO_RAD; angZ2 *= DEG_TO_RAD;

	EnableStuff(pblock, t);
	update();
}

void BerconXYZ::EnableStuff(IParamBlock2* pblock, TimeValue t) {	
	if (pblock) {
		IParamMap2 *map = pblock->GetMap();
		if (map) {
			map->Enable(xyz_size_y2, !lock);			
			map->Enable(xyz_size_z2, !lock);

			bool s = mappingType==0; // || mappingType==1;
			map->Enable(xyz_tile_x, s);
			map->Enable(xyz_tile_y, s);
			map->Enable(xyz_tile_z, s);

			int type = isRealworld();
			
			setSpinnerType(map, t, xyz_offset_x, IDC_OFF_X, IDC_OFF_X_SPIN, type, true);	
			setSpinnerType(map, t, xyz_offset_y, IDC_OFF_Y, IDC_OFF_Y_SPIN, type, true);	
			setSpinnerType(map, t, xyz_offset_z, IDC_OFF_Z, IDC_OFF_Z_SPIN, type, true);	
			setSpinnerType(map, t, xyz_size_x, IDC_SIZ_X, IDC_SIZ_X_SPIN, type);	
			setSpinnerType(map, t, xyz_size_y, IDC_SIZ_Y, IDC_SIZ_Y_SPIN, type);	
			setSpinnerType(map, t, xyz_size_z, IDC_SIZ_Z, IDC_SIZ_Z_SPIN, type);	
			setSpinnerType(map, t, xyz_offset_x2, IDC_OFF_X2, IDC_OFF_X_SPIN2, type);	
			setSpinnerType(map, t, xyz_offset_y2, IDC_OFF_Y2, IDC_OFF_Y_SPIN2, type);	
			setSpinnerType(map, t, xyz_offset_z2, IDC_OFF_Z2, IDC_OFF_Z_SPIN2, type);	
			setSpinnerType(map, t, xyz_size_x2, IDC_SIZ_X2, IDC_SIZ_X_SPIN2, type);	
			setSpinnerType(map, t, xyz_size_y2, IDC_SIZ_Y2, IDC_SIZ_Y_SPIN2, type);	
			setSpinnerType(map, t, xyz_size_z2, IDC_SIZ_Z2, IDC_SIZ_Z_SPIN2, type);
		}
	}
}

void BerconXYZ::getBasis(Matrix3 transform, Point3* b) {
	if (mappingType==4&&mode2D) {
		Matrix3 temp = transform;
		temp.Invert();
		b[0] = Normalize(temp.GetRow(0));
		b[1] = Normalize(temp.GetRow(1));
		b[2] = Normalize(temp.GetRow(2));
	} else {
		b[0] = Normalize(transform.GetRow(0));
		b[1] = Normalize(transform.GetRow(1));
		b[2] = Normalize(transform.GetRow(2));
	}
}

//#define NZERO(x) (x > 0.0001f)
void BerconXYZ::update() {
	tm.IdentityMatrix();
	tm.Translate(Point3(offX, offY, offZ));
 	tm.RotateX(angX);
	tm.RotateY(angY);
	tm.RotateZ(angZ);

	invNoScaleTm = tm;
	invNoScaleTm.Invert();

	tm.Scale(Point3(1.f/sizeX, 1.f/sizeY, 1.f/sizeZ));

	getBasis(tm, b);
	
	variance = (offX2 > 0.0001f) || (offY2 > 0.0001f) || (offZ2 > 0.0001f) || (sizeX2 > 0.0001f) || (sizeY2 > 0.0001f) || (sizeZ2 > 0.0001f) || (angX2 > 0.0001f) || (angY2 > 0.0001f) || (angZ2 > 0.0001f);
}

Matrix3 BerconXYZ::random(ShadeContext& sc, Matrix3* inv) {
	seedRandomGen(sc);

	Matrix3 transform;
	transform.IdentityMatrix();
	transform.Translate(Point3(offX+offX2*URANDF(), offY+offY2*URANDF(), offZ+offZ2*URANDF()));
 	transform.RotateX(angX+angX2*URANDF());
	transform.RotateY(angY+angY2*URANDF());
	transform.RotateZ(angZ+angZ2*URANDF());	

	if (inv) {
		*inv = transform;
		inv->Invert();
	}

	Point3 scale;
	if (lock) {
		float add = sizeX2*URANDF();
		scale = Point3(sizeX+add, sizeY+add, sizeZ+add);	
	} else {
		scale = Point3(sizeX+sizeX2*URANDF(), sizeY+sizeY2*URANDF(), sizeZ+sizeZ2*URANDF());
	}
	if (scale.x < 0.0001f) scale.x = 0.0001f;
	if (scale.y < 0.0001f) scale.y = 0.0001f;
	if (scale.z < 0.0001f) scale.z = 0.0001f;
	scale.x = 1.f/scale.x; scale.y = 1.f/scale.y; scale.z = 1.f/scale.z;
	transform.Scale(scale);

	return transform;
}

void BerconXYZ::seedRandomGen(ShadeContext& sc) {
	int seed = 1;

	if (p_randMat) { 		
		seed += sc.mtlNum;
	}
	if (p_randObj) {
		INode *node=sc.Node();
		if (node) {
			int hand = (int) node->GetHandle();
			seed += hand*(hand*hand*15731 + 789221);
		}
	}
	if (p_randPar) {
		Object *ob = sc.GetEvalObject();		
		if (ob && ob->IsParticleSystem()) {
			auto*obj = static_cast<ParticleObject*>(ob);
			auto* chkMtlAPI = static_cast<IChkMtlAPI*>(obj->GetInterface(I_NEWMTLINTERFACE));
			if ((chkMtlAPI && chkMtlAPI->SupportsParticleIDbyFace())) {
				int id = chkMtlAPI->GetParticleFromFace(sc.FaceNumber());
				seed += id*(id*id*571 + 789221);
			}			
		}
	}

	seed *= p_seed;
	srand(seed*(seed*seed*15731 + 789221));	
}

inline static int tiling(int type, float& x) {
	switch (type) {
		case 1: {D_STRE(x) return TRUE;}
		case 2: {D_LOOP(x) return TRUE;}
		case 3: {D_MIRR(x) return TRUE;}
		case 4: {if (x<0||x>1) return FALSE;}
	}
	return TRUE;
}

inline static int tiling(int type, float& x, int& flip) {
	switch (type) {
		case 1: {D_STRE(x) return TRUE;}
		case 2: {D_LOOP(x) return TRUE;}
		case 3: {
			if (x<0) x = -x;
			int ix = (int)x;
			if (ix%2==0) x = x - ix;
			else { x = 1.f - x + ix; flip = 1; }
			return TRUE;				
		}
		case 4: {if (x<0||x>1) return FALSE;}
	}
	return TRUE;
}

#define OFFSET_5 (Point3(0.5f, 0.5f, 0.f))

int BerconXYZ::get(ShadeContext& sc, Point3& p, Point3& dpdx, Point3& dpdy, Matrix3 transform, int* flips) {
	switch (mappingType) {
		case 0:
		case 1:
			if (mappingType)
				p = transform * sc.UVW(mappingChannel);			
			else
				p = transform * (sc.UVW(mappingChannel) - OFFSET_5) + OFFSET_5;
			{
			Point3 duvw = VectorTransform(transform, sc.DUVW(mappingChannel));
			dpdx = Point3(duvw.x, 0.f, 0.f);
			dpdy = Point3(0.f, duvw.y, 0.f);
			}
			if (flips) { if (!tiling(tileX, p.x, flips[0])) return FALSE; }
			else { if (!tiling(tileX, p.x)) return FALSE; }
			if (flips) { if (!tiling(tileY, p.y, flips[1])) return FALSE; }
			else { if (!tiling(tileY, p.y)) return FALSE; }
			if (flips) { if (!tiling(tileZ, p.z, flips[2])) return FALSE; }
			else { if (!tiling(tileZ, p.z)) return FALSE; }
			break;
		case 2: {
			p = transform * sc.PointTo(sc.P(),REF_OBJECT);
			sc.DP(dpdx, dpdy);	
			dpdx = VectorTransform(transform, sc.VectorTo(dpdx, REF_OBJECT));
			dpdy = VectorTransform(transform, sc.VectorTo(dpdy, REF_OBJECT));
			break;}
		case 3: {
			p = transform * sc.PointTo(sc.P(),REF_WORLD);
			sc.DP(dpdx, dpdy);	
			dpdx = VectorTransform(transform, sc.VectorTo(dpdx, REF_WORLD));
			dpdy = VectorTransform(transform, sc.VectorTo(dpdy, REF_WORLD));
			break;}
		case 4: {
			Point2 uv, duv;
			sc.ScreenUV(uv, duv);
			p = transform * Point3(uv.x, uv.y, 0.f);
			dpdx = VectorTransform(transform, Point3(duv.x, 0.f, 0.f));
			dpdy = VectorTransform(transform, Point3(0.f, duv.y, 0.f));
			break;}
	}

	dpdx = dpdx * filtering; dpdy = dpdy * filtering;
	
	return TRUE;
}

int BerconXYZ::get(ShadeContext& sc, Point3& p, Matrix3 transform, int* flips) {
	switch (mappingType) {
		case 0:
			p = transform * (sc.UVW(mappingChannel) - OFFSET_5) + OFFSET_5;	
			if (flips) { if (!tiling(tileX, p.x, flips[0])) return FALSE; }
			else { if (!tiling(tileX, p.x)) return FALSE; }
			if (flips) { if (!tiling(tileY, p.y, flips[1])) return FALSE; }
			else { if (!tiling(tileY, p.y)) return FALSE; }
			if (flips) { if (!tiling(tileZ, p.z, flips[2])) return FALSE; }
			else { if (!tiling(tileZ, p.z)) return FALSE; }
			break;
		case 1:			
			p = transform * sc.UVW(mappingChannel);			
			break;
		case 2: {
			p = transform * sc.PointTo(sc.P(),REF_OBJECT);
			break;}
		case 3: {
			p = transform * sc.PointTo(sc.P(),REF_WORLD);
			break;}
		case 4: {
			Point2 uv, duv;
			sc.ScreenUV(uv, duv);
			p = transform * Point3(uv.x, uv.y, 0.f);
			break;}
		}
	return TRUE;
}

int BerconXYZ::get(ShadeContext& sc, Point3& p, Point3& dpdx, Point3& dpdy) {
	return get(sc, p, dpdx, dpdy, variance?random(sc):tm);
}

int BerconXYZ::get(ShadeContext& sc, Point3& p) {
	return get(sc, p, variance?random(sc):tm);
}

int BerconXYZ::get(ShadeContext& sc, Point3& p, Point3& dpdx, Point3& dpdy, Point3* basis) {
	/*int* flips = NULL;
	if (mappingType == 0) {
		flips = new int[3];
		flips[0]=0;flips[1]=0;flips[2]=0;
	}*/

	if ((mappingType == 0 || mappingType == 1) && mode2D) {
		Matrix3 inv;
		Matrix3 transform = variance?random(sc, &inv):tm;
		if (!variance) inv = invNoScaleTm;
		
		if (!get(sc, p, dpdx, dpdy, transform)) {return FALSE;}
		Point3 dp[3];				
				 
		if (sc.BumpBasisVectors(dp, AXIS_UV, mappingChannel)) {
			basis[0] = VectorTransform(inv, dp[0]);
			basis[1] = VectorTransform(inv, dp[1]);
			basis[2] = VectorTransform(inv, dp[0]^dp[1]);
		} else {
			sc.DPdUVW(dp, mappingChannel);
			transform.Invert();
			basis[0] = VectorTransform(inv, dp[0]);
			basis[1] = VectorTransform(inv, dp[1]);
			basis[2] = VectorTransform(inv, dp[2]);			
		 }		
	} else {	
		Matrix3 transform = variance?random(sc):tm;
		if (!get(sc, p, dpdx, dpdy, transform)) {return FALSE;}
		if (variance)
			getBasis(transform, basis);
		else
			for (int i=0; i<3; i++)
				basis[i] = b[i];
	}

	/*if (flips) {
		if (flips[0]) basis[0] *= -1;
		if (flips[1]) basis[1] *= -1;
		if (flips[2]) basis[2] *= -1;
	}
	delete[] flips;*/

	return TRUE;
}

int BerconXYZ::get(ShadeContext& sc, Point3& p, Point3* basis) {	
	/*int* flips = NULL;
	if (mappingType == 0) {
		flips = new int[3];
		flips[0]=0;flips[1]=0;flips[2]=0;
	}*/

	if ((mappingType == 0 || mappingType == 1) && mode2D) {
		Matrix3 inv;
		Matrix3 transform = variance?random(sc, &inv):tm;
		if (!variance) inv = invNoScaleTm;
		
		if (!get(sc, p, transform)) {return FALSE;}
		Point3 dp[3];				
				 
		if (sc.BumpBasisVectors(dp, AXIS_UV, mappingChannel)) {
			basis[0] = VectorTransform(inv, dp[0]);
			basis[1] = VectorTransform(inv, dp[1]);
			basis[2] = VectorTransform(inv, dp[0]^dp[1]);
		} else {
			sc.DPdUVW(dp, mappingChannel);
			transform.Invert();
			basis[0] = VectorTransform(inv, dp[0]);
			basis[1] = VectorTransform(inv, dp[1]);
			basis[2] = VectorTransform(inv, dp[2]);			
		 }

	} else {
		Matrix3 transform = variance?random(sc):tm;
		if (!get(sc, p, transform)) {return FALSE;}
		if (variance)
			getBasis(transform, basis);
		else
			for (int i=0; i<3; i++)
				basis[i] = b[i];
	}

	/*if (flips) {
		if (flips[0]) basis[0] *= -1;
		if (flips[1]) basis[1] *= -1;
		if (flips[2]) basis[2] *= -1;
	}
	delete[] flips;*/

	return TRUE;
}

//CharStream *out = thread_local(current_stdout);			
//out->printf("Val: %d\n", mappingChannel);