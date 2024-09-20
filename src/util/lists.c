#include "lists.h"

Vector3List newVec3List() {
	Vector3List list;
	list.list = gfc_list_new();
	return list;
}

TriangleList newTriangleList() {
	TriangleList list;
	list.list = gfc_list_new();
	return list;
}

GFC_Vector3D vec3ListGet(Vector3List *list, int index) {
	index *= 3;
	GFC_Vector3D v;
	void *x = gfc_list_get_nth(list, index);
	v.x = *(float *)(&x);
	void *y = gfc_list_get_nth(list, index+1);
	v.y = *(float *)(&y);
	void *z = gfc_list_get_nth(list, index+2);
	v.z = *(float *)(&z);
}

GFC_Triangle3D triangleListGet(TriangleList *list, int index) {
	index *= 3;
	GFC_Triangle3D triangle;
	triangle.a = vec3ListGet(&list->list, index);
	triangle.a = vec3ListGet(&list->list, index+1);
	triangle.a = vec3ListGet(&list->list, index+2);
	return triangle;
}

void vec3ListAppend(Vector3List *list, GFC_Vector3D vector) {
	float *x = &vector.x;
	gfc_list_append(list->list, (float *)(&x));
	float *y = &vector.y;
	gfc_list_append(list->list, (float *)(&y));
	float *z = &vector.z;
	gfc_list_append(list->list, (float *)(&z));
}

void triangleListAppend(TriangleList *list, GFC_Triangle3D triangle) {
	vec3ListAppend(list->list, triangle.a);
	vec3ListAppend(list->list, triangle.b);
	vec3ListAppend(list->list, triangle.c);
}

void vec3ListRemove(Vector3List *list, int index) {
	gfc_list_delete_nth(list->list, index);
	gfc_list_delete_nth(list->list, index);
	gfc_list_delete_nth(list->list, index);
}

void triangleListRemove(TriangleList *list, int index) {
	vec3ListRemove(list->list, index);
	vec3ListRemove(list->list, index);
	vec3ListRemove(list->list, index);
}
