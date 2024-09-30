#ifndef __ALARA_LISTS__
#define __ALARA_LISTS__

#include "gfc_list.h"
#include "gfc_primitives.h"
#include "gfc_vector.h"

typedef struct GFC_List IntList;
typedef struct GFC_List Vector3List;
typedef struct GFC_List TriangleList;

Vector3List *newIntList();
Vector3List *newVec3List();
TriangleList *newTriangleList();
int intListGet(IntList *list, int index);
GFC_Vector3D vec3ListGet(Vector3List *list, int index);
GFC_Triangle3D triangleListGet(TriangleList *list, int index);
void intListAppend(IntList *list, int value);
void vec3ListAppend(Vector3List *list, GFC_Vector3D vector);
void triangleListAppend(TriangleList *list, GFC_Triangle3D triangle);
void intListRemove(IntList *list, int index);
void vec3ListRemove(Vector3List *list, int index);
void triangleListRemove(TriangleList *list, int index);
int intListLength(IntList *list);
int vec3ListLength(Vector3List *list);
int triangleListLength(Vector3List *list);
int intListPop(IntList *list);
GFC_Vector3D vec3ListPop(Vector3List *list);
GFC_Triangle3D triangleListPop(Vector3List *list);
void intListClear(IntList *list);
void vec3ListClear(Vector3List *list);
void triangleListClear(TriangleList *list);
void intListFree(IntList *list);
void vec3ListFree(Vector3List *list);
void triangleListFree(TriangleList *list);

#endif
