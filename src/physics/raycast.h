#ifndef __ALARA_RAYCAST_H__
#define __ALARA_RAYCAST_H__

#include "gfc_primitives.h"

typedef struct RayCollision {
	Bool hit;
	float dist;
	GFC_Vector3D position;
} RayCollision;

#include "physics.h"

RayCollision rayTest(GFC_Edge3D ray, PhysicsBody *body);

#endif
