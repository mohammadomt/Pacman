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
#include <physics.h>
#include <input.h>
#include <map.h>
#include <Basics.h>
#include <Graphics.h>
#include <SDL_ttf.h>
#include <string.h>

#ifdef main
#undef main
#endif

typedef struct
{
    char items[5][20];
    bool shownItems[5];
    int hoverItem;
    char title[40];
    char subtitle[40];
    char highScore[40];
    bool showHighScore;
    int itemCount;
} Menu;

const int timeDelta = 1000 / CYCLES_PER_SEC;

int Init(SDL_Window **window, SDL_Renderer **rndr, TTF_Font **font, TTF_Font **fontMenu, TTF_Font **fontMenuBig);

void DrawBlocks(SDL_Renderer *rndr, const Map *map);

void DrawMap(const Map *map, SDL_Renderer *rndr);

void DrawPacman(Pacman *pacman, Direction dir, Map *map, SDL_Renderer *rndr);

void DrawGhost(Ghost *ghost, Map *map, SDL_Renderer *rndr);

SDL_Texture *DrawText(SDL_Renderer *rndr, TTF_Font *font, char *text);

SDL_Texture *DrawScore(SDL_Renderer *rndr, TTF_Font *font, int score);

SDL_Texture *DrawTimer(SDL_Renderer *rndr, TTF_Font *font, unsigned int seconds);

void DrawMenu(SDL_Renderer *rndr, TTF_Font *font, TTF_Font *fontBig, Menu *mnu, int w, int h);

inline double DToNextX(double x, Direction dir);

double DToNextY(double y, Direction dir);


int MapWidth, MapHeight, WindowWidth, WindowHeight;

