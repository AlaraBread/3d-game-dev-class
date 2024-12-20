#include "moments_of_inertia.h"
#include "physics.h"
#include "util.h"

void movingPlatformPhysicsProcess(PhysicsBody *self, double delta) {
	double ratio = self->entity.platform.moveRatio;
	ratio += self->entity.platform.direction * self->entity.platform.movementSpeed * delta;
	if(ratio > 1.0) {
		self->entity.platform.direction = -1;
		ratio = 1.0;
	} else if(ratio < 0.0) {
		self->entity.platform.direction = 1;
		ratio = 0.0;
	}
	GFC_Vector3D newPosition =
		vector3DLerp(self->entity.platform.movementStart, self->entity.platform.movementEnd, ratio);
	GFC_Vector3D diff;
	gfc_vector3d_sub(diff, newPosition, self->position);
	self->position = newPosition;
	self->entity.platform.moveRatio = ratio;
	gfc_vector3d_scale(diff, diff, 1.0 / delta);
	self->linearVelocity = diff;
}

PhysicsBody *createMovingPlatform(GFC_Vector3D position, GFC_Vector3D extents, GFC_Vector3D movement, double speed) {
	PhysicsBody *platform = physicsCreateBody();
	platform->position = position;
	platform->entityType = PLATFORM;
	platform->entity.platform.movementStart = position;
	gfc_vector3d_add(platform->entity.platform.movementEnd, position, movement);
	double dist = gfc_vector3d_magnitude(movement);
	platform->entity.platform.movementSpeed = speed / dist;
	platform->entity.platform.direction = 1;
	Shape s;
	s.shapeType = BOX;
	s.shape.box.extents = extents;
	platform->shape = s;
	platform->model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	gfc_matrix4f_scale(
		platform->visualTransform, platform->visualTransform, gfc_vector3d_to_float(s.shape.box.extents)
	);
	platform->physicsProcess = movingPlatformPhysicsProcess;
	platform->motionType = STATIC;
	platform->friction = 1.0;
	calculateInertiaForBody(platform);
	return platform;
}
