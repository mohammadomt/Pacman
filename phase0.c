#include <stdio.h>

enum Dir
{
    Top = 1, Right = 2, Bottom = 3, Left = 4
};

struct Point
{
    int X;
    int Y;
};

struct Point DirToPt(enum Dir d)
{
    struct Point retVal;
    retVal.X = 0;
    retVal.Y = 0;
    switch (d)
    {
        case Top:
            retVal.Y = -1;
            break;
        case Right:
            retVal.X = 1;
            break;
        case Bottom:
            retVal.Y = 1;
            break;
        case Left:
            retVal.X = -1;
            break;
    }
    return retVal;
}

struct Ghost
{
    enum GTypes
    {
        Blinky = 0, Pinky = 1, Clyde = 2, Inky = 3
    } Type;

//    enum Modes
//    {
//        Deff = 0, Off = 1
//    } Mode;
    int DeffRemained;

    //Move Direction
    enum Dir MDir;

    struct Point BaseLoc;
    struct Point Loc;
} Ghosts[4];

struct Pacman
{
    int LivesC;

    //Move Direction
    enum Dir MDir;

    struct Point BaseLoc;
    struct Point Loc;
} Player;

struct TimeSpan
{
    int Hours;
    int Minutes;
    int Seconds;
} GameTime;


int CurrentScore = 0;

//Current Map
char Table[100][100];
struct Point MapSize;

int IsWon();
void MovePacman();
void MovePlayer(int,int);
void MakeGhostsPAed();
int ComparePoint(struct Point *, struct Point *);

#ifdef DEBUG
void PrintGhost(struct Ghost);
void PrintPac(struct Pacman);
#endif

//***As x stands for horizontal axis we use x for column number***\\

int main()
{
    //Input
    {
        int n, m;
        scanf("%d%d", &n, &m);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m;)
            {
                char c;
                scanf("%c", &c);
                if (c != '\n')
                {
                    Table[i][j] = c;
                    j++;
                }
            }
        MapSize.Y = n;
        MapSize.X = m;

        scanf("%d:%d %d", &GameTime.Minutes, &GameTime.Seconds, &CurrentScore);

        scanf("%*s %d %d (%d,%d) (%d,%d)", &Player.MDir, &Player.LivesC, &Player.BaseLoc.Y, &Player.BaseLoc.X,
              &Player.Loc.Y, &Player.Loc.X);

        for (int i = 0; i < 4; i++)
        {
            Ghosts[i].Type = i;
            int mode;
            scanf("%*s%d %d", &Ghosts[i].MDir, &mode);
            if (mode == 0)
                scanf(" %d", &Ghosts[i].DeffRemained);
            scanf(" (%d,%d) (%d,%d)", &Ghosts[i].BaseLoc.Y, &Ghosts[i].BaseLoc.X, &Ghosts[i].Loc.Y, &Ghosts[i].Loc.X);
        }
    }

    MovePacman();
    printf("(%d,%d)\n", Player.Loc.Y, Player.Loc.X);
    printf("%d\n", CurrentScore);
    printf(IsWon()? "Yes":"No");
}

int IsWon()
{
    for(int i=0; i<MapSize.Y;i++)
        for(int j=0; j<MapSize.X;j++)
            if(Table[i][j]=='*' || Table[i][j]=='O')
                return 0;
    return 1;
}

void MovePacman()
{
    struct Point p = DirToPt(Player.MDir);
    MovePlayer(p.X,p.Y);

    char c = Table[Player.Loc.Y][Player.Loc.X];
    switch (c)
    {
        case '*':
            CurrentScore++;
            break;
        case '^':
            CurrentScore += 20;
            break;
        case 'O':
            MakeGhostsPAed();
            break;
        case '#':
            //Undo the move
            MovePlayer(-p.X, -p.Y);
            break;
    }

    if (c == "#")
        goto GIgnored;
    int isDead = 0;
    for (int i = 0; i < 4; i++)
        if (ComparePoint(&Ghosts[i].Loc, &Player.Loc))
            if (Ghosts[i].DeffRemained > 0)
            {
                CurrentScore += 50;
                Ghosts[i].Loc = Ghosts[i].BaseLoc;
            } else
                isDead = 1;

    Table[Player.Loc.Y][Player.Loc.X] = '_';
    if (isDead)
    {
        Player.Loc = Player.BaseLoc;
        Player.LivesC--;
    }

    GIgnored:
    ;
}

//Manages out of bounds
void MovePlayer(int x, int y)
{
    struct Point loc = Player.Loc;
    loc.X += x;
    loc.Y += y;

    if (loc.X > MapSize.X - 1)
        loc.X = 0;
    if (loc.Y > MapSize.Y - 1)
        loc.Y = 0;
    if (loc.X < 0)
        loc.X = MapSize.X - 1;
    if (loc.Y < 0)
        loc.Y = MapSize.Y - 1;
    Player.Loc = loc;
}

//Applies pineapple effect on ghosts
void MakeGhostsPAed()
{
    for (int i = 0; i < 4; i++)
    {
        Ghosts[i].DeffRemained = 10;
    }
}


int ComparePoint(struct Point *p1, struct Point *p2)
{
    return p1->X == p2->X && p1->Y == p2->Y;
}

#ifdef DEBUG
void PrintGhost(struct Ghost g)
{
    printf("%d %d %d %d,%d %d,%d\n", g.Type, g.MDir, g.DeffRemained, g.BaseLoc.X, g.BaseLoc.Y, g.Loc.X,
           g.Loc.Y);
}

void PrintPac(struct Pacman p)
{
    printf("%d %d %d,%d\n", p.MDir, p.LivesC, p.BaseLoc.X, p.BaseLoc.Y);
}
#endif