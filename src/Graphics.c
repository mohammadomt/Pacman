//
// Created by Mohammad on 1/22/2018.
//
#include <Graphics.h>

void
roundedBoxX(SDL_Renderer *rndr, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 radius, int corners, Uint32 color)
{
    boxColor(rndr, x1 + radius, y1, x2 - radius, y2, color);
    boxColor(rndr, x1, y1 + radius, x2, y2 - radius, color);
    if (corners & 0b0001) //top left
        filledPieColor(rndr, x1 + radius, y1 + radius, radius, 180, 270, color);
    else
        boxColor(rndr, x1, y1, x1 + radius, y1 + radius, color);
    if (corners & 0b0010) //top right
        filledPieColor(rndr, x2 - radius, y1 + radius, radius, 270, 360, color);
    else
        boxColor(rndr, x2 - radius, y1, x2, y1 + radius, color);
    if (corners & 0b0100) //bottom right
        filledPieColor(rndr, x2 - radius, y2 - radius, radius, 0, 90, color);
    else
        boxColor(rndr, x2 - radius, y2 - radius, x2, y2, color);
    if (corners & 0b1000) //bottom left
        filledPieColor(rndr, x1 + radius, y2 - radius, radius, 90, 180, color);
    else
        boxColor(rndr, x1, y2 - radius, x1 + radius, y2, color);
}