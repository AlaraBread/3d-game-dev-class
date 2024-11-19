#include "physics.h"
#include "collision.h"
#include "gf3d_draw.h"
#include "raycast.h"
#include "simple_logger.h"
#include "util.h"

static struct {
	int maxPhysicsBodies;
	PhysicsBody *physicsBodies;
	double timeScale;
} physics = {0};

void physicsStart(int maxPhysicsBodies) {
	physics.maxPhysicsBodies = maxPhysicsBodies;
	physics.physicsBodies = gfc_allocate_array(sizeof(PhysicsBody), maxPhysicsBodies);
	physics.timeScale = 1.0;
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
	slog("not enough room for physics bodies");
	return NULL;
}

void physicsFreeBody(PhysicsBody *body) {
	if(!body->inuse) return;
	if(body->model) gf3d_model_free(body->model);
	if(body->free) body->free(body);
	body->inuse = false;
}

extern int g_numEnemies;
extern int g_maxEnemies;
void physicsClear() {
	for(int i = 0; i < physics.maxPhysicsBodies; i++)
		physicsFreeBody(&physics.physicsBodies[i]);
	g_numEnemies = 0;
	g_maxEnemies = 0;
}

static void physicsBodyInitialize(PhysicsBody *body) {
	memset(body, 0, sizeof(PhysicsBody));
	body->inuse = true;
	body->mass = 1.0;
	body->inertia = gfc_vector3d(1, 1, 1);
	gfc_matrix4f_identity(body->visualTransform);
	body->linearVelocity = gfc_vector3d(0.1, 0.1, 0.1);
	body->friction = 1.0;
	body->bounce = 1.0;
	body->colorMod = gfc_color(1, 1, 1, 1);
}

void setTimeScale(double scale) { physics.timeScale = scale; }

double getTimeScale() { return physics.timeScale; }

void physicsUpdate(double delta);

#define FIXED_TIMESTEP (1.0 / 120.0)
#define MAX_TIMESTEPS_PER_FRAME 5
void physicsFrame(double delta) {
	static double physicsDelta = 0;
	physicsDelta += delta;
	int i = 0;
	while(physicsDelta > FIXED_TIMESTEP) {
		physicsUpdate(FIXED_TIMESTEP * physics.timeScale);
		physicsDelta -= FIXED_TIMESTEP;
		i++;
		if(i > MAX_TIMESTEPS_PER_FRAME) {
			slog("frame taking too long");
			physicsDelta = 0;
			break;
		}
	}
	for(int i = 0; i < physics.maxPhysicsBodies; i++) {
		PhysicsBody *body = &physics.physicsBodies[i];
		if(!body->inuse) continue;
		if(body->frameProcess) body->frameProcess(body, delta);
	}
}

void reactToCollision(Collision col, PhysicsBody *a, PhysicsBody *b);

void physicsBodyUpdateInertiaTensor(PhysicsBody *body);

void physicsUpdate(double delta) {
	for(int i = 0; i < physics.maxPhysicsBodies; i++) {
		PhysicsBody *body = &physics.physicsBodies[i];
		if(!body->inuse) continue;
		body->numReportedCollisions = 0;
		memset(body->reportedCollisions, 0, sizeof(Collision) * MAX_REPORTED_COLLISIONS);
	}
	for(int i = 0; i < physics.maxPhysicsBodies; i++) {
		PhysicsBody *body = &physics.physicsBodies[i];
		if(!body->inuse) continue;
		for(int j = 0; j < physics.maxPhysicsBodies; j++) {
			if(i <= j) continue;
			PhysicsBody *otherBody = &physics.physicsBodies[j];
			if(!otherBody->inuse) continue;
			Collision col = doCollision(body, otherBody);
			if(!col.hit) continue;
			col.a = body;
			col.b = otherBody;
			reactToCollision(col, body, otherBody);
			if(body->numReportedCollisions < MAX_REPORTED_COLLISIONS) {
				body->reportedCollisions[body->numReportedCollisions] = col;
				body->numReportedCollisions++;
			}
			if(otherBody->numReportedCollisions < MAX_REPORTED_COLLISIONS) {
				otherBody->reportedCollisions[otherBody->numReportedCollisions] = col;
				otherBody->numReportedCollisions++;
			}
		}
		if(body->motionType == DYNAMIC) {
			// gravity
			body->linearVelocity.z -= delta * 100;
			// linear damp
			GFC_Vector3D linearDampVector;
			gfc_vector3d_scale(linearDampVector, body->linearVelocity, delta * 0.1);
			gfc_vector3d_sub(body->linearVelocity, body->linearVelocity, linearDampVector);
			// angular damp
			GFC_Vector3D angularDampVector;
			gfc_vector3d_scale(angularDampVector, body->angularVelocity, delta * 0.4);
			gfc_vector3d_sub(body->angularVelocity, body->angularVelocity, angularDampVector);
			// euler integration
			GFC_Vector3D angularMove;
			gfc_vector3d_scale(angularMove, body->angularVelocity, delta);
			body->rotation = compose_euler_vectors(body->rotation, angularMove);
			GFC_Vector3D linearMove;
			gfc_vector3d_scale(linearMove, body->linearVelocity, delta);
			gfc_vector3d_add(body->position, body->position, linearMove);
		}
		wrap_euler_vector(&body->rotation);
		// world boundary
		if((body->motionType == DYNAMIC && body->position.z < -200) || !vector3_is_finite(body->position) ||
		   !vector3_is_finite(body->rotation)) {
			resetPhysicsBody(body);
		}
		physicsBodyUpdateInertiaTensor(body);
	}
	for(int i = 0; i < physics.maxPhysicsBodies; i++) {
		PhysicsBody *body = &physics.physicsBodies[i];
		if(!body->inuse) continue;
		if(body->physicsProcess) body->physicsProcess(body, delta);
	}
}

