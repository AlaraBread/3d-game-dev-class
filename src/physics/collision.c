#include "simple_logger.h"
#include "collision.h"
#include "lists.h"
#include "gfc_hashmap.h"
#include "util.h"

typedef struct Simplex_s {
	int numPoints;
	GFC_Vector3D points[4];
	GFC_Vector3D aPoints[4];
	GFC_Vector3D bPoints[4];
} Simplex;

void freeCollision();

// using static memory to avoid heap allocations for every collision
static Vector3List *epaVertexList;
static Vector3List *epaAVertexList;
static Vector3List *epaBVertexList;
static IntList *epaFaceList;

void initCollision() {
	epaVertexList = newVec3List();
	epaAVertexList = newVec3List();
	epaBVertexList = newVec3List();
	epaFaceList = newIntList();
}

void freeCollision() {
	triangleListFree(&epaVertexList);
	intListFree(&epaFaceList);
}

Bool gjk(PhysicsBody *a, PhysicsBody *b, Simplex *simplex);
Collision epa(PhysicsBody *a, PhysicsBody *b, Simplex *simplex);

Collision doCollision(PhysicsBody *a, PhysicsBody *b) {
	Simplex simplex = {0};
	Collision col = {0};
	if(gjk(a, b, &simplex)) {
		col = epa(a, b, &simplex);
	}
	return col;
}

void resetEpa(Simplex *simplex);
GFC_Vector3D findClosestDirection(int idx[3]);
void addPointToPolytope(GFC_Vector3D point, GFC_Vector3D aSupport, GFC_Vector3D bSupport);
Collision calculateCollision(int triangle[3]);

Collision epa(PhysicsBody *a, PhysicsBody *b, Simplex *simplex) {
	resetEpa(simplex);
	int closestTriangle[3];
	GFC_Vector3D closestDirection = findClosestDirection(closestTriangle);
	while(1) {
		closestDirection = findClosestDirection(closestTriangle);
		GFC_Vector3D aSupport;
		GFC_Vector3D bSupport;
		GFC_Vector3D support = minkowskiPoint(a, b, closestDirection, &aSupport, &bSupport);
		if(gfc_vector3d_dot_product(support, closestDirection) < 0.01) {
			break;
		}
		addPointToPolytope(support, aSupport, bSupport);
	}
	return calculateCollision(closestTriangle);
}

void resetEpa(Simplex *simplex) {
	gfc_list_clear(epaFaceList);
	gfc_list_clear(epaVertexList);
	if(simplex->numPoints != 4) {
		slog("something has gone horribly wrong");
	}
	for(int i = 0; i < 4; i++) {
		vec3ListAppend(epaVertexList, simplex->points[i]);
		vec3ListAppend(epaAVertexList, simplex->aPoints[i]);
		vec3ListAppend(epaBVertexList, simplex->bPoints[i]);
	}
	for(int i = 0; i < 4; i++) {
		int a = i;
		int b = (i+1)%4;
		int c = (i+2)%4;
		GFC_Triangle3D triangle;
		triangle.a = vec3ListGet(epaVertexList, a);
		triangle.b = vec3ListGet(epaVertexList, b);
		triangle.c = vec3ListGet(epaVertexList, c);
		GFC_Vector3D center = triangleCenter(triangle);
		if(gfc_vector3d_dot_product(gfc_trigfc_angle_get_normal(triangle), center) > 0.0) {
			// dont need to swap winding
			intListAppend(epaFaceList, a);
			intListAppend(epaFaceList, b);
			intListAppend(epaFaceList, c);
		} else {
			// need to swap winding
			intListAppend(epaFaceList, b);
			intListAppend(epaFaceList, a);
			intListAppend(epaFaceList, c);
		}
	}
}

Collision calculateCollision(int triangleIdx[3]) {
	GFC_Triangle3D triangle;
	triangle.a = vec3ListGet(epaVertexList, triangleIdx[0]);
	triangle.b = vec3ListGet(epaVertexList, triangleIdx[1]);
	triangle.c = vec3ListGet(epaVertexList, triangleIdx[2]);
	// project origin onto triangle
	
	// keep track of distance we project
	// convert projected point to barycentric coordinates
	// use barycentric coordinates on support triangles to calculate contact points
	GFC_Triangle3D aTriangle;
	aTriangle.a = vec3ListGet(epaAVertexList, triangleIdx[0]);
	aTriangle.b = vec3ListGet(epaAVertexList, triangleIdx[1]);
	aTriangle.c = vec3ListGet(epaAVertexList, triangleIdx[2]);
	GFC_Triangle3D bTriangle;
	bTriangle.a = vec3ListGet(epaBVertexList, triangleIdx[0]);
	bTriangle.b = vec3ListGet(epaBVertexList, triangleIdx[1]);
	bTriangle.c = vec3ListGet(epaBVertexList, triangleIdx[2]);
}

