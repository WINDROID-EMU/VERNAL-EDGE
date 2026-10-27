#include "ds.h"
#include <stdlib.h>
#include <string.h>

#define MAX_DS_HANDLES 4096

// -------------------------------------------------------------
// DS List Implementation
// -------------------------------------------------------------
typedef struct InternalList {
    bool active;
    int32_t count;
    int32_t capacity;
    RValue* items;
} InternalList;

static InternalList s_lists[MAX_DS_HANDLES];

int32_t ds_list_create(void) {
    for (int32_t i = 0; i < MAX_DS_HANDLES; i++) {
        if (!s_lists[i].active) {
            s_lists[i].active = true;
            s_lists[i].count = 0;
            s_lists[i].capacity = 8;
            s_lists[i].items = (RValue*)calloc(8, sizeof(RValue));
            return i;
        }
    }
    return -1;
}

void ds_list_destroy(int32_t id) {
    if (id >= 0 && id < MAX_DS_HANDLES && s_lists[id].active) {
        ds_list_clear(id);
        if (s_lists[id].items) {
            free(s_lists[id].items);
            s_lists[id].items = NULL;
        }
        s_lists[id].active = false;
    }
}

void ds_list_clear(int32_t id) {
    if (id >= 0 && id < MAX_DS_HANDLES && s_lists[id].active) {
        for (int32_t i = 0; i < s_lists[id].count; i++) {
            RValue_Free(&s_lists[id].items[i]);
        }
        s_lists[id].count = 0;
    }
}

int32_t ds_list_size(int32_t id) {
    if (id >= 0 && id < MAX_DS_HANDLES && s_lists[id].active) {
        return s_lists[id].count;
    }
    return 0;
}

bool ds_list_empty(int32_t id) {
    return ds_list_size(id) == 0;
}

void ds_list_add(int32_t id, const RValue* val) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_lists[id].active) return;
    InternalList* l = &s_lists[id];
    if (l->count >= l->capacity) {
        int32_t new_cap = l->capacity * 2;
        RValue* new_items = (RValue*)realloc(l->items, new_cap * sizeof(RValue));
        if (!new_items) return;
        l->items = new_items;
        l->capacity = new_cap;
    }
    RValue_Init(&l->items[l->count]);
    RValue_Copy(&l->items[l->count], val);
    l->count++;
}

void ds_list_insert(int32_t id, int32_t pos, const RValue* val) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_lists[id].active) return;
    InternalList* l = &s_lists[id];
    if (pos < 0) pos = 0;
    if (pos > l->count) pos = l->count;
    if (l->count >= l->capacity) {
        int32_t new_cap = l->capacity * 2;
        RValue* new_items = (RValue*)realloc(l->items, new_cap * sizeof(RValue));
        if (!new_items) return;
        l->items = new_items;
        l->capacity = new_cap;
    }
    memmove(&l->items[pos + 1], &l->items[pos], (l->count - pos) * sizeof(RValue));
    RValue_Init(&l->items[pos]);
    RValue_Copy(&l->items[pos], val);
    l->count++;
}

void ds_list_delete(int32_t id, int32_t pos) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_lists[id].active) return;
    InternalList* l = &s_lists[id];
    if (pos < 0 || pos >= l->count) return;
    RValue_Free(&l->items[pos]);
    memmove(&l->items[pos], &l->items[pos + 1], (l->count - pos - 1) * sizeof(RValue));
    l->count--;
}

void ds_list_replace(int32_t id, int32_t pos, const RValue* val) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_lists[id].active) return;
    InternalList* l = &s_lists[id];
    if (pos >= 0 && pos < l->count) {
        RValue_Copy(&l->items[pos], val);
    }
}

RValue* ds_list_find_value(int32_t id, int32_t pos) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_lists[id].active) return NULL;
    InternalList* l = &s_lists[id];
    if (pos >= 0 && pos < l->count) {
        return &l->items[pos];
    }
    return NULL;
}

