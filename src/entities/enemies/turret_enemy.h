#ifndef __ALARA_TURRET_ENEMY_H__
#define __ALARA_TURRET_ENEMY_H__

#include "physics.h"

#define SHOOT_INTERVAL 2

PhysicsBody *createTurretEnemy(GFC_Vector3D position);
void turretEnemyPhysicsProcess(PhysicsBody *self, double delta);

#endif