GFC_Vector3D findClosestDirection(int idx[3]) {
	GFC_Vector3D closestNormal;
	float closestDist = INFINITY;
	int count = intListLength(epaFaceList);
	for(int i = 0; i < count; i += 3) {
		int a = intListGet(epaFaceList, i);
		int b = intListGet(epaFaceList, i+1);
		int c = intListGet(epaFaceList, i+2);

		GFC_Triangle3D triangle;
		triangle.a = vec3ListGet(epaVertexList, a);
		triangle.b = vec3ListGet(epaVertexList, b);
		triangle.c = vec3ListGet(epaVertexList, c);

		GFC_Vector3D center = triangleCenter(triangle);

		GFC_Vector3D normal = gfc_trigfc_angle_get_normal(triangle);
		float dist = gfc_vector3d_dot_product(normal, center);
		if(dist < closestDist) {
			closestDist = dist;
			closestNormal = normal;
			idx[0] = a;
			idx[1] = b;
			idx[2] = c;
		}
	}
	return closestNormal;
}

void addPointToPolytope(GFC_Vector3D point, GFC_Vector3D aSupport, GFC_Vector3D bSupport) {
	int pointIdx = vec3ListLength(epaVertexList);
	vec3ListAppend(epaVertexList, point);
	vec3ListAppend(epaAVertexList, aSupport);
	vec3ListAppend(epaBVertexList, bSupport);
	int numPoints = intListLength(epaFaceList);
	GFC_HashMap *edgeFreq = gfc_hashmap_new();
	for(int i = 0; i < numPoints; i += 3) {
		int triangleIdx[3] = {
			intListGet(epaFaceList, i),
			intListGet(epaFaceList, i+1),
			intListGet(epaFaceList, i+2),
		};
		GFC_Triangle3D triangle;
		triangle.a = vec3ListGet(epaVertexList, triangleIdx[0]);
		triangle.b = vec3ListGet(epaVertexList, triangleIdx[1]);
		triangle.c = vec3ListGet(epaVertexList, triangleIdx[2]);

		GFC_Vector3D normal = gfc_trigfc_angle_get_normal(triangle);
		if(gfc_vector3d_dot_product(normal, point) > 0.0) {
			for(int t = 0; t < 3; t++) {
				char edgeText[2*sizeof(int)+1];
				edgeText[sizeof(edgeText)-1] = 0;
				// we have to sort the edge indicies
				int a = MAX(triangleIdx[t], triangleIdx[(t+1)%3]);
				int b = MIN(triangleIdx[t], triangleIdx[(t+1)%3]);
				*(int *)(&edgeText[0*sizeof(int)]) = a;
				*(int *)(&edgeText[1*sizeof(int)]) = b;
				int edgeCount = (int)gfc_hashmap_get(edgeFreq, edgeText);
				gfc_hashmap_insert(edgeFreq, edgeText, (void *)(edgeCount+1));
			}
			intListRemove(epaFaceList, i);
			intListRemove(epaFaceList, i+1);
			intListRemove(epaFaceList, i+2);
			i -= 3;
		}
	}
	GFC_List *edges = gfc_hashmap_get_all_values(edgeFreq);
	int numEdges = gfc_list_get_count(edges);
	for(int i = 0; i < numEdges; i++) {
		GFC_HashElement *element = (GFC_HashElement *)gfc_list_get_nth(edges, i);
		if((int)element->data != 1) {
			continue;
		}
		int a = *(int *)(&element->key[0*sizeof(int)]);
		int b = *(int *)(&element->key[1*sizeof(int)]);
		GFC_Triangle3D triangle;
		triangle.a = vec3ListGet(epaVertexList, a);
		triangle.b = vec3ListGet(epaVertexList, b);
		triangle.c = point;

		GFC_Vector3D center = triangleCenter(triangle);

		if(gfc_vector3d_dot_product(gfc_trigfc_angle_get_normal(triangle), center) > 0.0) {
			// normal is in same direction as triangle position
			// we dont need to change winding
			intListAppend(epaFaceList, a);
			intListAppend(epaFaceList, b);
			intListAppend(epaFaceList, pointIdx);
		} else {
			// we need to swap winding order
			intListAppend(epaFaceList, b);
			intListAppend(epaFaceList, a);
			intListAppend(epaFaceList, pointIdx);
		}
	}
	gfc_hashmap_free(edgeFreq);
}

