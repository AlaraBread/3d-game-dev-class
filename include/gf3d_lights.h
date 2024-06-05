#ifndef __GF3D_LIGHTS_H__
#define __GF3D_LIGHTS_H__

#include "gfc_list.h"

#include "gf3d_vgraphics.h"

/**
 * needs to remain in sync with the shader ubo
 */
#define MAX_SHADER_LIGHTS 8

typedef enum
{
    LT_Directional,     //illuminates as if from infinitely far away in one direction. The sun or moon are examples
    LT_Area,            //illuminates from a single point in all directions like a match
    LT_Spot,            //illuminates in one direction with a focused cone of light like a flashlight
    LT_MAX              //limiter
}GF3D_LightTypes;

/**
 * @brief describes a standard light
 */
typedef struct
{
    GFC_Vector4D    color;              //color of light
    GFC_Vector4D    direction;          //used for directional light
    GFC_Vector4D    position;           //where the light is from.  w determines if its a directional light or attenuated light
    float           ambientCoefficient; //how strong the ambient
    float           attenuation;        //how fast this light falls off
    float           angle;              //If nonzero, it is a spot light.  
    float           range;              //if nonzero, light will not illuminate past this distance
}GF3D_Light;

/**
 * @brief the data sent to the fragment shader for lighting calculations
 * this will be binding point 3, sent to fragment shader
 */
typedef struct
{
    GF3D_Light lights[MAX_SHADER_LIGHTS];   //list of all lights
    GFC_Vector4D flags;                     //.x is how many lights are in use
}LightUBO;

/**
 * @brief initialize the lighting system and queue up its close atexit
 */
void gf3d_lights_init();


/**
 * @brief clear all of the current active lights.  This will invalidate all active light pointers
 * This also zeroes out the ambient light.
 * This is meant to be used between level transitions.
 */
void gf3d_lights_clear_all();

/**
 * @brief free a previously allocated light
 * @param light the light to free
 */
void gf3d_light_free(GF3D_Light *light);

/**
 * @brief get a pointer to a new blank light
 * @return NULL on error or a pointer to a blank light otherwise
 * @note free it with gf3d_light_free();
 */
GF3D_Light *gf3d_light_new();

/**
 * @brief set the global ambient light
 * @param color the color it should be set to
 * @param direction the direction the light should be in.  
 * @param strength how strong the ambient light should factor into the render (0.5 is average)
 */
void gf3d_light_set_ambient_light(GFC_Color color,GFC_Vector3D direction,float strength);

/**
 * @brief get a pointer to the global ambient light
 * @return a pointer to the global ambient light.
 */
GF3D_Light *gf3d_light_get_ambient_light();

/**
 * @brief create a new area light and return a pointer to it
 * @note area lights emit light in all directions
 * @param color the color the new light should be
 * @param position where the light should be
 * @param attenuation power of the light
 * @return NULL on error, or a pointer to the newly created light otherwise;
 * @note free it with gf3d_light_free();
 */
GF3D_Light *gf3d_light_new_area(GFC_Color color, GFC_Vector3D position, float attenuation);

/**
 * @brief create a new spot light and return a pointer to it
 * @note spot lights are cone shaped
 * @param color the color the new light should be
 * @param position where the light should be
 * @param attenuation power of the light
 * @param angle the angle of the spotlight. Note that an angle of zero is treated like an area light
 * @return NULL on error, or a pointer to the newly created light otherwise;
 * @note free it with gf3d_light_free();
 */
GF3D_Light *gf3d_light_new_spot(GFC_Color color, GFC_Vector3D position, GFC_Vector3D direction, float attenuation, float angle);

/**
 * @brief add a light to the light ubo
 * @param ubo [output] set the ambient light information based on the provided light
 * @param light the light to use for ambient light in the ubo
 */
void gf3d_light_add_light_to_ubo(LightUBO *ubo,GF3D_Light *light);

/**
 * @brief add the global ambient light to the light ubo
 * @param ubo [output] set the ambient light information based on the global light
 */
void gf3d_light_add_global_ambient_to_ubo(LightUBO *ubo);

/**
 * @brief build a lighting ubo based on the list provided
 * @param ubo [output] this will be populated with up to the first MAX_SHADER_LIGHTS
 * @param lights the list of lights to populate from
 * @note ubo->flags.y will be set to the number of lights set.
 */
void gf3d_light_build_ubo_from_list(LightUBO *ubo,GFC_List *lights);

/**
 * @brief build a lighting ubo based on the closest lights in the global list
 * @param ubo [output] this will be populated with up to MAX_SHADER_LIGHTS
 * @param relative this will be the reference point for chosing which lights will be added.
 * @note ubo->flags.y will be set to the number of lights set.
 */
void gf3d_light_build_ubo_from_closest(LightUBO *ubo,GFC_Vector3D relative);

/**
 * @brief build a lighting ubo based on the closest lights in the provided list
 * @param ubo [output] this will be populated with up to MAX_SHADER_LIGHTS
 * @param lights the list of lights to check against
 * @param relative this will be the reference point for chosing which lights will be added.
 * @note ubo->flags.y will be set to the number of lights set.
 */
void gf3d_light_build_ubo_from_closest_list(LightUBO *ubo,GFC_List *lights, GFC_Vector3D relative);

/**
 * @brief build a basic lightUbo from just the ambient light info.
 * @note this is a good starting point for making a light ubo
 * @return a light Ubo with just the ambient light set based on the global setting
 */
LightUBO gf3d_light_basic_ambient_ubo();

/**
 * @brief build a lightUBO from the global ambient light and the first few lights in the light manager
 * @return a light Ubo with the ambient and other lights set based on the global settings
 */
LightUBO gf3d_light_get_global_lights_ubo();

/**
 * @brief zero out a lightUBO for re-use
 */
void gf3d_list_reset_ubo(LightUBO *lights);

/**
 * @brief draw a light to show where it is and where it is pointing
 * @note this is mostly for debugging purposes
 * @param light the light to draw
 */
void gf3d_light_draw(GF3D_Light *light);

/**
 * @brief get the light type by its name
 * @param name the name of the lighttype
 * @return LT_MAX if the name does not match any light types, the GF3D_LightTypes otherwise
 */
GF3D_LightTypes gf3d_light_get_type_by_name(const char *name);

/**
 * @brief get the type name as a character string given a lightType
 * @param lightType the name to get
 * @return NULL if the lightType is invalid, a read-only character buffer containing the light type name otherwise
 */
const char *gf3d_light_get_type_name(GF3D_LightTypes lightType);

/**
 * @brief free a list of GF3D_Lights.  
 * @note this cannot type check so be sure the list only contains lights
 * @param list the light list to free.  The lights and the list will be cleaned up
 */
void gf3d_light_list_free(GFC_List *list);

/**
 * @brief load a list of lights from config
 * @param config the json array to load from
 * @return NULL if the config is bad or does not contain an array. A list with all of the lights in it otherwise
 */
GFC_List *gf3d_light_list_load_from_config(SJson *config);

/**
 * @brief extract a light from json config
 * @param config the json to extract from
 * @return NULL on error, or the light information from the config
 * @note this doesn't validate if the light data makes sense or not
 */
GF3D_Light *gf3d_light_load_from_config(SJson *config);

#endif
