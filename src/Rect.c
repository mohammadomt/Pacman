//
// Created by Mohammad on 1/4/2018.
//

#include <Basics.h>

bool RectContains(RectD rect, double x, double y)
{
    return (x >= rect.left && x <= rect.right) && (y >= rect.top && y <= rect.bottom);
}

//Check intersection.
bool RectIntersectsWith(RectD src, RectD dst)
{
    return src.top < dst.bottom &&
           src.left < dst.right &&
           src.bottom > dst.top &&
           src.right > dst.left;
}