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

#include "IGradient.h"

/*
mouse input

    * MK_LBUTTON - left
    * MK_MBUTTON - middle
    * MK_RBUTTON - right
    * MK_CONTROL - Ctrl pressed
    * MK_SHIFT - shift pressed

	To detect that the ALT key was pressed, check whether GetKeyState(VK_MENU) < 0. Note, this must not be GetAsyncKeyState.

int mouse_x = (int)LOWORD(lparam);
int mouse_y = (int)HIWORD(lparam); 
// get the button state
int buttons = (int)wparam; 
sprintf(buffer,"Right Button = %d ",((buttons & MK_RBUTTON) ? 1 : 0)); 

*/

LRESULT CALLBACK IGradient::GradientProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
   IGradient *grad = DLGetWindowLongPtr<IGradient*>(hWnd);
   if(grad == NULL && msg != WM_CREATE)
      return DefWindowProc(hWnd, msg, wParam, lParam);

   switch(msg) {	   
		case WM_CREATE:      {
			auto lpcs = (LPCREATESTRUCT)lParam;
			grad = (IGradient*)lpcs->lpCreateParams;
			DLSetWindowLongPtr(hWnd, grad);
			break;
		}
	   /*
		case WM_KILLFOCUS:
			if (ctrl->m_bMouseOver)	{
				ctrl->m_bMouseOver = false;
				ctrl->run_event_handler(n_mouseout, (short)LOWORD(lParam), (short)HIWORD(lParam), wParam, false);
			}
		break;
		 */
		case WM_MOUSEMOVE: {
			int buttons = (int)wParam; 
			if (buttons & MK_LBUTTON) {				
				int mouse_x = (int)LOWORD(lParam);
				int mouse_y = (int)HIWORD(lParam); 
				grad->gradient->dragging(mouse_x, mouse_y, buttons&MK_CONTROL, buttons&MK_SHIFT, GetKeyState(VK_MENU) < 0);
				//CharStream *out = thread_local(current_stdout);
				//out->printf("Dragging %d %d\n", mouse_x, mouse_y);
			}
			break;
		}
		 
		case WM_LBUTTONDOWN: {
			CharStream *out = thread_local(current_stdout);
			
			int buttons = (int)wParam; 
			int mouse_x = (int)LOWORD(lParam);
			int mouse_y = (int)HIWORD(lParam); 

			grad->gradient->leftDown(mouse_x, mouse_y, buttons&MK_CONTROL, buttons&MK_SHIFT, GetKeyState(VK_MENU) < 0);

			/*
			if (buttons & MK_CONTROL) {	
				out->printf("Left mouse DOWN + control\n");
			} else if (buttons & MK_SHIFT) {
				out->printf("Left mouse DOWN + shift\n");
			} else {
				out->printf("Left mouse DOWN %d %d\n", mouse_x, mouse_y);
			}*/

			break;
		}

		case WM_LBUTTONUP: {
			CharStream *out = thread_local(current_stdout);
			
			int buttons = (int)wParam; 
			int mouse_x = (int)LOWORD(lParam);
			int mouse_y = (int)HIWORD(lParam); 
			grad->gradient->leftUp(mouse_x, mouse_y, buttons&MK_CONTROL, buttons&MK_SHIFT, GetKeyState(VK_MENU) < 0);

			/*
			if (buttons & MK_CONTROL) {	
				out->printf("Left mouse + control\n");
			} else if (buttons & MK_SHIFT) {
				out->printf("Left mouse + shift\n");
			} else {
				out->printf("Left mouse\n");
			}  */
			break;
		}

		case WM_RBUTTONUP: {
			HMENU Popup;
			POINT pt;
			Popup = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_GRADIENTMENU));
			Popup = GetSubMenu(Popup, 0);
			GetCursorPos(&pt);
			int sel = TrackPopupMenuEx(Popup, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, hWnd, NULL);
			if (sel > 0) {
				int mouse_x = (int)LOWORD(lParam);
				int mouse_y = (int)HIWORD(lParam); 
				grad->gradient->popup(mouse_x, mouse_y, sel);
			}
			break;
		}

		case WM_MBUTTONUP: {
			/*
			CharStream *out = thread_local(current_stdout);

			int buttons = (int)wParam; 
			if (buttons & MK_CONTROL) {	
				out->printf("Middle mouse + control\n");
			} else if (buttons & MK_SHIFT) {
				out->printf("Middle mouse + shift\n");
			} else {
				out->printf("Middle mouse\n");
			}  */
			break;
		}
								 
		   /*
		case WM_LBUTTONUP:
			return ctrl->ButtonUp((short)LOWORD(lParam), (short)HIWORD(lParam), wParam, left);

		case WM_LBUTTONDBLCLK:
			return ctrl->ButtonDblClk((short)LOWORD(lParam), (short)HIWORD(lParam), wParam, left);

		case WM_RBUTTONDOWN:
			return ctrl->ButtonDown((short)LOWORD(lParam), (short)HIWORD(lParam), wParam, right);

		case WM_RBUTTONUP:
			return ctrl->ButtonUp((short)LOWORD(lParam), (short)HIWORD(lParam), wParam, right);

		case WM_RBUTTONDBLCLK:
			return ctrl->ButtonDblClk((short)LOWORD(lParam), (short)HIWORD(lParam), wParam, right);

		case WM_MBUTTONDOWN:
			return ctrl->ButtonDown((short)LOWORD(lParam), (short)HIWORD(lParam), wParam, middle);

		case WM_MBUTTONUP:
			return ctrl->ButtonUp((short)LOWORD(lParam), (short)HIWORD(lParam), wParam, middle);

		case WM_MBUTTONDBLCLK:
			return ctrl->ButtonDblClk((short)LOWORD(lParam), (short)HIWORD(lParam), wParam, middle);

		case WM_ERASEBKGND:
			return ctrl->EraseBkgnd((HDC)wParam);
		 
		 */
		case WM_PAINT: {			
			//CharStream *out = thread_local(current_stdout);
			//out->printf("!!Paint!!\n");
			PAINTSTRUCT ps;
			BeginPaint(hWnd,&ps);
			grad->gradient->paint(ps.hdc);
			EndPaint(hWnd,&ps);
			return FALSE;
		}
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return FALSE;
}

void InitGradientControls() {
	WNDCLASSEX wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.hCursor = LoadCursor(NULL, /*IDC_CROSS*/IDC_ARROW);
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = IGradient::GradientProc; //DlgProc control
	wc.lpszClassName = L"CustGradient";
	wc.lpszMenuName = NULL;
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.cbSize = sizeof(wc);
	RegisterClassEx(&wc);
}
