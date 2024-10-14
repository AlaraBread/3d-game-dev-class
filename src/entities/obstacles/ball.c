#include "gf3d_model.h"
#include "box.h"
#include "moments_of_inertia.h"

PhysicsBody *createBall(GFC_Vector3D position) {
	PhysicsBody *ball = physicsCreateBody();
	ball->position = position;
	Shape shape;
	shape.shapeType = SPHERE;
	shape.shape.sphere.radius = 6;
	ball->shape = shape;
	Model *model = gf3d_model_load("assets/models/test_sphere/test_sphere.model");
	ball->model = model;
	ball->mass = 0.004;
	ball->friction = 0.8;
	ball->bounce = 0.4;
	ball->visualScale = gfc_vector3d(shape.shape.sphere.radius, shape.shape.sphere.radius, shape.shape.sphere.radius);
	calculateInertiaForBody(ball);
	return ball;
}
