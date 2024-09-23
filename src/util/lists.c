#include "lists.h"

IntList *newIntList() {
	return gfc_list_new();
}

Vector3List *newVec3List() {
	return gfc_list_new();
}

TriangleList *newTriangleList() {
	return gfc_list_new();
}

int intListGet(IntList *list, int index) {
	void *p = gfc_list_get_nth(list, index);
	return *(int *)(&p);
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
	triangle.a = vec3ListGet(list, index);
	triangle.a = vec3ListGet(list, index+1);
	triangle.a = vec3ListGet(list, index+2);
	return triangle;
}

void intListAppend(IntList *list, int value) {
	int *p = &value;
	gfc_list_append(list, (int *)(&p));
}

void vec3ListAppend(Vector3List *list, GFC_Vector3D vector) {
	float *x = &vector.x;
	gfc_list_append(list, (float *)(&x));
	float *y = &vector.y;
	gfc_list_append(list, (float *)(&y));
	float *z = &vector.z;
	gfc_list_append(list, (float *)(&z));
}

void triangleListAppend(TriangleList *list, GFC_Triangle3D triangle) {
	vec3ListAppend(list, triangle.a);
	vec3ListAppend(list, triangle.b);
	vec3ListAppend(list, triangle.c);
}

void intListRemove(IntList *list, int index) {
	gfc_list_delete_nth(list, index);
}

void vec3ListRemove(Vector3List *list, int index) {
	gfc_list_delete_nth(list, index);
	gfc_list_delete_nth(list, index);
	gfc_list_delete_nth(list, index);
}

void triangleListRemove(TriangleList *list, int index) {
	vec3ListRemove(list, index);
	vec3ListRemove(list, index);
	vec3ListRemove(list, index);
}

int intListLength(IntList *list) {
	return gfc_list_get_count(list);
}

int vec3ListLength(Vector3List *list) {
	return gfc_list_get_count(list)/3;
}

int triangleListLength(Vector3List *list) {
	return gfc_list_get_count(list)/9;
}

int intListPop(IntList *list) {
	int count = intListLength(list);
	return intListGet(list, count-1);
}

GFC_Vector3D vec3ListPop(Vector3List *list) {
	int count = vec3ListLength(list);
	return vec3ListGet(list, count-1);
}

GFC_Triangle3D triangleListPop(Vector3List *list) {
	int count = triangleListLength(list);
	return triangleListGet(list, count-1);
}

void intListFree(IntList *list) {
	gfc_list_delete(list);
}

void vec3ListFree(Vector3List *list) {
	gfc_list_delete(list);
}

void triangleListFree(TriangleList *list) {
	gfc_list_delete(list);
}
