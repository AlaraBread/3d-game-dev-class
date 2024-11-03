#include "ball.h"
#include "ball_enemy.h"
#include "box.h"
#include "car.h"
#include "cylinder_enemy.h"
#include "fan.h"
#include "finish.h"
#include "floor.h"
#include "gf2d_mouse.h"
#include "ice.h"
#include "jump_pad.h"
#include "lava.h"
#include "magnet.h"
#include "moments_of_inertia.h"
#include "moving_platform.h"
#include "physics.h"
#include "player.h"
#include "powerup.h"
#include "rotating_platform.h"
#include "treadmill.h"
#include "ui.h"
#include "cobweb.h"
#include "box_enemy.h"
#include "turret_enemy.h"

void createTestScene() {
	clearUI();
	createPlayer();
	gf2d_mouse_set_captured(true);
	createBox()->position = gfc_vector3d(-24, 200, 10);
	createBox()->position = gfc_vector3d(0, 0, 20);
	createBall(gfc_vector3d(5, 10, 0));

	const double floorSize = 100;
	const double floorThickness = 5;
	createFloor(
		gfc_vector3d(0, 0, -floorSize / 2.0), gfc_vector3d(0, 0, 0),
		gfc_vector3d(floorSize * 4, floorSize * 4, floorThickness)
	);

	// createFloor(gfc_vector3d(floorSize, 0, 0), gfc_vector3d(0, -M_PI/4.0, 0), gfc_vector3d(floorSize, floorSize,
	// floorThickness));
	createFloor(
		gfc_vector3d(-floorSize, 0, 0), gfc_vector3d(0, M_PI / 4.0, 0),
		gfc_vector3d(floorSize, floorSize, floorThickness)
	);
	createFloor(
		gfc_vector3d(0, floorSize, 0), gfc_vector3d(M_PI / 4.0, 0, 0),
		gfc_vector3d(floorSize, floorSize, floorThickness)
	);
	createFloor(
		gfc_vector3d(0, -floorSize, 0), gfc_vector3d(-M_PI / 4.0, 0, 0),
		gfc_vector3d(floorSize, floorSize, floorThickness)
	);

	createIce(
		gfc_vector3d(floorSize * 4, floorSize * 4, -floorSize / 2.0), gfc_vector3d(0, 0, 0), gfc_vector3d(200, 200, 4)
	);
	createLava(
		gfc_vector3d(floorSize * 4 + 10, floorSize * 4, -floorSize / 2.0 + 30), gfc_vector3d(0, 0, 0),
		gfc_vector3d(10, 10, 10)
	);
	createCobweb(
		gfc_vector3d(floorSize * 4 + 40, floorSize * 4, -floorSize / 2.0), gfc_vector3d(0, 0, 0),
		gfc_vector3d(10, 10, 10)
	);

	createMovingPlatform(gfc_vector3d(-32, 4, -floorSize / 2.0 + 4), gfc_vector3d(256, 0, 0), 10);
	createRotatingPlatform(gfc_vector3d(-32, 200, -floorSize / 2.0 + 4), 4);
	createJumpPad(gfc_vector3d(-200, 0, -floorSize / 2.0 + 4));
	createTreadmill(gfc_vector3d(0, -200, -floorSize / 2.0 + 4));
	createFan(gfc_vector3d(200, 200, -floorSize / 2.0 + 10), gfc_vector3d(0, 0, 1), 500);
	createFan(gfc_vector3d(-200, 200, -floorSize / 2.0 + 10), gfc_vector3d(-1, 0, 0), 500);
	createMagnet(gfc_vector3d(-200, -200, -floorSize / 2.0 + 30), 75);
	createPowerup(gfc_vector3d(200, 0, -floorSize / 2 + floorThickness + 8), CAR);
	createPowerup(gfc_vector3d(200, 30, -floorSize / 2 + floorThickness + 8), SUPER_SPEED);
	createPowerup(gfc_vector3d(200, 60, -floorSize / 2 + floorThickness + 8), SUPER_JUMP);
	createPowerup(gfc_vector3d(200, 90, -floorSize / 2 + floorThickness + 8), BIG);
	createPowerup(gfc_vector3d(200, 120, -floorSize / 2 + floorThickness + 8), SLOW);

	createFinish(gfc_vector3d(100, 100, -floorSize / 2 + floorThickness + 8));

	createCylinderEnemy(gfc_vector3d(100, 0.1, 0));
	createBallEnemy(gfc_vector3d(101.1, 0.1, 0));
	createBoxEnemy(gfc_vector3d(100, 30, 0));
	createTurretEnemy(gfc_vector3d(100, -100, -floorSize / 2 + floorThickness + 4));
}
