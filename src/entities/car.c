#include "collision.h"
#include "gf3d_camera.h"
#include "gf3d_draw.h"
#include "gfc_input.h"
#include "moments_of_inertia.h"
#include "physics.h"
#include "player.h"
#include "simple_logger.h"
#include "util.h"

void jump(Collision cols[MAX_REPORTED_COLLISIONS]);

GFC_Vector3D wheelOffset(int i) { return gfc_vector3d(i <= 1 ? 5 : -5, i % 2 == 0 ? 3 : -3, 0); }

Bool wheelIsFront(int i) { return i <= 1; }

void carPhysicsProcess(PhysicsBody *self, double delta) {
	GFC_Vector3D invRotation;
	gfc_vector3d_negate(invRotation, self->rotation);
	GFC_Vector3D forward = gfc_vector3d(1, 0, 0);
	rotate_vector3_by_euler_vector(&forward, self->rotation);
	GFC_Vector3D up = gfc_vector3d(0, 0, 1);
	rotate_vector3_by_euler_vector(&up, self->rotation);
	GFC_Vector3D right = gfc_vector3d(0, 1, 0);
	rotate_vector3_by_euler_vector(&right, self->rotation);
	double upness = gfc_vector3d_dot_product(up, gfc_vector3d(0, 0, 1));
	self->entity.player.rightingTimer -= delta;
	if((self->reportedCollisions[0].hit && upness < 0.5) || self->entity.player.rightingTimer > 0.0) {
		if(self->entity.player.rightingTimer <= 0.0) { self->entity.player.rightingTimer = 0.5; }
		GFC_Vector3D righting;
		gfc_vector3d_cross_product(&righting, right, gfc_vector3d(0, 0, 1));
		if(gfc_vector3d_is_zero(righting)) {
			righting = forward;
		} else {
			gfc_vector3d_normalize(&righting);
		}
		GFC_Vector3D t;
		gfc_vector3d_cross_product(&t, up, gfc_vector3d(0, 0, 1));
		double rightDir = gfc_vector3d_dot_product(t, righting) > 0 ? 1 : -1;
		gfc_vector3d_scale(righting, righting, rightDir * -(upness - 1.0) * 80 * delta);
		gfc_vector3d_add(self->angularVelocity, self->angularVelocity, righting);
	}
	// angular damp
	GFC_Vector3D angularDampVector;
	gfc_vector3d_scale(angularDampVector, self->angularVelocity, delta * 2.0);
	gfc_vector3d_sub(self->angularVelocity, self->angularVelocity, angularDampVector);
	// car movement
	double steer = 0;
	GFC_Vector3D airControl = {0};
	if(gfc_input_command_held("left")) {
		steer += 0.5;
		gfc_vector3d_sub(airControl, airControl, forward);
	}
	if(gfc_input_command_held("right")) {
		steer -= 0.5;
		gfc_vector3d_add(airControl, airControl, forward);
	}
	self->entity.player.steer = steer;
	double engineForce = 0;
	if(gfc_input_command_held("forward")) {
		engineForce += 1.0;
		gfc_vector3d_add(airControl, airControl, right);
	}
	if(gfc_input_command_held("back")) {
		engineForce -= 1.0;
		gfc_vector3d_sub(airControl, airControl, right);
	}
	gfc_vector3d_scale(airControl, airControl, 20 * delta);
	Bool jump = gfc_input_command_pressed("jump");
	Bool onGround = false;
	const int suspensionDistance = 4;
	// cast wheel rays
	// fl, fr, bl, br
	for(int i = 0; i < 4; i++) {
		GFC_Vector3D start = wheelOffset(i);
		GFC_Vector3D end = start;
		end.z = -suspensionDistance;
		start = physicsBodyLocalToGlobal(self, start);
		end = physicsBodyLocalToGlobal(self, end);
		GFC_Edge3D ray = gfc_edge3d_from_vectors(start, end);
		RayCollision col = castRay(ray, self);
		self->entity.player.wheelRotations[i] += self->entity.player.wheelVelocities[i] * delta;
		if(!col.hit) {
			self->entity.player.wheelDistances[i] = suspensionDistance;
			continue;
		}
		onGround = true;
		self->entity.player.wheelDistances[i] = col.dist;
		GFC_Vector3D colVelocity = velocityAtPoint(col.body, col.position);
		GFC_Vector3D wheelVelocity = velocityAtPoint(self, col.position);
		gfc_vector3d_sub(wheelVelocity, wheelVelocity, colVelocity);
		rotate_vector3_by_euler_vector(&wheelVelocity, invRotation);
		double friction = col.body->friction;
		GFC_Vector3D force = {0};
		GFC_Vector3D leftLocal = gfc_vector3d(0, 1, 0);
		GFC_Vector3D forwardLocal = gfc_vector3d(1, 0, 0);
		if(wheelIsFront(i)) {
			// front
			rotate_vector3_by_axis_angle(&leftLocal, gfc_vector4d(0, 0, 1, steer));
			rotate_vector3_by_axis_angle(&forwardLocal, gfc_vector4d(0, 0, 1, steer));
		} else {
			// back
			force.x = engineForce;
		}
		if(col.dist < suspensionDistance) {
			// suspension force
			force.z = (suspensionDistance - col.dist) * 0.5;
			// damping
			force.z -= wheelVelocity.z * 0.005;
		}
		// slip friction
		double slip = gfc_vector3d_dot_product(leftLocal, wheelVelocity);
		force.y -= 0.05 * slip * friction;
		// rolling friction
		force.x -= gfc_vector3d_dot_product(forwardLocal, wheelVelocity) * 0.002 * friction;
		double wheelRadius = self->entity.player.wheelRadius;
		self->entity.player.wheelVelocities[i] = gfc_vector3d_dot_product(forwardLocal, wheelVelocity) / wheelRadius;
		// jump
		if(jump) { force.z += 10; }
		// apply forces
		rotate_vector3_by_euler_vector(&force, self->rotation);
		gfc_vector3d_scale(force, force, delta);
		GFC_Vector3D selfPoint;
		gfc_vector3d_sub(selfPoint, col.position, self->position);
		selfPoint = projectVectorOntoPlane(selfPoint, up);
		gfc_vector3d_add(selfPoint, selfPoint, self->position);
		applyImpulse(self, force, selfPoint);
		gfc_vector3d_negate(force, force);
		applyImpulse(col.body, force, col.position);
	}
	if(!onGround) { gfc_vector3d_add(self->angularVelocity, self->angularVelocity, airControl); }
	if(onGround && jump) { self->entity.player.timeSinceJump = 0; }
	self->entity.player.timeSinceJump += delta;
	self->entity.player.powerupTimer -= delta;
	if(!self->entity.player.done && self->entity.player.powerupTimer <= 0.0) {
		PhysicsBody *newPlayer = createPlayer();
		newPlayer->position = self->position;
		newPlayer->rotation = self->rotation;
		newPlayer->linearVelocity = self->linearVelocity;
		newPlayer->angularVelocity = self->angularVelocity;
		newPlayer->entity.player.pitch = self->entity.player.pitch;
		newPlayer->entity.player.yaw = self->entity.player.yaw;
		physicsFreeBody(self);
	}
}

