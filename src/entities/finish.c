#include "main_menu.h"
#include "gf2d_mouse.h"
#include "end_menu.h"
#include "finish.h"

extern GameMode g_gamemode;
extern int g_numEnemies;
void finishPhysicsProcess(PhysicsBody *finish, double delta) {
	for(int i = 0; i < finish->numReportedCollisions; i++) {
		Collision *col = &finish->reportedCollisions[i];
		PhysicsBody *other = finish == col->a ? col->b : col->a;
		if(other->entityType == PLAYER && !other->entity.player.done && !(g_gamemode == COMPLETION && g_numEnemies > 0)) {
			other->entity.player.done = true;
			gf2d_mouse_set_captured(false);
			setTimeScale(0.1);
			createEndMenu();
		}
	}
}

PhysicsBody *createFinish(GFC_Vector3D position) {
	PhysicsBody *finish = physicsCreateBody();
	finish->position = position;
	finish->motionType = TRIGGER;
		Shape boxShape;
	boxShape.shapeType = BOX;
	boxShape.shape.box.extents = gfc_vector3d(6, 6, 6);
	finish->shape = boxShape;
	Model *model = gf3d_model_load("assets/models/test_cube/test_cube.model");
	finish->model = model;
	finish->physicsProcess = finishPhysicsProcess;
	gfc_matrix4f_scale(finish->visualTransform, finish->visualTransform, gfc_vector3d_to_float(boxShape.shape.box.extents));
	finish->colorMod = gfc_color(1, 1, 1, 1.0);
	finish->colorMod.r = 3;
	finish->colorMod.g = 3;
	finish->colorMod.b = 3;
	return finish;
}
