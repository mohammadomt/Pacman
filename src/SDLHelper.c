//
// Created by Mohammad on 1/22/2018.
//

#include <SDLHelper.h>

unsigned int ToAbgr(unsigned int argb)
{
    unsigned int a, r, g, b;
    a = argb & 0xFF000000;
    r = argb & 0x00FF0000;
    g = argb & 0x0000FF00;
    b = argb & 0x000000FF;
    r >>= 16;
    b <<= 16;
    return a + b + g + r;
}