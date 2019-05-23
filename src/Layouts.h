/*
	* Layouts.h: Set Layout
*/

#pragma once
#include <Windows.h>
void AddMenus(HWND);
void AddButtons(HWND, HWND*, HWND*);
void LoginBox(HWND, BOOL*, HINSTANCE);
void CGSZBOX(HWND, HINSTANCE);
void SAVEBOX(HWND hwnd, HINSTANCE hinstance);
COLORREF ShowColorDialog(HWND);