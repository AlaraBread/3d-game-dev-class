#include "enemy_counter.h"

extern int g_numEnemies;
extern int g_maxEnemies;
void updateCounter(UIElement *counter, double delta) {
	sprintf(counter->text, "remaining enemies: %d/%d", g_numEnemies, g_maxEnemies);
}

UIElement *g_counter = NULL;
UIElement *createEnemyCounter() {
	UIElement *counter = newUIElement();
	counter->size = gfc_vector2d(1280, 100);
	counter->draw = drawTextRect;
	counter->update = updateCounter;
	g_counter = counter;
	return counter;
}
