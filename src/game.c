#undef TAHVIL

#include <stdio.h>

#include "game.h"
#include "map.h"
#include "physics.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#ifndef TAHVIL

#include "Values.h"
#include "map.h"

#endif

#ifdef TAHVIL
typedef struct
{
    double top;
    double left;
    double right;
    double bottom;
} RectD;

void MakeInBounds(int *x, int *y, const Map *map)
{
    if (*x < 0)
        *x += map->width;
    else
        *x = *x % map->width;
    if (*y < 0)
        *y += map->height;
    else
        *y = *y % map->height;
}

int GetDirSign(Direction dir)
{
    switch (dir)
    {
        case DIR_DOWN:
        case DIR_RIGHT:
            return 1;
        case DIR_UP:
        case DIR_LEFT:
            return -1;
        default:
            return 0;
    }
}

bool IsVertical(Direction dir)
{ return dir == DIR_UP || dir == DIR_DOWN; }

bool IsHorizontal(Direction dir)
{ return dir == DIR_LEFT || dir == DIR_RIGHT; }

void GetPacmanCCell(Pacman *pacman, int *x, int *y)
{
    *x = (int) round(pacman->x);
    *y = (int) round(pacman->y);
}


///////////Values
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
#endif

void initiateGame(char *filename, Map *outMap, Game *outGame, Pacman *outPacman, Ghost *outGhosts)
{
    outGame->cheeses = 0;
    outGame->cherries = 0;
    outGame->pineapples = 0;
    outGame->ghosts = MAX_GHOST_COUNT;
    outGame->score = 0;

    FILE *fs = fopen(filename, "r");
    int n, m;
    fscanf(fs, "%d%d", &outMap->height, &outMap->width);
    for (int i = 0; i < outMap->height; i++)
        for (int j = 0; j < outMap->width;)
        {
            char c;
            fscanf(fs, "%c", &c);
            if (c != '\n' && c != '\r')
            {
                outMap->cells[j][i] = (Cell) c;
                switch (outMap->cells[j][i])
                {
                    case CELL_CHEESE:
                        outGame->cheeses++;
                        break;
                    case CELL_CHERRY:
                        outGame->cherries++;
                        break;
                    case CELL_PINEAPPLE:
                        outGame->pineapples++;
                        break;
                }
                j++;
            }
        }

    fscanf(fs, "%d", &outGame->score);
    fscanf(fs, "%*s %d %d (%d,%d) (%lf,%lf)", &outPacman->dir, &outPacman->health, &outPacman->startX,
           &outPacman->startY, &outPacman->x, &outPacman->y);
    outPacman->speed = PACMAN_DEFAULT_SPEED;
    for (int i = 0; i < MAX_GHOST_COUNT; i++)
    {
        outGhosts[i].type = (GhostType) i;
        outGhosts[i].speed = GHOST_DEFAULT_SPEED;
        int mode;
        fscanf(fs, "%*s%d %d", &outGhosts[i].dir, &mode);
        if (mode == 0)
            fscanf(fs, " %lld", &outGhosts[i].blueCounterDown);
        outGhosts[i].blue = !mode;
        fscanf(fs, " (%d,%d) (%lf,%lf)", &outGhosts[i].startX, &outGhosts[i].startY, &outGhosts[i].x, &outGhosts[i].y);
    }

    fclose(fs);

    srand(time(NULL));

}

void checkEatables(Map *map, Game *outGame, Pacman *outPacman, Ghost *outGhosts)
{
    int x, y;
    GetPacmanCCell(outPacman, &x, &y);

    switch (map->cells[x][y])
    {
        case CELL_CHEESE:
            outGame->score += CHEESE_SCORE;
            outGame->cheeses--;
            break;
        case CELL_CHERRY:
            outGame->score += CHERRY_SCORE;
            outGame->cherries--;
            break;
        case CELL_PINEAPPLE:
            outGame->score += PINEAPPLE_SCORE;
            outGame->pineapples--;
            for (int i = 0; i < MAX_GHOST_COUNT; i++)
            {
                (outGhosts + i)->blue = true;
                (outGhosts + i)->blueCounterDown = BLUE_DURATION;
            }
            break;
    }

    if (map->cells[x][y] != CELL_BLOCK)
        map->cells[x][y] = CELL_EMPTY;
}

void checkGhostCollision(Pacman *outPacman, Ghost *outGhost)
{
    RectD pm, gh;
    pm.left = outPacman->x;
    pm.top = outPacman->y;
    pm.right = pm.left + 1;
    pm.bottom = pm.top + 1;
    gh.left = outGhost->x;
    gh.top = outGhost->y;
    gh.right = gh.left + 1;
    gh.bottom = gh.top + 1;
    if (!RectIntersectsWith(pm, gh))
        return;

    if (outGhost->blue)
    {
        outGhost->blue = false;
        outGhost->blueCounterDown = 0;
        outGhost->x = outGhost->startX;
        outGhost->y = outGhost->startY;
    } else
    {
        outPacman->x = outPacman->startX;
        outPacman->y = outPacman->startY;
        outPacman->health--;
    }
}

bool isGameFinished(Game *game, Pacman *pacman)
{
    if (pacman->health == 0) fprintf(stderr, "Pacman dead!\n");
    if (game->pineapples == 0 && game->cheeses == 0) fprintf(stderr, "Everything is eaten!\n");
    return (game->pineapples == 0 && game->cheeses == 0) || pacman->health == 0;
}

void checkGhostState(Ghost *ghost)
{
    if (!ghost->blue)
        return;
    if (ghost->blueCounterDown <= 0)
        ghost->blue = false;
    else
        ghost->blueCounterDown -= (double) 1 / CYCLES_PER_SEC;
}
