#include "player.h"
#include "collision.h"
#include "gf2d_mouse.h"
#include "gf3d_camera.h"
#include "gf3d_draw.h"
#include "gfc_input.h"
#include "moments_of_inertia.h"
#include "physics.h"
#include "simple_logger.h"
#include "util.h"

#define PLAYER_RADIUS 4

#define JUMP_BUFFER 0.25
#define COYOTE_TIME 0.25
#define AIR_CONTROL 1
#define ANGULAR_SPEED 40
#define JUMP_IMPULSE 0.5

void jump(PhysicsBody *self, Collision cols[MAX_REPORTED_COLLISIONS]);

void playerPhysicsProcess(PhysicsBody *self, double delta) {
	// jump
	self->entity.player.jumpBufferTimer -= delta;
	self->entity.player.jumpBufferTimer = MAX(self->entity.player.jumpBufferTimer, 0);
	self->entity.player.coyoteTimer -= delta;
	self->entity.player.coyoteTimer = MAX(self->entity.player.coyoteTimer, 0);
	self->entity.player.timeSinceJump += delta;
	if(gfc_input_command_pressed("jump")) {
		if(self->reportedCollisions[0].hit) {
			jump(self, self->reportedCollisions);
			self->entity.player.jumpBufferTimer = 0;
			self->entity.player.coyoteTimer = 0;
		} else {
			// pressed jump without touching surface
			if(self->entity.player.coyoteCollisions[0].hit && self->entity.player.coyoteTimer > 0.01) {
				// coyote jump
				jump(self, self->entity.player.coyoteCollisions);
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
			jump(self, self->reportedCollisions);
			self->entity.player.jumpBufferTimer = 0;
			self->entity.player.coyoteTimer = 0;
		} else {
			// save collision for coyote time
			memcpy(
				self->entity.player.coyoteCollisions, self->reportedCollisions,
				sizeof(Collision) * MAX_REPORTED_COLLISIONS
			);
			self->entity.player.coyoteTimer = COYOTE_TIME;
		}
	}
	// ball movement
	double speed = delta * ANGULAR_SPEED * self->entity.player.speedMult;
	GFC_Vector3D forward = gfc_vector3d(-speed, 0, 0);
	GFC_Vector3D left = gfc_vector3d(0, -speed, 0);
	gfc_vector3d_rotate_about_z(&forward, self->entity.player.yaw);
	gfc_vector3d_rotate_about_z(&left, self->entity.player.yaw);
	double airControlAmount = AIR_CONTROL * self->entity.player.speedMult;
	if(self->entity.player.coyoteTimer > 0.01) airControlAmount *= 0.5;
	GFC_Vector3D airControl;
	if(gfc_input_command_held("forward")) {
		gfc_vector3d_add(self->angularVelocity, self->angularVelocity, left);
		gfc_vector3d_scale(airControl, forward, airControlAmount);
		gfc_vector3d_add(self->linearVelocity, self->linearVelocity, airControl);
	}
	if(gfc_input_command_held("back")) {
		gfc_vector3d_sub(self->angularVelocity, self->angularVelocity, left);
		gfc_vector3d_scale(airControl, forward, -airControlAmount);
		gfc_vector3d_add(self->linearVelocity, self->linearVelocity, airControl);
	}
	if(gfc_input_command_held("left")) {
		gfc_vector3d_sub(self->angularVelocity, self->angularVelocity, forward);
		gfc_vector3d_scale(airControl, left, airControlAmount);
		gfc_vector3d_add(self->linearVelocity, self->linearVelocity, airControl);
	}
	if(gfc_input_command_held("right")) {
		gfc_vector3d_add(self->angularVelocity, self->angularVelocity, forward);
		gfc_vector3d_scale(airControl, left, -airControlAmount);
		gfc_vector3d_add(self->linearVelocity, self->linearVelocity, airControl);
	}
	if(!self->entity.player.done && self->entity.player.powerupTimer > 0.0 &&
	   (self->entity.player.powerupTimer -= delta) <= 0.0) {
		self->entity.player.jumpMult = self->entity.player.speedMult = 1.0;
		setTimeScale(1.0);
		self->model = self->entity.player.sphereModel;
		self->shape.shapeType = SPHERE;
		self->entity.player.isCubed = false;
		calculateInertiaForBody(self);
		self->shape.shape.sphere.radius = PLAYER_RADIUS;
		calculateInertiaForBody(self);
		gfc_matrix4f_identity(self->visualTransform);
		gfc_matrix4f_scale(
			self->visualTransform, self->visualTransform, gfc_vector3df(PLAYER_RADIUS, PLAYER_RADIUS, PLAYER_RADIUS)
		);
	}
}

void jump(PhysicsBody *self, Collision cols[MAX_REPORTED_COLLISIONS]) {
	int numCollisions = 0;
	while(numCollisions < MAX_REPORTED_COLLISIONS && cols[numCollisions].hit)
		numCollisions++;
	double jumpAmount = self->entity.player.jumpMult * JUMP_IMPULSE / numCollisions;
	self->entity.player.timeSinceJump = 0;
	for(int i = 0; i < numCollisions; i++) {
		Collision *col = &cols[i];
		PhysicsBody *other = col->a == self ? col->b : col->a;
		if(other->motionType == TRIGGER) continue;
		GFC_Vector3D jump;
		gfc_vector3d_scale(jump, col->normal, jumpAmount);
		applyImpulse(col->a, jump, col->aPosition);
		gfc_vector3d_negate(jump, jump);
		applyImpulse(col->b, jump, col->bPosition);
	}
}

#define CAMERA_DIST_MULT 12

void playerFrameProcess(PhysicsBody *self, double delta) {
	// camera movement
	if(!self->entity.player.done) {
		GFC_Vector2D mouseMotion = gfc_input_get_mouse_motion();
		self->entity.player.pitch -= mouseMotion.y * 0.01;
		self->entity.player.yaw -= mouseMotion.x * 0.01;
		self->entity.player.pitch = SDL_clamp(self->entity.player.pitch, -M_PI / 2.0 + 0.01, M_PI / 2.0 - 0.01);
		self->entity.player.yaw = wrapMinMax(self->entity.player.yaw, -M_PI, M_PI);
	}
	// position camera
	double cameraDist = self->shape.shape.sphere.radius * CAMERA_DIST_MULT;
	if(self->entity.player.isCar) { cameraDist = PLAYER_RADIUS * CAMERA_DIST_MULT; }
	GFC_Vector3D cameraPos = gfc_vector3d(cameraDist, 0, 0);
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
	GFC_Vector3D down = self->position;
	down.z -= 5000;
	ray = gfc_edge3d_from_vectors(self->position, down);
	rayCol = castRay(ray, self);
	if(rayCol.hit) gf3d_model_add_shadow(gfc_vector3d_to_float(rayCol.position), 4, self);
}

void freePlayer(PhysicsBody *self) {
	if(self->entity.player.boxModel) gf3d_model_free(self->entity.player.boxModel);
	if(self->entity.player.sphereModel) gf3d_model_free(self->entity.player.sphereModel);
	self->model = NULL; // make sure we dont free twice
}

PhysicsBody *g_player = NULL;
PhysicsBody *createPlayer() {
	Shape s;
	s.shapeType = SPHERE;
	s.shape.sphere.radius = PLAYER_RADIUS;
	PhysicsBody *player = physicsCreateBody();
	player->shape = s;
	Model *sphereModel = gf3d_model_load("assets/models/test_sphere/test_sphere.model");
	player->entity.player.sphereModel = sphereModel;
	player->entity.player.boxModel = gf3d_model_load("assets/models/test_cube/test_cube.model");
	player->model = sphereModel;
	gfc_matrix4f_scale(
		player->visualTransform, player->visualTransform,
		gfc_vector3df(s.shape.sphere.radius, s.shape.sphere.radius, s.shape.sphere.radius)
	);
	player->mass = 0.01;
	player->bounce = 0.5;
	player->friction = 1.0;
	player->motionType = DYNAMIC;
	calculateInertiaForBody(player);
	player->physicsProcess = playerPhysicsProcess;
	player->frameProcess = playerFrameProcess;
	player->entityType = PLAYER;
	player->entity.player.jumpMult = player->entity.player.speedMult = 1.0;
	g_player = player;
	return player;
}
