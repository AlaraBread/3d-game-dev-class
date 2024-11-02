#include "util.h"
#include "box_enemy.h"
#include "enemy_common.h"
#include "moments_of_inertia.h"
#include "simple_logger.h"

extern PhysicsBody *g_player;
void boxEnemyPhysicsProcess(PhysicsBody *self, double delta) {
	if(!g_player) return;
	GFC_Vector3D enemyToPlayer;
	gfc_vector3d_sub(enemyToPlayer, g_player->position, self->position);
	gfc_vector3d_normalize(&enemyToPlayer);
	Bool hitWall = false;
	for(int i = 0; i < self->numReportedCollisions; i++) {
		Collision *col = &self->reportedCollisions[i];
		if(!col->hit) continue;
		PhysicsBody *other = col->a == self ? col->b : col->a;
		GFC_Vector3D otherPos = col->a == self ? col->bPosition : col->aPosition;
		GFC_Vector3D normal = col->normal;
		if(col->a == self) gfc_vector3d_negate(normal, normal);
		if(other->entityType == PLAYER) {
			if(normal.z > 0.7) {
				// got goomba stomped
				physicsFreeBody(self);
			} else if(other->entity.player.powerupTimer > 0.0) {
				other->entity.player.powerupTimer = 0.01;
			} else {
				// turn player into box
				other->shape.shapeType = BOX;
				other->shape.shape.box.extents = gfc_vector3d(4, 4, 4);
				other->model = other->entity.player.boxModel;
				other->entity.player.powerupTimer = 1;
				calculateInertiaForBody(other);
			}
			GFC_Vector3D impulse;
			gfc_vector3d_scale(impulse, normal, 1);
			applyImpulse(other, impulse, otherPos);
			GFC_Vector3D offset;
			gfc_vector3d_scale(offset, normal, 2);
			gfc_vector3d_add(other->position, other->position, offset);
		} else if(fabs(normal.z) < 0.3) {
			// hit wall
			hitWall = true;
		}
	}
	if(gfc_vector3d_dot_product(self->entity.enemy.direction, enemyToPlayer) < 0 || hitWall) {
		// calculate new direction
		if(fabs(enemyToPlayer.x) > fabs(enemyToPlayer.y)) {
			self->entity.enemy.direction = gfc_vector3d(enemyToPlayer.x > 0 ? 1 : -1, 0, 0);
		} else {
			self->entity.enemy.direction = gfc_vector3d(0, enemyToPlayer.y > 0 ? 1 : -1, 0);
		}
	}
	GFC_Vector3D move;
	gfc_vector3d_scale(move, self->entity.enemy.direction, delta*100);
	gfc_vector3d_add(self->linearVelocity, self->linearVelocity, move);
	double vz = self->linearVelocity.z;
	self->linearVelocity = projectVector(self->linearVelocity, self->entity.enemy.direction);
	self->linearVelocity.z = vz;
	self->rotation = gfc_vector3d(0, 0, 0);
}

PhysicsBody *createBoxEnemy(GFC_Vector3D position) {
	PhysicsBody *enemy = createEnemy();
	enemy->motionType = DYNAMIC;
	enemy->shape.shapeType = BOX;
	enemy->shape.shape.box.extents = gfc_vector3d(4, 4, 5);
	enemy->colorMod = gfc_color(1, 0, 0, 1);
	enemy->model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	enemy->position = position;
	gfc_matrix4f_scale(enemy->visualTransform, enemy->visualTransform, gfc_vector3d_to_float(enemy->shape.shape.box.extents));
	enemy->entity.enemy.direction = gfc_vector3d(1, 0, 0);
	enemy->mass = 0.01;
	calculateInertiaForBody(enemy);
	enemy->physicsProcess = boxEnemyPhysicsProcess;
	enemy->friction = 1;
	enemy->bounce = 0;
	return enemy;
}
