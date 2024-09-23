#include "util.h"
#include "physics.h"
#include "collision.h"

void physicsStart(int maxPhysicsBodies) {
	physics.maxPhysicsBodies = maxPhysicsBodies;
	physics.physicsBodies = gfc_allocate_array(sizeof(PhysicsBody), maxPhysicsBodies);
	atexit(physicsEnd);
}

void physicsEnd() {
	if(physics.physicsBodies != NULL) {
		free(physics.physicsBodies);
		physics.physicsBodies = NULL;
	}
}

static void physicsBodyInitialize(PhysicsBody *body);

PhysicsBody *physicsCreateBody() {
	static int lastAllocatedBody = 0;
	int stop = POSMOD(lastAllocatedBody-1, physics.maxPhysicsBodies);
	for(int i = lastAllocatedBody; i != stop; i = (i+1)%physics.maxPhysicsBodies) {
		PhysicsBody *body = &physics.physicsBodies[i];
		if(!body->inuse) {
			physicsBodyInitialize(body);
			return body;
		}
	}
	return NULL;
}

static void physicsBodyInitialize(PhysicsBody *body) {
	memset(body, 0, sizeof(PhysicsBody));
	body->inuse = true;
	body->mass = 1.0;
	// todo: figure out inertia tensor
}

void physicsUpdate(float delta) {
	for(int i = 0; i < physics.maxPhysicsBodies; i++) {
		PhysicsBody *body = &physics.physicsBodies[i];
		if(!body->inuse) continue;
		for(int j = 0; j < physics.maxPhysicsBodies; j++) {
			PhysicsBody *otherBody = &physics.physicsBodies[j];
			if(!otherBody->inuse) continue;
			doCollision(body, otherBody);
		}
		GFC_Vector3D angularMove;
		gfc_vector3d_scale(angularMove, body->angularVelocity, delta);
		gfc_vector3d_add(body->rotation, body->rotation, angularMove);
		wrap_euler_vector(&body->rotation);
		GFC_Vector3D linearMove;
		gfc_vector3d_scale(linearMove, body->linearVelocity, delta);
		gfc_vector3d_add(body->position, body->position, linearMove);
		GFC_Vector4D quat;
		euler_vector_to_quat(&quat, body->rotation);
		gfc_matrix4_from_vectors_q(body->model->matrix, body->position, quat, gfc_vector3d(1, 1, 1));
	}
}
