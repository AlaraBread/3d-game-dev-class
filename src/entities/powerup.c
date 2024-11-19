#include "powerup.h"
#include "car.h"
#include "moments_of_inertia.h"
#include "player.h"
#include "simple_logger.h"
#include "util.h"

void powerupFrameProcess(PhysicsBody *self, double delta) {
	self->entity.powerup.time += delta;
	GFC_Vector3D r = compose_euler_vectors(gfc_vector3d(1, 0, 0), gfc_vector3d(0, self->entity.powerup.time, 0));
	gfc_vector3d_normalize(&r);
	gfc_vector3d_scale(r, r, delta);
	self->rotation = compose_euler_vectors(self->rotation, r);
	GFC_Vector4D q;
	euler_vector_to_quat(&q, self->rotation);
	if(self->entity.powerup.respawnTimer < 0.0) {
		gfc_matrix4f_make_translation(self->visualTransform, gfc_vector3df(0, 0, 2 * sin(self->entity.powerup.time)));
		gfc_matrix4f_scale(self->visualTransform, self->visualTransform, gfc_vector3df(2, 2, 2));
	} else {
		gfc_matrix4f_zero(self->visualTransform);
	}
	if(self->entity.powerup.respawnTimer > 0.0) return;
	GFC_Vector3D down = self->position;
	down.z -= 5000;
	GFC_Edge3D ray = gfc_edge3d_from_vectors(self->position, down);
	RayCollision rayCol = castRay(ray, self);
	if(rayCol.hit) gf3d_model_add_shadow(gfc_vector3d_to_float(rayCol.position), 2, self);
}

Bool pickupPowerup(PhysicsBody *self, PhysicsBody *player) {
	if(player->entity.player.powerupTimer > 0.0) return false;
	if(player->entity.player.done) return false;
	switch(self->entity.powerup.type) {
		case CAR: {
			PhysicsBody *newPlayer = createCarPlayer();
			newPlayer->position = player->position;
			newPlayer->rotation = gfc_vector3d(0, 0, M_PI + player->entity.player.yaw);
			newPlayer->linearVelocity = player->linearVelocity;
			newPlayer->entity.player.pitch = player->entity.player.pitch;
			newPlayer->entity.player.yaw = player->entity.player.yaw;
			newPlayer->entity.player.powerupTimer = 30;
			physicsFreeBody(player);
			self->entity.powerup.respawnTimer = 15;
		} break;
		case BIG: {
			player->entity.player.powerupTimer = 10;
			player->shape.shape.sphere.radius += 10;
			calculateInertiaForBody(player);
			gfc_matrix4f_identity(player->visualTransform);
			gfc_matrix4f_scale(
				player->visualTransform, player->visualTransform,
				gfc_vector3df(
					player->shape.shape.sphere.radius, player->shape.shape.sphere.radius,
					player->shape.shape.sphere.radius
				)
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
		case SLOW: {
			setTimeScale(0.2);
			player->entity.player.speedMult = 2.5;
			player->entity.player.powerupTimer = 10 * 0.2;
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
		if(other->entityType == PLAYER && pickupPowerup(self, other)) { break; }
	}
}

PhysicsBody *createPowerup(PowerupType type) {
	PhysicsBody *powerup = physicsCreateBody();
	powerup->shape.shapeType = SPHERE;
	powerup->shape.shape.sphere.radius = 4;
	powerup->entityType = POWERUP;
	powerup->motionType = TRIGGER;
	powerup->frameProcess = powerupFrameProcess;
	powerup->physicsProcess = powerupPhysicsProcess;
	powerup->model = gf3d_model_load("assets/models/test_cube/test_cube.model");
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
		case SLOW:
			powerup->colorMod = gfc_color(0.0, 0.0, 1.0, 1.0);
			break;
	}
	return powerup;
}
