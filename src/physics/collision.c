#include "collision.h"
#include "gf3d_draw.h"
#include "gfc_hashmap.h"
#include "intersection_tests.h"
#include "lists.h"
#include "shapes.h"
#include "simple_logger.h"
#include "util.h"

Collision mpr(PhysicsBody *a, PhysicsBody *b);

Collision doCollision(PhysicsBody *a, PhysicsBody *b) {
	switch(a->shape.shapeType) {
		case SPHERE: {
			switch(b->shape.shapeType) {
				case SPHERE:
					return sphereSphereIntersectionTest(a, b);
				default:
					break;
			}
			break;
		}
		default:
			break;
	}
	return mpr(a, b);
}

inline Bool triangleFacingOrigin(GFC_Triangle3D triangle);

#define DEBUG_MPR_EXPANSION 0
#define DEBUG_MPR_REFINEMENT 0

// as described in game programming gems 7 by Gary Snethen
Collision mpr(PhysicsBody *a, PhysicsBody *b) {
#if DEBUG_MPR_EXPANSION || DEBUG_MPR_REFINEMENT
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	Bool pressingAButton = false;
	for(int l = 0; l < 9; l++) {
		if(keys[SDL_SCANCODE_1+l]) {
			pressingAButton = true;
			break;
		}
	}
#endif
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
	if(gfc_vector3d_magnitude_squared(v1xv0) < 0.01) { v1xv0 = perpendicularVector3(v1.m); }
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
#if DEBUG_MPR_EXPANSION
		if(keys[SDL_SCANCODE_1+i] || (i == 0 && !pressingAButton)) {
			float r = 0.25;
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v0, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1, 1, 1, 1), gfc_color(1, 1, 1, 1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v1.m, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1, 0, 0, 1), gfc_color(1, 1, 1, 1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v2.m, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(0, 1, 0, 1), gfc_color(1, 1, 1, 1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v3.m, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(0, 0, 1, 1), gfc_color(1, 1, 1, 1));

			GFC_Vector3D c = triangleCenter(gfc_triangle(v1.m, v2.m, v3.m));
			gf3d_draw_edge_3d(gfc_edge3d(c.x, c.y, c.z, v0.x, v0.y, v0.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(1,1,0,1));
			gf3d_draw_edge_3d(gfc_edge3d(0,0,0, v0.x, v0.y, v0.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(1,1,0,1));
			gf3d_draw_edge_3d(gfc_edge3d(v1.m.x, v1.m.y, v1.m.z, v2.m.x, v2.m.y, v2.m.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(0,0,0,1));
			gf3d_draw_edge_3d(gfc_edge3d(v2.m.x, v2.m.y, v2.m.z, v3.m.x, v3.m.y, v3.m.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(0,0,0,1));
			gf3d_draw_edge_3d(gfc_edge3d(v1.m.x, v1.m.y, v1.m.z, v3.m.x, v3.m.y, v3.m.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(0,0,0,1));
			printf("p1=%d\np2=%d\np3=%d\n", p1, p2, p3);
		}
#endif
		if(p1 && p2 && p3) {
			// valid candidate
			break;
		}
		// choose new candidate
		if(!p1) {
			// swap winding to get normal facing away from origin
			GFC_Triangle3D t = gfc_triangle(v0, v1.m, v2.m);
			GFC_Vector3D normal = gfc_trigfc_angle_get_normal(t);
			v3 = minkowskiPoint(a, b, normal);
#if DEBUG_MPR_EXPANSION
			if(keys[SDL_SCANCODE_1+i] || (i == 0 && !pressingAButton)) {
				GFC_Vector3D c = triangleCenter(t);
				gf3d_draw_edge_3d(gfc_edge3d(c.x, c.y, c.z, v3.m.x, v3.m.y, v3.m.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(0,0,1,1));
				gf3d_draw_edge_3d(gfc_edge3d(c.x, c.y, c.z, normal.x, normal.y, normal.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.25, gfc_color(0,0,1,1));
				gf3d_draw_edge_3d(gfc_edge3d(t.a.x, t.a.y, t.a.z, t.b.x, t.b.y, t.b.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(1,0,1,1));
				gf3d_draw_edge_3d(gfc_edge3d(t.b.x, t.b.y, t.b.z, t.c.x, t.c.y, t.c.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(1,0,1,1));
				gf3d_draw_edge_3d(gfc_edge3d(t.a.x, t.a.y, t.a.z, t.c.x, t.c.y, t.c.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(1,0,1,1));
			}
#endif
			// preserve normal direction
			SupportPoint tmp = v1;
			v1 = v2;
			v2 = tmp;
		} else if(!p2) {
			// swap winding to get normal facing away from origin
			GFC_Triangle3D t = gfc_triangle(v0, v2.m, v3.m);
			GFC_Vector3D normal = gfc_trigfc_angle_get_normal(t);
			v1 = minkowskiPoint(a, b, normal);
#if DEBUG_MPR_EXPANSION
			if(keys[SDL_SCANCODE_1+i] || (i == 0 && !pressingAButton)) {
				GFC_Vector3D c = triangleCenter(t);
				gf3d_draw_edge_3d(gfc_edge3d(c.x, c.y, c.z, v1.m.x, v1.m.y, v1.m.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(1,0,0,1));
				gf3d_draw_edge_3d(gfc_edge3d(c.x, c.y, c.z, normal.x, normal.y, normal.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.25, gfc_color(1,0,0,1));
				gf3d_draw_edge_3d(gfc_edge3d(t.a.x, t.a.y, t.a.z, t.b.x, t.b.y, t.b.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(1,0,1,1));
				gf3d_draw_edge_3d(gfc_edge3d(t.b.x, t.b.y, t.b.z, t.c.x, t.c.y, t.c.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(1,0,1,1));
				gf3d_draw_edge_3d(gfc_edge3d(t.a.x, t.a.y, t.a.z, t.c.x, t.c.y, t.c.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(1,0,1,1));
			}
#endif
			// preserve normal direction
			SupportPoint tmp = v3;
			v3 = v2;
			v2 = tmp;
		} else if(!p3) {
			// swap winding to get normal facing away from origin
			GFC_Triangle3D t = gfc_triangle(v0, v3.m, v1.m);
			GFC_Vector3D normal = gfc_trigfc_angle_get_normal(t);
			v2 = minkowskiPoint(a, b, normal);
#if DEBUG_MPR_EXPANSION
			if(keys[SDL_SCANCODE_1+i] || (i == 0 && !pressingAButton)) {
				GFC_Vector3D c = triangleCenter(t);
				gf3d_draw_edge_3d(gfc_edge3d(c.x, c.y, c.z, v2.m.x, v2.m.y, v2.m.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(0,1,0,1));
				gf3d_draw_edge_3d(gfc_edge3d(c.x, c.y, c.z, normal.x, normal.y, normal.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.25, gfc_color(0,1,0,1));
				gf3d_draw_edge_3d(gfc_edge3d(t.a.x, t.a.y, t.a.z, t.b.x, t.b.y, t.b.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(1,0,1,1));
				gf3d_draw_edge_3d(gfc_edge3d(t.b.x, t.b.y, t.b.z, t.c.x, t.c.y, t.c.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(1,0,1,1));
				gf3d_draw_edge_3d(gfc_edge3d(t.a.x, t.a.y, t.a.z, t.c.x, t.c.y, t.c.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(1,0,1,1));
			}
#endif
			// preserve normal direction
			SupportPoint tmp = v1;
			v1 = v3;
			v3 = tmp;
		}
		i++;
		if(i > 50) {
			if(DEBUG_MPR_EXPANSION) slog("took too long to find portal");
			Collision col = {0};
			return col;
		}
	}
#if DEBUG_MPR_EXPANSION
	slog("found portal");
	Collision col = {0};
	return col;
#endif
	// portal refinement
	int collisionAccuracy = 6;
	i = 0;
	while(true) {
		if(triangleFacingOrigin(gfc_triangle(v1.m, v2.m, v3.m))) {
			// origin is inside portal, hit
			// keep going for a little bit to get more accurate collision
			collisionAccuracy--;
			if(collisionAccuracy <= 0) {
				if(DEBUG_MPR_REFINEMENT) slog("hit!");
				Collision col;
				col.hit = true;
				GFC_Triangle3D mTriangle = gfc_triangle(v1.m, v3.m, v2.m);
				col.normal = gfc_trigfc_angle_get_normal(mTriangle);
				col.penetrationDepth = gfc_vector3d_dot_product(col.normal, v1.m);
				GFC_Vector3D bary = toBarycentric(gfc_vector3d(0,0,0), mTriangle);
				col.aPosition = fromBarycentric(bary, gfc_triangle(v1.a, v3.a, v2.a));
				col.bPosition = fromBarycentric(bary, gfc_triangle(v1.b, v3.b, v2.b));
				return col;
			}
		}
		// find support in direction of portal
		GFC_Vector3D newNormal = gfc_trigfc_angle_get_normal(gfc_triangle(v1.m, v3.m, v2.m));
		SupportPoint v4 = minkowskiPoint(a, b, newNormal);
#if DEBUG_MPR_REFINEMENT
		if(keys[SDL_SCANCODE_1+i] || (i == 0 && !pressingAButton)) {
			float r = 0.25;
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v0, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1, 1, 1, 1), gfc_color(1, 1, 1, 1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v1.m, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1, 0, 0, 1), gfc_color(1, 1, 1, 1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v2.m, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(0, 1, 0, 1), gfc_color(1, 1, 1, 1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v3.m, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(0, 0, 1, 1), gfc_color(1, 1, 1, 1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v4.m, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1, 0, 1, 1), gfc_color(1, 1, 1, 1));

			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v1.a, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1, 0, 0, 1), gfc_color(1, 1, 1, 1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v2.a, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(0, 1, 0, 1), gfc_color(1, 1, 1, 1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v3.a, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(0, 0, 1, 1), gfc_color(1, 1, 1, 1));

			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v1.b, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1, 0, 0, 1), gfc_color(1, 1, 1, 1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v2.b, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(0, 1, 0, 1), gfc_color(1, 1, 1, 1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), v3.b, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(0, 0, 1, 1), gfc_color(1, 1, 1, 1));

			gf3d_draw_edge_3d(gfc_edge3d(0,0,0, v0.x, v0.y, v0.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(1,1,0,1));
			gf3d_draw_edge_3d(gfc_edge3d(v1.m.x, v1.m.y, v1.m.z, v2.m.x, v2.m.y, v2.m.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(0,0,0,1));
			gf3d_draw_edge_3d(gfc_edge3d(v2.m.x, v2.m.y, v2.m.z, v3.m.x, v3.m.y, v3.m.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(0,0,0,1));
			gf3d_draw_edge_3d(gfc_edge3d(v1.m.x, v1.m.y, v1.m.z, v3.m.x, v3.m.y, v3.m.z), gfc_vector3d(0,0,0), gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), 0.125, gfc_color(0,0,0,1));

			//GFC_Vector3D v1o; // from v1 to origin
			//gfc_vector3d_negate(v1o, v1.m);
			GFC_Triangle3D t = gfc_triangle(v1.m, v3.m, v2.m);
			//GFC_Vector3D v1op = projectVectorOntoPlane(v1o, gfc_trigfc_angle_get_normal(t)); // a to origin, projected onto triangle
			GFC_Vector3D bary = toBarycentric(gfc_vector3d(0,0,0), t);
			GFC_Vector3D aContact = fromBarycentric(bary, gfc_triangle(v1.a, v3.a, v2.a));
			GFC_Vector3D bContact = fromBarycentric(bary, gfc_triangle(v1.b, v3.b, v2.b));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), aContact, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1, 1, 0, 1), gfc_color(1, 1, 1, 1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, r), bContact, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(0, 1, 1, 1), gfc_color(1, 1, 1, 1));
		}
#endif
		if(gfc_vector3d_dot_product(newNormal, v4.m) < 0.0) {
			// origin lies outside support plane, miss
			if(DEBUG_MPR_REFINEMENT) slog("origin lies outside support plane, miss");
			break;
		}
		GFC_Vector3D d;
		gfc_vector3d_sub(d, v1.m, v4.m);
		d = projectVector(d, newNormal);
		/*float e = 0.001;
		if(gfc_vector3d_magnitude_squared(d) < e * e) {
			// support plane is too close to portal, miss
			slog("support plane is too close to portal, miss");
			break;
		}*/
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
			if(DEBUG_MPR_REFINEMENT) slog("floating point imprecision caused an invalid state");
			printf("p1=%d\np2=%d\np3=%d\n", p1, p2, p3);
			break;
		}
		i++;
		if(i > 40) {
			if(DEBUG_MPR_REFINEMENT) slog("refined for too long");
			break;
		}
	}
	Collision col = {0};
	return col;
}

// return true if triangle's normal is facing the origin
inline Bool triangleFacingOrigin(GFC_Triangle3D triangle) {
	GFC_Vector3D normal = gfc_trigfc_angle_get_normal(triangle);
	return gfc_vector3d_dot_product(normal, triangle.a) <= 0.0;
}
