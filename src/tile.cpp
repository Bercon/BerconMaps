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

#include "tile.h"
#include "perlin.h"

inline static int generateID(int x, int y) { return (y-101) * 1000 + x; }

inline static float noise(float x, float rand) { return Perlin::snoise(x * 31.4 + rand); }

inline static int STEPUP(int n, int m) {
	return (n + 1) % m;
}

inline static void rotate90(float& x, float& y)  { float temp = y; y = x; x = -temp; }
inline static void rotate270(float& x, float& y) { float temp = x; x = y; y = -temp; }

inline static int STEPDOWN(int n, int m) {	
	if (n-1 < 0) return m-1;
	return n-1;
}


float Tile::edgeBlur(float d, float r, int type) {
	switch (type) {		
		case 1: return d/r; // Linear
		case 2: // Cubic
			d = 1.f - (d/r);
			return sqrt(1-d*d);
		case 3: return smooth(d, 0.f, r); // Smooth					
		default: return 1.f;
	}	
}

TilePoint Tile::corner(float rX, float rY, float w, float h, TileParam& t) {	
	// Evaluate	maps
	if (t.tileRound) {
		bool inCorner = false;
		float cX=0.0f, cY=0.0f;
		// Detect corner
		if (rX < t.tileCrnrRad) { 
			if (rY < t.tileCrnrRad) { // Bottom left
				cX = t.tileCrnrRad - rX;
				cY = t.tileCrnrRad - rY; 				
				inCorner = true;
			} else if (rY > h - t.tileCrnrRad) { // Top left
				cX = t.tileCrnrRad - rX;
				cY = h - rY - t.tileCrnrRad; 				
				inCorner = true;
			}
		} else if (rX > w - t.tileCrnrRad) {
			if (rY < t.tileCrnrRad) { // Bottom right
				cX = w - rX - t.tileCrnrRad;
				cY = t.tileCrnrRad - rY; 				
				inCorner = true;
			} else if (rY > h - t.tileCrnrRad) { // Top right
				cX = w - rX - t.tileCrnrRad;
				cY = h - rY - t.tileCrnrRad;
				inCorner = true;
			}
		}
		float d = cY*cY + cX*cX;
		if (inCorner) {
			if (d > t.tileCrnrRad*t.tileCrnrRad)
				return TilePoint();
			// Calculate corner blur
			d = t.tileCrnrRad - sqrt(d);
			if (t.tileBlur > 0 && d <t. tileBlurRad)
				return TilePoint(Point3(rX/w, rY/h, 0.f), edgeBlur(d, t.tileBlurRad, t.tileBlur));
		}
	} 
	if (t.tileBlur) {		
		bool inCorner = false;
		float dX = rX;
		float dY = rY;
		// Distance to vertical walls
		if (rX < t.tileBlurRad) {
			inCorner = true;
		} else if (rX > w - t.tileBlurRad) {
			dX = w - rX;
			inCorner = true;
		}
		// Distance to horizontal walls
		if (rY < t.tileBlurRad) {
			inCorner = true;
		} else if (rY > h - t.tileBlurRad) {
			dY = h - rY;
			inCorner = true;
		}
		if (inCorner)
			return dX<dY?TilePoint(Point3(rX/w, rY/h, 0.f), edgeBlur(dX, t.tileBlurRad, t.tileBlur)):TilePoint(Point3(rX/w, rY/h, 0.f), edgeBlur(dY, t.tileBlurRad, t.tileBlur));
	}
	return TilePoint(Point3(rX/w, rY/h, 0.f), 1.f);
}

void Tile::rotateUV(int rotUV, float var, Point3& d) {

}

void Tile::rotatePoint2(Point3& d, float angle) {
	float rotX = (cos(angle)*d.x - sin(angle)*d.y);
	float rotY = (sin(angle)*d.x + cos(angle)*d.y);
	d.x = rotX;
	d.y = rotY;
}

