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

#include "commonMath.h"

float smooth(float d) { 
	return (d*d*(3.f-2.f*d));
}

float smooth(float d, float low, float high) { 
	d = (d-low)/(high-low);
	if (d < 0) return 0.f;
	if (d > 1) return 1.f;
	return (d*d*(3.f-2.f*d));
}

float linear(float d, float low, float high) { 
	d = (d-low)/(high-low);
	if (d < 0) return 0.f;
	if (d > 1) return 1.f;
	return d;
}

float lerp(float a, float b, float blend) {
	return b + blend * (a-b);
}