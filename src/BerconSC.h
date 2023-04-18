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

#include "max.h"

#define BERCON_SHADE_CONTEXT_CLASS_ID Class_ID(0x7c0a38f1, 0x2f1a67f2)

class BerconSC: public ShadeContext {
private:
	ShadeContext* const sc;

	int type;	
	Point3 uvPoint;
	Point3 uvPoint2;
	int uvChannel;
	int uvChannel2;
	bool useMultiTexture;
	float val;

public:	
	BerconSC(ShadeContext* const sc) : sc (sc) {
		mode = sc->mode;
		doMaps = sc->doMaps;
		filterMaps = sc->filterMaps;
		shadow = sc->shadow;
		backFace = sc->backFace;					
		mtlNum = sc->mtlNum;
		ambientLight = sc->ambientLight;
		nLights = sc->nLights;
		rayLevel = sc->rayLevel;
		xshadeID = sc->xshadeID;
		atmosSkipLight = sc->atmosSkipLight;
		globContext = sc->globContext;
		out = sc->out;

		uvChannel = -1; uvChannel2 = -1; useMultiTexture = false; type = -1;
	}

	void setParams(Point3 p, int type, int chan=0) {
		uvPoint = p;
		this->type = type;
		uvChannel = chan;
	}	

	void setUV1(Point3 uvw, int chan) {
		uvPoint = uvw;
		uvChannel = chan;
	}

	void setUV2(Point3 uvw, int chan) {
		uvPoint2 = uvw;
		uvChannel2 = chan;
	}

	void setMultiTexture(float val) {
		this->val = val;
		useMultiTexture = true;
	}