inline static float getAngle(int type, float amount) {	
	switch (type) {
		case 0:
			return 0.f;
		case 1: // 180
			if (rand() % 2)
				return (float)pi;
			return 0.f;
		case 2: // 90
			switch (rand() % 4) {
				case 0:
					return 0.f;
				case 1: 
					return (float)pi*.5f;
				case 2:
					return (float)pi;
				case 3:
					return (float)pi*1.5f;
			}			
		default: // Amount	
			return UFRAND() * amount;
	}
}

/*
	0 No auto scale
	1 UV
	2 UV Fit
	3 UV Fit Keep aspect
	4 UV Norm.	
	5 UV Norm. Keep aspect 	

	+ ensure inside uv with rand offset/scale
*/
#pragma warning (push)
#pragma warning (disable: 4005)	// macro redefinition warning

#define HALFPI 1.57079633f
#define QUATPI 0.785398163f
#define SQRTHALF 0.707106781f

#pragma warning (pop)


// left up right down
void Tile::uvMapping(TilePoint& tp, Point3 p, float edges[4], TileParam& t, int dir) {
	float w = edges[2] - edges[0];
	float h = edges[1] - edges[3];
		
	// Center uvw
	Point3 uvw;
	uvw.x = p.x - edges[0] - w * .5f;
	uvw.y = p.y - edges[3] - h * .5f;
	uvw.z = p.z;

	// Prime randomness
	srand(tp.id*(tp.id*tp.id*15731 + 789221));

	// Angle
	float angle = getAngle(t.rotUV, t.randRot);

	// Random scale
	float scaleX, scaleY;	
	switch (t.autoScale) {
		case 1: { // UV
			scaleX = w; scaleY = h;
			break; }		
		case 2: { // UV Fit
			scaleX = w; scaleY = h; // Same as UV, but with additiona scaling below
			break; }
		case 3: { // UV Fit Keep aspect
			float s = MAX(w, h);		
			scaleX = s; scaleY = s;
			break; }
		case 4: { // UV Norm.
			scaleX = t.tileMaxWidth;
			scaleY = t.tileMaxHeight;
			break; }
		case 5: { // UV Norm. Keep aspect
			float s = MAX(t.tileMaxWidth, t.tileMaxHeight);		
			scaleX = s;
			scaleY = s;
			break; }		
	}

	// Calculate scaling required to fit UVs tightly around the tile
	if ((t.autoScale == 2 || t.autoScale == 3) && t.rotUV) { // Scale based on rotation so whole tile stays inside 0..1 space
		float scale = 2.f * SQRTHALF * cos(fmod(angle > 0 ? -angle : angle, HALFPI) + QUATPI);
		scaleX *= scale;
		scaleY *= scale;
	}	

	// Apply auto scale
	if (t.autoScale) {
		uvw.x /= scaleX;
		uvw.y /= scaleY;
	}

	// Flip
	if (t.flipX) if (rand() % 2) uvw.x = -uvw.x;
	if (t.flipY) if (rand() % 2) uvw.y = -uvw.y;
	
	// Random scale
	if (t.randScale) {
		if (t.lock) {
			float s = 1.f + SFRAND() * t.randSX;
			scaleX = s;
			scaleY = s;
		} else {
			scaleX = 1.f + SFRAND() * t.randSX;
			scaleY = 1.f + SFRAND() * t.randSY;
		}

		if (scaleX < 0) scaleX = 0.f;
		if (scaleY < 0) scaleY = 0.f;

		uvw.x /= scaleX;	 
		uvw.y /= scaleY;
	}

	// Offset
	if (t.randOffset) {
		uvw.x += UFRAND() * t.randX;
		uvw.y += UFRAND() * t.randY;
	}

	// Rotate
	if (t.rotUV)
		rotatePoint2(uvw, angle);

	// Offset to 0..1
	if (t.autoScale) {
		uvw.x += .5f;
		uvw.y += .5f;
	}

	// Return
	tp.uvw = uvw;
}

// All parameters are in world space (p,edges,width,height)
// edges = left top right down

