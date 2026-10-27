#ifndef RUNNER_RVALUE_H
#define RUNNER_RVALUE_H

#include "gml_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Refcounted String wrapper
typedef struct RefString {
    int32_t refcount;
    int32_t length;
    char data[1]; // Flexible array member
} RefString;

// Refcounted Array wrapper
typedef struct RefArray {
    int32_t refcount;
    int32_t length;
    int32_t capacity;
    RValue* elements;
} RefArray;

// Struct key-value entry
typedef struct StructEntry {
    int32_t key;
    char* name;
    RValue value;
} StructEntry;

// Refcounted Struct / Object wrapper
typedef struct RefStruct {
    int32_t refcount;
    int32_t count;
    int32_t capacity;
    StructEntry* entries;
} RefStruct;

// RValue Core Lifecycle
void RValue_Init(RValue* val);
void RValue_Free(RValue* val);
void RValue_Copy(RValue* dst, const RValue* src);
void RValue_Set(RValue* dst, const RValue* src);

// Type Setters
void RValue_SetReal(RValue* dst, double r);
void RValue_SetInt32(RValue* dst, int32_t i);
void RValue_SetInt64(RValue* dst, int64_t i);
void RValue_SetBool(RValue* dst, bool b);
void RValue_SetString(RValue* dst, const char* str);
void RValue_SetPtr(RValue* dst, void* ptr);
void RValue_SetUndefined(RValue* dst);
void RValue_SetArray(RValue* dst, RefArray* arr);
void RValue_SetStruct(RValue* dst, RefStruct* strct);

// Type Getters / Converters
double RValue_AsReal(const RValue* val);
int32_t RValue_AsInt32(const RValue* val);
int64_t RValue_AsInt64(const RValue* val);
bool RValue_AsBool(const RValue* val);
const char* RValue_AsString(const RValue* val);
void* RValue_AsPointer(const RValue* val);
bool RValue_IsUndefined(const RValue* val);
bool RValue_IsNull(const RValue* val);

// Refcounted String / Array Helpers
RefString* RefString_Create(const char* str);
void RefString_AddRef(RefString* s);
void RefString_Release(RefString* s);

RefArray* RefArray_Create(int32_t length);
void RefArray_AddRef(RefArray* arr);
void RefArray_Release(RefArray* arr);
RValue* RefArray_Get(RefArray* arr, int32_t index);
void RefArray_Set(RefArray* arr, int32_t index, const RValue* val);

RefStruct* RefStruct_Create(void);
void RefStruct_AddRef(RefStruct* strct);
void RefStruct_Release(RefStruct* strct);
RValue* RefStruct_Get(RefStruct* strct, int32_t key);
void RefStruct_Set(RefStruct* strct, int32_t key, const RValue* val);

// Comparison Operations
// op: 0 ==, 1 !=, 2 <, 3 <=, 4 >, 5 >=
int YYCompare(const RValue* a, const RValue* b, ...);

// Memory Manager (Scope / GC Stack)
void MemoryManager_Init(void);
void MemoryManager_Push(void* obj);
void MemoryManager_Pop(int count);
void MemoryManager_Clean(void);

#ifdef __cplusplus
}
#endif

#endif // RUNNER_RVALUE_H
