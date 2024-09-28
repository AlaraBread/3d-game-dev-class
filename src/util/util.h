#ifndef __ALARA_UTIL__
#define __ALARA_UTIL__

#include "gfc_vector.h"
#include "gfc_primitives.h"

// positive modulus
#define POSMOD(a, b) (((a) % (b) + (b)) % (b))

// wrap x -> [0,max)
float wrapMax(float x, float max);
// wrap x -> [min,max)
float wrapMinMax(float x, float min, float max);
void inverse_quat(GFC_Vector4D *quat);
void quat_mult(GFC_Vector4D *dst, GFC_Vector4D a, GFC_Vector4D b);
void axis_angle_to_quat(GFC_Vector4D *dst, GFC_Vector4D rotation);
// NOT an euler angle, euler vector: https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation
void euler_vector_to_axis_angle(GFC_Vector4D *dst, GFC_Vector3D eulerVector);
void axis_angle_to_euler_vector(GFC_Vector3D *dst, GFC_Vector4D axisAngle);
void euler_vector_to_quat(GFC_Vector4D *dst, GFC_Vector3D eulerVector);
void rotate_vector3_by_quat(GFC_Vector3D *vector, GFC_Vector4D quat);
void rotate_vector3_by_axis_angle(GFC_Vector3D *vector, GFC_Vector4D axisAngle);
void rotate_vector3_by_euler_vector(GFC_Vector3D *vector, GFC_Vector3D eulerVector);
void wrap_euler_vector(GFC_Vector3D *eulerVector);
void wrap_axis_angle(GFC_Vector4D *axisAngle);
GFC_Vector3D triangleCenter(GFC_Triangle3D triangle);
GFC_Vector3D projectVector(GFC_Vector3D v, GFC_Vector3D normal);
GFC_Vector3D projectVectorOntoPlane(GFC_Vector3D v, GFC_Vector3D normal);
GFC_Vector3D toBarycentric(GFC_Vector3D p, GFC_Triangle3D triangle);
GFC_Vector3D fromBarycentric(GFC_Vector3D bary, GFC_Triangle3D triangle);
GFC_Vector3D vector3DLerp(GFC_Vector3D a, GFC_Vector3D b, float t);
GFC_Vector3D perpendicularVector3(GFC_Vector3D v);
GFC_Vector3D kindaPerpendicularVector3(GFC_Vector3D v);

#endif
