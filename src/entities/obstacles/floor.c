#include "gf3d_model.h"
#include "floor.h"
#include "moments_of_inertia.h"

PhysicsBody *createFloor(GFC_Vector3D position, GFC_Vector3D rotation, GFC_Vector3D extents) {
	PhysicsBody *floor = physicsCreateBody();
	Shape boxShape;
	boxShape.shapeType = BOX;
	boxShape.shape.box.extents = extents;
	floor->shape = boxShape;
	Model *model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	floor->model = model;
	floor->friction = 1.0;
	floor->bounce = 0.8;
	floor->motionType = STATIC;
	calculateInertiaForBody(floor);
	gfc_matrix4f_scale(
		floor->visualTransform,
		floor->visualTransform,
		gfc_vector3d_to_float(extents)
	);
	floor->position = position;
	floor->rotation = rotation;
	return floor;
}