static int compare_ascend(const void* a, const void* b) {
    return YYCompare((const RValue*)a, (const RValue*)b);
}

static int compare_descend(const void* a, const void* b) {
    return -YYCompare((const RValue*)a, (const RValue*)b);
}

void ds_list_sort(int32_t id, bool ascend) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_lists[id].active) return;
    InternalList* l = &s_lists[id];
    if (l->count > 1) {
        qsort(l->items, l->count, sizeof(RValue), ascend ? compare_ascend : compare_descend);
    }
}

// -------------------------------------------------------------
// DS Map Implementation
// -------------------------------------------------------------
typedef struct MapEntry {
    RValue key;
    RValue value;
} MapEntry;

typedef struct InternalMap {
    bool active;
    int32_t count;
    int32_t capacity;
    MapEntry* entries;
} InternalMap;

static InternalMap s_maps[MAX_DS_HANDLES];

int32_t ds_map_create(void) {
    for (int32_t i = 0; i < MAX_DS_HANDLES; i++) {
        if (!s_maps[i].active) {
            s_maps[i].active = true;
            s_maps[i].count = 0;
            s_maps[i].capacity = 16;
            s_maps[i].entries = (MapEntry*)calloc(16, sizeof(MapEntry));
            return i;
        }
    }
    return -1;
}

void ds_map_destroy(int32_t id) {
    if (id >= 0 && id < MAX_DS_HANDLES && s_maps[id].active) {
        ds_map_clear(id);
        if (s_maps[id].entries) {
            free(s_maps[id].entries);
            s_maps[id].entries = NULL;
        }
        s_maps[id].active = false;
    }
}

void ds_map_clear(int32_t id) {
    if (id >= 0 && id < MAX_DS_HANDLES && s_maps[id].active) {
        for (int32_t i = 0; i < s_maps[id].count; i++) {
            RValue_Free(&s_maps[id].entries[i].key);
            RValue_Free(&s_maps[id].entries[i].value);
        }
        s_maps[id].count = 0;
    }
}

int32_t ds_map_size(int32_t id) {
    if (id >= 0 && id < MAX_DS_HANDLES && s_maps[id].active) {
        return s_maps[id].count;
    }
    return 0;
}

bool ds_map_empty(int32_t id) {
    return ds_map_size(id) == 0;
}

static int32_t map_find_index(InternalMap* m, const RValue* key) {
    for (int32_t i = 0; i < m->count; i++) {
        if (YYCompare(&m->entries[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

bool ds_map_add(int32_t id, const RValue* key, const RValue* val) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_maps[id].active) return false;
    InternalMap* m = &s_maps[id];
    if (map_find_index(m, key) != -1) return false;
    if (m->count >= m->capacity) {
        int32_t new_cap = m->capacity * 2;
        MapEntry* new_entries = (MapEntry*)realloc(m->entries, new_cap * sizeof(MapEntry));
        if (!new_entries) return false;
        m->entries = new_entries;
        m->capacity = new_cap;
    }
    RValue_Init(&m->entries[m->count].key);
    RValue_Init(&m->entries[m->count].value);
    RValue_Copy(&m->entries[m->count].key, key);
    RValue_Copy(&m->entries[m->count].value, val);
    m->count++;
    return true;
}

void ds_map_replace(int32_t id, const RValue* key, const RValue* val) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_maps[id].active) return;
    InternalMap* m = &s_maps[id];
    int32_t idx = map_find_index(m, key);
    if (idx != -1) {
        RValue_Copy(&m->entries[idx].value, val);
    } else {
        ds_map_add(id, key, val);
    }
}

RValue* ds_map_find_value(int32_t id, const RValue* key) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_maps[id].active) return NULL;
    InternalMap* m = &s_maps[id];
    int32_t idx = map_find_index(m, key);
    if (idx != -1) {
        return &m->entries[idx].value;
    }
    return NULL;
}

bool ds_map_exists(int32_t id, const RValue* key) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_maps[id].active) return false;
    return map_find_index(&s_maps[id], key) != -1;
}

