#include "gf2d_mouse.h"
#include "gf3d_gltf_parse.h"
#include "gf3d_obj_load.h"
#include "gfc_config.h"
#include "moments_of_inertia.h"
#include "physics.h"
#include "simple_logger.h"
#include "ui.h"
#include "util.h"

#include "cobweb.h"
#include "enemy_counter.h"
#include "fan.h"
#include "finish.h"
#include "floor.h"
#include "ice.h"
#include "jump_pad.h"
#include "lava.h"
#include "magnet.h"
#include "moving_platform.h"
#include "player.h"
#include "rotating_platform.h"
#include "timer.h"
#include "treadmill.h"

#include "scene_loader.h"

GFC_Vector3D lookupTarget(SJson *node, SJson *nodes) {
	GFC_Vector3DF position = {0};
	SJson *extras = sj_object_get_value(node, "extras");
	const char *targetName = sj_get_string_value(sj_object_get_value(sj_object_get_value(extras, "target"), "name"));
	if(!targetName) {
		const char *name = sj_get_string_value(sj_object_get_value(node, "name"));
		slog("node %s has missing target", name);
		return gfc_vector3df_to_double(position);
	}
	int numNodes = sj_array_get_count(nodes);
	for(int i = 0; i < numNodes; i++) {
		SJson *target = sj_array_get_nth(nodes, i);
		const char *name = sj_get_string_value(sj_object_get_value(target, "name"));
		if(strcmp(name, targetName) == 0) {
			sj_object_get_vector3d(target, "translation", &position);
			return gfc_vector3df_to_double(position);
		}
	}
	const char *name = sj_get_string_value(sj_object_get_value(node, "name"));
	slog("node %s has invalid target", name);
	return gfc_vector3df_to_double(position);
}

GameMode g_gamemode = TIME;

