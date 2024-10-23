#include "fan.h"
#include "gf3d_draw.h"
#include "gf3d_model.h"
#include "util.h"

void fanPhysicsProcess(PhysicsBody *self, double delta) {
	gfc_matrix4f_rotate_y(self->visualTransform, self->visualTransform, delta * 4);
	for(int i = 0; i < self->numReportedCollisions; i++) {
		Collision *col = &self->reportedCollisions[i];
		PhysicsBody *other = col->a == self ? col->b : col->a;
		GFC_Vector3D force = gfc_vector3d(delta * self->entity.platform.movementSpeed, 0, 0);
		rotate_vector3_by_euler_vector(&force, self->rotation);
		if(other->motionType == DYNAMIC) { gfc_vector3d_add(other->linearVelocity, other->linearVelocity, force); }
	}
}

#define FAN_WIDTH 30
#define FAN_DIST 100

PhysicsBody *createFan(GFC_Vector3D position, GFC_Vector3D direction, float speed) {
	gfc_vector3d_normalize(&direction);
	GFC_Vector3D p = position;
	p.z = 0;
	gf3d_draw_edge_3d(
		gfc_edge3d_from_vectors(gfc_vector3d(0, 0, 0), direction), p, gfc_vector3d(0, 0, 0), gfc_vector3d(10, 10, 10),
		0.1, gfc_color(1, 1, 0, 1)
	);
	PhysicsBody *fan = physicsCreateBody();
	GFC_Vector3D offset = {0};
	gfc_vector3d_scale(offset, direction, FAN_DIST);
	gfc_vector3d_add(fan->position, position, offset);
	fan->model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	fan->motionType = TRIGGER;
	GFC_Vector4D q = get_rotation_between(direction, gfc_vector3d(1, 0, 0));
	quat_to_euler_vector(&fan->rotation, q);
	gfc_vector3d_negate(fan->rotation, fan->rotation);
	gfc_matrix4f_scale(fan->visualTransform, fan->visualTransform, gfc_vector3df(1, FAN_WIDTH, FAN_WIDTH));
	gfc_matrix4f_translate(fan->visualTransform, fan->visualTransform, gfc_vector3df(-FAN_DIST, 0, 0));
	fan->shape.shapeType = BOX;
	fan->shape.shape.box.extents = gfc_vector3d(FAN_DIST, FAN_WIDTH, FAN_WIDTH);
	fan->physicsProcess = fanPhysicsProcess;
	fan->entity.platform.movementSpeed = speed;
	return fan;
}