Bool nextSimplex(Simplex *simplex, GFC_Vector3D *direction);

Bool gjk(PhysicsBody *a, PhysicsBody *b, Simplex *simplex) {
	GFC_Vector3D direction;
	gfc_vector3d_sub(direction, a->position, b->position);
	gfc_vector3d_normalize(&direction);
	while(1) {
		GFC_Vector3D aSupport;
		GFC_Vector3D bSupport;
		simplex->points[simplex->numPoints] = minkowskiPoint(a, b, direction, &aSupport, &bSupport);
		simplex->aPoints[simplex->numPoints] = aSupport;
		simplex->bPoints[simplex->numPoints] = bSupport;
		if(gfc_vector3d_dot_product(simplex->points[simplex->numPoints], direction) < 0) {
			return false;
		}
		simplex->numPoints = MIN(4, simplex->numPoints+1);
		if(nextSimplex(simplex, &direction)) {
			return true;
		}
	}
}

// borrowed some math from https://winter.dev/articles/gjk-algorithm

Bool simplexLine(Simplex *simplex, GFC_Vector3D *direction) {
	GFC_Vector3D ab;
	gfc_vector3d_sub(ab, simplex->points[0], simplex->points[1]);
	GFC_Vector3D ao;
	gfc_vector3d_negate(ao, simplex->points[1]);
	if(gfc_vector3d_dot_product(ao, ab) > 0.0) {
		gfc_vector3d_cross_product(direction, ab, ao);
		gfc_vector3d_cross_product(direction, *direction, ab);
		gfc_vector3d_normalize(direction);
	} else {
		simplex->numPoints = 1;
		gfc_vector3d_normalize(&ao);
		gfc_vector3d_copy((*direction), ao);
	}
	return false;
}

Bool simplexTriangle(Simplex *simplex, GFC_Vector3D *direction) {
	GFC_Vector3D a = simplex->points[2];
	GFC_Vector3D b = simplex->points[1];
	GFC_Vector3D c = simplex->points[0];
	GFC_Vector3D ab;
	gfc_vector3d_sub(ab, b, a);
	GFC_Vector3D ac;
	gfc_vector3d_sub(ac, c, a);
	GFC_Vector3D ao;
	gfc_vector3d_negate(ao, a);
	GFC_Vector3D abc;
	gfc_vector3d_cross_product(&abc, ab, ac);
	GFC_Vector3D abcN;
	gfc_vector3d_cross_product(&abcN, abc, ac);
	if(gfc_vector3d_dot_product(abcN, ao) > 0.0) {
		if(gfc_vector3d_dot_product(ac, ao) > 0.0) {
			simplex->points[0] = c;
			simplex->aPoints[0] = simplex->aPoints[0];
			simplex->bPoints[0] = simplex->bPoints[0];
			simplex->points[1] = a;
			simplex->aPoints[1] = simplex->aPoints[2];
			simplex->bPoints[1] = simplex->bPoints[2];
			simplex->numPoints = 2;
			gfc_vector3d_cross_product(&direction, ac, ao);
			gfc_vector3d_cross_product(&direction, *direction, ac);
			gfc_vector3d_normalize(&direction);
			return false;
		} else {
			simplex->points[0] = b;
			simplex->aPoints[0] = simplex->aPoints[1];
			simplex->bPoints[0] = simplex->bPoints[1];
			simplex->points[1] = a;
			simplex->aPoints[1] = simplex->aPoints[2];
			simplex->bPoints[1] = simplex->bPoints[2];
			simplex->numPoints = 2;
			return simplexLine(simplex, direction);
		}
	} else {
		GFC_Vector3D t;
		gfc_vector3d_cross_product(&t, ab, abc);
		if(gfc_vector3d_dot_product(t, ao) > 0.0) {
			simplex->points[0] = b;
			simplex->aPoints[0] = simplex->aPoints[1];
			simplex->bPoints[0] = simplex->bPoints[1];
			simplex->points[1] = a;
			simplex->aPoints[1] = simplex->aPoints[2];
			simplex->bPoints[1] = simplex->bPoints[2];
			simplex->numPoints = 2;
			return simplexLine(simplex, direction);
		} else {
			if(gfc_vector3d_dot_product(abc, ao) > 0.0) {
				simplex->numPoints = 3;
				gfc_vector3d_normalize(&abc);
				gfc_vector3d_copy((*direction), abc);
				return false;
			} else {
				simplex->points[0] = b;
				simplex->aPoints[0] = simplex->aPoints[1];
				simplex->bPoints[0] = simplex->bPoints[1];
				simplex->points[1] = c;
				simplex->aPoints[1] = simplex->aPoints[0];
				simplex->bPoints[1] = simplex->bPoints[0];
				simplex->points[2] = a;
				simplex->aPoints[2] = simplex->aPoints[2];
				simplex->bPoints[2] = simplex->bPoints[2];
				simplex->numPoints = 3;
				gfc_vector3d_negate(abc, abc);
				gfc_vector3d_normalize(&abc);
				gfc_vector3d_copy((*direction), abc);
				return false;
			}
		}
	}
}

