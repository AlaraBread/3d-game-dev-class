#include "main_menu.h"
#include "physics.h"
#include "ui.h"
#include "util.h"

static void mainMenuPressed(UIElement *button) {
	setTimeScale(1.0);
	mainMenu();
}

extern GameMode g_gamemode;
extern UIElement *g_timer;
void createEndMenu() {
	double time = g_timer ? g_timer->timer : -1;
	clearUI();
	createPanel(gfc_vector2d(0, 0), gfc_vector2d(1280, 720), gfc_color(0, 0, 0, 0.5));
	UIElement *label = createLabel(gfc_vector2d(0, 0), gfc_vector2d(1280, 100));
	gfc_line_cpy(label->text, "you win");
	if(g_gamemode == TIME) {
		UIElement *label = createLabel(gfc_vector2d(0, 200), gfc_vector2d(1280, 100));
		sprintf(label->text, "level completed in  %02.0lf : %02.2lf", floor(time / 60.0), wrapMax(time, 60));
	}
	createButton(gfc_vector2d(1280 / 2, 720 / 2), gfc_vector2d(300, 100), "main menu")->click = mainMenuPressed;
}
