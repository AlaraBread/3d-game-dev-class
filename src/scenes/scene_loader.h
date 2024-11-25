#ifndef __ALARA_SCENE_LOADER_H__
#define __ALARA_SCENE_LOADER_H__

typedef enum GameMode {
	TIME,
	COMPLETION,
} GameMode;

void loadScene(const char *filename);

#endif
