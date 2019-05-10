/*
	* Layouts.h: Set Layout
*/

#pragma once
#include <Windows.h>
void AddMenus(HWND);
void AddButtons(HWND, HWND*, HWND*);
void LoginBox(HWND, BOOL*, HINSTANCE);
COLORREF ShowColorDialog(HWND);