void loadScene(const char *filename) {
	clearUI();
	physicsClear();
	gf2d_mouse_set_captured(true);
	GLTF *gltf = gf3d_gltf_load(filename);
	if(!gltf) {
		slog("GLTF file '%s' Failed to load", filename);
		return;
	}
	Model *meshes = gf3d_gltf_parse_model(filename);
	SJson *nodes = sj_object_get_value(gltf->json, "nodes");
	int numNodes = sj_array_get_count(nodes);
	for(int i = 0; i < numNodes; i++) {
		SJson *node = sj_array_get_nth(nodes, i);
		SJson *extras = sj_object_get_value(node, "extras");
		const char *entityType = sj_get_string_value(sj_object_get_value(extras, "entity"));
		GFC_Vector3DF position = {0};
		GFC_Vector4DF rotation = {0};
		rotation.w = 1.0;
		sj_object_get_vector3d(node, "translation", &position);
		sj_object_get_vector4d(node, "rotation", &rotation);
		int meshIndex = -1;
		sj_get_integer_value(sj_object_get_value(node, "mesh"), &meshIndex);
		if(meshIndex < 0) { continue; }
		Mesh *mesh = gfc_list_get_nth(meshes->mesh_list, meshIndex);
		GFC_Box boundingBox = mesh->bounds;
		GFC_Vector3D extents;
		extents.x = (boundingBox.w - boundingBox.x) / 2.0;
		extents.y = (boundingBox.h - boundingBox.y) / 2.0;
		extents.z = (boundingBox.d - boundingBox.z) / 2.0;
		PhysicsBody *entity = NULL;
		Bool useTransform = true;
		if(!entityType) {
			continue;
		} else if(strcmp(entityType, "level") == 0) {
			const char *gameModeString = sj_get_string_value(sj_object_get_value(extras, "game_mode"));
			if(!gameModeString) {
				slog("missing game mode");
				continue;
			}
			if(strcmp(gameModeString, "time") == 0) {
				g_gamemode = TIME;
				createTimer();
			} else if(strcmp(gameModeString, "completion") == 0) {
				g_gamemode = COMPLETION;
				createEnemyCounter();
			}
		} else if(strcmp(entityType, "floor_invisible") == 0) {
			entity = physicsCreateBody();
			entity->shape.shapeType = CONVEX_HULL;
			// dont worry about multiple primitives in a mesh
			ObjData *objData = ((MeshPrimitive *)gfc_list_get_nth(mesh->primitives, 0))->objData;
			entity->shape.shape.convexHull.mesh = objData;
			entity->motionType = STATIC;
			calculateInertiaForBody(entity);
			entity->boundingRadius = MAX(MAX(objData->bounds.w, objData->bounds.h), objData->bounds.d) * 2;
		} else if(strcmp(entityType, "visual") == 0) {
			entity = physicsCreateBody();
			Model *model = gf3d_model_new();
			model->texture = gf3d_texture_load("assets/textures/checker.png");
			gfc_list_append(model->mesh_list, mesh);
			mesh->_refCount += 1;
			entity->model = model;
			entity->motionType = TRIGGER;
		} else if(strcmp(entityType, "player") == 0) {
			entity = createPlayer();
		} else if(strcmp(entityType, "box_floor") == 0) {
			entity = createFloor(extents);
		} else if(strcmp(entityType, "lava") == 0) {
			entity = createLava(extents);
		} else if(strcmp(entityType, "cobweb") == 0) {
			entity = createCobweb(extents);
		} else if(strcmp(entityType, "box_ice") == 0) {
			entity = createIce(extents);
		} else if(strcmp(entityType, "moving_platform") == 0) {
			float speed = 10.;
			sj_get_float_value(sj_object_get_value(extras, "speed"), &speed);
			GFC_Vector3D targetPos = lookupTarget(node, nodes);
			GFC_Vector3D diff;
			gfc_vector3d_sub(diff, targetPos, position);
			entity = createMovingPlatform(gfc_vector3df_to_double(position), extents, diff, speed);
			useTransform = false;
		} else if(strcmp(entityType, "fan") == 0) {
			float speed = 200.;
			sj_get_float_value(sj_object_get_value(extras, "speed"), &speed);
			GFC_Vector3D targetPos = lookupTarget(node, nodes);
			GFC_Vector3D dir;
			gfc_vector3d_sub(dir, targetPos, position);
			double dist = gfc_vector3d_magnitude(dir);
			entity = createFan(gfc_vector3df_to_double(position), dir, dist, speed);
			useTransform = false;
		} else if(strcmp(entityType, "magnet") == 0) {
			float strength = 10.;
			sj_get_float_value(sj_object_get_value(extras, "strength"), &strength);
			GFC_Vector3D targetPos = lookupTarget(node, nodes);
			entity = createMagnet(
				gfc_vector3df_to_double(position),
				gfc_vector3d_magnitude_between(targetPos, gfc_vector3df_to_double(position)), strength
			);
			useTransform = false;
		} else if(strcmp(entityType, "jump_pad") == 0) {
			float strength = 100.;
			sj_get_float_value(sj_object_get_value(extras, "strength"), &strength);
			entity = createJumpPad(extents, strength);
		} else if(strcmp(entityType, "rotating_platform") == 0) {
			GFC_Vector3D targetPos = lookupTarget(node, nodes);
			GFC_Vector3D angularVelocity;
			gfc_vector3d_sub(angularVelocity, targetPos, position);
			gfc_vector3d_scale(angularVelocity, angularVelocity, 0.01);
			entity = createRotatingPlatform(extents, angularVelocity);
		} else if(strcmp(entityType, "treadmill") == 0) {
			GFC_Vector3D targetPos = lookupTarget(node, nodes);
			GFC_Vector3D velocity;
			gfc_vector3d_sub(velocity, targetPos, position);
			entity = createTreadmill(extents, velocity);
		} else if(strcmp(entityType, "finish") == 0) {
			entity = createFinish(extents);
		} else if(strcmp(entityType, "powerup") == 0) {
			const char *typeString = sj_get_string_value(sj_object_get_value(extras, "type"));
			if(!typeString) {
				slog("missing powerup type");
				continue;
			}
			PowerupType type;
			if(strcmp(typeString, "car") == 0) {
				type = CAR;
			} else if(strcmp(typeString, "speed") == 0) {
				type = SUPER_SPEED;
			} else if(strcmp(typeString, "big") == 0) {
				type = BIG;
			} else if(strcmp(typeString, "jump") == 0) {
				type = SUPER_JUMP;
			} else if(strcmp(typeString, "slow") == 0) {
				type = SLOW;
			} else {
				slog("unknown powerup type: %s", typeString);
				continue;
			}
			entity = createPowerup(type);
		} else {
			slog("unknown entity type: %s", entityType);
			continue;
		}
		if(entity && useTransform) {
			entity->position = gfc_vector3df_to_double(position);
			quat_to_euler_vector(&entity->rotation, gfc_vector4df_to_double(rotation));
		}
	}
	gf3d_model_free(meshes);
}
