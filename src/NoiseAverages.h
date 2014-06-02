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

#define sfrand() ((double)rand() / (double)RAND_MAX)
#define ufrand() ((double)rand() / (double)RAND_MAX * 2. - 1.)

/*
	Used to compute average values for noise functions, these can be used in filtering
	Generally the average value should be 0.0

	Results for the code below are:
		Noise #0 = 0.000004
		Noise #1 = -0.000373
		Noise #2 = 0.000208
		Noise #3 = 0.000020
		Noise #4 = -0.000007
		Noise #5 = 0.000211
		Noise #6 = -0.000336
		Noise #7 = -0.000121
	Which means they all average out pretty near 0.0 so we can use it
*/
class NoiseAverages {
public:
	static void computeAverages() {
		CharStream *out = thread_local(current_stdout);
		srand(12345);

		int SAMPLES = 1000000; // Number of samples computed to get average
		float RANGE = 12345.0f; // The size of the area where samples are taken
		
		double noiseValue[8];
		for (int i=0;i<8;i++)
			noiseValue[i] = 0.;
		
		for (int i=0;i<SAMPLES;i++) {
			float x = ufrand() * RANGE;
			float y = ufrand() * RANGE;
			float z = ufrand() * RANGE;
			float w = ufrand() * RANGE;
		
			noiseValue[0] += Perlin::noise(x);
			noiseValue[1] += Perlin::noise(x,y);
			noiseValue[2] += Perlin::noise(x,y,z);
			noiseValue[3] += Perlin::noise(x,y,z,w);
			noiseValue[4] += Perlin::snoise(x);
			noiseValue[5] += Perlin::snoise(x,y);
			noiseValue[6] += Perlin::snoise(x,y,z);
			noiseValue[7] += Perlin::snoise(x,y,z,w);
		}

		for (int i=0;i<8;i++) {
			out->printf("Noise #%d = %f\n", i, noiseValue[i] / (double)SAMPLES);	
		}			
	}
};
