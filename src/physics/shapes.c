#include "gf3d_draw.h"
#include "gf3d_obj_load.h"
#include "simple_logger.h"

#include "shapes.h"
#include "util.h"

#include "boss.h"

GFC_Vector3D sphereSupport(Shape *shape, GFC_Vector3D direction) {
	GFC_Vector3D support;
	gfc_vector3d_scale(support, direction, -shape->shape.sphere.radius);
	return support;
}

GFC_Vector3D boxSupport(Shape *shape, GFC_Vector3D direction) {
	GFC_Vector3D extents = shape->shape.box.extents;
	return gfc_vector3d(-extents.x * SIGN(direction.x), -extents.y * SIGN(direction.y), -extents.z * SIGN(direction.z));
}

GFC_Vector3D convexHullSupport(Shape *shape, GFC_Vector3D direction) {
	ObjData *obj = shape->shape.convexHull.mesh;
	int vertexCount = obj->vertex_count;
	GFC_Vector3DF *verts = obj->vertices;
	double closestDist = INFINITY;
	GFC_Vector3D closestVert = {0};
	for(int i = 0; i < vertexCount; i++) {
		GFC_Vector3D vert = gfc_vector3df_to_double(verts[i]);
		double dist = gfc_vector3d_dot_product(vert, direction);
		if(closestDist > dist) {
			closestDist = dist;
			closestVert = vert;
		}
	}
	return closestVert;
}

GFC_Vector3D circleSupport(double radius, GFC_Vector3D direction) {
	direction.z = 0;
	gfc_vector3d_normalize(&direction);
	gfc_vector3d_scale(direction, direction, -radius);
	return direction;
}

// hardcoding cue support function
GFC_Vector3D cueSupport(Shape *shape, GFC_Vector3D direction) {
	GFC_Vector3D s;
	if(direction.z > 0) {
		s = circleSupport(1.39256 * shape->shape.cueScale, direction);
		s.z -= 109.326 * shape->shape.cueScale;
		return s;
	} else {
		s = circleSupport(2.56803 * shape->shape.cueScale, direction);
		s.z += 45.3185 * shape->shape.cueScale;
		return s;
	}
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
		case BOX:
			support = boxSupport(shape, direction);
			break;
		case CONVEX_HULL:
			support = convexHullSupport(shape, direction);
			break;
		case CUE:
			support = cueSupport(shape, direction);
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
