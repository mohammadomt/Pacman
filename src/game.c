#include <stdio.h>

#include "game.h"
#include "map.h"
#include "Values.h"


void initiateGame(char *filename, Map *outMap, Game *outGame, Pacman *outPacman, Ghost *outGhosts)
{
    outGame->cheeses = 0;
    outGame->cherries = 0;
    outGame->pineapples = 0;
    outGame->score = 0;

    FILE *fs = fopen(filename, "r");
    int n, m;
    fscanf(fs, "%d%d", &outMap->height, &outMap->width);
    for (int i = 0; i < outMap->height; i++)
        for (int j = 0; j < outMap->width;)
        {
            char c;
            fscanf(fs, "%c", &c);
            if (c != '\n' && c!='\r')
            {
                outMap->cells[j][i] = c;
                switch (outMap->cells[i][j])
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

    for (int i = 0; i < MAX_GHOST_COUNT; i++)
    {
        outGhosts[i].type = i;
        int mode;
        fscanf(fs, "%*s%d %d", &outGhosts[i].dir, &mode);
        if (mode == 0)
            fscanf(fs, " %lld", &outGhosts[i].blueCounterDown);
        outGhosts[i].blue = !mode;
        fscanf(fs, " (%d,%d) (%lf,%lf)", &outGhosts[i].startX, &outGhosts[i].startY, &outGhosts[i].x, &outGhosts[i].y);
    }

    fclose(fs);

    for (int i = 0; i < outMap->height; i++)
    {
        for (int j = 0; j < outMap->width; j++)
            printf("%c", outMap->cells[i][j]);
        printf("\n");
    }
//    PrintPacman(outPacman);
//    for (int i = 0; i < MAX_GHOST_COUNT; i++)
//    {
//        PrintGhost(outGhosts+i);
//    }
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
                outGhosts->blue = true;
                outGhosts->blueCounterDown = BLUE_DURATION;
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
