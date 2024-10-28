#include "enemy_counter.h"

int g_numEnemies = 0;
int g_maxEnemies = 0;
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
