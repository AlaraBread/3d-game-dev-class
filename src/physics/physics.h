#ifndef __ALARA_PHYSICS__
#define __ALARA_PHYSICS__

#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gf3d_model.h"

typedef struct PhysicsBody_s PhysicsBody;

typedef enum ShapeType_e {
	SPHERE,
	CONVEX_HULL,
} ShapeType;

typedef struct Sphere_s {
	float radius;
} Sphere;

typedef struct ConvexHull_s {
	ObjData *mesh;
} ConvexHull;

typedef struct Shape_s {
	ShapeType shapeType;
	union {
		Sphere sphere;
	} shape;
} Shape;

struct PhysicsBody_s {
	Bool inuse;
	float mass;
	GFC_Vector3D inertia;
	GFC_Vector3D linearVelocity;
	GFC_Vector3D angularVelocity;
	GFC_Vector3D position;
	GFC_Vector3D rotation;
	GFC_Vector3D centerOfMass;
	Model *model;
	Shape shape;
	void (*think) (PhysicsBody *);
};

void physicsStart(int maxPhysicsBodies);
void physicsEnd();
PhysicsBody *physicsCreateBody();
void physicsUpdate(float delta);
void drawPhysicsObjects();

#endif
