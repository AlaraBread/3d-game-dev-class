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
	GFC_Vector3D reverseRotation;
	gfc_vector3d_negate(reverseRotation, body->rotation);
	rotate_vector3_by_euler_vector(&direction, reverseRotation);
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
	rotate_vector3_by_euler_vector(&support, body->rotation);
	gfc_vector3d_add(support, support, body->position);
	return support;
}

SupportPoint minkowskiPoint(PhysicsBody *a, PhysicsBody *b, GFC_Vector3D direction) {
	SupportPoint s;
	s.a = support(a, direction);
	gfc_vector3d_negate(direction, direction);
	s.b = support(b, direction);
	gfc_vector3d_sub(s.m, s.b, s.a);
	return s;
}
