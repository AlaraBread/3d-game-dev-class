#include "moments_of_inertia.h"

// https://en.wikipedia.org/wiki/List_of_moments_of_inertia
GFC_Vector3D sphereInertia(Sphere sphere, float mass) {
	float i = (2.0/5.0)*mass*sphere.radius*sphere.radius;
	return gfc_vector3d(i, i, i);
}

// https://en.wikipedia.org/wiki/List_of_moments_of_inertia
GFC_Vector3D boxInertia(Box box, float mass) {
	GFC_Vector3D e2 = gfc_vector3d_multiply(box.extents, box.extents);
	return gfc_vector3d(
			(1.0/12.0)*mass*(e2.y+e2.z),
			(1.0/12.0)*mass*(e2.x+e2.z),
			(1.0/12.0)*mass*(e2.x+e2.y));
}

GFC_Vector3D calculateInertiaForBody(PhysicsBody *body) {
	switch(body->shape.shapeType) {
		case SPHERE: return sphereInertia(body->shape.shape.sphere, body->mass);
		case BOX: return boxInertia(body->shape.shape.box, body->mass);
		default: return gfc_vector3d(body->mass, body->mass, body->mass);
	}
}
