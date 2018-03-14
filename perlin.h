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

// Based on Ken Perlin's perlin noise/simplex and Stefan Gustavson's
// implementation of perlin/simplex noise, (Simplex)Noise1234

#pragma once

#include "commonMath.h"

class Perlin {
public:
    Perlin() {}
    ~Perlin() {}

	static float noise(float x);
    static float noise(float x, float y);
    static float noise(float x, float y, float z);
    static float noise(float x, float y, float z, float w);

	static float snoise(float x);
    static float snoise(float x, float y);
    static float snoise(float x, float y, float z);
    static float snoise(float x, float y, float z, float w);

	/*	Filtered Noises, this means they'll fade to average value (0.0) when viewed from a distance
		to prevent noise caused by details smaller than pixel. Its worth pointing out that the 4D
		noise doesn't do filtering the fourth dimension, there is no point since you'll never render
		it as four dimensionsional pattern but choose some constant for the w paramater, basicly
		slicing the four dimensional noise into 3D space with using a plane. */	

	static float fnoise2D(float x, float y, float d);
	static float fnoise3D(float x, float y, float z, float d);
	static float fnoise4D(float x, float y, float z, float w, float d);

	static float fsnoise2D(float x, float y, float d);
	static float fsnoise3D(float x, float y, float z, float d);
	static float fsnoise4D(float x, float y, float z, float w, float d);

private:
	static float grad( int hash, float x);
	static float grad( int hash, float x, float y );
	static float grad( int hash, float x, float y , float z );
	static float grad( int hash, float x, float y, float z, float t );

	inline static float point(float x, int i);
	inline static float point(float x, float y, int i, int j);
	inline static float point(float x, float y, float z, int i, int j, int k);
	inline static float point(float x, float y, float z, float w, int i, int j, int k, int l);
};
