#include "main_menu.h"
#include "physics.h"
#include "ui.h"
#include "test_scene.h"

static void playClicked(UIElement *button) {
	createTestScene();
}

void mainMenu() {
	physicsClear();
	clearUI();
	createPanel(gfc_vector2d(0, 0), gfc_vector2d(1280, 720), gfc_color(0.3, 0, 0, 1));
	UIElement *label = createLabel(gfc_vector2d(0, 0), gfc_vector2d(1000, 200));
	createButton(gfc_vector2d(1280/2, 720/2), gfc_vector2d(300, 100), "play")->click = playClicked;
	gfc_line_cpy(label->text, "julia's marble roller");
}
