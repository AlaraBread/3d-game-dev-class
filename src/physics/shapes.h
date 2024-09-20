#include "gfc_vector.h"
#include "gf3d_mesh.h"

#ifndef __ALARA_SHAPES__
#define __ALARA_SHAPES__

typedef enum ShapeType_e {
	SPHERE,
	CONVEX_HULL,
} ShapeType;

typedef struct Sphere_s {
	float radius;
} Sphere;

typedef struct ConvexHull_s {
	ObjData *mesh;
} ConvexHull;

typedef struct Shape_s {
	ShapeType shapeType;
	union {
		Sphere sphere;
	} shape;
} Shape;

GFC_Vector3D support(Shape *shape, GFC_Vector3D direction);
GFC_Vector3D minkowskiPoint(Shape *a, Shape *b, GFC_Vector3D direction);

#endif
