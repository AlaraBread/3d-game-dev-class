#ifndef __ALARA_PHYSICS__
#define __ALARA_PHYSICS__

#include "collision.h"
#include "gf3d_model.h"
#include "gfc_matrix.h"
#include "gfc_vector.h"
#include "powerup.h"
#include "raycast.h"
#include "sound.h"

typedef enum ShapeType_e {
	SPHERE,
	BOX,
	CONVEX_HULL,
	CUE,
} ShapeType;

typedef struct Sphere_s {
	double radius;
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
		double cueScale;
	} shape;
} Shape;

typedef enum {
	TRIGGER,
	STATIC,
	DYNAMIC,
} MotionType;

#define MAX_REPORTED_COLLISIONS 3

typedef enum {
	NONE,
	PLAYER,
	PLATFORM,
	POWERUP,
	ENEMY,
} EntityType;

struct PhysicsBody_s {
	Bool inuse;
	MotionType motionType;
	double mass;
	double bounce;
	double friction;
	GFC_Vector3D inertia;
	GFC_Matrix3 invInertiaTensor;
	GFC_Vector3D linearVelocity;
	GFC_Vector3D angularVelocity;
	GFC_Vector3D position;
	GFC_Vector3D rotation;
	GFC_Vector3D centerOfMass;
	GFC_Matrix4F visualTransform;
	GFC_Color colorMod;
	Model *model;
	Shape shape;
	double boundingRadius;
	int numReportedCollisions;
	Collision reportedCollisions[MAX_REPORTED_COLLISIONS];
	void (*physicsProcess)(PhysicsBody *, double);
	void (*frameProcess)(PhysicsBody *, double);
	void (*draw)(PhysicsBody *);
	void (*free)(PhysicsBody *);
	EntityType entityType;
	union {
		struct {
			double yaw, pitch;
			Collision coyoteCollisions[MAX_REPORTED_COLLISIONS];
			double jumpBufferTimer, coyoteTimer, rightingTimer, powerupTimer, timeSinceJump;
			Bool isRighting;
			Model *wheelModel, *sphereModel, *boxModel;
			double wheelRotations[4];
			double wheelVelocities[4];
			double wheelDistances[4];
			RayCollision wheelContacts[4];
			double wheelRadius, steer;
			double speedMult, jumpMult;
			Bool isCar, isCubed;
			Bool done;
			Sound *rollSound, *hitSound;
			unsigned int rollSoundHandle;
			float prevVolume, hitTimer;
		} player;
		struct {
			GFC_Vector3D movementStart, movementEnd;
			double movementSpeed, direction, moveRatio;
			PhysicsBody *magnetCenter;
		} platform;
		struct {
			float time;
			float respawnTimer;
			PowerupType type;
		} powerup;
		struct {
			double rightingTimer, shootTimer;
			GFC_Vector3D direction;
			GFC_Vector3D movementStart, movementEnd;
			double movementSpeed, moveDirection, moveRatio, level;
			PhysicsBody *hand;
		} enemy;
	} entity;
};

void physicsStart(int maxPhysicsBodies);
void physicsEnd();
PhysicsBody *physicsCreateBody();
void physicsFreeBody(PhysicsBody *body);
void physicsClear();
void setTimeScale(double scale);
double getTimeScale();
void physicsFrame(double delta);
void drawPhysicsObjects();
void resetPhysicsBody(PhysicsBody *body);
GFC_Vector3D physicsBodyLocalToGlobal(PhysicsBody *body, GFC_Vector3D local);
GFC_Vector3D physicsBodyGlobalToLocal(PhysicsBody *body, GFC_Vector3D global);
GFC_Vector3D velocityAtPoint(PhysicsBody *body, GFC_Vector3D point);
void applyImpulse(PhysicsBody *body, GFC_Vector3D impulse, GFC_Vector3D point);
RayCollision castRay(GFC_Edge3D ray, PhysicsBody *exclude);

#endif
