#include "util.h"

// https://stackoverflow.com/a/29871193

float wrapMax(float x, float max) {
	return fmod(max + fmod(x, max), max);
}

float wrapMinMax(float x, float min, float max) {
	return min + wrapMax(x - min, max - min);
}

// using this resource as a reference for quaternion math
// https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html

void inverse_quat(GFC_Vector4D *quat) {
	quat->x = -quat->x;
	quat->y = -quat->y;
	quat->z = -quat->z;
}

void quat_mult(GFC_Vector4D *dst, GFC_Vector4D a, GFC_Vector4D b) {
	dst->w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
	dst->x = a.w*b.x + a.x*b.w - a.y*b.z + a.z*b.y;
	dst->y = a.w*b.y + a.x*b.z + a.y*b.w - a.z*b.x;
	dst->z = a.w*b.z - a.x*b.y + a.y*b.x + a.z*b.w;
}

void axis_angle_to_quat(GFC_Vector4D *dst, GFC_Vector4D rotation) {
	dst->w = cosf(rotation.w/2.0);
	float s = sinf(rotation.w/2.0);
	dst->x = rotation.x*s;
	dst->y = rotation.y*s;
	dst->z = rotation.z*s;
}

void quat_to_axis_angle(GFC_Vector4D *dst, GFC_Vector4D quat) {
	if(fabsf(quat.w) == 1.0) {
		*dst = gfc_vector4d(1, 0, 0, 0);
		return;
	}
	float angle = 2.0*acosf(quat.w);
	float s = sin(angle/2.0);
	dst->x = quat.x/s;
	dst->y = quat.y/s;
	dst->z = quat.z/s;
	dst->w = angle;
}

// NOT an euler angle, euler vector: https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation
void euler_vector_to_axis_angle(GFC_Vector4D *dst, GFC_Vector3D eulerVector) {
	float l = gfc_vector3d_magnitude(eulerVector);
	if(l == 0.0) {
		*dst = gfc_vector4d(1, 0, 0, 0);
		return;
	}
	gfc_vector3d_scale(eulerVector, eulerVector, (1.0/l));
	dst->w = l;
	dst->x = eulerVector.x;
	dst->y = eulerVector.y;
	dst->z = eulerVector.z;
}

void axis_angle_to_euler_vector(GFC_Vector3D *dst, GFC_Vector4D axisAngle) {
	dst->x = axisAngle.x*axisAngle.w;
	dst->y = axisAngle.y*axisAngle.w;
	dst->z = axisAngle.z*axisAngle.w;
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

void wrap_axis_angle(GFC_Vector4D *axisAngle) {
	axisAngle->w = wrapMinMax(axisAngle->w, -M_PI*2, M_PI*2);
}

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

GFC_Vector3D triangleCenter(GFC_Triangle3D triangle) {
	GFC_Vector3D triangleCenter;
	gfc_vector3d_add(triangleCenter, triangle.a, triangle.b);
	gfc_vector3d_add(triangleCenter, triangleCenter, triangle.c);
	gfc_vector3d_scale(triangleCenter, triangleCenter, 1.0/3.0);
	return triangleCenter;
}

GFC_Vector3D projectVector(GFC_Vector3D v, GFC_Vector3D normal) {
	float magnitude = gfc_vector3d_dot_product(v, normal);
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
	float d00 = gfc_vector3d_dot_product(v0, v0);
	float d01 = gfc_vector3d_dot_product(v0, v1);
	float d11 = gfc_vector3d_dot_product(v1, v1);
	float d20 = gfc_vector3d_dot_product(v2, v0);
	float d21 = gfc_vector3d_dot_product(v2, v1);
	float denom = d00 * d11 - d01 * d01;
	GFC_Vector3D out;
	out.y = (d11 * d20 - d01 * d21) / denom;
	out.z = (d00 * d21 - d01 * d20) / denom;
	out.x = 1.0f - out.y - out.z;
	return out;
}

GFC_Vector3D fromBarycentric(GFC_Vector3D bary, GFC_Triangle3D triangle) {
	GFC_Vector3D ab = vector3DLerp(triangle.a, triangle.b, bary.y/bary.x);
	return vector3DLerp(ab, triangle.c, bary.z/(bary.x+bary.y));
}

GFC_Vector3D vector3DLerp(GFC_Vector3D a, GFC_Vector3D b, float t) {
	gfc_vector3d_scale(a, a, t);
	gfc_vector3d_scale(b, b, (1.0-t));
	GFC_Vector3D out;
	gfc_vector3d_add(out, a, b);
	return out;
}

GFC_Vector3D perpendicularVector3(GFC_Vector3D v) {
	GFC_Vector3D c;
	gfc_vector3d_cross_product(&c, v, gfc_vector3d(1, 0, 0));
	if(gfc_vector3d_magnitude_squared(c) <= 0.01) {
		gfc_vector3d_cross_product(&c, v, gfc_vector3d(0, -1, 0));
	}
	gfc_vector3d_normalize(&c);
	return c;
}

GFC_Vector3D kindaPerpendicularVector3(GFC_Vector3D v) {
	GFC_Vector3D p = vector3DLerp(perpendicularVector3(v), v, 0.5);
	gfc_vector3d_normalize(&p);
	return p;
}
