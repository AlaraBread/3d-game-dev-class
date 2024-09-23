#ifndef __ALARA_PHYSICS__
#define __ALARA_PHYSICS__

#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gf3d_model.h"
#include "shapes.h"

typedef struct PhysicsBody_s {
	Bool inuse;
	float mass;
	GFC_Vector3D inertia;
	GFC_Vector3D linearVelocity;
	GFC_Vector3D angularVelocity;
	GFC_Vector3D position;
	GFC_Vector3D rotation;
	Model *model;
	Shape shape;
} PhysicsBody;

static struct {
	int maxPhysicsBodies;
	PhysicsBody *physicsBodies;
} physics = {0};

void physicsStart(int maxPhysicsBodies);
void physicsEnd();
PhysicsBody *physicsCreateBody();
void physicsUpdate(float delta);

#endif
