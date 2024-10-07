#include "util.h"
#include "player.h"
#include "gf3d_camera.h"
#include "moments_of_inertia.h"

void playerThink(PhysicsBody *self, float delta) {
	// camera movement
	// position camera
	GFC_Vector3D cameraPos = gfc_vector3d(10, 0, 0);
	gfc_vector3d_rotate_about_z(&cameraPos, self->pitch);
	gfc_vector3d_rotate_about_y(&cameraPos, self->yaw);
	gfc_vector3d_add(cameraPos, self->position, cameraPos);
	gf3d_camera_look_at(self->position, &cameraPos);
	// ball movement
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	float speed = delta*10.0;
	if(keys[SDL_SCANCODE_W]) self->angularVelocity.x += speed;
	if(keys[SDL_SCANCODE_S]) self->angularVelocity.x -= speed;
	if(keys[SDL_SCANCODE_A]) self->angularVelocity.y += speed;
	if(keys[SDL_SCANCODE_D]) self->angularVelocity.y -= speed;
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
	player->mass = 0.1;
	calculateInertiaForBody(player);
	player->think = playerThink;
	SDL_SetRelativeMouseMode(true);
	return player;
}
