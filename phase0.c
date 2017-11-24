#include <stdio.h>

char table [100][100];

enum MDir {Top = 1, Right = 2, Down = 3, Left =4};

struct Point
{
    int X;
    int Y;
};

struct Ghost
{
    enum GType {Blinky = 0, Pinky = 1, Clyde = 2, Inky= 3} Type;
    enum Mode {Deff = 0, Off =1 } Mode;

    enum MDir CurrentDir;

    struct Point BaseLoc;
    struct Point Loc;
};

struct Pacman
{
    int LivesC;

    enum MDir CurrentDir;

    struct Point BaseLoc;
    struct Point Loc;
};

int main()
{
    int n,m;
    scanf("%d%d", &n, &m);
    for(int i=0;i<n;i++)
        for (int j = 0 ; j<m;j++)
            scanf("%c", &table[i][j]);
    
}
