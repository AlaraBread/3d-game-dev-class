#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_color.h"
#include "gfc_primitives.h"

#include "gf3d_model.h"

typedef struct Entity_S
{
    Uint8       _inuse;         /**<keeps track of memory usage*/
    TextLine    name;
    ModelMat    mat;            /**<orientation matrix for the model*/
    Color       color;          /**<default color for the model*/
    Color       detailColor;    /**<detail color for the model*/
    Color       selectedColor;  /**<Color for highlighting*/
    Uint8       hidden;         /**<if true, not drawn*/
    Uint8       selected;
    
    Box         bounds;         // for collisions
    int         team;           //same team dont clip
    int         clips;          // if false, skip collisions

    void       (*think)(struct Entity_S *self); /**<pointer to the think function*/
    void       (*update)(struct Entity_S *self); /**<pointer to the update function*/
    void       (*draw)(struct Entity_S *self); /**<pointer to an optional extra draw funciton*/
    void       (*damage)(struct Entity_S *self, float damage, struct Entity_S *inflictor); /**<pointer to the think function*/
    void       (*onDeath)(struct Entity_S *self); /**<pointer to an funciton to call when the entity dies*/
    void       (*free)(struct Entity_S *self); /**<pointer to the custom free function, necessar when there is custom data*/
        
    float       roll;           //kept separate 
    Vector3D    velocity;
    Vector3D    acceleration;
    Vector3D    targetPosition;
    
    float       speed;// how fast it moves
    Bool        targetComplete;
    int         counter;//generic counting variable
            
    Uint32      health;         /**<entity dies when it reaches zero*/
    // WHATEVER ELSE WE MIGHT NEED FOR ENTITIES
    struct Entity_S *target;    /**<entity to target for weapons / ai*/
    
    void *data;   /**<IF an entity needs to keep track of extra data, we can do it here*/
}Entity;

/**
 * @brief initializes the entity subsystem
 * @param maxEntities the limit on number of entities that can exist at the same time
 */
void gf3d_entity_system_init(Uint32 maxEntities);

/**
 * @brief provide a pointer to a new empty entity
 * @return NULL on error or a valid entity pointer otherwise
 */
Entity *gf3d_entity_new();

/**
 * @brief free a previously created entity from memory
 * @param self the entity in question
 */
void gf3d_entity_free(Entity *self);


/**
 * @brief Draw an entity in the current frame
 * @param self the entity in question
 */
void gf3d_entity_draw(Entity *self);

/**
 * @brief draw ALL active entities
 */
void gf3d_entity_draw_all();

/**
 * @brief Call an entity's think function if it exists
 * @param self the entity in question
 */
void gf3d_entity_think(Entity *self);

/**
 * @brief run the think functions for ALL active entities
 */
void gf3d_entity_think_all();

/**
 * @brief run the update functions for ALL active entities
 */
void gf3d_entity_update_all();

/**
 * @brief get an entity by its name.  Names are not guaranteed to be unique, so be careful
 * @param name the name to search for
 * @return NULL if not found, or the first entity with the given name
 */
Entity *gf3d_entity_get_by_name(const char *name);

#endif