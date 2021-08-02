#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>


#define create_max_func(datatype)               \
datatype max_##datatype(datatype a, datatype b) \
{                                               \
	return a > b ? a : b;                       \
}                                               \


#define create_swap_func(datatype)              \
void swap_##datatype(datatype *a, datatype *b)  \
{                                               \
	datatype temp = *a;                         \
	*a = *b;                                    \
	*b = temp;                                  \
}                                               \


#define dt_ms(oldtime) (clock() - (oldtime)) / (CLOCKS_PER_SEC / 1000)
#define PI             3.14159

void   swap_int(int *, int *);
double max_double(double, double);

// Reads a line from file pointer. Returns the line and its length via pass by reference.
// Returns  0 if it successfully read a line.
// Returns -1 if it could not (EOF).
// Returns -2 if it could not ('\n').
int get_line(char **lineptr, size_t *len, FILE *objFile);


#endif