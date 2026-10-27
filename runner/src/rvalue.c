#include "rvalue.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

// -------------------------------------------------------------
// Refcounted Strings
// -------------------------------------------------------------
RefString* RefString_Create(const char* str) {
    if (!str) str = "";
    size_t len = strlen(str);
    RefString* rs = (RefString*)malloc(sizeof(RefString) + len);
    if (!rs) return NULL;
    rs->refcount = 1;
    rs->length = (int32_t)len;
    memcpy(rs->data, str, len + 1);
    return rs;
}

void RefString_AddRef(RefString* s) {
    if (s) {
        s->refcount++;
    }
}

void RefString_Release(RefString* s) {
    if (s) {
        if (--s->refcount <= 0) {
            free(s);
        }
    }
}

// -------------------------------------------------------------
// Refcounted Arrays
// -------------------------------------------------------------
RefArray* RefArray_Create(int32_t length) {
    if (length < 0) length = 0;
    int32_t capacity = length < 4 ? 4 : length;
    RefArray* arr = (RefArray*)malloc(sizeof(RefArray));
    if (!arr) return NULL;
    arr->refcount = 1;
    arr->length = length;
    arr->capacity = capacity;
    arr->elements = (RValue*)calloc(capacity, sizeof(RValue));
    for (int32_t i = 0; i < length; i++) {
        arr->elements[i].type = VALUE_UNDEFINED;
    }
    return arr;
}

void RefArray_AddRef(RefArray* arr) {
    if (arr) {
        arr->refcount++;
    }
}

void RefArray_Release(RefArray* arr) {
    if (arr) {
        if (--arr->refcount <= 0) {
            if (arr->elements) {
                for (int32_t i = 0; i < arr->length; i++) {
                    RValue_Free(&arr->elements[i]);
                }
                free(arr->elements);
            }
            free(arr);
        }
    }
}

RValue* RefArray_Get(RefArray* arr, int32_t index) {
    if (!arr || index < 0) return NULL;
    if (index >= arr->length) {
        // Auto-expand array if indexed beyond current length (GameMaker style)
        if (index >= arr->capacity) {
            int32_t new_cap = (index + 1) * 2;
            RValue* new_elems = (RValue*)realloc(arr->elements, new_cap * sizeof(RValue));
            if (!new_elems) return NULL;
            for (int32_t i = arr->capacity; i < new_cap; i++) {
                new_elems[i].val = 0;
                new_elems[i].flags = 0;
                new_elems[i].type = VALUE_UNDEFINED;
            }
            arr->elements = new_elems;
            arr->capacity = new_cap;
        }
        arr->length = index + 1;
    }
    return &arr->elements[index];
}

void RefArray_Set(RefArray* arr, int32_t index, const RValue* val) {
    RValue* slot = RefArray_Get(arr, index);
    if (slot) {
        RValue_Copy(slot, val);
    }
}

// -------------------------------------------------------------
// Refcounted Structs / Objects
// -------------------------------------------------------------
RefStruct* RefStruct_Create(void) {
    RefStruct* strct = (RefStruct*)malloc(sizeof(RefStruct));
    if (!strct) return NULL;
    strct->refcount = 1;
    strct->count = 0;
    strct->capacity = 8;
    strct->entries = (StructEntry*)calloc(strct->capacity, sizeof(StructEntry));
    return strct;
}

void RefStruct_AddRef(RefStruct* strct) {
    if (strct) {
        strct->refcount++;
    }
}

void RefStruct_Release(RefStruct* strct) {
    if (strct) {
        if (--strct->refcount <= 0) {
            if (strct->entries) {
                for (int32_t i = 0; i < strct->count; i++) {
                    if (strct->entries[i].name) free(strct->entries[i].name);
                    RValue_Free(&strct->entries[i].value);
                }
                free(strct->entries);
            }
            free(strct);
        }
    }
}

RValue* RefStruct_Get(RefStruct* strct, int32_t key) {
    if (!strct) return NULL;
    for (int32_t i = 0; i < strct->count; i++) {
        if (strct->entries[i].key == key) {
            return &strct->entries[i].value;
        }
    }
    return NULL;
}

