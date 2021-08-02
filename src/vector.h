#ifndef VECTOR_H
#define VECTOR_H


#define VECTOR_DEFAULT_W 1.0


typedef struct {
	double x, y, z, w;
} vector3d;


vector3d vector_add(vector3d *v1, vector3d *v2);
vector3d vector_sub(vector3d *v1, vector3d *v2);
vector3d vector_mul(vector3d *v, double k);
vector3d vector_div(vector3d *v, double k);

double   vector_length(vector3d *v);
vector3d vector_normalise(vector3d *v);

double   vector_dot_prod(vector3d *v1, vector3d *v2);
vector3d vector_cross_prod(vector3d *v1, vector3d *v2);


#endif