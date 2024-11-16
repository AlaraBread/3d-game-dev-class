#include "moments_of_inertia.h"
#include "physics.h"
#include "util.h"

void rotatingPlatformPhysicsProcess(PhysicsBody *self, double delta) {
	GFC_Vector3D angularMove;
	gfc_vector3d_scale(angularMove, self->angularVelocity, delta);
	GFC_Vector3D newRotation = compose_euler_vectors(self->rotation, angularMove);
	self->rotation = newRotation;
}

PhysicsBody *createRotatingPlatform(GFC_Vector3D extents, GFC_Vector3D angularVelocity) {
	PhysicsBody *platform = physicsCreateBody();
	platform->entityType = PLATFORM;
	Shape s;
	s.shapeType = BOX;
	s.shape.box.extents = extents;
	platform->shape = s;
	platform->model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	gfc_matrix4f_scale(
		platform->visualTransform, platform->visualTransform, gfc_vector3d_to_float(s.shape.box.extents)
	);
	platform->angularVelocity = angularVelocity;
	platform->physicsProcess = rotatingPlatformPhysicsProcess;
	platform->motionType = STATIC;
	platform->friction = 1.0;
	calculateInertiaForBody(platform);
	return platform;
}