SDL_Texture *tCherry = NULL;

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

    MapWidth = map.width * CellSize;
    MapHeight = map.height * CellSize;
    WindowWidth = MapWidth + 2 * MapPadding;
    WindowHeight = MapHeight + 2 * MapPadding + TitlebarHeight;


    SDL_Window *window;
    SDL_Renderer *rndr;
    TTF_Font *font;
    TTF_Font *fontMenu;
    TTF_Font *fontMenuBig;
    if (Init(&window, &rndr, &font, &fontMenu, &fontMenuBig) != 0)
    {
        SDL_Quit();
        return 0;
    }


    //region Prerendered Textures
    SDL_Rect dstTitlebar = {MapPadding, MapPadding, MapWidth, MapHeight};
    SDL_Texture *tTitlebar = SDL_CreateTexture(rndr, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dstTitlebar.w,
                                               dstTitlebar.h);

    SDL_Texture *tScore = DrawScore(rndr, font, game.score);
    SDL_Rect dstScore = {0, MapPadding, 0, 0};
    SDL_QueryTexture(tScore, NULL, NULL, &dstScore.w, &dstScore.h);
    dstScore.x = (MapWidth - dstScore.w) / 2 + MapPadding;

    SDL_Texture *tEscape = DrawText(rndr, font, "menu: Esc");
    SDL_Rect dstEscape = {0, MapPadding, 0, 0};
    SDL_QueryTexture(tEscape, NULL, NULL, &dstEscape.w, &dstEscape.h);
    dstEscape.x = (MapWidth - dstEscape.w) + MapPadding;

    SDL_Rect dstMap = {MapPadding, 2 * MapPadding + TitlebarHeight, MapWidth, MapHeight};
    SDL_Texture *tMap = SDL_CreateTexture(rndr, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dstMap.w, dstMap.h);

    SDL_Texture *tBlocks = SDL_CreateTexture(rndr, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dstMap.w,
                                             dstMap.h);
    SDL_SetRenderTarget(rndr, tBlocks);
    DrawBlocks(rndr, &map);
    SDL_SetRenderTarget(rndr, NULL);

    SDL_Rect dstTimer = {0, 0, 0, 0};
    SDL_Texture *tTimer = DrawTimer(rndr, font, 0);
    SDL_QueryTexture(tTimer, NULL, NULL, &dstTimer.w, &dstTimer.h);
    dstTimer.x = dstEscape.x - dstTimer.w - 2 * DefPadding;
    //endregion

    //region Menu
    Menu mnuMain;
    mnuMain.hoverItem = 0;
    strcpy(mnuMain.items[0], "Resume");
    strcpy(mnuMain.items[1], "New Game");
    strcpy(mnuMain.items[2], "Exit");
    mnuMain.shownItems[0] = false;
    mnuMain.shownItems[1] = true;
    mnuMain.shownItems[2] = true;
    mnuMain.itemCount = 3;
    mnuMain.hoverItem = 1;
    strcpy(mnuMain.title, "PACMAN");
    sprintf(mnuMain.highScore, "High Score: %4d", game.highScore);

    SDL_Texture *tMenu = SDL_CreateTexture(rndr, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WindowWidth,
                                           WindowHeight);
    //endregion

    SDL_Event e;
    bool quit = false, paused = true;
    bool updateMenu = true;
    Action arrow = 0;
    Direction lPacmanDir = DIR_NONE; //Shows correct dir for pacman in stops against blocks
    int lastScore = game.score; //Check it if we should update score texture.
    int finishState = 0;
    int cyclecounter = 0;
    while (!quit)
    {
        ///region Key Input
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
                        if (paused)
                        {
                            while (!mnuMain.shownItems[mnuMain.hoverItem = (mnuMain.hoverItem == 0 ? mnuMain.itemCount -
                                                                                                     1 :
                                                                            mnuMain.hoverItem - 1)]);
                            updateMenu = true;
                        } else
                            arrow = ACTION_UP;
                        break;
                    case SDLK_DOWN:
                        if (paused)
                        {
                            while (!mnuMain.shownItems[mnuMain.hoverItem = ((mnuMain.hoverItem + 1) %
                                                                            mnuMain.itemCount)]);
                            updateMenu = true;
                        } else
                            arrow = ACTION_DOWN;
                        break;
                    case SDLK_p:
                    case SDLK_ESCAPE:
                        paused = !paused;
                        if (paused)
                        {
                            sprintf(mnuMain.title, "Score: %4d", game.score);
                            mnuMain.shownItems[0] = true;
                            mnuMain.hoverItem = 0;
                            updateMenu = true;
                        }
                        break;
                    case SDLK_KP_ENTER:
                    case SDLK_RETURN:
                        if (!paused)
                            break;
                        switch (mnuMain.hoverItem)
                        {
                            case 0: //Resume
                                paused = false;
                                break;
                            case 1: //New Game
                                //if (mnuMain.shownItems[0]) //Check if game is started
                                initiateGame("res\\map.txt", &map, &game, &player, ghosts);
                                mnuMain.subtitle[0] = '\0';
                                mnuMain.showHighScore = false;
                                paused = false;
                                tTimer = DrawTimer(rndr, font, 0);
                                break;
                            case 2: //Exit
                                quit = true;
                        }
                        break;
                }
            }
        }
        ///endregion

        if (!paused && (finishState = isGameFinished(&game, &player)))
        {
            switch (finishState)
            {
                case 1:
                    sprintf(mnuMain.title, "You Lose :(");
                    sprintf(mnuMain.subtitle, "Score: %4d", game.score);
                    break;
                case 2:
                    sprintf(mnuMain.title, "You Win :)");
                    sprintf(mnuMain.subtitle, "Score: %4d", game.score);
                    break;
            }
            if (game.score > game.highScore)
            {
                game.highScore = game.score;
                sprintf(mnuMain.highScore, "New High Score: %4d", game.highScore);
                FILE *fs = fopen("res\\highscore.txt", "w");
                fprintf(fs, "%04d", game.highScore);
                fclose(fs);
            } else
                sprintf(mnuMain.highScore, "High Score: %4d", game.highScore);
            mnuMain.showHighScore = true;
            paused = true;
            updateMenu = true;
            mnuMain.shownItems[0] = false;
            mnuMain.hoverItem =  1;
        }
        if (paused)
        {
            if (updateMenu)
            {
                SDL_SetRenderDrawColor(rndr, 0, 0, 0, 255);
                SDL_RenderClear(rndr);

                SDL_SetRenderTarget(rndr, tMenu);
                SDL_SetRenderDrawColor(rndr, 0, 0, 0, 255);
                SDL_RenderClear(rndr);
                DrawMenu(rndr, fontMenu, fontMenuBig, &mnuMain, WindowWidth, WindowHeight);
                SDL_SetRenderTarget(rndr, NULL);
                SDL_RenderCopy(rndr, tMenu, NULL, NULL);

                updateMenu = false;
            }

        } else
        {
            SDL_SetRenderDrawColor(rndr, 0, 0, 0, 255);
            SDL_RenderClear(rndr);

            if (lastScore != game.score)
            {
                SDL_DestroyTexture(tScore);
                tScore = DrawScore(rndr, font, game.score);
                lastScore = game.score;
            }

            //region Titlebar
            SDL_SetRenderTarget(rndr, tTitlebar);
            SDL_RenderClear(rndr);
            for (Sint16 i = 0; i < player.health; i++)
                filledPieColor(rndr, PacmanLifeSize / 2 + i * PacmanLifeSize, PacmanLifeSize / 2,
                               PacmanLifeSize / 2, 45, 315, PacmanColor);
            if (cyclecounter >= CYCLES_PER_SEC)
            {
                cyclecounter = 0;
                game.timeSeconds++;
                SDL_DestroyTexture(tTimer);
                tTimer = DrawTimer(rndr, font, game.timeSeconds);
            } else
                cyclecounter++;

            SDL_RenderCopy(rndr, tTimer, NULL, &dstTimer);

            //endregion

            SDL_SetRenderTarget(rndr, tMap);
            SDL_SetRenderDrawColor(rndr, 0, 0, 0, 255);
            SDL_RenderClear(rndr);

            SDL_RenderCopy(rndr, tBlocks, NULL, NULL);
            DrawMap(&map, rndr);

            //region Pacman
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
            //endregion

            ///region Ghosts
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
                checkGhostCollision(&player, g, &game);

                DrawGhost(g, &map, rndr);
            }
            ///endregion

            SDL_SetRenderTarget(rndr, NULL);

            SDL_RenderCopy(rndr, tTitlebar, NULL, &dstTitlebar);
            SDL_RenderCopy(rndr, tScore, NULL, &dstScore);
            SDL_RenderCopy(rndr, tEscape, NULL, &dstEscape);
            SDL_RenderCopy(rndr, tMap, NULL, &dstMap);
        }
        SDL_RenderPresent(rndr);
        SDL_Delay(timeDelta);
    }

    //region Destroys
    SDL_DestroyTexture(tTitlebar);
    SDL_DestroyTexture(tScore);
    SDL_DestroyTexture(tEscape);
    SDL_DestroyTexture(tMap);
    SDL_DestroyTexture(tBlocks);
    SDL_DestroyTexture(tMenu);

    SDL_DestroyRenderer(rndr);
    SDL_DestroyWindow(window);

    TTF_CloseFont(font);
    TTF_CloseFont(fontMenu);
    //endregion

    SDL_Quit();
    TTF_Quit();

    return 0;
}

