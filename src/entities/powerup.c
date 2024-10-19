#include "simple_logger.h"
#include "powerup.h"
#include "util.h"
#include "car.h"
#include "player.h"
#include "moments_of_inertia.h"

void powerupFrameProcess(PhysicsBody *self, double delta) {
	self->entity.powerup.time += delta;
	GFC_Vector3D r = compose_euler_vectors(gfc_vector3d(1, 0, 0), gfc_vector3d(0, self->entity.powerup.time, 0));
	gfc_vector3d_normalize(&r);
	gfc_vector3d_scale(r, r, delta);
	self->rotation = compose_euler_vectors(self->rotation, r);
	GFC_Vector4D q;
	euler_vector_to_quat(&q, self->rotation);
	if(self->entity.powerup.respawnTimer < 0.0) {
		gfc_matrix4f_make_translation(self->visualTransform, gfc_vector3df(0, 0, 2*sin(self->entity.powerup.time)));
		gfc_matrix4f_scale(self->visualTransform, self->visualTransform, gfc_vector3df(2, 2, 2));
	} else {
		gfc_matrix4f_zero(self->visualTransform);
	}
}

Bool pickupPowerup(PhysicsBody *self, PhysicsBody *player) {
	if(player->entity.player.powerupTimer > 0.0) {
		return false;
	}
	switch(self->entity.powerup.type) {
		case CAR: {
			PhysicsBody *newPlayer = createCarPlayer();
			newPlayer->position = player->position;
			newPlayer->rotation = gfc_vector3d(0, 0, M_PI+player->entity.player.yaw);
			newPlayer->linearVelocity = player->linearVelocity;
			newPlayer->entity.player.pitch = player->entity.player.pitch;
			newPlayer->entity.player.yaw = player->entity.player.yaw;
			newPlayer->entity.player.powerupTimer = 10;
			physicsFreeBody(player);
			self->entity.powerup.respawnTimer = 15;
		} break;
		case BIG: {
			player->entity.player.powerupTimer = 10;
			player->shape.shape.sphere.radius += 10;
			calculateInertiaForBody(player);
			gfc_matrix4f_identity(player->visualTransform);
			gfc_matrix4f_scale(
				player->visualTransform,
				player->visualTransform,
				gfc_vector3df(player->shape.shape.sphere.radius, player->shape.shape.sphere.radius, player->shape.shape.sphere.radius)
			);
			self->entity.powerup.respawnTimer = 15;
		} break;
		case SUPER_SPEED: {
			player->entity.player.powerupTimer = 10;
			player->entity.player.speedMult = 2.5;
			self->entity.powerup.respawnTimer = 15;
		} break;
		case SUPER_JUMP: {
			player->entity.player.powerupTimer = 10;
			player->entity.player.jumpMult = 3.0;
			self->entity.powerup.respawnTimer = 15;
		} break;
		case EXTRA_TIME: {
			// todo
			self->entity.powerup.respawnTimer = 15;
		} break;
	}
	return true;
}

void powerupPhysicsProcess(PhysicsBody *self, double delta) {
	self->entity.powerup.respawnTimer -= delta;
	if(self->entity.powerup.respawnTimer > 0.0) return;
	for(int i = 0; i < self->numReportedCollisions; i++) {
		Collision *col = &self->reportedCollisions[i];
		if(!col->hit) break;
		PhysicsBody *other = self == col->a ? col->b : col->a;
		if(other->entityType == PLAYER && pickupPowerup(self, other)) {
			break;
		}
	}
}

PhysicsBody *createPowerup(GFC_Vector3D position, PowerupType type) {
	PhysicsBody *powerup = physicsCreateBody();
	powerup->shape.shapeType = SPHERE;
	powerup->shape.shape.sphere.radius = 4;
	powerup->entityType = POWERUP;
	powerup->motionType = TRIGGER;
	powerup->frameProcess = powerupFrameProcess;
	powerup->physicsProcess = powerupPhysicsProcess;
	powerup->model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	powerup->position = position;
	powerup->entity.powerup.type = type;
	powerup->rotation = gfc_vector3d(gfc_crandom(), gfc_crandom(), gfc_crandom());
	switch(type) {
		case CAR:
			powerup->colorMod = gfc_color(1.0, 1.0, 0.0, 1.0);
			break;
		case BIG:
			powerup->colorMod = gfc_color(0.0, 1.0, 1.0, 1.0);
			break;
		case SUPER_SPEED:
			powerup->colorMod = gfc_color(1.0, 0.0, 0.0, 1.0);
			break;
		case SUPER_JUMP:
			powerup->colorMod = gfc_color(0.0, 1.0, 0.0, 1.0);
			break;
		case EXTRA_TIME:
			powerup->colorMod = gfc_color(0.0, 0.0, 1.0, 1.0);
			break;
	}
	return powerup;
}
