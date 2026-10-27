#!/usr/bin/env python3
import os
import struct

EXE_PATH = "VernalEdge.exe"
OUTPUT_C = "runner/src/builtins_dispatcher.c"
OUTPUT_H = "runner/include/builtins_dispatcher.h"

def load_pe():
    with open(EXE_PATH, "rb") as f:
        pe = f.read()
    
    e_lfanew = struct.unpack_from("<I", pe, 0x3c)[0]
    image_base = struct.unpack_from("<Q", pe, e_lfanew + 0x30)[0]
    num_sections = struct.unpack_from("<H", pe, e_lfanew + 0x06)[0]
    opt_hdr_size = struct.unpack_from("<H", pe, e_lfanew + 0x14)[0]
    sec_hdr_start = e_lfanew + 0x18 + opt_hdr_size

    sections = []
    for i in range(num_sections):
        sec = pe[sec_hdr_start + i*40 : sec_hdr_start + (i+1)*40]
        name = sec[:8].rstrip(b"\x00").decode("latin1")
        vsize, vaddr, raw_size, raw_ptr = struct.unpack_from("<IIII", sec, 8)
        sections.append((name, vaddr, vsize, raw_ptr, raw_size))

    def va_to_file_off(va):
        rva = va - image_base
        for name, vaddr, vsize, raw_ptr, raw_size in sections:
            if vaddr <= rva < vaddr + vsize:
                return rva - vaddr + raw_ptr
        return None

    def read_str_at_va(va):
        foff = va_to_file_off(va)
        if foff is None or foff >= len(pe): return None
        end = pe.find(b"\x00", foff)
        if end != -1 and end - foff < 256:
            try:
                return pe[foff:end].decode("utf-8")
            except:
                return None
        return None

    return pe, va_to_file_off, read_str_at_va

