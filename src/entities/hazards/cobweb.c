#include "cobweb.h"
#include "moments_of_inertia.h"

void cobwebPhysicsProcess(PhysicsBody *self, double delta) {
	for(int i = 0; i < self->numReportedCollisions; i++) {
		Collision *col = &self->reportedCollisions[i];
		if(!col->hit) continue;
		PhysicsBody *other = col->a == self ? col->b : col->a;
		if(other->motionType == DYNAMIC) {
			// linear damp
			GFC_Vector3D linearDampVector;
			gfc_vector3d_scale(linearDampVector, other->linearVelocity, delta * 10);
			gfc_vector3d_sub(other->linearVelocity, other->linearVelocity, linearDampVector);
			// angular damp
			GFC_Vector3D angularDampVector;
			gfc_vector3d_scale(angularDampVector, other->angularVelocity, delta * 20);
			gfc_vector3d_sub(other->angularVelocity, other->angularVelocity, angularDampVector);
		}
	}
}

PhysicsBody *createCobweb(GFC_Vector3D extents) {
	PhysicsBody *cobweb = physicsCreateBody();
	Shape boxShape;
	boxShape.shapeType = BOX;
	boxShape.shape.box.extents = extents;
	cobweb->shape = boxShape;
	Model *model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	cobweb->model = model;
	cobweb->colorMod = gfc_color(0, 0.5, 1, 1);
	calculateInertiaForBody(cobweb);
	gfc_matrix4f_scale(cobweb->visualTransform, cobweb->visualTransform, gfc_vector3d_to_float(extents));
	cobweb->physicsProcess = cobwebPhysicsProcess;
	return cobweb;
}
