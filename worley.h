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

// Based on Steven Worley's cellular noise

#pragma once

#include "noise.h"


class Worley {
public:
    Worley() {}
    ~Worley() {}

	static void noise(double at[3], int order, double* F, int function);

private:
	static void add(long xi, long yi, long zi, double at[3], int order, double *F, int function);
};