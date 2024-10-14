#include "simple_logger.h"
#include "util.h"
#include "collision.h"
#include "physics.h"
#include "player.h"
#include "gf3d_camera.h"
#include "moments_of_inertia.h"
#include "gfc_input.h"
#include "gf3d_draw.h"

void jump(Collision cols[MAX_REPORTED_COLLISIONS]);

void carPhysicsProcess(PhysicsBody *self, float delta) {
	GFC_Vector3D invRotation;
	gfc_vector3d_negate(invRotation, self->rotation);
	// car movement
	float steer = 0;
	if(gfc_input_command_held("left")) {
		steer += 0.5;
	}
	if(gfc_input_command_held("right")) {
		steer -= 0.5;
	}
	float engineForce = 0;
	if(gfc_input_command_held("forward")) {
		engineForce += 1;
	}
	if(gfc_input_command_held("back")) {
		engineForce -= 1;
	}
	GFC_Vector3D forward = gfc_vector3d(1, 0, 0);
	rotate_vector3_by_euler_vector(&forward, self->rotation);
	GFC_Vector3D up = gfc_vector3d(0, 0, 1);
	rotate_vector3_by_euler_vector(&up, self->rotation);
	// cast wheel rays
	// fl, fr, bl, br
	for(int i = 0; i < 4; i++) {
		GFC_Vector3D start = gfc_vector3d(i <= 1 ? 6 : -6, i%2 == 0 ? 3 : -3, 0);
		GFC_Vector3D end = start;
		end.z = -4;
		start = physicsBodyLocalToGlobal(self, start);
		end = physicsBodyLocalToGlobal(self, end);
		GFC_Edge3D ray = gfc_edge3d_from_vectors(start, end);
		gf3d_draw_edge_3d(ray, gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.1, gfc_color(1,1,1,1));
		RayCollision col = castRay(ray, self);
		if(!col.hit) {
			continue;
		}
		GFC_Vector3D colVelocity = velocityAtPoint(col.body, col.position);
		GFC_Vector3D wheelVelocity = velocityAtPoint(self, col.position);
		gfc_vector3d_sub(wheelVelocity, wheelVelocity, colVelocity);
		rotate_vector3_by_euler_vector(&wheelVelocity, invRotation);
		GFC_Vector3D force = {0};
		GFC_Vector3D leftLocal = gfc_vector3d(0, 1, 0);
		if(i <= 1) {
			// front
			rotate_vector3_by_axis_angle(&leftLocal, gfc_vector4d(0, 0, 1, steer));
			gf3d_draw_sphere_solid(gfc_sphere(0,0,0,0.5), col.position, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1,1,0,1), gfc_color(1,1,1,1));
		} else {
			// back
			force.x = engineForce;
			gf3d_draw_sphere_solid(gfc_sphere(0,0,0,0.5), col.position, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1,0,1,1), gfc_color(1,1,1,1));
		}
		if(col.dist < 3) {
			// suspension force
			force.z = col.dist*0.5;
			// damping
			force.z -= wheelVelocity.z*0.05;
		}
		// friction
		float slip = gfc_vector3d_dot_product(leftLocal, wheelVelocity);
		force.y -= 0.05*slip;
		rotate_vector3_by_euler_vector(&force, self->rotation);
		gfc_vector3d_scale(force, force, delta);
		GFC_Vector3D selfPoint = start;
		projectVectorOntoPlane(selfPoint, up);
		applyImpulse(self, force, selfPoint);
		gfc_vector3d_negate(force, force);
		applyImpulse(col.body, force, col.position);
	}
}

void carFrameProcess(PhysicsBody *self, float delta) {
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

PhysicsBody *createCarPlayer() {
	Shape s;
	s.shapeType = BOX;
	s.shape.box.extents = gfc_vector3d(6, 3, 2);
	PhysicsBody *player = physicsCreateBody();
	player->shape = s;
	Model *cubeModel = gf3d_model_load("assets/models/test_cube/test_cube.model");
	player->model = cubeModel;
	player->visualScale = s.shape.box.extents;
	player->position = gfc_vector3d(0, 0, 10);
	player->rotation = gfc_vector3d(0, 0, 1);
	player->mass = 0.01;
	player->bounce = 0.5;
	player->friction = 1.0;
	calculateInertiaForBody(player);
	player->physicsProcess = carPhysicsProcess;
	player->frameProcess = carFrameProcess;
	SDL_SetRelativeMouseMode(true);
	player->entityType = PLAYER;
	return player;
}
