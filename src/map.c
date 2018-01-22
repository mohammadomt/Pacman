#include "game.h"
#include "map.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <Basics.h>


void MakeInBounds(const Map *map, int *x, int *y)
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

double GetXInBounds(double width, double x)
{
    if (x < 0)
        return x + width;
    else if (x > width - 1)
        return x - width;
    else
        return x;
}

double GetYInBounds(double height, double y)
{
    if (y < 0)
        return y + height;
    else if (y > height - 1)
        return y - height;
    else
        return y;
}

Point DirToPt(Direction dir)
{
    Point retVal;
    retVal.x = 0;
    retVal.y = 0;
    switch (dir)
    {
        case DIR_UP:
            retVal.y = -1;
            break;
        case DIR_RIGHT:
            retVal.x = 1;
            break;
        case DIR_DOWN:
            retVal.y = 1;
            break;
        case DIR_LEFT:
            retVal.x = -1;
            break;
    }
    return retVal;
}

Direction PtToDir(Point pt)
{
    if (pt.y == -1)
        return DIR_UP;
    else if (pt.x == 1)
        return DIR_RIGHT;
    else if (pt.y == 1)
        return DIR_DOWN;
    else if (pt.x == -1)
        return DIR_LEFT;
    return DIR_NONE;
}

void GetPacmanCCell(Pacman *pacman, int *x, int *y)
{
    *x = (int) round(pacman->x);
    *y = (int) round(pacman->y);
}


extern int FreeBlocksCount;
extern LinkedPoint *FoundPaths;
extern int FoundPathsCount;

LinkedPoint *FindPath(const Map *map, int fromX, int fromY, int toX, int toY) //deprecated
{
    LinkedPoint *queue = (LinkedPoint *) malloc(MAP_MAX_SIZE * MAP_MAX_SIZE * sizeof(LinkedPoint));
    bool visited[MAP_MAX_SIZE][MAP_MAX_SIZE] = {false};

    int index = 0, pointer = 0;
    Point from;
    from.x = fromX;
    from.y = fromY;
    queue[pointer++].current = from;

    while (index < pointer)
    {
        Point current = queue[index++].current;

        visited[current.x][current.y] = true;

        for (int dir = 1; dir < 5; dir++)
        {
            Point ptNext = DirToPt((Direction) dir);
            ptNext.x += current.x;
            ptNext.y += current.y;
            MakeInBounds(map, &ptNext.x, &ptNext.y);

            if (visited[ptNext.x][ptNext.y])
                continue;

            if (map->cells[ptNext.x][ptNext.y] != CELL_BLOCK)
            {
                LinkedPoint child;
                child.current = ptNext;
                child.pre = current;
                queue[pointer++] = child;
                if (ptNext.x == toX && ptNext.y == toY)
                    return queue + pointer - 1;
            }
        }
    }

    return NULL;
}

Point FindPathNext(const Map *map, int fromX, int fromY, int toX, int toY)
{
    if(fromX==toX && fromY==toY)
        return (Point){0,0};
    LinkedPoint *queue = (LinkedPoint *) malloc(FreeBlocksCount * FreeBlocksCount * sizeof(LinkedPoint));
    bool visited[MAP_MAX_SIZE][MAP_MAX_SIZE] = {false};

    int index = 0, pointer = 0;
    Point from;
    //We work reversely to find next move
    from.x = toX;
    from.y = toY;
    queue[pointer++].current = from;

    while (index < pointer)
    {
        Point current = queue[index++].current;
        visited[current.x][current.y] = true;

        for (int dir = 1; dir <= 4; dir++)
        {
            Point ptNext = DirToPt((Direction) dir);
            ptNext.x += current.x;
            ptNext.y += current.y;
            MakeInBounds(map, &ptNext.x, &ptNext.y);

            if (visited[ptNext.x][ptNext.y])
                continue;

            if (map->cells[ptNext.x][ptNext.y] != CELL_BLOCK)
            {
                LinkedPoint child;
                child.current = ptNext;
                child.pre = current;
                queue[pointer++] = child;
                if (ptNext.x == fromX && ptNext.y == fromY)
                {
                    free(queue);
                    return current;
                }
            }
        }
    }

    free(queue);
    fprintf(stderr,"%d, %d is unreachable index: %d from: %d, %d\n", toX, toY, index, fromX, fromY);
    return (Point) {-1, -1};
}

