//
// Created by Mohammad on 1/22/2018.
//
#include <stdio.h>
#include <SDLHelper.h>
#include <math.h>
#include <stdbool.h>
#include "SDL2_gfxPrimitives.h"
#include "map.h"
#include "game.h"
#include <game.h>
#include <physics.h>
#include <input.h>
#include <map.h>
#include <Basics.h>
#include <Graphics.h>

#ifdef main
#undef main
#endif

const int timeDelta = 1000 / CYCLES_PER_SEC;

void DrawMap(const Map *map, SDL_Renderer *rndr);

void DrawPacman(Pacman *pacman, Direction dir, Map *map, SDL_Renderer *rndr);

void DrawGhost(Ghost *ghost, Map *map, SDL_Renderer *rndr);

inline double DToNextX(double x, Direction dir);

double DToNextY(double y, Direction dir);

int main(int argc, char *argv[])
{
    Game game;
    Map map;
    Pacman player;
    Ghost ghosts[MAX_GHOST_COUNT];

    initiateGame("res\\map.txt", &map, &game, &player, ghosts);

    double pacmanStep = 1.0f / CYCLES_PER_SEC * player.speed;
    //TODO: change it for every ghosts
    double ghostStep = 1.0f / CYCLES_PER_SEC * GHOST_DEFAULT_SPEED;

    if (SDL_Init(SDL_INIT_VIDEO))
    {
        printf("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Pacman", 100, 100, map.width * CellSize, map.height * CellSize,
                                          SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        printf("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return 2;
    }

    SDL_Renderer *rndr = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (rndr == NULL)
    {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_Quit();
        return 3;
    }

    SDL_Surface *surface = SDL_LoadBMP("res\\ghost.bmp");
    SDL_Texture *t = SDL_CreateTextureFromSurface(rndr, surface);
    SDL_Rect dst = {50, 0, CellSize, CellSize};

    SDL_Event e;
    bool quit = false;
    Action arrow = 0;
    Direction lPacmanDir = DIR_NONE; //Shows correct dir for pacman in stops against blocks
    while (!quit && !isGameFinished(&game, &player))
    {
        if (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = true;
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                    case SDLK_q:
                        quit = true;
                        break;
                    case SDLK_RIGHT:
                        arrow = ACTION_RIGHT;
                        break;
                    case SDLK_LEFT:
                        arrow = ACTION_LEFT;
                        break;
                    case SDLK_UP:
                        arrow = ACTION_UP;
                        break;
                    case SDLK_DOWN:
                        arrow = ACTION_DOWN;
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(rndr, 0, 0, 0, 255);
        SDL_RenderClear(rndr);

        DrawMap(&map, rndr);

        //SDL_RenderCopy(rndr, t, NULL, &dst);


        if (DToNextX(player.x, player.dir) < pacmanStep && DToNextY(player.y, player.dir) < pacmanStep)
        {
            player.x = (int) player.x + (player.dir == DIR_RIGHT ? 1 : 0);
            player.y = (int) player.y + (player.dir == DIR_DOWN ? 1 : 0);

            if (player.x <= -1)
                player.x += map.width;
            if (player.y <= -1)
                player.y += map.height;
            if (player.x >= map.width)
                player.x -= map.width;
            if (player.y >= map.height)
                player.y -= map.height;

            player.dir = decidePacman(&map, &player, arrow);
            if (player.dir != DIR_NONE)
                lPacmanDir = player.dir;

            checkEatables(&map, &game, &player, ghosts);
        }

        Point ptDir = DirToPt(player.dir);
        player.x += ptDir.x * pacmanStep;
        player.y += ptDir.y * pacmanStep;

        DrawPacman(&player, lPacmanDir, &map, rndr);


        for (int i = 0; i < MAX_GHOST_COUNT; i++)
        {
            Ghost *g = ghosts + i;

            if (DToNextX(g->x, g->dir) < ghostStep && DToNextY(g->y, g->dir) < ghostStep)
            {
                g->x = (int) g->x + (g->dir == DIR_RIGHT ? 1 : 0);
                g->y = (int) g->y + (g->dir == DIR_DOWN ? 1 : 0);

                if (g->x <= -1)
                    g->x += map.width;
                if (g->y <= -1)
                    g->y += map.height;
                if (g->x >= map.width)
                    g->x -= map.width;
                if (g->y >= map.height)
                    g->y -= map.height;

                g->dir = decideGhost(&map, g, &player, ghosts + 0);
            }

            ptDir = DirToPt(g->dir);
            g->x += ptDir.x * ghostStep;
            g->y += ptDir.y * ghostStep;

            checkGhostState(g);
            checkGhostCollision(&player, g);

            DrawGhost(g, &map, rndr);
        }

        SDL_RenderPresent(rndr);
        SDL_Delay(timeDelta);
    }

    SDL_DestroyRenderer(rndr);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void DrawMap(const Map *map, SDL_Renderer *rndr)
{
    for (int i = 0; i < map->width; i++)
        for (int j = 0; j < map->height; j++)
        {
            int x = i * CellSize, y = j * CellSize;
            switch (map->cells[i][j])
            {
                case CELL_BLOCK:
                    boxColor(rndr, x, y, x + CellSize, y + CellSize, BlockColor);
                    break;
                case CELL_CHEESE:
                    filledCircleColor(rndr, x + CellSize / 2, y + CellSize / 2, CellSize / 10, CheeseColor);
                    break;
                case CELL_PINEAPPLE:
                    filledCircleColor(rndr, x + CellSize / 2, y + CellSize / 2, CellSize / 5, PineappleColor);
                    break;
                case CELL_CHERRY:

                    break;
            }
        }
}

void DrawPacmanChar(double x, double y, Sint16 start, Sint16 end, SDL_Renderer *rndr)
{
    filledPieColor(rndr, RCoordinate(x) + CellSize / 2, RCoordinate(y) + CellSize / 2, CellSize / 2,
                   start, end,
                   CheeseColor);
}

void DrawPacman(Pacman *pacman, Direction dir, Map *map, SDL_Renderer *rndr)
{
    Sint16 start, end;

    double frac = Abs(1 - (DToNextX(pacman->x, dir) + DToNextY(pacman->y, dir)) - 0.5) * 2;

    start = (dir - 2) * 90 + frac * 45; //This minus 2 works because of Direction enum order
    end = (dir - 2) * 90 - frac * 45;

    if (pacman->x < 0 || pacman->y < 0 || pacman->x > map->width - 1 || pacman->y > map->height - 1)
        DrawPacmanChar(GetXInBounds(map->width, pacman->x), GetYInBounds(map->height, pacman->y), start, end, rndr);
    DrawPacmanChar(pacman->x, pacman->y, start, end, rndr);
}

void DrawGhostChar(SDL_Renderer *rndr, Direction dir, Sint16 x, Sint16 y, Uint32 color)
{
    //Trunk
    filledPieColor(rndr, x + CellSize / 2, y + CellSize / 2, CellSize / 2, 180, 360, color);
    boxColor(rndr, x, y + CellSize / 2, x + CellSize, y + CellSize - GhostLegHeight, color);

    //Legs
    int dx = CellSize / ((GhostLegCount - 2) * 2 + 2);
    Sint16 xs[GhostLegCount * 2 + 1];
    Sint16 ys[GhostLegCount * 2 + 1];
    xs[0] = x;
    ys[0] = y + CellSize - GhostLegHeight;
    xs[GhostLegCount * 2] = x + CellSize;
    ys[GhostLegCount * 2] = ys[0];
    for (int i = 0; i <= ((GhostLegCount - 2) * 2 + 2); i++)
    {
        xs[i + 1] = x + i * dx;
        ys[i + 1] = i % 2 == 0 ? y + CellSize : ys[0];
    }

    filledPolygonColor(rndr, xs, ys, GhostLegCount * 2 + 1, color);

    //Eyes
    Point ptDir = DirToPt(dir);
    filledCircleColor(rndr, x + CellSize / 3, y + CellSize / 2, GhostEyeSize, 0xFFFFFFFF);
    filledCircleColor(rndr, x + CellSize * 2 / 3, y + CellSize / 2, GhostEyeSize, 0xFFFFFFFF);
    filledCircleColor(rndr, x + CellSize / 3 + ptDir.x*GhostPupilSize, y + CellSize / 2+ ptDir.y*GhostPupilSize, GhostPupilSize, 0xFF000000);
    filledCircleColor(rndr, x + CellSize * 2 / 3+ ptDir.x*GhostPupilSize, y + CellSize / 2+ ptDir.y*GhostPupilSize, GhostPupilSize, 0xFF000000);
}

void DrawGhost(Ghost *ghost, Map *map, SDL_Renderer *rndr)
{
    unsigned int color;
    if (ghost->blue)
        color = GhostBlueColor;
    else
        switch (ghost->type)
        {
            case BLINKY:
                color = BlinkyColor;
                break;
            case PINKY:
                color = PinkyColor;
                break;
            case CLYDE:
                color = ClydeColor;
                break;
            case INKY:
                color = InkyColor;
                break;
        }
    
    if (ghost->x < 0 || ghost->y < 0 || ghost->x > map->width - 1 || ghost->y > map->height - 1)
        DrawGhostChar(rndr, ghost->dir, RCoordinate(GetXInBounds(map->width, ghost->x)),
                      RCoordinate(GetYInBounds(map->height, ghost->y)),
                      color);
    DrawGhostChar(rndr, ghost->dir, RCoordinate(ghost->x), RCoordinate(ghost->y), color);
}

double DToNextX(double x, Direction dir) //Returns distance to next horizontal cell
{
    if (dir == DIR_LEFT || dir == DIR_RIGHT)
        return Abs(x - ((int) x + (dir == DIR_RIGHT ? 1 : 0)));
    else
        return 0;
}

double DToNextY(double y, Direction dir) //Returns distance to next vertical cell
{
    if (dir == DIR_UP || dir == DIR_DOWN)
        return Abs(y - ((int) y + (dir == DIR_DOWN ? 1 : 0)));
    else
        return 0;
}
