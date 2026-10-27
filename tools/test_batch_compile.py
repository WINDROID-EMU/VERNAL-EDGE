#!/usr/bin/env python3
import os
import re
import subprocess
from concurrent.futures import ProcessPoolExecutor

GML_DIR = "exe_decompiled/gml"
SCRATCH_DIR = "runner/scratch/batch"
INCLUDE_DIR = "runner/include"

os.makedirs(SCRATCH_DIR, exist_ok=True)

subword_rules = [
    (re.compile(r'([A-Za-z0-9_]+)\._0_4_'), r'GHIDRA_SUB_0_4(\1)'),
    (re.compile(r'([A-Za-z0-9_]+)\._4_4_'), r'GHIDRA_SUB_4_4(\1)'),
    (re.compile(r'([A-Za-z0-9_]+)\._4_1_'), r'GHIDRA_SUB_4_1(\1)'),
    (re.compile(r'([A-Za-z0-9_]+)\._0_8_'), r'GHIDRA_SUB_0_8(\1)'),
    (re.compile(r'([A-Za-z0-9_]+)\._4_8_'), r'GHIDRA_SUB_4_8(\1)')
]

def compile_file(f):
    src_path = os.path.join(GML_DIR, f)
    with open(src_path, "r", encoding="utf-8", errors="ignore") as fp:
        code = fp.read()
    
    # Extract function name from filename before '@'
    fn_name = f.split('@')[0]
    
    for pat, rep in subword_rules:
        code = pat.sub(rep, code)
    
    header = f'#define {fn_name} __self_def_{fn_name}\n#include "gml_types.h"\n#include "runner_stubs.h"\n#include "gml_prototypes.h"\n#undef {fn_name}\n'
    clean_c_path = os.path.join(SCRATCH_DIR, f)
    obj_path = os.path.join(SCRATCH_DIR, f[:-2] + ".o")
    
    with open(clean_c_path, "w", encoding="utf-8") as fp:
        fp.write(header + code)
    
    cmd = [
        "clang", "-c", "-std=gnu89", "-Wno-everything",
        f"-I{INCLUDE_DIR}", clean_c_path, "-o", obj_path
    ]
    res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if res.returncode != 0:
        return (f, False, res.stderr[:300])
    return (f, True, "")

def main():
    files = [f for f in os.listdir(GML_DIR) if f.endswith(".c")][:200]
    print(f"Testing compilation of {len(files)} diverse GML files...")
    
    ok_count = 0
    fail_count = 0
    errors = []
    
    with ProcessPoolExecutor() as executor:
        for f, ok, err in executor.map(compile_file, files):
            if ok:
                ok_count += 1
            else:
                fail_count += 1
                errors.append((f, err))
    
    print(f"Batch Result: {ok_count}/{len(files)} OK ({fail_count} failed)")
    if errors:
        print("Sample errors:")
        for fn, err in errors[:5]:
            print(f"[{fn}]: {err}\n")

if __name__ == "__main__":
    main()
