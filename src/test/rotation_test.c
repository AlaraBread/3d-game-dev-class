#include "util.h"

void rotation_test() {
	GFC_Vector3D v = gfc_vector3d(2, 0, 0);
	rotate_vector3_by_euler_vector(&v, gfc_vector3d(0, 1, 0));
	printf("rotated: %f %f %f\n", v.x, v.y, v.z);
}
