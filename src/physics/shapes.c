#include "shapes.h"
#include "util.h"

GFC_Vector3D sphereSupport(Shape *shape, GFC_Vector3D direction) {
	GFC_Vector3D support;
	gfc_vector3d_scale(support, direction, shape->shape.sphere.radius);
	return support;
}

GFC_Vector3D convexHullSupport(Shape *shape, GFC_Vector3D direction) {
	//todo
	return gfc_vector3d(1, 0, 0);
}

GFC_Vector3D support(PhysicsBody *body, GFC_Vector3D direction) {
	rotate_vector3_by_euler_vector(&direction, body->rotation);
	GFC_Vector3D support;
	Shape *shape = &body->shape;
	switch(shape->shapeType) {
		case SPHERE:
			support = sphereSupport(shape, direction);
		break;
		case CONVEX_HULL:
			support = convexHullSupport(shape, direction);
		break;
	}
	gfc_vector3d_add(support, support, body->position);
	return support;
}

GFC_Vector3D minkowskiPoint(PhysicsBody *a, PhysicsBody *b, GFC_Vector3D direction, GFC_Vector3D *aSupport, GFC_Vector3D *bSupport) {
	GFC_Vector3D as = support(a, direction);
	gfc_vector3d_negate(direction, direction);
	GFC_Vector3D bs = support(b, direction);
	GFC_Vector3D minkowskiPoint;
	gfc_vector3d_sub(minkowskiPoint, as, bs);
	if(aSupport) *aSupport = as;
	if(bSupport) *bSupport = bs;
	return minkowskiPoint;
}