static void offsetEdges(float edges[4], float x, float y) {
	edges[0] += x;
	edges[2] += x;
	edges[3] += y;
	edges[1] += y;
}

TilePoint Tile::drawTile(Point3 p, float edges[4], TileParam& t, int id, int dir) {
	float hEdgeW = t.edgeWidth * .5f;
	float hEdgeH = t.edgeHeight * .5f;
			
	float randomSeed = (float)id * 1.23f + 0.1234f;
	if (dir) {
		edges[0] += t.eH_var ? hEdgeH * (1.f + noise(edges[0], randomSeed) * t.edgeHeightVar) : hEdgeH;
		edges[2] -= t.eH_var ? hEdgeH * (1.f + noise(edges[2], randomSeed) * t.edgeHeightVar) : hEdgeH;
		edges[3] += t.eW_var ? hEdgeW * (1.f + noise(edges[3], randomSeed) * t.edgeWidthVar) : hEdgeW;
		edges[1] -= t.eW_var ? hEdgeW * (1.f + noise(edges[1], randomSeed) * t.edgeWidthVar) : hEdgeW;
	} else {
		edges[0] += t.eW_var ? hEdgeW * (1.f + noise(edges[0], randomSeed) * t.edgeWidthVar) : hEdgeW;
		edges[2] -= t.eW_var ? hEdgeW * (1.f + noise(edges[2], randomSeed) * t.edgeWidthVar) : hEdgeW;
		edges[3] += t.eH_var ? hEdgeH * (1.f + noise(edges[3], randomSeed) * t.edgeHeightVar) : hEdgeH;
		edges[1] -= t.eH_var ? hEdgeH * (1.f + noise(edges[1], randomSeed) * t.edgeHeightVar) : hEdgeH;
	}

	if (p.x < edges[0]) return TilePoint();
	if (p.x > edges[2]) return TilePoint();
	if (p.y < edges[3]) return TilePoint();
	if (p.y > edges[1]) return TilePoint();

	float width = edges[2] - edges[0];
	float height = edges[1] - edges[3];
	if (width < 0 || height < 0) return TilePoint();

	TilePoint tp = corner(p.x - edges[0], p.y - edges[3], width, height, t);	
	if (tp.d < 0) return tp; // On edge

	//if (t.tileID || t.mapUV)
		tp.id = id;

	if (t.center || (t.mapUV && dir))
		tp.center = Point3(edges[0] + (edges[2] - edges[0]) * .5f,
		                   edges[3] + (edges[1] - edges[3]) * .5f, p.z);

	if (dir) {
		offsetEdges(edges, -tp.center.x, -tp.center.y);
		rotate90(edges[0], edges[3]);
		rotate90(edges[2], edges[1]);
		offsetEdges(edges, tp.center.x, tp.center.y);
		p -= tp.center; rotate90(p.x, p.y); p += tp.center;
	}
	
	if (t.mapUV)
		uvMapping(tp, p, edges, t, dir);

	if (dir) {
		offsetEdges(edges, -tp.center.x, -tp.center.y);
		rotate270(edges[0], edges[3]);
		rotate270(edges[2], edges[1]);
		offsetEdges(edges, tp.center.x, tp.center.y);
		p -= tp.center; rotate270(p.x, p.y); p += tp.center;
	}

	return tp;	
}