void ds_map_delete(int32_t id, const RValue* key) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_maps[id].active) return;
    InternalMap* m = &s_maps[id];
    int32_t idx = map_find_index(m, key);
    if (idx != -1) {
        RValue_Free(&m->entries[idx].key);
        RValue_Free(&m->entries[idx].value);
        memmove(&m->entries[idx], &m->entries[idx + 1], (m->count - idx - 1) * sizeof(MapEntry));
        m->count--;
    }
}

// -------------------------------------------------------------
// DS Grid Implementation
// -------------------------------------------------------------
typedef struct InternalGrid {
    bool active;
    int32_t width;
    int32_t height;
    RValue* cells;
} InternalGrid;

static InternalGrid s_grids[MAX_DS_HANDLES];

int32_t ds_grid_create(int32_t w, int32_t h) {
    if (w <= 0 || h <= 0) return -1;
    for (int32_t i = 0; i < MAX_DS_HANDLES; i++) {
        if (!s_grids[i].active) {
            s_grids[i].active = true;
            s_grids[i].width = w;
            s_grids[i].height = h;
            s_grids[i].cells = (RValue*)calloc(w * h, sizeof(RValue));
            for (int32_t j = 0; j < w * h; j++) {
                s_grids[i].cells[j].type = VALUE_REAL;
                s_grids[i].cells[j].val = 0.0;
            }
            return i;
        }
    }
    return -1;
}

void ds_grid_destroy(int32_t id) {
    if (id >= 0 && id < MAX_DS_HANDLES && s_grids[id].active) {
        if (s_grids[id].cells) {
            for (int32_t j = 0; j < s_grids[id].width * s_grids[id].height; j++) {
                RValue_Free(&s_grids[id].cells[j]);
            }
            free(s_grids[id].cells);
            s_grids[id].cells = NULL;
        }
        s_grids[id].active = false;
    }
}

void ds_grid_resize(int32_t id, int32_t w, int32_t h) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_grids[id].active || w <= 0 || h <= 0) return;
    InternalGrid* g = &s_grids[id];
    RValue* new_cells = (RValue*)calloc(w * h, sizeof(RValue));
    for (int32_t cy = 0; cy < h; cy++) {
        for (int32_t cx = 0; cx < w; cx++) {
            int32_t n_idx = cy * w + cx;
            if (cx < g->width && cy < g->height) {
                int32_t o_idx = cy * g->width + cx;
                RValue_Copy(&new_cells[n_idx], &g->cells[o_idx]);
                RValue_Free(&g->cells[o_idx]);
            } else {
                new_cells[n_idx].type = VALUE_REAL;
                new_cells[n_idx].val = 0.0;
            }
        }
    }
    free(g->cells);
    g->cells = new_cells;
    g->width = w;
    g->height = h;
}

void ds_grid_clear(int32_t id, const RValue* val) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_grids[id].active) return;
    InternalGrid* g = &s_grids[id];
    for (int32_t j = 0; j < g->width * g->height; j++) {
        RValue_Copy(&g->cells[j], val);
    }
}

void ds_grid_set(int32_t id, int32_t x, int32_t y, const RValue* val) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_grids[id].active) return;
    InternalGrid* g = &s_grids[id];
    if (x >= 0 && x < g->width && y >= 0 && y < g->height) {
        RValue_Copy(&g->cells[y * g->width + x], val);
    }
}

RValue* ds_grid_get(int32_t id, int32_t x, int32_t y) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_grids[id].active) return NULL;
    InternalGrid* g = &s_grids[id];
    if (x >= 0 && x < g->width && y >= 0 && y < g->height) {
        return &g->cells[y * g->width + x];
    }
    return NULL;
}

int32_t ds_grid_width(int32_t id) {
    if (id >= 0 && id < MAX_DS_HANDLES && s_grids[id].active) return s_grids[id].width;
    return 0;
}

