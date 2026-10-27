#ifndef RUNNER_DS_H
#define RUNNER_DS_H

#include "rvalue.h"

#ifdef __cplusplus
extern "C" {
#endif

// -------------------------------------------------------------
// DS System Initialization
// -------------------------------------------------------------
void DS_Init(void);
void DS_Reset(void);

// -------------------------------------------------------------
// DS List
// -------------------------------------------------------------
int32_t ds_list_create(void);
void ds_list_destroy(int32_t id);
void ds_list_clear(int32_t id);
int32_t ds_list_size(int32_t id);
bool ds_list_empty(int32_t id);
void ds_list_add(int32_t id, const RValue* val);
void ds_list_insert(int32_t id, int32_t pos, const RValue* val);
void ds_list_delete(int32_t id, int32_t pos);
void ds_list_replace(int32_t id, int32_t pos, const RValue* val);
RValue* ds_list_find_value(int32_t id, int32_t pos);
void ds_list_sort(int32_t id, bool ascend);

// -------------------------------------------------------------
// DS Map
// -------------------------------------------------------------
int32_t ds_map_create(void);
void ds_map_destroy(int32_t id);
void ds_map_clear(int32_t id);
int32_t ds_map_size(int32_t id);
bool ds_map_empty(int32_t id);
bool ds_map_add(int32_t id, const RValue* key, const RValue* val);
void ds_map_replace(int32_t id, const RValue* key, const RValue* val);
RValue* ds_map_find_value(int32_t id, const RValue* key);
bool ds_map_exists(int32_t id, const RValue* key);
void ds_map_delete(int32_t id, const RValue* key);

// -------------------------------------------------------------
// DS Grid
// -------------------------------------------------------------
int32_t ds_grid_create(int32_t w, int32_t h);
void ds_grid_destroy(int32_t id);
void ds_grid_resize(int32_t id, int32_t w, int32_t h);
void ds_grid_clear(int32_t id, const RValue* val);
void ds_grid_set(int32_t id, int32_t x, int32_t y, const RValue* val);
RValue* ds_grid_get(int32_t id, int32_t x, int32_t y);
int32_t ds_grid_width(int32_t id);
int32_t ds_grid_height(int32_t id);

// -------------------------------------------------------------
// DS Stack
// -------------------------------------------------------------
int32_t ds_stack_create(void);
void ds_stack_destroy(int32_t id);
void ds_stack_clear(int32_t id);
int32_t ds_stack_size(int32_t id);
bool ds_stack_empty(int32_t id);
void ds_stack_push(int32_t id, const RValue* val);
RValue* ds_stack_pop(int32_t id);
RValue* ds_stack_top(int32_t id);

// -------------------------------------------------------------
// DS Queue
// -------------------------------------------------------------
int32_t ds_queue_create(void);
void ds_queue_destroy(int32_t id);
void ds_queue_clear(int32_t id);
int32_t ds_queue_size(int32_t id);
bool ds_queue_empty(int32_t id);
void ds_queue_enqueue(int32_t id, const RValue* val);
RValue* ds_queue_dequeue(int32_t id);
RValue* ds_queue_head(int32_t id);

// -------------------------------------------------------------
// DS Priority Queue
// -------------------------------------------------------------
int32_t ds_priority_create(void);
void ds_priority_destroy(int32_t id);
void ds_priority_clear(int32_t id);
int32_t ds_priority_size(int32_t id);
bool ds_priority_empty(int32_t id);
void ds_priority_add(int32_t id, const RValue* val, double priority);
RValue* ds_priority_delete_max(int32_t id);
RValue* ds_priority_delete_min(int32_t id);
RValue* ds_priority_find_max(int32_t id);
RValue* ds_priority_find_min(int32_t id);

#ifdef __cplusplus
}
#endif

#endif // RUNNER_DS_H
