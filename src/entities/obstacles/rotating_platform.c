#include "util.h"
#include "physics.h"
#include "moments_of_inertia.h"

void rotatingPlatformPhysicsProcess(PhysicsBody *self, double delta) {
	GFC_Vector3D angularMove = gfc_vector3d(0, 0, delta);
	GFC_Vector3D newRotation = compose_euler_vectors(self->rotation, angularMove);
	self->rotation = newRotation;
	gfc_vector3d_scale(angularMove, angularMove, 1.0/delta);
	self->angularVelocity = angularMove;
}

PhysicsBody *createRotatingPlatform(GFC_Vector3D position, double speed) {
	PhysicsBody *platform = physicsCreateBody();
	platform->position = position;
	platform->entityType = PLATFORM;
	platform->entity.platform.movementSpeed = speed;
	Shape s;
	s.shapeType = BOX;
	s.shape.box.extents = gfc_vector3d(30, 30, 2);
	platform->shape = s;
	platform->model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	gfc_matrix4f_scale(
		platform->visualTransform,
		platform->visualTransform,
		gfc_vector3d_to_float(s.shape.box.extents)
	);
	platform->physicsProcess = rotatingPlatformPhysicsProcess;
	platform->motionType = STATIC;
	platform->friction = 1.0;
	calculateInertiaForBody(platform);
	return platform;
}
