#include "magnet.h"
#include "moments_of_inertia.h"

#define MAX_MAGNET 3

void magnetPhysicsProcess(PhysicsBody *self, double delta) {
	for(int i = 0; i < self->numReportedCollisions; i++) {
		Collision *col = &self->reportedCollisions[i];
		PhysicsBody *other = col->a == self ? col->b : col->a;
		if(other->motionType == DYNAMIC) {
			if(other->entityType == PLAYER && other->entity.player.timeSinceJump < 0.25) { continue; }
			GFC_Vector3D force;
			gfc_vector3d_sub(force, self->position, other->position);
			double dist = gfc_vector3d_magnitude(force);
			gfc_vector3d_scale(force, force, self->entity.platform.movementSpeed * (1.0 / (dist)));
			double mag = SDL_clamp(
				delta * self->entity.platform.movementSpeed * (1.0 / (dist)), -MAX_MAGNET * delta, MAX_MAGNET * delta
			);
			gfc_vector3d_scale(force, force, mag);
			gfc_vector3d_add(other->linearVelocity, other->linearVelocity, force);
		}
	}
}

void magnetFree(PhysicsBody *self) { physicsFreeBody(self->entity.platform.magnetCenter); }

PhysicsBody *createMagnet(GFC_Vector3D position, double radius, double strength) {
	PhysicsBody *center = physicsCreateBody();
	center->shape.shapeType = SPHERE;
	center->shape.shape.sphere.radius = radius / 8.0;
	center->motionType = STATIC;
	center->position = position;
	calculateInertiaForBody(center);
	PhysicsBody *magnet = physicsCreateBody();
	magnet->entity.platform.magnetCenter = center;
	magnet->position = position;
	magnet->motionType = TRIGGER;
	magnet->shape.shapeType = SPHERE;
	magnet->shape.shape.sphere.radius = radius;
	magnet->physicsProcess = magnetPhysicsProcess;
	magnet->model = gf3d_model_load("assets/models/test_sphere/test_sphere.model");
	magnet->entity.platform.movementSpeed = strength * radius;
	magnet->colorMod = gfc_color(1, 0, 0, 1);
	gfc_matrix4f_scale(magnet->visualTransform, magnet->visualTransform, gfc_vector3df(4, 4, 4));
	return magnet;
}
