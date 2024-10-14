#include "simple_logger.h"
#include "util.h"

void rotation_test() {
	GFC_Vector3D a = gfc_vector3d(0, 1, 0);
	GFC_Vector3D b = gfc_vector3d(1, 1, 0);
	GFC_Vector4D q = get_rotation_between(a, b);
	quat_to_axis_angle(&q, q);
	slog("%f %f %f %f", q.x, q.y, q.z, q.w*GFC_RADTODEG);
}
