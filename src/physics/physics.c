#include "physics.h"
#include "collision.h"
#include "gf3d_draw.h"
#include "simple_logger.h"
#include "util.h"

static struct {
	int maxPhysicsBodies;
	PhysicsBody *physicsBodies;
} physics = {0};

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
	int stop = POSMOD(lastAllocatedBody - 1, physics.maxPhysicsBodies);
	for(int i = lastAllocatedBody; i != stop; i = (i + 1) % physics.maxPhysicsBodies) {
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
	body->inertia = gfc_vector3d(10, 10, 10);
	body->visualScale = gfc_vector3d(1, 1, 1);
}

void reactToCollision(Collision col, PhysicsBody *a, PhysicsBody *b);

void physicsUpdate(float delta) {
	for(int i = 0; i < physics.maxPhysicsBodies; i++) {
		PhysicsBody *body = &physics.physicsBodies[i];
		if(!body->inuse) continue;
		if(body->think) body->think(body);
		// gravity
		if(body->motionType == DYNAMIC) {
			body->linearVelocity.z -= delta*100;
		}
		for(int j = 0; j < physics.maxPhysicsBodies; j++) {
			if(i <= j) continue;
			PhysicsBody *otherBody = &physics.physicsBodies[j];
			if(!otherBody->inuse) continue;
			reactToCollision(doCollision(body, otherBody), body, otherBody);
		}
		// linear damp
		GFC_Vector3D linearDampVector;
		gfc_vector3d_scale(linearDampVector, body->linearVelocity, delta * 0.5);
		gfc_vector3d_sub(body->linearVelocity, body->linearVelocity, linearDampVector);
		// angular damp
		GFC_Vector3D angularDampVector;
		gfc_vector3d_scale(angularDampVector, body->angularVelocity, delta * 0.5);
		gfc_vector3d_sub(body->angularVelocity, body->angularVelocity, angularDampVector);
		// euler integration
		GFC_Vector3D angularMove;
		gfc_vector3d_scale(angularMove, body->angularVelocity, delta);
		body->rotation = compose_euler_vectors(body->rotation, angularMove);
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
		gfc_matrix4_from_vectors_q(matrix, body->position, quat, body->visualScale);
		gf3d_model_draw(body->model, matrix, body->think == NULL ? gfc_color(1, 1, 1, 1) : gfc_color(1, 1, 0, 1), 0);
	}
}

GFC_Vector3D physicsBodyLocalToGlobal(PhysicsBody *body, GFC_Vector3D local) {
	GFC_Vector3D global = local;
	rotate_vector3_by_euler_vector(&global, body->rotation);
	gfc_vector3d_add(global, global, body->position);
	return global;
}

GFC_Vector3D physicsBodyGlobalToLocal(PhysicsBody *body, GFC_Vector3D global) {
	GFC_Vector3D local = global;
	gfc_vector3d_sub(local, local, body->position);
	GFC_Vector3D invRotation = body->rotation;
	gfc_vector3d_negate(invRotation, invRotation);
	rotate_vector3_by_euler_vector(&global, invRotation);
	return local;
}

GFC_Vector3D velocityAtPoint(PhysicsBody *body, GFC_Vector3D point) {
	GFC_Vector3D diff;
	gfc_vector3d_sub(diff, point, body->position);
	GFC_Vector3D invRotation;
	gfc_vector3d_negate(invRotation, body->rotation);
	rotate_vector3_by_euler_vector(&diff, invRotation);
	GFC_Vector3D angularComponent;
	gfc_vector3d_cross_product(&angularComponent, body->angularVelocity, diff);
	GFC_Vector3D v;
	gfc_vector3d_add(v, body->linearVelocity, angularComponent);
	return v;
}

void applyImpulse(PhysicsBody *body, GFC_Vector3D impulse, GFC_Vector3D point);

// adapted from the 2d version that i wrote for 2d game programming
// https://github.com/AlaraBread/2d-game-dev-class/blob/main/src/entities/physics.c#L402
void reactToCollision(Collision col, PhysicsBody *a, PhysicsBody *b) {
	if(!col.hit) return;
	// react to collision
	float normalMass = a->mass + b->mass;
	float tangentMass = 1.0;
	// normal impulse
	float bounce = 0.0;
	GFC_Vector3D av = velocityAtPoint(a, col.aPosition);
	GFC_Vector3D bv = velocityAtPoint(b, col.bPosition);
	GFC_Vector3D dv;
	gfc_vector3d_sub(dv, bv, av);
	GFC_Vector3D normalImpulse = projectVector(dv, col.normal);
	gfc_vector3d_scale(normalImpulse, normalImpulse, (bounce + 1.0) * normalMass);
	float normalImpulseMagnitude = gfc_vector3d_magnitude(normalImpulse);
	// tangential impulse
	float friction = 1.0;
	GFC_Vector3D tv = projectVectorOntoPlane(dv, col.normal);
	float tvMagnitude = gfc_vector3d_magnitude(tv);
	float tangentImpulseMagnitude =
		SDL_clamp(-tvMagnitude * tangentMass, -friction * normalImpulseMagnitude, friction * normalImpulseMagnitude);
	GFC_Vector3D tangentImpulse = tv;
	gfc_vector3d_normalize(&tangentImpulse);
	gfc_vector3d_scale(tangentImpulse, tangentImpulse, tangentImpulseMagnitude);
	// total impulse
	GFC_Vector3D impulse;
	gfc_vector3d_add(impulse, normalImpulse, tangentImpulse);
	if(a->motionType == DYNAMIC) {
		applyImpulse(a, impulse, col.aPosition);
	}
	gfc_vector3d_negate(impulse, impulse);
	if(b->motionType == DYNAMIC) {
		applyImpulse(b, impulse, col.bPosition);
	}
	// resolve intersection
	GFC_Vector3D resolveVector;
	gfc_vector3d_scale(resolveVector, col.normal, col.penetrationDepth);
	if(a->motionType == DYNAMIC) {
		gfc_vector3d_add(a->position, a->position, resolveVector);
	}
	gfc_vector3d_negate(resolveVector, resolveVector);
	if(b->motionType == DYNAMIC) {
		gfc_vector3d_add(b->position, b->position, resolveVector);
	}
}

void applyImpulse(PhysicsBody *body, GFC_Vector3D impulse, GFC_Vector3D point) {
	GFC_Vector3D linearImpulse;
	gfc_vector3d_scale(linearImpulse, impulse, 1.0 / body->mass);
	gfc_vector3d_add(body->linearVelocity, body->linearVelocity, linearImpulse);

	gfc_vector3d_sub(point, point, body->position);
	GFC_Vector3D com = body->centerOfMass;
	rotate_vector3_by_euler_vector(&com, body->rotation);
	gfc_vector3d_add(point, point, com);
	// point is now in global space relative to com
	point.x = point.x / body->inertia.x;
	point.y = point.y / body->inertia.y;
	point.z = point.z / body->inertia.z;

	GFC_Vector3D angularImpulse;
	gfc_vector3d_cross_product(&angularImpulse, impulse, point);
	gfc_vector3d_add(body->angularVelocity, body->angularVelocity, angularImpulse);
}
