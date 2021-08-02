#include <math.h>

#include "vector.h"

vector3d vector_add(vector3d *v1, vector3d *v2)
{
	return (vector3d) {(v1->x + v2->x), (v1->y + v2->y), (v1->z + v2->z), (VECTOR_DEFAULT_W)};
}

vector3d vector_sub(vector3d *v1, vector3d *v2)
{
	return (vector3d) {(v1->x - v2->x), (v1->y - v2->y), (v1->z - v2->z), (VECTOR_DEFAULT_W)};
}

vector3d vector_mul(vector3d *v, double k)
{
	return (vector3d) {(v->x * k), (v->y * k), (v->z * k), (VECTOR_DEFAULT_W)};
}

vector3d vector_div(vector3d *v, double k)
{
	return (vector3d) {(v->x / k), (v->y / k), (v->z / k), (VECTOR_DEFAULT_W)};
}

double vector_length(vector3d *v)
{
	return sqrt(vector_dot_prod(v, v));
}

vector3d vector_normalise(vector3d *v)
{
	double len = vector_length(v);
	return (vector3d) {(v->x / len), (v->y / len), (v->z / len), (VECTOR_DEFAULT_W)};
}

double vector_dot_prod(vector3d *v1, vector3d *v2)
{
	return (v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z);
}

vector3d vector_cross_prod(vector3d *v1, vector3d *v2)
{
	return (vector3d) {
		v1->y * v2->z - v1->z * v2->y,
		v1->z * v2->x - v1->x * v2->z,
		v1->x * v2->y - v1->y * v2->x,
		VECTOR_DEFAULT_W
	};
}
