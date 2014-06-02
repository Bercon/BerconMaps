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
#include "fractal.h"

void Noise::alterUVW(Point3& p, int type) {
	switch (type) {
		case 1: 
			p.x = sqrt(p.x * p.x + p.y * p.y);
			p.y = 0.f;
			break;
		case 2:	
			p.x = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
			p.y = 0.f;			
			p.z = 0.f;
			break;
	}
}

float Noise::noise(Point3 p, NoiseParams& np) {
	switch (np.noiseFunction) {
		case 0: return Perlin::noise(p.x, p.y);
		case 1: return Perlin::noise(p.x, p.y, p.z);
		case 2: return Perlin::noise(p.x, p.y, p.z, np.phase);		
		case 3: return Perlin::snoise(p.x, p.y);
		case 4: return Perlin::snoise(p.x, p.y, p.z);
		case 5: return Perlin::snoise(p.x, p.y, p.z, np.phase);		
		case 6: return worleyWrapper(p, np);
		default: return 0.0f;
	}
}

float Noise::noise(Point3 p, float d, NoiseParams& np) {
	switch (np.noiseFunction) {
		case 0: return Perlin::fnoise2D(p.x, p.y, d);
		case 1: return Perlin::fnoise3D(p.x, p.y, p.z, d);
		case 2: return Perlin::fnoise4D(p.x, p.y, p.z, np.phase, d);		
		case 3: return Perlin::fsnoise2D(p.x, p.y, d);
		case 4: return Perlin::fsnoise3D(p.x, p.y, p.z, d);
		case 5: return Perlin::fsnoise4D(p.x, p.y, p.z, np.phase, d);		
		case 6: return worleyWrapper(p, np);
		default: return 0.0f;
	}
}

float Noise::worleyWrapper(Point3 p, NoiseParams& np) {	
	float val;
	double p3[3] = {(double)p.x, (double)p.y, (double)p.z};
	double f[4];
	int order = 1;
	// Calc order
	if (np.F4 > 0.0001f || np.F4 < -0.0001f)
		order = 4;
	else if (np.F3 > 0.0001f || np.F3 < -0.0001f)
		order = 3;
	else if (np.F2 > 0.0001f || np.F2 < -0.0001f)
		order = 2;
	// Worley
	Worley::noise(p3, order, f, np.worleyFunction);	
	// Return value
	switch (order) {
		case 1: val = (float)f[0]*np.F1; break;
		case 2: val = (float)f[0]*np.F1 + (float)f[1]*np.F2; break;
		case 3: val = (float)f[0]*np.F1 + (float)f[1]*np.F2 + (float)f[2]*np.F3; break;
		case 4: val = (float)f[0]*np.F1 + (float)f[1]*np.F2 + (float)f[2]*np.F3 + (float)f[3]*np.F4; break;
		default: val = 0.0f;
	}
	val = val/np.spread*2.f-1.f;
	return val;
}
 
float Noise::wood(Point3 p, Point3 &g, WoodParam &wp) {
	// Radial noise
	p.x += Perlin::snoise(p.x*wp.radialFreq, p.y*wp.radialFreq, p.z*wp.radialZ, wp.randSeed-1.125f) * wp.radialStr;
	p.y += Perlin::snoise(p.x*wp.radialFreq, p.y*wp.radialFreq, p.z*wp.radialZ, wp.randSeed+1.125f) * wp.radialStr;
	g = p; // Store radial component for gain
	// Trunk wobble		
	float angle = Perlin::snoise(p.z*wp.trunkFreq, wp.randSeed+3.f) * (float)pi; // Offset so we dont get the same value as below
	p += Point3(cos(angle), sin(angle), 0.f) * Perlin::snoise(p.z*wp.trunkFreq, wp.randSeed-5.5f) * wp.trunkStr;		
	g = .5f*g + .5f*p; // We don't want trunk wobble to affect too much to grain
	// Distance
	float dist = 0.f;
	switch (wp.woodType) {
		case 0: dist = sqrt(p.x*p.x+p.y*p.y); break;
		case 1: p*=.05f; dist = (Perlin::noise(p.x, p.y, p.z, wp.randSeed-7.1f)+1.f) / .5f * 15.f; break;
		case 2: p*=.05f; dist = (Perlin::snoise(p.x, p.y, p.z, wp.randSeed+3.15f)+1.f) / .5f * 15.f; break;
		case 3: dist = p.x<0.f?-p.x:p.x; break;
	}
	// Width variation
	dist += Perlin::snoise(dist+wp.randSeed*2.f) * wp.widthVar;
	// Angular noise
	float d = dist;
	if (d > wp.angleRad) d = wp.angleRad;	
	dist += smooth(d/wp.angleRad) * Perlin::noise(p.x*wp.angleFreq, p.y*wp.angleFreq, p.z*wp.angleFreq*.5f, wp.randSeed+10.6f) * wp.angleStr;
	// Mod
	float ipart = (float)(int)dist;		
	dist -= ipart;
	// Skew
	if (dist < wp.skew)
		dist *= .5f / wp.skew;
	else
		dist = dist * .5f / (1.f-wp.skew) - wp.skew * (.5f/(1.f-wp.skew)) + .5f;	
	// Reverse
	dist *= 2.f;
	if (dist > 1.f)
		dist = 2.f-dist;
	// Smooth and scale
	dist = smooth(dist, wp.lowTresh, wp.highTresh);	
	// Gain variation		
	float gain = (Perlin::snoise((ipart + wp.randSeed) * 314.134f) + 1.f) * .5f;
	dist *= (1.f-wp.gainVar) + gain * wp.gainVar;
	// Return	
	return dist;
}

