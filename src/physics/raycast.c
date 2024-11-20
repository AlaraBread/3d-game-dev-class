#include "raycast.h"
#include "gf3d_obj_load.h"
#include "gfc_vector.h"
#include "physics.h"
#include "util.h"

// from "Real Time Collision Detection" by Christer Ericson
// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
// returns t value of intersection and intersection point q
RayCollision sphereRayTest(GFC_Edge3D ray, PhysicsBody *body) {
	GFC_Vector3D p = ray.a;
	GFC_Vector3D d;
	gfc_vector3d_sub(d, ray.b, ray.a);
	gfc_vector3d_normalize(&d);
	RayCollision col = {0};
	GFC_Vector3D m;
	gfc_vector3d_sub(m, p, body->position);
	double b = gfc_vector3d_dot_product(m, d);
	double c = gfc_vector3d_dot_product(m, m) - body->shape.shape.sphere.radius * body->shape.shape.sphere.radius;
	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
	if(c > 0.0f && b > 0.0f) return col;
	double discr = b * b - c;
	// A negative discriminant corresponds to ray missing sphere
	if(discr < 0.0f) return col;
	// Ray now found to intersect sphere, compute smallest t value of intersection
	double t = -b - sqrt(discr);
	// If t is negative, ray started inside sphere so clamp t to zero
	if(t < 0.0f) t = 0.0f;
	GFC_Vector3D q;
	gfc_vector3d_scale(q, d, t);
	gfc_vector3d_add(q, q, p);
	col.hit = true;
	col.dist = t;
	col.position = q;
	return col;
}

typedef struct AABB {
	GFC_Vector3D min, max;
} AABB;

// from "Real Time Collision Detection" by Christer Ericson
// typo corrected by me
// Intersect ray R(t) = p + t*d against AABB a. When intersecting,
// return intersection distance tmin and point q of intersection
RayCollision intersectRayAABB(GFC_Vector3D p, GFC_Vector3D d, AABB a) {
	RayCollision col = {0};
	double tmin = 0.0;
	double tmax = DBL_MAX;

	// For all three slabs
	for(int i = 0; i < 3; i++) {
		if(fabsf(vec3_idx(d, i)) < 0.001) {
			// Ray is parallel to slab. No hit if origin not within slab
			if(vec3_idx(p, i) < vec3_idx(a.min, i) || vec3_idx(p, i) > vec3_idx(a.max, i)) return col;
		} else {
			// Compute intersection t value of ray with near and far plane of slab
			double ood = 1.0f / vec3_idx(d, i);
			double t1 = (vec3_idx(a.min, i) - vec3_idx(p, i)) * ood;
			double t2 = (vec3_idx(a.max, i) - vec3_idx(p, i)) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if(t1 > t2) SWAP(t1, t2);
			// Compute the intersection of slab intersection intervals
			if(t1 > tmin) tmin = t1;
			if(t2 < tmax) tmax = t2;
			// Exit with no collision as soon as slab intersection becomes empty
			if(tmin > tmax) return col;
		}
	}
	// Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
	GFC_Vector3D q;
	gfc_vector3d_scale(q, d, tmin);
	gfc_vector3d_add(q, q, p);
	col.position = q;
	col.dist = tmin;
	col.hit = true;
	return col;
}

RayCollision boxRayTest(GFC_Edge3D ray, PhysicsBody *body) {
	GFC_Vector3D invRotation;
	gfc_vector3d_negate(invRotation, body->rotation);
	gfc_vector3d_sub(ray.a, ray.a, body->position);
	gfc_vector3d_sub(ray.b, ray.b, body->position);
	rotate_vector3_by_euler_vector(&ray.a, invRotation);
	rotate_vector3_by_euler_vector(&ray.b, invRotation);
	GFC_Vector3D dir;
	gfc_vector3d_sub(dir, ray.b, ray.a);
	gfc_vector3d_normalize(&dir);
	AABB aabb = {0};
	gfc_vector3d_copy(aabb.max, body->shape.shape.box.extents);
	gfc_vector3d_negate(aabb.min, aabb.max);
	RayCollision col = intersectRayAABB(ray.a, dir, aabb);
	rotate_vector3_by_euler_vector(&col.position, body->rotation);
	gfc_vector3d_add(col.position, col.position, body->position);
	return col;
}

RayCollision meshRayTest(GFC_Edge3D ray, PhysicsBody *body) {
	GFC_Vector3D startGlobal = ray.a;
	GFC_Vector3D invRotation;
	gfc_vector3d_negate(invRotation, body->rotation);
	gfc_vector3d_sub(ray.a, ray.a, body->position);
	gfc_vector3d_sub(ray.b, ray.b, body->position);
	rotate_vector3_by_euler_vector(&ray.a, invRotation);
	rotate_vector3_by_euler_vector(&ray.b, invRotation);
	GFC_Vector3D dir;
	gfc_vector3d_sub(dir, ray.b, ray.a);
	gfc_vector3d_normalize(&dir);
	RayCollision col = {0};
	GFC_Matrix4F identity;
	gfc_matrix4f_identity(identity);
	col.hit = gf3d_obj_edge_test(body->shape.shape.convexHull.mesh, identity, ray, &col.position);
	rotate_vector3_by_euler_vector(&col.position, body->rotation);
	gfc_vector3d_add(col.position, col.position, body->position);
	if(col.hit) col.dist = gfc_vector3d_magnitude_between(col.position, startGlobal);
	return col;
}

RayCollision rayTest(GFC_Edge3D ray, PhysicsBody *body) {
	RayCollision col = {0};
	if(body->motionType == TRIGGER) { return col; }
	switch(body->shape.shapeType) {
		case SPHERE:
			return sphereRayTest(ray, body);
		case BOX:
			return boxRayTest(ray, body);
		case CONVEX_HULL:
			return meshRayTest(ray, body);
	}
	return col;
}
