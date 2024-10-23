#include "util.h"

// https://stackoverflow.com/a/29871193

double wrapMax(double x, double max) { return fmod(max + fmod(x, max), max); }

double wrapMinMax(double x, double min, double max) { return min + wrapMax(x - min, max - min); }

// using this resource as a reference for quaternion math
// https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html

void inverse_quat(GFC_Vector4D *quat) {
	quat->x = -quat->x;
	quat->y = -quat->y;
	quat->z = -quat->z;
}

void quat_mult(GFC_Vector4D *dst, GFC_Vector4D a, GFC_Vector4D b) {
	dst->w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
	dst->x = a.w * b.x + a.x * b.w - a.y * b.z + a.z * b.y;
	dst->y = a.w * b.y + a.x * b.z + a.y * b.w - a.z * b.x;
	dst->z = a.w * b.z - a.x * b.y + a.y * b.x + a.z * b.w;
}

void axis_angle_to_quat(GFC_Vector4D *dst, GFC_Vector4D rotation) {
	dst->w = cos(rotation.w / 2.0);
	double s = sin(rotation.w / 2.0);
	dst->x = rotation.x * s;
	dst->y = rotation.y * s;
	dst->z = rotation.z * s;
}

void quat_to_axis_angle(GFC_Vector4D *dst, GFC_Vector4D quat) {
	if(fabsf(quat.w) == 1.0) {
		*dst = gfc_vector4d(1, 0, 0, 0);
		return;
	}
	double angle = 2.0 * acos(quat.w);
	double s = sin(angle / 2.0);
	dst->x = quat.x / s;
	dst->y = quat.y / s;
	dst->z = quat.z / s;
	dst->w = angle;
}

void quat_to_euler_vector(GFC_Vector3D *dst, GFC_Vector4D quat) {
	quat_to_axis_angle(&quat, quat);
	axis_angle_to_euler_vector(dst, quat);
}

// NOT an euler angle, euler vector: https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation
void euler_vector_to_axis_angle(GFC_Vector4D *dst, GFC_Vector3D eulerVector) {
	double l = gfc_vector3d_magnitude(eulerVector);
	if(l == 0.0) {
		*dst = gfc_vector4d(1, 0, 0, 0);
		return;
	}
	gfc_vector3d_scale(eulerVector, eulerVector, (1.0 / l));
	dst->w = l;
	dst->x = eulerVector.x;
	dst->y = eulerVector.y;
	dst->z = eulerVector.z;
}

void axis_angle_to_euler_vector(GFC_Vector3D *dst, GFC_Vector4D axisAngle) {
	dst->x = axisAngle.x * axisAngle.w;
	dst->y = axisAngle.y * axisAngle.w;
	dst->z = axisAngle.z * axisAngle.w;
}

void euler_vector_to_quat(GFC_Vector4D *dst, GFC_Vector3D eulerVector) {
	euler_vector_to_axis_angle(dst, eulerVector);
	axis_angle_to_quat(dst, *dst);
}

void rotate_vector3_by_quat(GFC_Vector3D *vector, GFC_Vector4D quat) {
	GFC_Vector4D p;
	p.x = vector->x;
	p.y = vector->y;
	p.z = vector->z;
	p.w = 0;
	GFC_Vector4D invQuat = quat;
	inverse_quat(&invQuat);
	quat_mult(&p, invQuat, p);
	quat_mult(&p, p, quat);
	gfc_vector3d_copy((*vector), p);
}

void rotate_vector3_by_axis_angle(GFC_Vector3D *vector, GFC_Vector4D axisAngle) {
	GFC_Vector4D quat;
	axis_angle_to_quat(&quat, axisAngle);
	rotate_vector3_by_quat(vector, quat);
}

void rotate_vector3_by_euler_vector(GFC_Vector3D *vector, GFC_Vector3D eulerVector) {
	GFC_Vector4D axisAngle;
	euler_vector_to_axis_angle(&axisAngle, eulerVector);
	rotate_vector3_by_axis_angle(vector, axisAngle);
}

