#include "pause.h"
#include "gf2d_mouse.h"
#include "main_menu.h"
#include "physics.h"
#include "ui.h"

void createPauseMenu();

Bool g_paused = false;
void pause(Bool paused) {
	g_paused = paused;
	if(paused) {
		gf2d_mouse_set_captured(false);
		createPauseMenu();
	} else {
		gf2d_mouse_set_captured(true);
		clearUIGroup(UI_PAUSE);
	}
}

static void mainMenuPressed(UIElement *button) {
	pause(false);
	setTimeScale(1.0);
	mainMenu();
}

static void unpausePressed(UIElement *button) { pause(false); }

void createPauseMenu() {
	clearUIGroup(UI_PAUSE);
	createPanel(gfc_vector2d(0, 0), gfc_vector2d(1280, 720), gfc_color(0, 0, 0, 0.5))->group = UI_PAUSE;
	UIElement *label = createLabel(gfc_vector2d(0, 0), gfc_vector2d(1280, 100));
	gfc_line_cpy(label->text, "paused");
	label->group = UI_PAUSE;
	UIElement *unpauseButton = createButton(gfc_vector2d(1280 / 2, 720 / 2), gfc_vector2d(300, 100), "unpause");
	unpauseButton->click = unpausePressed;
	unpauseButton->group = UI_PAUSE;
	UIElement *mainMenuButton =
		createButton(gfc_vector2d(1280 / 2, 720 / 2 + 120), gfc_vector2d(300, 100), "main menu");
	mainMenuButton->click = mainMenuPressed;
	mainMenuButton->group = UI_PAUSE;
}
