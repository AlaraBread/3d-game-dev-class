#include "simple_logger.h"
#include "powerup.h"
#include "util.h"
#include "car.h"
#include "player.h"

void powerupFrameProcess(PhysicsBody *self, double delta) {
	self->rotation = compose_euler_vectors(self->rotation, gfc_vector3d(0, 0, delta));
	GFC_Vector4D q;
	euler_vector_to_quat(&q, self->rotation);
	self->entity.powerup.time += delta;
	if(self->entity.powerup.respawnTimer < 0.0) {
		gfc_matrix4f_make_translation(self->visualTransform, gfc_vector3df(0, 0, 2*sin(self->entity.powerup.time)));
		gfc_matrix4f_scale(self->visualTransform, self->visualTransform, gfc_vector3df(2, 2, 2));
	} else {
		gfc_matrix4f_zero(self->visualTransform);
	}
}

void pickupPowerup(PhysicsBody *self, PhysicsBody *player) {
	PhysicsBody *newPlayer = createCarPlayer();
	newPlayer->position = player->position;
	newPlayer->rotation = gfc_vector3d(0, 0, M_PI+player->entity.player.yaw);
	newPlayer->linearVelocity = player->linearVelocity;
	newPlayer->entity.player.pitch = player->entity.player.pitch;
	newPlayer->entity.player.yaw = player->entity.player.yaw;
	newPlayer->entity.player.powerupTimer = 10;
	physicsFreeBody(player);
	self->entity.powerup.respawnTimer = 15;
}

void powerupPhysicsProcess(PhysicsBody *self, double delta) {
	self->entity.powerup.respawnTimer -= delta;
	if(self->entity.powerup.respawnTimer > 0.0) return;
	for(int i = 0; i < self->numReportedCollisions; i++) {
		Collision *col = &self->reportedCollisions[i];
		if(!col->hit) break;
		PhysicsBody *other = self == col->a ? col->b : col->a;
		if(other->entityType == PLAYER) {
			pickupPowerup(self, other);
			break;
		}
	}
}

PhysicsBody *createPowerup(GFC_Vector3D position) {
	PhysicsBody *powerup = physicsCreateBody();
	powerup->shape.shapeType = SPHERE;
	powerup->shape.shape.sphere.radius = 4;
	powerup->entityType = POWERUP;
	powerup->motionType = TRIGGER;
	powerup->frameProcess = powerupFrameProcess;
	powerup->physicsProcess = powerupPhysicsProcess;
	powerup->model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	powerup->position = position;
	return powerup;
}