static int rowcol(float& low, float& high, int& id, float pos, float total, std::vector<float>& arr, float size, float var, float rand) {
	int num = int(arr.size());
	float h = total * size;
	float y = pos / h;
	float yi = (float)FASTFLOOR(y); // group ID
	y = pos - yi * h; // pos within group

	float sumY = 0.f;
	float tileHeight = 0.f;
	int cur = 0;	
	while (cur < num) {		
		tileHeight = arr[cur] * size;
		if (y < sumY + tileHeight)
			break;		
		sumY += tileHeight;
		cur++;
	}
	if (cur < 0 || cur >= num) // Necessary due floating point error
		return -1;

	id = yi * num + cur;

	// Variance
	// ----------- To determine max var
	// ----------- If we rose
	// ----------- eTop -> neTop
	//    x
	// ----------- eBot -> neBot
	// ----------- If we droped
	// ----------- To determine max var

	float eBot = yi * h + sumY;	
	float eTop = eBot + tileHeight;

	if (var > 0.0001f) {
		float n;
		
		float bTileHeight = arr[STEPDOWN(cur, num)]*size;
		float tTileHeight = arr[STEPUP(cur, num)]*size;

		n = noise(eBot, rand);
		float neBot = eBot + (n>0 ? tileHeight : bTileHeight) * n * var;		
		n = noise(eTop, rand);
		float neTop = eTop + (n<0 ? tileHeight : tTileHeight) * n * var;

		if (pos < neBot) { // We droped one cur down
			id--;
			cur = STEPDOWN(cur, num);
			low = eBot-bTileHeight;
			n = noise(low, rand);
			low += (n>0 ? bTileHeight : arr[STEPDOWN(cur, num)]*size) * n * var;
			high = neBot;				

		} else if (pos > neTop) { // We rose one cur up
			id++;
			cur = STEPUP(cur, num);
			high = eTop+tTileHeight;
			n = noise(high, rand);
			high += (n<0 ? tTileHeight : arr[STEPUP(cur, num)]*size) * n * var;
			low = neTop;		

		} else { // Still in the same cur
			low = neBot;
			high = neTop;				
		}
	} else {
		low = eBot;
		high = eTop;
	}

	return cur;
}

TilePoint Tile::pat_xBond(Point3 p, TileParam& t) {
	TilePattern* pat = t.pattern;

	if (!pat) return TilePoint();

	float edges[4];
	int id = 0;
	int id2 = 0;
	
	// Tile top and bottom
	float rand = 3.14f;
	int row = rowcol(edges[3], edges[1], id, p.y, pat->totalHeight, pat->heights, t.tileHeight, t.tileHeightVar, rand);
	if (row == -1) return TilePoint();

	// Tile sides
	rand = edges[3] * 1.325f + 31.41213f;
	float offset = pat->rows[row].offset * t.tileWidth;
	if (offset < 0) offset *= -id;

	row = rowcol(edges[0], edges[2], id2, p.x + offset, pat->rows[row].totalWidth, pat->rows[row].tiles, t.tileWidth, t.tileWidthVar, rand);		
	if (row == -1) return TilePoint();

	edges[0] -= offset;
	edges[2] -= offset;

	id = generateID(id2, id);

	// Draw it
	return drawTile(p, edges, t, id);	
}

static unsigned char pat_herring_dir[] = {
	0, 0, 1, 1,
	0, 1, 1, 0,
	1, 1, 0, 0,
	1, 0, 0, 1	
};

static float pat_herring_x[] = {
	1.0f, 1.0f, 2.5f, 3.5f,
	0.0f, 1.5f, 2.5f, 4.0f,
	0.5f, 1.5f, 3.0f, 3.0f,
	0.5f, 2.0f, 2.0f, 3.5f		
};

static float pat_herring_y[] = {	
	0.5f, 0.5f, 1.0f, 0.0f,
	1.5f, 2.0f, 1.0f, 1.5f,
	3.0f, 2.0f, 2.5f, 2.5f,
	3.0f, 3.5f, 3.5f, 4.0f
};

static unsigned char pat_herring_id[] = {
	1, 1, 1, 0,
	0, 2, 1, 4,
	3, 2, 3, 3,
	3, 2, 2, 4	
};

