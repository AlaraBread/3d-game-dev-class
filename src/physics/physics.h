#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gf3d_model.h"
#include "shapes.h"

#ifndef __ALARA_PHYSICS__
#define __ALARA_PHYSICS__

typedef struct PhysicsBody_s {
	Bool inuse;
	float mass;
	GFC_Matrix3 inertiaTensor;
	GFC_Vector3D linearVelocity;
	GFC_Vector3D angularVelocity;
	GFC_Vector3D position;
	GFC_Vector4D rotation;
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
