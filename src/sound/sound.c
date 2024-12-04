#include <gfc_text.h>
#include <gfc_vector.h>
#include <simple_logger.h>
#include <soloud_c.h>

#include "sound.h"
#include "util.h"

Soloud *g_soloud = NULL;
Sound *g_sounds = NULL;

void endSound();
void startSound() {
	g_soloud = Soloud_create();
	Soloud_initEx(g_soloud, SOLOUD_CLIP_ROUNDOFF, SOLOUD_AUTO, SOLOUD_AUTO, 128, SOLOUD_AUTO);
	atexit(endSound);
	Soloud_set3dSoundSpeed(g_soloud, 500); // todo: experiment
	g_sounds = calloc(MAX_SOUNDS, sizeof(Sound));
}

size_t findEmptySound() {
	for(int i = 0; i < MAX_SOUNDS; i++) {
		if(g_sounds[i].refs <= 0) {
			if(g_sounds[i].sound) WavStream_destroy(g_sounds[i].sound);
			return i;
		}
	}
	return -1;
}

Sound *findSound(const char *file) {
	for(int i = 0; i < MAX_SOUNDS; i++) {
		if(strcmp(g_sounds[i].filename, file) == 0) return &g_sounds[i];
	}
	return NULL;
}

Sound *loadSound(const char *file) {
	if(!g_soloud) return NULL;
	Sound *sound = findSound(file);
	if(sound) {
		sound->refs++;
		return sound;
	}
	WavStream *stream = WavStream_create();
	if(!stream) slog("failed to create wavstream");
	int err = WavStream_load(stream, file);
	if(err != 0) slog("loading %s failed, error %d", file, err);
	size_t idx = findEmptySound();
	if(idx < 0) {
		slog("too many loaded sounds");
		return NULL;
	}
	sound = &g_sounds[idx];
	sound->refs = 1;
	gfc_line_cpy(sound->filename, file);
	sound->sound = stream;
	return sound;
}

void freeSound(Sound *sound) {
	if(sound->refs <= 0) {
		slog("tried to free sound with no refs");
		return;
	}
	sound->refs--;
}

unsigned int playSound3D(GFC_Vector3D position, GFC_Vector3D velocity, double volume, Sound *sound, Bool looping) {
	if(!(vector3_is_finite(position) && vector3_is_finite(velocity) && isfinite(volume))) return -1;
	if(!g_soloud) return 0;
	unsigned int handle = Soloud_play3d(g_soloud, sound->sound, position.x, position.y, position.z);
	Soloud_setLooping(g_soloud, handle, looping);
	Soloud_setInaudibleBehavior(g_soloud, handle, looping, false);
	Soloud_set3dSourceVelocity(g_soloud, handle, velocity.x, velocity.y, velocity.z);
	Soloud_set3dSourceDopplerFactor(g_soloud, handle, 1.0);
	// use inverse distance
	Soloud_set3dSourceAttenuation(g_soloud, handle, 1, 1.0); // todo: play with this value
	Soloud_set3dSourceMinMaxDistance(g_soloud, handle, 20, 40);
	return handle;
}

void stopSound(unsigned int handle) { Soloud_stop(g_soloud, handle); }

void updateSound3D(unsigned int handle, GFC_Vector3D position, GFC_Vector3D velocity, float volume, float speed) {
	if(!(vector3_is_finite(position) && vector3_is_finite(velocity) && isfinite(volume) && isfinite(speed))) return;
	Soloud_set3dSourcePosition(g_soloud, handle, position.x, position.y, position.z);
	Soloud_set3dSourceVelocity(g_soloud, handle, velocity.x, velocity.y, velocity.z);
	Soloud_setRelativePlaySpeed(g_soloud, handle, speed);
	Soloud_setVolume(g_soloud, handle, volume);
}

void updateListener3D(GFC_Vector3D position, GFC_Vector3D forward, GFC_Vector3D up, GFC_Vector3D velocity) {
	Soloud_set3dListenerParametersEx(
		g_soloud, position.x, position.y, position.z, forward.x, forward.y, forward.z, up.x, up.y, up.z, velocity.x,
		velocity.y, velocity.z
	);
	Soloud_update3dAudio(g_soloud);
}

void endSound() {
	if(g_soloud) {
		Soloud_deinit(g_soloud);
		Soloud_destroy(g_soloud);
	}
	g_soloud = NULL;
}
