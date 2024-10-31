#include "moments_of_inertia.h"
#include "physics.h"
#include "simple_logger.h"
#include "util.h"

PhysicsBody *createTreadmill(GFC_Vector3D position) {
	PhysicsBody *platform = physicsCreateBody();
	platform->position = position;
	platform->entityType = PLATFORM;
	Shape s;
	s.shapeType = BOX;
	s.shape.box.extents = gfc_vector3d(30, 30, 2);
	platform->shape = s;
	platform->model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	gfc_matrix4f_scale(
		platform->visualTransform, platform->visualTransform, gfc_vector3d_to_float(s.shape.box.extents)
	);
	platform->motionType = STATIC;
	platform->friction = 1.0;
	calculateInertiaForBody(platform);
	platform->linearVelocity = gfc_vector3d(100, 0, 0);
	return platform;
}
