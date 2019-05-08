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

// Implementation of Steven Worley's cellular noise

#include <math.h>
#include "worley.h"

static char Poisson_count[256]= {
	4,3,1,1,1,2,4,2,2,2,5,1,0,2,1,2,2,0,4,3,2,1,2,1,3,2,2,4,2,2,5,1,2,3,2,2,2,2,2,3,
	2,4,2,5,3,2,2,2,5,3,3,5,2,1,3,3,4,4,2,3,0,4,2,2,2,1,3,2,2,2,3,3,3,1,2,0,2,1,1,2,
	2,2,2,5,3,2,3,2,3,2,2,1,0,2,1,1,2,1,2,2,1,3,4,2,2,2,5,4,2,4,2,2,5,4,3,2,2,5,4,3,
	3,3,5,2,2,2,2,2,3,1,1,4,2,1,3,3,4,3,2,4,3,3,3,4,5,1,4,2,4,3,1,2,3,5,3,2,1,3,1,3,
	3,3,2,3,1,5,5,4,2,2,4,1,3,4,1,5,3,3,5,3,4,3,2,2,1,1,1,1,1,2,4,5,4,5,4,2,1,5,1,1,
	2,3,3,3,2,5,2,3,3,2,0,2,1,1,4,2,1,3,2,1,2,2,3,2,5,5,3,4,5,5,2,4,4,5,3,2,2,2,1,4,
	2,3,3,4,2,5,4,2,4,2,2,2,4,5,3,2};

#define DENSITY_ADJUSTMENT  0.398150
#define FASTFLOORL(x) ((x)<0 ? ((long)x-1) : ((long)x) )
#define ADD(a,b,c) ( add(int_at[0]+a, int_at[1]+b, int_at[2]+c, new_at, order, F, function) )

void Worley::noise(double at[3], int order, double *F, int function) {
	double x2, y2, z2, mx2, my2, mz2;
	double new_at[3];
	long int_at[3];

	for (int i=0; i<order; i++) F[i] = 999999.9;

	for (int i=0; i<3; i++) {
		new_at[i] = DENSITY_ADJUSTMENT * at[i];
		int_at[i] = FASTFLOORL(new_at[i]);
	}
	
	add(int_at[0], int_at[1], int_at[2], new_at, order, F, function);

	x2=new_at[0]-int_at[0];
	y2=new_at[1]-int_at[1];
	z2=new_at[2]-int_at[2];
	mx2=(1.0-x2)*(1.0-x2);
	my2=(1.0-y2)*(1.0-y2);
	mz2=(1.0-z2)*(1.0-z2);
	x2*=x2;
	y2*=y2;
	z2*=z2;

	// Faces
	if (x2          < F[order-1]) ADD(-1,  0,  0);
	if (y2          < F[order-1]) ADD( 0, -1,  0);
	if (z2          < F[order-1]) ADD( 0,  0, -1);
	if (mx2         < F[order-1]) ADD( 1,  0,  0);
	if (my2         < F[order-1]) ADD( 0,  1,  0);
	if (mz2         < F[order-1]) ADD( 0,  0,  1);
	// Edges
	if ( x2+ y2     < F[order-1]) ADD(-1, -1,  0);
	if ( x2+ z2     < F[order-1]) ADD(-1,  0, -1);
	if ( y2+ z2     < F[order-1]) ADD( 0, -1, -1);
	if (mx2+my2     < F[order-1]) ADD( 1,  1,  0);
	if (mx2+mz2     < F[order-1]) ADD( 1,  0,  1);
	if (my2+mz2     < F[order-1]) ADD( 0,  1,  1);
	if ( x2+my2     < F[order-1]) ADD(-1,  1,  0);
	if ( x2+mz2     < F[order-1]) ADD(-1,  0,  1);
	if ( y2+mz2     < F[order-1]) ADD( 0, -1,  1);
	if (mx2+ y2     < F[order-1]) ADD( 1, -1,  0);
	if (mx2+ z2     < F[order-1]) ADD( 1,  0, -1);
	if (my2+ z2     < F[order-1]) ADD( 0,  1, -1);
	// Corners
	if ( x2+ y2+ z2 < F[order-1]) ADD(-1, -1, -1);
	if ( x2+ y2+mz2 < F[order-1]) ADD(-1, -1,  1);
	if ( x2+my2+ z2 < F[order-1]) ADD(-1,  1, -1);
	if ( x2+my2+mz2 < F[order-1]) ADD(-1,  1,  1);
	if (mx2+ y2+ z2 < F[order-1]) ADD( 1, -1, -1);
	if (mx2+ y2+mz2 < F[order-1]) ADD( 1, -1,  1);
	if (mx2+my2+ z2 < F[order-1]) ADD( 1,  1, -1);
	if (mx2+my2+mz2 < F[order-1]) ADD( 1,  1,  1);

	for (int i=0; i<order; i++)
		F[i] = sqrt(F[i])*(1.0/DENSITY_ADJUSTMENT);      

	return;
}

#define ROLL(seed) (seed=1402024253*seed+586950981)
void Worley::add(long xi, long yi, long zi, double at[3], int order, double *F, int function) {
	double d = 0;
	double d3[3];
	double f3[3];
	unsigned long seed, this_id;

	seed = 702395077*xi + 915488749*yi + 2120969693*zi;
	int count = Poisson_count[seed>>24];
	ROLL(seed);

	for (int j=0; j<count; j++) {
		this_id = seed;
		ROLL(seed);

		for (int i=0; i<3; i++) {
			f3[i] = (seed+0.5)*(1.0/4294967296.0);			
			ROLL(seed);
		}

		d3[0] = xi + f3[0] - at[0]; 
		d3[1] = yi + f3[1] - at[1];
		d3[2] = zi + f3[2] - at[2];

		// Distance function to the point
		switch (function) {
			case 0: // Linear
				d = sqrt(d3[0]*d3[0]+d3[1]*d3[1]+d3[2]*d3[2]);
				break;
			case 1: // Square
				d = d3[0]*d3[0]+d3[1]*d3[1]+d3[2]*d3[2];
				break;
			case 2: // Manhattan
				d = fabs(d3[0])+fabs(d3[1])+fabs(d3[2]);
				break;			
			case 3: {// Chebychev				
				double x = fabs(d3[0]);
				double y = fabs(d3[1]);
				double z = fabs(d3[2]);
				double t = (x>y)?x:y;
				d = ((z>t)?z:t);
				break; }
			case 4: // Minkovsky 0.5
				d = sqrt(fabs(d3[0])) + sqrt(fabs(d3[1])) + sqrt(fabs(d3[2]));	
				d *= d;
				break;
			case 5: {// Minkovsky 4
				double x = d3[0]*d3[0];
				double y = d3[1]*d3[1];
				double z = d3[2]*d3[2];				
				d = sqrt(sqrt(x*x + y*y + z*z));
				break; }
		}		

		if (d < F[order-1]) {
			int index = order;		
			while (index > 0 && d < F[index-1]) index--;
			for (int i=order-2; i>=index; i--)
				F[i+1] = F[i];
			F[index] = d;
		}
	}
}
