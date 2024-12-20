#include "moments_of_inertia.h"

// https://en.wikipedia.org/wiki/List_of_moments_of_inertia
GFC_Vector3D sphereInertia(Sphere sphere, double mass) {
	double i = (2.0 / 5.0) * mass * sphere.radius * sphere.radius;
	return gfc_vector3d(i, i, i);
}

// https://en.wikipedia.org/wiki/List_of_moments_of_inertia
GFC_Vector3D boxInertia(Box box, double mass) {
	GFC_Vector3D e = box.extents;
	gfc_vector3d_scale(e, e, 2);
	GFC_Vector3D e2 = gfc_vector3d_multiply(e, e);
	return gfc_vector3d(
		(1.0 / 12.0) * mass * (e2.y + e2.z), (1.0 / 12.0) * mass * (e2.x + e2.z), (1.0 / 12.0) * mass * (e2.x + e2.y)
	);
}

GFC_Vector3D getInertiaForBody(PhysicsBody *body) {
	switch(body->shape.shapeType) {
		case SPHERE:
			return sphereInertia(body->shape.shape.sphere, body->mass);
		case BOX:
			return boxInertia(body->shape.shape.box, body->mass);
		default:
			return gfc_vector3d(body->mass, body->mass, body->mass);
	}
}

void calculateInertiaForBody(PhysicsBody *body) {
	switch(body->shape.shapeType) {
		case SPHERE:
			body->boundingRadius = body->shape.shape.sphere.radius;
			break;
		case BOX:
			body->boundingRadius =
				MAX(MAX(body->shape.shape.box.extents.x * 2, body->shape.shape.box.extents.y * 2),
					body->shape.shape.box.extents.z * 2);
			break;
		default:
			break;
	}
	if(body->motionType == STATIC) {
		body->inertia = gfc_vector3d(1000000000000, 1000000000000, 1000000000000);
		body->mass = 1000000000000;
		return;
	}
	body->inertia = getInertiaForBody(body);
}
