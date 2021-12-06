#pragma once
#ifndef IMGG_H
#define IMGG_H

typedef struct _Imgg {
    int width;
    int height;
    unsigned char* data;
    
} Imgg;

void Imgg_ctor(Imgg* const me, int width, int height);
void Imgg_assignment(Imgg* const me, int x, int y, int data);
int Imgg_get(Imgg* const me, int x, int y);

#endif /*IMGG_H*/