TilePoint Tile::pat_herring(Point3 p, TileParam& t) {
	float s = t.tileWidth * 2.f;	
	float h = t.tileWidth * .5f;
	float x = p.x / s;
	float y = p.y / s;
	int xi = FASTFLOOR(x);
	int yi = FASTFLOOR(y);
	x = xi * s;
	y = yi * s;
	int x_id = (int)((p.x - x) / h) % 4;
	int y_id = (int)((p.y - y) / h) % 4;	
	int id = x_id + 4 * y_id;

	//Point3 center = Point3(pat_herring_x[id]*h+x, pat_herring_y[id]*h+y, p.z);

	float edges[4];

	if (!pat_herring_dir[id]) {
		edges[0] = (pat_herring_x[id]-1.f) * h + x;
		edges[2] = edges[0] + t.tileWidth;
		edges[3] = (pat_herring_y[id]-.5f) * h + y;
		edges[1] = edges[3] + h;
	} else {
		edges[0] = (pat_herring_x[id]-.5f) * h + x;
		edges[2] = edges[0] + h;
		edges[3] = (pat_herring_y[id]-1.f) * h + y;
		edges[1] = edges[3] + t.tileWidth;	
	}

	int tid = generateID(xi*4 + (pat_herring_dir[id] ? 0 : pat_herring_id[id]),
						 yi*4 + (pat_herring_dir[id] ? pat_herring_id[id] : 0));			
	
	return drawTile(p, edges, t, tid, pat_herring_dir[id]);
}

void TilePattern::setPreset(int preset) {
	switch (preset) {
		case 0: setPattern(L"0, 1, 1"); break; // Stack
		case 1: setPattern(L"0, 1, 1 / 0.5, 1, 1"); break; // Stretcher
		case 2: setPattern(L".25, 1, 1, .5 / -.5, 1, 1 / -.5, 1, 1 / -.5, 1, 1 / -.5, 1, 1"); break; // Flemish Strecher
		case 3: setPattern(L".25, 1, .5 / -.5, 1, 1 / -.5, 1, 1 / -.5, 1, 1 / -.5, 1, 1"); break; // Common
		case 4: setPattern(L"0, 1, 1, .5 / .75, 1, 1, .5"); break; // Flemish
		case 5: setPattern(L"0, 1, 1, 1, .5 / 1.25, 1, 1, 1, .5"); break; // Monk Bond
		case 6: setPattern(L"0, 1, 1, 1, 1, .5 / 1.75, 1, 1, 1, 1, .5"); break; // Flemish Garden Wall
		case 7: setPattern(L".25, 1, .5 / 0, 1, 1"); break; // English
		case 8: setPattern(L"0,1,1 / -.25,1,.5 / -.25,1,1 / -.25,1,.5"); break; // English Cross
		case 9: setPattern(L"0,1,.5 / -.25,1,.5 / -.25,1,1 / -.25,1,1"); break; // Double English Cross
	}
}

TilePoint Tile::draw(Point3 p, TileParam& t) {	
	if (t.tilingType == 1)
		return pat_herring(p, t);
	return pat_xBond(p, t);
}

static void tokenize(const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters) {
	std::wstring::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::wstring::size_type pos = str.find_first_of(delimiters, lastPos);
	
	while (std::wstring::npos != pos || std::wstring::npos != lastPos) {
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}

static int parsePattern(std::wstring str, TilePattern* pat) {
	pat->rows.clear();
	pat->heights.clear();

	std::vector<std::wstring> rowStrings = std::vector<std::wstring>();
	tokenize(str, rowStrings, L"/");

	for (int i=0; i<rowStrings.size(); i++) {		
		std::vector<std::wstring> row = std::vector<std::wstring>();
		tokenize(rowStrings[i], row, L",");
		
		if (row.size() < 3)
			return FALSE;	

		double offset, height, width;
		std::wstringstream s0(row[0]);
		s0 >> offset;
		std::wstringstream s1(row[1]);
		s1 >> height;

		TileRow r = TileRow(offset);

		for (int j=2; j<row.size(); j++) {
			std::wstringstream sn(row[j]);
			sn >> width;

			r.tiles.push_back(width);
		}

		pat->rows.push_back(r);
		pat->heights.push_back((float)height);
	}

	return TRUE;
}

void TilePattern::setPattern(std::wstring s) {
	parsePattern(s, this);
	update();
}
