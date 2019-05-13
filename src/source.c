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
LRESULT CALLBACK COLORPanelProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PAPERPanelProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CSZPROC(HWND, UINT, WPARAM, LPARAM);

COLORREF gColor = RGB(255, 255, 255);
HINSTANCE hinstance;
BOOL check_login = FALSE;
int now_mode = 1;
object_polygon* head_saving_path;
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

	head_saving_path = (object_polygon*)malloc(sizeof(object_polygon));
	INIT_POLYGON(&head_saving_path);

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
	static HWND hwndPanel;
	static HWND paperPanel;

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
				UNDO_PEN(recent_node, hwnd);
				recent_node = recent_node->prev;
			}
		}
		if (LOWORD(wParam) == MODE_REDO)
		{
			if (recent_node -> next != NULL)
			{
				recent_node = recent_node->next;
				REDO_PEN(recent_node, hwnd);
			}
		}
		if (LOWORD(wParam) >= MENU_MULTI_CREATE && LOWORD(wParam) <= MENU_MULTI_FRIEND)
		{
			if (!check_login)
				LoginBox(hwnd, &check_login, hinstance);

		}
		if (LOWORD(wParam) == CHANGING_SIZE)
			CGSZBOX(hwnd, hinstance);
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
	HPEN hpen;
	static int sx, sy;
	static int oldx, oldy;
	static BOOL bnowDraw = FALSE;
	static object_polygon* new_node = NULL;

	switch (msg) 
	{
	case WM_LBUTTONDOWN:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		oldx = sx;
		oldy = sy;
		bnowDraw = TRUE;
		hdc = GetDC(hwnd);
		if (head_saving_path->next->path == NULL)
		{
			if (now_mode != MODE_FILL)
				ADD_PATH(head_saving_path->next, sx, sy, GetPixel(hdc, sx, sy), gColor);
			else
				ADD_PATH(head_saving_path->next, -1, -1, GetPixel(hdc, sx, sy), gColor);
			recent_node = head_saving_path->next;
		}
		else
		{
			new_node = ADD_POLYGON(&head_saving_path);
			recent_node = new_node;
			if (now_mode != MODE_FILL)
				ADD_PATH(new_node, sx, sy, GetPixel(hdc, sx, sy), gColor);
			else
				ADD_PATH(new_node, -1, -1, GetPixel(hdc, sx, sy), gColor);
		}
		if(now_mode == MODE_FILL)
		{
			HBRUSH fillbrush;
			fillbrush = CreateSolidBrush(gColor);
			SelectObject(hdc, fillbrush);
			if (new_node == NULL)
				ADD_PATH(head_saving_path->next, sx, sy, GetPixel(hdc, sx, sy), gColor);
			else
				ADD_PATH(new_node, sx, sy, GetPixel(hdc, sx, sy), gColor);
			ExtFloodFill(hdc, sx, sy, GetPixel(hdc, sx, sy), FLOODFILLSURFACE);
			DeleteObject(fillbrush);
		}
		else
			SetPixel(hdc, sx, sy, gColor);
		ReleaseDC(hwnd, hdc);
		break;

	case WM_MOUSEMOVE:
		if (bnowDraw == TRUE) 
		{
			int now_x, now_y;
			hdc = GetDC(hwnd);
			hpen = CreatePen(PS_SOLID, 1, gColor);
			SelectObject(hdc, hpen);
			now_x = LOWORD(lParam);
			now_y = HIWORD(lParam);
			if (now_mode == MODE_PEN && !(now_x == oldx && now_y == oldy))
			{
				if (new_node == NULL)
					Drawing_b_LINE(hdc, oldx, oldy, now_x, now_y, gColor, head_saving_path->next, TRUE, FALSE);
				else
					Drawing_b_LINE(hdc, oldx, oldy, now_x, now_y, gColor, new_node, TRUE, FALSE);
			}
			else if (now_mode == MODE_RECT || now_mode == MODE_LINE)
			{
				SelectObject(hdc, GetStockObject(NULL_BRUSH));
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
					/*
						* Code for drawing circle
					*/
				}
			}
			oldx = now_x;
			oldy = now_y;
			DeleteObject(hpen);
			ReleaseDC(hwnd, hdc);
		}
		break;
	case WM_LBUTTONUP:
		bnowDraw = FALSE;
		hdc = GetDC(hwnd);
		hpen = CreatePen(PS_SOLID, 1, gColor);
		if(now_mode == MODE_RECT || now_mode == MODE_LINE)
		{
			SelectObject(hdc, hpen);
			SetDCPenColor(hdc, gColor);
			SelectObject(hdc, GetStockObject(NULL_BRUSH));
			int now_x, now_y;
			now_x = LOWORD(lParam);
			now_y = HIWORD(lParam);
			if (now_mode == MODE_RECT)
			{
				if (new_node == NULL)
				{
					Drawing_b_RECT(hdc, sx, sy, now_x, now_y, gColor, head_saving_path->next, TRUE, TRUE);
				}
				else
				{
					Drawing_b_RECT(hdc, sx, sy, now_x, now_y, gColor, new_node, TRUE, TRUE);
				}
			}
			else if (now_mode == MODE_LINE)
			{
				if (new_node == NULL)
					Drawing_b_LINE(hdc, sx, sy, now_x, now_y, gColor, head_saving_path->next, TRUE, TRUE);
				else
					Drawing_b_LINE(hdc, sx, sy, now_x, now_y, gColor, new_node, TRUE, TRUE);
			}
		}
		DeleteObject(hpen);
		ReleaseDC(hwnd, hdc);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, msg, wParam, lParam));
}


LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{

	case WM_CREATE:
		CreateWindowW(L"button", L"·Î±×ÀÎ",
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