void RefStruct_Set(RefStruct* strct, int32_t key, const RValue* val) {
    if (!strct) return;
    for (int32_t i = 0; i < strct->count; i++) {
        if (strct->entries[i].key == key) {
            RValue_Copy(&strct->entries[i].value, val);
            return;
        }
    }
    if (strct->count >= strct->capacity) {
        int32_t new_cap = strct->capacity * 2;
        StructEntry* new_entries = (StructEntry*)realloc(strct->entries, new_cap * sizeof(StructEntry));
        if (!new_entries) return;
        strct->entries = new_entries;
        strct->capacity = new_cap;
    }
    strct->entries[strct->count].key = key;
    strct->entries[strct->count].name = NULL;
    RValue_Init(&strct->entries[strct->count].value);
    RValue_Copy(&strct->entries[strct->count].value, val);
    strct->count++;
}

// -------------------------------------------------------------
// RValue Lifecycle Implementation
// -------------------------------------------------------------
void RValue_Init(RValue* val) {
    if (!val) return;
    val->val = 0;
    val->flags = 0;
    val->type = VALUE_UNDEFINED;
}

void RValue_Free(RValue* val) {
    if (!val) return;
    uint32_t type = val->type & 0xffffff;
    if (type == VALUE_STRING && val->ptr) {
        RefString_Release((RefString*)val->ptr);
    } else if (type == VALUE_ARRAY && val->ptr) {
        RefArray_Release((RefArray*)val->ptr);
    } else if (type == VALUE_OBJECT && val->ptr) {
        RefStruct_Release((RefStruct*)val->ptr);
    }
    val->val = 0;
    val->flags = 0;
    val->type = VALUE_UNDEFINED;
}

void RValue_Copy(RValue* dst, const RValue* src) {
    if (!dst) return;
    if (dst == src) return;
    RValue_Free(dst);
    if (!src) {
        RValue_Init(dst);
        return;
    }
    dst->val = src->val;
    dst->flags = src->flags;
    dst->type = src->type;

    uint32_t type = src->type & 0xffffff;
    if (type == VALUE_STRING && dst->ptr) {
        RefString_AddRef((RefString*)dst->ptr);
    } else if (type == VALUE_ARRAY && dst->ptr) {
        RefArray_AddRef((RefArray*)dst->ptr);
    } else if (type == VALUE_OBJECT && dst->ptr) {
        RefStruct_AddRef((RefStruct*)dst->ptr);
    }
}

void RValue_Set(RValue* dst, const RValue* src) {
    RValue_Copy(dst, src);
}

// Global C wrappers used by GML decompiled calls
void* FREE_RValue(RValue* val) {
    RValue_Free(val);
    return NULL;
}

void* COPY_RValue(RValue* dst, const RValue* src) {
    RValue_Copy(dst, src);
    return dst;
}

void* SET_RValue(RValue* dst, const RValue* src) {
    RValue_Set(dst, src);
    return dst;
}

void FREE_RValue_Array(RValue* arr, int count) {
    if (!arr) return;
    for (int i = 0; i < count; i++) {
        RValue_Free(&arr[i]);
    }
}

void InitArray(RValue* arr, int size) {
    if (!arr) return;
    RefArray* ra = RefArray_Create(size);
    RValue_SetArray(arr, ra);
}

// -------------------------------------------------------------
// Type Setters
// -------------------------------------------------------------
void RValue_SetReal(RValue* dst, double r) {
    RValue_Free(dst);
    dst->val = r;
    dst->type = VALUE_REAL;
}

void RValue_SetInt32(RValue* dst, int32_t i) {
    RValue_Free(dst);
    dst->i32 = i;
    dst->type = VALUE_INT32;
}

void RValue_SetInt64(RValue* dst, int64_t i) {
    RValue_Free(dst);
    dst->i64 = i;
    dst->type = VALUE_INT64;
}

void RValue_SetBool(RValue* dst, bool b) {
    RValue_Free(dst);
    dst->val = b ? 1.0 : 0.0;
    dst->type = VALUE_BOOL;
}

void RValue_SetString(RValue* dst, const char* str) {
    RValue_Free(dst);
    RefString* rs = RefString_Create(str);
    dst->ptr = rs;
    dst->type = VALUE_STRING;
}

void RValue_SetPtr(RValue* dst, void* ptr) {
    RValue_Free(dst);
    dst->ptr = ptr;
    dst->type = VALUE_PTR;
}

void RValue_SetUndefined(RValue* dst) {
    RValue_Free(dst);
}

void RValue_SetArray(RValue* dst, RefArray* arr) {
    RValue_Free(dst);
    dst->ptr = arr;
    dst->type = VALUE_ARRAY;
}

