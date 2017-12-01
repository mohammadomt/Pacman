#include "game.h"
#include "map.h"
#include <math.h>

void MakeInBounds(int *x, int * y, Map *map)
{
    *x = *x%map->width;
    *y= *y%map->height;
}

int GetDirSign(Direction dir)
{
    switch(dir)
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

bool IsVertical(Direction dir)
{ return dir== DIR_UP || dir == DIR_DOWN;}
bool IsHorizontal(Direction dir)
{ return dir== DIR_LEFT || dir == DIR_RIGHT;}

void GetPacmanCCell(Pacman *pacman, int *x, int *y)
{
    *x= (int)round(pacman->x);
    *y= (int)round(pacman->y);
}