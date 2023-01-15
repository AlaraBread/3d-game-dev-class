#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_list.h"

#include "gf3d_draw.h"

#include "config_def.h"
#include "ship.h"
#include "ship_entity.h"


void ship_entity_update(Entity *self);
void ship_entity_draw(Entity *self);
void ship_entity_think(Entity *self);
void ship_entity_think_moving(Entity *self);
void ship_entity_free(Entity *self);

Entity *ship_entity_new(Vector3D position,Ship *data,Color detailColor)
{
    const char *str;
    SJson *def;
    Entity *ent = NULL;    
    if (!data)
    {
        slog("ship_entity: null ship data provided");
        return NULL;
    }
    def = config_def_get_by_name("ships",data->name);
    if (!def)return NULL;
    ent = gf3d_entity_new();
    if (!ent)return NULL;
    
    str = sj_object_get_value_as_string(def,"model");
    if (str)
    {
        ent->mat.model = gf3d_model_load(str);
        if (!ent->mat.model)
        {
            slog("SHIP MODEL DID NOT LOAD");
        }
    }
    else
    {
        slog("no model specified for ship %s",data->name);
    }

    gfc_line_cpy(ent->name,data->displayName);
    sj_value_as_vector3d(sj_object_get_value(def,"scale"),&ent->mat.scale);
    sj_value_as_vector3d(sj_object_get_value(def,"rotation"),&ent->mat.rotation);
    vector3d_copy(ent->mat.position,position);
    
    ent->selectedColor = gfc_color(0.9,0.7,0.1,1);
    ent->color = gfc_color(1,1,1,1);
    gfc_color_copy(ent->detailColor,detailColor);
    ent->think = ship_entity_think;
    ent->draw = ship_entity_draw;
    ent->update = ship_entity_update;
    ent->free = ship_entity_free;
    ent->data = data;
    vector3d_copy(ent->mat.position,position);
    return ent;
}

void ship_entity_move_to(Entity *ent,Vector3D position)
{
    if (!ent)return;
    vector3d_copy(ent->targetPosition,position);
    ent->targetComplete = 0;
    ent->think = ship_entity_think_moving;
}

void ship_entity_free(Entity *self)
{
    // the ship entity doesn't own the Ship data, ship data owns this
}

void ship_entity_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
}

void ship_entity_draw(Entity *self)
{
    Ship *data;
    if ((!self)||(!self->data))return;
    data = self->data;
    if ((strcmp(data->location,"docked")==0)||
        (strcmp(data->location,"interstellar")==0))
    {
        self->hidden = 1;
        return;// inside the station
    }
    self->hidden = 0;
}

void ship_entity_think_moving(Entity *self)
{
    Vector3D dir;
    if (!self)return;
    // do maintenance
    if (vector3d_distance_between_less_than(self->mat.position,self->targetPosition,1))
    {
        self->think = ship_entity_think;
        self->targetComplete = 1;
        vector3d_clear(self->velocity);
        return;
    }
    vector3d_sub(dir,self->targetPosition,self->mat.position);
    vector3d_normalize(&dir);
    vector3d_scale(self->velocity,dir,self->speed);
}

void ship_entity_think(Entity *self)
{
    if (!self)return;
    // do maintenance
}

/*eol@eof*/
