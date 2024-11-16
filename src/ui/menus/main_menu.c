#include "main_menu.h"
#include "enemy_counter.h"
#include "physics.h"
#include "scene_loader.h"
#include "timer.h"
#include "ui.h"

GameMode g_gamemode = TIME;
static void playCompletion(UIElement *button) {
	g_gamemode = COMPLETION;
	loadScene("assets/scenes/level1.gltf");
	createEnemyCounter();
}

static void playTime(UIElement *button) {
	g_gamemode = TIME;
	loadScene("assets/scenes/level1.gltf");
	createTimer();
}

void mainMenu() {
	physicsClear();
	clearUI();
	createPanel(gfc_vector2d(0, 0), gfc_vector2d(1280, 720), gfc_color(0.3, 0, 0, 1));
	UIElement *label = createLabel(gfc_vector2d(0, 0), gfc_vector2d(1000, 200));
	createButton(gfc_vector2d(1280 / 2, 720 / 2), gfc_vector2d(300, 100), "completion mode")->click = playCompletion;
	createButton(gfc_vector2d(1280 / 2, 720 / 2 + 150), gfc_vector2d(300, 100), "time mode")->click = playTime;
	gfc_line_cpy(label->text, "julia's marble roller");
}
