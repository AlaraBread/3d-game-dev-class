#include "util.h"

void rotation_test() {
	GFC_Vector3D angularVelocity = gfc_vector3d(3.170366, 0.000000, 2.377771);
	GFC_Vector3D rotation = gfc_vector3d(0, 0, 0);
	gfc_vector3d_scale(angularVelocity, angularVelocity, (1.0/60.0));
	printf("angular velocity: %f %f %f\n", angularVelocity.x, angularVelocity.y, angularVelocity.z);
	GFC_Vector3D rotated = compose_euler_vectors(rotation, angularVelocity);
	printf("rotated: %f %f %f\n", rotated.x, rotated.y, rotated.z);
}
