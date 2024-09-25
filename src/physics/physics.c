#include "util.h"
#include "physics.h"
#include "collision.h"

static struct {
	int maxPhysicsBodies;
	PhysicsBody *physicsBodies;
} physics = {0};

void physicsStart(int maxPhysicsBodies) {
	initCollision();
	physics.maxPhysicsBodies = maxPhysicsBodies;
	physics.physicsBodies = gfc_allocate_array(sizeof(PhysicsBody), maxPhysicsBodies);
	atexit(physicsEnd);
	atexit(freeCollision);
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
	body->inertia = gfc_vector3d(1, 1, 1);
}

void reactToCollision(Collision col, PhysicsBody *a, PhysicsBody *b);

void physicsUpdate(float delta) {
	for(int i = 0; i < physics.maxPhysicsBodies; i++) {
		PhysicsBody *body = &physics.physicsBodies[i];
		if(!body->inuse) continue;
		if(body->think) body->think(body);
		for(int j = 0; j < physics.maxPhysicsBodies; j++) {
			if(i == j) continue;
			PhysicsBody *otherBody = &physics.physicsBodies[j];
			if(!otherBody->inuse) continue;
			reactToCollision(doCollision(body, otherBody), body, otherBody);
		}
		GFC_Vector3D angularMove;
		gfc_vector3d_scale(angularMove, body->angularVelocity, delta);
		gfc_vector3d_add(body->rotation, body->rotation, angularMove);
		wrap_euler_vector(&body->rotation);
		GFC_Vector3D linearMove;
		gfc_vector3d_scale(linearMove, body->linearVelocity, delta);
		gfc_vector3d_add(body->position, body->position, linearMove);
	}
}

void drawPhysicsObjects() {
	for(int i = 0; i < physics.maxPhysicsBodies; i++) {
		PhysicsBody *body = &physics.physicsBodies[i];
		if(!body->inuse) continue;
		GFC_Vector4D quat;
		euler_vector_to_quat(&quat, body->rotation);
		GFC_Matrix4 matrix;
		gfc_matrix4_from_vectors_q(matrix, body->position, quat, gfc_vector3d(1, 1, 1));
		gf3d_model_draw(body->model, matrix, gfc_color(1, 1, 1, 1), 0);
	}
}

void reactToCollision(Collision col, PhysicsBody *a, PhysicsBody *b) {
	GFC_Vector3D resolveVector;
	gfc_vector3d_scale(resolveVector, col.normal, col.penetrationDepth);
	gfc_vector3d_add(a->position, a->position, resolveVector);
	gfc_vector3d_negate(resolveVector, resolveVector);
	gfc_vector3d_add(b->position, b->position, resolveVector);
}
