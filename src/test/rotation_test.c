#include "util.h"

void rotation_test() {
	GFC_Vector3D v = gfc_vector3d(2, 0, 0);
	rotate_vector3_by_euler_vector(&v, gfc_vector3d(0, M_PI/2.0, 0));
	rotate_vector3_by_euler_vector(&v, gfc_vector3d(M_PI/2.0, 0, 0));
	printf("rotated: %f %f %f\n", v.x, v.y, v.z);
	v = gfc_vector3d(2, 0, 0);
	GFC_Vector3D composed = compose_euler_vectors(
		gfc_vector3d(0, M_PI/2.0, 0),
		gfc_vector3d(M_PI/2.0, 0, 0));
	rotate_vector3_by_euler_vector(&v, composed);
	printf("rotated again: %f %f %f\n", v.x, v.y, v.z);
	printf("composed: %f %f %f\n", composed.x, composed.y, composed.z);
}
