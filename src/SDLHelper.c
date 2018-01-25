//
// Created by Mohammad on 1/22/2018.
//

#include <SDLHelper.h>
#include <SDL_pixels.h>
#include <stdio.h>

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

SDL_Color ToSDLColor(unsigned int abgr)
{
    unsigned int a, r, g, b;
    a = (abgr & 0xFF000000)>>24;
    r = abgr & 0x000000FF;
    g = (abgr & 0x0000FF00)>>8;
    b = (abgr & 0x00FF0000)>>16;

    return (SDL_Color){r,g,b,a};
}