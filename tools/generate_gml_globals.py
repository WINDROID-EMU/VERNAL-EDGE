#!/usr/bin/env python3
import os
import re

GML_DIR = "exe_decompiled/gml"
INDEX_FILE = "exe_decompiled/index.tsv"
OUT_HEADER = "runner/include/gml_globals.h"
OUT_STUBS = "runner/src/gml_globals_stub.c"
OUT_PROTOS = "runner/include/gml_prototypes.h"

os.makedirs("runner/include", exist_ok=True)
os.makedirs("runner/src", exist_ok=True)

print("Scanning GML files for global symbols...")
symbol_pattern = re.compile(r'\b(_?DAT_14[0-9a-fA-F]+|_?PTR_14[0-9a-fA-F]+|_?PTR_[A-Za-z0-9_]+)\b')
all_symbols = set()

files = [os.path.join(GML_DIR, f) for f in os.listdir(GML_DIR) if f.endswith(".c")]
for idx, fpath in enumerate(files):
    with open(fpath, "r", encoding="utf-8", errors="ignore") as f:
        content = f.read()
    for sym in symbol_pattern.findall(content):
        all_symbols.add(sym)

# Add all routine dispatch table symbols (1092 routines)
for va in range(0x1424a6130, 0x1424aa560 + 16, 16):
    idx_va = va + 8
    all_symbols.add(f"DAT_{idx_va:x}")

sorted_symbols = sorted(all_symbols)
print(f"Total unique global symbols: {len(sorted_symbols)}")

special_ptrs = {"DAT_1425fce50", "DAT_1425d4258"}

with open(OUT_HEADER, "w", encoding="utf-8") as f:
    f.write("#ifndef GML_GLOBALS_H\n#define GML_GLOBALS_H\n\n#include <stdint.h>\n\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n")
    f.write("// Windows TLS emulation globals\n")
    f.write("extern void* ThreadLocalStoragePointer;\n")
    f.write("extern uint32_t _tls_index;\n\n")
    for sym in sorted_symbols:
        if sym in special_ptrs:
            f.write(f"extern void** {sym};\n")
        else:
            f.write(f"extern uint64_t {sym};\n")
    f.write("\n#ifdef __cplusplus\n}\n#endif\n\n#endif // GML_GLOBALS_H\n")

print(f"Generated {OUT_HEADER}")

with open(OUT_STUBS, "w", encoding="utf-8") as f:
    f.write('#include "gml_globals.h"\n\n')
    f.write("void* ThreadLocalStoragePointer = 0;\n")
    f.write("uint32_t _tls_index = 0;\n\n")
    for sym in sorted_symbols:
        if sym in special_ptrs:
            f.write(f"void** {sym} = 0;\n")
        else:
            f.write(f"uint64_t {sym} = 0;\n")

print(f"Generated {OUT_STUBS}")

# Generate gml_prototypes.h with correct return types
with open(INDEX_FILE) as f:
    lines = [line.strip().split('\t') for line in f if line.strip()]

gml_funcs = sorted(set(parts[2] for parts in lines if len(parts) >= 3 and parts[1] == 'gml'))

with open(OUT_PROTOS, "w", encoding="utf-8") as f:
    f.write("#ifndef GML_PROTOTYPES_H\n#define GML_PROTOTYPES_H\n\n#include \"gml_types.h\"\n\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n")
    for fn in gml_funcs:
        if fn.startswith("gml_Script") or fn.startswith("gml_GlobalScript"):
            f.write(f"undefined8* {fn}();\n")
        else:
            f.write(f"void {fn}();\n")
    f.write("\n#ifdef __cplusplus\n}\n#endif\n\n#endif // GML_PROTOTYPES_H\n")

print(f"Generated {OUT_PROTOS} with {len(gml_funcs)} function prototypes.")
