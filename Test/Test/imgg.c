#include "imgg.h"

void Imgg_ctor(Imgg* const me, int width, int height)
{
    me->width = width;
    me->height = height;
}

void Imgg_assignment(Imgg* const me, int x, int y, int data)
{
    me->data[y * me->width + x] = data;
}

int Imgg_get(Imgg* const me, int x, int y)
{
    return me->data[y * me->width + x];
}
