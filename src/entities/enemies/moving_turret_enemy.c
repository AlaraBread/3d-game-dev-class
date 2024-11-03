#include "util.h"
#include "turret_enemy.h"
#include "moving_turret_enemy.h"
#include "enemy_common.h"
#include "moments_of_inertia.h"
#include "simple_logger.h"

extern PhysicsBody *g_player;
void movingTurretEnemyPhysicsProcess(PhysicsBody *self, double delta) {
	if(!g_player) return;
	// move
	double ratio = self->entity.enemy.moveRatio;
	ratio += self->entity.enemy.moveDirection * self->entity.enemy.movementSpeed * delta;
	if(ratio > 1.0) {
		self->entity.enemy.moveDirection = -1;
		ratio = 1.0;
	} else if(ratio < 0.0) {
		self->entity.enemy.moveDirection = 1;
		ratio = 0.0;
	}
	GFC_Vector3D newPosition =
		vector3DLerp(self->entity.enemy.movementStart, self->entity.enemy.movementEnd, ratio);
	GFC_Vector3D diff;
	gfc_vector3d_sub(diff, newPosition, self->position);
	self->position = newPosition;
	self->entity.enemy.moveRatio = ratio;
	gfc_vector3d_scale(diff, diff, 1.0 / delta);
	self->linearVelocity = diff;
	turretEnemyPhysicsProcess(self, delta);
}

PhysicsBody *createMovingTurretEnemy(GFC_Vector3D position, GFC_Vector3D movement, double speed) {
	PhysicsBody *enemy = createEnemy();
	enemy->motionType = STATIC;
	enemy->shape.shapeType = BOX;
	enemy->shape.shape.box.extents = gfc_vector3d(6, 6, 4);
	enemy->colorMod = gfc_color(1, 0, 0, 1);
	enemy->model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	enemy->position = position;
	gfc_matrix4f_scale(enemy->visualTransform, enemy->visualTransform, gfc_vector3d_to_float(enemy->shape.shape.box.extents));
	enemy->entity.enemy.direction = gfc_vector3d(1, 0, 0);
	calculateInertiaForBody(enemy);
	enemy->physicsProcess = movingTurretEnemyPhysicsProcess;
	enemy->entity.enemy.shootTimer = SHOOT_INTERVAL;
	enemy->entity.enemy.movementStart = position;
	gfc_vector3d_add(enemy->entity.enemy.movementEnd, position, movement);
	double dist = gfc_vector3d_magnitude(movement);
	enemy->entity.enemy.movementSpeed = speed / dist;
	enemy->entity.enemy.moveDirection = 1;
	return enemy;
}
