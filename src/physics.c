#undef TAHVIL

#include "physics.h"
#include "map.h"
#include "game.h"
#include "Values.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef TAHVIL
void MakeInBounds(int *, int *, const Map *);

typedef struct
{
    int x;
    int y;
} Point;

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

#endif

Direction decideGhost(const Map *map , Ghost *ghost , Pacman *pacman , Ghost *blinky)
{
    LinkedPoint *queue = FindPath(map, (int) ghost->x, (int) ghost->y, 1, 1);
    Point current;
    current.x = (int) ghost->x;
    current.y = (int) ghost->y;
    Point search;
    search.x = 1;
    search.y = 1;
    while (true)
    {
        if (PtIsEqual(&queue->current, &search))
            if (PtIsEqual(&queue->pre, &current))
            {
                Point ptDir = search;
                ptDir.x -= current.x;
                //ptDir.x = -ptDir.x;
                ptDir.y -= current.y;
                //ptDir.y = -ptDir.y;
//                free(queue);
                fprintf(stderr, "\n%d%d %d, %d %d, %d", ptDir.x,ptDir.y, search.x, search.y, current.x, current.y);
                return PtToDir(ptDir);
            } else
                search = queue->pre;
        queue--;
    }


#if DEBUG
    PrintGhost(ghost);
#endif
    fprintf(stderr, "%lf,%lf\n", ghost->x, ghost->y);
    bool availableDirs[4] = {false};
    for (int i = 0; i < 4; i++)
    {
        Point dirPt = DirToPt(i + 1);
        int nextX = (int) ghost->x + dirPt.x;
        int nextY = (int) ghost->y + dirPt.y;
        MakeInBounds(&nextX, &nextY, map);
        availableDirs[i] = map->cells[nextX][nextY] != CELL_BLOCK;
        fprintf(stderr, "%d", availableDirs[i]);
    }

    {
        int i = rand() % 4;
        while (!availableDirs[(++i) % 4]);
        fprintf(stderr, "\n%d\n", i);
        return (Direction) (i % 4 + 1);
    }

#ifndef TAHVIL
    //Check if it is time for decision

    //We need pacman info to decide
    Pacman pm;

    Direction best = ghost->dir;
    switch (ghost->type)
    {
        case BLINKY:
            for (int i = 0; i < 4; i++)
            {
                if (!availableDirs[i])
                    continue;

                if ((IsVertical(i) && GetDirSign(i) == Sign(pm.y - ghost->y)) ||
                    IsHorizontal(i) && GetDirSign(i) == Sign(pm.x - ghost->x))
                    best = (IsVertical(i) * Abs(pm.y - ghost->y) < IsHorizontal(best) * Abs(pm.x - ghost->x)) ||
                           (IsVertical(best) * Abs(pm.y - ghost->y) > IsHorizontal(i) * Abs(pm.x - ghost->x)) ? i
                                                                                                              : best;

            }
            break;
        case PINKY:

            break;
        case CLYDE:

            break;
        case INKY:

            break;
    }
//    Point  pt  =DirToPt(DIR_UP);
//    ghost->x += (double)pt.x/(double)CYCLES_PER_SEC;
//    ghost->y+=(double)pt.y/(double)CYCLES_PER_SEC;
    return DIR_NONE;
#endif
}

Direction decidePacman(const Map *map, Pacman *pacman, Action action)
{
    Direction dirAction;
    switch (action)
    {
        case ACTION_UP:
            dirAction = DIR_UP;
            break;
        case ACTION_DOWN:
            dirAction = DIR_DOWN;
            break;
        case ACTION_LEFT:
            dirAction = DIR_LEFT;
            break;
        case ACTION_RIGHT:
            dirAction = DIR_RIGHT;
            break;
        default:
            dirAction = pacman->dir;
            break;
    }

    Point dirPt;
    int nextX, nextY;
    Direction retVal;

    Again:
    dirPt = DirToPt(dirAction);
    nextX = (int) pacman->x + dirPt.x;
    nextY = (int) pacman->y + dirPt.y;
    MakeInBounds(&nextX, &nextY, map);

    retVal = dirAction;

    if (map->cells[nextX][nextY] == CELL_BLOCK)
        if (dirAction == pacman->dir)
            retVal = DIR_NONE;
        else
        {
            dirAction = pacman->dir;
            goto Again;
        }

    return retVal;
}
