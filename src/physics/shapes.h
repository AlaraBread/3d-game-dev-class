#ifndef __ALARA_SHAPES__
#define __ALARA_SHAPES__

#include "gfc_vector.h"
#include "gf3d_mesh.h"
#include "physics.h"

GFC_Vector3D support(PhysicsBody *body, GFC_Vector3D direction);
GFC_Vector3D minkowskiPoint(PhysicsBody *a, PhysicsBody *b, GFC_Vector3D direction, GFC_Vector3D *aSupport, GFC_Vector3D *bSupport);

#endif
