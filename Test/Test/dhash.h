#ifndef DHASH_H
#define DHASH_H
#include "imgg.h"

int dhash_cvresize(Imgg* const imgg, int* const array, int const height, int const width);
double dhash_similarity(unsigned char* array1, unsigned char* array2, int const hash_size);
#endif

