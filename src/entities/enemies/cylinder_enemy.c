#include "simple_logger.h"
#include "util.h"
#include "ball_enemy.h"
#include "moments_of_inertia.h"

extern PhysicsBody *g_player;

void cylinderEnemyPhysicsProcess(PhysicsBody *self, double delta) {
	if(!g_player || !g_player->inuse) return;
	GFC_Vector3D dir;
	gfc_vector3d_sub(dir, g_player->position, self->position);
	gfc_vector3d_normalize(&dir);
	Bool onGround = false;
	for(int i = 0; i < self->numReportedCollisions; i++) {
		Collision *col = &self->reportedCollisions[i];
		if(!col->hit) continue;
		PhysicsBody *other = self == col->a ? col->b : col->a;
		GFC_Vector3D normal = col->normal;
		if(self == col->a) {
			gfc_vector3d_negate(normal, normal);
		}
		GFC_Vector3D invRotation;
		gfc_vector3d_negate(invRotation, self->rotation);
		rotate_vector3_by_euler_vector(&normal, invRotation);
		GFC_Vector3D up = gfc_vector3d(0, 0, 1);
		rotate_vector3_by_euler_vector(&up, self->rotation);
		if(up.z < 0.0) gfc_vector3d_negate(normal, normal);
		if(other->entityType == PLAYER) {
			if(normal.z > 0.5) {
				// got goomba stomped
				other->linearVelocity.z += 100;
				physicsFreeBody(self);
			} else {
				// hit player
				GFC_Vector3D push;
				gfc_vector3d_scale(push, dir, 100);
				gfc_vector3d_add(other->linearVelocity, other->linearVelocity, push);
			}
		} else {
			onGround = true;
		}
	}
	if(onGround) {
		GFC_Vector3D move;
		gfc_vector3d_scale(move, dir, delta*150);
		gfc_vector3d_add(self->linearVelocity, self->linearVelocity, move);
	}
	GFC_Vector3D angularDampVector;
	gfc_vector3d_scale(angularDampVector, self->angularVelocity, delta * 2);
	gfc_vector3d_sub(self->angularVelocity, self->angularVelocity, angularDampVector);
}

PhysicsBody *createCylinderEnemy(GFC_Vector3D position) {
	PhysicsBody *enemy = physicsCreateBody();
	enemy->position = position;
	enemy->entityType = ENEMY;
	enemy->motionType = DYNAMIC;
	enemy->shape.shapeType = BOX;
	const double e = 8;
	enemy->position.z += e;
	enemy->shape.shape.box.extents = gfc_vector3d(e, e, e/2.0);
	enemy->model = gf3d_model_load("assets/models/test_cylinder/test_cylinder.model");
	gfc_matrix4f_rotate_y(enemy->visualTransform, enemy->visualTransform, M_PI/2.0);
	gfc_matrix4f_scale(enemy->visualTransform, enemy->visualTransform, gfc_vector3df(e, e, e));
	enemy->mass = 0.1;
	enemy->bounce = 0.2;
	enemy->friction = 0;
	calculateInertiaForBody(enemy);
	enemy->colorMod = gfc_color(1, 0, 0, 1);
	enemy->physicsProcess = cylinderEnemyPhysicsProcess;
	return enemy;
}
