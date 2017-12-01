#include "physics.h"
#include "map.h"
#include "game.h"
#include "values.h"
#include "Values.h"
#include <math.h>

Direction decideGhost(const Map *map, Ghost *ghost)
{
    //Check if it is time for decision

    //We need pacman info to decide
    Pacman pm;

    bool availableDirs[4] = {false};

    Direction best = ghost->dir;
    switch (ghost->type)
    {
        case BLINKY:
            for (int i = 0; i < 4; i++)
            {
                if (!availableDirs[i])
                    continue;

                if((IsVertical(i) && GetDirSign(i) == Sign(pm.y - ghost->y))||IsHorizontal(i) && GetDirSign(i) == Sign(pm.x - ghost->x))
                    best = (IsVertical(i)*Abs(pm.y - ghost->y) < IsHorizontal(best)* Abs(pm.x - ghost->x))||(IsVertical(best)*Abs(pm.y - ghost->y) > IsHorizontal(i)* Abs(pm.x - ghost->x)) ? i:best;

            }
            break;
    }
}

Direction decidePacman(const Map *map, Pacman *pacman, Action action)
{
    Direction dirAction;
    switch(action)
    {
        case ACTION_UP:
            dirAction = DIR_UP;
            break;
        case ACTION_DOWN:
            dirAction=  DIR_DOWN;
            break;
        case ACTION_LEFT:
            dirAction = DIR_LEFT;
            break;
        case ACTION_RIGHT:
            dirAction= DIR_RIGHT;
            break;
        default:
            dirAction = pacman->dir;
            break;
    }

    Point dirPt = DirToPt(dirAction);
    RectD rectNext;
    //TODO: Making in bounds
    rectNext.top = (int)pacman->y + dirPt.y;
    rectNext.left = (int)pacman->x + dirPt.x;
    rectNext.bottom = rectNext.top + 1;
    rectNext.right = rectNext.left+1;

    if(map->cells[(int)rectNext.left][(int)rectNext.top]!= CELL_BLOCK)
        return dirAction;

    if(!RectIntersectsWith(GetPacmanRect(pacman),rectNext))
        return dirAction;
    else if(dirAction == pacman->dir)
        return DIR_NONE;
    else
        return pacman->dir;
}
