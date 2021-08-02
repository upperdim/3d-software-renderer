#include <math.h>

#include "matrix.h"
#include "util.h"

vector3d matrix_mult_vector(matrix4x4 *m, vector3d *i)
{
	return (vector3d) {
		i->x * m->m[0][0] + i->y * m->m[1][0] + i->z * m->m[2][0] + i->w * m->m[3][0],
		i->x * m->m[0][1] + i->y * m->m[1][1] + i->z * m->m[2][1] + i->w * m->m[3][1],
		i->x * m->m[0][2] + i->y * m->m[1][2] + i->z * m->m[2][2] + i->w * m->m[3][2],
		i->x * m->m[0][3] + i->y * m->m[1][3] + i->z * m->m[2][3] + i->w * m->m[3][3]
	};
}

matrix4x4 matrix_identity()
{
	matrix4x4 id_m = {0};
	id_m.m[0][0] = 1.0;
	id_m.m[1][1] = 1.0;
	id_m.m[2][2] = 1.0;
	id_m.m[3][3] = 1.0;
	return id_m;
}

matrix4x4 matrix_rotation_x(double angleRadian)
{
	matrix4x4 m_rot_x = {0};
	m_rot_x.m[0][0] = 1.0;
	m_rot_x.m[1][1] = cos(angleRadian);
	m_rot_x.m[1][2] = sin(angleRadian);
	m_rot_x.m[2][1] = -sin(angleRadian);
	m_rot_x.m[2][2] = cos(angleRadian);
	m_rot_x.m[3][3] = 1.0;
	return m_rot_x;
}

matrix4x4 matrix_rotation_y(float angleRadian)
{
	matrix4x4 m_rot_y = {0};
	m_rot_y.m[0][0] = cos(angleRadian);
	m_rot_y.m[0][2] = sin(angleRadian);
	m_rot_y.m[2][0] = -sin(angleRadian);
	m_rot_y.m[1][1] = 1.0;
	m_rot_y.m[2][2] = cos(angleRadian);
	m_rot_y.m[3][3] = 1.0;
	return m_rot_y;
}

matrix4x4 matrix_rotation_z(float angleRadian)
{
	matrix4x4 m_rot_z = {0};
	m_rot_z.m[0][0] = cos(angleRadian);
	m_rot_z.m[0][1] = sin(angleRadian);
	m_rot_z.m[1][0] = -sin(angleRadian);
	m_rot_z.m[1][1] = cos(angleRadian);
	m_rot_z.m[2][2] = 1.0;
	m_rot_z.m[3][3] = 1.0;
	return m_rot_z;
}

matrix4x4 matrix_translation(double x, double y, double z)
{
	matrix4x4 m_trans = {0};
	m_trans.m[0][0] = 1.0;
	m_trans.m[1][1] = 1.0;
	m_trans.m[2][2] = 1.0;
	m_trans.m[3][3] = 1.0;
	m_trans.m[3][0] = x;
	m_trans.m[3][1] = y;
	m_trans.m[3][2] = z;
	return m_trans;
}

matrix4x4 matrix_projection(double fovDegrees, double aspectRatio, double near, double far)
{
	double fovRadians = 1.0 / tan(fovDegrees * 0.5 / 180.0 * PI);

	matrix4x4 m_proj = {0};
	m_proj.m[0][0] = aspectRatio * fovRadians;
	m_proj.m[1][1] = fovRadians;
	m_proj.m[2][2] = far / (far - near);
	m_proj.m[3][2] = (-far * near) / (far - near);
	m_proj.m[2][3] = 1.0;
	m_proj.m[3][3] = 0.0;
	return m_proj;
}

matrix4x4 matrix_mult_matrix(matrix4x4 *m1, matrix4x4 *m2)
{
	matrix4x4 result = { 0 };

	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			result.m[row][col] = m1->m[row][0] * m2->m[0][col] + m1->m[row][1] * m2->m[1][col] + m1->m[row][2] * m2->m[2][col] + m1->m[row][3] * m2->m[3][col];
		}
	}

	return result;
}