int32_t ds_grid_height(int32_t id) {
    if (id >= 0 && id < MAX_DS_HANDLES && s_grids[id].active) return s_grids[id].height;
    return 0;
}

// -------------------------------------------------------------
// DS Stack Implementation
// -------------------------------------------------------------
int32_t ds_stack_create(void) {
    return ds_list_create();
}

void ds_stack_destroy(int32_t id) {
    ds_list_destroy(id);
}

void ds_stack_clear(int32_t id) {
    ds_list_clear(id);
}

int32_t ds_stack_size(int32_t id) {
    return ds_list_size(id);
}

bool ds_stack_empty(int32_t id) {
    return ds_list_empty(id);
}

void ds_stack_push(int32_t id, const RValue* val) {
    ds_list_add(id, val);
}

static RValue s_ds_temp_pop;

RValue* ds_stack_pop(int32_t id) {
    int32_t sz = ds_list_size(id);
    if (sz > 0) {
        RValue* top = ds_list_find_value(id, sz - 1);
        RValue_Copy(&s_ds_temp_pop, top);
        ds_list_delete(id, sz - 1);
        return &s_ds_temp_pop;
    }
    s_ds_temp_pop.type = VALUE_UNDEFINED;
    return &s_ds_temp_pop;
}

RValue* ds_stack_top(int32_t id) {
    int32_t sz = ds_list_size(id);
    if (sz > 0) {
        return ds_list_find_value(id, sz - 1);
    }
    return NULL;
}

// -------------------------------------------------------------
// DS Queue Implementation
// -------------------------------------------------------------
int32_t ds_queue_create(void) {
    return ds_list_create();
}

void ds_queue_destroy(int32_t id) {
    ds_list_destroy(id);
}

void ds_queue_clear(int32_t id) {
    ds_list_clear(id);
}

int32_t ds_queue_size(int32_t id) {
    return ds_list_size(id);
}

bool ds_queue_empty(int32_t id) {
    return ds_list_empty(id);
}

void ds_queue_enqueue(int32_t id, const RValue* val) {
    ds_list_add(id, val);
}

RValue* ds_queue_dequeue(int32_t id) {
    int32_t sz = ds_list_size(id);
    if (sz > 0) {
        RValue* head = ds_list_find_value(id, 0);
        RValue_Copy(&s_ds_temp_pop, head);
        ds_list_delete(id, 0);
        return &s_ds_temp_pop;
    }
    s_ds_temp_pop.type = VALUE_UNDEFINED;
    return &s_ds_temp_pop;
}

RValue* ds_queue_head(int32_t id) {
    return ds_list_find_value(id, 0);
}

// -------------------------------------------------------------
// DS Priority Queue Implementation
// -------------------------------------------------------------
typedef struct PriorityEntry {
    RValue val;
    double priority;
} PriorityEntry;

typedef struct InternalPriority {
    bool active;
    int32_t count;
    int32_t capacity;
    PriorityEntry* items;
} InternalPriority;

static InternalPriority s_priorities[MAX_DS_HANDLES];

int32_t ds_priority_create(void) {
    for (int32_t i = 0; i < MAX_DS_HANDLES; i++) {
        if (!s_priorities[i].active) {
            s_priorities[i].active = true;
            s_priorities[i].count = 0;
            s_priorities[i].capacity = 8;
            s_priorities[i].items = (PriorityEntry*)calloc(8, sizeof(PriorityEntry));
            return i;
        }
    }
    return -1;
}

void ds_priority_destroy(int32_t id) {
    if (id >= 0 && id < MAX_DS_HANDLES && s_priorities[id].active) {
        ds_priority_clear(id);
        if (s_priorities[id].items) {
            free(s_priorities[id].items);
            s_priorities[id].items = NULL;
        }
        s_priorities[id].active = false;
    }
}

void ds_priority_clear(int32_t id) {
    if (id >= 0 && id < MAX_DS_HANDLES && s_priorities[id].active) {
        for (int32_t i = 0; i < s_priorities[id].count; i++) {
            RValue_Free(&s_priorities[id].items[i].val);
        }
        s_priorities[id].count = 0;
    }
}

