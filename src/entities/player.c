#include "simple_logger.h"
#include "util.h"
#include "collision.h"
#include "physics.h"
#include "player.h"
#include "gf3d_camera.h"
#include "moments_of_inertia.h"
#include "gfc_input.h"

#define JUMP_BUFFER 0.25
#define COYOTE_TIME 0.25
#define AIR_CONTROL 2
#define ANGULAR_SPEED 20
#define JUMP_IMPULSE 0.5

void jump(Collision cols[MAX_REPORTED_COLLISIONS]);

void playerThink(PhysicsBody *self, float delta) {
	// camera movement
	GFC_Vector2D mouseMotion = gfc_input_get_mouse_motion();
	self->pitch -= mouseMotion.y*0.01;
	self->yaw -= mouseMotion.x*0.01;
	self->pitch = SDL_clamp(self->pitch, -M_PI/3.0, M_PI/3.0);
	// position camera
	GFC_Vector3D cameraPos = gfc_vector3d(30, 0, 0);
	gfc_vector3d_rotate_about_y(&cameraPos, self->pitch);
	gfc_vector3d_rotate_about_z(&cameraPos, self->yaw);
	gfc_vector3d_add(cameraPos, self->position, cameraPos);
	gf3d_camera_look_at(self->position, &cameraPos);
	// ball movement
	float speed = delta*ANGULAR_SPEED;
	GFC_Vector3D forward = gfc_vector3d(-speed, 0, 0);
	GFC_Vector3D left = gfc_vector3d(0, -speed, 0);
	gfc_vector3d_rotate_about_z(&forward, self->yaw);
	gfc_vector3d_rotate_about_z(&left, self->yaw);
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
	self->jumpBufferTimer -= delta;
	self->jumpBufferTimer = MAX(self->jumpBufferTimer, 0);
	self->coyoteTimer -= delta;
	self->coyoteTimer = MAX(self->coyoteTimer, 0);
	if(gfc_input_command_pressed("jump")) {
		if(self->reportedCollisions[0].hit) {
			jump(self->reportedCollisions);
			self->jumpBufferTimer = 0;
			self->coyoteTimer = 0;
		} else {
			// pressed jump without touching surface
			if(self->coyoteCollisions[0].hit && self->coyoteTimer > 0.01) {
				// coyote jump
				jump(self->coyoteCollisions);
				self->jumpBufferTimer = 0;
				self->coyoteTimer = 0;
				self->coyoteCollisions[0].hit = false;
			} else {
				// buffer jump
				self->jumpBufferTimer = JUMP_BUFFER;
			}
		}
	} else if(self->reportedCollisions[0].hit) {
		// didnt press jump, but touching surface
		if(self->jumpBufferTimer > 0.01) {
			// we pressed jump recently, so jump
			jump(self->reportedCollisions);
			self->jumpBufferTimer = 0;
			self->coyoteTimer = 0;
		} else {
			// save collision for coyote time
			memcpy(self->coyoteCollisions, self->reportedCollisions, sizeof(Collision)*MAX_REPORTED_COLLISIONS);
			self->coyoteTimer = COYOTE_TIME;
		}
	}
}

void jump(Collision cols[MAX_REPORTED_COLLISIONS]) {
	int numCollisions = 0;
	while(numCollisions < MAX_REPORTED_COLLISIONS && cols[numCollisions].hit) numCollisions++;
	float jumpAmount = JUMP_IMPULSE/numCollisions;
	for(int i = 0; i < numCollisions; i++) {
		Collision *col = &cols[i];
		GFC_Vector3D jump;
		gfc_vector3d_scale(jump, col->normal, jumpAmount);
		applyImpulse(col->a, jump, col->aPosition);
		gfc_vector3d_negate(jump, jump);
		applyImpulse(col->b, jump, col->bPosition);
	}
}

PhysicsBody *createPlayer() {
	Shape s;
	s.shapeType = SPHERE;
	s.shape.sphere.radius = 4.0;
	PhysicsBody *player = physicsCreateBody();
	player->shape = s;
	Model *sphereModel = gf3d_model_load("assets/models/test_sphere/test_sphere.model");
	player->model = sphereModel;
	player->position = gfc_vector3d(0, 0, 10);
	player->mass = 0.01;
	player->bounce = 0.5;
	player->friction = 1.0;
	calculateInertiaForBody(player);
	player->think = playerThink;
	SDL_SetRelativeMouseMode(true);
	return player;
}
