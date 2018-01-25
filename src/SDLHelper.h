//
// Created by Mohammad on 1/22/2018.
//

#ifndef PACMAN_SDLHELPER_H
#define PACMAN_SDLHELPER_H

#include <SDL_pixels.h>

unsigned int ToAbgr(unsigned int argb);
SDL_Color ToSDLColor(unsigned int abgr);

#endif //PACMAN_SDLHELPER_H