int Init(SDL_Window **window, SDL_Renderer **rndr, TTF_Font **font, TTF_Font **fontMenu, TTF_Font **fontMenuBig)
{
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        printf("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }
    *window = SDL_CreateWindow("Pacman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               WindowWidth, WindowHeight, SDL_WINDOW_OPENGL);
    if (*window == NULL)
    {
        printf("SDL_CreateWindow Error: %s", SDL_GetError());
        return 2;
    }

    *rndr = SDL_CreateRenderer(*window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (*rndr == NULL)
    {
        SDL_DestroyWindow(*window);
        printf("SDL_CreateRenderer Error: %s", SDL_GetError());
        return 3;
    }

    if (!TTF_Init() < 0)
    {
        printf("TTF_Init Error: %s", TTF_GetError());
        return 4;
    }

    *font = TTF_OpenFont("res\\crackman2.ttf", DefFontSize);
    if (*font == NULL)
    {
        printf("TTF_OpenFont Error: %s", TTF_GetError());
        return 5;
    }

    *fontMenu = TTF_OpenFont("res\\crackman2.ttf", MenuFontSize);
    if (*fontMenu == NULL)
    {
        printf("TTF_OpenFont Error: %s", TTF_GetError());
        return 5;
    }

    *fontMenuBig = TTF_OpenFont("res\\crackman2.ttf", MenuFontBigSize);
    if (*fontMenuBig == NULL)
    {
        printf("TTF_OpenFont Error: %s", TTF_GetError());
        return 6;
    }

    return 0;
}

void DrawBlocks(SDL_Renderer *rndr, const Map *map)
{
    for (int i = 0; i < map->width; i++)
        for (int j = 0; j < map->height; j++)
            if (map->cells[i][j] == CELL_BLOCK)
            {
                int corners = 0;
                corners |=
                        0b0001 * (/*i > 0 && j > 0 &&*/ map->cells[i - 1][j] != CELL_BLOCK &&
                                                        map->cells[i][j - 1] != CELL_BLOCK);
                corners |= 0b0010 * (/*i < map->width - 1 && j > 0 &&*/ map->cells[i + 1][j] != CELL_BLOCK &&
                                                                        map->cells[i][j - 1] != CELL_BLOCK);
                corners |=
                        0b0100 * (/*i < map->width - 1 && j < map->height - 1 &&*/ map->cells[i + 1][j] != CELL_BLOCK &&
                                                                                   map->cells[i][j + 1] != CELL_BLOCK);
                corners |= 0b1000 * (/*i > 0 && j < map->height - 1 &&*/ map->cells[i - 1][j] != CELL_BLOCK &&
                                                                         map->cells[i][j + 1] != CELL_BLOCK);

                roundedBoxX(rndr, RCoordinate(i), RCoordinate(j), RCoordinate(i) + CellSize, RCoordinate(j) + CellSize,
                            BlockRadius,
                            corners, BlockColor);
            }
}

void DrawMap(const Map *map, SDL_Renderer *rndr)
{
    for (int i = 0; i < map->width; i++)
        for (int j = 0; j < map->height; j++)
        {
            int x = i * CellSize, y = j * CellSize;
            switch (map->cells[i][j])
            {
                case CELL_CHEESE:
                    filledCircleColor(rndr, x + CellSize / 2, y + CellSize / 2, CheeseSize, CheeseColor);
                    break;
                case CELL_PINEAPPLE:
                    filledCircleColor(rndr, x + CellSize / 2, y + CellSize / 2, PineappleSize, PineappleColor);
                    break;
                case CELL_CHERRY:
                {
                    if (tCherry == NULL)
                    {
                        SDL_Texture *tPre = SDL_GetRenderTarget(rndr);
                        tCherry = SDL_CreateTexture(rndr, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, CellSize,
                                                    CellSize);
                        SDL_SetRenderTarget(rndr, tCherry);
                        Sint16 x1 = CellSize * 9 / 12, x2 = CellSize * 3 / 12,
                                y1 = CellSize * 4 / 5, y2 = CellSize * 4 / 6;
                        filledCircleColor(rndr, x1, y1, CherrySize, CherryColor);
                        filledCircleColor(rndr, x2, y2, CherrySize, CherryColor);
                        Sint16 xs1[] = {CellSize / 2, CellSize / 2, x1, x1};
                        Sint16 ys1[] = {CellPadding, CellSize / 2 - CherrySize, CellSize / 2,
                                        y1 - CherrySize};
                        bezierColor(rndr, xs1, ys1, 4, 10, BranchColor);
                        Sint16 xs2[] = {CellSize / 2, CellSize / 2, x2, x2};
                        Sint16 ys2[] = {CellPadding, CellSize / 2 - CherrySize, CellSize / 2,
                                        y2 - CherrySize};
                        bezierColor(rndr, xs2, ys2, 4, 2, BranchColor);

                        SDL_SetRenderTarget(rndr, tPre);
                    }

                    SDL_Rect dst = {x, y, CellSize, CellSize};
                    SDL_RenderCopy(rndr, tCherry, NULL, &dst);
                }
                    break;
            }
        }
}

void DrawPacmanChar(double x, double y, Sint16 start, Sint16 end, SDL_Renderer *rndr)
{
    filledPieColor(rndr, RCoordinate(x) + CellSize / 2, RCoordinate(y) + CellSize / 2, CellSize / 2 - CellPadding,
                   start, end,
                   PacmanColor);
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

const Uint32 GhostsColors[] = {BlinkyColor, PinkyColor, ClydeColor, InkyColor, GhostBlueColor};

void DrawGhostChar2(SDL_Renderer *rndr, Direction dir, Sint16 x, Sint16 y, Sint16 w, Uint32 color)
{
    //Trunk
    filledPieColor(rndr, x + w / 2, y + w / 2, w / 2, 180, 360, color);
    boxColor(rndr, x, y + w / 2, x + w, y + w - GhostLegHeight, color);

    //Legs
    int dx = w / ((GhostLegCount - 2) * 2 + 2);
    Sint16 xs[GhostLegCount * 2 + 1];
    Sint16 ys[GhostLegCount * 2 + 1];
    xs[0] = x;
    ys[0] = y + w - GhostLegHeight;
    xs[GhostLegCount * 2] = x + w;
    ys[GhostLegCount * 2] = ys[0];
    for (int i = 0; i <= ((GhostLegCount - 2) * 2 + 2); i++)
    {
        xs[i + 1] = x + i * dx;
        ys[i + 1] = i % 2 == 0 ? y + w : ys[0];
    }

    filledPolygonColor(rndr, xs, ys, GhostLegCount * 2 + 1, color);

    //Eyes
    filledCircleColor(rndr, x + w / 3, y + w / 2, GhostEyeSize, 0xFFFFFFFF);
    filledCircleColor(rndr, x + w * 2 / 3, y + w / 2, GhostEyeSize, 0xFFFFFFFF);
    Point ptDir = DirToPt(dir);
    filledCircleColor(rndr, x + w / 3 + ptDir.x * GhostPupilSize, y + w / 2 + ptDir.y * GhostPupilSize,
                      GhostPupilSize, 0xFF000000);
    filledCircleColor(rndr, x + w * 2 / 3 + ptDir.x * GhostPupilSize,
                      y + w / 2 + ptDir.y * GhostPupilSize, GhostPupilSize, 0xFF000000);
}

void DrawGhost(Ghost *ghost, Map *map, SDL_Renderer *rndr)
{
    unsigned int color;
    if (ghost->blue)
        color = GhostBlueColor;
    else
        color = GhostsColors[ghost->type];

    if (ghost->x < 0 || ghost->y < 0 || ghost->x > map->width - 1 || ghost->y > map->height - 1)
        DrawGhostChar2(rndr, ghost->dir, RCoordinate(GetXInBounds(map->width, ghost->x)) + CellPadding,
                       RCoordinate(GetYInBounds(map->height, ghost->y)) + CellPadding, CellSize - 2 * CellPadding,
                       color);
    DrawGhostChar2(rndr, ghost->dir, RCoordinate(ghost->x) + CellPadding, RCoordinate(ghost->y) + CellPadding,
                   CellSize - 2 * CellPadding, color);
}

SDL_Texture *DrawTextColorBlended(SDL_Renderer *rndr, TTF_Font *font, char *text, unsigned int color)
{
    SDL_Surface *sText = TTF_RenderText_Blended(font, text, ToSDLColor(color));
    return SDL_CreateTextureFromSurface(rndr, sText);
}

SDL_Texture *DrawTextColor(SDL_Renderer *rndr, TTF_Font *font, char *text, unsigned int color)
{
    SDL_Surface *sText = TTF_RenderText_Solid(font, text, ToSDLColor(color));
    return SDL_CreateTextureFromSurface(rndr, sText);
}

SDL_Texture *DrawText(SDL_Renderer *rndr, TTF_Font *font, char *text)
{
    SDL_Surface *sText = TTF_RenderText_Solid(font, text, (SDL_Color) {255, 255, 255});
    return SDL_CreateTextureFromSurface(rndr, sText);
}

SDL_Texture *DrawScore(SDL_Renderer *rndr, TTF_Font *font, int score)
{
    char strScore[20];
    sprintf(strScore, "Score: %4d", score);
    return DrawText(rndr, font, strScore);
}

SDL_Texture *DrawTimer(SDL_Renderer *rndr, TTF_Font *font, unsigned int seconds)
{
    char strTime[20];
    sprintf(strTime, "Time: %4u", seconds);
    return DrawText(rndr, font, strTime);
}

void DrawMenu(SDL_Renderer *rndr, TTF_Font *font, TTF_Font *fontBig, Menu *mnu, int w, int h)
{
    int counter = 0, i;
    for (i = 0; i < mnu->itemCount; i++)
        counter += mnu->shownItems[i];

    SDL_Texture *t = SDL_CreateTexture(rndr, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, MenuItemWidth,
                                       (counter) * (MenuItemHeight + MenuItemPadding) + MenuItemHeight +
                                       (strcmp(mnu->subtitle, "") != 0 ? MenuItemHeight : 0) + 2 * MenuItemPadding +
                                       (mnu->showHighScore ? MenuItemHeight : 0));
    SDL_Texture *tPre = SDL_GetRenderTarget(rndr);
    SDL_SetRenderTarget(rndr, t);

    int baseY = 0;

    SDL_Rect dst = {0, 0, 0, 0};
    {
        SDL_Texture *tTitle = DrawTextColorBlended(rndr, fontBig, mnu->title, PacmanColor);
        SDL_QueryTexture(tTitle, NULL, NULL, &dst.w, &dst.h);
        dst.x = (MenuItemWidth - dst.w) / 2;
        SDL_RenderCopy(rndr, tTitle, NULL, &dst);
        SDL_DestroyTexture(tTitle);
        baseY += MenuTitleHeight;
    }
    if (strcmp(mnu->subtitle, "") != 0)
    {
        SDL_Texture *tSubtitle = DrawTextColorBlended(rndr, font, mnu->subtitle, PacmanColor);
        SDL_QueryTexture(tSubtitle, NULL, NULL, &dst.w, &dst.h);
        dst.x = (MenuItemWidth - dst.w) / 2;
        dst.y = baseY;
        SDL_RenderCopy(rndr, tSubtitle, NULL, &dst);
        SDL_DestroyTexture(tSubtitle);
        baseY += MenuItemHeight;
    }
    if (mnu->showHighScore)
    {
        SDL_Texture *tHighScore = DrawTextColorBlended(rndr, font, mnu->highScore, PacmanColor);
        SDL_QueryTexture(tHighScore, NULL, NULL, &dst.w, &dst.h);
        dst.x = (MenuItemWidth - dst.w) / 2;
        dst.y = baseY;
        SDL_RenderCopy(rndr, tHighScore, NULL, &dst);
        SDL_DestroyTexture(tHighScore);
        baseY += MenuItemHeight;
    }
    baseY += 2 * MenuItemPadding;

    for (i = 0, counter = 0; i < mnu->itemCount; i++)
    {
        if (!mnu->shownItems[i])
            continue;
        SDL_Texture *tItem = DrawTextColorBlended(rndr, font, mnu->items[i], mnu->hoverItem == i ? BlinkyColor : 0xFFFFFFFF);
        dst.y = baseY;
        SDL_QueryTexture(tItem, NULL, NULL, &dst.w, &dst.h);
        dst.x = (MenuItemWidth - dst.w) / 2;
        dst.y += (MenuItemHeight - dst.h) / 2;
        SDL_RenderCopy(rndr, tItem, NULL, &dst);
        SDL_DestroyTexture(tItem);
        counter++;
        baseY += MenuItemHeight + MenuItemPadding;
    }

    SDL_SetRenderTarget(rndr, tPre);
    SDL_QueryTexture(t, NULL, NULL, &dst.w, &dst.h);
    dst.x = (w - dst.w) / 2;
    dst.y = (h - dst.h) / 2;
    SDL_RenderCopy(rndr, t, NULL, &dst);
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
