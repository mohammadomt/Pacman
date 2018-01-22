#ifndef __MAP_H
#define __MAP_H

#include <Basics.h>

typedef enum
{
    CELL_EMPTY = '_',
    CELL_BLOCK = '#',
    CELL_CHEESE = '*',
    CELL_CHERRY = '^',
    CELL_PINEAPPLE = 'O'
} Cell;

#define MAP_MAX_SIZE 100

typedef struct
{
    int width;
    int height;
    Cell cells[MAP_MAX_SIZE][MAP_MAX_SIZE];
} Map;

void MakeInBounds(const Map *, int *, int *);
double GetXInBounds(double width, double x);
double GetYInBounds(double height, double y);

Point GetNearestNB(const Map *map, int x, int y, Point helper);

#endif
