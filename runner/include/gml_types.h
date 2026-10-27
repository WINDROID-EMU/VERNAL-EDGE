#ifndef GML_TYPES_H
#define GML_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Ghidra base types
typedef uint8_t   undefined;
typedef uint8_t   undefined1;
typedef uint16_t  undefined2;
typedef uint32_t  undefined4;
typedef uint64_t  undefined8;

typedef uint8_t   byte;
typedef uint16_t  word;
typedef uint32_t  dword;
typedef uint64_t  qword;

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

typedef int64_t   longlong;
typedef uint64_t  ulonglong;

typedef void* (*code)();

// Ghidra concatenation and slicing macros
#define CONCAT44(hi, lo) (((uint64_t)(uint32_t)(hi) << 32) | (uint32_t)(lo))
#define CONCAT31(hi, lo) (((uint32_t)(hi) << 8) | (uint8_t)(lo))
#define CONCAT71(hi, lo) (((uint64_t)(hi) << 8) | (uint8_t)(lo))
#define SUB84(val, off)  ((uint32_t)(((uint64_t)(val)) >> ((off) * 8)))

// Subword helpers (lvalue and rvalue capable via pointer cast)
#define GHIDRA_SUB_0_4(x) (*(uint32_t*)((char*)&(x) + 0))
#define GHIDRA_SUB_4_4(x) (*(uint32_t*)((char*)&(x) + 4))
#define GHIDRA_SUB_4_1(x) (*(uint8_t*)((char*)&(x) + 4))
#define GHIDRA_SUB_0_8(x) (*(uint64_t*)((char*)&(x) + 0))
#define GHIDRA_SUB_4_8(x) (*(uint64_t*)((char*)&(x) + 4))

// Low address constant symbol
#define _DAT_0000000c 0xc

// RValue Type Enum
typedef enum RValueType {
    VALUE_REAL = 0,
    VALUE_STRING = 1,
    VALUE_ARRAY = 2,
    VALUE_PTR = 3,
    VALUE_VEC3 = 4,
    VALUE_UNDEFINED = 5,
    VALUE_OBJECT = 6,
    VALUE_INT32 = 7,
    VALUE_VEC4 = 8,
    VALUE_MATRIX = 9,
    VALUE_INT64 = 10,
    VALUE_ACCESSOR = 11,
    VALUE_JSNULL = 12,
    VALUE_BOOL = 13,
    VALUE_ITERATOR = 14,
    VALUE_REF = 15,
    VALUE_UNSET = 0xffffff
} RValueType;

// GameMaker YYC RValue
typedef struct RValue {
    union {
        double val;
        int64_t i64;
        int32_t i32;
        void* ptr;
        char* str;
    };
    uint32_t flags;
    uint32_t type;
} RValue;

// Forward declarations of instance and object
typedef struct CInstance CInstance;
typedef struct CObject CObject;

// Include all generated globals (DAT_14... symbols)
#include "gml_globals.h"

#ifdef __cplusplus
}
#endif

#endif // GML_TYPES_H
