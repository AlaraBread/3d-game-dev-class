#include "physics.h"
#include "ui.h"
#include "main_menu.h"

static void mainMenuPressed(UIElement *button) {
	setTimeScale(1.0);
	mainMenu();
}

void createEndMenu() {
	clearUI();
	createPanel(gfc_vector2d(0, 0), gfc_vector2d(1280, 720), gfc_color(0, 0, 0, 0.5));
	UIElement *label = createLabel(gfc_vector2d(0, 0), gfc_vector2d(1280, 100));
	gfc_line_cpy(label->text, "you win");
	createButton(gfc_vector2d(1280/2, 720/2), gfc_vector2d(300, 100), "main menu")->click = mainMenuPressed;
}
