#undef TAHVIL

#ifndef TAHVIL
#include <stdbool.h>
#include <stdio.h>
#include "Values.h"
#include "game.h"

int Sign(double number)
{
    return number >= 0 ? 1 : -1;
}

double Abs(double number)
{
    return number >= 0 ? number : -number;
}

bool RectContains(RectD rect, double x, double y)
{
    return (x >= rect.left && x <= rect.right) && (y >= rect.top && y <= rect.bottom);
}

//Check intersection. Shared edges are also OK.
bool RectIntersectsWith(RectD src, RectD dst)
{
    return src.top < dst.bottom &&
           src.left < dst.right &&
           src.bottom > dst.top &&
           src.right > dst.left;
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

bool PtIsEqual(Point *p1, Point *p2)
{
    return p1->x == p2->x && p1->y == p2->y;
}

#endif