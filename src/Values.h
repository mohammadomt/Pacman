//
// Created by Mohammad on 11/30/2017.
//

#ifndef TAHVIL

#include <stdbool.h>

#ifndef PACMAN_VALUES_H
#define PACMAN_VALUES_H

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

#endif //PACMAN_VALUES_H
#endif