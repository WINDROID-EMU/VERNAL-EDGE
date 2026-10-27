#ifndef RUNNER_INSTANCE_H
#define RUNNER_INSTANCE_H

#include "rvalue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_ALARMS 12
#define MAX_OBJECTS 2048
#define MAX_INSTANCES 8192
#define INSTANCE_BASE_ID 100000

// Event function signature: void (*EventFunc)(CInstance* self, CInstance* other)
typedef void (*EventFunc)(void* self, void* other, ...);

// Object definition table
typedef struct CObjectDef {
    int32_t object_index;
    int32_t parent_index;
    const char* name;
    EventFunc ev_create;
    EventFunc ev_destroy;
    EventFunc ev_cleanup;
    EventFunc ev_step_begin;
    EventFunc ev_step;
    EventFunc ev_step_end;
    EventFunc ev_draw;
    EventFunc ev_draw_gui;
    EventFunc ev_alarm[MAX_ALARMS];
} CObjectDef;

// GameMaker CInstance structure
struct CInstance {
    void** vtable;
    RefStruct* variables; // Dynamic instance variables
    
    // Core Identity
    int32_t id;
    int32_t object_index;
    bool active;
    bool marked_for_destroy;
    bool persistent;
    
    // Transform & Physics
    double x;
    double y;
    double z;
    double xprevious;
    double yprevious;
    double xstart;
    double ystart;
    double hspeed;
    double vspeed;
    double direction;
    double speed;
    double gravity;
    double gravity_direction;
    double friction;
    
    // Rendering & Animation
    double depth;
    bool visible;
    bool solid;
    int32_t sprite_index;
    double image_index;
    double image_speed;
    double image_xscale;
    double image_yscale;
    double image_angle;
    uint32_t image_blend;
    double image_alpha;
    int32_t mask_index;
    
    // Timers
    int32_t alarm[MAX_ALARMS];
};

// Instance Management API
void InstanceManager_Init(void);
void InstanceManager_Reset(void);

CInstance* instance_create_depth(double x, double y, double depth, int32_t object_index);
void instance_destroy(int32_t id);
bool instance_exists(int32_t id);
CInstance* instance_find(int32_t obj_index, int32_t n);
int32_t instance_number(int32_t obj_index);
CInstance* Instance_GetById(int32_t id);

// Variable Access
RValue* Instance_GetVariable(CInstance* inst, int32_t var_id);
RValue* Instance_GetVariableRef(CInstance* inst, int32_t var_id);
void Instance_SetVariable(CInstance* inst, int32_t var_id, const RValue* val);

// Global Variables
RValue* Variable_GetGlobal(int32_t var_id);
void Variable_Global_Set(int32_t var_id, const RValue* val);

// Object Registration
void Object_Register(int32_t obj_idx, int32_t parent_idx, const char* name);
void Object_SetEvent(int32_t obj_idx, int32_t event_type, EventFunc func);
CObjectDef* Object_Get(int32_t obj_idx);

#ifdef __cplusplus
}
#endif

#endif // RUNNER_INSTANCE_H