void RValue_SetStruct(RValue* dst, RefStruct* strct) {
    RValue_Free(dst);
    dst->ptr = strct;
    dst->type = VALUE_OBJECT;
}

// -------------------------------------------------------------
// Type Getters / Converters
// -------------------------------------------------------------
double RValue_AsReal(const RValue* val) {
    if (!val) return 0.0;
    uint32_t type = val->type & 0xffffff;
    switch (type) {
        case VALUE_REAL:
        case VALUE_BOOL:
            return val->val;
        case VALUE_INT32:
            return (double)val->i32;
        case VALUE_INT64:
            return (double)val->i64;
        case VALUE_STRING: {
            if (!val->ptr) return 0.0;
            RefString* rs = (RefString*)val->ptr;
            return atof(rs->data);
        }
        default:
            return 0.0;
    }
}

int32_t RValue_AsInt32(const RValue* val) {
    if (!val) return 0;
    return (int32_t)RValue_AsReal(val);
}

int64_t RValue_AsInt64(const RValue* val) {
    if (!val) return 0;
    uint32_t type = val->type & 0xffffff;
    if (type == VALUE_INT64) return val->i64;
    return (int64_t)RValue_AsReal(val);
}

bool RValue_AsBool(const RValue* val) {
    if (!val) return false;
    uint32_t type = val->type & 0xffffff;
    switch (type) {
        case VALUE_REAL:
        case VALUE_BOOL:
            return val->val > 0.5 || val->val < -0.5;
        case VALUE_INT32:
            return val->i32 != 0;
        case VALUE_INT64:
            return val->i64 != 0;
        case VALUE_STRING:
            return val->ptr != NULL;
        case VALUE_OBJECT:
        case VALUE_ARRAY:
        case VALUE_PTR:
            return val->ptr != NULL;
        default:
            return false;
    }
}

const char* RValue_AsString(const RValue* val) {
    if (!val) return "";
    uint32_t type = val->type & 0xffffff;
    if (type == VALUE_STRING && val->ptr) {
        return ((RefString*)val->ptr)->data;
    }
    return "";
}

void* RValue_AsPointer(const RValue* val) {
    if (!val) return NULL;
    return val->ptr;
}

bool RValue_IsUndefined(const RValue* val) {
    if (!val) return true;
    return (val->type & 0xffffff) == VALUE_UNDEFINED;
}

bool RValue_IsNull(const RValue* val) {
    if (!val) return true;
    return (val->type & 0xffffff) == VALUE_JSNULL || val->ptr == NULL;
}

// -------------------------------------------------------------
// YYCompare: General RValue comparison implementation
// -------------------------------------------------------------
int YYCompare(const RValue* a, const RValue* b, ...) {
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;

    uint32_t ta = a->type & 0xffffff;
    uint32_t tb = b->type & 0xffffff;

    // Numerical comparison if both numbers
    bool a_num = (ta == VALUE_REAL || ta == VALUE_INT32 || ta == VALUE_INT64 || ta == VALUE_BOOL);
    bool b_num = (tb == VALUE_REAL || tb == VALUE_INT32 || tb == VALUE_INT64 || tb == VALUE_BOOL);

    if (a_num && b_num) {
        double da = RValue_AsReal(a);
        double db = RValue_AsReal(b);
        if (fabs(da - db) < 1e-9) return 0;
        return (da < db) ? -1 : 1;
    }

    if (ta == VALUE_STRING && tb == VALUE_STRING) {
        const char* sa = RValue_AsString(a);
        const char* sb = RValue_AsString(b);
        return strcmp(sa, sb);
    }

    if (ta != tb) {
        return (int)ta - (int)tb;
    }

    return (a->val == b->val) ? 0 : ((a->val < b->val) ? -1 : 1);
}

// -------------------------------------------------------------
// Memory Manager Stack Implementation
// -------------------------------------------------------------
#define MM_STACK_CAP 8192
static void* mm_stack[MM_STACK_CAP];
static int mm_top = 0;

void MemoryManager_Init(void) {
    mm_top = 0;
}

void MemoryManager_Push(void* obj) {
    if (mm_top < MM_STACK_CAP - 1) {
        mm_stack[mm_top++] = obj;
    }
}

void MemoryManager_Pop(int count) {
    mm_top -= count;
    if (mm_top < 0) mm_top = 0;
}

void MemoryManager_Clean(void) {
    mm_top = 0;
}

void CheckStackPush(int tag) {
    // Debug stack canary check
    (void)tag;
}

void YYError(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[YYError] ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}
