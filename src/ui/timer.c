#include "timer.h"
#include "util.h"

static void updateTimer(UIElement *timer, double delta) {
	timer->timer += delta;
	sprintf(timer->text, "%02.0lf : %02.2lf", floor(timer->timer / 60.0), wrapMax(timer->timer, 60));
}

UIElement *g_timer = NULL;
UIElement *createTimer() {
	UIElement *timer = newUIElement();
	timer->size = gfc_vector2d(1280, 100);
	timer->draw = drawTextRect;
	timer->update = updateTimer;
	g_timer = timer;
	return timer;
}
