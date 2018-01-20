#ifndef __PHYSICS_H
#define __PHYSICS_H

#include "game.h"
#include "input.h"

#define PACMAN_DEFAULT_SPEED 4.
#define GHOST_DEFAULT_SPEED 4.

// TO WRITE
Direction decidePacman(const Map* map, Pacman* pacman, Action action);

// TO WRITE
Direction decideGhost(const Map *map , Ghost *ghost , Pacman *pacman , Ghost *blinky);
#endif