#ifndef __ALARA_SHAPES__
#define __ALARA_SHAPES__

#include "gf3d_mesh.h"
#include "gfc_vector.h"
#include "physics.h"

typedef struct SupportPoint {
	GFC_Vector3D m;
	GFC_Vector3D a;
	GFC_Vector3D b;
} SupportPoint;

GFC_Vector3D support(PhysicsBody *body, GFC_Vector3D direction);
SupportPoint minkowskiPoint(PhysicsBody *a, PhysicsBody *b, GFC_Vector3D direction);

#endif
