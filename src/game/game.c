#include <time.h>
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
#include "moments_of_inertia.h"
#include "player.h"
#include "car.h"
#include "box.h"
#include "ball.h"
#include "floor.h"
#include "moving_platform.h"
#include "rotating_platform.h"
#include "jump_pad.h"
#include "treadmill.h"
#include "fan.h"
#include "powerup.h"
#include "util.h"

extern int __DEBUG;

void parse_arguments(int argc, char *argv[]);

double calculate_delta_time();

int main(int argc, char *argv[]) {
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
	srand(time(NULL));
	slog_sync();

	// game setup
	Model *sky = gf3d_model_load("assets/models/sky.model");
	GFC_Matrix4F skyMat;
	gfc_matrix4f_identity(skyMat);
	// camera
	gf3d_camera_set_scale(gfc_vector3d(1, 1, 1));
	gf3d_camera_set_position(gfc_vector3d(15, -15, 10));
	gf3d_camera_look_at(gfc_vector3d(0, 0, 0), NULL);
	gf3d_camera_set_move_step(10.0);
	gf3d_camera_set_rotate_step(2.0);

	gf3d_camera_enable_free_look(1);
	physicsStart(100);
	createPlayer();
	createBox()->position = gfc_vector3d(-24, 200, 10);
	createBox()->position = gfc_vector3d(0, 0, 20);
	createBall(gfc_vector3d(5, 10, 0));

	double floorSize = 100;
	double floorThickness = 5;
	createFloor(gfc_vector3d(0, 0, -floorSize/2.0), gfc_vector3d(0, 0, 0), gfc_vector3d(floorSize*4, floorSize*4, floorThickness));

	//createFloor(gfc_vector3d(floorSize, 0, 0), gfc_vector3d(0, -M_PI/4.0, 0), gfc_vector3d(floorSize, floorSize, floorThickness));
	createFloor(gfc_vector3d(-floorSize, 0, 0), gfc_vector3d(0, M_PI/4.0, 0), gfc_vector3d(floorSize, floorSize, floorThickness));
	createFloor(gfc_vector3d(0, floorSize, 0), gfc_vector3d(M_PI/4.0, 0, 0), gfc_vector3d(floorSize, floorSize, floorThickness));
	createFloor(gfc_vector3d(0, -floorSize, 0), gfc_vector3d(-M_PI/4.0, 0, 0), gfc_vector3d(floorSize, floorSize, floorThickness));

	createMovingPlatform(gfc_vector3d(-32, 4, -floorSize/2.0+4), gfc_vector3d(256, 0, 0), 10);
	createRotatingPlatform(gfc_vector3d(-32, 200, -floorSize/2.0+4), 4);
	createJumpPad(gfc_vector3d(-200, 0, -floorSize/2.0+4));
	createTreadmill(gfc_vector3d(0, -200, -floorSize/2.0+4));
	createFan(gfc_vector3d(200, 200, -floorSize/2.0+10), gfc_vector3d(0, 0, 1), 500);
	createFan(gfc_vector3d(-200, 200, -floorSize/2.0+10), gfc_vector3d(1, 0, 0), 500);
	createPowerup(gfc_vector3d(200, 0, -floorSize/2+floorThickness+8), CAR);
	createPowerup(gfc_vector3d(200, 30, -floorSize/2+floorThickness+8), SUPER_SPEED);
	createPowerup(gfc_vector3d(200, 60, -floorSize/2+floorThickness+8), SUPER_JUMP);
	createPowerup(gfc_vector3d(200, 90, -floorSize/2+floorThickness+8), BIG);
	// windows

	// main game loop
	Bool done = false;
	while(!done) {
		gfc_input_update();
		double delta = calculate_delta_time();
		gf2d_font_update();
		// camera updaes
		gf3d_camera_update_view();
		gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

		gf3d_vgraphics_render_start();

		physicsFrame(delta);
		// 3D draws
		gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, 1), gfc_vector3d(200, 200, 0), gfc_vector3d(0,0,0), gfc_vector3d(1, 1, 1), gfc_color(1, 1, 1, 1), gfc_color(1, 1, 1, 1));
		gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, 1), gfc_vector3d(-200, 200, 0), gfc_vector3d(0,0,0), gfc_vector3d(1, 1, 1), gfc_color(1, 1, 1, 1), gfc_color(1, 1, 1, 1));
		gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, 1), gfc_vector3d(200, -200, 0), gfc_vector3d(0,0,0), gfc_vector3d(1, 1, 1), gfc_color(1, 1, 1, 1), gfc_color(1, 1, 1, 1));
		gf3d_draw_sphere_solid(gfc_sphere(0, 0, 0, 1), gfc_vector3d(-200, -200, 0), gfc_vector3d(0,0,0), gfc_vector3d(1, 1, 1), gfc_color(1, 1, 1, 1), gfc_color(1, 1, 1, 1));
		drawPhysicsObjects();
		gf3d_model_draw_sky(sky, skyMat, GFC_COLOR_WHITE);
		render_outlines();
		// 2D draws
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
	static Uint64 now = 0;
	Uint64 then = now;
	now = SDL_GetTicks64();
	double delta = (double)(now - then) / 1000.0;
	//double fps = 1.0/MAX(delta, 0.00001);
	//slog("fps: %f",fps);
	return delta;
}

/*eol@eof*/
