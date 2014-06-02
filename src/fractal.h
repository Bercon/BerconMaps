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

#include "noise.h"

class Fractal {
public:
    Fractal() {}
    ~Fractal() {}

	static float fractal(Point3 p, NoiseParams& np);
	static float turbulence(Point3 p, NoiseParams& np);	
	static float fBm(Point3 p, NoiseParams& np);
	static float fBmTurb(Point3 p, NoiseParams& np);
	static float heteroTerrain(Point3 p, NoiseParams& np);
	static float hybridMultiFractal(Point3 p, NoiseParams& np);
	static float ridgedMultiFractal(Point3 p, NoiseParams& np);

	static float fractal(Point3 p, float d, NoiseParams& np);
	static float turbulence(Point3 p, float d, NoiseParams& np);	
	static float fBm(Point3 p, float d, NoiseParams& np);
	static float fBmTurb(Point3 p, float d, NoiseParams& np);
	static float heteroTerrain(Point3 p, float d, NoiseParams& np);
	static float hybridMultiFractal(Point3 p, float d, NoiseParams& np);
	static float ridgedMultiFractal(Point3 p, float d, NoiseParams& np);

	static float marble(Point3 p, float d, NoiseParams& np);

	// Generic 
	static float fBm(Point3 p, float levels, float lacunarity, float H);
	// Grain
	static float grain(Point3 p, float amount, float freq);

	static float f(Point3 p, NoiseParams &np) {
		switch (np.fractalFunction) {
			case 0: return (1.0f + Noise::noise(p, np)) * .5f;
			case 1: return fractal(p, np);
			case 2: return turbulence(p, np);
			case 3: return fBm(p, np);
			case 4: return fBmTurb(p, np);
			case 5: return heteroTerrain(p, np);
			case 6: return hybridMultiFractal(p, np);
			case 7: return ridgedMultiFractal(p, np);
		}
		return 0.f;
	}

	static float f(Point3 p, float d, NoiseParams &np) {
		switch (np.fractalFunction) {
			case 0: return (1.0f + Noise::noise(p, d, np)) * .5f;
			case 1: return fractal(p, d, np);
			case 2: return turbulence(p, d, np);
			case 3: return fBm(p, d, np);
			case 4: return fBmTurb(p, d, np);
			case 5: return heteroTerrain(p, np);
			case 6: return hybridMultiFractal(p, d, np);
			case 7: return ridgedMultiFractal(p, d, np);
		}
		return 0.f;
	}
};
/*
float BerconNoise::noise(Point3 p, Point3 dp, NoiseParams &np) {
	float res;
	switch (noiseF.type) {
    	case 0:
			res = (1.0f + noiseF.noise(p, dp, np)) * .5f;
			break;
		case 1:
			res = Fractal::fractal(noiseF, p, dp, np);
			break;
		case 2:
			res = Fractal::turbulence(noiseF, p, dp, np);
			break;	
		case 3:
			res = Fractal::fBm(noiseF, p, dp, np);
			break;	
		case 4:
			res = Fractal::heteroTerrain(noiseF, p, dp, np);
			break;	
		case 5:
			res = Fractal::hybridMultiFractal(noiseF, p, dp, np);
			break;	
		case 6:
			res = Fractal::ridgedMultiFractal(noiseF, p, dp, np);
			break;	
		case 7:
			res = Fractal::fBmTurb(noiseF, p, dp, np);
			break;	
		default: res = 1.0f;
	}
	return res;
}
};*/