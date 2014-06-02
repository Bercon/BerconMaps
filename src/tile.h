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
#include <vector>
#include <string>
#include <sstream>

#pragma once

/*
	x_offset, height, a, b, c, d... /
	x_offset, height, a, b, c, d... /
	x_offset, height, a, b, c, d... /
	x_offset, height, a, b, c, d... /
	...

	Strecher Bond:
	0,   1, 1 /
	0.5, 1, 1

	Auto scaling
		scale to fit
		scale to fit no stretch
		scale to tile size
		no scale		

	random offset xy
	random scale xy (lock)
	rotation (random amount -+) (0..180)
	random flip x axis, random flip y axis

	randomize z coordinate (for MultiTexture etc.)

	map tile center to channel X



*/

class TileRow {
public:
	TileRow(float offset) {this->offset = offset;}

	float offset;
	float totalWidth;
	std::vector<float> tiles;

	void update() {
		totalWidth = 0.f;
		for (int i=0; i<tiles.size(); i++)
			totalWidth += tiles[i];
	}
};

class TilePattern {
public:
	TilePattern() {update();}
	~TilePattern() {}

	float totalHeight;
	std::vector<float> heights;
	std::vector<TileRow> rows;	

	void setPattern(std::wstring s);

	void update() {
		totalHeight = 0.f;
		for (int i=0; i<rows.size(); i++) {
			rows[i].update();
			totalHeight += heights[i];
		}
	}

	void setPreset(int preset);
};

class TileParam { // Tile parameters
public:
	int tilingType; 

	float tileHeight;
	float tileWidth;	
	float edgeHeight;
	float edgeWidth;			

	float tileHeightVar; // .5f == 100%
	float tileWidthVar;	
	float edgeHeightVar; // 1.f == 100%
	float edgeWidthVar;
	bool eW_var, eH_var;

	BOOL tileRound;
	int tileBlur;
	float tileBlurRad;
	float tileCrnrRad;	

	int autoScale;

	BOOL mapUV;
	float randX, randY;
	float randSX, randSY;
	BOOL lock;
	int rotUV;
	float randRot; // rad
	int flipX, flipY;
	int tileID;
	int center;

	TilePattern* pattern;

	// Automatically set parameters
	float tileMaxHeight;
	float tileMaxWidth;	
	bool randScale;
	bool randOffset;

	TileParam() { pattern = NULL; }

	void checkValues() {
		if (tileHeightVar > .5f) tileHeightVar = .5f;
		if (tileWidthVar > .5f) tileWidthVar = .5f;		
		if (edgeHeightVar > 1.f) edgeHeightVar = 1.f;		
		if (edgeWidthVar > 1.f) edgeWidthVar = 1.f;

		if (tilingType == 1) { // Herringbond can't have variation in these
			tileMaxHeight = tileWidth * .5f;
			tileMaxWidth = tileWidth;
		} else {
			tileMaxHeight = tileHeight * (1.f + tileHeightVar * 2.f);
			tileMaxWidth = tileWidth * (1.f + tileWidthVar * 2.f);
		}

		eW_var = edgeWidthVar > 0.0001f;
		eH_var = edgeHeightVar > 0.0001f;
		
		randScale  = randSX > 0.0001f || randSY > 0.0001f;
		randOffset = randX  > 0.0001f || randY  > 0.0001f;
	}
};

class TilePoint { // TilePoint class is used to define point in tiling texture
public:
	
	Point3 center; // Tile center
	Point3 uvw;      // UVW
	float d;       // Shade, -1 == edge, 0..1 = tile
	int id;        // ID for MultiTexture

	TilePoint() {d = -1.f;}	
	TilePoint(float y) {d = y;}	
	TilePoint(Point3 x) {uvw = x;}
	TilePoint(Point3 x, float y) {uvw = x; d = y;}
	//TilePoint(Point3 x, float y, Color z) {p = x; d = y; c = z;}
	//TilePoint(Color z) {c = z; d = -3.f; }
};

class Tile {
private:
// Helpers
	static float edgeBlur(float d, float r, int type);
	static TilePoint corner(float rX, float rY, float w, float h, TileParam& t);
	static void rotatePoint2(Point3& d, float angle);
	static void rotateUV(int rotUV, float var, Point3& d);

	static void uvMapping(TilePoint& tp, Point3 p, float edges[4], TileParam& t, int dir=0);
	static TilePoint drawTile(Point3 p, float edges[4], TileParam& t, int id=0, int dir=0);

// Patterns
	static TilePoint pat_xBond(Point3 p, TileParam& t);
	static TilePoint pat_herring(Point3 p, TileParam& t);

public:

	static TilePoint draw(Point3 p, TileParam& t);

};

