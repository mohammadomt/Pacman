#include <stdbool.h>
#include <stdio.h>
#include "game.h"
#include <Basics.h>

int Sign(double number)
{
    return number >= 0 ? 1 : -1;
}

double Abs(double number)
{
    return number >= 0 ? number : -number;
}

void PrintPacman(Pacman *outPacman)
{
    fprintf(stderr, "%d %d (%d,%d) (%lf,%lf)\n", outPacman->dir, outPacman->health, outPacman->startX,
           outPacman->startY, outPacman->x, outPacman->y);
}

void PrintGhost(Ghost *outGhost)
{
    fprintf(stderr, "%d %d %d %lld %lf,%lf, %d,%d\n", outGhost->type, outGhost->dir, outGhost->blue, outGhost->blueCounterDown,
           outGhost->x, outGhost->y, outGhost->startX, outGhost->startY);
}

RectD GetPacmanRect(Pacman *pacman)
{
    RectD retVal;
    retVal.top =pacman->y;
    retVal.left = pacman->x;
    retVal.bottom = retVal.top +1;
    retVal.right = retVal.left+1;
    return retVal;
}