	void ResetOutput (int n) { sc->ResetOutput(n); }
	Class_ID ClassID () { return BERCON_SHADE_CONTEXT_CLASS_ID; }
	BOOL InMtlEditor () { return sc->InMtlEditor(); }
	int Antialias () { return sc->Antialias(); }
	int ProjType () { return sc->ProjType(); }
	LightDesc* Light (int n) { return sc->Light(n); }
	TimeValue CurTime () { return sc->CurTime(); }
	int NodeID () { return sc->NodeID(); }
	INode* Node () { return sc->Node(); }
	Object* GetEvalObject () { return sc->GetEvalObject(); }
	Point3 BarycentricCoords () { return sc->BarycentricCoords(); }
	int FaceNumber () { return sc->FaceNumber(); }
	Point3 Normal () { return sc->Normal(); }
	void SetNormal (Point3 p) { sc->SetNormal(p); }
	Point3 OrigNormal () { return sc->OrigNormal(); }
	Point3 GNormal () { return sc->GNormal(); }
	float Curve () { return sc->Curve(); }
	Point3 V () { return sc->V(); }
	void SetView (Point3 p) { sc->SetView(p); }
	Point3 OrigView () { return sc->OrigView(); }
	Point3 ReflectVector () { return sc->ReflectVector(); }
	Point3 RefractVector (float ior) { return sc->RefractVector(ior); }
	void SetIOR (float ior) { sc->SetIOR(ior); }
	float GetIOR () { return sc->GetIOR(); }
	Point3 CamPos () { return sc->CamPos(); }
	inline Point3 P (); // Overridden
	Point3 DP () { return sc->DP(); }
	void DP (Point3& dpdx, Point3& dpdy) { sc->DP(dpdx, dpdx); }
	inline Point3 PObj (); // Overridden
	Point3 DPObj () { return sc->DPObj(); }
	Box3 ObjectBox () { return sc->ObjectBox(); }
	inline Point3 PObjRelBox (); // Overridden
	Point3 DPObjRelBox () { return sc->DPObjRelBox(); }
	inline void ScreenUV (Point2& uv, Point2 &duv); // Overridden
	IPoint2 ScreenCoord () { return sc->ScreenCoord(); }
	Point2 SurfacePtScreen (){ return sc->SurfacePtScreen(); }
	inline Point3 UVW (int channel); // Overridden
	Point3 DUVW (int channel) { return sc->DUVW(channel); }
	void DPdUVW (Point3 dP[3], int channel){ sc->DPdUVW(dP, channel); }
	int BumpBasisVectors (Point3 dP[2], int axis, int channel){ return sc->BumpBasisVectors(dP, axis, channel); }
	BOOL IsSuperSampleOn () { return sc->IsSuperSampleOn(); }
	BOOL IsTextureSuperSampleOn () { return sc->IsTextureSuperSampleOn(); }
	int GetNSuperSample () { return sc->GetNSuperSample(); }
	float GetSampleSizeScale (){ return sc->GetSampleSizeScale(); }
	Point3 UVWNormal (int channel){ return sc->UVWNormal(channel); }
	float RayDiam (){ return sc->RayDiam(); }
	float RayConeAngle (){ return sc->RayConeAngle(); }
	AColor EvalEnvironMap (Texmap* map, Point3 view) { return sc->EvalEnvironMap(map, view); }
	void GetBGColor (Color& bgcol, Color& transp, BOOL fogBG){ sc->GetBGColor(bgcol, transp, fogBG); }
	float CamNearRange () { return sc->CamNearRange(); }
	float CamFarRange (){ return sc->CamFarRange(); }
	Point3 PointTo (const Point3& p, RefFrame ito) { return sc->PointTo(p, ito); }
	Point3 PointFrom (const Point3& p, RefFrame ifrom) { return sc->PointFrom(p, ifrom); }
	Point3 VectorTo (const Point3& p, RefFrame ito) { return sc->VectorTo(p, ito); }
	Point3 VectorFrom (const Point3& p, RefFrame ifrom) { return sc->VectorFrom(p, ifrom); }
	Point3 VectorToNoScale (const Point3& p, RefFrame ito) { return sc->VectorToNoScale(p, ito); }
	Point3 VectorFromNoScale (const Point3& p, RefFrame ifrom){ return sc->VectorFromNoScale(p, ifrom); }
	void SetGBufferID (int gbid){ sc->SetGBufferID(gbid); }
	FILE* DebugFile (){ return sc->DebugFile(); }
	AColor EvalGlobalEnvironMap (Point3 dir){ return sc->EvalGlobalEnvironMap(dir); }
	BOOL GetCache (Texmap* map, AColor &c) { return sc->GetCache(map, c); }
	BOOL GetCache (Texmap* map, float &f) { return sc->GetCache(map, f); }
	BOOL GetCache (Texmap* map, Point3 &p) { return sc->GetCache(map, p); }
	void PutCache (Texmap* map, const AColor &c) { sc->PutCache(map, c); }
	void PutCache (Texmap* map, const float f) { sc->PutCache(map, f); }
	void PutCache (Texmap* map, const Point3 &p) { sc->PutCache(map, p); }
	void TossCache (Texmap* map){ sc->TossCache(map); }
	INT_PTR Execute (int cmd, ULONG_PTR arg1, ULONG_PTR arg2, ULONG_PTR arg3){ return sc->Execute(cmd, arg1, arg2, arg3); }
	LightDesc* GetAtmosSkipLight () { return sc->GetAtmosSkipLight(); }
	void SetAtmosSkipLight (LightDesc* lt){ sc->SetAtmosSkipLight(lt); }
	int NRenderElements () { return sc->NRenderElements(); }
	IRenderElement* GetRenderElement (int n){ return sc->GetRenderElement(n); }
	Color DiffuseIllum (){ return sc->DiffuseIllum(); }

#if MAX_RELEASE < MAX_RELEASE_R24_PREVIEW
	bool IsPhysicalSpace () const{ return sc->IsPhysicalSpace(); }
	template<class T> void ScaledToRGB (T& energy) const { sc->ScaledToRGB(energy); }
	float ScaledToRGB (float energy) const{ return sc->ScaledToRGB(energy); }
	void ScaledToRGB (){ sc->ScaledToRGB(); }
	template<class T> void ScalePhysical (T& energy) const { sc->ScaledPhysical(energy); }
	float ScalePhysical (float energy) const{ return sc->ScalePhysical(energy); }
	template<class T> void ScaleRGB (T& energy) const { sc->ScaleRGB(energy); }
	float ScaleRGB (float energy) const{ return sc->ScaleRGB(energy); }
#endif

#if MAX_RELEASE >= MAX_RELEASE_R26_PREVIEW
	Matrix3 MatrixTo(RefFrame ito) override { return sc->MatrixTo(ito); }

	Matrix3 MatrixFrom(RefFrame ifrom) override { return sc->MatrixFrom(ifrom); }
#endif
};

inline Point3 BerconSC::PObjRelBox() {
	return sc->PObjRelBox();
}

inline void BerconSC::ScreenUV(Point2& uv, Point2 &duv) {
	sc->ScreenUV(uv, duv);
	if (type == 4) {
		uv.x = uvPoint.x;
		uv.y = uvPoint.y;
	}
}

inline Point3 BerconSC::P() {
	return type == 3 ? uvPoint : sc->P();
}

inline Point3 BerconSC::PObj() {
	return type == 2 ? uvPoint : sc->PObj();	
}

inline Point3 BerconSC::UVW(int channel) {
	if (useMultiTexture && 99 == channel)
		return Point3(val, val, val);

	if ((type == 0 || type == 1) && uvChannel == channel)
		return uvPoint;

	if (uvChannel > 0 && uvChannel == channel)
		return uvPoint;
	
	if (uvChannel2 > 0 && uvChannel2 == channel)
		return uvPoint2;
	
	return sc->UVW(channel);
}