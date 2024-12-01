#ifndef __ALARA_SOUND_H__
#define __ALARA_SOUND_H__

#include <gfc_text.h>
#include <soloud_c.h>

#define MAX_SOUNDS 64
typedef struct Sound_s {
	int refs;
	WavStream *sound;
	GFC_TextLine filename;
} Sound;

void startSound();
Sound *loadSound(const char *file);
void freeSound(Sound *sound);
unsigned int playSound3D(GFC_Vector3D position, GFC_Vector3D velocity, double volume, Sound *sound, Bool looping);
void updateSound3D(unsigned int handle, GFC_Vector3D position, GFC_Vector3D velocity, float volume, float speed);
void updateListener3D(GFC_Vector3D position, GFC_Vector3D forward, GFC_Vector3D up, GFC_Vector3D velocity);

#endif
