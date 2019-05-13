#pragma once
#include "definelist.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

/*
	* DrawingTools.h
	* Drawing_PEN(): Drawing something using pen
	* Drawing_
*/

void Drawing_PEN(HDC hdc, int ex_x, int ex_y, int now_x, int now_y);
void Drawing_b_LINE(HDC hdc, int sx, int sy, int x, int y, COLORREF colour, object_polygon* p_poly, BOOL save, 
	BOOL reverse);
void Drawing_b_CIRCLE(HDC hdc, int cx, int cy, int radius, COLORREF colour, object_polygon* p_poly, BOOL save,
	BOOL reverse);
void Drawing_b_RECT(HDC hdc, int sx, int sy, int fx, int fy, COLORREF colour, object_polygon* p_poly, BOOL save,
	BOOL reverse);
void Drawing_RECT();
void ERASER();
void UNDO();
void UNDO_PEN(object_polygon* p_poly, HWND hwnd);
void UNDO_LINE();
void UNDO_RECT();
void UNDO_CIRCLE();

void REDO_PEN(object_polygon* p_poly, HWND hwnd);

void INIT_POLYGON(object_polygon** p_poly); // 첫 장면을 할당함
object_polygon* ADD_POLYGON(object_polygon** p_poly); // 다음 장면을 할당함
void ADD_PATH(object_polygon* p_poly, int x, int y, COLORREF c, COLORREF d); // 새 경로를 넣음