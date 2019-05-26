/*
	* Cloudy Pen: Drawing tools for multiplaying
	* 2019. 05. 01, Stynel(@Lostynel)
	* Email: ltnyst10@gmail.com
*/

#include "definelist.h"
#include "Layouts.h"
#include "DrawingTools.h"
#include <stdlib.h>

// #pragma comment (lib, "lib_32.dll")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK COLORPanelProc(HWND, UINT, WPARAM, LPARAM); // 색상 선택하는 창에 관한 함수
LRESULT CALLBACK PAPERPanelProc(HWND, UINT, WPARAM, LPARAM); // 그림 영역에 관한 함수
LRESULT CALLBACK LoginProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CSZPROC(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SAVEPROC(HWND, UINT, WPARAM, LPARAM);

BOOL check_login = FALSE;
COLORREF gColor = RGB(255, 255, 255);
HINSTANCE hinstance;
int now_mode = 1;
HWND hwndPanel;
HWND paperPanel;
object_polygon* head_saving_path;
object_polygon* tail_saving_path;
object_polygon* recent_node;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	MSG  msg;
	WNDCLASSW wc = { 0 };

	wc.lpszClassName = L"CP";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	RegisterClassW(&wc);
	INIT_POLYGON(&head_saving_path, &tail_saving_path);
	recent_node = head_saving_path;

	CreateWindowW(wc.lpszClassName, L"CLOUDY PEN: beta 0.0", 
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100, 100, 350, 250, 0, 0, hInstance, 0);

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
	case WM_CREATE:
		AddMenus(hwnd);
		AddButtons(hwnd, &hwndPanel, &paperPanel);
		break;
	case WM_COMMAND:
		if(LOWORD(wParam) >= MODE_PEN && LOWORD(wParam) <= MODE_FILL)
			now_mode = LOWORD(wParam);
		if (LOWORD(wParam) == MODE_UNDO)
		{
			if (recent_node != head_saving_path)
			{
				UNDO_PEN(recent_node, paperPanel);
				recent_node = recent_node->prev;
			}
		}
		if (LOWORD(wParam) == MODE_REDO)
		{
			if (recent_node->next != tail_saving_path)
			{
				recent_node = recent_node->next;
				REDO_PEN(recent_node, paperPanel);
			}
		}
		if (LOWORD(wParam) >= MENU_MULTI_CREATE && LOWORD(wParam) <= MENU_MULTI_FRIEND)
		{
			if (!check_login)
				LoginBox(hwnd, &check_login, hinstance);

		}
		if (LOWORD(wParam) == CHANGING_SIZE)
			CGSZBOX(hwnd, hinstance);
		if (LOWORD(wParam) == MENU_FILE_SAVE)
			SAVEBOX(hwnd, hinstance);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK COLORPanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	switch (msg) 
	{
	case WM_LBUTTONDOWN:
		gColor = ShowColorDialog(hwnd);
		InvalidateRect(hwnd, NULL, TRUE);
		break;

	case WM_PAINT:
		GetClientRect(hwnd, &rect);
		hdc = BeginPaint(hwnd, &ps);
		SetBkColor(hdc, gColor);
		ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, "", 0, NULL);
		EndPaint(hwnd, &ps);
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK PAPERPanelProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	object_polygon *ggaz;
	static int sx, sy;
	static int oldx, oldy;
	static BOOL bnowDraw = FALSE;

	switch (msg) 
	{
	case WM_LBUTTONDOWN:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		oldx = sx;
		oldy = sy;
		bnowDraw = TRUE;
		hdc = GetDC(hwnd);
		if (now_mode != MODE_SPOID)
		{
			if (recent_node->next != tail_saving_path)
				Deleting_After(recent_node);

			ADD_POLYGON(&tail_saving_path, &recent_node);
		}
		else
		{
			gColor = GetPixel(hdc, sx, sy);
		}
		if(now_mode != MODE_CIRCLE)
			ADD_PATH(recent_node, sx, sy, GetPixel(hdc, sx, sy), gColor);

		if(now_mode == MODE_FILL)
			b_Filling(hdc, sx, sy, gColor, GetPixel(hdc, sx, sy), 0, 0, 0, recent_node);
		else if(now_mode != MODE_CIRCLE)
			SetPixel(hdc, sx, sy, gColor);
		ReleaseDC(hwnd, hdc);
		break;

	case WM_MOUSEMOVE:
		if (bnowDraw == TRUE) 
		{
			int now_x, now_y;
			hdc = GetDC(hwnd);
			now_x = LOWORD(lParam);
			now_y = HIWORD(lParam);
			if (now_mode == MODE_PEN && !(now_x == oldx && now_y == oldy))
			{
				Drawing_b_LINE(hdc, oldx, oldy, now_x, now_y, gColor, recent_node, TRUE, FALSE);
			}
			else if (now_mode == MODE_RECT || now_mode == MODE_LINE || now_mode == MODE_CIRCLE)
			{
				SetROP2(hdc, R2_NOT);
				if (now_mode == MODE_RECT)
				{
					Drawing_b_RECT(hdc, sx, sy, oldx, oldy, gColor, NULL, FALSE, FALSE);
					Drawing_b_RECT(hdc, sx, sy, now_x, now_y, gColor, NULL, FALSE, FALSE);
				}
				else if (now_mode == MODE_LINE)
				{
					Drawing_b_LINE(hdc, sx, sy, oldx, oldy, gColor, NULL, FALSE, FALSE);
					Drawing_b_LINE(hdc, sx, sy, now_x, now_y, gColor, NULL, FALSE, FALSE);
				}
				else if (now_mode == MODE_CIRCLE)
				{
					Drawing_b_CIRCLE(hdc, sx, sy, getRadius(sx, sy, oldx, oldy), gColor, NULL, FALSE, FALSE);
					Drawing_b_CIRCLE(hdc, sx, sy, getRadius(sx, sy, now_x, now_y), gColor, NULL, FALSE, FALSE);
				}
			}
			oldx = now_x;
			oldy = now_y;
			ReleaseDC(hwnd, hdc);
		}
		break;
	case WM_LBUTTONUP:
		bnowDraw = FALSE;
		hdc = GetDC(hwnd);
		if(now_mode == MODE_RECT || now_mode == MODE_LINE || now_mode == MODE_CIRCLE)
		{
			SelectObject(hdc, GetStockObject(NULL_BRUSH));
			int now_x, now_y;
			now_x = LOWORD(lParam);
			now_y = HIWORD(lParam);
			if (now_mode == MODE_RECT)
				Drawing_b_RECT(hdc, sx, sy, now_x, now_y, gColor, recent_node, TRUE, TRUE);
			else if (now_mode == MODE_LINE)
				Drawing_b_LINE(hdc, sx, sy, now_x, now_y, gColor, recent_node, TRUE, TRUE);
			else if (now_mode == MODE_CIRCLE)
				Drawing_b_CIRCLE(hdc, sx, sy, getRadius(sx, sy, now_x, now_y), gColor, recent_node, TRUE, TRUE);
		}
		
		ReleaseDC(hwnd, hdc);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		ggaz = head_saving_path;
		while (1)
		{
			ggaz = ggaz->next;
			if (ggaz == tail_saving_path || ggaz == recent_node->next)
				break;
			else
				REDO_PEN(ggaz, hwnd);
		}
		EndPaint(hwnd, &ps);
		break;
	}
	return(DefWindowProc(hwnd, msg, wParam, lParam));
}