void wrap_euler_vector(GFC_Vector3D *eulerVector) {
	GFC_Vector4D axisAngle;
	euler_vector_to_axis_angle(&axisAngle, *eulerVector);
	wrap_axis_angle(&axisAngle);
	axis_angle_to_euler_vector(eulerVector, axisAngle);
}

void wrap_axis_angle(GFC_Vector4D *axisAngle) { axisAngle->w = wrapMinMax(axisAngle->w, -M_PI * 2, M_PI * 2); }

GFC_Vector3D compose_euler_vectors(GFC_Vector3D a, GFC_Vector3D b) {
	GFC_Vector4D aAxisAngle;
	euler_vector_to_axis_angle(&aAxisAngle, a);
	GFC_Vector4D bAxisAngle;
	euler_vector_to_axis_angle(&bAxisAngle, b);
	GFC_Vector3D composed;
	axis_angle_to_euler_vector(&composed, compose_axis_angles(aAxisAngle, bAxisAngle));
	return composed;
}

GFC_Vector4D compose_axis_angles(GFC_Vector4D a, GFC_Vector4D b) {
	axis_angle_to_quat(&a, a);
	axis_angle_to_quat(&b, b);
	GFC_Vector4D q;
	quat_mult(&q, a, b);
	quat_to_axis_angle(&q, q);
	return q;
}

void quatToRotationMatrix(GFC_Matrix3 out, GFC_Vector4D q) {
	double xx = q.x * q.x;
	double yy = q.y * q.y;
	double zz = q.z * q.z;
	out[0][0] = 1.0 - 2.0 * yy - 2.0 * zz;
	out[0][1] = 2.0 * q.x * q.y - 2.0 * q.w * q.z;
	out[0][2] = 2.0 * q.x * q.z + 2.0 * q.w * q.y;
	out[1][0] = 2.0 * q.x * q.y + 2.0 * q.w * q.z;
	out[1][1] = 1.0 - 2.0 * xx - 2.0 * zz;
	out[1][2] = 2.0 * q.y * q.z - 2.0 * q.w * q.x;
	out[2][0] = 2.0 * q.x * q.z - 2.0 * q.w * q.y;
	out[2][1] = 2.0 * q.y * q.z + 2.0 * q.w * q.x;
	out[2][2] = 1.0 - 2.0 * xx - 2.0 * yy;
}

GFC_Vector3D triangleCenter(GFC_Triangle3D triangle) {
	GFC_Vector3D triangleCenter;
	gfc_vector3d_add(triangleCenter, triangle.a, triangle.b);
	gfc_vector3d_add(triangleCenter, triangleCenter, triangle.c);
	gfc_vector3d_scale(triangleCenter, triangleCenter, 1.0 / 3.0);
	return triangleCenter;
}

GFC_Vector3D projectVector(GFC_Vector3D v, GFC_Vector3D normal) {
	double magnitude = gfc_vector3d_dot_product(v, normal);
	GFC_Vector3D projected;
	gfc_vector3d_scale(projected, normal, magnitude);
	return projected;
}

GFC_Vector3D projectVectorOntoPlane(GFC_Vector3D v, GFC_Vector3D normal) {
	GFC_Vector3D p = projectVector(v, normal);
	GFC_Vector3D r;
	gfc_vector3d_sub(r, v, p);
	return r;
}

// https://gamedev.stackexchange.com/a/23745
// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
GFC_Vector3D toBarycentric(GFC_Vector3D p, GFC_Triangle3D triangle) {
	GFC_Vector3D v0;
	gfc_vector3d_sub(v0, triangle.b, triangle.a);
	GFC_Vector3D v1;
	gfc_vector3d_sub(v1, triangle.c, triangle.a);
	GFC_Vector3D v2;
	gfc_vector3d_sub(v2, p, triangle.a);
	double d00 = gfc_vector3d_dot_product(v0, v0);
	double d01 = gfc_vector3d_dot_product(v0, v1);
	double d11 = gfc_vector3d_dot_product(v1, v1);
	double d20 = gfc_vector3d_dot_product(v2, v0);
	double d21 = gfc_vector3d_dot_product(v2, v1);
	double denom = d00 * d11 - d01 * d01;
	GFC_Vector3D out;
	out.y = (d11 * d20 - d01 * d21) / denom;
	out.z = (d00 * d21 - d01 * d20) / denom;
	out.x = 1.0f - out.y - out.z;
	return out;
}

