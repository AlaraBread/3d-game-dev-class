#ifndef __ALARA_PLAYER_H__
#define __ALARA_PLAYER_H__

#include "physics.h"

PhysicsBody *createPlayer();
void playerFrameProcess(PhysicsBody *self, double delta);
void processPlayerSFX(PhysicsBody *self, double delta);

#endif
