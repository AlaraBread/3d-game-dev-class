#include "simple_logger.h"
#include "util.h"
#include "collision.h"
#include "physics.h"
#include "player.h"
#include "gf3d_camera.h"
#include "moments_of_inertia.h"
#include "gfc_input.h"
#include "gf3d_draw.h"

#define JUMP_BUFFER 0.25
#define COYOTE_TIME 0.25
#define AIR_CONTROL 2
#define ANGULAR_SPEED 20
#define JUMP_IMPULSE 0.5

void jump(Collision cols[MAX_REPORTED_COLLISIONS]);

void playerPhysicsProcess(PhysicsBody *self, double delta) {
	// ball movement
	double speed = delta*ANGULAR_SPEED;
	GFC_Vector3D forward = gfc_vector3d(-speed, 0, 0);
	GFC_Vector3D left = gfc_vector3d(0, -speed, 0);
	gfc_vector3d_rotate_about_z(&forward, self->entity.player.yaw);
	gfc_vector3d_rotate_about_z(&left, self->entity.player.yaw);
	GFC_Vector3D airControl;
	if(gfc_input_command_held("forward")) {
		gfc_vector3d_add(self->angularVelocity, self->angularVelocity, left);
		gfc_vector3d_scale(airControl, forward, AIR_CONTROL);
		gfc_vector3d_add(self->linearVelocity, self->linearVelocity, airControl);
	}
	if(gfc_input_command_held("back")) {
		gfc_vector3d_sub(self->angularVelocity, self->angularVelocity, left);
		gfc_vector3d_scale(airControl, forward, -AIR_CONTROL);
		gfc_vector3d_add(self->linearVelocity, self->linearVelocity, airControl);
	}
	if(gfc_input_command_held("left")) {
		gfc_vector3d_sub(self->angularVelocity, self->angularVelocity, forward);
		gfc_vector3d_scale(airControl, left, AIR_CONTROL);
		gfc_vector3d_add(self->linearVelocity, self->linearVelocity, airControl);
	}
	if(gfc_input_command_held("right")) {
		gfc_vector3d_add(self->angularVelocity, self->angularVelocity, forward);
		gfc_vector3d_scale(airControl, left, -AIR_CONTROL);
		gfc_vector3d_add(self->linearVelocity, self->linearVelocity, airControl);
	}
	self->entity.player.jumpBufferTimer -= delta;
	self->entity.player.jumpBufferTimer = MAX(self->entity.player.jumpBufferTimer, 0);
	self->entity.player.coyoteTimer -= delta;
	self->entity.player.coyoteTimer = MAX(self->entity.player.coyoteTimer, 0);
	if(gfc_input_command_pressed("jump")) {
		if(self->reportedCollisions[0].hit) {
			jump(self->reportedCollisions);
			self->entity.player.jumpBufferTimer = 0;
			self->entity.player.coyoteTimer = 0;
		} else {
			// pressed jump without touching surface
			if(self->entity.player.coyoteCollisions[0].hit && self->entity.player.coyoteTimer > 0.01) {
				// coyote jump
				jump(self->entity.player.coyoteCollisions);
				self->entity.player.jumpBufferTimer = 0;
				self->entity.player.coyoteTimer = 0;
				self->entity.player.coyoteCollisions[0].hit = false;
			} else {
				// buffer jump
				self->entity.player.jumpBufferTimer = JUMP_BUFFER;
			}
		}
	} else if(self->reportedCollisions[0].hit) {
		// didnt press jump, but touching surface
		if(self->entity.player.jumpBufferTimer > 0.01) {
			// we pressed jump recently, so jump
			jump(self->reportedCollisions);
			self->entity.player.jumpBufferTimer = 0;
			self->entity.player.coyoteTimer = 0;
		} else {
			// save collision for coyote time
			memcpy(self->entity.player.coyoteCollisions, self->reportedCollisions, sizeof(Collision)*MAX_REPORTED_COLLISIONS);
			self->entity.player.coyoteTimer = COYOTE_TIME;
		}
	}
}

void jump(Collision cols[MAX_REPORTED_COLLISIONS]) {
	int numCollisions = 0;
	while(numCollisions < MAX_REPORTED_COLLISIONS && cols[numCollisions].hit) numCollisions++;
	double jumpAmount = JUMP_IMPULSE/numCollisions;
	for(int i = 0; i < numCollisions; i++) {
		Collision *col = &cols[i];
		GFC_Vector3D jump;
		gfc_vector3d_scale(jump, col->normal, jumpAmount);
		applyImpulse(col->a, jump, col->aPosition);
		gfc_vector3d_negate(jump, jump);
		applyImpulse(col->b, jump, col->bPosition);
	}
}

void playerFrameProcess(PhysicsBody *self, double delta) {
	// camera movement
	GFC_Vector2D mouseMotion = gfc_input_get_mouse_motion();
	self->entity.player.pitch -= mouseMotion.y*0.01;
	self->entity.player.yaw -= mouseMotion.x*0.01;
	self->entity.player.pitch = SDL_clamp(self->entity.player.pitch, -M_PI/2.0+0.01, M_PI/2.0-0.01);
	// position camera
	GFC_Vector3D cameraPos = gfc_vector3d(30, 0, 0);
	gfc_vector3d_rotate_about_y(&cameraPos, self->entity.player.pitch);
	gfc_vector3d_rotate_about_z(&cameraPos, self->entity.player.yaw);
	gfc_vector3d_add(cameraPos, self->position, cameraPos);
	GFC_Edge3D ray = gfc_edge3d_from_vectors(self->position, cameraPos);
	RayCollision rayCol = castRay(ray, self);
	if(rayCol.hit) {
		GFC_Vector3D dir;
		gfc_vector3d_sub(dir, ray.b, ray.a);
		gfc_vector3d_normalize(&dir);
		gfc_vector3d_scale(dir, dir, -0.25);
		gfc_vector3d_add(rayCol.position, rayCol.position, dir);
		cameraPos = rayCol.position;
	}
	gf3d_camera_look_at(self->position, &cameraPos);
}

PhysicsBody *createPlayer() {
	Shape s;
	s.shapeType = SPHERE;
	s.shape.sphere.radius = 4.0;
	PhysicsBody *player = physicsCreateBody();
	player->shape = s;
	Model *sphereModel = gf3d_model_load("assets/models/test_sphere/test_sphere.model");
	player->model = sphereModel;
	player->visualScale = gfc_vector3d(s.shape.sphere.radius, s.shape.sphere.radius, s.shape.sphere.radius);
	player->position = gfc_vector3d(0, 0, 10);
	player->mass = 0.01;
	player->bounce = 0.5;
	player->friction = 1.0;
	calculateInertiaForBody(player);
	player->physicsProcess = playerPhysicsProcess;
	player->frameProcess = playerFrameProcess;
	SDL_SetRelativeMouseMode(true);
	player->entityType = PLAYER;
	return player;
}
