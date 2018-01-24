#include "physics.h"
#include "game.h"
#include <Basics.h>
#include <math.h>
#include <stdio.h>

Direction decideGhost(const Map *map, Ghost *ghost, Pacman *pacman, Ghost *blinky)
{
    Point from = {(int) ghost->x, (int) ghost->y}, to;

    if (ghost->blue || ghost->state == SCATTER)
    {
        //fprintf(stderr, "%d: from: %d, %d to: %d, %d(blue)\n", ghost->type, from.x, from.y, ghost->startX, ghost->startY);
        return GetMoveDirTo(map, from, (Point) {ghost->startX, ghost->startY});
    }
    switch (ghost->type)
    {
        case BLINKY:
            to = (Point) {(int) pacman->x, (int) pacman->y};
            break;
        case PINKY:
        {
            //TODO: Get a log to check it
            int i;
            Point ptDir = DirToPt(pacman->dir);
            int x, y;
            for (i = 1; i <= 4; i++)
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
            pivot.x = (int) pacman->x + pivot.x * 2;
            pivot.y = (int) pacman->y + pivot.y * 2;
            int x = 2 * pivot.x - (int) blinky->x, y = 2 * pivot.y - (int) blinky->y;
            MakeInBounds(map, &x, &y);
            to = GetNearestNB(map, x, y, from);
        }
            break;
        case CLYDE:
            if (Abs(ghost->x - pacman->x) + Abs(ghost->y - pacman->y) > 8)
                to = (Point) {(int) pacman->x, (int) pacman->y};
            else
                to = GetNearestNB(map, 0, map->height - 1, from);

            break;
    }
    //fprintf(stderr, "%d: from: %d, %d to: %d, %d\n", ghost->type, from.x, from.y, to.x, to.y);
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
