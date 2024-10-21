#include "gf3d_model.h"
#include "ice.h"
#include "moments_of_inertia.h"

PhysicsBody *createIce(GFC_Vector3D position, GFC_Vector3D rotation, GFC_Vector3D extents) {
	PhysicsBody *ice = physicsCreateBody();
	Shape boxShape;
	boxShape.shapeType = BOX;
	boxShape.shape.box.extents = extents;
	ice->shape = boxShape;
	Model *model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	ice->model = model;
	ice->friction = 0.2;
	ice->bounce = 0.8;
	ice->motionType = STATIC;
	calculateInertiaForBody(ice);
	gfc_matrix4f_scale(
		ice->visualTransform,
		ice->visualTransform,
		gfc_vector3d_to_float(extents)
	);
	ice->position = position;
	ice->rotation = rotation;
	ice->colorMod = gfc_color(0.2, 0.2, 1, 1);
	return ice;
}
