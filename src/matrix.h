#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct {
	double m[4][4];
} matrix4x4;


vector3d matrix_multiply_vector(matrix4x4 *m, vector3d *i);
matrix4x4 matrix_identity();
matrix4x4 matrix_rotation_x(double angleRadian);
matrix4x4 matrix_rotation_y(float angleRadian);
matrix4x4 matrix_rotation_z(float angleRadian);
matrix4x4 matrix_translation(double x, double y, double z);
matrix4x4 matrix_projection(double fovDegrees, double aspectRatio, double near, double far);
matrix4x4 matrix_mult_matrix(matrix4x4 *m1, matrix4x4 *m2);


#endif