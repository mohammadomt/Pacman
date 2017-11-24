#include <stdio.h>

char table[100][100];

enum MDir
{
    Top = 1, Right = 2, Down = 3, Left = 4
};

struct Point
{
    int X;
    int Y;
};

struct Ghost
{
    enum GTypes
    {
        Blinky = 0, Pinky = 1, Clyde = 2, Inky = 3
    } Type;

    enum Modes
    {
        Deff = 0, Off = 1
    } Mode;
    int DeffRemained;

    enum MDir CurrentDir;

    struct Point BaseLoc;
    struct Point Loc;
} Ghosts[4];

struct Pacman
{
    int LivesC;

    enum MDir CurrentDir;

    struct Point BaseLoc;
    struct Point Loc;
} Player;


int main()
{
    int n,m;
    scanf("%d%d", &n, &m);
    for(int i=0;i<n;i++)
        for (int j = 0 ; j<m;j++)
            scanf("%c", &table[i][j]);

    //TODO: Time and Game Score?!
    scanf("%s%d");

    scanf("%d %d (%d,%d) (%d,%d)", &Player.CurrentDir, &Player.LivesC, &Player.BaseLoc.X, &Player.BaseLoc.Y,
          &Player.Loc.X, &Player.Loc.Y);

    for (int i = 0; i < 4; i++)
    {
        Ghosts[i].Type = i;
        scanf("%d %d", &Ghosts[i].CurrentDir, &Ghosts[i].Mode);

        if (Ghosts[i].Mode == Deff)
            scanf(" %d", &Ghosts[i].DeffRemained);
        scanf(" (%d,%d) (%d,%d)", &Ghosts[i].BaseLoc.X, &Ghosts[i].BaseLoc.Y, &(Ghosts[i].Loc.X), &Ghosts[i].Loc.Y);
    }
}


void PrintGhost(struct Ghost g)
{
    printf("%d %d %d %d %d,%d %d,%d\n", g.Type, g.CurrentDir, g.Mode, g.DeffRemained, g.BaseLoc.X, g.BaseLoc.Y, g.Loc.X,
           g.Loc.Y);
}

void PrintPac(struct Pacman p)
{
    printf("%d %d %d,%d\n", p.CurrentDir, p.LivesC, p.BaseLoc.X, p.BaseLoc.Y);
}