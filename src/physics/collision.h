#ifndef __ALARA_GJK__
#define __ALARA_GJK__

#include "physics.h"

typedef struct Collision_s {
	// true if we detected a collision
	Bool hit;
	// the closest point to the collision on the surface of a in global coordinates
	GFC_Vector3D aPosition;
	GFC_Vector3D bPosition;
	float penetrationDepth;
	GFC_Vector3D normal;
} Collision;

void initCollision();
void freeCollision();

Collision doCollision(PhysicsBody *a, PhysicsBody *b);

#endif
