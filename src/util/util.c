#include "util.h"

// https://stackoverflow.com/a/29871193

float wrapMax(float x, float max) {
	return fmod(max + fmod(x, max), max);
}

float wrapMinMax(float x, float min, float max) {
	return min + wrapMax(x - min, max - min);
}

void inverse_quat(GFC_Vector4D *quat) {
	quat->x = -quat->x;
	quat->y = -quat->y;
	quat->z = -quat->z;
}

void quat_mult(GFC_Vector4D *dst, GFC_Vector4D a, GFC_Vector4D b) {
	dst->w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
	dst->x = a.w*b.x - a.x*b.w - a.y*b.z - a.z*b.y;
	dst->y = a.w*b.y - a.x*b.z - a.y*b.w - a.z*b.x;
	dst->z = a.w*b.z - a.x*b.y - a.y*b.x - a.z*b.w;
}

void axis_angle_to_quat(GFC_Vector4D *dst, GFC_Vector4D rotation) {
	dst->w = cosf(rotation.w/2.0);
	float s = sinf(rotation.w/2.0);
	dst->x = rotation.x*s;
	dst->y = rotation.y*s;
	dst->z = rotation.z*s;
}

// NOT an euler angle, euler vector: https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation
void euler_vector_to_axis_angle(GFC_Vector4D *dst, GFC_Vector3D eulerVector) {
	float l = gfc_vector3d_magnitude(eulerVector);
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

GFC_Vector3D triangleCenter(GFC_Triangle3D triangle) {
	GFC_Vector3D triangleCenter;
	gfc_vector3d_add(triangleCenter, triangle.a, triangle.b);
	gfc_vector3d_add(triangleCenter, triangleCenter, triangle.c);
	gfc_vector3d_scale(triangleCenter, triangleCenter, 1.0/3.0);
	return triangleCenter;
}
