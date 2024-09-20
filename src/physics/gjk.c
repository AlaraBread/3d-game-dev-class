#include "gjk.h"

typedef struct Simplex_s {
	int numPoints;
	GFC_Vector3D points[4];
} Simplex;

typedef struct Polytope_s {
	GFC_List points;
	GFC_List triangles;
} Polytope;

Bool gjk(PhysicsBody *a, PhysicsBody *b, Simplex *simplex);

Collision doCollision(PhysicsBody *a, PhysicsBody *b) {

}

Bool nextSimplex(Simplex *simplex, GFC_Vector3D *direction);

Bool gjk(PhysicsBody *a, PhysicsBody *b, Simplex *simplex) {
	GFC_Vector3D direction;
	gfc_vector3d_sub(direction, a->position, b->position);
	gfc_vector3d_normalize(&direction);
	while(1) {
		simplex->points[simplex->numPoints] = minkowskiPoint(a, b, direction);
		if(gfc_vector3d_dot_product(simplex->points[simplex->numPoints], direction) < 0) {
			return false;
		}
		simplex->numPoints = MIN(4, simplex->numPoints+1);
		if(nextSimplex(simplex, &direction)) {
			return true;
		}
	}
}

// borrowed some math from https://www.youtube.com/watch?v=MDusDn8oTSE

Bool simplexLine(Simplex *simplex, GFC_Vector3D *direction) {
	GFC_Vector3D ab;
	gfc_vector3d_sub(ab, simplex->points[0], simplex->points[1]);
	GFC_Vector3D ao;
	gfc_vector3d_negate(ao, simplex->points[1]);
	if(gfc_vector3d_dot_product(ao, ab) < 0.0) {
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
	if(gfc_vector3d_dot_product(abcN, ao) < 0.0) {
		if(gfc_vector3d_dot_product(ac, ao) < 0.0) {
			simplex->points[0] = c;
			simplex->points[1] = a;
			simplex->numPoints = 2;
			gfc_vector3d_cross_product(&direction, ac, ao);
			gfc_vector3d_cross_product(&direction, *direction, ac);
			gfc_vector3d_normalize(&direction);
			return false;
		} else {
			simplex->points[0] = b;
			simplex->points[1] = a;
			simplex->numPoints = 2;
			return simplexLine(simplex, direction);
		}
	} else {
		GFC_Vector3D t;
		gfc_vector3d_cross_product(&t, ab, abc);
		if(gfc_vector3d_dot_product(t, ao) < 0.0) {
			simplex->points[0] = b;
			simplex->points[1] = a;
			simplex->numPoints = 2;
			return simplexLine(simplex, direction);
		} else {
			if(gfc_vector3d_dot_product(abc, ao) < 0.0) {
				simplex->numPoints = 3;
				gfc_vector3d_normalize(&abc);
				gfc_vector3d_copy((*direction), abc);
				return false;
			} else {
				simplex->points[0] = b;
				simplex->points[1] = c;
				simplex->points[2] = a;
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
	if(gfc_vector3d_dot_product(abc, ao) < 0.0) {
		simplex->points[0] = c;
		simplex->points[1] = b;
		simplex->points[2] = a;
		simplex->numPoints = 3;
		return simplexTriangle(simplex, direction);
	}
	if(gfc_vector3d_dot_product(acd, ao) < 0.0) {
		simplex->points[0] = d;
		simplex->points[1] = c;
		simplex->points[2] = a;
		simplex->numPoints = 3;
		return simplexTriangle(simplex, direction);
	}
	if(gfc_vector3d_dot_product(adb, ao) < 0.0) {
		simplex->points[1] = a;
		simplex->points[1] = d;
		simplex->points[2] = b;
		simplex->numPoints = 3;
		return simplexTriangle(simplex, direction);
	}
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
