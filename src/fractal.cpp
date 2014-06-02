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

#include "fractal.h"

float Fractal::fractal(Point3 p, NoiseParams& np) {
	float sum = 0.0f;
	float l, f = 1.0f;
	for (l = np.levels; l >= 1.0f; l-=1.0f) {				
		sum += Noise::noise(p*f, np)/f;
		f *= 2.0f;
	}	
	if (l>0.0f)				
		sum += l*Noise::noise(p*f, np)/f;				
	return 0.5f*(sum+1.0f);
}

float Fractal::turbulence(Point3 p, NoiseParams& np) {
	float sum = 0.0f;
	float l,f = 1.0f;	
	for (l = np.levels; l >= 1.0f; l-=1.0f) {
		sum += fabs(Noise::noise(p*f, np))/f;
		f *= 2.0f;
	}
	if (l>0.0f)
		sum += l*fabs(Noise::noise(p*f, np))/f;
	return sum;
}

float Fractal::fBm(Point3 p, NoiseParams& np) {
	float sum = 0.0f;
	float pwr = 1.0f;
	float pwHL = pow(np.lacunarity, -np.H);	

	for (int i=0; i<(int)np.levels; i++) {
		sum += Noise::noise(p, np) * pwr;
		pwr *= pwHL;
		p *= np.lacunarity;
	}

	float rmd = np.levels - FASTFLOOR(np.levels);
	if (rmd!=0.f) sum += rmd * Noise::noise(p, np) * pwr;

	return (sum+1.f)*.5f;
}

float Fractal::fBmTurb(Point3 p, NoiseParams& np) {
	float sum = 0.0f;
	float pwr = 1.0f;
	float pwHL = pow(np.lacunarity, -np.H);	

	for (int i=0; i<(int)np.levels; i++) {
		sum += fabs(Noise::noise(p, np)) * pwr;
		pwr *= pwHL;
		p *= np.lacunarity;
	}

	float rmd = np.levels - FASTFLOOR(np.levels);
	if (rmd!=0.f) sum += rmd * fabs(Noise::noise(p, np)) * pwr;

	return sum;
}

float Fractal::heteroTerrain(Point3 p, NoiseParams& np) {
	float sum = 0.0f;
	float pwHL = pow(np.lacunarity, -np.H);	
	float pwr = pwHL;

	sum = Noise::noise(p, np) + np.offset;
	p *= np.lacunarity;

	for (int i=0; i<(int)np.levels; i++) {
		sum += (Noise::noise(p, np) + np.offset) * pwr * sum;
		pwr *= pwHL;
		p *= np.lacunarity;
	}

	float rmd = np.levels - FASTFLOOR(np.levels);
	if (rmd!=0.f) sum += rmd * (Noise::noise(p, np) + np.offset) * pwr * sum;

	return (sum+1.f)*.5f;
}

float Fractal::hybridMultiFractal(Point3 p, NoiseParams& np) {
	float sum = 0.0f;
	float pwHL = pow(np.lacunarity, -np.H);	
	float pwr = pwHL;

	float signal, weight, rmd;	

	sum = Noise::noise(p, np) + np.offset;
	weight = np.gain * sum;
	p *= np.lacunarity;

	for (int i=1; (weight>0.001) && (i<(int)np.levels); i++) {
		if (weight>1.0)  weight=1.0;
		signal = (Noise::noise(p, np) + np.offset) * pwr;
		pwr *= pwHL;
		sum += weight * signal;
		weight *= np.gain * signal;
		p *= np.lacunarity;
	}

	rmd = np.levels - FASTFLOOR(np.levels);
	if (rmd!=0.f) sum += rmd * (Noise::noise(p, np) + np.offset) * pwr;	
													
	return (sum+1.f)*.5f;
}

float Fractal::ridgedMultiFractal(Point3 p, NoiseParams& np) {
	float sum = 0.0f;
	float pwHL = pow(np.lacunarity, -np.H);	
	float pwr = pwHL;

	float signal, weight;	

	signal = np.offset - fabs(Noise::noise(p, np));
	signal *= signal;
	sum = signal;
	weight = 1.f;

	for(int i=1; i<(int)np.levels; i++ ) {
		p *= np.lacunarity;
		weight = signal * np.gain;
		if (weight>1.0) weight=1.0; else if (weight<0.0) weight=0.0;
		signal = np.offset - fabs(Noise::noise(p, np));
		signal *= signal;
		signal *= weight;
		sum += signal * pwr;
		pwr *= pwHL;
	}
	
	return sum;
}

// Filtered

float Fractal::fractal(Point3 p, float d, NoiseParams& np) {
	float sum = 0.0f;
	float l, f = 1.0f;
	for (l = np.levels; l >= 1.0f; l-=1.0f) {				
		sum += Noise::noise(p*f, d*f, np)/f;
		f *= 2.0f;
	}	
	if (l>0.0f)				
		sum += l*Noise::noise(p*f, d*f, np)/f;				
	return 0.5f*(sum+1.0f);
}

