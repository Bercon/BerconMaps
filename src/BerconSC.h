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

#include "max.h"

#define BERCON_SHADE_CONTEXT_CLASS_ID Class_ID(0x7c0a38f1, 0x2f1a67f2)

class BerconSC : public ShadeContext {
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
	explicit BerconSC(ShadeContext* const sc) : sc(sc), val(0)
	{
		mode = sc->mode;												// Line 370 of ShadeContext.h
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

		uvChannel = -1;
		uvChannel2 = -1;
		useMultiTexture = false;
		type = -1;
	}

	void setParams(Point3 p, int type, int chan = 0) {
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

	void ResetOutput(int n) { sc->ResetOutput(n); }
	Class_ID ClassID() { return BERCON_SHADE_CONTEXT_CLASS_ID; }
	virtual BOOL InMtlEditor() { return sc->InMtlEditor(); }
	int Antialias() override { return sc->Antialias(); }
	virtual int ProjType() { return sc->ProjType(); }
	virtual LightDesc* Light(int n) { return sc->Light(n); }
	TimeValue CurTime() override { return sc->CurTime(); }
	virtual int NodeID() { return sc->NodeID(); }
	virtual INode* Node() { return sc->Node(); }
	virtual Object* GetEvalObject() { return sc->GetEvalObject(); }
	Point3 BarycentricCoords() override { return sc->BarycentricCoords(); }
	virtual int FaceNumber() { return sc->FaceNumber(); }
	virtual Point3 Normal() { return sc->Normal(); }
	void SetNormal(Point3 p) { sc->SetNormal(p); }
	virtual Point3 OrigNormal() { return sc->OrigNormal(); }
	virtual Point3 GNormal() { return sc->GNormal(); }
	virtual float Curve() { return sc->Curve(); }
	virtual Point3 V() { return sc->V(); }
	void SetView(Point3 p) { sc->SetView(p); }
	virtual Point3 OrigView() { return sc->OrigView(); }
	virtual Point3 ReflectVector() { return sc->ReflectVector(); }
	virtual Point3 RefractVector(float ior) { return sc->RefractVector(ior); }
	void SetIOR(float ior) { sc->SetIOR(ior); }
	float GetIOR() { return sc->GetIOR(); }
	virtual Point3 CamPos() { return sc->CamPos(); }
	Point3 P() override; // Overridden
	Point3 DP() override { return sc->DP(); }
	void DP(Point3& dpdx, Point3& dpdy) override { sc->DP(dpdx, dpdy); }		// error fixed May 2019
	Point3 PObj() override; // Overridden
	Point3 DPObj() override { return sc->DPObj(); }
	virtual Box3 ObjectBox() { return sc->ObjectBox(); }
	inline Point3 PObjRelBox() override; // Overridden
	Point3 DPObjRelBox() { return sc->DPObjRelBox(); }
	inline void ScreenUV(Point2& uv, Point2& duv) override; // Overridden		[why are some of these 'overridden' instead of virtual? I ask 'cuz I suck at C++]
	virtual IPoint2 ScreenCoord() { return sc->ScreenCoord(); }
	virtual Point2 SurfacePtScreen() { return sc->SurfacePtScreen(); }
	inline Point3 UVW(int channel) override; // Overridden
	Point3 DUVW(int channel) { return sc->DUVW(channel); }
	void DPdUVW(Point3 dP[3], int channel) { sc->DPdUVW(dP, channel); }
	int BumpBasisVectors(Point3 dP[2], int axis, int channel) { return sc->BumpBasisVectors(dP, axis, channel); }
	BOOL IsSuperSampleOn() { return sc->IsSuperSampleOn(); }
	BOOL IsTextureSuperSampleOn() { return sc->IsTextureSuperSampleOn(); }
	int GetNSuperSample() { return sc->GetNSuperSample(); }
	float GetSampleSizeScale() { return sc->GetSampleSizeScale(); }
	Point3 UVWNormal(int channel) { return sc->UVWNormal(channel); }
	float RayDiam() { return sc->RayDiam(); }
	float RayConeAngle() { return sc->RayConeAngle(); }
	AColor EvalEnvironMap(Texmap* map, Point3 view) { return sc->EvalEnvironMap(map, view); }
	void GetBGColor(Color& bgcol, Color& transp, BOOL fogBG) { sc->GetBGColor(bgcol, transp, fogBG); }
	virtual float CamNearRange() { return sc->CamNearRange(); }
	virtual float CamFarRange() { return sc->CamFarRange(); }
	virtual Point3 PointTo(const Point3& p, RefFrame ito) { return sc->PointTo(p, ito); }
	virtual Point3 PointFrom(const Point3& p, RefFrame ifrom) { return sc->PointFrom(p, ifrom); }
	virtual Point3 VectorTo(const Point3& p, RefFrame ito) { return sc->VectorTo(p, ito); }
	virtual Point3 VectorFrom(const Point3& p, RefFrame ifrom) { return sc->VectorFrom(p, ifrom); }
	virtual Point3 VectorToNoScale(const Point3& p, RefFrame ito) { return sc->VectorToNoScale(p, ito); }
	virtual Point3 VectorFromNoScale(const Point3& p, RefFrame ifrom) { return sc->VectorFromNoScale(p, ifrom); }
	virtual void SetGBufferID(int gbid) { sc->SetGBufferID(gbid); }
	virtual FILE* DebugFile() { return 0; }																//	This is only used internally by ShadeContext anyway. But virtualize this because SC is virtual.
	virtual AColor EvalGlobalEnvironMap(Point3 dir) { return sc->EvalGlobalEnvironMap(dir); }
	virtual BOOL GetCache(Texmap* map, AColor& c) { return sc->GetCache(map, c); }
	virtual BOOL GetCache(Texmap* map, float& f) { return sc->GetCache(map, f); }
	virtual BOOL GetCache(Texmap* map, Point3& p) { return sc->GetCache(map, p); }
	virtual void PutCache(Texmap* map, const AColor& c) { sc->PutCache(map, c); }
	virtual void PutCache(Texmap* map, const float f) { sc->PutCache(map, f); }
	virtual void PutCache(Texmap* map, const Point3& p) { sc->PutCache(map, p); }
	virtual void TossCache(Texmap* map) { sc->TossCache(map); }
	virtual INT_PTR Execute(int cmd, ULONG_PTR arg1, ULONG_PTR arg2, ULONG_PTR arg3)
	{
		return sc->Execute(cmd, arg1, arg2, arg3);
	}

	LightDesc* GetAtmosSkipLight() { return sc->GetAtmosSkipLight(); }
	void SetAtmosSkipLight(LightDesc* lt) { sc->SetAtmosSkipLight(lt); }
	int NRenderElements() { return sc->NRenderElements(); }
	virtual IRenderElement* GetRenderElement(int n) { return sc->GetRenderElement(n); }
	Color DiffuseIllum() { return sc->DiffuseIllum(); }

	// REVISED METHODS OF CALLING GLOBCONTEXT AND TONEOPERATOR FOR THESE FUNCTIONS

	virtual ToneOperator* IsPhysicalSpace() const
	{
		if (globContext->pToneOp) return sc->globContext->pToneOp;
		else return NULL;
	}

	template<class T> void ScaledToRGB(T& /*energy*/) const { sc->globContext->pToneOp; }

	virtual ToneOperator* ScaledToRGB(float /*energy*/) const 
	{
		if (globContext->pToneOp) return sc->globContext->pToneOp;
		else return NULL;
	}

	virtual ToneOperator* ScaledToRGB() const
	{
		if (globContext->pToneOp) return sc->globContext->pToneOp;
		else return NULL;
	}

	template<class T> void ScalePhysical(T& /*energy*/) const { sc->globContext->pToneOp; }
	ToneOperator* ScalePhysical(float /*energy*/) const 
	{
		if (globContext->pToneOp) return sc->globContext->pToneOp;
		else return NULL;
	}

	template<class T> void ScaleRGB(T& /*energy*/) const { sc->globContext->pToneOp; }

	virtual ToneOperator* ScaleRGB(float /*energy*/) const 
	{
		if (globContext->pToneOp) return sc->globContext->pToneOp;
		else return NULL;
	}

	/*	THIS SECTION IS DEPRECATED AND WILL THROW COMPILER ERROR C4996
	bool IsPhysicalSpace () const{ return sc->IsPhysicalSpace(); }
	template<class T> void ScaledToRGB (T& energy) const { sc->ScaledToRGB(energy); }
	float ScaledToRGB (float energy) const{ return sc->ScaledToRGB(energy); }
	void ScaledToRGB (){ sc->ScaledToRGB(); }
	template<class T> void ScalePhysical (T& energy) const { sc->ScaledPhysical(energy); }
	float ScalePhysical (float energy) const{ return sc->ScalePhysical(energy); }
	template<class T> void ScaleRGB (T& energy) const { sc->ScaleRGB(energy); }
	float ScaleRGB (float energy) const{ return sc->ScaleRGB(energy); }
};
	*/
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