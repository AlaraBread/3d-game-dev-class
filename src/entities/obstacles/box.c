#include "gf3d_model.h"
#include "box.h"
#include "moments_of_inertia.h"

PhysicsBody *createBox() {
	PhysicsBody *box = physicsCreateBody();
	Shape boxShape;
	boxShape.shapeType = BOX;
	boxShape.shape.box.extents = gfc_vector3d(6, 6, 6);
	box->shape = boxShape;
	Model *model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	box->model = model;
	box->mass = 0.01;
	box->friction = 0.8;
	box->bounce = 0.4;
	box->motionType = DYNAMIC;
	calculateInertiaForBody(box);
	gfc_matrix4f_scale(
		box->visualTransform,
		box->visualTransform,
		gfc_vector3d_to_float(boxShape.shape.box.extents)
	);
	return box;
}
