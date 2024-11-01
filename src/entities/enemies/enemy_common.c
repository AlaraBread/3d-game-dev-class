#include "enemy_common.h"

int g_numEnemies = 0;
int g_maxEnemies = 0;
void freeEnemy(PhysicsBody *enemy) {
	g_numEnemies--;
}

PhysicsBody *createEnemy() {
	g_numEnemies++;
	g_maxEnemies++;
	return physicsCreateBody();
}