// https://en.wikipedia.org/wiki/Barycentric_coordinate_system#Conversion_between_barycentric_and_Cartesian_coordinates
GFC_Vector3D fromBarycentric(GFC_Vector3D bary, GFC_Triangle3D triangle) {
	GFC_Vector3D a, b, c;
	gfc_vector3d_scale(a, triangle.a, bary.x);
	gfc_vector3d_scale(b, triangle.b, bary.y);
	gfc_vector3d_scale(c, triangle.c, bary.z);
	GFC_Vector3D out;
	gfc_vector3d_add(out, a, b);
	gfc_vector3d_add(out, out, c);
	return out;
}

GFC_Vector3D vector3DLerp(GFC_Vector3D a, GFC_Vector3D b, double t) {
	gfc_vector3d_scale(a, a, t);
	gfc_vector3d_scale(b, b, (1.0 - t));
	GFC_Vector3D out;
	gfc_vector3d_add(out, a, b);
	return out;
}

GFC_Vector3D perpendicularVector3(GFC_Vector3D v) {
	GFC_Vector3D c;
	gfc_vector3d_cross_product(&c, v, gfc_vector3d(1, 0, 0));
	if(gfc_vector3d_magnitude_squared(c) <= 0.01) { gfc_vector3d_cross_product(&c, v, gfc_vector3d(0, -1, 0)); }
	gfc_vector3d_normalize(&c);
	return c;
}

GFC_Vector3D kindaPerpendicularVector3(GFC_Vector3D v) {
	GFC_Vector3D p = vector3DLerp(perpendicularVector3(v), v, 0.5);
	gfc_vector3d_normalize(&p);
	return p;
}

void transposeMat3(GFC_Matrix3 mat) {
	double swap = mat[1][0];
	mat[1][0] = mat[0][1];
	mat[0][1] = swap;
	swap = mat[2][0];
	mat[2][0] = mat[0][2];
	mat[0][2] = swap;
	swap = mat[2][1];
	mat[2][1] = mat[1][2];
	mat[1][2] = swap;
}

void scaleMat3(GFC_Matrix3 mat, GFC_Vector3D scale) {
	mat[0][0] *= scale.x;
	mat[0][1] *= scale.x;
	mat[0][2] *= scale.x;

	mat[1][0] *= scale.y;
	mat[1][1] *= scale.y;
	mat[1][2] *= scale.y;

	mat[2][0] *= scale.z;
	mat[2][1] *= scale.z;
	mat[2][2] *= scale.z;
}

// https://stackoverflow.com/a/11741520
GFC_Vector4D get_rotation_between(GFC_Vector3D u, GFC_Vector3D v) {
	// It is important that the inputs are of equal length when
	// calculating the half-way vector.
	gfc_vector3d_normalize(&u);
	gfc_vector3d_normalize(&v);

	// Unfortunately, we have to check for when u == -v, as u + v
	// in this case will be (0, 0, 0), which cannot be normalized.
	if(gfc_vector3d_dot_product(u, v) == -1) {
		// 180 degree rotation around any orthogonal vector
		GFC_Vector4D quat;
		u = perpendicularVector3(u);
		gfc_vector3d_copy(quat, u);
		quat.w = 0;
		return quat;
	}

	GFC_Vector3D half;
	gfc_vector3d_add(half, u, v);
	gfc_vector3d_normalize(&half);
	GFC_Vector4D quat;
	quat.w = gfc_vector3d_dot_product(u, half);
	gfc_vector3d_cross_product(&half, u, half);
	gfc_vector3d_copy(quat, half);
	return quat;
}

Bool vector3_is_finite(GFC_Vector3D v) { return isfinite(v.x) && isfinite(v.y) && isfinite(v.z); }
