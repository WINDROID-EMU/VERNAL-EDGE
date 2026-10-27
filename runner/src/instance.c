#include "instance.h"
#include "data_win.h"
#include <stdlib.h>
#include <string.h>

static CInstance s_instances[MAX_INSTANCES];
static CObjectDef s_objects[MAX_OBJECTS];
static RefStruct* s_globals = NULL;
static int32_t s_next_id = INSTANCE_BASE_ID;

// Mock CInstance VTable function pointers matching YYC binary layout
static void* instance_vtable[16];

static RValue* VTable_GetValue(CInstance* inst, int32_t var_id) {
    return Instance_GetVariable(inst, var_id);
}

static RValue* VTable_GetRef(CInstance* inst, int32_t var_id) {
    return Instance_GetVariableRef(inst, var_id);
}

void InstanceManager_Init(void) {
    memset(s_instances, 0, sizeof(s_instances));
    memset(s_objects, 0, sizeof(s_objects));
    s_next_id = INSTANCE_BASE_ID;
    
    if (!s_globals) {
        s_globals = RefStruct_Create();
    }

    // Setup VTable slots
    // slot 1 (offset +8) = Variable_GetValue
    // slot 2 (offset +16) = Variable_GetRef
    instance_vtable[0] = NULL;
    instance_vtable[1] = (void*)&VTable_GetValue;
    instance_vtable[2] = (void*)&VTable_GetRef;
}

void InstanceManager_Reset(void) {
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        if (s_instances[i].active) {
            instance_destroy(s_instances[i].id);
        }
    }
    s_next_id = INSTANCE_BASE_ID;
}

CInstance* instance_create_depth(double x, double y, double depth, int32_t object_index) {
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        if (!s_instances[i].active) {
            CInstance* inst = &s_instances[i];
            memset(inst, 0, sizeof(CInstance));
            inst->vtable = instance_vtable;
            inst->variables = RefStruct_Create();
            inst->id = s_next_id++;
            inst->object_index = object_index;
            inst->active = true;
            inst->marked_for_destroy = false;
            inst->persistent = false;
            
            inst->x = x;
            inst->y = y;
            inst->z = 0.0;
            inst->xprevious = x;
            inst->yprevious = y;
            inst->xstart = x;
            inst->ystart = y;
            inst->hspeed = 0.0;
            inst->vspeed = 0.0;
            inst->direction = 0.0;
            inst->speed = 0.0;
            inst->friction = 0.0;
            inst->gravity = 0.0;
            inst->gravity_direction = 270.0;
            
            inst->depth = depth;
            inst->visible = true;
            inst->solid = false;
            inst->sprite_index = -1;
            inst->image_index = 0.0;
            inst->image_speed = 1.0;
            inst->image_xscale = 1.0;
            inst->image_yscale = 1.0;
            inst->image_angle = 0.0;
            inst->image_blend = 0xffffffff;
            inst->image_alpha = 1.0;
            inst->mask_index = -1;

            /* Initialize default properties from data.win GameObject definition */
            if (object_index >= 0 && (uint32_t)object_index < g_DataWin.object_count) {
                GameObjectDef *god = &g_DataWin.objects[object_index];
                inst->sprite_index = god->sprite_index;
                inst->visible = (god->visible != 0);
                inst->solid = (god->solid != 0);
                inst->persistent = (god->persistent != 0);
                inst->mask_index = god->mask_index;
                if (depth == 0.0 && god->depth != 0) {
                    inst->depth = (double)god->depth;
                }
            }

            for (int a = 0; a < MAX_ALARMS; a++) {
                inst->alarm[a] = -1;
            }

            // Invoke Object Create Event if defined
            if (object_index >= 0 && object_index < MAX_OBJECTS) {
                CObjectDef* obj = &s_objects[object_index];
                if (obj->ev_create) {
                    obj->ev_create(inst, NULL);
                }
            }

            return inst;
        }
    }
    return NULL;
}

void instance_destroy(int32_t id) {
    CInstance* inst = Instance_GetById(id);
    if (inst && inst->active) {
        // Invoke Destroy & CleanUp events
        if (inst->object_index >= 0 && inst->object_index < MAX_OBJECTS) {
            CObjectDef* obj = &s_objects[inst->object_index];
            if (obj->ev_destroy) obj->ev_destroy(inst, NULL);
            if (obj->ev_cleanup) obj->ev_cleanup(inst, NULL);
        }
        if (inst->variables) {
            RefStruct_Release(inst->variables);
            inst->variables = NULL;
        }
        inst->active = false;
    }
}

