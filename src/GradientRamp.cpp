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

/*

#include "Max.h"
#include "resource.h"
#include "plugapi.h"
#include "3dsmaxport.h"

#include "maxscrpt/MAXScrpt.h"*/

#include "GradientRamp.h"


void GradientRamp::setHWND(HWND hWnd) {
	m_hWnd = hWnd; 
	RECT rect;
	GetWindowRect(m_hWnd, &rect);													
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;	
}

// #############################################################################################
// #################################/ Paint                    \################################
// #############################################################################################

void GradientRamp::paint(HDC hDC) {	
	if (m_hWnd == NULL) return;
	//SetBkMode(hDC, TRANSPARENT);	

	//RECT rect;
	//GetClientRect(m_hWnd, &rect);
	
	/*SelectObject(hdc,red_pen);
	MoveToEx(hdc,10,20,NULL);
	LineTo(hdc,10,40,NULL);
	LineTo(hdc,50,150,NULL);*/
	/*	
	int width = rect.right - rect.left;
	int height = rect.top - rect.bottom;

	RECT rect2;
	GetWindowRect(m_hWnd, &rect2);
									
	CharStream *out = thread_local(current_stdout);
	out->printf("Rectangle: %d %d %d %d\n", rect.left, rect.right, rect.top, rect.bottom);
	out->printf("Rectangle2: %d %d %d %d\n", (int)(rect2.left, (int)(rect2.right), (int)(rect2.top), (int)(rect2.bottom));
	*/

	HDC tempHDC = CreateCompatibleDC(hDC);
	HBITMAP hbm_Buffer = CreateCompatibleBitmap(hDC, width, height);
	HBITMAP hbm_oldBuffer = (HBITMAP)SelectObject(tempHDC, hbm_Buffer);

	// Borders
	RECT rect;	
	HBRUSH gray = CreateSolidBrush(RGB(175,175,175)); 	
	rect.left = 0; rect.top = 0; rect.right = width; rect.bottom = PADDING;
	FillRect(tempHDC,&rect,gray);
	rect.right = PADDING; rect.bottom = height;
	FillRect(tempHDC,&rect,gray);
	rect.left = width-PADDING; rect.right = width;
	FillRect(tempHDC,&rect,gray);
	rect.left = 0; rect.top = height-PADDING;
	FillRect(tempHDC,&rect,gray);

	int g_Width = width - PADDING * 2;
	int g_Height = height - PADDING *2;
	float f_Width = (float)g_Width;	

	// Gradient
	for (int x=PADDING; x<g_Width+PADDING; x++) {
		AColor col = getColor((float)(x-PADDING) / (f_Width-1.f));

		COLORREF colR = RGB((int)(col.r*255.f), (int)(col.g*255.f), (int)(col.b*255.f));		
		for (int y=PADDING; y<g_Height+PADDING; y++)		
			SetPixel(tempHDC, x, y, colR);
	}

	// Keys
	int yTop = PADDING - 1;
	int yBottom = height - PADDING;
	int sel = -1;
	for (int i=0;i<keys;i++) {
		//CharStream *out = thread_local(current_stdout);
		//out->printf("Key pos: %f\n", position[i]);	
		if (number[i] != selected) {
			int x = (int)(f_Width * position[i]) + PADDING;
			paintArrow(x, yTop, false, tempHDC, ARROWDESEL);
			paintArrow(x, yBottom, true, tempHDC, ARROWDESEL);
		} else {
			sel = i;
		}
	}	
	if (sel != -1) {
		paintArrow((int)(f_Width * position[sel]) + PADDING, yTop, false, tempHDC, ARROWSEL);
		paintArrow((int)(f_Width * position[sel]) + PADDING, yBottom, true, tempHDC, ARROWSEL);	
	}

	//GetClientRect(m_hWnd, &rect);
	BitBlt(hDC, 0, 0, width, height, tempHDC, 0, 0, SRCCOPY);
	//BitBlt(hDC, rect.left, rect.top, rect.right, rect.bottom, tempHDC, 0, 0, SRCCOPY);

    SelectObject(tempHDC, hbm_oldBuffer);
 	DeleteDC(tempHDC);
    DeleteObject(hbm_Buffer); 
}

