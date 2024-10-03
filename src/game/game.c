#include <SDL.h>

#include "simple_json.h"
#include "simple_logger.h"

#include "gfc_actions.h"
#include "gfc_audio.h"
#include "gfc_config_def.h"
#include "gfc_input.h"
#include "gfc_matrix.h"
#include "gfc_string.h"
#include "gfc_vector.h"

#include "gf2d_actor.h"
#include "gf2d_draw.h"
#include "gf2d_font.h"
#include "gf2d_mouse.h"
#include "gf2d_sprite.h"

#include "gf3d_camera.h"
#include "gf3d_draw.h"
#include "gf3d_model.h"
#include "gf3d_outline.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_texture.h"
#include "gf3d_vgraphics.h"

#include "physics.h"

extern int __DEBUG;

void parse_arguments(int argc, char *argv[]);

void draw_origin() {
	gf3d_draw_edge_3d(
		gfc_edge3d_from_vectors(gfc_vector3d(-100, 0, 0), gfc_vector3d(100, 0, 0)), gfc_vector3d(0, 0, 0),
		gfc_vector3d(0, 0, 0), gfc_vector3d(1, 1, 1), 0.1, gfc_color(1, 0, 0, 1)
	);
	gf3d_draw_edge_3d(
		gfc_edge3d_from_vectors(gfc_vector3d(0, -100, 0), gfc_vector3d(0, 100, 0)), gfc_vector3d(0, 0, 0),
		gfc_vector3d(0, 0, 0), gfc_vector3d(1, 1, 1), 0.1, gfc_color(0, 1, 0, 1)
	);
	gf3d_draw_edge_3d(
		gfc_edge3d_from_vectors(gfc_vector3d(0, 0, -100), gfc_vector3d(0, 0, 100)), gfc_vector3d(0, 0, 0),
		gfc_vector3d(0, 0, 0), gfc_vector3d(1, 1, 1), 0.1, gfc_color(0, 0, 1, 1)
	);
}

double calculate_delta_time();

void controlledThink(PhysicsBody *self) {
	float d = 0.1;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if(keys[SDL_SCANCODE_I]) { self->linearVelocity.y -= d; }
	if(keys[SDL_SCANCODE_J]) { self->linearVelocity.x += d; }
	if(keys[SDL_SCANCODE_K]) { self->linearVelocity.y += d; }
	if(keys[SDL_SCANCODE_L]) { self->linearVelocity.x -= d; }
	if(keys[SDL_SCANCODE_U]) { self->linearVelocity.z += d; }
	if(keys[SDL_SCANCODE_O]) { self->linearVelocity.z -= d; }
	if(keys[SDL_SCANCODE_B]) { self->angularVelocity.x += 0.05; }
	if(keys[SDL_SCANCODE_N]) { self->angularVelocity.y += 0.05; }
	if(keys[SDL_SCANCODE_M]) { self->angularVelocity.z += 0.05; }
}

int main(int argc, char *argv[]) {
	// local variables
	Model *sky, *testSphere;
	GFC_Matrix4 skyMat, dinoMat;
	// initializtion
	parse_arguments(argc, argv);
	init_logger("gf3d.log", 0);
	slog("gf3d begin");
	// gfc init
	gfc_input_init("assets/config/input.cfg");
	gfc_config_def_init();
	gfc_action_init(1024);
	// gf3d init
	gf3d_vgraphics_init("assets/config/setup.cfg");
	gf3d_materials_init();
	gf2d_font_init("assets/config/font.cfg");
	gf2d_actor_init(1000);
	gf3d_draw_init();			  // 3D
	gf2d_draw_manager_init(1000); // 2D

	// game init
	srand(SDL_GetTicks());
	slog_sync();

	// game setup
	gf2d_mouse_load("assets/actors/mouse.actor");
	sky = gf3d_model_load("assets/models/sky.model");
	gfc_matrix4_identity(skyMat);
	testSphere = gf3d_model_load("assets/models/test_sphere/test_sphere.model");
	gfc_matrix4_identity(dinoMat);
	// camera
	gf3d_camera_set_scale(gfc_vector3d(1, 1, 1));
	gf3d_camera_set_position(gfc_vector3d(15, -15, 10));
	gf3d_camera_look_at(gfc_vector3d(0, 0, 0), NULL);
	gf3d_camera_set_move_step(10.0);
	gf3d_camera_set_rotate_step(2.0);

	gf3d_camera_enable_free_look(1);
	physicsStart(10);
	Shape s;
	s.shapeType = SPHERE;
	s.shape.sphere.radius = 4.0;
	PhysicsBody *a = physicsCreateBody();
	a->think = controlledThink;
	a->shape = s;
	a->model = testSphere;
	PhysicsBody *b = physicsCreateBody();
	b->shape = s;
	b->model = testSphere;
	b->position.x = 10;
	// b->angularVelocity = gfc_vector3d(0, 1, 0);
	// a->angularVelocity = gfc_vector3d(4, 0, 3);

	// windows

	// main game loop
	Bool done = false;
	while(!done) {
		gfc_input_update();
		gf2d_mouse_update();
		double delta = calculate_delta_time();
		gf2d_font_update();
		// camera updaes
		gf3d_camera_controls_update(delta);
		gf3d_camera_update_view();
		gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

		gf3d_vgraphics_render_start();
		physicsUpdate(delta);

		// 3D draws
		drawPhysicsObjects();
		gf3d_model_draw_sky(sky, skyMat, GFC_COLOR_WHITE);
		draw_origin();
		render_outlines();
		// 2D draws
		gf2d_mouse_draw();
		gf2d_font_draw_line_tag("ALT+F4 to exit", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(10, 10));
		gf3d_vgraphics_render_end();
		slog_sync();
		if(gfc_input_command_down("exit")) done = true; // exit condition
	}
	vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());
	// cleanup
	slog("gf3d program end");
	exit(0);
	slog_sync();
	return 0;
}

void parse_arguments(int argc, char *argv[]) {
	int a;

	for(a = 1; a < argc; a++) {
		if(strcmp(argv[a], "--debug") == 0) { __DEBUG = 1; }
	}
}

double calculate_delta_time() {
	static Uint64 now;
	Uint64 then = now;
	now = SDL_GetTicks64();
	double delta = (double)(now - then) / 1000.0;
	// double fps = 1.0/MAX(delta, 0.00001);
	// slog("fps: %f",fps);
	return delta;
}

/*eol@eof*/
