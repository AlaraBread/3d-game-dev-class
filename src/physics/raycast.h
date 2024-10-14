#ifndef __ALARA_RAYCAST_H__
#define __ALARA_RAYCAST_H__

#include "gfc_primitives.h"

typedef struct PhysicsBody_s PhysicsBody;

typedef struct RayCollision_s {
	Bool hit;
	double dist;
	GFC_Vector3D position;
	PhysicsBody *body;
} RayCollision;

RayCollision rayTest(GFC_Edge3D ray, PhysicsBody *body);

#endif
