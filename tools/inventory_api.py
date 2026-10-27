#!/usr/bin/env python3
import os
import re
import struct
from collections import Counter, defaultdict

EXE_PATH = "VernalEdge.exe"
GML_DIR = "exe_decompiled/gml"
ENGINE_DIR = "exe_decompiled/engine"
OUTPUT_MD = "api.md"

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

def get_routine_table(pe, va_to_file_off, read_str_at_va):
    start_va = 0x1424a6130
    end_va = 0x1424aa560
    
    routines = {} # "DAT_1424xxxxx": {"name": ..., "idx_va": ...}
    for va in range(start_va, end_va + 16, 16):
        foff = va_to_file_off(va)
        if foff is None: continue
        ptr = struct.unpack_from("<Q", pe, foff)[0]
        name = read_str_at_va(ptr)
        idx_va = va + 8
        dat_name = f"DAT_{idx_va:x}"
        routines[dat_name] = {
            "name": name,
            "va": va,
            "idx_va": idx_va,
            "dat_name": dat_name
        }
    return routines

def get_engine_registrations():
    reg_funcs = [
        ('Highscore', 'FUN_141998b10'),
        ('Sistema', 'FUN_1418837a0'),
        ('Matemática', 'FUN_141873880'),
        ('Gráficos', 'FUN_14186b0c0'),
        ('Ação', 'FUN_1419e4660'),
        ('Arquivos', 'FUN_1418a3b50'),
        ('Recursos', 'FUN_1419c77f0'),
        ('Interação', 'FUN_14188aec0'),
        ('Estruturas de Dados', 'FUN_1419acdd0'),
        ('Partículas', 'FUN_1419e5b20'),
        ('Diversos', 'FUN_14185c460'),
        ('Tempo', 'FUN_1419e6a10'),
        ('Strings', 'FUN_141898f20'),
        ('Áudio', 'FUN_1419e3b60'),
        ('Física', 'FUN_1419eba40'),
        ('Gamepad', 'FUN_1419edd70'),
        ('Buffers', 'FUN_141887a60'),
        ('Buffers', 'FUN_14189e8f0'),
        ('Rede', 'FUN_1419b8bf0'),
        ('Shaders', 'FUN_14188de30'),
        ('Shaders', 'FUN_141994310'),
        ('YoYo/GML', 'FUN_141956b10'),
        ('YoYo/GML', 'FUN_1419e1440'),
        ('YoYo/GML', 'FUN_1418f3b90'),
        ('Multiplayer', 'FUN_1419cb030')
    ]
    call_pattern = re.compile(r'FUN_1418d30b0\(\s*\"([^\"]+)\"\s*,\s*([&A-Za-z0-9_]+)\s*,\s*([^,\)]+)')
    reg_map = {} # name -> {category, argc, ptr}
    ptr_map = {} # ptr -> {name, category, argc}
    for cat, fname in reg_funcs:
        path = os.path.join(ENGINE_DIR, f"{fname}@{fname[4:]}.c")
        if not os.path.exists(path): continue
        with open(path, "r", encoding="utf-8", errors="ignore") as f:
            content = f.read()
        for name, ptr, argc in call_pattern.findall(content):
            clean_ptr = ptr.replace('&', '')
            reg_map[name] = {"category": cat, "argc": argc.strip(), "ptr": clean_ptr}
            ptr_map[clean_ptr] = {"name": name, "category": cat, "argc": argc.strip()}
    return reg_map, ptr_map

def categorize(name, engine_cat=None):
    n = name.lower()
    if any(n.startswith(x) for x in ['draw_', 'surface_', 'shader_', 'gpu_', 'camera_', 'matrix_', 'display_', 'window_', 'texture_', 'sprite_']):
        return "Gráficos"
    if any(n.startswith(x) for x in ['audio_', 'sound_']):
        return "Áudio"
    if any(n.startswith(x) for x in ['keyboard_', 'gamepad_', 'mouse_', 'device_mouse_', 'io_', 'input_']):
        return "Input"
    if any(n.startswith(x) for x in ['ds_list_', 'ds_map_', 'ds_grid_', 'ds_stack_', 'ds_queue_', 'ds_priority_', 'array_', 'struct_', 'variable_']):
        return "Estruturas de Dados"
    if any(n.startswith(x) for x in ['file_', 'ini_', 'buffer_', 'directory_']):
        return "Arquivos e Buffers"
    if any(n.startswith(x) for x in ['instance_', 'object_', 'room_', 'event_', 'motion_', 'move_', 'place_', 'position_', 'collision_']):
        return "Instâncias e Salas"
    if any(n.startswith(x) for x in ['string_', 'math_', 'random', 'choose', 'clamp', 'lerp', 'min', 'max', 'floor', 'ceil', 'round', 'point_', 'lengthdir_', 'angle_', 'dot_product', 'sin', 'cos', 'tan', 'sqrt', 'power']):
        return "Matemática e Strings"
    if any(n.startswith(x) for x in ['game_', 'show_debug', 'parameter_', 'environment_', 'asset_']):
        return "Sistema"
    return engine_cat or "Geral"