bool instance_exists(int32_t id_or_obj) {
    if (id_or_obj >= INSTANCE_BASE_ID) {
        CInstance* inst = Instance_GetById(id_or_obj);
        return inst != NULL && inst->active && !inst->marked_for_destroy;
    }
    // Check if any instance of object exists
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        if (s_instances[i].active && !s_instances[i].marked_for_destroy) {
            if (s_instances[i].object_index == id_or_obj) return true;
            // Check inheritance parent
            int32_t p = s_objects[s_instances[i].object_index].parent_index;
            while (p >= 0 && p < MAX_OBJECTS) {
                if (p == id_or_obj) return true;
                p = s_objects[p].parent_index;
            }
        }
    }
    return false;
}

CInstance* Instance_GetById(int32_t id) {
    if (id < INSTANCE_BASE_ID) return NULL;
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        if (s_instances[i].active && s_instances[i].id == id) {
            return &s_instances[i];
        }
    }
    return NULL;
}

CInstance* instance_find(int32_t obj_index, int32_t n) {
    int32_t match = 0;
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        if (s_instances[i].active && !s_instances[i].marked_for_destroy) {
            bool matches = (s_instances[i].object_index == obj_index);
            if (!matches) {
                int32_t p = s_objects[s_instances[i].object_index].parent_index;
                while (p >= 0 && p < MAX_OBJECTS) {
                    if (p == obj_index) { matches = true; break; }
                    p = s_objects[p].parent_index;
                }
            }
            if (matches) {
                if (match == n) return &s_instances[i];
                match++;
            }
        }
    }
    return NULL;
}

int32_t instance_number(int32_t obj_index) {
    int32_t count = 0;
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        if (s_instances[i].active && !s_instances[i].marked_for_destroy) {
            if (obj_index < 0 || s_instances[i].object_index == obj_index) count++;
        }
    }
    return count;
}

// -------------------------------------------------------------
// Variable Management
// -------------------------------------------------------------
static RValue s_temp_undef = { .val = 0, .flags = 0, .type = VALUE_UNDEFINED };

RValue* Instance_GetVariable(CInstance* inst, int32_t var_id) {
    if (!inst) return &s_temp_undef;
    RValue* val = RefStruct_Get(inst->variables, var_id);
    if (val) return val;
    return &s_temp_undef;
}

RValue* Instance_GetVariableRef(CInstance* inst, int32_t var_id) {
    if (!inst) return &s_temp_undef;
    RValue* val = RefStruct_Get(inst->variables, var_id);
    if (!val) {
        RValue init_val;
        RValue_Init(&init_val);
        RefStruct_Set(inst->variables, var_id, &init_val);
        val = RefStruct_Get(inst->variables, var_id);
    }
    return val;
}

void Instance_SetVariable(CInstance* inst, int32_t var_id, const RValue* val) {
    if (!inst) return;
    RefStruct_Set(inst->variables, var_id, val);
}

void* Variable_Lookup(CInstance* inst, int32_t var_id, ...) {
    return Instance_GetVariableRef(inst, var_id);
}

RValue* Variable_GetGlobal(int32_t var_id) {
    if (!s_globals) s_globals = RefStruct_Create();
    RValue* val = RefStruct_Get(s_globals, var_id);
    if (!val) {
        RValue init_val;
        RValue_Init(&init_val);
        RefStruct_Set(s_globals, var_id, &init_val);
        val = RefStruct_Get(s_globals, var_id);
    }
    return val;
}

void Variable_Global_Set(int32_t var_id, const RValue* val) {
    if (!s_globals) s_globals = RefStruct_Create();
    RefStruct_Set(s_globals, var_id, val);
}

// -------------------------------------------------------------
// Object Registration
// -------------------------------------------------------------
void Object_Register(int32_t obj_idx, int32_t parent_idx, const char* name) {
    if (obj_idx >= 0 && obj_idx < MAX_OBJECTS) {
        s_objects[obj_idx].object_index = obj_idx;
        s_objects[obj_idx].parent_index = parent_idx;
        s_objects[obj_idx].name = name;
    }
}

void Object_SetEvent(int32_t obj_idx, int32_t event_type, EventFunc func) {
    if (obj_idx >= 0 && obj_idx < MAX_OBJECTS) {
        CObjectDef* obj = &s_objects[obj_idx];
        switch (event_type) {
            case 0: obj->ev_create = func; break;
            case 1: obj->ev_destroy = func; break;
            case 2: obj->ev_cleanup = func; break;
            case 3: obj->ev_step_begin = func; break;
            case 4: obj->ev_step = func; break;
            case 5: obj->ev_step_end = func; break;
            case 6: obj->ev_draw = func; break;
            case 7: obj->ev_draw_gui = func; break;
            default: break;
        }
    }
}

CObjectDef* Object_Get(int32_t obj_idx) {
    if (obj_idx >= 0 && obj_idx < MAX_OBJECTS) {
        return &s_objects[obj_idx];
    }
    return NULL;
}
