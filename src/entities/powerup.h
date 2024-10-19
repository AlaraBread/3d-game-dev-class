#ifndef __ALARA_POWERUP_H__
#define __ALARA_POWERUP_H__

typedef enum {
	CAR,
	BIG,
	SUPER_SPEED,
	SUPER_JUMP,
	EXTRA_TIME,
} PowerupType;

#include "physics.h"

PhysicsBody *createPowerup(GFC_Vector3D position, PowerupType type);

#endif
