#include "util.h"
#include "player.h"
#include "gf3d_camera.h"
#include "moments_of_inertia.h"
#include "gfc_input.h"

void playerThink(PhysicsBody *self, float delta) {
	// camera movement
	GFC_Vector2D mouseMotion = gfc_input_get_mouse_motion();
	self->pitch -= mouseMotion.y*0.01;
	self->yaw -= mouseMotion.x*0.01;
	self->pitch = SDL_clamp(self->pitch, -M_PI/2.0, M_PI/2.0);
	// position camera
	GFC_Vector3D cameraPos = gfc_vector3d(30, 0, 0);
	gfc_vector3d_rotate_about_y(&cameraPos, self->pitch);
	gfc_vector3d_rotate_about_z(&cameraPos, self->yaw);
	gfc_vector3d_add(cameraPos, self->position, cameraPos);
	gf3d_camera_look_at(self->position, &cameraPos);
	// ball movement
	float speed = delta*20.0;
	GFC_Vector3D forward = gfc_vector3d(speed, 0, 0);
	GFC_Vector3D left = gfc_vector3d(0, speed, 0);
	gfc_vector3d_rotate_about_z(&forward, self->yaw);
	gfc_vector3d_rotate_about_z(&left, self->yaw);
	if(gfc_input_command_held("forward")) {
		gfc_vector3d_sub(self->angularVelocity, self->angularVelocity, left);
	}
	if(gfc_input_command_held("back")) {
		gfc_vector3d_add(self->angularVelocity, self->angularVelocity, left);
	}
	if(gfc_input_command_held("left")) {
		gfc_vector3d_add(self->angularVelocity, self->angularVelocity, forward);
	}
	if(gfc_input_command_held("right")) {
		gfc_vector3d_sub(self->angularVelocity, self->angularVelocity, forward);
	}
	if(gfc_input_command_pressed("jump")) {
		GFC_Vector3D jump = gfc_vector3d(0, 0, 100);
		gfc_vector3d_add(self->linearVelocity, self->linearVelocity, jump);
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
	calculateInertiaForBody(player);
	player->think = playerThink;
	SDL_SetRelativeMouseMode(true);
	return player;
}