def main():
    print("Loading PE routine table...")
    pe, va_to_file_off, read_str_at_va = load_pe()
    
    start_va = 0x1424a6130
    end_va = 0x1424aa560
    
    routines = []
    for va in range(start_va, end_va + 16, 16):
        foff = va_to_file_off(va)
        if foff is None: continue
        ptr = struct.unpack_from("<Q", pe, foff)[0]
        name = read_str_at_va(ptr)
        idx_va = va + 8
        routines.append((name, idx_va))

    print(f"Loaded {len(routines)} routines.")

    # Generate header
    with open(OUTPUT_H, "w", encoding="utf-8") as f:
        f.write("#ifndef RUNNER_BUILTINS_DISPATCHER_H\n#define RUNNER_BUILTINS_DISPATCHER_H\n\n#include \"rvalue.h\"\n#include \"ds.h\"\n#include \"instance.h\"\n#include \"room.h\"\n#include \"gml_globals.h\"\n\n")
        f.write("#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n")
        f.write("void BuiltinsDispatcher_Init(void);\n")
        f.write("void* FUN_14183b9b0(void* self, void* other, RValue* res, int argc, uint64_t routine_idx, RValue** argv);\n")
        f.write("\n#ifdef __cplusplus\n}\n#endif\n\n#endif // RUNNER_BUILTINS_DISPATCHER_H\n")

    # Generate C implementation
    with open(OUTPUT_C, "w", encoding="utf-8") as f:
        f.write('#include "builtins_dispatcher.h"\n#include <stdio.h>\n#include <math.h>\n\n')
        
        f.write("""// Method setup
void SetupMethod(void* res, void* func, void* inst) {
    (void)inst;
    RValue* r = (RValue*)res;
    RValue_Init(r);
    r->ptr = func;
    r->type = VALUE_OBJECT;
}

void* Variable_GetArrayElem(RValue* arr, int32_t idx, ...) {
    if (!arr) return NULL;
    if ((arr->type & 0xffffff) == VALUE_ARRAY && arr->ptr) {
        return RefArray_Get((RefArray*)arr->ptr, idx);
    }
    return NULL;
}

void Variable_SetArrayElem(RValue* arr, int32_t idx, const RValue* val) {
    if (!arr) return;
    if ((arr->type & 0xffffff) == VALUE_ARRAY && arr->ptr) {
        RefArray_Set((RefArray*)arr->ptr, idx, val);
    }
}

void CallScriptMethod(void* self, void* other, void* res, int argc, ...) {
    (void)self; (void)other; (void)res; (void)argc;
}

void BuiltinsDispatcher_Init(void) {
    // Routine IDs are initialized to their VAs for fast switch dispatch
""")
        for name, idx_va in routines:
            f.write(f"    DAT_{idx_va:x} = 0x{idx_va:x};\n")
        
        f.write("""}

void* FUN_14183b9b0(void* self, void* other, RValue* res, int argc, uint64_t routine_idx, RValue** argv) {
    (void)self; (void)other;
    if (res) RValue_Init(res);

    switch (routine_idx) {
""")
        
        for name, idx_va in routines:
            f.write(f"        case 0x{idx_va:x}: // {name}\n")
            if name == "ds_map_create":
                f.write("            RValue_SetReal(res, ds_map_create());\n            return res;\n")
            elif name == "ds_map_destroy":
                f.write("            if (argc > 0) ds_map_destroy((int32_t)RValue_AsReal(argv[0]));\n            return res;\n")
            elif name == "ds_map_add":
                f.write("            if (argc >= 3) RValue_SetBool(res, ds_map_add((int32_t)RValue_AsReal(argv[0]), argv[1], argv[2]));\n            return res;\n")
            elif name == "ds_map_find_value":
                f.write("            if (argc >= 2) {\n")
                f.write("                RValue* v = ds_map_find_value((int32_t)RValue_AsReal(argv[0]), argv[1]);\n")
                f.write("                if (v) RValue_Copy(res, v);\n")
                f.write("            }\n            return res;\n")
            elif name == "ds_map_exists":
                f.write("            if (argc >= 2) RValue_SetBool(res, ds_map_exists((int32_t)RValue_AsReal(argv[0]), argv[1]));\n            return res;\n")
            elif name == "ds_list_create":
                f.write("            RValue_SetReal(res, ds_list_create());\n            return res;\n")
            elif name == "ds_list_destroy":
                f.write("            if (argc > 0) ds_list_destroy((int32_t)RValue_AsReal(argv[0]));\n            return res;\n")
            elif name == "ds_list_add":
                f.write("            if (argc >= 2) ds_list_add((int32_t)RValue_AsReal(argv[0]), argv[1]);\n            return res;\n")
            elif name == "ds_list_find_value":
                f.write("            if (argc >= 2) {\n")
                f.write("                RValue* v = ds_list_find_value((int32_t)RValue_AsReal(argv[0]), (int32_t)RValue_AsReal(argv[1]));\n")
                f.write("                if (v) RValue_Copy(res, v);\n")
                f.write("            }\n            return res;\n")
            elif name == "ds_list_size":
                f.write("            if (argc > 0) RValue_SetReal(res, ds_list_size((int32_t)RValue_AsReal(argv[0])));\n            return res;\n")
            elif name == "instance_create_depth":
                f.write("            if (argc >= 4) {\n")
                f.write("                CInstance* inst = instance_create_depth(RValue_AsReal(argv[0]), RValue_AsReal(argv[1]), RValue_AsReal(argv[2]), (int32_t)RValue_AsReal(argv[3]));\n")
                f.write("                RValue_SetReal(res, inst ? inst->id : -1);\n")
                f.write("            }\n            return res;\n")
            elif name == "instance_destroy":
                f.write("            if (argc > 0) instance_destroy((int32_t)RValue_AsReal(argv[0]));\n            return res;\n")
            elif name == "instance_exists":
                f.write("            if (argc > 0) RValue_SetBool(res, instance_exists((int32_t)RValue_AsReal(argv[0])));\n            return res;\n")
            elif name == "room_goto":
                f.write("            if (argc > 0) room_goto((int32_t)RValue_AsReal(argv[0]));\n            return res;\n")
            elif name == "room_restart":
                f.write("            room_restart();\n            return res;\n")
            elif name == "show_debug_message":
                f.write("            if (argc > 0) printf(\"[GML DEBUG] %s\\n\", RValue_AsString(argv[0]));\n            return res;\n")
            else:
                f.write("            return res;\n")

        f.write("""        default:
            return res;
    }
}
""")
    print(f"Generated {OUTPUT_C} and {OUTPUT_H} successfully.")

if __name__ == "__main__":
    main()