float Fractal::turbulence(Point3 p, float d, NoiseParams& np) {
	float sum = 0.0f;
	float l,f = 1.0f;	
	for (l = np.levels; l >= 1.0f; l-=1.0f) {
		sum += fabs(Noise::noise(p*f, d*f, np))/f;
		f *= 2.0f;
	}
	if (l>0.0f)
		sum += l*fabs(Noise::noise(p*f, d*f, np))/f;
	return sum;
}

float Fractal::fBm(Point3 p, float d, NoiseParams& np) {
	float sum = 0.0f;
	float pwr = 1.0f;
	float pwHL = pow(np.lacunarity, -np.H);	

	for (int i=0; i<(int)np.levels; i++) {
		sum += Noise::noise(p, d, np) * pwr;
		pwr *= pwHL;
		p *= np.lacunarity;
		d *= np.lacunarity;
	}

	float rmd = np.levels - FASTFLOOR(np.levels);
	if (rmd!=0.f) sum += rmd * Noise::noise(p, d, np) * pwr;

	return (sum+1.f)*.5f;
}

float Fractal::fBmTurb(Point3 p, float d, NoiseParams& np) {
	float sum = 0.0f;
	float pwr = 1.0f;
	float pwHL = pow(np.lacunarity, -np.H);	

	for (int i=0; i<(int)np.levels; i++) {
		sum += fabs(Noise::noise(p, d, np)) * pwr;
		pwr *= pwHL;
		p *= np.lacunarity;
		d *= np.lacunarity;
	}

	float rmd = np.levels - FASTFLOOR(np.levels);
	if (rmd!=0.f) sum += rmd * fabs(Noise::noise(p, d, np)) * pwr;

	return sum;
}

float Fractal::heteroTerrain(Point3 p, float d, NoiseParams& np) {
	float sum = 0.0f;
	float pwHL = pow(np.lacunarity, -np.H);	
	float pwr = pwHL;

	sum = Noise::noise(p, np) + np.offset;
	p *= np.lacunarity;
	d *= np.lacunarity;

	for (int i=0; i<(int)np.levels; i++) {
		sum += (Noise::noise(p, d, np) + np.offset) * pwr * sum;
		pwr *= pwHL;
		p *= np.lacunarity;
		d *= np.lacunarity;
	}

	float rmd = np.levels - FASTFLOOR(np.levels);
	if (rmd!=0.f) sum += rmd * (Noise::noise(p, d, np) + np.offset) * pwr * sum;

	return (sum+1.f)*.5f;
}

float Fractal::hybridMultiFractal(Point3 p, float d, NoiseParams& np) {
	float sum = 0.0f;
	float pwHL = pow(np.lacunarity, -np.H);	
	float pwr = pwHL;

	float signal, weight, rmd;	

	sum = Noise::noise(p, d, np) + np.offset;
	weight = np.gain * sum;
	p *= np.lacunarity;
	d *= np.lacunarity;

	for (int i=1; (weight>0.001) && (i<(int)np.levels); i++) {
		if (weight>1.0)  weight=1.0;
		signal = (Noise::noise(p, d, np) + np.offset) * pwr;
		pwr *= pwHL;
		sum += weight * signal;
		weight *= np.gain * signal;
		p *= np.lacunarity;
		d *= np.lacunarity;
	}

	rmd = np.levels - FASTFLOOR(np.levels);
	if (rmd!=0.f) sum += rmd * (Noise::noise(p, d, np) + np.offset) * pwr;	
													
	return (sum+1.f)*.5f;
}

float Fractal::ridgedMultiFractal(Point3 p, float d, NoiseParams& np) {
	float sum = 0.0f;
	float pwHL = pow(np.lacunarity, -np.H);	
	float pwr = pwHL;

	float signal, weight;	

	signal = np.offset - fabs(Noise::noise(p, d, np));
	signal *= signal;
	sum = signal;
	weight = 1.f;

	for(int i=1; i<(int)np.levels; i++ ) {
		p *= np.lacunarity;
		d *= np.lacunarity;
		weight = signal * np.gain;
		if (weight>1.0) weight=1.0; else if (weight<0.0) weight=0.0;
		signal = np.offset - fabs(Noise::noise(p, d, np));
		signal *= signal;
		signal *= weight;
		sum += signal * pwr;
		pwr *= pwHL;
	}
	
	return sum;
}

// Generic
float Fractal::fBm(Point3 p, float levels, float lacunarity, float H) {
	float sum = 0.0f;
	float pwr = 1.0f;
	float pwHL = pow(lacunarity, -H);	

	for (int i=0; i<(int)levels; i++) {
		sum += Perlin::noise(p.x, p.y, p.z) * pwr;
		pwr *= pwHL;
		p *= lacunarity;
	}

	float rmd = levels - FASTFLOOR(levels);
	if (rmd!=0.f) sum += rmd * Perlin::noise(p.x, p.y, p.z) * pwr;

	return (sum+1.f)*.5f;
}

// Wood specific
float Fractal::grain(Point3 p, float amount, float freq) {
		p *= freq;
		p.z *= .05f;
		float g = (fBm(p, 4.f, 2.f, .5f)+1.f)*.5f;
		return smooth(g, (1.f-amount), 1.f);;
}