void resetPhysicsBody(PhysicsBody *body) {
	body->position = gfc_vector3d(gfc_crandom(), gfc_crandom(), gfc_crandom());
	body->linearVelocity = gfc_vector3d(0, 0, 0);
	body->angularVelocity = gfc_vector3d(0, 0, 0);
	body->rotation = gfc_vector3d(0, 0, 0);
}

void drawPhysicsObjects() {
	for(int i = 0; i < physics.maxPhysicsBodies; i++) {
		PhysicsBody *body = &physics.physicsBodies[i];
		if(!body->inuse) continue;
		if(body->draw) {
			body->draw(body);
		} else if(body->model) {
			GFC_Vector4D quat;
			euler_vector_to_quat(&quat, body->rotation);
			GFC_Matrix4F matrix;
			gfc_matrix4f_from_vectors_q(
				matrix, gfc_vector3d_to_float(body->position), gfc_vector4d_to_float(quat), gfc_vector3df(1, 1, 1)
			);
			gfc_matrix4f_multiply(matrix, body->visualTransform, matrix);
			gf3d_model_draw(body->model, matrix, body->colorMod, 0, body);
		}
	}
}

void physicsBodyUpdateInertiaTensor(PhysicsBody *body) {
	// https://github.com/godotengine/godot/blob/master/modules/godot_physics_3d/godot_body_3d.cpp#L47
	GFC_Vector4D rotation;
	euler_vector_to_quat(&rotation, body->rotation);

	GFC_Matrix3 principalAxes;
	quatToRotationMatrix(principalAxes, rotation);
	GFC_Matrix3 principalAxesTransposed;
	gfc_matrix3_copy(principalAxesTransposed, principalAxes);
	transposeMat3(principalAxesTransposed);

	GFC_Vector3D invInertia;
	invInertia.x = 1.0 / body->inertia.x;
	invInertia.y = 1.0 / body->inertia.y;
	invInertia.z = 1.0 / body->inertia.z;

	GFC_Matrix3 diag;
	gfc_matrix3_identity(diag);
	scaleMat3(diag, invInertia);
	gfc_matrix3_multiply(body->invInertiaTensor, principalAxes, diag);
	gfc_matrix3_multiply(body->invInertiaTensor, body->invInertiaTensor, principalAxesTransposed);
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
	GFC_Vector3D angularComponent;
	gfc_vector3d_cross_product(&angularComponent, body->angularVelocity, diff);
	GFC_Vector3D v;
	gfc_vector3d_add(v, body->linearVelocity, angularComponent);
	return v;
}

