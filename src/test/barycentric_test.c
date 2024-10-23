#include "simple_logger.h"
#include "util.h"

void barycentric_test() {
	GFC_Triangle3D triangle = gfc_triangle(gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 1), gfc_vector3d(0, 1, 0));
	GFC_Vector3D v = gfc_vector3d(0, 0.5, 0.5);
	GFC_Vector3D bary = toBarycentric(v, triangle);
	slog("bary: %f %f %f", bary.x, bary.y, bary.z);
	GFC_Vector3D back =
		fromBarycentric(bary, gfc_triangle(gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 1), gfc_vector3d(0, 0.5, 0)));
	slog("back: %f %f %f", back.x, back.y, back.z);
}
