#include "game.h"
#include "map.h"
#include <math.h>
#include "Values.h"

void MakeInBounds(int *x, int *y, const Map *map)
{
    if (*x < 0)
        *x += map->width;
    else
        *x = *x % map->width;
    if (*y < 0)
        *y += map->height;
    else
        *y = *y % map->height;
}

int GetDirSign(Direction dir)
{
    switch (dir)
    {
        case DIR_DOWN:
        case DIR_RIGHT:
            return 1;
        case DIR_UP:
        case DIR_LEFT:
            return -1;
        default:
            return 0;
    }
}

Point DirToPt(Direction dir)
{
    Point retVal;
    retVal.x = 0;
    retVal.y = 0;
    switch (dir)
    {
        case DIR_UP:
            retVal.y = -1;
            break;
        case DIR_RIGHT:
            retVal.x = 1;
            break;
        case DIR_DOWN:
            retVal.y = 1;
            break;
        case DIR_LEFT:
            retVal.x = -1;
            break;
    }
    return retVal;
}

bool IsVertical(Direction dir)
{ return dir == DIR_UP || dir == DIR_DOWN; }

bool IsHorizontal(Direction dir)
{ return dir == DIR_LEFT || dir == DIR_RIGHT; }

void GetPacmanCCell(Pacman *pacman, int *x, int *y)
{
    *x = (int) round(pacman->x);
    *y = (int) round(pacman->y);
}