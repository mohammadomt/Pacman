#include "physics.h"
#include "map.h"
#include "game.h"
#include "values.h"
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
    // fill me
}
