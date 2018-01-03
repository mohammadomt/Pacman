#undef TAHVIL

#include "game.h"
#include "map.h"
#include <math.h>
#include <stdlib.h>
#include "Values.h"

#ifndef TAHVIL

extern int FreeBlocks = 0;

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

Direction PtToDir(Point pt)
{
    if (pt.y == -1)
        return DIR_UP;
    else if (pt.x == 1)
        return DIR_RIGHT;
    else if (pt.y == 1)
        return DIR_DOWN;
    else if (pt.x == -1)
        return DIR_LEFT;
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


LinkedPoint *FindPath(const Map *map, int fromX, int fromY, int toX, int toY)
{
    LinkedPoint *queue = (LinkedPoint *) malloc(MAP_MAX_SIZE * MAP_MAX_SIZE * sizeof(LinkedPoint));
    bool visited[MAP_MAX_SIZE][MAP_MAX_SIZE] = {false};

    int index = 0, pointer = 0;
    Point from;
    from.x = fromX;
    from.y = fromY;
    queue[pointer++].current = from;

    while (true)
    {
        Point current = queue[index++].current;

        visited[current.x][current.y] = true;

        for (int dir = 1; dir < 5; dir++)
        {
            Point ptNext = DirToPt((Direction) dir);
            ptNext.x += current.x;
            ptNext.y += current.y;
            MakeInBounds(&ptNext.x, &ptNext.y, map);

            if (visited[ptNext.x][ptNext.y])
                continue;

            if (map->cells[ptNext.x][ptNext.y] != CELL_BLOCK)
            {
                LinkedPoint child;
                child.current = ptNext;
                child.pre = current;
                queue[pointer++] = child;
                if (ptNext.x == toX && ptNext.y == toY)
                    return queue + pointer - 1;
            }
        }
    }
}

#endif