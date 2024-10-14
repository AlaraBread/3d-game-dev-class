#ifndef __ALARA_UTIL__
#define __ALARA_UTIL__

#include "gfc_primitives.h"
#include "gfc_vector.h"

// https://stackoverflow.com/a/3982430
#define SWAP(x,y) do \
   { unsigned char swap_temp[sizeof(x) == sizeof(y) ? (signed)sizeof(x) : -1]; \
     memcpy(swap_temp,&y,sizeof(x)); \
     memcpy(&y,&x,       sizeof(x)); \
     memcpy(&x,swap_temp,sizeof(x)); \
    } while(0)

// positive modulus
#define POSMOD(a, b) (((a) % (b) + (b)) % (b))

#define SIGN(x) (x > 0 ? 1 : (x < 0 ? -1 : 0))

#define VEC3ISNAN(v) (isnan(v.x) || isnan(v.y) || isnan(v.z))

#define vec3_idx(v, i) ((i) == 0 ? (v).x : (i) == 1 ? (v).y : (v).z)

// wrap x -> [0,max)
float wrapMax(float x, float max);
// wrap x -> [min,max)
float wrapMinMax(float x, float min, float max);
void inverse_quat(GFC_Vector4D *quat);
void quat_mult(GFC_Vector4D *dst, GFC_Vector4D a, GFC_Vector4D b);
void axis_angle_to_quat(GFC_Vector4D *dst, GFC_Vector4D rotation);
void quat_to_axis_angle(GFC_Vector4D *dst, GFC_Vector4D quat);
void quat_to_euler_vector(GFC_Vector3D *dst, GFC_Vector4D quat);
// NOT an euler angle, euler vector: https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation
void euler_vector_to_axis_angle(GFC_Vector4D *dst, GFC_Vector3D eulerVector);
void axis_angle_to_euler_vector(GFC_Vector3D *dst, GFC_Vector4D axisAngle);
void euler_vector_to_quat(GFC_Vector4D *dst, GFC_Vector3D eulerVector);
void rotate_vector3_by_quat(GFC_Vector3D *vector, GFC_Vector4D quat);
void rotate_vector3_by_axis_angle(GFC_Vector3D *vector, GFC_Vector4D axisAngle);
void rotate_vector3_by_euler_vector(GFC_Vector3D *vector, GFC_Vector3D eulerVector);
void wrap_euler_vector(GFC_Vector3D *eulerVector);
void wrap_axis_angle(GFC_Vector4D *axisAngle);
GFC_Vector3D compose_euler_vectors(GFC_Vector3D a, GFC_Vector3D b);
GFC_Vector4D compose_axis_angles(GFC_Vector4D a, GFC_Vector4D b);
void quatToRotationMatrix(GFC_Matrix3 out, GFC_Vector4D q);
GFC_Vector3D triangleCenter(GFC_Triangle3D triangle);
GFC_Vector3D projectVector(GFC_Vector3D v, GFC_Vector3D normal);
GFC_Vector3D projectVectorOntoPlane(GFC_Vector3D v, GFC_Vector3D normal);
GFC_Vector3D toBarycentric(GFC_Vector3D p, GFC_Triangle3D triangle);
GFC_Vector3D fromBarycentric(GFC_Vector3D bary, GFC_Triangle3D triangle);
GFC_Vector3D vector3DLerp(GFC_Vector3D a, GFC_Vector3D b, float t);
GFC_Vector3D perpendicularVector3(GFC_Vector3D v);
GFC_Vector3D kindaPerpendicularVector3(GFC_Vector3D v);
void transposeMat3(GFC_Matrix3 mat);
void scaleMat3(GFC_Matrix3 mat, GFC_Vector3D scale);
GFC_Vector4D get_rotation_between(GFC_Vector3D u, GFC_Vector3D v);

#endif
