#include <stdio.h>

#include "game.h"
#include "map.h"
#include "physics.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>


int FreeBlocksCount = 0;
LinkedPoint *FoundPaths;
int FoundPathsCount = 0;

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
    int blocksCount = 0;
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
                    case CELL_BLOCK:
                        blocksCount++;
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

    FreeBlocksCount = outMap->height * outMap->width - blocksCount;
    FoundPaths = (LinkedPoint *) malloc(FreeBlocksCount * FreeBlocksCount * sizeof(LinkedPoint));
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
        outGhost->dir = DIR_NONE;
    } else
    {
        outPacman->x = outPacman->startX;
        outPacman->y = outPacman->startY;
        outPacman->health--;
        outPacman->dir = DIR_NONE;
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
        ghost->blueCounterDown -= 1;
}
