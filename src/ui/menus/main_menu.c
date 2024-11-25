#include "main_menu.h"
#include "enemy_counter.h"
#include "physics.h"
#include "scene_loader.h"
#include "timer.h"
#include "ui.h"

static void level1(UIElement *button) { loadScene("assets/scenes/level1.gltf"); }
static void level2(UIElement *button) { loadScene("assets/scenes/level2.gltf"); }
static void level3(UIElement *button) { loadScene("assets/scenes/level2.gltf"); }

void mainMenu() {
	physicsClear();
	clearUI();
	createPanel(gfc_vector2d(0, 0), gfc_vector2d(1280, 720), gfc_color(0.3, 0, 0, 1));
	UIElement *label = createLabel(gfc_vector2d(0, 0), gfc_vector2d(1000, 200));
	createButton(gfc_vector2d(1280 / 2, 720 / 2), gfc_vector2d(300, 100), "level 1")->click = level1;
	createButton(gfc_vector2d(1280 / 2, 720 / 2 + 150), gfc_vector2d(300, 100), "level 2")->click = level2;
	createButton(gfc_vector2d(1280 / 2, 720 / 2 + 300), gfc_vector2d(300, 100), "level 3")->click = level3;
	gfc_line_cpy(label->text, "julia's marble roller");
}