// adapted from the 2d version that i wrote for 2d game programming
// https://github.com/AlaraBread/2d-game-dev-class/blob/main/src/entities/physics.c#L402
void reactToCollision(Collision col, PhysicsBody *a, PhysicsBody *b) {
	if(a->motionType == TRIGGER || b->motionType == TRIGGER) { return; }
	// react to collision
	GFC_Vector3D ra;
	GFC_Vector3D aCom = physicsBodyLocalToGlobal(a, a->centerOfMass);
	gfc_vector3d_sub(ra, col.aPosition, aCom);
	GFC_Vector3D aInertia;
	gfc_vector3d_cross_product(&aInertia, ra, col.normal);
	gfc_matrix3_v_multiply(&aInertia, aInertia, a->invInertiaTensor);
	GFC_Vector3D rb;
	GFC_Vector3D bCom = physicsBodyLocalToGlobal(b, b->centerOfMass);
	gfc_vector3d_sub(rb, col.bPosition, bCom);
	GFC_Vector3D bInertia;
	gfc_vector3d_cross_product(&bInertia, rb, col.normal);
	gfc_matrix3_v_multiply(&bInertia, bInertia, b->invInertiaTensor);

	gfc_vector3d_cross_product(&aInertia, aInertia, ra);
	gfc_vector3d_cross_product(&bInertia, bInertia, rb);
	double normalMass = 1.0 / (1.0 / a->mass + 1.0 / b->mass + gfc_vector3d_dot_product(col.normal, aInertia) +
							   gfc_vector3d_dot_product(col.normal, bInertia));
	// normal impulse
	double bounce = SDL_clamp(a->bounce * b->bounce, 0.0, 1.0);
	GFC_Vector3D av = velocityAtPoint(a, col.aPosition);
	GFC_Vector3D bv = velocityAtPoint(b, col.bPosition);
	GFC_Vector3D dv;
	gfc_vector3d_sub(dv, bv, av);
	GFC_Vector3D normalImpulse = projectVector(dv, col.normal);
	gfc_vector3d_scale(normalImpulse, normalImpulse, (bounce + 1.0) * normalMass);
	double normalImpulseMagnitude = gfc_vector3d_magnitude(normalImpulse);
	// tangential impulse
	// https://github.com/godotengine/godot/blob/master/modules/godot_physics_3d/godot_body_pair_3d.cpp#L566
	double friction = SDL_clamp(a->friction * b->friction, 0.0, 1.0);
	GFC_Vector3D tv = projectVectorOntoPlane(dv, col.normal);
	double tvl = gfc_vector3d_magnitude(tv);
	gfc_vector3d_scale(tv, tv, 1.0 / tvl);
	GFC_Vector3D raxtv;
	gfc_vector3d_cross_product(&raxtv, ra, tv);
	GFC_Vector3D rbxtv;
	gfc_vector3d_cross_product(&rbxtv, rb, tv);
	GFC_Vector3D temp1;
	gfc_matrix3_v_multiply(&temp1, raxtv, a->invInertiaTensor);
	GFC_Vector3D temp2;
	gfc_matrix3_v_multiply(&temp2, rbxtv, b->invInertiaTensor);
	gfc_vector3d_cross_product(&temp1, temp1, ra);
	gfc_vector3d_cross_product(&temp2, temp2, rb);
	GFC_Vector3D temp3;
	gfc_vector3d_add(temp3, temp1, temp2);
	double t = tvl / (1.0 / a->mass + 1.0 / b->mass + gfc_vector3d_dot_product(tv, temp3));
	t = SDL_clamp(t, -friction * normalImpulseMagnitude, friction * normalImpulseMagnitude);
	GFC_Vector3D tangentImpulse;
	gfc_vector3d_scale(tangentImpulse, tv, t);
	// total impulse
	GFC_Vector3D impulse;
	gfc_vector3d_add(impulse, normalImpulse, tangentImpulse);
	if(VEC3ISNAN(impulse)) { return; }
	if(a->motionType == DYNAMIC) { applyImpulse(a, impulse, col.aPosition); }
	gfc_vector3d_negate(impulse, impulse);
	if(b->motionType == DYNAMIC) { applyImpulse(b, impulse, col.bPosition); }
	// resolve intersection
	GFC_Vector3D resolveVector;
	double depth = col.penetrationDepth;
	if(a->motionType != DYNAMIC || b->motionType != DYNAMIC) { depth *= 2.0; }
	gfc_vector3d_scale(resolveVector, col.normal, depth);
	if(a->motionType == DYNAMIC) { gfc_vector3d_add(a->position, a->position, resolveVector); }
	gfc_vector3d_negate(resolveVector, resolveVector);
	if(b->motionType == DYNAMIC) { gfc_vector3d_add(b->position, b->position, resolveVector); }
}

void applyImpulse(PhysicsBody *body, GFC_Vector3D impulse, GFC_Vector3D point) {
	if(body->motionType != DYNAMIC) { return; }
	GFC_Vector3D linearImpulse;
	gfc_vector3d_scale(linearImpulse, impulse, 1.0 / body->mass);
	gfc_vector3d_add(body->linearVelocity, body->linearVelocity, linearImpulse);

	GFC_Vector3D com = physicsBodyLocalToGlobal(body, body->centerOfMass);
	gfc_vector3d_sub(point, point, com);
	GFC_Vector3D angularImpulse;
	gfc_vector3d_cross_product(&angularImpulse, point, impulse);
	gfc_matrix3_v_multiply(&angularImpulse, angularImpulse, body->invInertiaTensor);
	gfc_vector3d_add(body->angularVelocity, body->angularVelocity, angularImpulse);
}

RayCollision castRay(GFC_Edge3D ray, PhysicsBody *exclude) {
	double maxDist = gfc_vector3d_magnitude_between(ray.a, ray.b);
	double closest = INFINITY;
	RayCollision closestCol = {0};
	for(int i = 0; i < physics.maxPhysicsBodies; i++) {
		PhysicsBody *body = &physics.physicsBodies[i];
		if(!body->inuse || exclude == body) continue;
		RayCollision col = rayTest(ray, body);
		col.body = body;
		if(!col.hit || col.dist > maxDist) continue;
		if(col.dist < closest) {
			closest = col.dist;
			closestCol = col;
		}
	}
	return closestCol;
}
