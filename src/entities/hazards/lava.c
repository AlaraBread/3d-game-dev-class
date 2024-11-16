#include "lava.h"
#include "moments_of_inertia.h"

void lavaPhysicsProcess(PhysicsBody *self, double delta) {
	for(int i = 0; i < self->numReportedCollisions; i++) {
		Collision *col = &self->reportedCollisions[i];
		if(!col->hit) continue;
		PhysicsBody *other = col->a == self ? col->b : col->a;
		if(other->entityType == PLAYER) { resetPhysicsBody(other); }
	}
}

PhysicsBody *createLava(GFC_Vector3D extents) {
	PhysicsBody *lava = physicsCreateBody();
	Shape boxShape;
	boxShape.shapeType = BOX;
	boxShape.shape.box.extents = extents;
	lava->shape = boxShape;
	Model *model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	lava->model = model;
	lava->colorMod = gfc_color(1, 0, 0, 1);
	calculateInertiaForBody(lava);
	gfc_matrix4f_scale(lava->visualTransform, lava->visualTransform, gfc_vector3d_to_float(extents));
	lava->physicsProcess = lavaPhysicsProcess;
	return lava;
}
