#ifndef __ALARA_COLLISION__
#define __ALARA_COLLISION__

#include "gfc_vector.h"

typedef struct PhysicsBody_s PhysicsBody;

typedef struct Collision_s {
	// true if we detected a collision
	Bool hit;
	// the closest point to the collision on the surface of a in global coordinates
	GFC_Vector3D aPosition;
	GFC_Vector3D bPosition;
	double penetrationDepth;
	GFC_Vector3D normal;
	PhysicsBody *a, *b;
} Collision;

Collision doCollision(PhysicsBody *a, PhysicsBody *b);

#endif