def main():
    print("Loading PE and symbol tables...")
    pe, va_to_file_off, read_str_at_va = load_pe()
    routines = get_routine_table(pe, va_to_file_off, read_str_at_va)
    reg_map, ptr_map = get_engine_registrations()
    print(f"Routines in dispatch table: {len(routines)}")
    print(f"Engine registered builtins: {len(reg_map)}")

    # Core Runner Helpers & Common Runtime Routines
    core_helpers = {
        "FUN_140014ca0": ("FREE_RValue", "Core / RValue", "void FREE_RValue(RValue* val)"),
        "FUN_140014d20": ("COPY_RValue", "Core / RValue", "void COPY_RValue(RValue* dst, RValue* src)"),
        "FUN_140014dd0": ("SET_RValue", "Core / RValue", "void SET_RValue(RValue* dst, RValue* src)"),
        "FUN_141822580": ("RValue::AsBool", "Core / RValue", "bool RValue_AsBool(RValue* val)"),
        "FUN_141823f70": ("RValue::AsReal", "Core / RValue", "double RValue_AsReal(RValue* val)"),
        "FUN_14184ad40": ("YYCompare", "Core / RValue", "int YYCompare(RValue* a, RValue* b, int op)"),
        "FUN_14183f740": ("Variable_Lookup", "Instâncias e Salas", "RValue* Variable_Lookup(CInstance* inst, int var_id, ...)"),
        "FUN_14183c710": ("SetupMethod", "Core / RValue", "void SetupMethod(RValue* res, void* func, CInstance* inst)"),
        "FUN_14183bef0": ("CheckStackPush", "Core / Runtime", "void CheckStackPush(int tag)"),
        "FUN_141840cc0": ("Variable_GetArrayElem", "Estruturas de Dados", "RValue* Variable_GetArrayElem(RValue* arr, int idx, ...)"),
        "FUN_140015210": ("FREE_RValue_Array", "Core / RValue", "void FREE_RValue_Array(RValue* arr, int count)"),
        "FUN_1418407e0": ("Variable_SetArrayElem", "Estruturas de Dados", "void Variable_SetArrayElem(RValue* arr, int idx, RValue* val)"),
        "FUN_14183c650": ("RValue::AsString", "Core / RValue", "const char* RValue_AsString(RValue* val)"),
        "FUN_14183f530": ("Variable_GetGlobal", "Instâncias e Salas", "RValue* Variable_GetGlobal(int var_id)"),
        "FUN_14183c630": ("RValue::AsInt64", "Core / RValue", "long long RValue_AsInt64(RValue* val)"),
        "FUN_14183bfa0": ("CallScriptMethod", "Core / Runtime", "void CallScriptMethod(...)"),
        "FUN_140017bc0": ("InitArray", "Estruturas de Dados", "void InitArray(RValue* arr, int size)"),
        "FUN_1418391b0": ("MemoryManager::Push", "Core / Runtime", "void MemoryManager_Push(void* obj)"),
        "FUN_141839100": ("MemoryManager::Pop", "Core / Runtime", "void MemoryManager_Pop(int count)"),
        "FUN_14183b820": ("YYError", "Sistema", "void YYError(const char* fmt, ...)"),
        "FUN_14184d9f0": ("ArrayRef_Inc", "Estruturas de Dados", "void ArrayRef_Inc(RValue* arr)"),
        "FUN_1418409f0": ("Variable_Array_Get", "Estruturas de Dados", "RValue* Variable_Array_Get(RValue* arr, int idx)"),
        "FUN_140015020": ("RValue::AssignString", "Core / RValue", "void RValue_AssignString(RValue* dst, const char* str)"),
        "FUN_1418415d0": ("Variable_Struct_Get", "Estruturas de Dados", "RValue* Variable_Struct_Get(RValue* strct, int key)"),
        "FUN_14183bae0": ("CallFunctionByName", "Core / Runtime", "void CallFunctionByName(...)"),
        "FUN_141823920": ("RValue::AsInt32", "Core / RValue", "int RValue_AsInt32(RValue* val)"),
        "FUN_140014e80": ("RValue::SetReal", "Core / RValue", "void RValue_SetReal(RValue* dst, double r)"),
        "FUN_140042480": ("RValue::SetInt64", "Core / RValue", "void RValue_SetInt64(RValue* dst, long long i)"),
        "FUN_140015370": ("RValue::SetArray", "Core / RValue", "void RValue_SetArray(RValue* dst, void* arr)"),
        "FUN_14185efc0": ("YYFree", "Core / Runtime", "void YYFree(void* ptr)"),
        "FUN_14184da10": ("ArrayRef_Dec", "Estruturas de Dados", "void ArrayRef_Dec(void* arr)"),
        "FUN_14184d9e0": ("ArrayRef_Check", "Estruturas de Dados", "void ArrayRef_Check(void* arr)"),
        "FUN_140014fa0": ("RValue::SetBool", "Core / RValue", "void RValue_SetBool(RValue* dst, bool b)"),
        "FUN_14185ce50": ("String_Create", "Matemática e Strings", "void String_Create(RValue* dst, const char* s)"),
        "FUN_14183bfb0": ("CallScriptMethod_Args", "Core / Runtime", "void CallScriptMethod_Args(...)"),
        "FUN_14183bdc0": ("CallScriptMethod_Simple", "Core / Runtime", "void CallScriptMethod_Simple(...)"),
        "FUN_14183c660": ("RValue::AsPointer", "Core / RValue", "void* RValue_AsPointer(RValue* val)"),
        "FUN_141874c40": ("sin", "Matemática e Strings", "double sin(double a)"),
        "FUN_141840460": ("Variable_Global_Set", "Instâncias e Salas", "void Variable_Global_Set(int var_id, RValue* val)"),
        "FUN_141884a00": ("instance_exists", "Instâncias e Salas", "bool instance_exists(int id)"),
        "FUN_141822240": ("RValue::IsUndefined", "Core / RValue", "bool RValue_IsUndefined(RValue* val)"),
        "FUN_141884a80": ("instance_find", "Instâncias e Salas", "int instance_find(int obj, int n)"),
        "FUN_141874ff0": ("cos", "Matemática e Strings", "double cos(double a)"),
        "FUN_141874d50": ("sqrt", "Matemática e Strings", "double sqrt(double a)"),
        "FUN_141875130": ("point_direction / atan2", "Matemática e Strings", "double point_direction(double x1, double y1, double x2, double y2)"),
        "FUN_141823aa0": ("RValue::IsNull", "Core / RValue", "bool RValue_IsNull(RValue* val)"),
        "FUN_140a37090": ("RValue::AddRef", "Core / RValue", "void RValue_AddRef(RValue* dst, RValue* src)"),
        "FUN_14186cd80": ("draw_sprite_ext", "Gráficos", "void draw_sprite_ext(int spr, float subimg, double x, double y, ...)"),
        "FUN_141875350": ("random_range", "Matemática e Strings", "double random_range(double min, double max)")
    }

    builtin_counts = Counter()
    direct_counts = Counter()

    disp_call_regex = re.compile(r'FUN_14183b9b0\s*\([^,]+,[^,]+,[^,]+,[^,]+,\s*(DAT_1424[0-9a-fA-F]+)')
    fun_call_regex = re.compile(r'(FUN_14[0-9a-f]{7})')

    files = [os.path.join(GML_DIR, f) for f in os.listdir(GML_DIR) if f.endswith(".c")]
    print(f"Scanning {len(files)} GML files...")

    for fpath in files:
        with open(fpath, "r", encoding="utf-8", errors="ignore") as f:
            content = f.read()
        
        for m in disp_call_regex.findall(content):
            builtin_counts[m] += 1
            
        for fn in fun_call_regex.findall(content):
            if fn != "FUN_14183b9b0":
                direct_counts[fn] += 1

    print(f"Total distinct builtins called via dispatcher: {len(builtin_counts)}")
    print(f"Total distinct direct engine functions called: {len(direct_counts)}")

    inventory = {}

    for dat_name, count in builtin_counts.items():
        r_info = routines.get(dat_name)
        if r_info:
            b_name = r_info["name"]
        else:
            b_name = f"unknown_builtin_{dat_name}"
        
        reg_info = reg_map.get(b_name, {})
        cat = categorize(b_name, reg_info.get("category"))
        argc = reg_info.get("argc", "?")
        sig = f"{b_name}(... [{argc} args])"
        
        inventory[b_name] = {
            "name": b_name,
            "category": cat,
            "calls": count,
            "signature": sig,
            "status": "pendente",
            "type": "Builtin GM"
        }

    for fn, count in direct_counts.items():
        if fn in ptr_map:
            p_info = ptr_map[fn]
            b_name = p_info["name"]
            cat = categorize(b_name, p_info.get("category"))
            argc = p_info.get("argc", "?")
            sig = f"{b_name}(... [{argc} args])"
            
            if b_name in inventory:
                inventory[b_name]["calls"] += count
            else:
                inventory[b_name] = {
                    "name": b_name,
                    "category": cat,
                    "calls": count,
                    "signature": sig,
                    "status": "pendente",
                    "type": "Fastpath Nativo"
                }
        elif fn in core_helpers:
            c_name, cat, sig = core_helpers[fn]
            if c_name in inventory:
                inventory[c_name]["calls"] += count
            else:
                inventory[c_name] = {
                    "name": c_name,
                    "category": cat,
                    "calls": count,
                    "signature": sig,
                    "status": "pendente",
                    "type": "Core / Runtime"
                }
        else:
            inventory[fn] = {
                "name": fn,
                "category": "Runner Interno / Auxiliar",
                "calls": count,
                "signature": f"{fn}(...)",
                "status": "pendente",
                "type": "Interno"
            }

    print(f"Total unified inventory entries: {len(inventory)}")

    by_category = defaultdict(list)
    for item in inventory.values():
        by_category[item["category"]].append(item)

    for cat in by_category:
        by_category[cat].sort(key=lambda x: x["calls"], reverse=True)

    with open(OUTPUT_MD, "w", encoding="utf-8") as out:
        out.write("# Inventário da API do Runner - Vernal Edge (Fase 1)\n\n")
        out.write("Este documento lista todas as funções do GameMaker Runner e rotinas internas de que o código `gml/` descompilado depende para execução nativa em Android arm64.\n\n")
        out.write("## 1. Resumo Estatístico da API\n\n")
        out.write(f"- **Total de funções identificadas**: {len(inventory)}\n")
        total_calls = sum(x["calls"] for x in inventory.values())
        out.write(f"- **Total de chamadas a funções do runner**: {total_calls:,}\n\n")

        out.write("| Categoria | Qtd. Funções Únicas | Total de Chamadas | Prioridade no Runner |\n")
        out.write("| :--- | :---: | :---: | :---: |\n")
        for cat in sorted(by_category.keys(), key=lambda c: sum(x["calls"] for x in by_category[c]), reverse=True):
            items = by_category[cat]
            c_calls = sum(x["calls"] for x in items)
            prio = "Alta (Crítico)" if c_calls > 5000 else ("Média" if c_calls > 200 else "Baixa")
            anchor = cat.lower().replace(' ', '-').replace('/', '').replace('&', '')
            out.write(f"| [{cat}](#{anchor}) | {len(items)} | {c_calls:,} | {prio} |\n")
        out.write("\n---\n\n")

        for cat in sorted(by_category.keys(), key=lambda c: sum(x["calls"] for x in by_category[c]), reverse=True):
            items = by_category[cat]
            out.write(f"## {cat}\n\n")
            out.write(f"Total de funções: **{len(items)}** | Total de invocações: **{sum(x['calls'] for x in items):,}**\n\n")
            out.write("| Função | Assinatura / Papel | Tipo | Chamadas | Status |\n")
            out.write("| :--- | :--- | :---: | :---: | :---: |\n")
            for item in items:
                out.write(f"| `{item['name']}` | `{item['signature']}` | {item['type']} | {item['calls']} | {item['status']} |\n")
            out.write("\n")

    print(f"API inventory successfully updated in {OUTPUT_MD}!")

if __name__ == "__main__":
    main()
