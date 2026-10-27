#!/usr/bin/env python3
"""
Pipeline de compilação da lógica do jogo (gml/) para biblioteca estática C.
Normaliza pseudo-código do Ghidra e compila com clang em paralelo.
"""

import os
import re
import sys
import subprocess
from concurrent.futures import ProcessPoolExecutor

GML_DIR = "exe_decompiled/gml"
CLEAN_DIR = "runner/gml_clean"
OBJ_DIR = "runner/obj"
INCLUDE_DIR = "runner/include"
OUTPUT_LIB = "runner/libgml.a"

os.makedirs(CLEAN_DIR, exist_ok=True)
os.makedirs(OBJ_DIR, exist_ok=True)

subword_rules = [
    (re.compile(r'([A-Za-z0-9_]+)\._0_4_'), r'GHIDRA_SUB_0_4(\1)'),
    (re.compile(r'([A-Za-z0-9_]+)\._4_4_'), r'GHIDRA_SUB_4_4(\1)'),
    (re.compile(r'([A-Za-z0-9_]+)\._4_1_'), r'GHIDRA_SUB_4_1(\1)'),
    (re.compile(r'([A-Za-z0-9_]+)\._0_8_'), r'GHIDRA_SUB_0_8(\1)'),
    (re.compile(r'([A-Za-z0-9_]+)\._4_8_'), r'GHIDRA_SUB_4_8(\1)')
]

def process_and_compile(fname):
    src_path = os.path.join(GML_DIR, fname)
    clean_path = os.path.join(CLEAN_DIR, fname)
    obj_path = os.path.join(OBJ_DIR, fname[:-2] + ".o")
    
    # Se o .o já existe e é mais novo que o fonte, pula
    if os.path.exists(obj_path) and os.path.getmtime(obj_path) >= os.path.getmtime(src_path):
        return (fname, True, "skipped")

    with open(src_path, "r", encoding="utf-8", errors="ignore") as fp:
        code = fp.read()
    
    fn_name = fname.split('@')[0]
    for pat, rep in subword_rules:
        code = pat.sub(rep, code)
    
    header = (
        f'#define {fn_name} __self_def_{fn_name}\n'
        f'#include "gml_types.h"\n'
        f'#include "runner_stubs.h"\n'
        f'#include "gml_prototypes.h"\n'
        f'#undef {fn_name}\n'
    )
    
    with open(clean_path, "w", encoding="utf-8") as fp:
        fp.write(header + code)
    
    cmd = [
        "clang", "-c", "-std=gnu89", "-O2", "-Wno-everything",
        f"-I{INCLUDE_DIR}", clean_path, "-o", obj_path
    ]
    res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if res.returncode != 0:
        return (fname, False, res.stderr[:300])
    return (fname, True, "compiled")

def main():
    max_files = int(sys.argv[1]) if len(sys.argv) > 1 else None
    all_files = sorted([f for f in os.listdir(GML_DIR) if f.endswith(".c")])
    if max_files:
        all_files = all_files[:max_files]
    
    print(f"=== Compilando biblioteca libgml ({len(all_files)} arquivos) ===")
    
    success = 0
    failed = 0
    errors = []
    
    with ProcessPoolExecutor() as executor:
        for fname, ok, status in executor.map(process_and_compile, all_files):
            if ok:
                success += 1
            else:
                failed += 1
                errors.append((fname, status))
            
            total_done = success + failed
            if total_done % 500 == 0 or total_done == len(all_files):
                print(f"Progresso: {total_done}/{len(all_files)} (OK={success}, Falhas={failed})")

    print(f"\nResultado final: {success}/{len(all_files)} compilados com sucesso.")
    if errors:
        print(f"Total de falhas: {len(errors)}")
        for fn, err in errors[:10]:
            print(f"[{fn}]: {err}\n")

if __name__ == "__main__":
    main()
