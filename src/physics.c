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

Direction decideGhost(const Map *map, Ghost *ghost, Pacman *pacman, Ghost *blinky)
{
    Point from, to;
    from.x = (int) ghost->x;
    from.y = (int) ghost->y;

    switch (ghost->type)
    {
        case BLINKY:
            to.x = (int) pacman->x;
            to.y = (int) pacman->y;
            break;
        case PINKY:
        {
            //TODO: Get a log to check it
            int i;
            Point ptDir = DirToPt(pacman->dir);
            int x, y;
            for (i = 0; i <= 4; i++)
            {
                x = (int) pacman->x + ptDir.x * i;
                y = (int) pacman->y + ptDir.y * i;
                MakeInBounds(map, &x, &y);
                if (map->cells[x][y] == CELL_BLOCK)
                    break;
            }
            i--;
            x = (int) pacman->x + ptDir.x * i;
            y = (int) pacman->y + ptDir.y * i;
            MakeInBounds(map, &x, &y);
            to.x = x;
            to.y = y;
        }
            break;
        case INKY:
        {
            Point pivot = DirToPt(pacman->dir);
            pivot.x = (int)pacman->x + pivot.x *2 ;
            pivot.y = (int)pacman->y + pivot.y *2 ;
            int x  = 2 * pivot.x - (int)blinky->x, y = 2 * pivot.y  - (int)blinky->y;
            MakeInBounds(map, &x, &y);
            Point toGo = GetNearestNB(map, x, y);
            to.x = toGo.x;
            to.y = toGo.y;
        }
            break;
        case CLYDE:

            if (Abs(ghost->x - pacman->x) + Abs(ghost->y - pacman->y) > 8)
            {
                to.x = (int) pacman->x;
                to.y = (int) pacman->y;
                break;
            }

            Point lb  = GetNearestNB(map, 0, map->height - 1);
            to.x = lb.x;
            to.y = lb.y;
            break;
    }
    fprintf(stderr,"%d: from: %d, %d to: %d, %d\n", ghost->type, from.x, from.y, to.x, to.y);
    return GetMoveDirTo(map, from, to);

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
    MakeInBounds(map, &nextX, &nextY);

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
