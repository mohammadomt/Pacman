//
// Created by Mohammad on 1/4/2018.
//

#ifndef PACMAN_BASICS_H
#define PACMAN_BASICS_H

#include <stdbool.h>

typedef struct
{
    double top;
    double left;
    double right;
    double bottom;
} RectD;

typedef struct
{
    int x;
    int y;
} Point;

int Sign(double);

double Abs(double);

bool RectContains(RectD, double, double);

bool RectIntersectsWith(RectD, RectD);

bool PtIsEqual(Point *p1, Point *p2);

#endif //PACMAN_BASICS_H
