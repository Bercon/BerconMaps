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

#include "Max.h"

#include "texutil.h"

#include "worley.h"
#include "perlin.h"

#include "commonMath.h"

class NoiseParams {
public:
	int noiseFunction;	
	int fractalFunction;
	int worleyFunction;

	float phase;
	float spread;
	float F1, F2, F3, F4;

	float levels;
	float H;
	float lacunarity;
	float offset;
	float gain;

	float smoothWidth;

	float low, high;
};

class WoodParam { // Wood parameters
public:
	int woodType;
	float randSeed;
	int samples;

	float lowTresh;
	float highTresh;
	float skew;
	float widthVar;
	float gainVar;
	
	float trunkStr;	
	float trunkFreq;
	
	float radialStr;
	float radialFreq;
	float radialZ;

	float angleStr;
	float angleFreq;
	float angleRad;
};

class Noise {
public:
    Noise() {}
    ~Noise() {}

	// The main noise function
	static float noise(Point3 p, NoiseParams& np);
	static float noise(Point3 p, float d, NoiseParams& np);

	// Wrapper for worley
	static float worleyWrapper(Point3 p, NoiseParams& np);

	// Wood function
	static float wood(Point3 p, Point3 &g, WoodParam& wp);	
	static float wood(Point3 p, Point3 dPdx, Point3 dPdy, Point3 &g, WoodParam wp); // Filtered version

	// UVW
	static void alterUVW(Point3& p, int type);

	// Cell noise
	static float cellNoise(float x, float y, float z) {
		int xi = FASTFLOOR(x);
		int yi = FASTFLOOR(y);
		int zi = FASTFLOOR(z);
		unsigned int n = xi + yi*1301 + zi*314159;
		n ^= (n<<13);
		return ((float)(n*(n*n*15731 + 789221) + 1376312589) / 4294967296.0f);
	}

	static float limitedNoise(Point3 p, NoiseParams &np);
	static float limitedNoise(Point3 p, Point3 dpdx, Point3 dpdy, NoiseParams &np);
};

