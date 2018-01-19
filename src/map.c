#undef TAHVIL
#ifndef TAHVIL

#include "game.h"
#include "map.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Values.h"
#include <Basics.h>


void MakeInBounds(const Map *map, int *x, int *y)
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


extern int FreeBlocksCount;
extern LinkedPoint *FoundPaths;
extern int FoundPathsCount;

LinkedPoint *FindPath(const Map *map, int fromX, int fromY, int toX, int toY)
{
    LinkedPoint *queue = (LinkedPoint *) malloc(MAP_MAX_SIZE * MAP_MAX_SIZE * sizeof(LinkedPoint));
    bool visited[MAP_MAX_SIZE][MAP_MAX_SIZE] = {false};

    int index = 0, pointer = 0;
    Point from;
    from.x = fromX;
    from.y = fromY;
    queue[pointer++].current = from;

    LinkedPoint *res = NULL;

    while (index <= pointer)
    {
        Point current = queue[index++].current;

        visited[current.x][current.y] = true;

        for (int dir = 1; dir < 5; dir++)
        {
            Point ptNext = DirToPt((Direction) dir);
            ptNext.x += current.x;
            ptNext.y += current.y;
            MakeInBounds(map, &ptNext.x, &ptNext.y);

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

    return NULL;
}

Direction GetMoveDirTo(const Map *map, Point from, Point to)
{
    if (PtIsEqual(&to, &from))
    {
        //Copied from Phase1
        fprintf(stderr, "from and to are equal\n");
        bool availableDirs[4] = {false};
        for (int i = 0; i < 4; i++)
        {
            Point dirPt = DirToPt(i + 1);
            int nextX = from.x + dirPt.x;
            int nextY = from.y + dirPt.y;
            MakeInBounds(map, &nextX, &nextY);
            availableDirs[i] = map->cells[nextX][nextY] != CELL_BLOCK;
        }
        int i = rand() % 4;
        while (!availableDirs[(++i) % 4]);
        return (Direction) (i % 4 + 1);
    }

    for (int i = 0; i < FoundPathsCount; i++)
        if (PtIsEqual(&FoundPaths[i].pre, &from) && PtIsEqual(&FoundPaths[i].current, &to))
        {
            fprintf(stderr, "Repeated: %d, %d to %d, %d\n", from.x, from.y, to.x, to.y);
            return FoundPaths[i].dir;
        }

    LinkedPoint *queue = FindPath(map, from.x, from.y, to.x, to.y);
    if (queue == NULL)
    {
        fprintf(stderr, "Unreachable point: %d, %d\n", to.x, to.y);
        return DIR_NONE;
    }
    Point search = to;
    Direction res;
    while (true)
    {
        if (PtIsEqual(&queue->current, &search))
            if (PtIsEqual(&queue->pre, &from))
            {
                Point ptDir = search;
                ptDir.x -= from.x;
                ptDir.y -= from.y;
                res = PtToDir(ptDir);
                break;
            } else
                search = queue->pre;
        queue--;
    }

    LinkedPoint newRec;
    newRec.pre = from;
    newRec.current = to;
    newRec.dir = res;
    FoundPaths[FoundPathsCount++] = newRec;

    return res;
}

Point GetNearestNB(const Map *map, int x, int y)
{
    int radius = 0;
    while (true)
    {
        radius++;
        for (int i = -radius; i <= radius; i++)
            for (int j = -radius; j <= radius; j++)
                if (x + i >= 0 && x + i < map->width && y + j >= 0 && y + j < map->height)
                    if (map->cells[x + i][y + j] != CELL_BLOCK)
                    {
                        Point retVal;
                        retVal.x = x + i;
                        retVal.y = y + j;
                        return retVal;
                    }
    }
}

#endif