//
// Created by Mohammad on 11/30/2017.
//

#ifndef PACMAN_VALUES_H
#define PACMAN_VALUES_H

typedef struct
{
    double top;
    double left;
    double right;
    double bottom;
} RectD;

int Sign(double);

double Abs(double);

bool RectContains(RectD, double, double);
bool RectIntersectsWith(RectD,RectD);


#endif //PACMAN_VALUES_H