/*
	This version super samples the mod function to avoid aliasing	
	Filtered versions of Noises are used, meaning they fade out once their frequency is too high compared to sampled area
	Noises are also asumed constant in the sampled area, meaning they are not super sampled, just calculated once and filtered
	With the optimizations above this filtered version shouldn't result in any significan performance loss

	2 x Filtered 4D Simplex
	2 x Filtered 2D Simplex
	1 x Filtered 4D Perlin
	samples x samples x 2 x 1D Simplex 
*/

float Noise::wood(Point3 p, Point3 dPdx, Point3 dPdy, Point3 &g, WoodParam wp) {
	// Filtered noises are only intrested in maximum change in X, Y and Z axis.
	float lx = dPdx.Length();
	float ly = dPdy.Length();
	float filter = MAX(lx, ly);
	// Radial noise
	p.x += Perlin::fsnoise4D(p.x*wp.radialFreq, p.y*wp.radialFreq, p.z*wp.radialZ, wp.randSeed-1.125f, filter*wp.radialFreq) * wp.radialStr;
	p.y += Perlin::fsnoise4D(p.x*wp.radialFreq, p.y*wp.radialFreq, p.z*wp.radialZ, wp.randSeed+1.125f, filter*wp.radialFreq) * wp.radialStr;
	g = p; // Store radial component for gain
	// Trunk wobble		
	float angle = Perlin::fsnoise2D(p.z*wp.trunkFreq, wp.randSeed+3.f, filter*wp.trunkFreq) * (float)pi;
	p += Point3(cos(angle), sin(angle), 0.f) * Perlin::fsnoise2D(p.z*wp.trunkFreq, wp.randSeed-5.5f, filter*wp.trunkFreq) * wp.trunkStr;		
	g = .5f*g + .5f*p; // We don't want trunk wobble to affect too much to grain
	// Angular noise (applied inside super sampled code, but not dependant on it)
	float angNoise = Perlin::fnoise4D(p.x*wp.angleFreq, p.y*wp.angleFreq, p.z*wp.angleFreq*.5f, wp.randSeed+10.6f, filter*wp.angleFreq);	
	// Super sampling the mod function and functions dependant on it starts here
	float samplesf = (float)wp.samples;
	float total = 0.f;
	Point3 fp = p - dPdx * .5f - dPdy * .5f;
	for (int i=0; i<wp.samples; i++) for (int j=0; j<wp.samples; j++) {
		// Compute sample location
		p = fp + dPdx * (((float)i + .5f) / samplesf) + dPdy * (((float)j + .5f) / samplesf);
		// Distance	
		float dist = 0.f;	
		switch (wp.woodType) {
			case 0: dist = sqrt(p.x*p.x+p.y*p.y); break;
			case 1: p*=.05f; dist = (Perlin::noise(p.x, p.y, p.z, wp.randSeed-7.1f)+1.f) / .5f * 15.f; break;
			case 2: p*=.05f; dist = (Perlin::snoise(p.x, p.y, p.z, wp.randSeed+3.15f)+1.f) / .5f * 15.f; break;
			case 3: dist = p.x<0.f?-p.x:p.x; break;
		}
		// Width variation
		dist += Perlin::snoise(dist+wp.randSeed*2.f) * wp.widthVar; // We need FAST and continous random function here, 1D Simplex does fairly well
		// Angular noise
		float d = dist;
		if (d > wp.angleRad) d = wp.angleRad;	
		dist += SMOOTH(d/wp.angleRad) * angNoise * wp.angleStr;
		// Mod
		float ipart = (float)(int)dist;		
		dist -= ipart;
		// Skew
		if (dist < wp.skew)
			dist *= .5f / wp.skew;
		else
			dist = dist * .5f / (1.f-wp.skew) - wp.skew * (.5f/(1.f-wp.skew)) + .5f;	
		// Reverse
		dist *= 2.f;
		if (dist > 1.f)
			dist = 2.f - dist;
		// Smooth and scale
		dist = smooth(dist, wp.lowTresh, wp.highTresh);	
		// Gain variation				
		float gain = (Perlin::snoise((ipart + wp.randSeed) * 314.134f) + 1.f) * .5f; // Again we need FAST random function, in this case it doesn't have to be continous		
		dist *= (1.f-wp.gainVar) + gain * wp.gainVar;
		// Add to total value
	 	total += dist;
	}
	// Return final value, just need to divide sum with number of samples taken
	return total / (float)(wp.samples * wp.samples);
}

float Noise::limitedNoise(Point3 p, NoiseParams &np) {
	float res = Fractal::f(p, np);	
	if (np.low<np.high) res = threshold(res,np.low,np.high);		
	if (res < 0) return 0.0f;
	if (res > 1) return 1.0f;
	return res;
}

float Noise::limitedNoise(Point3 p, Point3 dpdx, Point3 dpdy, NoiseParams &np) {
	float lx = dpdx.Length();
	float ly = dpdy.Length();
	lx = MAX(lx, ly);
	float res = Fractal::f(p, lx, np);	
	if (np.low<np.high) res = threshold(res,np.low,np.high);		
	if (res < 0) return 0.0f;
	if (res > 1) return 1.0f;
	return res;
}