int32_t ds_priority_size(int32_t id) {
    if (id >= 0 && id < MAX_DS_HANDLES && s_priorities[id].active) return s_priorities[id].count;
    return 0;
}

bool ds_priority_empty(int32_t id) {
    return ds_priority_size(id) == 0;
}

void ds_priority_add(int32_t id, const RValue* val, double priority) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_priorities[id].active) return;
    InternalPriority* p = &s_priorities[id];
    if (p->count >= p->capacity) {
        int32_t new_cap = p->capacity * 2;
        PriorityEntry* new_items = (PriorityEntry*)realloc(p->items, new_cap * sizeof(PriorityEntry));
        if (!new_items) return;
        p->items = new_items;
        p->capacity = new_cap;
    }
    RValue_Init(&p->items[p->count].val);
    RValue_Copy(&p->items[p->count].val, val);
    p->items[p->count].priority = priority;
    p->count++;
}

RValue* ds_priority_delete_max(int32_t id) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_priorities[id].active) return NULL;
    InternalPriority* p = &s_priorities[id];
    if (p->count <= 0) return NULL;
    int32_t max_idx = 0;
    for (int32_t i = 1; i < p->count; i++) {
        if (p->items[i].priority > p->items[max_idx].priority) max_idx = i;
    }
    RValue_Copy(&s_ds_temp_pop, &p->items[max_idx].val);
    RValue_Free(&p->items[max_idx].val);
    memmove(&p->items[max_idx], &p->items[max_idx + 1], (p->count - max_idx - 1) * sizeof(PriorityEntry));
    p->count--;
    return &s_ds_temp_pop;
}

RValue* ds_priority_delete_min(int32_t id) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_priorities[id].active) return NULL;
    InternalPriority* p = &s_priorities[id];
    if (p->count <= 0) return NULL;
    int32_t min_idx = 0;
    for (int32_t i = 1; i < p->count; i++) {
        if (p->items[i].priority < p->items[min_idx].priority) min_idx = i;
    }
    RValue_Copy(&s_ds_temp_pop, &p->items[min_idx].val);
    RValue_Free(&p->items[min_idx].val);
    memmove(&p->items[min_idx], &p->items[min_idx + 1], (p->count - min_idx - 1) * sizeof(PriorityEntry));
    p->count--;
    return &s_ds_temp_pop;
}

RValue* ds_priority_find_max(int32_t id) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_priorities[id].active) return NULL;
    InternalPriority* p = &s_priorities[id];
    if (p->count <= 0) return NULL;
    int32_t max_idx = 0;
    for (int32_t i = 1; i < p->count; i++) {
        if (p->items[i].priority > p->items[max_idx].priority) max_idx = i;
    }
    return &p->items[max_idx].val;
}

RValue* ds_priority_find_min(int32_t id) {
    if (id < 0 || id >= MAX_DS_HANDLES || !s_priorities[id].active) return NULL;
    InternalPriority* p = &s_priorities[id];
    if (p->count <= 0) return NULL;
    int32_t min_idx = 0;
    for (int32_t i = 1; i < p->count; i++) {
        if (p->items[i].priority < p->items[min_idx].priority) min_idx = i;
    }
    return &p->items[min_idx].val;
}

void DS_Init(void) {
    memset(s_lists, 0, sizeof(s_lists));
    memset(s_maps, 0, sizeof(s_maps));
    memset(s_grids, 0, sizeof(s_grids));
    memset(s_priorities, 0, sizeof(s_priorities));
}

void DS_Reset(void) {
    for (int32_t i = 0; i < MAX_DS_HANDLES; i++) {
        if (s_lists[i].active) ds_list_destroy(i);
        if (s_maps[i].active) ds_map_destroy(i);
        if (s_grids[i].active) ds_grid_destroy(i);
        if (s_priorities[i].active) ds_priority_destroy(i);
    }
}
