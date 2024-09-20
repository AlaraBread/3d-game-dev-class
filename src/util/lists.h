#include "gfc_vector.h"
#include "gfc_primitives.h"
#include "gfc_list.h"

#ifndef __ALARA_LISTS__
#define __ALARA_LISTS__

typedef struct Vector3List_s {
	GFC_List *list;
} Vector3List;

typedef struct TriangleList_s {
	Vector3List *list;
} TriangleList;

Vector3List newVec3List();
TriangleList newTriangleList();
GFC_Vector3D vec3ListGet(Vector3List *list, int index);
GFC_Triangle3D triangleListGet(TriangleList *list, int index);
void vec3ListAppend(Vector3List *list, GFC_Vector3D vector);
void triangleListAppend(TriangleList *list, GFC_Triangle3D triangle);
void vec3ListRemove(Vector3List *list, int index);
void triangleListRemove(TriangleList *list, int index);

#endif
