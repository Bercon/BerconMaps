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

#include "math.h"

typedef float real;
typedef double ireal;

// Functions

float smooth(float d);

float smooth(float d, float low, float high);

float linear(float d, float low, float high);

float lerp(float a, float b, float blend);

inline float avg(float a, float b) { return (a + b) / 2.f; }
inline float avg(float a, float b, float c) { return (a + b + c) / 3.f; }

inline float length(float a, float b) { return sqrt(a*a + b*b); }
inline float length(float a, float b, float c) { return sqrt(a*a + b*b + c*c); }

// Macros

#define SMOOTH(d) (d*d*(3.f-2.f*d))

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#define MIN(a,b) (((a) > (b)) ? (b) : (a))

#define FASTFLOOR(x) ((x) < 0 ? ((int)x-1) : ((int)x) )

#define FASTFLOORI(x) ((x) < 0 ? ((int)x-1) : ((int)x) )

#define FASTFLOORL(x) ((x) < 0 ? ((long)x-1) : ((long)x) )

#define FADE(t) ( t * t * t * ( t * ( t * 6 - 15 ) + 10 ) )

#define LERP(t, a, b) ((a) + (t)*((b)-(a)))

#define ABS(x) ((x) < 0 ? (-x) : (x))

//#define LERP(a, b, blend) (b + blend * (a-b))

#define SFRAND() ((double)rand() / (double)RAND_MAX) // Random number (0..1)

#define UFRAND() ((double)rand() / (double)RAND_MAX * 2. - 1.) // Random number (-1..1)

#define SQRT2 1.41421356 // sqrt(2);

#define DEG2RAD 0.0174532925f