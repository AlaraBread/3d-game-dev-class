#include "intersection_tests.h"
#include "simple_logger.h"

Collision sphereSphereIntersectionTest(PhysicsBody *a, PhysicsBody *b) {
	GFC_Vector3D ab;
	gfc_vector3d_sub(ab, b->position, a->position);
	float aRadius = a->shape.shape.sphere.radius;
	float bRadius = b->shape.shape.sphere.radius;
	float radius = aRadius + bRadius;
	float distance = gfc_vector3d_magnitude(ab);
	Collision col = {0};
	if(distance > radius) { return col; }
	col.hit = true;
	gfc_vector3d_normalize(&ab);
	gfc_vector3d_scale(col.aPosition, ab, aRadius);
	gfc_vector3d_add(col.aPosition, col.aPosition, a->position);
	gfc_vector3d_negate(ab, ab);
	gfc_vector3d_scale(col.bPosition, ab, bRadius);
	gfc_vector3d_add(col.bPosition, col.bPosition, b->position);
	col.normal = ab;
	col.penetrationDepth = radius - distance;
	return col;
}
