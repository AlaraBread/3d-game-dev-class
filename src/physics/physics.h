#ifndef __ALARA_PHYSICS__
#define __ALARA_PHYSICS__

#include "gf3d_model.h"
#include "gfc_matrix.h"
#include "gfc_vector.h"

typedef struct PhysicsBody_s PhysicsBody;

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

struct PhysicsBody_s {
	Bool inuse;
	MotionType motionType;
	float mass;
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
	void (*think)(PhysicsBody *, float);
};

void physicsStart(int maxPhysicsBodies);
void physicsEnd();
PhysicsBody *physicsCreateBody();
void physicsUpdate(float delta);
void drawPhysicsObjects();
GFC_Vector3D physicsBodyLocalToGlobal(PhysicsBody *body, GFC_Vector3D local);
GFC_Vector3D physicsBodyGlobalToLocal(PhysicsBody *body, GFC_Vector3D global);

#endif
