#include "lists.h"

IntList *newIntList() { return (IntList *)gfc_list_new(); }

Vector3List *newVec3List() { return (Vector3List *)gfc_list_new(); }

TriangleList *newTriangleList() { return (TriangleList *)gfc_list_new(); }

int intListGet(IntList *list, int index) {
	void *p = gfc_list_get_nth((GFC_List *)list, index);
	return *(int *)(&p);
}

GFC_Vector3D vec3ListGet(Vector3List *list, int index) {
	index *= 3;
	GFC_Vector3D v;
	void *x = gfc_list_get_nth((GFC_List *)list, index);
	v.x = *(float *)(&x);
	void *y = gfc_list_get_nth((GFC_List *)list, index + 1);
	v.y = *(float *)(&y);
	void *z = gfc_list_get_nth((GFC_List *)list, index + 2);
	v.z = *(float *)(&z);
	return v;
}

GFC_Triangle3D triangleListGet(TriangleList *list, int index) {
	index *= 3;
	GFC_Triangle3D triangle;
	triangle.a = vec3ListGet((Vector3List *)list, index);
	triangle.a = vec3ListGet((Vector3List *)list, index + 1);
	triangle.a = vec3ListGet((Vector3List *)list, index + 2);
	return triangle;
}

void intListAppend(IntList *list, int value) { gfc_list_append((GFC_List *)list, *(void **)(&value)); }

void vec3ListAppend(Vector3List *list, GFC_Vector3D vector) {
	gfc_list_append((GFC_List *)list, *(void **)(&vector.x));
	gfc_list_append((GFC_List *)list, *(void **)(&vector.y));
	gfc_list_append((GFC_List *)list, *(void **)(&vector.z));
}

void triangleListAppend(TriangleList *list, GFC_Triangle3D triangle) {
	vec3ListAppend((Vector3List *)list, triangle.a);
	vec3ListAppend((Vector3List *)list, triangle.b);
	vec3ListAppend((Vector3List *)list, triangle.c);
}

void intListRemove(IntList *list, int index) { gfc_list_delete_nth((GFC_List *)list, index); }

void vec3ListRemove(Vector3List *list, int index) {
	gfc_list_delete_nth((GFC_List *)list, index);
	gfc_list_delete_nth((GFC_List *)list, index);
	gfc_list_delete_nth((GFC_List *)list, index);
}

void triangleListRemove(TriangleList *list, int index) {
	vec3ListRemove((Vector3List *)list, index);
	vec3ListRemove((Vector3List *)list, index);
	vec3ListRemove((Vector3List *)list, index);
}

int intListLength(IntList *list) { return gfc_list_get_count((GFC_List *)list); }

int vec3ListLength(Vector3List *list) { return gfc_list_get_count((GFC_List *)list) / 3; }

int triangleListLength(Vector3List *list) { return gfc_list_get_count((GFC_List *)list) / 9; }

int intListPop(IntList *list) {
	int count = intListLength(list);
	return intListGet(list, count - 1);
}

GFC_Vector3D vec3ListPop(Vector3List *list) {
	int count = vec3ListLength(list);
	return vec3ListGet(list, count - 1);
}

GFC_Triangle3D triangleListPop(Vector3List *list) {
	int count = triangleListLength(list);
	return triangleListGet(list, count - 1);
}

void intListClear(IntList *list) { gfc_list_clear((GFC_List *)list); }

void vec3ListClear(Vector3List *list) { gfc_list_clear((GFC_List *)list); }

void triangleListClear(TriangleList *list) { gfc_list_clear((GFC_List *)list); }

void intListFree(IntList *list) { gfc_list_delete((GFC_List *)list); }

void vec3ListFree(Vector3List *list) { gfc_list_delete((GFC_List *)list); }

void triangleListFree(TriangleList *list) { gfc_list_delete((GFC_List *)list); }
