#include "dhash.h"

int dhash_cvresize(Imgg* const imgg, int* const array, int const height, int const width)
{
	int cols = 0;
	int row = 0;
	int count = 0;
	for (cols = 0; cols < height; cols++) {
		for (row = 0; row < width - 1; row++) {
			if (imgg->data[cols * width + row] < imgg->data[cols * width + row + 1]) {
				(array)[count] = 1;
			}
			else { (array)[count] = 0; }
			count++;
		}
	}
	return 0;
}

double dhash_similarity(unsigned char* array1, unsigned char* array2, int const hash_size)

{
	double similarity;
	int Different_quantity = 0;
	int i = 0;
	for (i = 0; i < hash_size; ++i)
	{
		if (array1[i] != array2[i])
		{
			Different_quantity += 1;
		}
	}
	similarity = 1 - (double)Different_quantity / (double)(hash_size);
	return similarity;
}




