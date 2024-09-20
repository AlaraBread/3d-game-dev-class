#include "shapes.h"

GFC_Vector3D sphereSupport(Shape *shape, GFC_Vector3D direction) {
	GFC_Vector3D support;
	gfc_vector3d_scale(support, direction, shape->shape.sphere.radius);
	return support;
}

GFC_Vector3D convexHullSupport(Shape *shape, GFC_Vector3D direction) {
	//todo
	return gfc_vector3d(1, 0, 0);
}

GFC_Vector3D support(Shape *shape, GFC_Vector3D direction) {
	switch(shape->shapeType) {
		case SPHERE:
			return sphereSupport(shape, direction);
		break;
		case CONVEX_HULL:
			return convexHullSupport(shape, direction);
		break;
	}
}

GFC_Vector3D minkowskiPoint(Shape *a, Shape *b, GFC_Vector3D direction) {
	GFC_Vector3D aSupport = support(a, direction);
	gfc_vector3d_negate(direction, direction);
	GFC_Vector3D bSupport = support(b, direction);
	GFC_Vector3D minkowskiPoint;
	gfc_vector3d_sub(minkowskiPoint, aSupport, bSupport);
	return minkowskiPoint;
}