void carDraw(PhysicsBody *self) {
	GFC_Vector4D quat;
	euler_vector_to_quat(&quat, self->rotation);
	GFC_Matrix4F matrix;
	gfc_matrix4f_from_vectors_q(
		matrix, gfc_vector3d_to_float(self->position), gfc_vector4d_to_float(quat), gfc_vector3df(1, 1, 1)
	);
	gfc_matrix4f_multiply(matrix, self->visualTransform, matrix);
	gf3d_model_draw(self->model, matrix, gfc_color(1, 1, 1, 1), 0, self);
	for(int i = 0; i < 4; i++) {
		GFC_Vector3D wheelPos = wheelOffset(i);
		gfc_vector3d_add(
			wheelPos, wheelPos,
			gfc_vector3d(0, 0, -(self->entity.player.wheelDistances[i] - self->entity.player.wheelRadius))
		);
		wheelPos = physicsBodyLocalToGlobal(self, wheelPos);
		GFC_Matrix4F wheelMat;
		euler_vector_to_quat(&quat, self->rotation);
		GFC_Vector4D wheelRotation = gfc_vector4d(0, 0, 1, self->entity.player.steer);
		axis_angle_to_quat(&wheelRotation, wheelRotation);
		if(wheelIsFront(i)) quat_mult(&quat, wheelRotation, quat);
		wheelRotation = gfc_vector4d(0, 1, 0, self->entity.player.wheelRotations[i]);
		axis_angle_to_quat(&wheelRotation, wheelRotation);
		quat_mult(&quat, wheelRotation, quat);
		double radius = self->entity.player.wheelRadius;
		gfc_matrix4f_from_vectors_q(
			wheelMat, gfc_vector3d_to_float(wheelPos), gfc_vector4d_to_float(quat), gfc_vector3df(radius, 1, radius)
		);
		gf3d_model_draw(self->entity.player.wheelModel, wheelMat, gfc_color(1, 1, 1, 1), 0, self);
	}
}

void carFree(PhysicsBody *self) { gf3d_model_free(self->entity.player.wheelModel); }

extern PhysicsBody *g_player;
PhysicsBody *createCarPlayer() {
	Shape s;
	s.shapeType = BOX;
	s.shape.box.extents = gfc_vector3d(6, 3, 2);
	PhysicsBody *player = physicsCreateBody();
	player->shape = s;
	Model *cubeModel = gf3d_model_load("assets/models/test_cube/test_cube.model");
	player->entity.player.wheelModel = gf3d_model_load("assets/models/test_cylinder/test_cylinder.model");
	player->entity.player.wheelRadius = 2;
	player->model = cubeModel;
	gfc_matrix4f_scale(player->visualTransform, player->visualTransform, gfc_vector3d_to_float(s.shape.box.extents));
	player->draw = carDraw;
	player->position = gfc_vector3d(0, 0, 10);
	player->mass = 0.01;
	player->bounce = 0.5;
	player->friction = 1.0;
	player->motionType = DYNAMIC;
	calculateInertiaForBody(player);
	player->physicsProcess = carPhysicsProcess;
	player->frameProcess = playerFrameProcess;
	player->free = carFree;
	player->entityType = PLAYER;
	player->entity.player.isCar = true;
	g_player = player;
	return player;
}
