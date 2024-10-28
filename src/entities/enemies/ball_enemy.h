#ifndef __ALARA_BALL_ENEMY_H__
#define __ALARA_BALL_ENEMY_H__

#include "physics.h"

PhysicsBody *createBallEnemy(GFC_Vector3D position);
void freeEnemy(PhysicsBody *enemy);

#endif
