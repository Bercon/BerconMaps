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
	ShadeContext* const sc_;			// added underscore to keep these straight (May 8 2019)

	int type;
	Point3 uvPoint;
	Point3 uvPoint2;
	int uvChannel;
	int uvChannel2;
	bool useMultiTexture;
	float val;

public:
	explicit BerconSC(ShadeContext* const sc) : sc_(sc), val(0)
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

	void ResetOutput(int n) { sc_->ResetOutput(n); }
	Class_ID ClassID() { return BERCON_SHADE_CONTEXT_CLASS_ID; }
	virtual BOOL InMtlEditor() { return sc_->InMtlEditor(); }
	virtual int Antialias() { return sc_->Antialias(); }
	virtual int ProjType() { return sc_->ProjType(); }
	virtual LightDesc* Light(int n) { return sc_->Light(n); }
	virtual TimeValue CurTime() { return sc_->CurTime(); }
	virtual int NodeID() { return sc_->NodeID(); }
	virtual INode* Node() { return sc_->Node(); }
	virtual Object* GetEvalObject() { return sc_->GetEvalObject(); }
	virtual Point3 BarycentricCoords() { return sc_->BarycentricCoords(); }
	virtual int FaceNumber() { return sc_->FaceNumber(); }
	virtual Point3 Normal() { return sc_->Normal(); }
	void SetNormal(Point3 p) { sc_->SetNormal(p); }
	virtual Point3 OrigNormal() { return sc_->OrigNormal(); }
	virtual Point3 GNormal() { return sc_->GNormal(); }
	virtual float Curve() { return sc_->Curve(); }
	virtual Point3 V() { return sc_->V(); }
	void SetView(Point3 p) { sc_->SetView(p); }
	virtual Point3 OrigView() { return sc_->OrigView(); }
	virtual Point3 ReflectVector() { return sc_->ReflectVector(); }
	virtual Point3 RefractVector(float ior) { return sc_->RefractVector(ior); }
	void SetIOR(float ior) { sc_->SetIOR(ior); }
	float GetIOR() { return sc_->GetIOR(); }
	virtual Point3 CamPos() { return sc_->CamPos(); }
	virtual Point3 P(); // Overridden
	virtual Point3 DP() { return sc_->DP(); }
	void DP(Point3& dpdx, Point3& dpdy) { sc_->DP(dpdx, dpdx); }
	Point3 PObj() override; // Overridden
	virtual Point3 DPObj() { return sc_->DPObj(); }
	virtual Box3 ObjectBox() { return sc_->ObjectBox(); }
	inline Point3 PObjRelBox() override; // Overridden
	Point3 DPObjRelBox() { return sc_->DPObjRelBox(); }
	inline void ScreenUV(Point2& uv, Point2& duv) override; // Overridden		[why are some of these 'overridden' instead of virtual?]
	virtual IPoint2 ScreenCoord() { return sc_->ScreenCoord(); }
	virtual Point2 SurfacePtScreen() { return sc_->SurfacePtScreen(); }
	inline Point3 UVW(int channel) override; // Overridden
	Point3 DUVW(int channel) { return sc_->DUVW(channel); }
	void DPdUVW(Point3 dP[3], int channel) { sc_->DPdUVW(dP, channel); }
	int BumpBasisVectors(Point3 dP[2], int axis, int channel) { return sc_->BumpBasisVectors(dP, axis, channel); }
	BOOL IsSuperSampleOn() { return sc_->IsSuperSampleOn(); }
	BOOL IsTextureSuperSampleOn() { return sc_->IsTextureSuperSampleOn(); }
	int GetNSuperSample() { return sc_->GetNSuperSample(); }
	float GetSampleSizeScale() { return sc_->GetSampleSizeScale(); }
	Point3 UVWNormal(int channel) { return sc_->UVWNormal(channel); }
	float RayDiam() { return sc_->RayDiam(); }
	float RayConeAngle() { return sc_->RayConeAngle(); }
	AColor EvalEnvironMap(Texmap* map, Point3 view) { return sc_->EvalEnvironMap(map, view); }
	void GetBGColor(Color& bgcol, Color& transp, BOOL fogBG) { sc_->GetBGColor(bgcol, transp, fogBG); }
	virtual float CamNearRange() { return sc_->CamNearRange(); }
	virtual float CamFarRange() { return sc_->CamFarRange(); }
	virtual Point3 PointTo(const Point3& p, RefFrame ito) { return sc_->PointTo(p, ito); }
	virtual Point3 PointFrom(const Point3& p, RefFrame ifrom) { return sc_->PointFrom(p, ifrom); }
	virtual Point3 VectorTo(const Point3& p, RefFrame ito) { return sc_->VectorTo(p, ito); }
	virtual Point3 VectorFrom(const Point3& p, RefFrame ifrom) { return sc_->VectorFrom(p, ifrom); }
	virtual Point3 VectorToNoScale(const Point3& p, RefFrame ito) { return sc_->VectorToNoScale(p, ito); }
	virtual Point3 VectorFromNoScale(const Point3& p, RefFrame ifrom) { return sc_->VectorFromNoScale(p, ifrom); }
	virtual void SetGBufferID(int gbid) { sc_->SetGBufferID(gbid); }
	virtual FILE* DebugFile() { return 0; }																//	This is only used internally by ShadeContext anyway. But virtualize this because SC is virtual.
	virtual AColor EvalGlobalEnvironMap(Point3 dir) { return sc_->EvalGlobalEnvironMap(dir); }
	virtual BOOL GetCache(Texmap* map, AColor& c) { return sc_->GetCache(map, c); }
	virtual BOOL GetCache(Texmap* map, float& f) { return sc_->GetCache(map, f); }
	virtual BOOL GetCache(Texmap* map, Point3& p) { return sc_->GetCache(map, p); }
	virtual void PutCache(Texmap* map, const AColor& c) { sc_->PutCache(map, c); }
	virtual void PutCache(Texmap* map, const float f) { sc_->PutCache(map, f); }
	virtual void PutCache(Texmap* map, const Point3& p) { sc_->PutCache(map, p); }
	virtual void TossCache(Texmap* map) { sc_->TossCache(map); }
	virtual INT_PTR Execute(int cmd, ULONG_PTR arg1, ULONG_PTR arg2, ULONG_PTR arg3)
	{
		return sc_->Execute(cmd, arg1, arg2, arg3);
	}

	LightDesc* GetAtmosSkipLight() { return sc_->GetAtmosSkipLight(); }
	void SetAtmosSkipLight(LightDesc* lt) { sc_->SetAtmosSkipLight(lt); }
	int NRenderElements() { return sc_->NRenderElements(); }
	virtual IRenderElement* GetRenderElement(int n) { return sc_->GetRenderElement(n); }
	Color DiffuseIllum() { return sc_->DiffuseIllum(); }

	// REVISED METHODS OF CALLING GLOBCONTEXT AND TONEOPERATOR FOR THESE FUNCTIONS

	virtual ToneOperator* IsPhysicalSpace() const
	{
		if (globContext->pToneOp) return sc_->globContext->pToneOp;
		else return NULL;
	}

	template<class T> void ScaledToRGB(T& energy) const { sc_->globContext->pToneOp; }

	virtual ToneOperator* ScaledToRGB(float energy) const 
	{
		if (globContext->pToneOp) return sc_->globContext->pToneOp;
		else return NULL;
	}

	virtual ToneOperator* ScaledToRGB() const { sc_->globContext->pToneOp; }

	template<class T> void ScalePhysical(T& energy) const { sc_->globContext->pToneOp; }
	ToneOperator* ScalePhysical(float energy) const 
	{
		if (globContext->pToneOp) return sc_->globContext->pToneOp;
		else return NULL;
	}

	template<class T> void ScaleRGB(T& energy) const { sc_->globContext->pToneOp; }

	virtual ToneOperator* ScaleRGB(float energy) const 
	{
		if (globContext->pToneOp) return sc_->globContext->pToneOp;
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
	return sc_->PObjRelBox();
}

	inline void BerconSC::ScreenUV(Point2& uv, Point2 &duv) {
	sc_->ScreenUV(uv, duv);
	if (type == 4) {
		uv.x = uvPoint.x;
		uv.y = uvPoint.y;
	}
}

inline Point3 BerconSC::P() {
	return type == 3 ? uvPoint : sc_->P();
}

inline Point3 BerconSC::PObj() {
	return type == 2 ? uvPoint : sc_->PObj();	
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
	
	return sc_->UVW(channel);
}