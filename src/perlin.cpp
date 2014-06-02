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

#include	"perlin.h"

// Chart given by Ken Perlin, using the same to get identical results
static unsigned char perm[] = {		
	151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,	
	151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

static unsigned char simplex[64][4] = {
	{0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
	{0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
	{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
	{1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
	{1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
	{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
	{2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
	{2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0}};
	
float lerp2(float t, float a,float b) {return a+t*(b-a);}

// #################### // Gradients \\ ####################

float Perlin::grad( int hash, float x ) {
    int h = hash & 15;
    float grad = 1.0f + (h & 7);
    if (h&8) grad = -grad;
    return ( grad * x );
}

float Perlin::grad(int hash, float x, float y) {
    int h = hash & 7;
    float i = h<4 ? x : y;
    float j = h<4 ? y : x;
    return ((h&1)? -i : i) + ((h&2)? -2.f*j : 2.f*j);
}

float Perlin::grad(int hash, float x, float y, float z) {
    int h = hash & 15;
    float i = h<8 ? x : y;
    float j = h<4 ? y : h==12||h==14 ? x : z;
    return ((h&1) ? -i : i) + ((h&2) ? -j : j);
}

float Perlin::grad(int hash, float x, float y, float z, float w) {
	int h = hash & 31;
    float i = h<24 ? x : y;
    float j = h<16 ? y : z;
    float k = h<8 ? z : w;
    return ((h&1)? -i : i) + ((h&2)? -j : j) + ((h&4)? -k : k);
}

// #################### // Perlin \\ ####################

float Perlin::noise(float x) {    
    int ix0 = FASTFLOOR(x);		
	int ix1 = (ix0+1) & 255;
    float xf0 = x - ix0;
	float xf1 = xf0 - 1.0f;   
	ix0 = ix0 & 0xff;    // Wrap to 0..255
    float s = FADE(xf0);
    return lerp2(s, grad(perm[ix0], xf0),
				   grad(perm[ix1], xf1)) * 0.188f;
}

float Perlin::noise(float x, float y) {    
    int xi = FASTFLOOR(x);
	int yi = FASTFLOOR(y);
    float xf0 = x - xi;
    float yf0 = y - yi;
    float xf1 = xf0 - 1.0f;
    float yf1 = yf0 - 1.0f;
    xi = xi & 255;
    yi = yi & 255;       
    float i = FADE(xf0);
	float j = FADE(yf0);
	int A = perm[xi  ] + yi;
	int B = perm[xi+1] + yi;

    return lerp2(j, lerp2(i, grad(perm[A  ], xf0, yf0),
		                   grad(perm[B  ], xf1, yf0)),
		           lerp2(i, grad(perm[A+1], xf0, yf1),
		                   grad(perm[B+1], xf1, yf1))) * 0.507f;
}

float Perlin::noise(float x, float y, float z) {
	int xi = FASTFLOOR(x);
	int yi = FASTFLOOR(y);
	int zi = FASTFLOOR(z);
	float xf = x - (float)xi;
	float yf = y - (float)yi;
	float zf = z - (float)zi;
	xi = xi & 255;
	yi = yi & 255;
	zi = zi & 255;
	float i = FADE(xf);
	float j = FADE(yf);
	float k = FADE(zf);
	int A = perm[xi  ]+yi, AA = perm[A]+zi, AB = perm[A+1]+zi;
	int B = perm[xi+1]+yi, BA = perm[B]+zi, BB = perm[B+1]+zi;

	return lerp2(k, lerp2(j, lerp2(i, grad(perm[AA  ], xf  , yf  , zf   ),
								   grad(perm[BA  ], xf-1, yf  , zf   )),
						   lerp2(i, grad(perm[AB  ], xf  , yf-1, zf   ),
								   grad(perm[BB  ], xf-1, yf-1, zf   ))),
				   lerp2(j, lerp2(i, grad(perm[AA+1], xf  , yf  , zf-1 ),
								   grad(perm[BA+1], xf-1, yf  , zf-1 )),
						   lerp2(i, grad(perm[AB+1], xf  , yf-1, zf-1 ),
								   grad(perm[BB+1], xf-1, yf-1, zf-1 )))) * 0.936f;
}

float Perlin::noise(float x, float y, float z, float w) {
    int xi = FASTFLOOR(x);
	int yi = FASTFLOOR(y);
	int zi = FASTFLOOR(z);
	int wi = FASTFLOOR(w);
    float xf0 = x - xi;
    float yf0 = y - yi;
	float zf0 = z - zi;
	float wf0 = w - wi;
    float xf1 = xf0 - 1.0f;
    float yf1 = yf0 - 1.0f;
	float zf1 = zf0 - 1.0f;
	float wf1 = wf0 - 1.0f;
    xi = xi & 255;
    yi = yi & 255;   
	zi = zi & 255;
	wi = wi & 255;
    float i = FADE(xf0);
    float j = FADE(yf0);
	float k = FADE(zf0);
	float l = FADE(wf0);

	int A = perm[xi]  +yi, AA = perm[A]  +zi, AAA = perm[AA]  +wi,
                                              AAB = perm[AA+1]+wi,
		                   AB = perm[A+1]+zi, ABA = perm[AB]  +wi,
                                              ABB = perm[AB+1]+wi;
	int B = perm[xi+1]+yi, BA = perm[B]  +zi, BAA = perm[BA]  +wi,
                                              BAB = perm[BA+1]+wi,
		                   BB = perm[B+1]+zi, BBA = perm[BB]  +wi,
                                              BBB = perm[BB+1]+wi;
											  
	
	float l1 = lerp2(i, grad(perm[AAA  ], xf0, yf0, zf0, wf0), grad(perm[BAA  ], xf1, yf0, zf0, wf0));
	float l2 = lerp2(i, grad(perm[ABA  ], xf0, yf1, zf0, wf0), grad(perm[BBA  ], xf1, yf1, zf0, wf0));
	float l3 = lerp2(i, grad(perm[AAB  ], xf0, yf0, zf1, wf0), grad(perm[BAB  ], xf1, yf0, zf1, wf0));
	float l4 = lerp2(i, grad(perm[ABB  ], xf0, yf1, zf1, wf0), grad(perm[BBB  ], xf1, yf1, zf1, wf0));
	float l5 = lerp2(i, grad(perm[AAA+1], xf0, yf0, zf0, wf1), grad(perm[BAA+1], xf1, yf0, zf0, wf1));
	float l6 = lerp2(i, grad(perm[ABA+1], xf0, yf1, zf0, wf1), grad(perm[BBA+1], xf1, yf1, zf0, wf1));
	float l7 = lerp2(i, grad(perm[AAB+1], xf0, yf0, zf1, wf1), grad(perm[BAB+1], xf1, yf0, zf1, wf1));
	float l8 = lerp2(i, grad(perm[ABB+1], xf0, yf1, zf1, wf1), grad(perm[BBB+1], xf1, yf1, zf1, wf1));

	l1 = lerp2(j, l1, l2);
	l2 = lerp2(j, l3, l4);
	l3 = lerp2(j, l5, l6);
	l4 = lerp2(j, l7, l8);

	l1 = lerp2(k, l1, l2);
	l2 = lerp2(k, l3, l4);

	return lerp2(l, l1, l2) * 0.87f;
}

// #################### // Simplex points \\ ####################

inline float Perlin::point(float x, int i) {
	float t = 1.0f - x*x;
	t *= t;
	return t * t * grad(perm[i & 255], x);
}

inline float Perlin::point(float x, float y, int i, int j) {
    float t = 0.5f - x*x - y*y;
    if(t < 0.0f) 
		return 0.0f;
    t *= t;
    return t * t * grad(perm[i+perm[j]], x, y);
}

inline float Perlin::point(float x, float y, float z, int i, int j, int k) {
	float t = 0.6f - x*x - y*y - z*z;
	if(t < 0.0f) 
		return 0.0f;
	t *= t;
	return t * t * grad(perm[i+perm[j+perm[k]]], x, y, z);
}

inline float Perlin::point(float x, float y, float z, float w, int i, int j, int k, int l) {
	float t = 0.6f - x*x - y*y - z*z - w*w;
	if(t < 0.0f)
		return 0.0f;	
	t *= t;
	return t * t * grad(perm[i+perm[j+perm[k+perm[l]]]], x, y, z, w);	
}

// #################### // Simplex \\ ####################

float Perlin::snoise(float x) {
	int i0 = FASTFLOOR(x);
	int i1 = i0 + 1;
	float x0 = x - i0;
	float x1 = x0 - 1.0f;
	return (1.f/2.53125f) * (point(x0, i0) + point(x1, i1));
}

float Perlin::snoise(float x, float y) {

	#define F2 0.366025403f // F2 = 0.5*(sqrt(3.0)-1.0)
	#define G2 0.211324865f // G2 = (3.0-Math.sqrt(3.0))/6.0

    float s = (x+y)*F2;
    float xs = x + s;
    float ys = y + s;
    int i = FASTFLOOR(xs);
    int j = FASTFLOOR(ys);

    float t = (float)(i+j)*G2;
    float X0 = i-t;
    float Y0 = j-t;
    float x0 = x-X0;
    float y0 = y-Y0;

    int i1, j1;
    if(x0>y0) {i1=1; j1=0;}
    else {i1=0; j1=1;}

    float x1 = x0 - i1 + G2, x2 = x0 - 1.0f + 2.0f * G2;
    float y1 = y0 - j1 + G2, y2 = y0 - 1.0f + 2.0f * G2;     

    int ii = i % 256;
    int jj = j % 256;

    return 40.0f * (point(x0, y0, ii,    jj) +
					point(x1, y1, ii+i1, jj+j1) +
					point(x2, y2, ii+1,  jj+1));
}

float Perlin::snoise(float x, float y, float z) {

	#define F3 0.333333333f
	#define G3 0.166666667f
	#define G3a	(2.0f * G3)
	#define G3b (3.0f * G3 - 1.0f)

    float s = (x+y+z)*F3;
    float xs = x+s;
    float ys = y+s;
    float zs = z+s;
    int i = FASTFLOOR(xs);
    int j = FASTFLOOR(ys);
    int k = FASTFLOOR(zs);

    float t = (float)(i+j+k)*G3; 
    float X0 = i-t;
    float Y0 = j-t;
    float Z0 = k-t;
    float x0 = x-X0;
    float y0 = y-Y0;
    float z0 = z-Z0;

    int i1, j1, k1;
    int i2, j2, k2;

    if(x0>=y0) {
		if(y0>=z0)		{ i1=1; j1=0; k1=0; i2=1; j2=1; k2=0; }
        else if(x0>=z0) { i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; }
        else			{ i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; }
	} else {
		if(y0<z0)		{ i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; }
		else if(x0<z0)	{ i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; }
		else			{ i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; }
    }

    float x1 = x0 - i1 + G3,  x2 = x0 - i2 + G3a,  x3 = x0 + G3b;
    float y1 = y0 - j1 + G3,  y2 = y0 - j2 + G3a,  y3 = y0 + G3b;
    float z1 = z0 - k1 + G3,  z2 = z0 - k2 + G3a,  z3 = z0 + G3b;

    int ii = i % 256;
    int jj = j % 256;
    int kk = k % 256;

	return 32.0f * (point(x0, y0, z0, ii,    jj,    kk) +
					point(x1, y1, z1, ii+i1, jj+j1, kk+k1) +
					point(x2, y2, z2, ii+i2, jj+j2, kk+k2) +
					point(x3, y3, z3, ii+1,  jj+1,  kk+1));
}

float Perlin::snoise(float x, float y, float z, float w) {
  
	#define F4 0.309016994f // F4 = (Math.sqrt(5.0)-1.0)/4.0
	#define G4 0.138196601f // G4 = (5.0-Math.sqrt(5.0))/20.0
	#define G4a (G4*2.f)
	#define G4b (G4*3.f)
	#define G4c (G4*4.f-1.f)

    float s = (x + y + z + w) * F4;
    float xs = x + s;
    float ys = y + s;
    float zs = z + s;
    float ws = w + s;
    int i = FASTFLOOR(xs);
    int j = FASTFLOOR(ys);
    int k = FASTFLOOR(zs);
    int l = FASTFLOOR(ws);

    float t = (i + j + k + l) * G4;
    float X0 = i - t; 
    float Y0 = j - t;
    float Z0 = k - t;
    float W0 = l - t;

    float x0 = x - X0;
    float y0 = y - Y0;
    float z0 = z - Z0;
    float w0 = w - W0;

    int c1 = (x0 > y0) ? 32 : 0;
    int c2 = (x0 > z0) ? 16 : 0;
    int c3 = (y0 > z0) ? 8 : 0;
    int c4 = (x0 > w0) ? 4 : 0;
    int c5 = (y0 > w0) ? 2 : 0;
    int c6 = (z0 > w0) ? 1 : 0;
    int c = c1 + c2 + c3 + c4 + c5 + c6;

    int i1, j1, k1, l1;
    int i2, j2, k2, l2;
    int i3, j3, k3, l3;

    i1 = simplex[c][0]>=3 ? 1 : 0; i2 = simplex[c][0]>=2 ? 1 : 0; i3 = simplex[c][0]>=1 ? 1 : 0;
    j1 = simplex[c][1]>=3 ? 1 : 0; j2 = simplex[c][1]>=2 ? 1 : 0; j3 = simplex[c][1]>=1 ? 1 : 0;
    k1 = simplex[c][2]>=3 ? 1 : 0; k2 = simplex[c][2]>=2 ? 1 : 0; k3 = simplex[c][2]>=1 ? 1 : 0;
    l1 = simplex[c][3]>=3 ? 1 : 0; l2 = simplex[c][3]>=2 ? 1 : 0; l3 = simplex[c][3]>=1 ? 1 : 0;

	float x1 = x0 - i1 + G4,  x2 = x0 - i2 + G4a,  x3 = x0 - i3 + G4b,  x4 = x0 + G4c;
    float y1 = y0 - j1 + G4,  y2 = y0 - j2 + G4a,  y3 = y0 - j3 + G4b,  y4 = y0 + G4c;
    float z1 = z0 - k1 + G4,  z2 = z0 - k2 + G4a,  z3 = z0 - k3 + G4b,  z4 = z0 + G4c;
    float w1 = w0 - l1 + G4,  w2 = w0 - l2 + G4a,  w3 = w0 - l3 + G4b,  w4 = w0 + G4c;

    int ii = i % 256;
    int jj = j % 256;
    int kk = k % 256;
    int ll = l % 256;

	return 27.0f * (point(x0, y0, z0, w0, ii,    jj,    kk,    ll) +
					point(x1, y1, z1, w1, ii+i1, jj+j1, kk+k1, ll+l1) +
					point(x2, y2, z2, w2, ii+i2, jj+j2, kk+k2, ll+l2) +
					point(x3, y3, z3, w3, ii+i3, jj+j3, kk+k3, ll+l3) +
					point(x4, y4, z4, w4, ii+1,  jj+1,  kk+1,  ll+1));
}

/*
	Filtered functions, simply add the fading functinality once the size of the sampled
	area becomes larger than the details.
*/

#define FILTDIV		1.2f
#define FILTSTART	1.2f
#define FILTEND		2.2f

#define FILTER(d) d = smooth(d / FILTDIV, FILTSTART, FILTEND); \
				  if (d >= .9999f) return 0.f;

float Perlin::fnoise2D(float x, float y, float d) {
	FILTER(d)
	return lerp2(d, noise(x,y), 0.f);
}

float Perlin::fnoise3D(float x, float y, float z, float d) {
	FILTER(d)
	return lerp2(d, noise(x,y,z), 0.f);
}

float Perlin::fnoise4D(float x, float y, float z, float w, float d) {
	FILTER(d)
	return lerp2(d, noise(x,y,z,w), 0.f);
}	

float Perlin::fsnoise2D(float x, float y, float d) {
	FILTER(d)
	return lerp2(d, snoise(x,y), 0.f);
}

float Perlin::fsnoise3D(float x, float y, float z, float d) {
	FILTER(d)
	return lerp2(d, snoise(x,y,z), 0.f);
}

float Perlin::fsnoise4D(float x, float y, float z, float w, float d) {
	FILTER(d)
	return lerp2(d, snoise(x,y,z,w), 0.f);
}	