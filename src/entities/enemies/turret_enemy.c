#include "util.h"
#include "turret_enemy.h"
#include "enemy_common.h"
#include "moments_of_inertia.h"
#include "simple_logger.h"

void bulletPhysicsProcess(PhysicsBody *self, double delta) {
	if(self->entity.enemy.shootTimer > 0 && (self->entity.enemy.shootTimer -= delta) <= 0) {
		physicsFreeBody(self);
	}
}

extern PhysicsBody *g_player;
void turretEnemyPhysicsProcess(PhysicsBody *self, double delta) {
	if(!g_player) return;
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
			}
			GFC_Vector3D impulse;
			gfc_vector3d_scale(impulse, normal, 1);
			applyImpulse(other, impulse, otherPos);
		}
	}
	if(self->entity.enemy.shootTimer > 0 && (self->entity.enemy.shootTimer -= delta) <= 0) {
		GFC_Vector3D shootOrigin = self->position;
		shootOrigin.z += 6;
		self->entity.enemy.shootTimer = SHOOT_INTERVAL;
		RayCollision rayCol = castRay(gfc_edge3d_from_vectors(shootOrigin, g_player->position), self);
		if(rayCol.body == g_player) {
			PhysicsBody *bullet = physicsCreateBody();
			if(!bullet) return;
			bullet->motionType = DYNAMIC;
			bullet->position = shootOrigin;
			bullet->shape.shapeType = SPHERE;
			bullet->shape.shape.sphere.radius = 4;
			bullet->model = gf3d_model_load("assets/models/test_sphere/test_sphere.model");
			gfc_matrix4f_scale(bullet->visualTransform, bullet->visualTransform, gfc_vector3df(bullet->shape.shape.sphere.radius, bullet->shape.shape.sphere.radius, bullet->shape.shape.sphere.radius));
			bullet->mass = 0.5;
			calculateInertiaForBody(bullet);
			GFC_Vector3D v;
			gfc_vector3d_sub(v, g_player->position, self->position);
			gfc_vector3d_normalize(&v);
			gfc_vector3d_scale(bullet->linearVelocity, v, 100);
			bullet->entity.enemy.shootTimer = 5;
			bullet->physicsProcess = bulletPhysicsProcess;
			bullet->colorMod = gfc_color(0.3, 0.3, 0.3, 1.0);
		}
	}
}

PhysicsBody *createTurretEnemy(GFC_Vector3D position) {
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
	enemy->physicsProcess = turretEnemyPhysicsProcess;
	enemy->entity.enemy.shootTimer = SHOOT_INTERVAL;
	return enemy;
}
