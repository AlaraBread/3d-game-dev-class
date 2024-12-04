#include "boss.h"
#include "enemy_common.h"
#include "moments_of_inertia.h"
#include "simple_logger.h"
#include "util.h"

extern PhysicsBody *g_player;

#define SHOOTING_TIME 1
#define SHOOT_DEPTH 50
#define SHOOT_CYCLE_TIME 4
#define STARTING_SCALE 2.0
void bossPhysicsProcess(PhysicsBody *self, double delta) {
	if(!g_player || !g_player->inuse) return;
	PhysicsBody *hand = self->entity.enemy.hand;
	if(!hand) return;
	GFC_Vector3D forward = gfc_vector3d(0, 0, -1);
	rotate_vector3_by_euler_vector(&forward, self->rotation);

	// shooting
	GFC_Vector3D target;
	double beforeTimer = self->entity.enemy.shootTimer;
	if((self->entity.enemy.shootTimer -= delta) <= SHOOTING_TIME) {
		if(beforeTimer > SHOOTING_TIME) self->entity.enemy.direction = forward;
		gfc_vector3d_scale(target, self->entity.enemy.direction, SHOOT_DEPTH);
		gfc_vector3d_add(target, target, self->entity.enemy.movementStart);
		if(self->entity.enemy.shootTimer <= 0) self->entity.enemy.shootTimer += SHOOT_CYCLE_TIME;
	} else {
		target = self->entity.enemy.movementStart;
	}
	GFC_Vector3D v = self->position;
	self->position = vector3DLerp(self->position, target, 1.0 - exp(-200 * delta));
	gfc_vector3d_sub(v, self->position, v);
	gfc_vector3d_scale(v, v, 1.0 / delta);
	self->linearVelocity = v;
	hand->linearVelocity = v;
	hand->position = self->position;

	// rotate
	if(self->entity.enemy.shootTimer > SHOOTING_TIME) {
		GFC_Vector3D toPlayer;
		gfc_vector3d_sub(toPlayer, g_player->position, self->position);
		gfc_vector3d_normalize(&toPlayer);
		GFC_Vector4D rotation_q = get_rotation_between(forward, toPlayer);
		rotation_q = slerp(gfc_vector4d(0, 0, 0, 1), rotation_q, 1.0 - exp(-2 * delta));
		GFC_Vector3D rotation;
		quat_to_euler_vector(&rotation, rotation_q);
		self->rotation = compose_euler_vectors(self->rotation, rotation);
		hand->rotation = self->rotation;
		gfc_vector3d_scale(rotation, rotation, 1.0 / delta);
		self->angularVelocity = rotation;
		hand->angularVelocity = rotation;
	} else {
		self->angularVelocity = gfc_vector3d(0, 0, 0);
		hand->angularVelocity = gfc_vector3d(0, 0, 0);
	}
	// hurt
	for(int i = 0; i < hand->numReportedCollisions; i++) {
		Collision *col = &hand->reportedCollisions[i];
		if(!col->hit) continue;
		PhysicsBody *other = hand == col->a ? col->b : col->a;
		GFC_Vector3D normal = col->normal;
		if(self == col->a) gfc_vector3d_negate(normal, normal);
		if(other->entityType == PLAYER) {
			GFC_Vector3D impulse;
			gfc_vector3d_scale(impulse, normal, -2);
			applyImpulse(other, impulse, col->aPosition);
			self->entity.enemy.level += 1;
		}
	}
	double targetSize = STARTING_SCALE / pow(1.5, self->entity.enemy.level);
	double size = lerp(self->shape.shape.cueScale, targetSize, 1.0 - exp(-5 * delta));
	self->shape.shape.cueScale = size;
	gfc_matrix4f_identity(self->visualTransform);
	gfc_matrix4f_scale(self->visualTransform, self->visualTransform, gfc_vector3df(size, size, size));
	hand->shape.shape.sphere.radius = 8 * size;
}

void bossFrameProcess(PhysicsBody *self, double delta) {
	GFC_Vector3D down = self->position;
	down.z -= 5000;
	GFC_Edge3D ray = gfc_edge3d_from_vectors(self->position, down);
	RayCollision rayCol = castRay(ray, self->entity.enemy.hand);
	float shadowSize = self->entity.enemy.hand->shape.shape.sphere.radius;
	if(rayCol.hit)
		gf3d_model_add_shadow(
			gfc_vector3d_to_float(self->position), gfc_vector3d_to_float(rayCol.position), shadowSize, self
		);
	else
		gf3d_model_add_shadow(gfc_vector3d_to_float(self->position), gfc_vector3d_to_float(down), shadowSize, self);
}

void bossFree(PhysicsBody *self) { physicsFreeBody(self->entity.enemy.hand); }

PhysicsBody *createBoss(GFC_Vector3D position) {
	PhysicsBody *boss = createEnemy();
	boss->position = position;
	boss->entity.enemy.movementStart = position;
	boss->entity.enemy.shootTimer = SHOOT_CYCLE_TIME;
	boss->position.y = 150;
	boss->entityType = ENEMY;
	boss->motionType = STATIC;
	boss->shape.shapeType = CUE;
	boss->shape.shape.cueScale = STARTING_SCALE;
	boss->model = gf3d_model_load("assets/models/boss/boss.model");
	gfc_matrix4f_scale(
		boss->visualTransform, boss->visualTransform,
		gfc_vector3df(boss->shape.shape.cueScale, boss->shape.shape.cueScale, boss->shape.shape.cueScale)
	);
	boss->mass = 0.1;
	boss->bounce = 0.2;
	calculateInertiaForBody(boss);
	boss->colorMod = gfc_color(1, 0.5, 0, 1);
	boss->physicsProcess = bossPhysicsProcess;
	PhysicsBody *hand = physicsCreateBody();
	hand->motionType = STATIC;
	hand->shape.shapeType = SPHERE;
	hand->shape.shape.sphere.radius = 8 * boss->shape.shape.cueScale;
	hand->position = position;
	boss->entity.enemy.hand = hand;
	boss->free = bossFree;
	boss->frameProcess = bossFrameProcess;
	return boss;
}