void GradientRamp::paintArrow(int px, int py, bool up, HDC hDC, COLORREF colR) {
	int offset = 0;
	for (int y=0;y<ARROWS;y++) {
		for (int x=0;x<(y*2+1);x++) {
			if (up)
				SetPixel(hDC, px+x-offset, py+y, colR);
			else
				SetPixel(hDC, px+x-offset, py-y, colR);
		}
		offset++;
	}
}

void GradientRamp::invalidate() {
   if (m_hWnd == NULL) return;
   //RECT rect;
   //GetClientRect(m_hWnd, &rect);
   //MapWindowPoints(m_hWnd, p_hWnd, (POINT*)&rect, 2);
   //InvalidateRect(p_hWnd, &rect, TRUE);
   InvalidateRect(m_hWnd, NULL, TRUE);
}

// #############################################################################################
// #################################/ Mouse                    \################################
// #############################################################################################

float GradientRamp::toPos(int x) {
	float pos = 0.f;
	if (x > width - PADDING)
		pos = 1.f;
	else if (x > PADDING)
		pos = (float)(x - PADDING) / (float)(width - PADDING * 2);
	return pos;
}

int GradientRamp::toIndex(int n) {	
	for (int i=0;i<keys;i++)
		if (number[i] == n)
			return i;
	return -1;
}

// 0...N	Key
// -1		On empty gradient
int GradientRamp::hit(int x, int y, bool broad) {
	// On gradient, no keys there
	if (y > PADDING && y < (height - PADDING) && !broad)
		return -1;

	// Distance from gradient
	int dist = 0;
	if (broad)
		dist  = ARROWS;
	else if (y < PADDING)
		dist = PADDING - y;
	else
		dist = y - height + PADDING;	

	//CharStream *out = thread_local(current_stdout);
	// Intersect all keys
	float f_Width = (float)(width - PADDING * 2);	
	for (int i=0;i<keys;i++) {
		int kx = (int)(f_Width * position[i]) + PADDING;
		//out->printf("Hit key: %d %d %d\n", x-dist, x+dist, x);
		if (kx-dist <= x && kx+dist >= x)
			return i;
	}

	// No keys found
	return -1;	
}

void GradientRamp::leftDown(int x, int y, bool ctrl, bool shift, bool alt) {
	int key = hit(x,y,true);

	//CharStream *out = thread_local(current_stdout);
	//out->printf("Hit key: %d %d\n", key, selected);
	//out->printf("Left down\n");

	if (!alt && key >= 0) {
		selectKey(number[key]);
	} else if (key == -1) {
		selectKey(-1);
	}

	//out->printf("Selected key: %d\n", selected);
	//out->printf("## Seletion DONE! ##\n", selected);
}

void GradientRamp::leftUp(int x, int y, bool ctrl, bool shift, bool alt) {
	int key = hit(x,y);	 
	if (alt && key == -1) {
		parent->gradAddKey(toPos(x));
		selected = keys - 1; // New key gets the last id
		parent->gradSelKey();
		
		//CharStream *out = thread_local(current_stdout);
		//out->printf("Key added!\n");	

	} else if (alt && key >= 0) {
		if (selected == number[key]) selected = -1;
		parent->gradDelKey(number[key]);
	}	
}

void GradientRamp::dragging(int x, int y, bool ctrl, bool shift, bool alt) {				
	if (selected <= 1) // Refuse to move ends and empty
		return; 	
	if (selected < keys)
		parent->gradMoveKey(selected, toPos(x));		
	//CharStream *out = thread_local(current_stdout);
	//out->printf("Move key: %d %f\n", number[selected], pos);
}

void GradientRamp::popup(int x, int y, int sel) {
	int key = hit(x,y,true);
	switch (sel) {
		case ID_MENU_ADDKEY:
			if (key == -1) {
				parent->gradAddKey(toPos(x));
				selected = keys - 1; // New key gets the last id
				parent->gradSelKey();
			}
			break;
		case ID_MENU_REMOVEKEY:
			if (key >= 0) {
				if (selected == number[key]) selected = -1;
				parent->gradDelKey(number[key]);
			} else if (selected >= 0) {
				int k = selected;				
				selected = -1;
				parent->gradDelKey(k);
			}
			break;
		case ID_RESET_RESET:
			selected = -1;
			parent->gradReset();
			break;
	}
}