Bool simplexTetrahedron(Simplex *simplex, GFC_Vector3D *direction) {
	GFC_Vector3D a = simplex->points[3];
	GFC_Vector3D b = simplex->points[2];
	GFC_Vector3D c = simplex->points[1];
	GFC_Vector3D d = simplex->points[0];
	GFC_Vector3D ab;
	gfc_vector3d_sub(ab, b, a);
	GFC_Vector3D ac;
	gfc_vector3d_sub(ac, c, a);
	GFC_Vector3D ad;
	gfc_vector3d_sub(ad, d, a);
	GFC_Vector3D ao;
	gfc_vector3d_negate(ao, a);
	GFC_Vector3D abc;
	gfc_vector3d_cross_product(&abc, ab, ac);
	GFC_Vector3D acd;
	gfc_vector3d_cross_product(&acd, ac, ad);
	GFC_Vector3D adb;
	gfc_vector3d_cross_product(&adb, ad, ab);
	if(gfc_vector3d_dot_product(abc, ao) > 0.0) {
		simplex->points[0] = c;
		simplex->aPoints[0] = simplex->aPoints[1];
		simplex->bPoints[0] = simplex->bPoints[1];
		simplex->points[1] = b;
		simplex->aPoints[1] = simplex->aPoints[2];
		simplex->bPoints[1] = simplex->bPoints[2];
		simplex->points[2] = a;
		simplex->aPoints[2] = simplex->aPoints[3];
		simplex->bPoints[2] = simplex->bPoints[3];
		simplex->numPoints = 3;
		return simplexTriangle(simplex, direction);
	}
	if(gfc_vector3d_dot_product(acd, ao) > 0.0) {
		simplex->points[0] = d;
		simplex->aPoints[0] = simplex->aPoints[0];
		simplex->bPoints[0] = simplex->bPoints[0];
		simplex->points[1] = c;
		simplex->aPoints[1] = simplex->aPoints[1];
		simplex->bPoints[1] = simplex->bPoints[1];
		simplex->points[2] = a;
		simplex->aPoints[2] = simplex->aPoints[3];
		simplex->bPoints[2] = simplex->bPoints[3];
		simplex->numPoints = 3;
		return simplexTriangle(simplex, direction);
	}
	if(gfc_vector3d_dot_product(adb, ao) > 0.0) {
		simplex->points[0] = b;
		simplex->aPoints[0] = simplex->aPoints[2];
		simplex->bPoints[0] = simplex->bPoints[2];
		simplex->points[1] = d;
		simplex->aPoints[1] = simplex->aPoints[0];
		simplex->bPoints[1] = simplex->bPoints[0];
		simplex->points[2] = a;
		simplex->aPoints[2] = simplex->aPoints[3];
		simplex->bPoints[2] = simplex->bPoints[3];
		simplex->numPoints = 3;
		return simplexTriangle(simplex, direction);
	}
	return true;
}

Bool nextSimplex(Simplex *simplex, GFC_Vector3D *direction) {
	if(simplex->numPoints == 0) {
		return false;
	}
	if(simplex->numPoints == 1) {
		gfc_vector3d_negate((*direction), simplex->points[0]);
		gfc_vector3d_normalize(direction);
		return false;
	}
	if(simplex->numPoints == 2) {
		return simplexLine(simplex, direction);
	}
	if(simplex->numPoints == 3) {
		return simplexTriangle(simplex, direction);
	}
	if(simplex->numPoints == 4) {
		return simplexTetrahedron(simplex, direction);
	}
	slog("This shouldnt happen");
	return false;
}
