#ifndef __ALARA_GJK__
#define __ALARA_GJK__

#include "physics.h"

typedef struct Collision_s {
	// true if we detected a collision
	Bool hit;
	// the closest point to the collision on the surface of a in global coordinates
	GFC_Vector3D aPosition;
	float penetrationDepth;
	GFC_Vector3D normal;
} Collision;

Collision doCollision(PhysicsBody *a, PhysicsBody *b);

#endif
