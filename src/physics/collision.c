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
	GFC_Vector3D v1 = minkowskiPoint(a, b, v0_n, NULL, NULL);
	GFC_Vector3D v1xv0;
	gfc_vector3d_cross_product(&v1xv0, v1, v0);
	gfc_vector3d_normalize(&v1xv0);
	if(gfc_vector3d_magnitude_squared(v1xv0) < 0.01) {
		v1xv0 = perpendicularVector3(v1);
	}
	GFC_Vector3D v2 = minkowskiPoint(a, b, v1xv0, NULL, NULL);
	GFC_Vector3D v0v2;
	gfc_vector3d_sub(v0v2, v2, v0);
	GFC_Vector3D v0v1;
	gfc_vector3d_sub(v0v1, v1, v0);
	GFC_Vector3D v0v2xv0v1;
	gfc_vector3d_cross_product(&v0v2xv0v1, v0v2, v0v1);
	gfc_vector3d_normalize(&v0v2xv0v1);
	GFC_Vector3D v3 = minkowskiPoint(a, b, v0v2xv0v1, NULL, NULL);
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	int i = 0;
	while(true) {
		Bool p1 = triangleFacingOrigin(gfc_triangle(v0, v2, v1));
		Bool p2 = triangleFacingOrigin(gfc_triangle(v0, v3, v2));
		Bool p3 = triangleFacingOrigin(gfc_triangle(v0, v1, v3));
		if(p1 && p2 && p3) {
			// valid candidate
			break;
		}
		// choose new candidate
		if(!p1) {
			// swap winding to get normal facing away from origin
			v3 = minkowskiPoint(a, b, gfc_trigfc_angle_get_normal(gfc_triangle(v0, v2, v1)), NULL, NULL);
			// preserve normal direction
			GFC_Vector3D tmp = v1;
			v1 = v2;
			v2 = tmp;
		}
		else if(!p2) {
			// swap winding to get normal facing away from origin
			v1 = minkowskiPoint(a, b, gfc_trigfc_angle_get_normal(gfc_triangle(v0, v3, v2)), NULL, NULL);
			// preserve normal direction
			GFC_Vector3D tmp = v3;
			v3 = v2;
			v2 = tmp;
		}
		else if(!p3) {
			// swap winding to get normal facing away from origin
			v2 = minkowskiPoint(a, b, gfc_trigfc_angle_get_normal(gfc_triangle(v0, v3, v1)), NULL, NULL);
			// preserve normal direction
			GFC_Vector3D tmp = v1;
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
	i = 0;
	while(true) {
		if(keys[SDL_SCANCODE_1+i]) {
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, .5), v0, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1,1,1,1), gfc_color(1,1,1,1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, .5), v1, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1,0,0,1), gfc_color(1,1,1,1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, .5), v2, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(0,1,0,1), gfc_color(1,1,1,1));
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, .5), v3, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(0,0,1,1), gfc_color(1,1,1,1));
		}
		if(triangleFacingOrigin(gfc_triangle(v1, v2, v3))) {
			// origin is inside portal
			slog("Hit!");
			break;
		}
		// find support in direction of portal
		GFC_Vector3D newNormal = gfc_trigfc_angle_get_normal(gfc_triangle(v1, v2, v3));
		GFC_Vector3D v4 = minkowskiPoint(a, b, newNormal, NULL, NULL);
		if(keys[SDL_SCANCODE_1+i]) {
			gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, .5), v4, gfc_vector3d(0,0,0), gfc_vector3d(1,1,1), gfc_color(1,0,1,1), gfc_color(1,1,1,1));
		}
		if(gfc_vector3d_dot_product(newNormal, v4) > 0.0) {
			// origin lies outside support plane
			slog("Miss! outside support plane");
			break;
		}
		GFC_Vector3D d;
		gfc_vector3d_sub(d, v1, v4);
		d = projectVector(d, newNormal);
		if(gfc_vector3d_magnitude_squared(d) < 0.01*0.01) {
			// support plane is too close to portal
			slog("Miss! too close.");
			break;
		}
		// choose new portal
		Bool p1 = triangleFacingOrigin(gfc_triangle(v0, v1, v4));
		Bool p2 = triangleFacingOrigin(gfc_triangle(v0, v2, v4));
		Bool p3 = triangleFacingOrigin(gfc_triangle(v0, v3, v4));
		if(p1 && !p2) {
			v3 = v4;
		} else if(!p1 && p3) {
			v2 = v4;
		} else if(p2 && !p3) {
			v1 = v4;
		} else {
			slog("i dont think this can happen?");
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
