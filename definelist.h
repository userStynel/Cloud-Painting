#pragma once
#include <Windows.h>

#define MENU_FILE_OPEN 10
#define MENU_FILE_SAVE 11

#define MENU_MULTI_CREATE 12
#define MENU_MULTI_JOIN 13
#define MENU_MULTI_FRIEND 14

#define MODE_PEN 1
#define MODE_ERASER 2
#define MODE_LINE 3
#define MODE_RECT 4
#define MODE_CIRCLE 5
#define MODE_FILL 6
#define MODE_UNDO 7
#define MODE_REDO 8

#define PATH_PEN 'p'
#define PATH_RECT 'r'
#define PATH_CIRCLE 'c'
#define PATH_FILL 'f'
#define PATH_LINE 'l'

typedef struct polygon_path
{
	COLORREF prev_colour;
	COLORREF next_colour;
	int x, y;
	struct polygon_path *next;
}polygon_path;

typedef struct object_polygon
{
	polygon_path *path;
	struct object_polygon *prev;
	struct object_polygon *next;
}object_polygon;