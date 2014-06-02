// Copyright (c) 2008 Jerry Ylilammi
// All rights reserved.

/*
	Extra map interface that should be filled by all my maps that support extra features.
	
	Features:
		mapID		Similar to mtlNum but aimed for maps not materials

*/

#pragma once

#include "BerconNoise.h"

class BerconMap : public Texmap {
public:
	virtual RGBA BerconEvalColor(ShadeContext& sc, int mapID) = 0;
	virtual float BerconEvalMono(ShadeContext& sc, int mapID) = 0;
	virtual Point3 BerconEvalNormalPerturb(ShadeContext& sc, int mapID) = 0;
};