// #############################################################################################
// #################################/ Keys                     \################################
// #############################################################################################

void GradientRamp::selectKey(int n) {
	selected = n;	
	//CharStream *out = thread_local(current_stdout);
	//out->printf("Selection sent (%d)\n", selected);
	parent->gradSelKey();
	//out->printf("Invalidate\n");
	//invalidate();
}

/*
// n is the number of the key
void GradientRamp::moveKey(int n, float pos) {	
	if (n == 0 || n == 1) // Refuse to move ends
		return; 

	for (int i=0;i<keys;i++) 
		if (number[i] == n) {
			position[i] = pos;
			break;
		}	
	sort();
	invalidate();
	keyMoved = true;
	parent->moveKey(n, pos);
}
*/

void GradientRamp::addKey(int n, float pos, AColor col, Texmap* sub) {
	if (pos < 0.f) pos = 0.f;
	else if (pos > 1.f) pos = 1.f;

	int key = -1;
	for (int i=0;i<keys;i++)
		if (number[i] == n)
			key = i;

	if (key >= 0) { // update only, no keys added
		subtex[key] = sub;
		position[key] = pos;
		color[key] = col;
		return;
	}

	keys++;	
	Texmap** t_subtex = new Texmap*[keys];
	float* t_position = new float[keys];
	AColor* t_color = new AColor[keys];
	int* t_number = new int[keys];
	keys--;

	for (int i=0;i<keys;i++) {
		t_subtex[i] = subtex[i];
		t_position[i] = position[i];
		t_color[i] = color[i];
		t_number[i] = number[i];
	}

	t_subtex[keys] = sub;
	t_position[keys] = pos;
	t_color[keys] = col;
	t_number[keys] = n;

	delete[] subtex;
	delete[] position;
	delete[] color;
	delete[] number;

	subtex = t_subtex;
	position = t_position;
	color = t_color;
	number = t_number;

	keys++;		
}

void GradientRamp::reset() {		
	keys = 0;
	
	if (subtex) delete[] subtex;
	if (position) delete[] position;
	if (color) delete[] color;
	if (number) delete[] number;
	
	subtex = NULL;
	position = NULL;
	color = NULL;
	number = NULL;

	addKey(0, 0.f, AColor(0.f,0.f,0.f,1.f), NULL);
	addKey(1, 1.f, AColor(1.f,1.f,1.f,1.f), NULL);	
}

void GradientRamp::swap(int a, int b) {
	Texmap* sub	= subtex[a];
	float pos	= position[a];
	AColor col	= color[a];
	int num		= number[a];
	
	subtex[a]	= subtex[b];
	position[a] = position[b];
	color[a]	= color[b];
	number[a]	= number[b];

	subtex[b]	= sub;
	position[b] = pos;
	color[b]	= col;
	number[b]	= num;
}

void GradientRamp::sort() {
	int i = 1;	
	while (i < keys) {
		if (position[i-1] <= position[i]) {
	        i++;			
		} else {
			swap(i-1, i);
			i--;
			if (i <= 0)
				i = 1;
		}
	} 
}

// #############################################################################################
// #################################/ Subtex                   \################################
// #############################################################################################

Texmap* GradientRamp::getSubtex(int n) {
	if (n == -1)
		if (selected >= 0 && selected < keys)
			return subtex[toIndex(selected)];
		else
			return NULL;
	else
		return subtex[toIndex(n)]; 
}


void GradientRamp::setSubtex(int n, Texmap* sub) {
	subtex[toIndex(n)] = sub;
}

void GradientRamp::setSubtex(Texmap* sub) { 
	if (selected >= 0 && selected < keys)
		setSubtex(selected, sub);
}	

// #############################################################################################
// #################################/ Color                    \################################
// #############################################################################################

// Linear Search
// O(N)
/*
int GradientRamp::findHighKey(float x) {
	int high = 1; // Always 2 keys so we start from key number two since they are ordered by position	
	while (x > position[high]) {
		high++;
		if (high >= keys)
			return (keys - 1);					
	}	
	return high;
}
*/

