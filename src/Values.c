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

bool RectIntersectsWith(RectD src, RectD dst)
{
    return src.top < dst.bottom &&
           src.left < dst.right &&
           src.bottom > dst.top &&
           src.right > dst.left;
}

void PrintPacman(Pacman *outPacman)
{
    printf("%d %d (%d,%d) (%lf,%lf)\n", outPacman->dir, outPacman->health, outPacman->startX,
           outPacman->startY, outPacman->x, outPacman->y);
}

void PrintGhost(Ghost *outGhost)
{
    printf("%d %d %d %lld %lf,%lf, %d,%d\n", outGhost->type, outGhost->dir, outGhost->blue, outGhost->blueCounterDown,
           outGhost->x, outGhost->y, outGhost->startX, outGhost->startY);
}
