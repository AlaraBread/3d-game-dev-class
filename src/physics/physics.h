#ifndef __ALARA_PHYSICS__
#define __ALARA_PHYSICS__

#include "gf3d_model.h"
#include "gfc_matrix.h"
#include "gfc_vector.h"
#include "collision.h"

typedef enum ShapeType_e {
	SPHERE,
	BOX,
	CONVEX_HULL,
} ShapeType;

typedef struct Sphere_s {
	float radius;
} Sphere;

typedef struct Box_s {
	GFC_Vector3D extents;
} Box;

typedef struct ConvexHull_s {
	ObjData *mesh;
} ConvexHull;

typedef struct Shape_s {
	ShapeType shapeType;
	union {
		Sphere sphere;
		Box box;
		ConvexHull convexHull;
	} shape;
} Shape;

typedef enum {
	DYNAMIC,
	STATIC,
} MotionType;

#define MAX_REPORTED_COLLISIONS 3

typedef enum {
	NONE,
	PLAYER,
	PLATFORM,
} EntityType;

struct PhysicsBody_s {
	Bool inuse;
	MotionType motionType;
	float mass;
	float bounce;
	float friction;
	GFC_Vector3D inertia;
	GFC_Matrix3 invInertiaTensor;
	GFC_Vector3D linearVelocity;
	GFC_Vector3D angularVelocity;
	GFC_Vector3D position;
	GFC_Vector3D rotation;
	GFC_Vector3D centerOfMass;
	GFC_Vector3D visualScale;
	Model *model;
	Shape shape;
	int numReportedCollisions;
	Collision reportedCollisions[MAX_REPORTED_COLLISIONS];
	void (*physicsProcess)(PhysicsBody *, float);
	void (*frameProcess)(PhysicsBody *, float);
	EntityType entityType;
	union {
		struct {
			float yaw, pitch;
			float jumpBufferTimer, coyoteTimer;
			Collision coyoteCollisions[MAX_REPORTED_COLLISIONS];
		} player;
		struct {
			GFC_Vector3D movementStart, movementEnd;
			float movementSpeed, direction, moveRatio;
		} platform;
	} entity;
};

void physicsStart(int maxPhysicsBodies);
void physicsEnd();
PhysicsBody *physicsCreateBody();
void physicsFrame(float delta);
void drawPhysicsObjects();
GFC_Vector3D physicsBodyLocalToGlobal(PhysicsBody *body, GFC_Vector3D local);
GFC_Vector3D physicsBodyGlobalToLocal(PhysicsBody *body, GFC_Vector3D global);
void applyImpulse(PhysicsBody *body, GFC_Vector3D impulse, GFC_Vector3D point);

#endif
