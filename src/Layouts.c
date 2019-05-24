#include "Layouts.h"
#include "definelist.h"

// extern: source.c 에 위치해있음
extern COLORREF gColor;
extern LRESULT CALLBACK PAPERPanelProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK COLORPanelProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK LoginProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK CSZPROC(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK SAVEPROC(HWND, UINT, WPARAM, LPARAM);

COLORREF ShowColorDialog(HWND hwnd)
{
	CHOOSECOLOR cc;
	static COLORREF crCustClr[16];

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hwnd;
	cc.lpCustColors = (LPDWORD)crCustClr;
	cc.rgbResult = gColor;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	ChooseColor(&cc);

	return cc.rgbResult;
}

void AddMenus(HWND hwnd)
{
	HMENU hMenubar;
	HMENU hFILEMenu;
	HMENU hMULTIMenu;
	HMENU hADDONMenu;

	hMenubar = CreateMenu();
	hFILEMenu = CreateMenu();
	hMULTIMenu = CreateMenu();
	hADDONMenu = CreateMenu();

	AppendMenuW(hFILEMenu, MF_STRING, MENU_FILE_OPEN, L"&열기");
	AppendMenuW(hFILEMenu, MF_STRING, MENU_FILE_SAVE, L"&저장");

	AppendMenuW(hMULTIMenu, MF_STRING, MENU_MULTI_CREATE, L"&방 생성");
	AppendMenuW(hMULTIMenu, MF_STRING, MENU_MULTI_JOIN, L"&들어가기");
	AppendMenuW(hMULTIMenu, MF_STRING, MENU_MULTI_FRIEND, L"&친구");
	AppendMenuW(hMULTIMenu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hMULTIMenu, MF_STRING, 1, L"&업로드");
	AppendMenuW(hMULTIMenu, MF_STRING, 1, L"&다운로드");

	AppendMenuW(hADDONMenu, MF_STRING, 3, L"&네비게이션");
	AppendMenuW(hADDONMenu, MF_STRING, 1, L"&레이어");
	AppendMenuW(hADDONMenu, MF_STRING, CHANGING_SIZE, L"&크기변경");

	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hFILEMenu, L"&파일");
	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hADDONMenu, L"&애드온");
	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMULTIMenu, L"&멀티플레이");

	SetMenu(hwnd, hMenubar);
}

void AddButtons(HWND hwnd, HWND* p_panel, HWND* p_paper)
{
	WNDCLASSW rwc = { 0 };
	rwc.lpszClassName = L"Panel";
	rwc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	rwc.lpfnWndProc = COLORPanelProc;
	RegisterClassW(&rwc);

	WNDCLASSW paper = { 0 };
	paper.lpszClassName = L"Paper";
	paper.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	paper.lpfnWndProc = PAPERPanelProc;
	RegisterClassW(&paper);

	int y = 10;
	CreateWindowW(L"Button", L"P",
		WS_VISIBLE | WS_CHILD,
		10, y, 25, 25, hwnd, (HMENU)MODE_PEN, NULL, NULL);
	y += 30;

	CreateWindowW(L"Button", L"S",
		WS_VISIBLE | WS_CHILD,
		10, y, 25, 25, hwnd, (HMENU)MODE_SPOID, NULL, NULL);
	y += 30;

	CreateWindowW(L"Button", L"L",
		WS_VISIBLE | WS_CHILD,
		10, y, 25, 25, hwnd, (HMENU)MODE_LINE, NULL, NULL);
	y += 30;

	CreateWindowW(L"Button", L"R",
		WS_VISIBLE | WS_CHILD,
		10, y, 25, 25, hwnd, (HMENU)MODE_RECT, NULL, NULL);
	y += 30;

	CreateWindowW(L"Button", L"Ci",
		WS_VISIBLE | WS_CHILD,
		10, y, 25, 25, hwnd, (HMENU)MODE_CIRCLE, NULL, NULL);
	y += 30;

	CreateWindowW(L"Button", L"F",
		WS_VISIBLE | WS_CHILD,
		10, y, 25, 25, hwnd, (HMENU)MODE_FILL, NULL, NULL);
	y += 30;

	*p_panel = CreateWindowW(L"Panel", NULL,
		WS_CHILD | WS_VISIBLE,
		10, y, 25, 25, hwnd, NULL, NULL, NULL);

	*p_paper = CreateWindowW(L"Paper", NULL,
		WS_CHILD | WS_VISIBLE,
		50, 10, 500, 350, hwnd, NULL, NULL, NULL);

	CreateWindowW(L"Button", L"<",
		WS_VISIBLE | WS_CHILD,
		50, 370, 25, 25, hwnd, (HMENU)MODE_UNDO, NULL, NULL);
	CreateWindowW(L"Button", L">",
		WS_VISIBLE | WS_CHILD,
		80, 370, 25, 25, hwnd, (HMENU)MODE_REDO, NULL, NULL);
}

void LoginBox(HWND hwnd, BOOL* check_login, HINSTANCE hinstance)
{
	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.lpfnWndProc = (WNDPROC)LoginProc;
	wc.hInstance = hinstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpszClassName = L"DialogClass";
	RegisterClassExW(&wc);

	CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, L"DialogClass", L"로그인",
		WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 100, 100, 200, 150,
		NULL, NULL, hinstance, NULL);
}

void CGSZBOX(HWND hwnd, HINSTANCE hinstance)
{
	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.lpfnWndProc = (WNDPROC)CSZPROC;
	wc.hInstance = hinstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpszClassName = L"CGBOXCLASS";
	RegisterClassExW(&wc);

	CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, L"CGBOXCLASS", L"사이즈 변경",
		WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 100, 100, 200, 200,
		NULL, NULL, hinstance, NULL);
}

void SAVEBOX(HWND hwnd, HINSTANCE hinstance)
{
	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.lpfnWndProc = (WNDPROC)SAVEPROC;
	wc.hInstance = hinstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpszClassName = L"SAVEBOXCLASS";
	RegisterClassExW(&wc);

	CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, L"SAVEBOXCLASS", L"저장",
		WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 100, 100, 200, 200,
		NULL, NULL, hinstance, NULL);
}