// More intelligent search
// O(log N)
int GradientRamp::findHighKey(float x) {
	int low = 0;
	int high = keys-1;
	int mid;	
	if (x < position[low])
		return low;
	while (low < (high-1)) {
		mid = (low + high) / 2;
		if (x < position[mid])
			high  = mid;
		else
			low = mid;		
	}
	return high;
}

float GradientRamp::interpolate(float x, float low, float high) {
	//int interpolation; // 0 linear 1 smooth 2 solid near 3 solid left 4 solid right	
	switch (interpolation) {
		case 0:	
			return (x - low) / (high - low);
		case 1:	
			x = (x-low)/(high-low);
			if (x < 0.f) x = 0.f;
			else if (x > 1.f) x = 1.f;
			return (x*x*(3.f-2.f*x));
	}
	return 0.f;
}

AColor GradientRamp::getColor(float x) {
	if (x<=0) return color[0];
	if (x>=1) return color[keys-1];
	int high = findHighKey(x);
	if (interpolation == 4) return color[high];
	int low = (high > 0 ? high - 1 : 0); // should never happen but just in case
	if (interpolation == 3) return color[low];
	if (interpolation == 2) return color[x-position[low]<position[high]-x?low:high];
	float mult = interpolate(x, position[low], position[high]);
	return color[low] * (1.f - mult) + color[high] * mult;
}

AColor GradientRamp::getColor(float x, ShadeContext& sc) {
	if (x<=0) return subtex[0]?subtex[0]->EvalColor(sc):color[0];
	if (x>=1) return subtex[keys-1]?subtex[keys-1]->EvalColor(sc):color[keys-1];
	int high = findHighKey(x);
	if (interpolation == 4) return subtex[high]?subtex[high]->EvalColor(sc):color[high];
	int low = (high > 0 ? high - 1 : 0); // should never happen but just in case
	if (interpolation == 3) return subtex[low]?subtex[low]->EvalColor(sc):color[low];	
	if (interpolation == 2) {		
		int key = x-position[low]<position[high]-x?low:high;
		return subtex[key]?subtex[key]->EvalColor(sc):color[key];
	}
	float mult = interpolate(x, position[low], position[high]);
	return (subtex[low]?subtex[low]->EvalColor(sc):color[low])
		* (1.f - mult) + (subtex[high]?subtex[high]->EvalColor(sc):color[high]) * mult;
}

#define NOBUMP Point3(0.f,0.f,0.f)
Point3 GradientRamp::getBump(float x, Point3 normal, ShadeContext& sc) {
	if (x<=0) return subtex[0]?subtex[0]->EvalNormalPerturb(sc):NOBUMP;
	if (x>=1) return subtex[keys-1]?subtex[keys-1]->EvalNormalPerturb(sc):NOBUMP;
	int high = findHighKey(x);
	if (interpolation == 4) return subtex[high]?subtex[high]->EvalNormalPerturb(sc):NOBUMP;
	int low = high - 1;	
	if (interpolation == 3) return subtex[low]?subtex[low]->EvalNormalPerturb(sc):NOBUMP;	
	if (interpolation == 2) {		
		int key = x-position[low]<position[high]-x?low:high;
		return subtex[key]?subtex[key]->EvalNormalPerturb(sc):NOBUMP;
	}

	//float f1, f2;
	Point3 v1, v2;
	bool maps = false;
	if (subtex[0]) {
		//f1 = subtex[low]->EvalMono(sc);
		v1 = subtex[low]->EvalNormalPerturb(sc);
		maps = true;
	} else {
		//f1 = Intens(color[low]);
		v1 = Point3(0.f, 0.f, 0.f);
	}
	if (subtex[1]) {
		//f2 = subtex[high]->EvalMono(sc);
		v2 = subtex[high]->EvalNormalPerturb(sc);
		maps = true;
	} else {
		//f2 = Intens(color[high]);
		v2 = Point3(0.f, 0.f, 0.f);
	}
	
	// Calculate vector	
	if (maps) {
		float mult = interpolate(x, position[low], position[high]);
		v1 = /*(f2-f1)**/ normal + mult*v2 + (1.f-mult)*v1;
	} else {
		v1 = normal; // * (f2 - f1);
	}

	return v1;
}
