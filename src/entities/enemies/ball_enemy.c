#include "ball_enemy.h"
#include "enemy_common.h"
#include "moments_of_inertia.h"
#include "simple_logger.h"

extern PhysicsBody *g_player;

void ballEnemyPhysicsProcess(PhysicsBody *self, double delta) {
	if(!g_player || !g_player->inuse) return;
	Bool onGround = false;
	for(int i = 0; i < self->numReportedCollisions; i++) {
		Collision *col = &self->reportedCollisions[i];
		if(!col->hit) continue;
		PhysicsBody *other = self == col->a ? col->b : col->a;
		GFC_Vector3D normal = col->normal;
		if(self == col->a) { gfc_vector3d_negate(normal, normal); }
		if(other->entityType == PLAYER) {
			if(normal.z > 0.75) {
				// got goomba stomped
				other->linearVelocity.z += 100;
				physicsFreeBody(self);
			}
		} else {
			onGround = true;
		}
	}
	if(onGround) {
		GFC_Vector3D dir;
		gfc_vector3d_sub(dir, self->position, g_player->position);
		dir.z = 0;
		gfc_vector3d_normalize(&dir);
		GFC_Vector3D left = dir;
		gfc_vector3d_rotate_about_z(&left, M_PI / 2.0);
		gfc_vector3d_scale(left, left, -delta * 40);
		gfc_vector3d_add(self->angularVelocity, self->angularVelocity, left);
		GFC_Vector3D angularDampVector;
		gfc_vector3d_scale(angularDampVector, self->angularVelocity, delta * 2);
		gfc_vector3d_sub(self->angularVelocity, self->angularVelocity, angularDampVector);
	}
}

PhysicsBody *createBallEnemy(GFC_Vector3D position) {
	PhysicsBody *enemy = createEnemy();
	enemy->position = position;
	enemy->entityType = ENEMY;
	enemy->motionType = DYNAMIC;
	enemy->shape.shapeType = SPHERE;
	const double r = 6;
	enemy->shape.shape.sphere.radius = r;
	enemy->model = gf3d_model_load("assets/models/test_sphere/test_sphere.model");
	gfc_matrix4f_scale(enemy->visualTransform, enemy->visualTransform, gfc_vector3df(r, r, r));
	enemy->mass = 0.1;
	enemy->bounce = 0.2;
	calculateInertiaForBody(enemy);
	enemy->colorMod = gfc_color(1, 0, 0, 1);
	enemy->physicsProcess = ballEnemyPhysicsProcess;
	return enemy;
}