Direction GetMoveDirTo(const Map *map, Point from, Point to)
{
    //Checking equality
    if (PtIsEqual(&to, &from))
    {
        //Copied from Phase1
        fprintf(stderr, "from and to are equal\n");
        bool availableDirs[4] = {false};
        for (int i = 0; i < 4; i++)
        {
            Point dirPt = DirToPt(i + 1);
            int nextX = from.x + dirPt.x;
            int nextY = from.y + dirPt.y;
            MakeInBounds(map, &nextX, &nextY);
            availableDirs[i] = map->cells[nextX][nextY] != CELL_BLOCK;
        }
        int i = rand() % 4;
        while (!availableDirs[(++i) % 4]);
        return (Direction) (i % 4 + 1);
    }

    //Checking repeated moves
    for (int i = 0; i < FoundPathsCount; i++)
        if (PtIsEqual(&FoundPaths[i].current, &to))
            if (FoundPaths[i].dir == DIR_NONE)
            {
                fprintf(stderr, "Repeated unreachable point: %d, %d\n", to.x, to.y);
                return DIR_NONE;
            } else if (PtIsEqual(&FoundPaths[i].pre, &from))
            {
                fprintf(stderr, "Repeated: %d, %d to %d, %d\n", from.x, from.y, to.x, to.y);
                return FoundPaths[i].dir;
            }

    //Getting path
//    LinkedPoint *queue = FindPath(map, from.x, from.y, to.x, to.y);
    Direction res;
//    if (queue == NULL)
//    {
//        fprintf(stderr, "Unreachable point: %d, %d\n", to.x, to.y);
//        res = DIR_NONE;
//    } else
//    {
//        //Extracting the next move
//        Point search = to;
//        while (true)
//        {
//            if (PtIsEqual(&queue->current, &search))
//                if (PtIsEqual(&queue->pre, &from))
//                {
//                    Point ptDir = search;
//                    ptDir.x -= from.x;
//                    ptDir.y -= from.y;
//                    if (Abs(ptDir.x) > 1)
//                        ptDir.x += -Sign(ptDir.x) * map->width;
//                    if (Abs(ptDir.y) > 1)
//                        ptDir.y += -Sign(ptDir.y) * map->height;
//                    res = PtToDir(ptDir);
//                    break;
//                } else
//                    search = queue->pre;
//            queue--;
//        }
//    }

    Point ptDir = FindPathNext(map, from.x, from.y, to.x, to.y);
    if (ptDir.x == -1)
    {
        fprintf(stderr, "Unreachable point is found and stored: %d, %d\n", to.x, to.y);
        res = DIR_NONE;
    } else
    {
        ptDir.x -= from.x;
        ptDir.y -= from.y;
        if (Abs(ptDir.x) > 1)
            ptDir.x += -Sign(ptDir.x) * map->width;
        if (Abs(ptDir.y) > 1)
            ptDir.y += -Sign(ptDir.y) * map->height;
        res = PtToDir(ptDir);
    }

    //Adding new found path
    LinkedPoint newRec;
    newRec.pre = from;
    newRec.current = to;
    newRec.dir = res;
    FoundPaths[FoundPathsCount++] = newRec;

    return res;
}

bool IsReachable(const Map *map, Point pt, Point from)
{
    for (int i = 0; i < FoundPathsCount; i++)
        if (PtIsEqual(&FoundPaths[i].current, &pt))
            if (FoundPaths[i].dir == DIR_NONE)
            {
                fprintf(stderr, "Repeated unreachable point: %d, %d\n", pt.x, pt.y);
                return false;
            } else
                return true;
    if (FindPathNext(map, from.x, from.y, pt.x, pt.y).x == -1)
    {
        LinkedPoint newRec;
        newRec.pre = from;
        newRec.current = pt;
        newRec.dir = DIR_NONE;
        FoundPaths[FoundPathsCount++] = newRec;
        fprintf(stderr, "Unreachable point is found and stored: %d, %d\n", pt.x, pt.y);
        return false;
    } else
        return true;
}

Point GetNearestNB(const Map *map, int x, int y, Point helper)
{
    int radius = 0;
    while (true)
    {
        radius++;
        for (int i = -radius; i <= radius; i++)
            for (int j = -radius; j <= radius; j++)
                if (x + i >= 0 && x + i < map->width && y + j >= 0 && y + j < map->height)
                    if (map->cells[x + i][y + j] != CELL_BLOCK && IsReachable(map, (Point) {x + i, y + j}, helper))
                        return (Point) {x + i, y + j};
    }
}