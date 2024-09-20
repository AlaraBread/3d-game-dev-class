#include "physics.h"

#ifndef __ALARA_GJK__
#define __ALARA_GJK__

typedef struct Collision_s {
	// true if we detected a collision
	Bool hit;
	PhysicsBody *a;
	PhysicsBody *b;
	// the closest point to the collision on the surface of a in global coordinates
	GFC_Vector3D aPosition;
	// the closest point to the collision on the surface of b in global coordinates
	GFC_Vector3D bPosition;
} Collision;

Collision gjk(PhysicsBody *a, PhysicsBody *b);

#endif
