#include "DrawingTools.h"

extern COLORREF gColor;

void Drawing_PEN(HDC hdc, int ex_x, int ex_y, int now_x, int now_y)
{
	MoveToEx(hdc, ex_x, ex_y, NULL);
	LineTo(hdc, now_x, now_y);
}

void ADD_PATH(object_polygon* p_poly, int x, int y, COLORREF c, COLORREF d)
{
	polygon_path *searcher = p_poly->path;
	if (p_poly->path == NULL)
	{
		p_poly->path = (polygon_path *)malloc(sizeof(polygon_path));
		p_poly->path->x = x;
		p_poly->path->y = y;
		p_poly->path->prev_colour = c;
		p_poly->path->next_colour = d;
		p_poly->path->next = NULL;
	}
	else
	{
		while (searcher->next != NULL)
		{
			if (searcher->x == x && searcher->y == y)
				return;
			searcher = searcher->next;
		}
		if (searcher->x == x && searcher->y == y)
			return;
		searcher->next = (polygon_path*)malloc(sizeof(polygon_path));
		searcher->next->x = x;
		searcher->next->y = y;
		searcher->next->prev_colour = c;
		searcher->next->next_colour = d;
		searcher->next->next = NULL;
	}
}

void INIT_POLYGON(object_polygon** p_poly)
{
	(*p_poly)->next = (object_polygon *)malloc(sizeof(object_polygon));
	(*p_poly)->next->prev = *p_poly;
	(*p_poly)->next->next = NULL;
	(*p_poly)->next->path = NULL;
}

object_polygon* ADD_POLYGON(object_polygon** p_poly)
{
	object_polygon *searcher = *p_poly;
	while (searcher->next != NULL)
		searcher = searcher->next;
	searcher->next = (object_polygon*)malloc(sizeof(object_polygon));
	searcher->next->prev = searcher;
	searcher->next->next = NULL;
	searcher->next->path = NULL;
	return (searcher->next);
}

void UNDO_PEN(object_polygon* p_poly, HWND hwnd)
{
	HDC hdc;
	HPEN hpen;
	int x, y;
	polygon_path *searcher = p_poly->path;
	while (searcher != NULL)
	{
		hdc = GetDC(hwnd);
		x = searcher->x;
		y = searcher->y;
		if(x >= 0 && y >= 0)
			SetPixel(hdc, x + 50, y + 10, searcher->prev_colour);
		else
		{
			ExtFloodFill(hdc, searcher->next->x+50, searcher->next->y+10, searcher->prev_colour, FLOODFILLSURFACE);
			break;
		}
		searcher = searcher->next;
		ReleaseDC(hwnd, hdc);
	}
}

void REDO_PEN(object_polygon* p_poly, HWND hwnd)
{
	HDC hdc;
	HPEN hpen;
	int x, y;
	polygon_path *searcher = p_poly->path;
	while (searcher != NULL)
	{
		hdc = GetDC(hwnd);
		x = searcher->x;
		y = searcher->y;
		if(x>=0 && y>=0)
			SetPixel(hdc, x + 50, y + 10, searcher->next_colour);
		else
		{
			ExtFloodFill(hdc, searcher->next->x + 50, searcher->next->y + 10, searcher->next_colour, FLOODFILLSURFACE);
			break;
		}
		searcher = searcher->next;
		ReleaseDC(hwnd, hdc);
	}
}

void Drawing_b_LINE(HDC hdc, int sx, int sy, int x, int y, COLORREF colour, object_polygon* p_poly, BOOL save, 
	BOOL reverse)
{
	int inc;
	int px, py;
	int dx, dy;
	int d;
	if (abs(x - sx) >= abs(y - sy))
	{
		if (sx > x)
		{
			int temp;
			temp = sx;
			sx = x;
			x = temp;

			temp = sy;
			sy = y;
			y = temp;
			py = y;
		}
		py = sy;

		inc = (y > sy) ? 1 : -1;
		dx = x - sx;
		dy = abs(y - sy);
		d = 2 * dy - dx;
		for (int i = sx; i <= x; i++)
		{
			if (save)
			{
				if (!reverse)
					ADD_PATH(p_poly, i, py, GetPixel(hdc, i, py), colour);
				else
					ADD_PATH(p_poly, i, py, RGB(255, 255, 255) - GetPixel(hdc, i, py), colour);
			}
			SetPixel(hdc, i, py, colour);
			if (d > 0)
			{
				py += inc;
				d += 2 * (dy - dx);
			}
			else
				d += 2 * dy;
		}
	}
	else
	{
		if (sy > y)
		{
			int temp;
			temp = sy;
			sy = y;
			y = temp;
			
			temp = sx;
			sx = x;
			x = temp;
			px = x;
		}
		px = sx;

		inc = (x > sx) ? 1 : -1;
		dx = abs(x - sx);
		dy = y - sy;
		d = 2 * dx - dy;
		for (int i = sy; i <= y; i++)
		{
			if (save)
			{
				if (!reverse)
					ADD_PATH(p_poly, px, i, GetPixel(hdc, px, i), colour);
				else
					ADD_PATH(p_poly, px, i, RGB(255, 255, 255) - GetPixel(hdc, px, i), colour);
			}
			SetPixel(hdc, px, i, colour);
			if (d > 0)
			{
				px += inc;
				d += 2 * (dx - dy);
			}
			else
				d += 2 * dx;
		}
	}
}