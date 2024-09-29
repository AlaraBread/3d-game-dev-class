#include "simple_logger.h"
#include "collision.h"
#include "lists.h"
#include "gfc_hashmap.h"
#include "util.h"
#include "shapes.h"
#include "gf3d_draw.h"

Collision mpr(PhysicsBody *a, PhysicsBody *b);

Collision doCollision(PhysicsBody *a, PhysicsBody *b) {
	// todo: analytic solutions for specific shape pairs
	return mpr(a, b);
}

inline Bool triangleFacingOrigin(GFC_Triangle3D triangle);

// as described in game programming gems 7 by Gary Snethen
Collision mpr(PhysicsBody *a, PhysicsBody *b) {
	// find origin ray
	GFC_Vector3D v0;
	gfc_vector3d_sub(v0, b->position, a->position);
	// find candidate portal
	GFC_Vector3D v0_n = v0;
	gfc_vector3d_normalize(&v0_n);
	SupportPoint v1 = minkowskiPoint(a, b, v0_n);
	GFC_Vector3D v1xv0;
	gfc_vector3d_cross_product(&v1xv0, v1.m, v0);
	gfc_vector3d_normalize(&v1xv0);
	if(gfc_vector3d_magnitude_squared(v1xv0) < 0.01) {
		v1xv0 = perpendicularVector3(v1.m);
	}
	SupportPoint v2 = minkowskiPoint(a, b, v1xv0);
	GFC_Vector3D v0v2;
	gfc_vector3d_sub(v0v2, v2.m, v0);
	GFC_Vector3D v0v1;
	gfc_vector3d_sub(v0v1, v1.m, v0);
	GFC_Vector3D v0v2xv0v1;
	gfc_vector3d_cross_product(&v0v2xv0v1, v0v2, v0v1);
	gfc_vector3d_normalize(&v0v2xv0v1);
	SupportPoint v3 = minkowskiPoint(a, b, v0v2xv0v1);
	int i = 0;
	while(true) {
		Bool p1 = triangleFacingOrigin(gfc_triangle(v0, v2.m, v1.m));
		Bool p2 = triangleFacingOrigin(gfc_triangle(v0, v3.m, v2.m));
		Bool p3 = triangleFacingOrigin(gfc_triangle(v0, v1.m, v3.m));
		if(p1 && p2 && p3) {
			// valid candidate
			break;
		}
		// choose new candidate
		if(!p1) {
			// swap winding to get normal facing away from origin
			v3 = minkowskiPoint(a, b, gfc_trigfc_angle_get_normal(gfc_triangle(v0, v2.m, v1.m)));
			// preserve normal direction
			SupportPoint tmp = v1;
			v1 = v2;
			v2 = tmp;
		}
		else if(!p2) {
			// swap winding to get normal facing away from origin
			v1 = minkowskiPoint(a, b, gfc_trigfc_angle_get_normal(gfc_triangle(v0, v3.m, v2.m)));
			// preserve normal direction
			SupportPoint tmp = v3;
			v3 = v2;
			v2 = tmp;
		}
		else if(!p3) {
			// swap winding to get normal facing away from origin
			v2 = minkowskiPoint(a, b, gfc_trigfc_angle_get_normal(gfc_triangle(v0, v3.m, v1.m)));
			// preserve normal direction
			SupportPoint tmp = v1;
			v1 = v3;
			v3 = tmp;
		}
		i++;
		if(i > 50) {
			slog("took too long to find portal");
			Collision col = {0};
			return col;
		}
	}
	// portal refinement
	int collisionAccuracy = 10;
	while(true) {
		if(triangleFacingOrigin(gfc_triangle(v1.m, v2.m, v3.m))) {
			// origin is inside portal, hit
			// keep going for a little bit to get more accurate collision
			collisionAccuracy--;
			if(collisionAccuracy <= 0) {
				Collision col;
				col.hit = true;
				GFC_Triangle3D mTriangle = gfc_triangle(v1.m, v2.m, v3.m);
				col.normal = gfc_trigfc_angle_get_normal(mTriangle);
				col.penetrationDepth = gfc_vector3d_dot_product(col.normal, v1.m);
				GFC_Vector3D ao; // from a to origin
				gfc_vector3d_negate(ao, mTriangle.a);
				GFC_Vector3D aop = projectVectorOntoPlane(ao, col.normal); // a to origin, projected onto triangle
				GFC_Vector3D bary = toBarycentric(aop, mTriangle);
				col.aPosition = fromBarycentric(bary, gfc_triangle(v1.a, v2.a, v3.a));
				col.bPosition = fromBarycentric(bary, gfc_triangle(v1.b, v2.b, v3.b));
				gf3d_draw_sphere_solid(gfc_sphere(0,0,0,0.5), col.aPosition, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1,0,0,1), gfc_color(1,1,1,1));
				gf3d_draw_sphere_solid(gfc_sphere(0,0,0,0.5), col.bPosition, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(0,0,1,1), gfc_color(1,1,1,1));
				return col;
			}
		}
		// find support in direction of portal
		GFC_Vector3D newNormal = gfc_trigfc_angle_get_normal(gfc_triangle(v1.m, v2.m, v3.m));
		SupportPoint v4 = minkowskiPoint(a, b, newNormal);
		if(gfc_vector3d_dot_product(newNormal, v4.m) > 0.0) {
			// origin lies outside support plane, miss
			break;
		}
		GFC_Vector3D d;
		gfc_vector3d_sub(d, v1.m, v4.m);
		d = projectVector(d, newNormal);
		if(gfc_vector3d_magnitude_squared(d) < 0.01*0.01) {
			// support plane is too close to portal, miss
			break;
		}
		// choose new portal
		Bool p1 = triangleFacingOrigin(gfc_triangle(v0, v1.m, v4.m));
		Bool p2 = triangleFacingOrigin(gfc_triangle(v0, v2.m, v4.m));
		Bool p3 = triangleFacingOrigin(gfc_triangle(v0, v3.m, v4.m));
		if(p1 && !p2) {
			v3 = v4;
		} else if(!p1 && p3) {
			v2 = v4;
		} else if(p2 && !p3) {
			v1 = v4;
		} else {
			// floating point imprecision caused an invalid state
			// nothing we can do here, just return a miss
			break;
		}
		i++;
	}
	Collision col = {0};
	return col;
}

// return true if triangle's normal is facing the origin
inline Bool triangleFacingOrigin(GFC_Triangle3D triangle) {
	GFC_Vector3D normal = gfc_trigfc_angle_get_normal(triangle);
	return gfc_vector3d_dot_product(normal, triangle.a) <= 0.0;
}