LRESULT CALLBACK LoginProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{

	case WM_CREATE:
		CreateWindowW(L"button", L"로그인",
			WS_VISIBLE | WS_CHILD,
			50, 50, 80, 25, hwnd, (HMENU)1, NULL, NULL);
		break;
	case WM_COMMAND:
		DestroyWindow(hwnd);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	}
	return(DefWindowProc(hwnd, msg, wParam, lParam));
}

LRESULT CALLBACK CSZPROC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		CreateWindowW(L"button", L"32px",
			WS_VISIBLE | WS_CHILD,
			50, 30, 30, 25, hwnd, (HMENU)1, NULL, NULL);
		CreateWindowW(L"button", L"64px",
			WS_VISIBLE | WS_CHILD,
			85, 30, 30, 25, hwnd, (HMENU)1, NULL, NULL);
		CreateWindowW(L"button", L"128px",
			WS_VISIBLE | WS_CHILD,
			115, 30, 30, 25, hwnd, (HMENU)1, NULL, NULL);
		CreateWindowW(L"button", L"256px",
			WS_VISIBLE | WS_CHILD,
			150, 30, 30, 25, hwnd, (HMENU)1, NULL, NULL);
		break;
	case WM_COMMAND:
		DestroyWindow(hwnd);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	}
	return(DefWindowProc(hwnd, msg, wParam, lParam));
}


LRESULT CALLBACK SAVEPROC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, memDC;
	OPENFILENAME ofn;
	HBITMAP hBit, oldBit;
	COLORREF color;
	TCHAR lpstrFile[MAX_PATH] = TEXT("");
	int check = 0;
	switch (msg)
	{
	case WM_CREATE:
		CreateWindowW(L"button", L"저장하기",
			WS_VISIBLE | WS_CHILD,
			5, 5, 75, 25, hwnd, (HMENU)1, NULL, NULL);
		break;
	case WM_COMMAND:
		hdc = GetDC(paperPanel);
		memDC = CreateCompatibleDC(hdc);
		hBit = CreateCompatibleBitmap(hdc, 500, 350);
		oldBit = (HBITMAP)SelectObject(memDC, hBit);
		BitBlt(memDC, 0, 0, 500, 350, hdc, 0, 0, SRCCOPY);
		SelectObject(memDC, oldBit);
		DeleteObject(hdc);
		DeleteObject(memDC);
		memset(&ofn, 0, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFilter = TEXT("Bmp File(*Bmp)\0");
		ofn.lpstrFile = lpstrFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrDefExt = TEXT("bmp");
		ofn.lpstrTitle = TEXT("내마음속에저장");
		if (GetOpenFileName(&ofn) != 0)
		{
			HBITMAP2BMP(hBit, ofn.lpstrFile);
		}
		DeleteObject(hBit);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	}
	return(DefWindowProc(hwnd, msg, wParam, lParam));
}