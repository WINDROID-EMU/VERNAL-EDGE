# Inventário da API do Runner - Vernal Edge (Fase 1)

Este documento lista todas as funções do GameMaker Runner e rotinas internas de que o código `gml/` descompilado depende para execução nativa em Android arm64.

## 1. Resumo Estatístico da API

- **Total de funções identificadas**: 1789
- **Total de chamadas a funções do runner**: 308,137

| Categoria | Qtd. Funções Únicas | Total de Chamadas | Prioridade no Runner |
| :--- | :---: | :---: | :---: |
| [Core / RValue](#core--rvalue) | 20 | 238,191 | Alta (Crítico) |
| [Core / Runtime](#core--runtime) | 8 | 27,272 | Alta (Crítico) |
| [Estruturas de Dados](#estruturas-de-dados) | 41 | 13,835 | Alta (Crítico) |
| [Runner Interno / Auxiliar](#runner-interno--auxiliar) | 1379 | 12,215 | Alta (Crítico) |
| [Instâncias e Salas](#instâncias-e-salas) | 21 | 8,230 | Alta (Crítico) |
| [Geral](#geral) | 30 | 3,559 | Média |
| [Matemática e Strings](#matemática-e-strings) | 25 | 2,024 | Média |
| [Sistema](#sistema) | 6 | 1,604 | Média |
| [Gráficos](#gráficos) | 92 | 754 | Média |
| [YoYo/GML](#yoyogml) | 66 | 121 | Baixa |
| [Partículas](#partículas) | 23 | 107 | Baixa |
| [Arquivos e Buffers](#arquivos-e-buffers) | 26 | 74 | Baixa |
| [Áudio](#áudio) | 11 | 29 | Baixa |
| [Recursos](#recursos) | 9 | 26 | Baixa |
| [Input](#input) | 12 | 21 | Baixa |
| [Shaders](#shaders) | 7 | 20 | Baixa |
| [Matemática](#matemática) | 4 | 18 | Baixa |
| [Buffers](#buffers) | 5 | 18 | Baixa |
| [Interação](#interação) | 2 | 14 | Baixa |
| [Diversos](#diversos) | 1 | 4 | Baixa |
| [Arquivos](#arquivos) | 1 | 1 | Baixa |

---

## Core / RValue

Total de funções: **20** | Total de invocações: **238,191**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `FREE_RValue` | `void FREE_RValue(RValue* val)` | Core / Runtime | 108491 | pendente |
| `COPY_RValue` | `void COPY_RValue(RValue* dst, RValue* src)` | Core / Runtime | 58257 | pendente |
| `SET_RValue` | `void SET_RValue(RValue* dst, RValue* src)` | Core / Runtime | 38933 | pendente |
| `RValue::AsBool` | `bool RValue_AsBool(RValue* val)` | Core / Runtime | 5918 | pendente |
| `YYCompare` | `int YYCompare(RValue* a, RValue* b, int op)` | Core / Runtime | 5198 | pendente |
| `RValue::AsReal` | `double RValue_AsReal(RValue* val)` | Core / Runtime | 4893 | pendente |
| `SetupMethod` | `void SetupMethod(RValue* res, void* func, CInstance* inst)` | Core / Runtime | 3044 | pendente |
| `FREE_RValue_Array` | `void FREE_RValue_Array(RValue* arr, int count)` | Core / Runtime | 2525 | pendente |
| `RValue::AsString` | `const char* RValue_AsString(RValue* val)` | Core / Runtime | 2066 | pendente |
| `RValue::AsInt64` | `long long RValue_AsInt64(RValue* val)` | Core / Runtime | 1785 | pendente |
| `RValue::AssignString` | `void RValue_AssignString(RValue* dst, const char* str)` | Core / Runtime | 1479 | pendente |
| `RValue::AsInt32` | `int RValue_AsInt32(RValue* val)` | Core / Runtime | 1074 | pendente |
| `RValue::SetReal` | `void RValue_SetReal(RValue* dst, double r)` | Core / Runtime | 1020 | pendente |
| `RValue::SetInt64` | `void RValue_SetInt64(RValue* dst, long long i)` | Core / Runtime | 897 | pendente |
| `RValue::SetArray` | `void RValue_SetArray(RValue* dst, void* arr)` | Core / Runtime | 878 | pendente |
| `RValue::SetBool` | `void RValue_SetBool(RValue* dst, bool b)` | Core / Runtime | 678 | pendente |
| `RValue::AsPointer` | `void* RValue_AsPointer(RValue* val)` | Core / Runtime | 562 | pendente |
| `RValue::IsUndefined` | `bool RValue_IsUndefined(RValue* val)` | Core / Runtime | 289 | pendente |
| `RValue::IsNull` | `bool RValue_IsNull(RValue* val)` | Core / Runtime | 173 | pendente |
| `RValue::AddRef` | `void RValue_AddRef(RValue* dst, RValue* src)` | Core / Runtime | 31 | pendente |

## Core / Runtime

Total de funções: **8** | Total de invocações: **27,272**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `MemoryManager::Push` | `void MemoryManager_Push(void* obj)` | Core / Runtime | 9530 | pendente |
| `MemoryManager::Pop` | `void MemoryManager_Pop(int count)` | Core / Runtime | 8899 | pendente |
| `CheckStackPush` | `void CheckStackPush(int tag)` | Core / Runtime | 3904 | pendente |
| `CallScriptMethod` | `void CallScriptMethod(...)` | Core / Runtime | 1785 | pendente |
| `CallFunctionByName` | `void CallFunctionByName(...)` | Core / Runtime | 1182 | pendente |
| `YYFree` | `void YYFree(void* ptr)` | Core / Runtime | 830 | pendente |
| `CallScriptMethod_Args` | `void CallScriptMethod_Args(...)` | Core / Runtime | 571 | pendente |
| `CallScriptMethod_Simple` | `void CallScriptMethod_Simple(...)` | Core / Runtime | 571 | pendente |

## Estruturas de Dados

Total de funções: **41** | Total de invocações: **13,835**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `Variable_GetArrayElem` | `RValue* Variable_GetArrayElem(RValue* arr, int idx, ...)` | Core / Runtime | 3499 | pendente |
| `Variable_SetArrayElem` | `void Variable_SetArrayElem(RValue* arr, int idx, RValue* val)` | Core / Runtime | 2436 | pendente |
| `InitArray` | `void InitArray(RValue* arr, int size)` | Core / Runtime | 1615 | pendente |
| `ArrayRef_Inc` | `void ArrayRef_Inc(RValue* arr)` | Core / Runtime | 1590 | pendente |
| `Variable_Array_Get` | `RValue* Variable_Array_Get(RValue* arr, int idx)` | Core / Runtime | 1489 | pendente |
| `Variable_Struct_Get` | `RValue* Variable_Struct_Get(RValue* strct, int key)` | Core / Runtime | 1261 | pendente |
| `ArrayRef_Dec` | `void ArrayRef_Dec(void* arr)` | Core / Runtime | 795 | pendente |
| `ArrayRef_Check` | `void ArrayRef_Check(void* arr)` | Core / Runtime | 795 | pendente |
| `ds_map_set` | `ds_map_set(... [3 args])` | Builtin GM | 122 | pendente |
| `ds_map_find_value` | `ds_map_find_value(... [2 args])` | Builtin GM | 31 | pendente |
| `ds_list_destroy` | `ds_list_destroy(... [1 args])` | Builtin GM | 18 | pendente |
| `ds_list_find_value` | `ds_list_find_value(... [2 args])` | Builtin GM | 18 | pendente |
| `ds_map_create` | `ds_map_create(... [0 args])` | Builtin GM | 18 | pendente |
| `variable_struct_exists` | `variable_struct_exists(... [2 args])` | Builtin GM | 16 | pendente |
| `ds_list_create` | `ds_list_create(... [0 args])` | Builtin GM | 15 | pendente |
| `ds_list_add` | `ds_list_add(... [0xffffffff args])` | Builtin GM | 13 | pendente |
| `variable_instance_get` | `variable_instance_get(... [2 args])` | Builtin GM | 11 | pendente |
| `array_create` | `array_create(... [0xffffffff args])` | Builtin GM | 10 | pendente |
| `variable_instance_exists` | `variable_instance_exists(... [2 args])` | Builtin GM | 10 | pendente |
| `ds_list_size` | `ds_list_size(... [1 args])` | Builtin GM | 8 | pendente |
| `ds_map_destroy` | `ds_map_destroy(... [1 args])` | Builtin GM | 8 | pendente |
| `ds_priority_create` | `ds_priority_create(... [0 args])` | Builtin GM | 6 | pendente |
| `ds_priority_add` | `ds_priority_add(... [3 args])` | Builtin GM | 6 | pendente |
| `variable_global_exists` | `variable_global_exists(... [1 args])` | Builtin GM | 6 | pendente |
| `variable_instance_set` | `variable_instance_set(... [3 args])` | Builtin GM | 6 | pendente |
| `ds_map_exists` | `ds_map_exists(... [2 args])` | Builtin GM | 4 | pendente |
| `ds_priority_delete_min` | `ds_priority_delete_min(... [1 args])` | Builtin GM | 4 | pendente |
| `ds_priority_destroy` | `ds_priority_destroy(... [1 args])` | Builtin GM | 4 | pendente |
| `ds_list_delete` | `ds_list_delete(... [2 args])` | Builtin GM | 3 | pendente |
| `array_delete` | `array_delete(... [3 args])` | Builtin GM | 3 | pendente |
| `ds_priority_size` | `ds_priority_size(... [1 args])` | Builtin GM | 3 | pendente |
| `ds_list_clear` | `ds_list_clear(... [1 args])` | Builtin GM | 3 | pendente |
| `ds_priority_copy` | `ds_priority_copy(... [2 args])` | Builtin GM | 1 | pendente |
| `ds_priority_find_min` | `ds_priority_find_min(... [1 args])` | Builtin GM | 1 | pendente |
| `ds_priority_empty` | `ds_priority_empty(... [1 args])` | Builtin GM | 1 | pendente |
| `array_length_1d` | `array_length_1d(... [1 args])` | Builtin GM | 1 | pendente |
| `array_height_2d` | `array_height_2d(... [1 args])` | Builtin GM | 1 | pendente |
| `array_length_2d` | `array_length_2d(... [2 args])` | Builtin GM | 1 | pendente |
| `ds_map_size` | `ds_map_size(... [1 args])` | Builtin GM | 1 | pendente |
| `ds_map_values_to_array` | `ds_map_values_to_array(... [0xffffffff args])` | Builtin GM | 1 | pendente |
| `ds_map_clear` | `ds_map_clear(... [1 args])` | Builtin GM | 1 | pendente |

## Runner Interno / Auxiliar

Total de funções: **1379** | Total de invocações: **12,215**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `FUN_140034d50` | `FUN_140034d50(...)` | Interno | 650 | pendente |
| `FUN_140037fe0` | `FUN_140037fe0(...)` | Interno | 548 | pendente |
| `FUN_140035d00` | `FUN_140035d00(...)` | Interno | 364 | pendente |
| `FUN_1400380a0` | `FUN_1400380a0(...)` | Interno | 289 | pendente |
| `FUN_140034c50` | `FUN_140034c50(...)` | Interno | 265 | pendente |
| `FUN_141825900` | `FUN_141825900(...)` | Interno | 257 | pendente |
| `FUN_140034e50` | `FUN_140034e50(...)` | Interno | 239 | pendente |
| `FUN_14011f340` | `FUN_14011f340(...)` | Interno | 169 | pendente |
| `FUN_14186cd00` | `FUN_14186cd00(...)` | Interno | 159 | pendente |
| `FUN_14183be10` | `FUN_14183be10(...)` | Interno | 148 | pendente |
| `FUN_1418753a0` | `FUN_1418753a0(...)` | Interno | 144 | pendente |
| `FUN_141891100` | `FUN_141891100(...)` | Interno | 130 | pendente |
| `FUN_14187baf0` | `FUN_14187baf0(...)` | Interno | 127 | pendente |
| `FUN_141841690` | `FUN_141841690(...)` | Interno | 125 | pendente |
| `FUN_14185cee0` | `FUN_14185cee0(...)` | Interno | 117 | pendente |
| `FUN_141840e50` | `FUN_141840e50(...)` | Interno | 104 | pendente |
| `FUN_141875270` | `FUN_141875270(...)` | Interno | 100 | pendente |
| `FUN_14185d2a0` | `FUN_14185d2a0(...)` | Interno | 98 | pendente |
| `FUN_141875320` | `FUN_141875320(...)` | Interno | 86 | pendente |
| `FUN_141899f40` | `FUN_141899f40(...)` | Interno | 83 | pendente |
| `FUN_141132d30` | `FUN_141132d30(...)` | Interno | 78 | pendente |
| `FUN_140796090` | `FUN_140796090(...)` | Interno | 77 | pendente |
| `FUN_140a54a70` | `FUN_140a54a70(...)` | Interno | 76 | pendente |
| `FUN_14151e260` | `FUN_14151e260(...)` | Interno | 76 | pendente |
| `FUN_140a97060` | `FUN_140a97060(...)` | Interno | 74 | pendente |
| `FUN_140f0f6c0` | `FUN_140f0f6c0(...)` | Interno | 71 | pendente |
| `FUN_1417c53d0` | `FUN_1417c53d0(...)` | Interno | 69 | pendente |
| `FUN_14186cc20` | `FUN_14186cc20(...)` | Interno | 68 | pendente |
| `FUN_14189c320` | `FUN_14189c320(...)` | Interno | 67 | pendente |
| `FUN_14189c2d0` | `FUN_14189c2d0(...)` | Interno | 58 | pendente |
| `FUN_1418884f0` | `FUN_1418884f0(...)` | Interno | 52 | pendente |
| `FUN_140689ec0` | `FUN_140689ec0(...)` | Interno | 50 | pendente |
| `FUN_1414edc20` | `FUN_1414edc20(...)` | Interno | 45 | pendente |
| `FUN_1405201d0` | `FUN_1405201d0(...)` | Interno | 45 | pendente |
| `FUN_14022cd60` | `FUN_14022cd60(...)` | Interno | 45 | pendente |
| `FUN_1407e5460` | `FUN_1407e5460(...)` | Interno | 44 | pendente |
| `FUN_14080ff70` | `FUN_14080ff70(...)` | Interno | 43 | pendente |
| `FUN_14170ef60` | `FUN_14170ef60(...)` | Interno | 43 | pendente |
| `FUN_1409b8f60` | `FUN_1409b8f60(...)` | Interno | 41 | pendente |
| `FUN_1403abb20` | `FUN_1403abb20(...)` | Interno | 40 | pendente |
| `FUN_141874cd0` | `FUN_141874cd0(...)` | Interno | 39 | pendente |
| `FUN_1401aa620` | `FUN_1401aa620(...)` | Interno | 39 | pendente |
| `FUN_14189f2e0` | `FUN_14189f2e0(...)` | Interno | 38 | pendente |
| `FUN_14189ef30` | `FUN_14189ef30(...)` | Interno | 38 | pendente |
| `FUN_141874c50` | `FUN_141874c50(...)` | Interno | 38 | pendente |
| `FUN_1400b45f0` | `FUN_1400b45f0(...)` | Interno | 36 | pendente |
| `FUN_141772530` | `FUN_141772530(...)` | Interno | 35 | pendente |
| `FUN_141891020` | `FUN_141891020(...)` | Interno | 35 | pendente |
| `FUN_14189f3b0` | `FUN_14189f3b0(...)` | Interno | 34 | pendente |
| `FUN_1412d6ec0` | `FUN_1412d6ec0(...)` | Interno | 34 | pendente |
| `FUN_141319a00` | `FUN_141319a00(...)` | Interno | 34 | pendente |
| `FUN_14188b3e0` | `FUN_14188b3e0(...)` | Interno | 33 | pendente |
| `FUN_141891070` | `FUN_141891070(...)` | Interno | 33 | pendente |
| `FUN_141392d20` | `FUN_141392d20(...)` | Interno | 33 | pendente |
| `FUN_1400f77b0` | `FUN_1400f77b0(...)` | Interno | 32 | pendente |
| `FUN_14116cfa0` | `FUN_14116cfa0(...)` | Interno | 32 | pendente |
| `FUN_1401b7410` | `FUN_1401b7410(...)` | Interno | 32 | pendente |
| `FUN_141840ac0` | `FUN_141840ac0(...)` | Interno | 31 | pendente |
| `FUN_141236230` | `FUN_141236230(...)` | Interno | 31 | pendente |
| `FUN_140f870f0` | `FUN_140f870f0(...)` | Interno | 31 | pendente |
| `FUN_140361a20` | `FUN_140361a20(...)` | Interno | 31 | pendente |
| `FUN_141698ac0` | `FUN_141698ac0(...)` | Interno | 31 | pendente |
| `FUN_1404b2e10` | `FUN_1404b2e10(...)` | Interno | 30 | pendente |
| `FUN_14031aba0` | `FUN_14031aba0(...)` | Interno | 30 | pendente |
| `FUN_1415d9fc0` | `FUN_1415d9fc0(...)` | Interno | 30 | pendente |
| `FUN_141208630` | `FUN_141208630(...)` | Interno | 30 | pendente |
| `FUN_141840530` | `FUN_141840530(...)` | Interno | 29 | pendente |
| `FUN_140118a10` | `FUN_140118a10(...)` | Interno | 29 | pendente |
| `FUN_1410e55b0` | `FUN_1410e55b0(...)` | Interno | 29 | pendente |
| `FUN_140310b70` | `FUN_140310b70(...)` | Interno | 28 | pendente |
| `FUN_1403e5670` | `FUN_1403e5670(...)` | Interno | 28 | pendente |
| `FUN_141781550` | `FUN_141781550(...)` | Interno | 28 | pendente |
| `FUN_141041de0` | `FUN_141041de0(...)` | Interno | 28 | pendente |
| `FUN_1401918f0` | `FUN_1401918f0(...)` | Interno | 28 | pendente |
| `FUN_1401fd740` | `FUN_1401fd740(...)` | Interno | 27 | pendente |
| `FUN_141989170` | `FUN_141989170(...)` | Interno | 27 | pendente |
| `FUN_14168da20` | `FUN_14168da20(...)` | Interno | 27 | pendente |
| `FUN_14162b910` | `FUN_14162b910(...)` | Interno | 27 | pendente |
| `FUN_141b1ec00` | `FUN_141b1ec00(...)` | Interno | 26 | pendente |
| `FUN_14189c2a0` | `FUN_14189c2a0(...)` | Interno | 26 | pendente |
| `FUN_140968fa0` | `FUN_140968fa0(...)` | Interno | 26 | pendente |
| `FUN_140438db0` | `FUN_140438db0(...)` | Interno | 26 | pendente |
| `FUN_1410352c0` | `FUN_1410352c0(...)` | Interno | 25 | pendente |
| `FUN_140541370` | `FUN_140541370(...)` | Interno | 25 | pendente |
| `FUN_1401be850` | `FUN_1401be850(...)` | Interno | 25 | pendente |
| `FUN_140834f70` | `FUN_140834f70(...)` | Interno | 25 | pendente |
| `FUN_140fc3f70` | `FUN_140fc3f70(...)` | Interno | 24 | pendente |
| `FUN_1407ef5d0` | `FUN_1407ef5d0(...)` | Interno | 24 | pendente |
| `FUN_1402ac860` | `FUN_1402ac860(...)` | Interno | 24 | pendente |
| `FUN_140491400` | `FUN_140491400(...)` | Interno | 24 | pendente |
| `FUN_14038ced0` | `FUN_14038ced0(...)` | Interno | 23 | pendente |
| `FUN_1412751a0` | `FUN_1412751a0(...)` | Interno | 23 | pendente |
| `FUN_140880ae0` | `FUN_140880ae0(...)` | Interno | 23 | pendente |
| `FUN_141478d50` | `FUN_141478d50(...)` | Interno | 23 | pendente |
| `FUN_140452d40` | `FUN_140452d40(...)` | Interno | 22 | pendente |
| `FUN_14127ab50` | `FUN_14127ab50(...)` | Interno | 22 | pendente |
| `FUN_141411760` | `FUN_141411760(...)` | Interno | 22 | pendente |
| `FUN_1408b49e0` | `FUN_1408b49e0(...)` | Interno | 21 | pendente |
| `FUN_140ac0e20` | `FUN_140ac0e20(...)` | Interno | 21 | pendente |
| `FUN_1402304a0` | `FUN_1402304a0(...)` | Interno | 21 | pendente |
| `FUN_140f04910` | `FUN_140f04910(...)` | Interno | 21 | pendente |
| `FUN_14064bef0` | `FUN_14064bef0(...)` | Interno | 21 | pendente |
| `FUN_1409837f0` | `FUN_1409837f0(...)` | Interno | 21 | pendente |
| `FUN_14159b810` | `FUN_14159b810(...)` | Interno | 21 | pendente |
| `FUN_14149bd10` | `FUN_14149bd10(...)` | Interno | 21 | pendente |
| `FUN_1410b30f0` | `FUN_1410b30f0(...)` | Interno | 21 | pendente |
| `FUN_1404842f0` | `FUN_1404842f0(...)` | Interno | 21 | pendente |
| `FUN_1401c3790` | `FUN_1401c3790(...)` | Interno | 20 | pendente |
| `FUN_140265fe0` | `FUN_140265fe0(...)` | Interno | 20 | pendente |
| `FUN_1406c3580` | `FUN_1406c3580(...)` | Interno | 20 | pendente |
| `FUN_140055810` | `FUN_140055810(...)` | Interno | 20 | pendente |
| `FUN_14186cf60` | `FUN_14186cf60(...)` | Interno | 19 | pendente |
| `FUN_1418894b0` | `FUN_1418894b0(...)` | Interno | 19 | pendente |
| `FUN_140444aa0` | `FUN_140444aa0(...)` | Interno | 19 | pendente |
| `FUN_140aab6e0` | `FUN_140aab6e0(...)` | Interno | 19 | pendente |
| `FUN_1409d2480` | `FUN_1409d2480(...)` | Interno | 19 | pendente |
| `FUN_14097c740` | `FUN_14097c740(...)` | Interno | 19 | pendente |
| `FUN_1410d1260` | `FUN_1410d1260(...)` | Interno | 19 | pendente |
| `FUN_140fa8fb0` | `FUN_140fa8fb0(...)` | Interno | 19 | pendente |
| `FUN_1401070a0` | `FUN_1401070a0(...)` | Interno | 19 | pendente |
| `FUN_141088460` | `FUN_141088460(...)` | Interno | 19 | pendente |
| `FUN_1412438b0` | `FUN_1412438b0(...)` | Interno | 19 | pendente |
| `FUN_1403987f0` | `FUN_1403987f0(...)` | Interno | 19 | pendente |
| `FUN_14167e590` | `FUN_14167e590(...)` | Interno | 18 | pendente |
| `FUN_1407d0c80` | `FUN_1407d0c80(...)` | Interno | 18 | pendente |
| `FUN_1404da440` | `FUN_1404da440(...)` | Interno | 18 | pendente |
| `FUN_14010b810` | `FUN_14010b810(...)` | Interno | 18 | pendente |
| `FUN_1414db640` | `FUN_1414db640(...)` | Interno | 18 | pendente |
| `FUN_1413e5e70` | `FUN_1413e5e70(...)` | Interno | 18 | pendente |
| `FUN_141701760` | `FUN_141701760(...)` | Interno | 18 | pendente |
| `FUN_140299690` | `FUN_140299690(...)` | Interno | 17 | pendente |
| `FUN_1404e36e0` | `FUN_1404e36e0(...)` | Interno | 17 | pendente |
| `FUN_1406732f0` | `FUN_1406732f0(...)` | Interno | 17 | pendente |
| `FUN_14142dfb0` | `FUN_14142dfb0(...)` | Interno | 17 | pendente |
| `FUN_14179f1e0` | `FUN_14179f1e0(...)` | Interno | 17 | pendente |
| `FUN_1413fa870` | `FUN_1413fa870(...)` | Interno | 17 | pendente |
| `FUN_14110d6f0` | `FUN_14110d6f0(...)` | Interno | 17 | pendente |
| `FUN_1416ae3b0` | `FUN_1416ae3b0(...)` | Interno | 17 | pendente |
| `FUN_140014870` | `FUN_140014870(...)` | Interno | 16 | pendente |
| `FUN_14123a3b0` | `FUN_14123a3b0(...)` | Interno | 16 | pendente |
| `FUN_14181d510` | `FUN_14181d510(...)` | Interno | 16 | pendente |
| `FUN_140ac41a0` | `FUN_140ac41a0(...)` | Interno | 16 | pendente |
| `FUN_14061a940` | `FUN_14061a940(...)` | Interno | 16 | pendente |
| `FUN_1401c36c0` | `FUN_1401c36c0(...)` | Interno | 16 | pendente |
| `FUN_1402bff50` | `FUN_1402bff50(...)` | Interno | 16 | pendente |
| `FUN_140777130` | `FUN_140777130(...)` | Interno | 16 | pendente |
| `FUN_1412822d0` | `FUN_1412822d0(...)` | Interno | 15 | pendente |
| `FUN_140215b00` | `FUN_140215b00(...)` | Interno | 15 | pendente |
| `FUN_1400d1ef0` | `FUN_1400d1ef0(...)` | Interno | 15 | pendente |
| `FUN_1411a0c70` | `FUN_1411a0c70(...)` | Interno | 15 | pendente |
| `FUN_140e87790` | `FUN_140e87790(...)` | Interno | 15 | pendente |
| `FUN_1403cfeb0` | `FUN_1403cfeb0(...)` | Interno | 15 | pendente |
| `FUN_140042370` | `FUN_140042370(...)` | Interno | 14 | pendente |
| `FUN_140aafc00` | `FUN_140aafc00(...)` | Interno | 14 | pendente |
| `FUN_1412a5940` | `FUN_1412a5940(...)` | Interno | 14 | pendente |
| `FUN_1402de730` | `FUN_1402de730(...)` | Interno | 14 | pendente |
| `FUN_140332280` | `FUN_140332280(...)` | Interno | 14 | pendente |
| `FUN_1416dca80` | `FUN_1416dca80(...)` | Interno | 14 | pendente |
| `FUN_141255b80` | `FUN_141255b80(...)` | Interno | 14 | pendente |
| `FUN_14107cd20` | `FUN_14107cd20(...)` | Interno | 14 | pendente |
| `FUN_141501010` | `FUN_141501010(...)` | Interno | 14 | pendente |
| `FUN_140fdbae0` | `FUN_140fdbae0(...)` | Interno | 14 | pendente |
| `FUN_141113930` | `FUN_141113930(...)` | Interno | 14 | pendente |
| `FUN_1414f8e10` | `FUN_1414f8e10(...)` | Interno | 14 | pendente |
| `FUN_14150acf0` | `FUN_14150acf0(...)` | Interno | 14 | pendente |
| `FUN_1417e33d0` | `FUN_1417e33d0(...)` | Interno | 14 | pendente |
| `FUN_14032d3d0` | `FUN_14032d3d0(...)` | Interno | 13 | pendente |
| `FUN_14086d040` | `FUN_14086d040(...)` | Interno | 13 | pendente |
| `FUN_14189f150` | `FUN_14189f150(...)` | Interno | 13 | pendente |
| `FUN_14185ced0` | `FUN_14185ced0(...)` | Interno | 13 | pendente |
| `FUN_1416b1d90` | `FUN_1416b1d90(...)` | Interno | 13 | pendente |
| `FUN_141649720` | `FUN_141649720(...)` | Interno | 13 | pendente |
| `FUN_140510dd0` | `FUN_140510dd0(...)` | Interno | 13 | pendente |
| `FUN_14039d490` | `FUN_14039d490(...)` | Interno | 13 | pendente |
| `FUN_140698a00` | `FUN_140698a00(...)` | Interno | 13 | pendente |
| `FUN_140361970` | `FUN_140361970(...)` | Interno | 13 | pendente |
| `FUN_140efe000` | `FUN_140efe000(...)` | Interno | 13 | pendente |
| `FUN_140815280` | `FUN_140815280(...)` | Interno | 12 | pendente |
| `FUN_14026b670` | `FUN_14026b670(...)` | Interno | 12 | pendente |
| `FUN_1406699d0` | `FUN_1406699d0(...)` | Interno | 12 | pendente |
| `FUN_140801da0` | `FUN_140801da0(...)` | Interno | 12 | pendente |
| `FUN_140f42160` | `FUN_140f42160(...)` | Interno | 12 | pendente |
| `FUN_140a67cc0` | `FUN_140a67cc0(...)` | Interno | 12 | pendente |
| `FUN_141145f90` | `FUN_141145f90(...)` | Interno | 12 | pendente |
| `FUN_140265980` | `FUN_140265980(...)` | Interno | 12 | pendente |
| `FUN_14063cdd0` | `FUN_14063cdd0(...)` | Interno | 12 | pendente |
| `FUN_1407e9df0` | `FUN_1407e9df0(...)` | Interno | 12 | pendente |
| `FUN_140f47140` | `FUN_140f47140(...)` | Interno | 12 | pendente |
| `FUN_1415a8140` | `FUN_1415a8140(...)` | Interno | 12 | pendente |
| `FUN_141840430` | `FUN_141840430(...)` | Interno | 12 | pendente |
| `FUN_14042cc40` | `FUN_14042cc40(...)` | Interno | 12 | pendente |
| `FUN_141222300` | `FUN_141222300(...)` | Interno | 12 | pendente |
| `FUN_14171bb40` | `FUN_14171bb40(...)` | Interno | 12 | pendente |
| `FUN_141289630` | `FUN_141289630(...)` | Interno | 12 | pendente |
| `FUN_14157fdb0` | `FUN_14157fdb0(...)` | Interno | 12 | pendente |
| `FUN_1406375d0` | `FUN_1406375d0(...)` | Interno | 12 | pendente |
| `FUN_1413326d0` | `FUN_1413326d0(...)` | Interno | 12 | pendente |
| `FUN_140fb0400` | `FUN_140fb0400(...)` | Interno | 12 | pendente |
| `FUN_14125dd00` | `FUN_14125dd00(...)` | Interno | 12 | pendente |
| `FUN_1401c6060` | `FUN_1401c6060(...)` | Interno | 12 | pendente |
| `FUN_1406b7920` | `FUN_1406b7920(...)` | Interno | 12 | pendente |
| `FUN_1403f0640` | `FUN_1403f0640(...)` | Interno | 12 | pendente |
| `FUN_1414144f0` | `FUN_1414144f0(...)` | Interno | 12 | pendente |
| `FUN_1414bfb10` | `FUN_1414bfb10(...)` | Interno | 12 | pendente |
| `FUN_141456cb0` | `FUN_141456cb0(...)` | Interno | 12 | pendente |
| `FUN_14186cc10` | `FUN_14186cc10(...)` | Interno | 11 | pendente |
| `FUN_14186cc00` | `FUN_14186cc00(...)` | Interno | 11 | pendente |
| `FUN_14186cbf0` | `FUN_14186cbf0(...)` | Interno | 11 | pendente |
| `FUN_141796e70` | `FUN_141796e70(...)` | Interno | 11 | pendente |
| `FUN_140034b00` | `FUN_140034b00(...)` | Interno | 11 | pendente |
| `FUN_1415b0ff0` | `FUN_1415b0ff0(...)` | Interno | 11 | pendente |
| `FUN_141624310` | `FUN_141624310(...)` | Interno | 11 | pendente |
| `FUN_1401c92c0` | `FUN_1401c92c0(...)` | Interno | 11 | pendente |
| `FUN_14005c320` | `FUN_14005c320(...)` | Interno | 11 | pendente |
| `FUN_14088f9e0` | `FUN_14088f9e0(...)` | Interno | 11 | pendente |
| `FUN_14175d500` | `FUN_14175d500(...)` | Interno | 11 | pendente |
| `FUN_140f17270` | `FUN_140f17270(...)` | Interno | 11 | pendente |
| `FUN_1415c0ae0` | `FUN_1415c0ae0(...)` | Interno | 11 | pendente |
| `FUN_1411216e0` | `FUN_1411216e0(...)` | Interno | 11 | pendente |
| `FUN_140fd3d60` | `FUN_140fd3d60(...)` | Interno | 11 | pendente |
| `FUN_140f74a60` | `FUN_140f74a60(...)` | Interno | 11 | pendente |
| `FUN_141154a20` | `FUN_141154a20(...)` | Interno | 11 | pendente |
| `FUN_14067a8f0` | `FUN_14067a8f0(...)` | Interno | 11 | pendente |
| `FUN_14081a5d0` | `FUN_14081a5d0(...)` | Interno | 11 | pendente |
| `FUN_1413eca10` | `FUN_1413eca10(...)` | Interno | 11 | pendente |
| `FUN_141989130` | `FUN_141989130(...)` | Interno | 10 | pendente |
| `FUN_14063e2f0` | `FUN_14063e2f0(...)` | Interno | 10 | pendente |
| `FUN_14027fbe0` | `FUN_14027fbe0(...)` | Interno | 10 | pendente |
| `FUN_14052c3e0` | `FUN_14052c3e0(...)` | Interno | 10 | pendente |
| `FUN_140aa07a0` | `FUN_140aa07a0(...)` | Interno | 10 | pendente |
| `FUN_1402116d0` | `FUN_1402116d0(...)` | Interno | 10 | pendente |
| `FUN_140ed5850` | `FUN_140ed5850(...)` | Interno | 10 | pendente |
| `FUN_140621760` | `FUN_140621760(...)` | Interno | 10 | pendente |
| `FUN_1416cff80` | `FUN_1416cff80(...)` | Interno | 10 | pendente |
| `FUN_14045a640` | `FUN_14045a640(...)` | Interno | 10 | pendente |
| `FUN_141445d40` | `FUN_141445d40(...)` | Interno | 10 | pendente |
| `FUN_14166f760` | `FUN_14166f760(...)` | Interno | 10 | pendente |
| `FUN_1415ca630` | `FUN_1415ca630(...)` | Interno | 10 | pendente |
| `FUN_14096fd20` | `FUN_14096fd20(...)` | Interno | 10 | pendente |
| `FUN_140246e60` | `FUN_140246e60(...)` | Interno | 10 | pendente |
| `FUN_1407a7100` | `FUN_1407a7100(...)` | Interno | 10 | pendente |
| `FUN_14161d490` | `FUN_14161d490(...)` | Interno | 10 | pendente |
| `FUN_140e708e0` | `FUN_140e708e0(...)` | Interno | 10 | pendente |
| `FUN_1408e70a0` | `FUN_1408e70a0(...)` | Interno | 10 | pendente |
| `FUN_141052940` | `FUN_141052940(...)` | Interno | 10 | pendente |
| `FUN_1414944c0` | `FUN_1414944c0(...)` | Interno | 10 | pendente |
| `FUN_140eb7f90` | `FUN_140eb7f90(...)` | Interno | 10 | pendente |
| `FUN_140ee32f0` | `FUN_140ee32f0(...)` | Interno | 10 | pendente |
| `FUN_14079d2a0` | `FUN_14079d2a0(...)` | Interno | 10 | pendente |
| `FUN_14100fa70` | `FUN_14100fa70(...)` | Interno | 10 | pendente |
| `FUN_141160c80` | `FUN_141160c80(...)` | Interno | 10 | pendente |
| `FUN_140f49ef0` | `FUN_140f49ef0(...)` | Interno | 10 | pendente |
| `FUN_14028a880` | `FUN_14028a880(...)` | Interno | 10 | pendente |
| `FUN_140a3e270` | `FUN_140a3e270(...)` | Interno | 10 | pendente |
| `FUN_14122e720` | `FUN_14122e720(...)` | Interno | 10 | pendente |
| `FUN_1410c1b10` | `FUN_1410c1b10(...)` | Interno | 10 | pendente |
| `FUN_1417411d0` | `FUN_1417411d0(...)` | Interno | 10 | pendente |
| `FUN_14185d0a0` | `FUN_14185d0a0(...)` | Interno | 9 | pendente |
| `FUN_141424a10` | `FUN_141424a10(...)` | Interno | 9 | pendente |
| `FUN_1403eaef0` | `FUN_1403eaef0(...)` | Interno | 9 | pendente |
| `FUN_14174e2e0` | `FUN_14174e2e0(...)` | Interno | 9 | pendente |
| `FUN_141174020` | `FUN_141174020(...)` | Interno | 9 | pendente |
| `FUN_140502eb0` | `FUN_140502eb0(...)` | Interno | 9 | pendente |
| `FUN_1406c6480` | `FUN_1406c6480(...)` | Interno | 9 | pendente |
| `FUN_1408de1d0` | `FUN_1408de1d0(...)` | Interno | 9 | pendente |
| `FUN_1415e8a50` | `FUN_1415e8a50(...)` | Interno | 9 | pendente |
| `FUN_140231f80` | `FUN_140231f80(...)` | Interno | 9 | pendente |
| `FUN_140ee55d0` | `FUN_140ee55d0(...)` | Interno | 9 | pendente |
| `FUN_140063000` | `FUN_140063000(...)` | Interno | 9 | pendente |
| `FUN_14108b140` | `FUN_14108b140(...)` | Interno | 9 | pendente |
| `FUN_14065cb20` | `FUN_14065cb20(...)` | Interno | 9 | pendente |
| `FUN_14171ebb0` | `FUN_14171ebb0(...)` | Interno | 9 | pendente |
| `FUN_140f575a0` | `FUN_140f575a0(...)` | Interno | 9 | pendente |
| `FUN_1415a42c0` | `FUN_1415a42c0(...)` | Interno | 9 | pendente |
| `FUN_141596040` | `FUN_141596040(...)` | Interno | 9 | pendente |
| `FUN_140898680` | `FUN_140898680(...)` | Interno | 9 | pendente |
| `FUN_141101b50` | `FUN_141101b50(...)` | Interno | 9 | pendente |
| `FUN_1414c77f0` | `FUN_1414c77f0(...)` | Interno | 9 | pendente |
| `FUN_1412e9fb0` | `FUN_1412e9fb0(...)` | Interno | 9 | pendente |
| `FUN_14181cb70` | `FUN_14181cb70(...)` | Interno | 9 | pendente |
| `FUN_140eab9a0` | `FUN_140eab9a0(...)` | Interno | 9 | pendente |
| `FUN_1416e2790` | `FUN_1416e2790(...)` | Interno | 9 | pendente |
| `FUN_1410dcc60` | `FUN_1410dcc60(...)` | Interno | 9 | pendente |
| `FUN_140960210` | `FUN_140960210(...)` | Interno | 9 | pendente |
| `FUN_1416b3120` | `FUN_1416b3120(...)` | Interno | 8 | pendente |
| `FUN_14111b8e0` | `FUN_14111b8e0(...)` | Interno | 8 | pendente |
| `FUN_140f9c790` | `FUN_140f9c790(...)` | Interno | 8 | pendente |
| `FUN_1402f5ec0` | `FUN_1402f5ec0(...)` | Interno | 8 | pendente |
| `FUN_1412ca230` | `FUN_1412ca230(...)` | Interno | 8 | pendente |
| `FUN_1417cbf70` | `FUN_1417cbf70(...)` | Interno | 8 | pendente |
| `FUN_1414bd140` | `FUN_1414bd140(...)` | Interno | 8 | pendente |
| `FUN_140374120` | `FUN_140374120(...)` | Interno | 8 | pendente |
| `FUN_140826c80` | `FUN_140826c80(...)` | Interno | 8 | pendente |
| `FUN_141490460` | `FUN_141490460(...)` | Interno | 8 | pendente |
| `FUN_141627e30` | `FUN_141627e30(...)` | Interno | 8 | pendente |
| `FUN_14111e1f0` | `FUN_14111e1f0(...)` | Interno | 8 | pendente |
| `FUN_14044c150` | `FUN_14044c150(...)` | Interno | 8 | pendente |
| `FUN_14025aec0` | `FUN_14025aec0(...)` | Interno | 8 | pendente |
| `FUN_1408ad740` | `FUN_1408ad740(...)` | Interno | 8 | pendente |
| `FUN_1414c5460` | `FUN_1414c5460(...)` | Interno | 8 | pendente |
| `FUN_1414f5340` | `FUN_1414f5340(...)` | Interno | 8 | pendente |
| `FUN_140ebd0b0` | `FUN_140ebd0b0(...)` | Interno | 8 | pendente |
| `FUN_14131f020` | `FUN_14131f020(...)` | Interno | 8 | pendente |
| `FUN_141663580` | `FUN_141663580(...)` | Interno | 8 | pendente |
| `FUN_141669e80` | `FUN_141669e80(...)` | Interno | 8 | pendente |
| `FUN_140830680` | `FUN_140830680(...)` | Interno | 8 | pendente |
| `FUN_1417799e0` | `FUN_1417799e0(...)` | Interno | 8 | pendente |
| `FUN_1412f0080` | `FUN_1412f0080(...)` | Interno | 8 | pendente |
| `FUN_14160fd00` | `FUN_14160fd00(...)` | Interno | 8 | pendente |
| `FUN_14122a350` | `FUN_14122a350(...)` | Interno | 8 | pendente |
| `FUN_1416c05b0` | `FUN_1416c05b0(...)` | Interno | 8 | pendente |
| `FUN_141612260` | `FUN_141612260(...)` | Interno | 8 | pendente |
| `FUN_14113f0a0` | `FUN_14113f0a0(...)` | Interno | 8 | pendente |
| `FUN_14172d540` | `FUN_14172d540(...)` | Interno | 8 | pendente |
| `FUN_1416b45c0` | `FUN_1416b45c0(...)` | Interno | 8 | pendente |
| `FUN_141761120` | `FUN_141761120(...)` | Interno | 8 | pendente |
| `FUN_140ec65e0` | `FUN_140ec65e0(...)` | Interno | 8 | pendente |
| `FUN_140674cf0` | `FUN_140674cf0(...)` | Interno | 8 | pendente |
| `FUN_1411ce0e0` | `FUN_1411ce0e0(...)` | Interno | 8 | pendente |
| `FUN_1402d8a30` | `FUN_1402d8a30(...)` | Interno | 8 | pendente |
| `FUN_140840540` | `FUN_140840540(...)` | Interno | 8 | pendente |
| `FUN_14174b670` | `FUN_14174b670(...)` | Interno | 8 | pendente |
| `FUN_1415f9240` | `FUN_1415f9240(...)` | Interno | 8 | pendente |
| `FUN_140359d70` | `FUN_140359d70(...)` | Interno | 8 | pendente |
| `FUN_1410074e0` | `FUN_1410074e0(...)` | Interno | 7 | pendente |
| `FUN_1415e6820` | `FUN_1415e6820(...)` | Interno | 7 | pendente |
| `FUN_1410689b0` | `FUN_1410689b0(...)` | Interno | 7 | pendente |
| `FUN_1401c9240` | `FUN_1401c9240(...)` | Interno | 7 | pendente |
| `FUN_1417c87c0` | `FUN_1417c87c0(...)` | Interno | 7 | pendente |
| `FUN_141735e20` | `FUN_141735e20(...)` | Interno | 7 | pendente |
| `FUN_1414d84d0` | `FUN_1414d84d0(...)` | Interno | 7 | pendente |
| `FUN_1413f3c60` | `FUN_1413f3c60(...)` | Interno | 7 | pendente |
| `FUN_14012e770` | `FUN_14012e770(...)` | Interno | 7 | pendente |
| `FUN_14186cf50` | `FUN_14186cf50(...)` | Interno | 7 | pendente |
| `FUN_1411fe1d0` | `FUN_1411fe1d0(...)` | Interno | 7 | pendente |
| `FUN_1400f7730` | `FUN_1400f7730(...)` | Interno | 7 | pendente |
| `FUN_140377c80` | `FUN_140377c80(...)` | Interno | 7 | pendente |
| `FUN_14090f330` | `FUN_14090f330(...)` | Interno | 7 | pendente |
| `FUN_14189f220` | `FUN_14189f220(...)` | Interno | 7 | pendente |
| `FUN_141625250` | `FUN_141625250(...)` | Interno | 7 | pendente |
| `FUN_14153a310` | `FUN_14153a310(...)` | Interno | 7 | pendente |
| `FUN_14105f450` | `FUN_14105f450(...)` | Interno | 7 | pendente |
| `FUN_140125960` | `FUN_140125960(...)` | Interno | 7 | pendente |
| `FUN_140051840` | `FUN_140051840(...)` | Interno | 7 | pendente |
| `FUN_1411a2b40` | `FUN_1411a2b40(...)` | Interno | 7 | pendente |
| `FUN_14093da20` | `FUN_14093da20(...)` | Interno | 7 | pendente |
| `FUN_14079fc40` | `FUN_14079fc40(...)` | Interno | 7 | pendente |
| `FUN_1416ef620` | `FUN_1416ef620(...)` | Interno | 7 | pendente |
| `FUN_1410d3d10` | `FUN_1410d3d10(...)` | Interno | 7 | pendente |
| `FUN_14131c6d0` | `FUN_14131c6d0(...)` | Interno | 7 | pendente |
| `FUN_1407e85c0` | `FUN_1407e85c0(...)` | Interno | 7 | pendente |
| `FUN_14120fec0` | `FUN_14120fec0(...)` | Interno | 7 | pendente |
| `FUN_14173b5a0` | `FUN_14173b5a0(...)` | Interno | 7 | pendente |
| `FUN_141245460` | `FUN_141245460(...)` | Interno | 7 | pendente |
| `FUN_1416f3e90` | `FUN_1416f3e90(...)` | Interno | 7 | pendente |
| `FUN_1416a75e0` | `FUN_1416a75e0(...)` | Interno | 7 | pendente |
| `FUN_1416bb1f0` | `FUN_1416bb1f0(...)` | Interno | 7 | pendente |
| `FUN_141721470` | `FUN_141721470(...)` | Interno | 7 | pendente |
| `FUN_140e8e6e0` | `FUN_140e8e6e0(...)` | Interno | 7 | pendente |
| `FUN_140119e60` | `FUN_140119e60(...)` | Interno | 7 | pendente |
| `FUN_140496620` | `FUN_140496620(...)` | Interno | 7 | pendente |
| `FUN_1401121a0` | `FUN_1401121a0(...)` | Interno | 7 | pendente |
| `FUN_14189eeb0` | `FUN_14189eeb0(...)` | Interno | 6 | pendente |
| `FUN_14189f010` | `FUN_14189f010(...)` | Interno | 6 | pendente |
| `FUN_1409339b0` | `FUN_1409339b0(...)` | Interno | 6 | pendente |
| `FUN_1402a5c70` | `FUN_1402a5c70(...)` | Interno | 6 | pendente |
| `FUN_1403d3b10` | `FUN_1403d3b10(...)` | Interno | 6 | pendente |
| `FUN_141874c00` | `FUN_141874c00(...)` | Interno | 6 | pendente |
| `FUN_140692160` | `FUN_140692160(...)` | Interno | 6 | pendente |
| `FUN_1404984c0` | `FUN_1404984c0(...)` | Interno | 6 | pendente |
| `FUN_1418847c0` | `FUN_1418847c0(...)` | Interno | 6 | pendente |
| `FUN_1407fa3d0` | `FUN_1407fa3d0(...)` | Interno | 6 | pendente |
| `FUN_1410623a0` | `FUN_1410623a0(...)` | Interno | 6 | pendente |
| `FUN_1409bb3e0` | `FUN_1409bb3e0(...)` | Interno | 6 | pendente |
| `FUN_14121ae80` | `FUN_14121ae80(...)` | Interno | 6 | pendente |
| `FUN_14135be30` | `FUN_14135be30(...)` | Interno | 6 | pendente |
| `FUN_1413fdfa0` | `FUN_1413fdfa0(...)` | Interno | 6 | pendente |
| `FUN_141588820` | `FUN_141588820(...)` | Interno | 6 | pendente |
| `FUN_141874ef0` | `FUN_141874ef0(...)` | Interno | 6 | pendente |
| `FUN_141874f70` | `FUN_141874f70(...)` | Interno | 6 | pendente |
| `FUN_14146ec50` | `FUN_14146ec50(...)` | Interno | 6 | pendente |
| `FUN_141884af0` | `FUN_141884af0(...)` | Interno | 6 | pendente |
| `FUN_1415aab30` | `FUN_1415aab30(...)` | Interno | 6 | pendente |
| `FUN_1403f4d90` | `FUN_1403f4d90(...)` | Interno | 6 | pendente |
| `FUN_141644a60` | `FUN_141644a60(...)` | Interno | 6 | pendente |
| `FUN_141190a60` | `FUN_141190a60(...)` | Interno | 6 | pendente |
| `FUN_1402eb570` | `FUN_1402eb570(...)` | Interno | 6 | pendente |
| `FUN_1417ce430` | `FUN_1417ce430(...)` | Interno | 6 | pendente |
| `FUN_14178b690` | `FUN_14178b690(...)` | Interno | 6 | pendente |
| `FUN_141874e60` | `FUN_141874e60(...)` | Interno | 6 | pendente |
| `FUN_1417a13f0` | `FUN_1417a13f0(...)` | Interno | 6 | pendente |
| `FUN_1410a6f10` | `FUN_1410a6f10(...)` | Interno | 6 | pendente |
| `FUN_14090c000` | `FUN_14090c000(...)` | Interno | 6 | pendente |
| `FUN_141013860` | `FUN_141013860(...)` | Interno | 6 | pendente |
| `FUN_140facc40` | `FUN_140facc40(...)` | Interno | 6 | pendente |
| `FUN_14066d5e0` | `FUN_14066d5e0(...)` | Interno | 6 | pendente |
| `FUN_1416a5d80` | `FUN_1416a5d80(...)` | Interno | 6 | pendente |
| `FUN_141449b30` | `FUN_141449b30(...)` | Interno | 6 | pendente |
| `FUN_141656790` | `FUN_141656790(...)` | Interno | 6 | pendente |
| `FUN_14011f2c0` | `FUN_14011f2c0(...)` | Interno | 6 | pendente |
| `FUN_1416396d0` | `FUN_1416396d0(...)` | Interno | 6 | pendente |
| `FUN_1406959b0` | `FUN_1406959b0(...)` | Interno | 6 | pendente |
| `FUN_14123b950` | `FUN_14123b950(...)` | Interno | 6 | pendente |
| `FUN_1416f57b0` | `FUN_1416f57b0(...)` | Interno | 6 | pendente |
| `FUN_141009b40` | `FUN_141009b40(...)` | Interno | 6 | pendente |
| `FUN_140a01270` | `FUN_140a01270(...)` | Interno | 6 | pendente |
| `FUN_141531b30` | `FUN_141531b30(...)` | Interno | 6 | pendente |
| `FUN_140095340` | `FUN_140095340(...)` | Interno | 6 | pendente |
| `FUN_1415acb70` | `FUN_1415acb70(...)` | Interno | 6 | pendente |
| `FUN_140510ce0` | `FUN_140510ce0(...)` | Interno | 6 | pendente |
| `FUN_1400b88c0` | `FUN_1400b88c0(...)` | Interno | 6 | pendente |
| `FUN_140989e10` | `FUN_140989e10(...)` | Interno | 6 | pendente |
| `FUN_140f9ea00` | `FUN_140f9ea00(...)` | Interno | 6 | pendente |
| `FUN_1414a1a10` | `FUN_1414a1a10(...)` | Interno | 6 | pendente |
| `FUN_14106bc90` | `FUN_14106bc90(...)` | Interno | 6 | pendente |
| `FUN_141054d50` | `FUN_141054d50(...)` | Interno | 6 | pendente |
| `FUN_141184c10` | `FUN_141184c10(...)` | Interno | 6 | pendente |
| `FUN_140841670` | `FUN_140841670(...)` | Interno | 6 | pendente |
| `FUN_14010fe50` | `FUN_14010fe50(...)` | Interno | 6 | pendente |
| `FUN_141657a20` | `FUN_141657a20(...)` | Interno | 6 | pendente |
| `FUN_14133b050` | `FUN_14133b050(...)` | Interno | 5 | pendente |
| `FUN_140917210` | `FUN_140917210(...)` | Interno | 5 | pendente |
| `FUN_1407d6460` | `FUN_1407d6460(...)` | Interno | 5 | pendente |
| `FUN_140ffa6f0` | `FUN_140ffa6f0(...)` | Interno | 5 | pendente |
| `FUN_14168f420` | `FUN_14168f420(...)` | Interno | 5 | pendente |
| `FUN_140488890` | `FUN_140488890(...)` | Interno | 5 | pendente |
| `FUN_141082aa0` | `FUN_141082aa0(...)` | Interno | 5 | pendente |
| `FUN_1412df220` | `FUN_1412df220(...)` | Interno | 5 | pendente |
| `FUN_1412a84c0` | `FUN_1412a84c0(...)` | Interno | 5 | pendente |
| `FUN_140768b90` | `FUN_140768b90(...)` | Interno | 5 | pendente |
| `FUN_1417b1900` | `FUN_1417b1900(...)` | Interno | 5 | pendente |
| `FUN_141148f60` | `FUN_141148f60(...)` | Interno | 5 | pendente |
| `FUN_14187bbf0` | `FUN_14187bbf0(...)` | Interno | 5 | pendente |
| `FUN_1410fd670` | `FUN_1410fd670(...)` | Interno | 5 | pendente |
| `FUN_141306a90` | `FUN_141306a90(...)` | Interno | 5 | pendente |
| `FUN_140127a80` | `FUN_140127a80(...)` | Interno | 5 | pendente |
| `FUN_140eceec0` | `FUN_140eceec0(...)` | Interno | 5 | pendente |
| `FUN_141343890` | `FUN_141343890(...)` | Interno | 5 | pendente |
| `FUN_1409877f0` | `FUN_1409877f0(...)` | Interno | 5 | pendente |
| `FUN_140ef00f0` | `FUN_140ef00f0(...)` | Interno | 5 | pendente |
| `FUN_1406813a0` | `FUN_1406813a0(...)` | Interno | 5 | pendente |
| `FUN_14150dc40` | `FUN_14150dc40(...)` | Interno | 5 | pendente |
| `FUN_1410ef170` | `FUN_1410ef170(...)` | Interno | 5 | pendente |
| `FUN_14134cb20` | `FUN_14134cb20(...)` | Interno | 5 | pendente |
| `FUN_1416d21d0` | `FUN_1416d21d0(...)` | Interno | 5 | pendente |
| `FUN_1413d4050` | `FUN_1413d4050(...)` | Interno | 5 | pendente |
| `FUN_140f2bc50` | `FUN_140f2bc50(...)` | Interno | 5 | pendente |
| `FUN_140a7c3b0` | `FUN_140a7c3b0(...)` | Interno | 5 | pendente |
| `FUN_1403a3510` | `FUN_1403a3510(...)` | Interno | 5 | pendente |
| `FUN_141470e70` | `FUN_141470e70(...)` | Interno | 5 | pendente |
| `FUN_140ffc440` | `FUN_140ffc440(...)` | Interno | 5 | pendente |
| `FUN_1410aa7b0` | `FUN_1410aa7b0(...)` | Interno | 5 | pendente |
| `FUN_1416e7e80` | `FUN_1416e7e80(...)` | Interno | 5 | pendente |
| `FUN_1411d6870` | `FUN_1411d6870(...)` | Interno | 5 | pendente |
| `FUN_1402b84a0` | `FUN_1402b84a0(...)` | Interno | 5 | pendente |
| `FUN_140824f50` | `FUN_140824f50(...)` | Interno | 5 | pendente |
| `FUN_141092510` | `FUN_141092510(...)` | Interno | 5 | pendente |
| `FUN_1400e3750` | `FUN_1400e3750(...)` | Interno | 5 | pendente |
| `FUN_1405c21a0` | `FUN_1405c21a0(...)` | Interno | 5 | pendente |
| `FUN_1417a6b60` | `FUN_1417a6b60(...)` | Interno | 5 | pendente |
| `FUN_140a9b3d0` | `FUN_140a9b3d0(...)` | Interno | 5 | pendente |
| `FUN_1414de9f0` | `FUN_1414de9f0(...)` | Interno | 5 | pendente |
| `FUN_1417f0a10` | `FUN_1417f0a10(...)` | Interno | 5 | pendente |
| `FUN_14173cf30` | `FUN_14173cf30(...)` | Interno | 5 | pendente |
| `FUN_1410fb100` | `FUN_1410fb100(...)` | Interno | 5 | pendente |
| `FUN_14003adb0` | `FUN_14003adb0(...)` | Interno | 5 | pendente |
| `FUN_1400ee620` | `FUN_1400ee620(...)` | Interno | 5 | pendente |
| `FUN_141170740` | `FUN_141170740(...)` | Interno | 5 | pendente |
| `FUN_1409e4fb0` | `FUN_1409e4fb0(...)` | Interno | 5 | pendente |
| `FUN_1409158b0` | `FUN_1409158b0(...)` | Interno | 5 | pendente |
| `FUN_141109060` | `FUN_141109060(...)` | Interno | 5 | pendente |
| `FUN_140017ab0` | `FUN_140017ab0(...)` | Interno | 5 | pendente |
| `FUN_14044ec70` | `FUN_14044ec70(...)` | Interno | 5 | pendente |
| `FUN_140a48df0` | `FUN_140a48df0(...)` | Interno | 4 | pendente |
| `FUN_141609340` | `FUN_141609340(...)` | Interno | 4 | pendente |
| `FUN_14036d4b0` | `FUN_14036d4b0(...)` | Interno | 4 | pendente |
| `FUN_141716a70` | `FUN_141716a70(...)` | Interno | 4 | pendente |
| `FUN_1413300b0` | `FUN_1413300b0(...)` | Interno | 4 | pendente |
| `FUN_140895690` | `FUN_140895690(...)` | Interno | 4 | pendente |
| `FUN_140820200` | `FUN_140820200(...)` | Interno | 4 | pendente |
| `FUN_1406b0d50` | `FUN_1406b0d50(...)` | Interno | 4 | pendente |
| `FUN_14165f450` | `FUN_14165f450(...)` | Interno | 4 | pendente |
| `FUN_1411e6db0` | `FUN_1411e6db0(...)` | Interno | 4 | pendente |
| `FUN_1409312c0` | `FUN_1409312c0(...)` | Interno | 4 | pendente |
| `FUN_141874e40` | `FUN_141874e40(...)` | Interno | 4 | pendente |
| `FUN_14105cdf0` | `FUN_14105cdf0(...)` | Interno | 4 | pendente |
| `FUN_14105fe40` | `FUN_14105fe40(...)` | Interno | 4 | pendente |
| `FUN_1417fa010` | `FUN_1417fa010(...)` | Interno | 4 | pendente |
| `FUN_140fe4050` | `FUN_140fe4050(...)` | Interno | 4 | pendente |
| `FUN_14023d2a0` | `FUN_14023d2a0(...)` | Interno | 4 | pendente |
| `FUN_140e93f60` | `FUN_140e93f60(...)` | Interno | 4 | pendente |
| `FUN_140af52c0` | `FUN_140af52c0(...)` | Interno | 4 | pendente |
| `FUN_141434d60` | `FUN_141434d60(...)` | Interno | 4 | pendente |
| `FUN_141620890` | `FUN_141620890(...)` | Interno | 4 | pendente |
| `FUN_141670540` | `FUN_141670540(...)` | Interno | 4 | pendente |
| `FUN_140ea73d0` | `FUN_140ea73d0(...)` | Interno | 4 | pendente |
| `FUN_140f213c0` | `FUN_140f213c0(...)` | Interno | 4 | pendente |
| `FUN_14174c680` | `FUN_14174c680(...)` | Interno | 4 | pendente |
| `FUN_140f52f30` | `FUN_140f52f30(...)` | Interno | 4 | pendente |
| `FUN_14159de00` | `FUN_14159de00(...)` | Interno | 4 | pendente |
| `FUN_141373300` | `FUN_141373300(...)` | Interno | 4 | pendente |
| `FUN_141571640` | `FUN_141571640(...)` | Interno | 4 | pendente |
| `FUN_141617c30` | `FUN_141617c30(...)` | Interno | 4 | pendente |
| `FUN_141891260` | `FUN_141891260(...)` | Interno | 4 | pendente |
| `FUN_140353430` | `FUN_140353430(...)` | Interno | 4 | pendente |
| `FUN_141075f90` | `FUN_141075f90(...)` | Interno | 4 | pendente |
| `FUN_1417b9700` | `FUN_1417b9700(...)` | Interno | 4 | pendente |
| `FUN_1410da020` | `FUN_1410da020(...)` | Interno | 4 | pendente |
| `FUN_140fa1220` | `FUN_140fa1220(...)` | Interno | 4 | pendente |
| `FUN_141734270` | `FUN_141734270(...)` | Interno | 4 | pendente |
| `FUN_14113c300` | `FUN_14113c300(...)` | Interno | 4 | pendente |
| `FUN_1412e1090` | `FUN_1412e1090(...)` | Interno | 4 | pendente |
| `FUN_1414ad9c0` | `FUN_1414ad9c0(...)` | Interno | 4 | pendente |
| `FUN_1410b6900` | `FUN_1410b6900(...)` | Interno | 4 | pendente |
| `FUN_141404750` | `FUN_141404750(...)` | Interno | 4 | pendente |
| `FUN_141467900` | `FUN_141467900(...)` | Interno | 4 | pendente |
| `FUN_1415c2d90` | `FUN_1415c2d90(...)` | Interno | 4 | pendente |
| `FUN_141122fd0` | `FUN_141122fd0(...)` | Interno | 4 | pendente |
| `FUN_1407a0580` | `FUN_1407a0580(...)` | Interno | 4 | pendente |
| `FUN_1416047c0` | `FUN_1416047c0(...)` | Interno | 4 | pendente |
| `FUN_141755720` | `FUN_141755720(...)` | Interno | 4 | pendente |
| `FUN_14129a1a0` | `FUN_14129a1a0(...)` | Interno | 4 | pendente |
| `FUN_1410d5520` | `FUN_1410d5520(...)` | Interno | 4 | pendente |
| `FUN_141805410` | `FUN_141805410(...)` | Interno | 4 | pendente |
| `FUN_14147b240` | `FUN_14147b240(...)` | Interno | 4 | pendente |
| `FUN_141118120` | `FUN_141118120(...)` | Interno | 4 | pendente |
| `FUN_141417840` | `FUN_141417840(...)` | Interno | 4 | pendente |
| `FUN_1411d2750` | `FUN_1411d2750(...)` | Interno | 4 | pendente |
| `FUN_1411e7900` | `FUN_1411e7900(...)` | Interno | 4 | pendente |
| `FUN_1412f6f50` | `FUN_1412f6f50(...)` | Interno | 4 | pendente |
| `FUN_14105a720` | `FUN_14105a720(...)` | Interno | 4 | pendente |
| `FUN_140fee3a0` | `FUN_140fee3a0(...)` | Interno | 4 | pendente |
| `FUN_140e77ee0` | `FUN_140e77ee0(...)` | Interno | 4 | pendente |
| `FUN_140fead80` | `FUN_140fead80(...)` | Interno | 4 | pendente |
| `FUN_1418016d0` | `FUN_1418016d0(...)` | Interno | 4 | pendente |
| `FUN_1414b1ef0` | `FUN_1414b1ef0(...)` | Interno | 4 | pendente |
| `FUN_1417e5650` | `FUN_1417e5650(...)` | Interno | 4 | pendente |
| `FUN_141366170` | `FUN_141366170(...)` | Interno | 4 | pendente |
| `FUN_141328c10` | `FUN_141328c10(...)` | Interno | 4 | pendente |
| `FUN_1414805d0` | `FUN_1414805d0(...)` | Interno | 4 | pendente |
| `FUN_1412572d0` | `FUN_1412572d0(...)` | Interno | 4 | pendente |
| `FUN_141224a70` | `FUN_141224a70(...)` | Interno | 4 | pendente |
| `FUN_1407bc3f0` | `FUN_1407bc3f0(...)` | Interno | 4 | pendente |
| `FUN_14064f710` | `FUN_14064f710(...)` | Interno | 4 | pendente |
| `FUN_1415f1bf0` | `FUN_1415f1bf0(...)` | Interno | 4 | pendente |
| `FUN_1408a2750` | `FUN_1408a2750(...)` | Interno | 4 | pendente |
| `FUN_140f13650` | `FUN_140f13650(...)` | Interno | 4 | pendente |
| `FUN_1411ecce0` | `FUN_1411ecce0(...)` | Interno | 4 | pendente |
| `FUN_1400f9cf0` | `FUN_1400f9cf0(...)` | Interno | 3 | pendente |
| `FUN_1404ea930` | `FUN_1404ea930(...)` | Interno | 3 | pendente |
| `FUN_1400934a0` | `FUN_1400934a0(...)` | Interno | 3 | pendente |
| `FUN_1407fc9c0` | `FUN_1407fc9c0(...)` | Interno | 3 | pendente |
| `FUN_141759e40` | `FUN_141759e40(...)` | Interno | 3 | pendente |
| `FUN_1405d6300` | `FUN_1405d6300(...)` | Interno | 3 | pendente |
| `FUN_1405bf2f0` | `FUN_1405bf2f0(...)` | Interno | 3 | pendente |
| `FUN_140e6dd50` | `FUN_140e6dd50(...)` | Interno | 3 | pendente |
| `FUN_140590aa0` | `FUN_140590aa0(...)` | Interno | 3 | pendente |
| `FUN_1405bf270` | `FUN_1405bf270(...)` | Interno | 3 | pendente |
| `FUN_1402de7b0` | `FUN_1402de7b0(...)` | Interno | 3 | pendente |
| `FUN_14175e490` | `FUN_14175e490(...)` | Interno | 3 | pendente |
| `FUN_141874ea0` | `FUN_141874ea0(...)` | Interno | 3 | pendente |
| `FUN_14101e570` | `FUN_14101e570(...)` | Interno | 3 | pendente |
| `FUN_141884740` | `FUN_141884740(...)` | Interno | 3 | pendente |
| `FUN_1401af4d0` | `FUN_1401af4d0(...)` | Interno | 3 | pendente |
| `FUN_141747e10` | `FUN_141747e10(...)` | Interno | 3 | pendente |
| `FUN_14124a850` | `FUN_14124a850(...)` | Interno | 3 | pendente |
| `FUN_1411515a0` | `FUN_1411515a0(...)` | Interno | 3 | pendente |
| `FUN_140624a90` | `FUN_140624a90(...)` | Interno | 3 | pendente |
| `FUN_140fff390` | `FUN_140fff390(...)` | Interno | 3 | pendente |
| `FUN_14157d430` | `FUN_14157d430(...)` | Interno | 3 | pendente |
| `FUN_1415b5d60` | `FUN_1415b5d60(...)` | Interno | 3 | pendente |
| `FUN_1404e5820` | `FUN_1404e5820(...)` | Interno | 3 | pendente |
| `FUN_141262c30` | `FUN_141262c30(...)` | Interno | 3 | pendente |
| `FUN_141175880` | `FUN_141175880(...)` | Interno | 3 | pendente |
| `FUN_1415fcc10` | `FUN_1415fcc10(...)` | Interno | 3 | pendente |
| `FUN_140f98bd0` | `FUN_140f98bd0(...)` | Interno | 3 | pendente |
| `FUN_1409e18a0` | `FUN_1409e18a0(...)` | Interno | 3 | pendente |
| `FUN_1403a3440` | `FUN_1403a3440(...)` | Interno | 3 | pendente |
| `FUN_141097790` | `FUN_141097790(...)` | Interno | 3 | pendente |
| `FUN_1400d5410` | `FUN_1400d5410(...)` | Interno | 3 | pendente |
| `FUN_14135e720` | `FUN_14135e720(...)` | Interno | 3 | pendente |
| `FUN_14173e160` | `FUN_14173e160(...)` | Interno | 3 | pendente |
| `FUN_141283650` | `FUN_141283650(...)` | Interno | 3 | pendente |
| `FUN_1405b0750` | `FUN_1405b0750(...)` | Interno | 3 | pendente |
| `FUN_140221be0` | `FUN_140221be0(...)` | Interno | 3 | pendente |
| `FUN_140f6ab60` | `FUN_140f6ab60(...)` | Interno | 3 | pendente |
| `FUN_140505040` | `FUN_140505040(...)` | Interno | 3 | pendente |
| `FUN_141095c50` | `FUN_141095c50(...)` | Interno | 3 | pendente |
| `FUN_14163b0c0` | `FUN_14163b0c0(...)` | Interno | 3 | pendente |
| `FUN_141782850` | `FUN_141782850(...)` | Interno | 3 | pendente |
| `FUN_1410bf6e0` | `FUN_1410bf6e0(...)` | Interno | 3 | pendente |
| `FUN_14186ce70` | `FUN_14186ce70(...)` | Interno | 3 | pendente |
| `FUN_141469a70` | `FUN_141469a70(...)` | Interno | 3 | pendente |
| `FUN_140f1d6e0` | `FUN_140f1d6e0(...)` | Interno | 3 | pendente |
| `FUN_14152df10` | `FUN_14152df10(...)` | Interno | 3 | pendente |
| `FUN_1416ea260` | `FUN_1416ea260(...)` | Interno | 3 | pendente |
| `FUN_14038e8f0` | `FUN_14038e8f0(...)` | Interno | 3 | pendente |
| `FUN_1417b2d80` | `FUN_1417b2d80(...)` | Interno | 3 | pendente |
| `FUN_14095ab60` | `FUN_14095ab60(...)` | Interno | 3 | pendente |
| `FUN_14120bba0` | `FUN_14120bba0(...)` | Interno | 3 | pendente |
| `FUN_1400ff100` | `FUN_1400ff100(...)` | Interno | 3 | pendente |
| `FUN_14082d2f0` | `FUN_14082d2f0(...)` | Interno | 3 | pendente |
| `FUN_1403f9620` | `FUN_1403f9620(...)` | Interno | 3 | pendente |
| `FUN_14036ff70` | `FUN_14036ff70(...)` | Interno | 3 | pendente |
| `FUN_1409ddd60` | `FUN_1409ddd60(...)` | Interno | 3 | pendente |
| `FUN_1402184c0` | `FUN_1402184c0(...)` | Interno | 3 | pendente |
| `FUN_1407f64c0` | `FUN_1407f64c0(...)` | Interno | 3 | pendente |
| `FUN_14024abc0` | `FUN_14024abc0(...)` | Interno | 3 | pendente |
| `FUN_141607c90` | `FUN_141607c90(...)` | Interno | 3 | pendente |
| `FUN_1409ea570` | `FUN_1409ea570(...)` | Interno | 3 | pendente |
| `FUN_1405d41f0` | `FUN_1405d41f0(...)` | Interno | 3 | pendente |
| `FUN_1416c3740` | `FUN_1416c3740(...)` | Interno | 3 | pendente |
| `FUN_140e90540` | `FUN_140e90540(...)` | Interno | 3 | pendente |
| `FUN_1414b8eb0` | `FUN_1414b8eb0(...)` | Interno | 3 | pendente |
| `FUN_14120a3e0` | `FUN_14120a3e0(...)` | Interno | 3 | pendente |
| `FUN_1417b8900` | `FUN_1417b8900(...)` | Interno | 3 | pendente |
| `FUN_140f11520` | `FUN_140f11520(...)` | Interno | 3 | pendente |
| `FUN_140035020` | `FUN_140035020(...)` | Interno | 3 | pendente |
| `FUN_140f333a0` | `FUN_140f333a0(...)` | Interno | 3 | pendente |
| `FUN_141193ed0` | `FUN_141193ed0(...)` | Interno | 3 | pendente |
| `FUN_14149d870` | `FUN_14149d870(...)` | Interno | 3 | pendente |
| `FUN_140652730` | `FUN_140652730(...)` | Interno | 3 | pendente |
| `FUN_140eb3ce0` | `FUN_140eb3ce0(...)` | Interno | 3 | pendente |
| `FUN_140ed7010` | `FUN_140ed7010(...)` | Interno | 3 | pendente |
| `FUN_1411525d0` | `FUN_1411525d0(...)` | Interno | 3 | pendente |
| `FUN_14018a320` | `FUN_14018a320(...)` | Interno | 3 | pendente |
| `FUN_1415f36b0` | `FUN_1415f36b0(...)` | Interno | 3 | pendente |
| `FUN_140037e90` | `FUN_140037e90(...)` | Interno | 3 | pendente |
| `FUN_1410fbe10` | `FUN_1410fbe10(...)` | Interno | 3 | pendente |
| `FUN_14132a400` | `FUN_14132a400(...)` | Interno | 3 | pendente |
| `FUN_141666be0` | `FUN_141666be0(...)` | Interno | 3 | pendente |
| `FUN_140eb4bc0` | `FUN_140eb4bc0(...)` | Interno | 3 | pendente |
| `FUN_14130a770` | `FUN_14130a770(...)` | Interno | 3 | pendente |
| `FUN_141472070` | `FUN_141472070(...)` | Interno | 3 | pendente |
| `FUN_141463c70` | `FUN_141463c70(...)` | Interno | 3 | pendente |
| `FUN_1410ab2a0` | `FUN_1410ab2a0(...)` | Interno | 3 | pendente |
| `FUN_1417444c0` | `FUN_1417444c0(...)` | Interno | 3 | pendente |
| `FUN_14009a000` | `FUN_14009a000(...)` | Interno | 3 | pendente |
| `FUN_140edc640` | `FUN_140edc640(...)` | Interno | 3 | pendente |
| `FUN_140fcae60` | `FUN_140fcae60(...)` | Interno | 3 | pendente |
| `FUN_1412ca960` | `FUN_1412ca960(...)` | Interno | 3 | pendente |
| `FUN_141321340` | `FUN_141321340(...)` | Interno | 3 | pendente |
| `FUN_14146b6a0` | `FUN_14146b6a0(...)` | Interno | 3 | pendente |
| `FUN_140f2d3d0` | `FUN_140f2d3d0(...)` | Interno | 3 | pendente |
| `FUN_1410b4980` | `FUN_1410b4980(...)` | Interno | 3 | pendente |
| `FUN_1417b5810` | `FUN_1417b5810(...)` | Interno | 3 | pendente |
| `FUN_140e690a0` | `FUN_140e690a0(...)` | Interno | 3 | pendente |
| `FUN_1400a2270` | `FUN_1400a2270(...)` | Interno | 3 | pendente |
| `FUN_141265720` | `FUN_141265720(...)` | Interno | 3 | pendente |
| `FUN_141244170` | `FUN_141244170(...)` | Interno | 3 | pendente |
| `FUN_141537500` | `FUN_141537500(...)` | Interno | 3 | pendente |
| `FUN_140f1a5f0` | `FUN_140f1a5f0(...)` | Interno | 3 | pendente |
| `FUN_1410185c0` | `FUN_1410185c0(...)` | Interno | 3 | pendente |
| `FUN_141021df0` | `FUN_141021df0(...)` | Interno | 3 | pendente |
| `FUN_1410f23e0` | `FUN_1410f23e0(...)` | Interno | 3 | pendente |
| `FUN_140304410` | `FUN_140304410(...)` | Interno | 3 | pendente |
| `FUN_14125e820` | `FUN_14125e820(...)` | Interno | 2 | pendente |
| `FUN_14020a360` | `FUN_14020a360(...)` | Interno | 2 | pendente |
| `FUN_14142fa20` | `FUN_14142fa20(...)` | Interno | 2 | pendente |
| `FUN_14067e380` | `FUN_14067e380(...)` | Interno | 2 | pendente |
| `FUN_1410b9060` | `FUN_1410b9060(...)` | Interno | 2 | pendente |
| `FUN_1400802e0` | `FUN_1400802e0(...)` | Interno | 2 | pendente |
| `FUN_1403053d0` | `FUN_1403053d0(...)` | Interno | 2 | pendente |
| `FUN_140f26930` | `FUN_140f26930(...)` | Interno | 2 | pendente |
| `FUN_140e95720` | `FUN_140e95720(...)` | Interno | 2 | pendente |
| `FUN_14017d130` | `FUN_14017d130(...)` | Interno | 2 | pendente |
| `FUN_140a5bd70` | `FUN_140a5bd70(...)` | Interno | 2 | pendente |
| `FUN_140100260` | `FUN_140100260(...)` | Interno | 2 | pendente |
| `FUN_14101a730` | `FUN_14101a730(...)` | Interno | 2 | pendente |
| `FUN_141140ae0` | `FUN_141140ae0(...)` | Interno | 2 | pendente |
| `FUN_140f1f0b0` | `FUN_140f1f0b0(...)` | Interno | 2 | pendente |
| `FUN_14052c460` | `FUN_14052c460(...)` | Interno | 2 | pendente |
| `FUN_1403f1b00` | `FUN_1403f1b00(...)` | Interno | 2 | pendente |
| `FUN_1415db630` | `FUN_1415db630(...)` | Interno | 2 | pendente |
| `FUN_1414cf0b0` | `FUN_1414cf0b0(...)` | Interno | 2 | pendente |
| `FUN_1416f11a0` | `FUN_1416f11a0(...)` | Interno | 2 | pendente |
| `FUN_14062cc00` | `FUN_14062cc00(...)` | Interno | 2 | pendente |
| `FUN_140ffd2c0` | `FUN_140ffd2c0(...)` | Interno | 2 | pendente |
| `FUN_141756b20` | `FUN_141756b20(...)` | Interno | 2 | pendente |
| `FUN_140ed1100` | `FUN_140ed1100(...)` | Interno | 2 | pendente |
| `FUN_14029f950` | `FUN_14029f950(...)` | Interno | 2 | pendente |
| `FUN_140332200` | `FUN_140332200(...)` | Interno | 2 | pendente |
| `FUN_1410f44d0` | `FUN_1410f44d0(...)` | Interno | 2 | pendente |
| `FUN_140f36800` | `FUN_140f36800(...)` | Interno | 2 | pendente |
| `FUN_141762300` | `FUN_141762300(...)` | Interno | 2 | pendente |
| `FUN_14134d730` | `FUN_14134d730(...)` | Interno | 2 | pendente |
| `FUN_141457720` | `FUN_141457720(...)` | Interno | 2 | pendente |
| `FUN_140257a70` | `FUN_140257a70(...)` | Interno | 2 | pendente |
| `FUN_1416a3490` | `FUN_1416a3490(...)` | Interno | 2 | pendente |
| `FUN_14085dfe0` | `FUN_14085dfe0(...)` | Interno | 2 | pendente |
| `FUN_141353fd0` | `FUN_141353fd0(...)` | Interno | 2 | pendente |
| `FUN_1408172e0` | `FUN_1408172e0(...)` | Interno | 2 | pendente |
| `FUN_14160a6e0` | `FUN_14160a6e0(...)` | Interno | 2 | pendente |
| `FUN_1410e6bf0` | `FUN_1410e6bf0(...)` | Interno | 2 | pendente |
| `FUN_14081db10` | `FUN_14081db10(...)` | Interno | 2 | pendente |
| `FUN_14145c470` | `FUN_14145c470(...)` | Interno | 2 | pendente |
| `FUN_141763240` | `FUN_141763240(...)` | Interno | 2 | pendente |
| `FUN_14017d2b0` | `FUN_14017d2b0(...)` | Interno | 2 | pendente |
| `FUN_1417a2c20` | `FUN_1417a2c20(...)` | Interno | 2 | pendente |
| `FUN_140f93d70` | `FUN_140f93d70(...)` | Interno | 2 | pendente |
| `FUN_140f28490` | `FUN_140f28490(...)` | Interno | 2 | pendente |
| `FUN_14107ee10` | `FUN_14107ee10(...)` | Interno | 2 | pendente |
| `FUN_140390910` | `FUN_140390910(...)` | Interno | 2 | pendente |
| `FUN_1414d4620` | `FUN_1414d4620(...)` | Interno | 2 | pendente |
| `FUN_1415b3470` | `FUN_1415b3470(...)` | Interno | 2 | pendente |
| `FUN_141093250` | `FUN_141093250(...)` | Interno | 2 | pendente |
| `FUN_141723060` | `FUN_141723060(...)` | Interno | 2 | pendente |
| `FUN_141884b30` | `FUN_141884b30(...)` | Interno | 2 | pendente |
| `FUN_14028ead0` | `FUN_14028ead0(...)` | Interno | 2 | pendente |
| `FUN_140361b40` | `FUN_140361b40(...)` | Interno | 2 | pendente |
| `FUN_14039ae60` | `FUN_14039ae60(...)` | Interno | 2 | pendente |
| `FUN_1400b6e70` | `FUN_1400b6e70(...)` | Interno | 2 | pendente |
| `FUN_1409747b0` | `FUN_1409747b0(...)` | Interno | 2 | pendente |
| `FUN_140195f40` | `FUN_140195f40(...)` | Interno | 2 | pendente |
| `FUN_140811020` | `FUN_140811020(...)` | Interno | 2 | pendente |
| `FUN_140e7cac0` | `FUN_140e7cac0(...)` | Interno | 2 | pendente |
| `FUN_141043880` | `FUN_141043880(...)` | Interno | 2 | pendente |
| `FUN_1402d3af0` | `FUN_1402d3af0(...)` | Interno | 2 | pendente |
| `FUN_1417f1c20` | `FUN_1417f1c20(...)` | Interno | 2 | pendente |
| `FUN_1407c3990` | `FUN_1407c3990(...)` | Interno | 2 | pendente |
| `FUN_1407eafc0` | `FUN_1407eafc0(...)` | Interno | 2 | pendente |
| `FUN_140eb1960` | `FUN_140eb1960(...)` | Interno | 2 | pendente |
| `FUN_140ea5ea0` | `FUN_140ea5ea0(...)` | Interno | 2 | pendente |
| `FUN_140f34e50` | `FUN_140f34e50(...)` | Interno | 2 | pendente |
| `FUN_140276b40` | `FUN_140276b40(...)` | Interno | 2 | pendente |
| `FUN_1400f2690` | `FUN_1400f2690(...)` | Interno | 2 | pendente |
| `FUN_140107320` | `FUN_140107320(...)` | Interno | 2 | pendente |
| `FUN_1411ea6e0` | `FUN_1411ea6e0(...)` | Interno | 2 | pendente |
| `FUN_1417f5760` | `FUN_1417f5760(...)` | Interno | 2 | pendente |
| `FUN_14088ae60` | `FUN_14088ae60(...)` | Interno | 2 | pendente |
| `FUN_1404057e0` | `FUN_1404057e0(...)` | Interno | 2 | pendente |
| `FUN_14061c240` | `FUN_14061c240(...)` | Interno | 2 | pendente |
| `FUN_141874e10` | `FUN_141874e10(...)` | Interno | 2 | pendente |
| `FUN_14008e590` | `FUN_14008e590(...)` | Interno | 2 | pendente |
| `FUN_140e74ff0` | `FUN_140e74ff0(...)` | Interno | 2 | pendente |
| `FUN_1412f3de0` | `FUN_1412f3de0(...)` | Interno | 2 | pendente |
| `FUN_141725db0` | `FUN_141725db0(...)` | Interno | 2 | pendente |
| `FUN_14097fa70` | `FUN_14097fa70(...)` | Interno | 2 | pendente |
| `FUN_140181d90` | `FUN_140181d90(...)` | Interno | 2 | pendente |
| `FUN_140fb5cf0` | `FUN_140fb5cf0(...)` | Interno | 2 | pendente |
| `FUN_1412f6060` | `FUN_1412f6060(...)` | Interno | 2 | pendente |
| `FUN_141461670` | `FUN_141461670(...)` | Interno | 2 | pendente |
| `FUN_14067ccd0` | `FUN_14067ccd0(...)` | Interno | 2 | pendente |
| `FUN_141614e00` | `FUN_141614e00(...)` | Interno | 2 | pendente |
| `FUN_140e92dd0` | `FUN_140e92dd0(...)` | Interno | 2 | pendente |
| `FUN_141752140` | `FUN_141752140(...)` | Interno | 2 | pendente |
| `FUN_1402df240` | `FUN_1402df240(...)` | Interno | 2 | pendente |
| `FUN_141376cc0` | `FUN_141376cc0(...)` | Interno | 2 | pendente |
| `FUN_140779aa0` | `FUN_140779aa0(...)` | Interno | 2 | pendente |
| `FUN_140694150` | `FUN_140694150(...)` | Interno | 2 | pendente |
| `FUN_140e890d0` | `FUN_140e890d0(...)` | Interno | 2 | pendente |
| `FUN_14066a650` | `FUN_14066a650(...)` | Interno | 2 | pendente |
| `FUN_141186c50` | `FUN_141186c50(...)` | Interno | 2 | pendente |
| `FUN_1408de2a0` | `FUN_1408de2a0(...)` | Interno | 2 | pendente |
| `FUN_1413051d0` | `FUN_1413051d0(...)` | Interno | 2 | pendente |
| `FUN_141196930` | `FUN_141196930(...)` | Interno | 2 | pendente |
| `FUN_1409c3aa0` | `FUN_1409c3aa0(...)` | Interno | 2 | pendente |
| `FUN_1410e61b0` | `FUN_1410e61b0(...)` | Interno | 2 | pendente |
| `FUN_14124b7e0` | `FUN_14124b7e0(...)` | Interno | 2 | pendente |
| `FUN_1410228b0` | `FUN_1410228b0(...)` | Interno | 2 | pendente |
| `FUN_141058970` | `FUN_141058970(...)` | Interno | 2 | pendente |
| `FUN_1411e8900` | `FUN_1411e8900(...)` | Interno | 2 | pendente |
| `FUN_14158e010` | `FUN_14158e010(...)` | Interno | 2 | pendente |
| `FUN_14023f7f0` | `FUN_14023f7f0(...)` | Interno | 2 | pendente |
| `FUN_141250cc0` | `FUN_141250cc0(...)` | Interno | 2 | pendente |
| `FUN_1412da3b0` | `FUN_1412da3b0(...)` | Interno | 2 | pendente |
| `FUN_14147ed30` | `FUN_14147ed30(...)` | Interno | 2 | pendente |
| `FUN_14136aaa0` | `FUN_14136aaa0(...)` | Interno | 2 | pendente |
| `FUN_1405122b0` | `FUN_1405122b0(...)` | Interno | 2 | pendente |
| `FUN_141179310` | `FUN_141179310(...)` | Interno | 2 | pendente |
| `FUN_140798090` | `FUN_140798090(...)` | Interno | 2 | pendente |
| `FUN_141482390` | `FUN_141482390(...)` | Interno | 2 | pendente |
| `FUN_140282850` | `FUN_140282850(...)` | Interno | 2 | pendente |
| `FUN_140f12fb0` | `FUN_140f12fb0(...)` | Interno | 2 | pendente |
| `FUN_1409c4b30` | `FUN_1409c4b30(...)` | Interno | 2 | pendente |
| `FUN_141581a40` | `FUN_141581a40(...)` | Interno | 2 | pendente |
| `FUN_1410b7c20` | `FUN_1410b7c20(...)` | Interno | 2 | pendente |
| `FUN_140e7b890` | `FUN_140e7b890(...)` | Interno | 2 | pendente |
| `FUN_140ae8bf0` | `FUN_140ae8bf0(...)` | Interno | 2 | pendente |
| `FUN_14107d5c0` | `FUN_14107d5c0(...)` | Interno | 2 | pendente |
| `FUN_141710920` | `FUN_141710920(...)` | Interno | 2 | pendente |
| `FUN_140045be0` | `FUN_140045be0(...)` | Interno | 2 | pendente |
| `FUN_140199980` | `FUN_140199980(...)` | Interno | 2 | pendente |
| `FUN_14043ca50` | `FUN_14043ca50(...)` | Interno | 2 | pendente |
| `FUN_14117c300` | `FUN_14117c300(...)` | Interno | 2 | pendente |
| `FUN_140ac4fe0` | `FUN_140ac4fe0(...)` | Interno | 2 | pendente |
| `FUN_140659760` | `FUN_140659760(...)` | Interno | 2 | pendente |
| `FUN_140f47770` | `FUN_140f47770(...)` | Interno | 2 | pendente |
| `FUN_1416d4210` | `FUN_1416d4210(...)` | Interno | 2 | pendente |
| `FUN_141605c00` | `FUN_141605c00(...)` | Interno | 2 | pendente |
| `FUN_1415dc770` | `FUN_1415dc770(...)` | Interno | 2 | pendente |
| `FUN_14159c230` | `FUN_14159c230(...)` | Interno | 2 | pendente |
| `FUN_1414ae560` | `FUN_1414ae560(...)` | Interno | 2 | pendente |
| `FUN_1401871f0` | `FUN_1401871f0(...)` | Interno | 2 | pendente |
| `FUN_14076e490` | `FUN_14076e490(...)` | Interno | 2 | pendente |
| `FUN_1402d2df0` | `FUN_1402d2df0(...)` | Interno | 2 | pendente |
| `FUN_140e9ae00` | `FUN_140e9ae00(...)` | Interno | 2 | pendente |
| `FUN_141402330` | `FUN_141402330(...)` | Interno | 2 | pendente |
| `FUN_14003ca10` | `FUN_14003ca10(...)` | Interno | 2 | pendente |
| `FUN_141011580` | `FUN_141011580(...)` | Interno | 2 | pendente |
| `FUN_14108b850` | `FUN_14108b850(...)` | Interno | 2 | pendente |
| `FUN_1416ecd20` | `FUN_1416ecd20(...)` | Interno | 2 | pendente |
| `FUN_141671890` | `FUN_141671890(...)` | Interno | 2 | pendente |
| `FUN_14162fec0` | `FUN_14162fec0(...)` | Interno | 2 | pendente |
| `FUN_141532690` | `FUN_141532690(...)` | Interno | 2 | pendente |
| `FUN_1410ed390` | `FUN_1410ed390(...)` | Interno | 2 | pendente |
| `FUN_14106d5f0` | `FUN_14106d5f0(...)` | Interno | 2 | pendente |
| `FUN_1402d16d0` | `FUN_1402d16d0(...)` | Interno | 2 | pendente |
| `FUN_1411f7880` | `FUN_1411f7880(...)` | Interno | 2 | pendente |
| `FUN_14063f630` | `FUN_14063f630(...)` | Interno | 2 | pendente |
| `FUN_1402b91d0` | `FUN_1402b91d0(...)` | Interno | 2 | pendente |
| `FUN_140fc7e10` | `FUN_140fc7e10(...)` | Interno | 2 | pendente |
| `FUN_1411cfe20` | `FUN_1411cfe20(...)` | Interno | 2 | pendente |
| `FUN_141802d80` | `FUN_141802d80(...)` | Interno | 2 | pendente |
| `FUN_1400efa50` | `FUN_1400efa50(...)` | Interno | 2 | pendente |
| `FUN_141164d90` | `FUN_141164d90(...)` | Interno | 2 | pendente |
| `FUN_14124f620` | `FUN_14124f620(...)` | Interno | 2 | pendente |
| `FUN_141138250` | `FUN_141138250(...)` | Interno | 2 | pendente |
| `FUN_1407d3c60` | `FUN_1407d3c60(...)` | Interno | 2 | pendente |
| `FUN_140f3aa80` | `FUN_140f3aa80(...)` | Interno | 2 | pendente |
| `FUN_140f3b580` | `FUN_140f3b580(...)` | Interno | 2 | pendente |
| `FUN_1410bd230` | `FUN_1410bd230(...)` | Interno | 2 | pendente |
| `FUN_1411866a0` | `FUN_1411866a0(...)` | Interno | 2 | pendente |
| `FUN_140a69070` | `FUN_140a69070(...)` | Interno | 2 | pendente |
| `FUN_141249420` | `FUN_141249420(...)` | Interno | 2 | pendente |
| `FUN_14158bd20` | `FUN_14158bd20(...)` | Interno | 2 | pendente |
| `FUN_14066e410` | `FUN_14066e410(...)` | Interno | 2 | pendente |
| `FUN_1413742b0` | `FUN_1413742b0(...)` | Interno | 2 | pendente |
| `FUN_140f61fd0` | `FUN_140f61fd0(...)` | Interno | 2 | pendente |
| `FUN_140fd6600` | `FUN_140fd6600(...)` | Interno | 2 | pendente |
| `FUN_140fff890` | `FUN_140fff890(...)` | Interno | 2 | pendente |
| `FUN_140a42e00` | `FUN_140a42e00(...)` | Interno | 2 | pendente |
| `FUN_14164fd70` | `FUN_14164fd70(...)` | Interno | 2 | pendente |
| `FUN_140ae4b60` | `FUN_140ae4b60(...)` | Interno | 2 | pendente |
| `FUN_14067d210` | `FUN_14067d210(...)` | Interno | 2 | pendente |
| `FUN_141227740` | `FUN_141227740(...)` | Interno | 2 | pendente |
| `FUN_140f38ce0` | `FUN_140f38ce0(...)` | Interno | 2 | pendente |
| `FUN_140f63630` | `FUN_140f63630(...)` | Interno | 2 | pendente |
| `FUN_141980d20` | `FUN_141980d20(...)` | Interno | 2 | pendente |
| `FUN_140e69d70` | `FUN_140e69d70(...)` | Interno | 2 | pendente |
| `FUN_1407bef60` | `FUN_1407bef60(...)` | Interno | 2 | pendente |
| `FUN_140f77700` | `FUN_140f77700(...)` | Interno | 2 | pendente |
| `FUN_1401cf360` | `FUN_1401cf360(...)` | Interno | 1 | pendente |
| `FUN_14133aef0` | `FUN_14133aef0(...)` | Interno | 1 | pendente |
| `FUN_1402c7ba0` | `FUN_1402c7ba0(...)` | Interno | 1 | pendente |
| `FUN_1404fe4f0` | `FUN_1404fe4f0(...)` | Interno | 1 | pendente |
| `FUN_14041c9b0` | `FUN_14041c9b0(...)` | Interno | 1 | pendente |
| `FUN_14041c6a0` | `FUN_14041c6a0(...)` | Interno | 1 | pendente |
| `FUN_1402417a0` | `FUN_1402417a0(...)` | Interno | 1 | pendente |
| `FUN_14063e040` | `FUN_14063e040(...)` | Interno | 1 | pendente |
| `FUN_140400500` | `FUN_140400500(...)` | Interno | 1 | pendente |
| `FUN_14038cb30` | `FUN_14038cb30(...)` | Interno | 1 | pendente |
| `FUN_14038ca80` | `FUN_14038ca80(...)` | Interno | 1 | pendente |
| `FUN_14038cbe0` | `FUN_14038cbe0(...)` | Interno | 1 | pendente |
| `FUN_14038cc90` | `FUN_14038cc90(...)` | Interno | 1 | pendente |
| `FUN_1407b7ba0` | `FUN_1407b7ba0(...)` | Interno | 1 | pendente |
| `FUN_1400800d0` | `FUN_1400800d0(...)` | Interno | 1 | pendente |
| `FUN_140865610` | `FUN_140865610(...)` | Interno | 1 | pendente |
| `FUN_1402a27f0` | `FUN_1402a27f0(...)` | Interno | 1 | pendente |
| `FUN_1406755f0` | `FUN_1406755f0(...)` | Interno | 1 | pendente |
| `FUN_1403c7330` | `FUN_1403c7330(...)` | Interno | 1 | pendente |
| `FUN_1403c6ef0` | `FUN_1403c6ef0(...)` | Interno | 1 | pendente |
| `FUN_140444890` | `FUN_140444890(...)` | Interno | 1 | pendente |
| `FUN_140101b20` | `FUN_140101b20(...)` | Interno | 1 | pendente |
| `FUN_14104beb0` | `FUN_14104beb0(...)` | Interno | 1 | pendente |
| `FUN_1401eff60` | `FUN_1401eff60(...)` | Interno | 1 | pendente |
| `FUN_1401fe200` | `FUN_1401fe200(...)` | Interno | 1 | pendente |
| `FUN_141343ef0` | `FUN_141343ef0(...)` | Interno | 1 | pendente |
| `FUN_141343e20` | `FUN_141343e20(...)` | Interno | 1 | pendente |
| `FUN_1407cb440` | `FUN_1407cb440(...)` | Interno | 1 | pendente |
| `FUN_1403a6d40` | `FUN_1403a6d40(...)` | Interno | 1 | pendente |
| `FUN_1407faaf0` | `FUN_1407faaf0(...)` | Interno | 1 | pendente |
| `FUN_141063810` | `FUN_141063810(...)` | Interno | 1 | pendente |
| `FUN_1404b3c90` | `FUN_1404b3c90(...)` | Interno | 1 | pendente |
| `FUN_1401268a0` | `FUN_1401268a0(...)` | Interno | 1 | pendente |
| `FUN_1400dd040` | `FUN_1400dd040(...)` | Interno | 1 | pendente |
| `FUN_1400dccd0` | `FUN_1400dccd0(...)` | Interno | 1 | pendente |
| `FUN_1404c8280` | `FUN_1404c8280(...)` | Interno | 1 | pendente |
| `FUN_1404c7e60` | `FUN_1404c7e60(...)` | Interno | 1 | pendente |
| `FUN_1405bcb30` | `FUN_1405bcb30(...)` | Interno | 1 | pendente |
| `FUN_1405bc920` | `FUN_1405bc920(...)` | Interno | 1 | pendente |
| `FUN_140494fc0` | `FUN_140494fc0(...)` | Interno | 1 | pendente |
| `FUN_1403a61a0` | `FUN_1403a61a0(...)` | Interno | 1 | pendente |
| `FUN_1403a5fe0` | `FUN_1403a5fe0(...)` | Interno | 1 | pendente |
| `FUN_14147d370` | `FUN_14147d370(...)` | Interno | 1 | pendente |
| `FUN_14132ff80` | `FUN_14132ff80(...)` | Interno | 1 | pendente |
| `FUN_14111b310` | `FUN_14111b310(...)` | Interno | 1 | pendente |
| `FUN_140ab2980` | `FUN_140ab2980(...)` | Interno | 1 | pendente |
| `FUN_1408860b0` | `FUN_1408860b0(...)` | Interno | 1 | pendente |
| `FUN_14127e3a0` | `FUN_14127e3a0(...)` | Interno | 1 | pendente |
| `FUN_141280da0` | `FUN_141280da0(...)` | Interno | 1 | pendente |
| `FUN_140487420` | `FUN_140487420(...)` | Interno | 1 | pendente |
| `FUN_1410284f0` | `FUN_1410284f0(...)` | Interno | 1 | pendente |
| `FUN_141049d70` | `FUN_141049d70(...)` | Interno | 1 | pendente |
| `FUN_1410005c0` | `FUN_1410005c0(...)` | Interno | 1 | pendente |
| `FUN_1406149b0` | `FUN_1406149b0(...)` | Interno | 1 | pendente |
| `FUN_1412360b0` | `FUN_1412360b0(...)` | Interno | 1 | pendente |
| `FUN_141236180` | `FUN_141236180(...)` | Interno | 1 | pendente |
| `FUN_14135bd00` | `FUN_14135bd00(...)` | Interno | 1 | pendente |
| `FUN_140f41b50` | `FUN_140f41b50(...)` | Interno | 1 | pendente |
| `FUN_140ed0c00` | `FUN_140ed0c00(...)` | Interno | 1 | pendente |
| `FUN_140240ea0` | `FUN_140240ea0(...)` | Interno | 1 | pendente |
| `FUN_140a66fb0` | `FUN_140a66fb0(...)` | Interno | 1 | pendente |
| `FUN_14165f2f0` | `FUN_14165f2f0(...)` | Interno | 1 | pendente |
| `FUN_14068eed0` | `FUN_14068eed0(...)` | Interno | 1 | pendente |
| `FUN_14068ecc0` | `FUN_14068ecc0(...)` | Interno | 1 | pendente |
| `FUN_1411e6bf0` | `FUN_1411e6bf0(...)` | Interno | 1 | pendente |
| `FUN_141884990` | `FUN_141884990(...)` | Interno | 1 | pendente |
| `FUN_141145eb0` | `FUN_141145eb0(...)` | Interno | 1 | pendente |
| `FUN_14032d480` | `FUN_14032d480(...)` | Interno | 1 | pendente |
| `FUN_1404a0520` | `FUN_1404a0520(...)` | Interno | 1 | pendente |
| `FUN_14101e470` | `FUN_14101e470(...)` | Interno | 1 | pendente |
| `FUN_14025f910` | `FUN_14025f910(...)` | Interno | 1 | pendente |
| `FUN_1417b1780` | `FUN_1417b1780(...)` | Interno | 1 | pendente |
| `FUN_141796da0` | `FUN_141796da0(...)` | Interno | 1 | pendente |
| `FUN_140a39a30` | `FUN_140a39a30(...)` | Interno | 1 | pendente |
| `FUN_140222b70` | `FUN_140222b70(...)` | Interno | 1 | pendente |
| `FUN_141266e00` | `FUN_141266e00(...)` | Interno | 1 | pendente |
| `FUN_14061ff30` | `FUN_14061ff30(...)` | Interno | 1 | pendente |
| `FUN_140ea0b90` | `FUN_140ea0b90(...)` | Interno | 1 | pendente |
| `FUN_140033910` | `FUN_140033910(...)` | Interno | 1 | pendente |
| `FUN_1400339f0` | `FUN_1400339f0(...)` | Interno | 1 | pendente |
| `FUN_141465a10` | `FUN_141465a10(...)` | Interno | 1 | pendente |
| `FUN_1409364d0` | `FUN_1409364d0(...)` | Interno | 1 | pendente |
| `FUN_140a31b80` | `FUN_140a31b80(...)` | Interno | 1 | pendente |
| `FUN_14041e980` | `FUN_14041e980(...)` | Interno | 1 | pendente |
| `FUN_140521340` | `FUN_140521340(...)` | Interno | 1 | pendente |
| `FUN_1416b1c00` | `FUN_1416b1c00(...)` | Interno | 1 | pendente |
| `FUN_1402b6170` | `FUN_1402b6170(...)` | Interno | 1 | pendente |
| `FUN_1402ed5e0` | `FUN_1402ed5e0(...)` | Interno | 1 | pendente |
| `FUN_1415a5060` | `FUN_1415a5060(...)` | Interno | 1 | pendente |
| `FUN_140ac8290` | `FUN_140ac8290(...)` | Interno | 1 | pendente |
| `FUN_1409bfe00` | `FUN_1409bfe00(...)` | Interno | 1 | pendente |
| `FUN_1409bfba0` | `FUN_1409bfba0(...)` | Interno | 1 | pendente |
| `FUN_1415b0f20` | `FUN_1415b0f20(...)` | Interno | 1 | pendente |
| `FUN_14011a000` | `FUN_14011a000(...)` | Interno | 1 | pendente |
| `FUN_140976c10` | `FUN_140976c10(...)` | Interno | 1 | pendente |
| `FUN_1410e5420` | `FUN_1410e5420(...)` | Interno | 1 | pendente |
| `FUN_141353ec0` | `FUN_141353ec0(...)` | Interno | 1 | pendente |
| `FUN_1406767e0` | `FUN_1406767e0(...)` | Interno | 1 | pendente |
| `FUN_140e815b0` | `FUN_140e815b0(...)` | Interno | 1 | pendente |
| `FUN_140eced90` | `FUN_140eced90(...)` | Interno | 1 | pendente |
| `FUN_140fe0e50` | `FUN_140fe0e50(...)` | Interno | 1 | pendente |
| `FUN_1416cfe50` | `FUN_1416cfe50(...)` | Interno | 1 | pendente |
| `FUN_140a98090` | `FUN_140a98090(...)` | Interno | 1 | pendente |
| `FUN_14045a2d0` | `FUN_14045a2d0(...)` | Interno | 1 | pendente |
| `FUN_1402b4b00` | `FUN_1402b4b00(...)` | Interno | 1 | pendente |
| `FUN_1402b47f0` | `FUN_1402b47f0(...)` | Interno | 1 | pendente |
| `FUN_140a6b9c0` | `FUN_140a6b9c0(...)` | Interno | 1 | pendente |
| `FUN_14134fde0` | `FUN_14134fde0(...)` | Interno | 1 | pendente |
| `FUN_140796640` | `FUN_140796640(...)` | Interno | 1 | pendente |
| `FUN_1406b2790` | `FUN_1406b2790(...)` | Interno | 1 | pendente |
| `FUN_14125f900` | `FUN_14125f900(...)` | Interno | 1 | pendente |
| `FUN_14146d080` | `FUN_14146d080(...)` | Interno | 1 | pendente |
| `FUN_14069b830` | `FUN_14069b830(...)` | Interno | 1 | pendente |
| `FUN_1404529e0` | `FUN_1404529e0(...)` | Interno | 1 | pendente |
| `FUN_140452ae0` | `FUN_140452ae0(...)` | Interno | 1 | pendente |
| `FUN_14171d550` | `FUN_14171d550(...)` | Interno | 1 | pendente |
| `FUN_1413f3bb0` | `FUN_1413f3bb0(...)` | Interno | 1 | pendente |
| `FUN_1400f20f0` | `FUN_1400f20f0(...)` | Interno | 1 | pendente |
| `FUN_14156dec0` | `FUN_14156dec0(...)` | Interno | 1 | pendente |
| `FUN_1401db250` | `FUN_1401db250(...)` | Interno | 1 | pendente |
| `FUN_1406c7e30` | `FUN_1406c7e30(...)` | Interno | 1 | pendente |
| `FUN_140f8acc0` | `FUN_140f8acc0(...)` | Interno | 1 | pendente |
| `FUN_140e43100` | `FUN_140e43100(...)` | Interno | 1 | pendente |
| `FUN_141343700` | `FUN_141343700(...)` | Interno | 1 | pendente |
| `FUN_141445bb0` | `FUN_141445bb0(...)` | Interno | 1 | pendente |
| `FUN_140975ba0` | `FUN_140975ba0(...)` | Interno | 1 | pendente |
| `FUN_1410454a0` | `FUN_1410454a0(...)` | Interno | 1 | pendente |
| `FUN_141644990` | `FUN_141644990(...)` | Interno | 1 | pendente |
| `FUN_1407d0850` | `FUN_1407d0850(...)` | Interno | 1 | pendente |
| `FUN_140048b90` | `FUN_140048b90(...)` | Interno | 1 | pendente |
| `FUN_141724cc0` | `FUN_141724cc0(...)` | Interno | 1 | pendente |
| `FUN_1401b4dc0` | `FUN_1401b4dc0(...)` | Interno | 1 | pendente |
| `FUN_1401b4e70` | `FUN_1401b4e70(...)` | Interno | 1 | pendente |
| `FUN_1415d14f0` | `FUN_1415d14f0(...)` | Interno | 1 | pendente |
| `FUN_140937c20` | `FUN_140937c20(...)` | Interno | 1 | pendente |
| `FUN_1403a4910` | `FUN_1403a4910(...)` | Interno | 1 | pendente |
| `FUN_140254750` | `FUN_140254750(...)` | Interno | 1 | pendente |
| `FUN_1402544c0` | `FUN_1402544c0(...)` | Interno | 1 | pendente |
| `FUN_140759830` | `FUN_140759830(...)` | Interno | 1 | pendente |
| `FUN_14061e7c0` | `FUN_14061e7c0(...)` | Interno | 1 | pendente |
| `FUN_140061140` | `FUN_140061140(...)` | Interno | 1 | pendente |
| `FUN_1415c5360` | `FUN_1415c5360(...)` | Interno | 1 | pendente |
| `FUN_140a1f2e0` | `FUN_140a1f2e0(...)` | Interno | 1 | pendente |
| `FUN_140a1edd0` | `FUN_140a1edd0(...)` | Interno | 1 | pendente |
| `FUN_14142de30` | `FUN_14142de30(...)` | Interno | 1 | pendente |
| `FUN_14142df00` | `FUN_14142df00(...)` | Interno | 1 | pendente |
| `FUN_14020b300` | `FUN_14020b300(...)` | Interno | 1 | pendente |
| `FUN_1402f0400` | `FUN_1402f0400(...)` | Interno | 1 | pendente |
| `FUN_1410d40b0` | `FUN_1410d40b0(...)` | Interno | 1 | pendente |
| `FUN_1409eb9b0` | `FUN_1409eb9b0(...)` | Interno | 1 | pendente |
| `FUN_1403d6970` | `FUN_1403d6970(...)` | Interno | 1 | pendente |
| `FUN_1403d6530` | `FUN_1403d6530(...)` | Interno | 1 | pendente |
| `FUN_14126e820` | `FUN_14126e820(...)` | Interno | 1 | pendente |
| `FUN_140edd970` | `FUN_140edd970(...)` | Interno | 1 | pendente |
| `FUN_1415df7f0` | `FUN_1415df7f0(...)` | Interno | 1 | pendente |
| `FUN_140872b80` | `FUN_140872b80(...)` | Interno | 1 | pendente |
| `FUN_140689b90` | `FUN_140689b90(...)` | Interno | 1 | pendente |
| `FUN_14004a4a0` | `FUN_14004a4a0(...)` | Interno | 1 | pendente |
| `FUN_1410ecdd0` | `FUN_1410ecdd0(...)` | Interno | 1 | pendente |
| `FUN_140399340` | `FUN_140399340(...)` | Interno | 1 | pendente |
| `FUN_1402ec9d0` | `FUN_1402ec9d0(...)` | Interno | 1 | pendente |
| `FUN_1407e5130` | `FUN_1407e5130(...)` | Interno | 1 | pendente |
| `FUN_1407e5190` | `FUN_1407e5190(...)` | Interno | 1 | pendente |
| `FUN_1411fe120` | `FUN_1411fe120(...)` | Interno | 1 | pendente |
| `FUN_14058b830` | `FUN_14058b830(...)` | Interno | 1 | pendente |
| `FUN_1404467f0` | `FUN_1404467f0(...)` | Interno | 1 | pendente |
| `FUN_141582510` | `FUN_141582510(...)` | Interno | 1 | pendente |
| `FUN_141460990` | `FUN_141460990(...)` | Interno | 1 | pendente |
| `FUN_1407fe4f0` | `FUN_1407fe4f0(...)` | Interno | 1 | pendente |
| `FUN_1407a9260` | `FUN_1407a9260(...)` | Interno | 1 | pendente |
| `FUN_141724410` | `FUN_141724410(...)` | Interno | 1 | pendente |
| `FUN_14025f030` | `FUN_14025f030(...)` | Interno | 1 | pendente |
| `FUN_1417cce10` | `FUN_1417cce10(...)` | Interno | 1 | pendente |
| `FUN_141197fa0` | `FUN_141197fa0(...)` | Interno | 1 | pendente |
| `FUN_14039ec90` | `FUN_14039ec90(...)` | Interno | 1 | pendente |
| `FUN_1404fc130` | `FUN_1404fc130(...)` | Interno | 1 | pendente |
| `FUN_1404fbd40` | `FUN_1404fbd40(...)` | Interno | 1 | pendente |
| `FUN_1417814a0` | `FUN_1417814a0(...)` | Interno | 1 | pendente |
| `FUN_140669660` | `FUN_140669660(...)` | Interno | 1 | pendente |
| `FUN_140966970` | `FUN_140966970(...)` | Interno | 1 | pendente |
| `FUN_14076b790` | `FUN_14076b790(...)` | Interno | 1 | pendente |
| `FUN_14076b5a0` | `FUN_14076b5a0(...)` | Interno | 1 | pendente |
| `FUN_1410d11b0` | `FUN_1410d11b0(...)` | Interno | 1 | pendente |
| `FUN_1405b01e0` | `FUN_1405b01e0(...)` | Interno | 1 | pendente |
| `FUN_14042fe90` | `FUN_14042fe90(...)` | Interno | 1 | pendente |
| `FUN_14085deb0` | `FUN_14085deb0(...)` | Interno | 1 | pendente |
| `FUN_140220920` | `FUN_140220920(...)` | Interno | 1 | pendente |
| `FUN_1405c5160` | `FUN_1405c5160(...)` | Interno | 1 | pendente |
| `FUN_140e71b10` | `FUN_140e71b10(...)` | Interno | 1 | pendente |
| `FUN_140e71ab0` | `FUN_140e71ab0(...)` | Interno | 1 | pendente |
| `FUN_140211560` | `FUN_140211560(...)` | Interno | 1 | pendente |
| `FUN_1402eeb30` | `FUN_1402eeb30(...)` | Interno | 1 | pendente |
| `FUN_1416251c0` | `FUN_1416251c0(...)` | Interno | 1 | pendente |
| `FUN_140f6aa80` | `FUN_140f6aa80(...)` | Interno | 1 | pendente |
| `FUN_1410c3b00` | `FUN_1410c3b00(...)` | Interno | 1 | pendente |
| `FUN_140f2eb40` | `FUN_140f2eb40(...)` | Interno | 1 | pendente |
| `FUN_14048a300` | `FUN_14048a300(...)` | Interno | 1 | pendente |
| `FUN_14010c7d0` | `FUN_14010c7d0(...)` | Interno | 1 | pendente |
| `FUN_14159dca0` | `FUN_14159dca0(...)` | Interno | 1 | pendente |
| `FUN_14159dd70` | `FUN_14159dd70(...)` | Interno | 1 | pendente |
| `FUN_140ab32f0` | `FUN_140ab32f0(...)` | Interno | 1 | pendente |
| `FUN_14007cd80` | `FUN_14007cd80(...)` | Interno | 1 | pendente |
| `FUN_1411d4680` | `FUN_1411d4680(...)` | Interno | 1 | pendente |
| `FUN_140a3ff60` | `FUN_140a3ff60(...)` | Interno | 1 | pendente |
| `FUN_141373170` | `FUN_141373170(...)` | Interno | 1 | pendente |
| `FUN_140a7c020` | `FUN_140a7c020(...)` | Interno | 1 | pendente |
| `FUN_1417b8ec0` | `FUN_1417b8ec0(...)` | Interno | 1 | pendente |
| `FUN_141494410` | `FUN_141494410(...)` | Interno | 1 | pendente |
| `FUN_1409028a0` | `FUN_1409028a0(...)` | Interno | 1 | pendente |
| `FUN_140f0f1d0` | `FUN_140f0f1d0(...)` | Interno | 1 | pendente |
| `FUN_140f0f2a0` | `FUN_140f0f2a0(...)` | Interno | 1 | pendente |
| `FUN_140f0f450` | `FUN_140f0f450(...)` | Interno | 1 | pendente |
| `FUN_140f0f520` | `FUN_140f0f520(...)` | Interno | 1 | pendente |
| `FUN_140f0f5f0` | `FUN_140f0f5f0(...)` | Interno | 1 | pendente |
| `FUN_141617b80` | `FUN_141617b80(...)` | Interno | 1 | pendente |
| `FUN_140ecfbe0` | `FUN_140ecfbe0(...)` | Interno | 1 | pendente |
| `FUN_14083b6c0` | `FUN_14083b6c0(...)` | Interno | 1 | pendente |
| `FUN_14083b3b0` | `FUN_14083b3b0(...)` | Interno | 1 | pendente |
| `FUN_1405941d0` | `FUN_1405941d0(...)` | Interno | 1 | pendente |
| `FUN_141797f20` | `FUN_141797f20(...)` | Interno | 1 | pendente |
| `FUN_141797e50` | `FUN_141797e50(...)` | Interno | 1 | pendente |
| `FUN_14176cd20` | `FUN_14176cd20(...)` | Interno | 1 | pendente |
| `FUN_14008e1e0` | `FUN_14008e1e0(...)` | Interno | 1 | pendente |
| `FUN_14175d450` | `FUN_14175d450(...)` | Interno | 1 | pendente |
| `FUN_1410bc950` | `FUN_1410bc950(...)` | Interno | 1 | pendente |
| `FUN_141075e60` | `FUN_141075e60(...)` | Interno | 1 | pendente |
| `FUN_140e9f8d0` | `FUN_140e9f8d0(...)` | Interno | 1 | pendente |
| `FUN_1405266b0` | `FUN_1405266b0(...)` | Interno | 1 | pendente |
| `FUN_1415c5680` | `FUN_1415c5680(...)` | Interno | 1 | pendente |
| `FUN_141665440` | `FUN_141665440(...)` | Interno | 1 | pendente |
| `FUN_141270160` | `FUN_141270160(...)` | Interno | 1 | pendente |
| `FUN_1412d8d80` | `FUN_1412d8d80(...)` | Interno | 1 | pendente |
| `FUN_1403d7800` | `FUN_1403d7800(...)` | Interno | 1 | pendente |
| `FUN_141132950` | `FUN_141132950(...)` | Interno | 1 | pendente |
| `FUN_141132c60` | `FUN_141132c60(...)` | Interno | 1 | pendente |
| `FUN_1404236f0` | `FUN_1404236f0(...)` | Interno | 1 | pendente |
| `FUN_140423480` | `FUN_140423480(...)` | Interno | 1 | pendente |
| `FUN_14152dd60` | `FUN_14152dd60(...)` | Interno | 1 | pendente |
| `FUN_14095e340` | `FUN_14095e340(...)` | Interno | 1 | pendente |
| `FUN_1410d9f90` | `FUN_1410d9f90(...)` | Interno | 1 | pendente |
| `FUN_1410c6330` | `FUN_1410c6330(...)` | Interno | 1 | pendente |
| `FUN_140ede9e0` | `FUN_140ede9e0(...)` | Interno | 1 | pendente |
| `FUN_140ede930` | `FUN_140ede930(...)` | Interno | 1 | pendente |
| `FUN_14169a1e0` | `FUN_14169a1e0(...)` | Interno | 1 | pendente |
| `FUN_140fa1170` | `FUN_140fa1170(...)` | Interno | 1 | pendente |
| `FUN_1402e1b60` | `FUN_1402e1b60(...)` | Interno | 1 | pendente |
| `FUN_140235400` | `FUN_140235400(...)` | Interno | 1 | pendente |
| `FUN_140e3a270` | `FUN_140e3a270(...)` | Interno | 1 | pendente |
| `FUN_141478ca0` | `FUN_141478ca0(...)` | Interno | 1 | pendente |
| `FUN_1412a97f0` | `FUN_1412a97f0(...)` | Interno | 1 | pendente |
| `FUN_140ac0130` | `FUN_140ac0130(...)` | Interno | 1 | pendente |
| `FUN_140fac700` | `FUN_140fac700(...)` | Interno | 1 | pendente |
| `FUN_140f2d750` | `FUN_140f2d750(...)` | Interno | 1 | pendente |
| `FUN_140ea2250` | `FUN_140ea2250(...)` | Interno | 1 | pendente |
| `FUN_14120cd30` | `FUN_14120cd30(...)` | Interno | 1 | pendente |
| `FUN_140abeea0` | `FUN_140abeea0(...)` | Interno | 1 | pendente |
| `FUN_140abefa0` | `FUN_140abefa0(...)` | Interno | 1 | pendente |
| `FUN_14171eb20` | `FUN_14171eb20(...)` | Interno | 1 | pendente |
| `FUN_141672930` | `FUN_141672930(...)` | Interno | 1 | pendente |
| `FUN_1409ba330` | `FUN_1409ba330(...)` | Interno | 1 | pendente |
| `FUN_140ae9de0` | `FUN_140ae9de0(...)` | Interno | 1 | pendente |
| `FUN_1401aa220` | `FUN_1401aa220(...)` | Interno | 1 | pendente |
| `FUN_1401aa320` | `FUN_1401aa320(...)` | Interno | 1 | pendente |
| `FUN_1401aa3f0` | `FUN_1401aa3f0(...)` | Interno | 1 | pendente |
| `FUN_14081b920` | `FUN_14081b920(...)` | Interno | 1 | pendente |
| `FUN_1400988e0` | `FUN_1400988e0(...)` | Interno | 1 | pendente |
| `FUN_140628880` | `FUN_140628880(...)` | Interno | 1 | pendente |
| `FUN_14086ecc0` | `FUN_14086ecc0(...)` | Interno | 1 | pendente |
| `FUN_140e6e7e0` | `FUN_140e6e7e0(...)` | Interno | 1 | pendente |
| `FUN_1400e91b0` | `FUN_1400e91b0(...)` | Interno | 1 | pendente |
| `FUN_1400e8f80` | `FUN_1400e8f80(...)` | Interno | 1 | pendente |
| `FUN_1407b6ac0` | `FUN_1407b6ac0(...)` | Interno | 1 | pendente |
| `FUN_1407b66e0` | `FUN_1407b66e0(...)` | Interno | 1 | pendente |
| `FUN_1409ddad0` | `FUN_1409ddad0(...)` | Interno | 1 | pendente |
| `FUN_1416be6e0` | `FUN_1416be6e0(...)` | Interno | 1 | pendente |
| `FUN_1410cbd80` | `FUN_1410cbd80(...)` | Interno | 1 | pendente |
| `FUN_140684840` | `FUN_140684840(...)` | Interno | 1 | pendente |
| `FUN_140684630` | `FUN_140684630(...)` | Interno | 1 | pendente |
| `FUN_1407f62b0` | `FUN_1407f62b0(...)` | Interno | 1 | pendente |
| `FUN_1416a4fe0` | `FUN_1416a4fe0(...)` | Interno | 1 | pendente |
| `FUN_140e926f0` | `FUN_140e926f0(...)` | Interno | 1 | pendente |
| `FUN_140870930` | `FUN_140870930(...)` | Interno | 1 | pendente |
| `FUN_140795720` | `FUN_140795720(...)` | Interno | 1 | pendente |
| `FUN_1410c7d90` | `FUN_1410c7d90(...)` | Interno | 1 | pendente |
| `FUN_1410c7cc0` | `FUN_1410c7cc0(...)` | Interno | 1 | pendente |
| `FUN_1402d5b40` | `FUN_1402d5b40(...)` | Interno | 1 | pendente |
| `FUN_140e39940` | `FUN_140e39940(...)` | Interno | 1 | pendente |
| `FUN_141041b10` | `FUN_141041b10(...)` | Interno | 1 | pendente |
| `FUN_141041be0` | `FUN_141041be0(...)` | Interno | 1 | pendente |
| `FUN_141041d10` | `FUN_141041d10(...)` | Interno | 1 | pendente |
| `FUN_1407a17f0` | `FUN_1407a17f0(...)` | Interno | 1 | pendente |
| `FUN_1414b8090` | `FUN_1414b8090(...)` | Interno | 1 | pendente |
| `FUN_1402e3000` | `FUN_1402e3000(...)` | Interno | 1 | pendente |
| `FUN_14164d080` | `FUN_14164d080(...)` | Interno | 1 | pendente |
| `FUN_14158de70` | `FUN_14158de70(...)` | Interno | 1 | pendente |
| `FUN_14158df40` | `FUN_14158df40(...)` | Interno | 1 | pendente |
| `FUN_1407d7930` | `FUN_1407d7930(...)` | Interno | 1 | pendente |
| `FUN_141437dd0` | `FUN_141437dd0(...)` | Interno | 1 | pendente |
| `FUN_14134a7f0` | `FUN_14134a7f0(...)` | Interno | 1 | pendente |
| `FUN_14110d620` | `FUN_14110d620(...)` | Interno | 1 | pendente |
| `FUN_14021b5a0` | `FUN_14021b5a0(...)` | Interno | 1 | pendente |
| `FUN_14011e170` | `FUN_14011e170(...)` | Interno | 1 | pendente |
| `FUN_140333970` | `FUN_140333970(...)` | Interno | 1 | pendente |
| `FUN_14089ebe0` | `FUN_14089ebe0(...)` | Interno | 1 | pendente |
| `FUN_14089e9a0` | `FUN_14089e9a0(...)` | Interno | 1 | pendente |
| `FUN_14075c0e0` | `FUN_14075c0e0(...)` | Interno | 1 | pendente |
| `FUN_1416395a0` | `FUN_1416395a0(...)` | Interno | 1 | pendente |
| `FUN_140f1b7f0` | `FUN_140f1b7f0(...)` | Interno | 1 | pendente |
| `FUN_1415c09b0` | `FUN_1415c09b0(...)` | Interno | 1 | pendente |
| `FUN_140238550` | `FUN_140238550(...)` | Interno | 1 | pendente |
| `FUN_14096e1a0` | `FUN_14096e1a0(...)` | Interno | 1 | pendente |
| `FUN_140850600` | `FUN_140850600(...)` | Interno | 1 | pendente |
| `FUN_1408501e0` | `FUN_1408501e0(...)` | Interno | 1 | pendente |
| `FUN_140690130` | `FUN_140690130(...)` | Interno | 1 | pendente |
| `FUN_1400c5520` | `FUN_1400c5520(...)` | Interno | 1 | pendente |
| `FUN_14017a770` | `FUN_14017a770(...)` | Interno | 1 | pendente |
| `FUN_140103600` | `FUN_140103600(...)` | Interno | 1 | pendente |
| `FUN_14002e560` | `FUN_14002e560(...)` | Interno | 1 | pendente |
| `FUN_14002e0b0` | `FUN_14002e0b0(...)` | Interno | 1 | pendente |
| `FUN_1412da350` | `FUN_1412da350(...)` | Interno | 1 | pendente |
| `FUN_140285980` | `FUN_140285980(...)` | Interno | 1 | pendente |
| `FUN_14124d030` | `FUN_14124d030(...)` | Interno | 1 | pendente |
| `FUN_140ac0b50` | `FUN_140ac0b50(...)` | Interno | 1 | pendente |
| `FUN_1408ab0f0` | `FUN_1408ab0f0(...)` | Interno | 1 | pendente |
| `FUN_14082e290` | `FUN_14082e290(...)` | Interno | 1 | pendente |
| `FUN_141746950` | `FUN_141746950(...)` | Interno | 1 | pendente |
| `FUN_1410883b0` | `FUN_1410883b0(...)` | Interno | 1 | pendente |
| `FUN_140368f80` | `FUN_140368f80(...)` | Interno | 1 | pendente |
| `FUN_140368d40` | `FUN_140368d40(...)` | Interno | 1 | pendente |
| `FUN_140499890` | `FUN_140499890(...)` | Interno | 1 | pendente |
| `FUN_141766f70` | `FUN_141766f70(...)` | Interno | 1 | pendente |
| `FUN_14136a990` | `FUN_14136a990(...)` | Interno | 1 | pendente |
| `FUN_1417b6a10` | `FUN_1417b6a10(...)` | Interno | 1 | pendente |
| `FUN_1407c3750` | `FUN_1407c3750(...)` | Interno | 1 | pendente |
| `FUN_140630870` | `FUN_140630870(...)` | Interno | 1 | pendente |
| `FUN_14043a1d0` | `FUN_14043a1d0(...)` | Interno | 1 | pendente |
| `FUN_1416c0520` | `FUN_1416c0520(...)` | Interno | 1 | pendente |
| `FUN_140824d20` | `FUN_140824d20(...)` | Interno | 1 | pendente |
| `FUN_14092f4a0` | `FUN_14092f4a0(...)` | Interno | 1 | pendente |
| `FUN_14092ef30` | `FUN_14092ef30(...)` | Interno | 1 | pendente |
| `FUN_1416fbeb0` | `FUN_1416fbeb0(...)` | Interno | 1 | pendente |
| `FUN_1407698d0` | `FUN_1407698d0(...)` | Interno | 1 | pendente |
| `FUN_1402821f0` | `FUN_1402821f0(...)` | Interno | 1 | pendente |
| `FUN_14029f8d0` | `FUN_14029f8d0(...)` | Interno | 1 | pendente |
| `FUN_1403f5af0` | `FUN_1403f5af0(...)` | Interno | 1 | pendente |
| `FUN_1410923b0` | `FUN_1410923b0(...)` | Interno | 1 | pendente |
| `FUN_1402c2130` | `FUN_1402c2130(...)` | Interno | 1 | pendente |
| `FUN_140892cc0` | `FUN_140892cc0(...)` | Interno | 1 | pendente |
| `FUN_140fb01b0` | `FUN_140fb01b0(...)` | Interno | 1 | pendente |
| `FUN_140fb0280` | `FUN_140fb0280(...)` | Interno | 1 | pendente |
| `FUN_140fb0350` | `FUN_140fb0350(...)` | Interno | 1 | pendente |
| `FUN_141392b20` | `FUN_141392b20(...)` | Interno | 1 | pendente |
| `FUN_14083d520` | `FUN_14083d520(...)` | Interno | 1 | pendente |
| `FUN_1410bdeb0` | `FUN_1410bdeb0(...)` | Interno | 1 | pendente |
| `FUN_141745db0` | `FUN_141745db0(...)` | Interno | 1 | pendente |
| `FUN_140624940` | `FUN_140624940(...)` | Interno | 1 | pendente |
| `FUN_141468b40` | `FUN_141468b40(...)` | Interno | 1 | pendente |
| `FUN_1400551c0` | `FUN_1400551c0(...)` | Interno | 1 | pendente |
| `FUN_1400552c0` | `FUN_1400552c0(...)` | Interno | 1 | pendente |
| `FUN_1400553a0` | `FUN_1400553a0(...)` | Interno | 1 | pendente |
| `FUN_140237610` | `FUN_140237610(...)` | Interno | 1 | pendente |
| `FUN_1402373d0` | `FUN_1402373d0(...)` | Interno | 1 | pendente |
| `FUN_1402841a0` | `FUN_1402841a0(...)` | Interno | 1 | pendente |
| `FUN_140283d30` | `FUN_140283d30(...)` | Interno | 1 | pendente |
| `FUN_140a00ec0` | `FUN_140a00ec0(...)` | Interno | 1 | pendente |
| `FUN_1416677a0` | `FUN_1416677a0(...)` | Interno | 1 | pendente |
| `FUN_140391f40` | `FUN_140391f40(...)` | Interno | 1 | pendente |
| `FUN_140493300` | `FUN_140493300(...)` | Interno | 1 | pendente |
| `FUN_14128bd30` | `FUN_14128bd30(...)` | Interno | 1 | pendente |
| `FUN_14025f8b0` | `FUN_14025f8b0(...)` | Interno | 1 | pendente |
| `FUN_140f438c0` | `FUN_140f438c0(...)` | Interno | 1 | pendente |
| `FUN_140ec6fc0` | `FUN_140ec6fc0(...)` | Interno | 1 | pendente |
| `FUN_14173b000` | `FUN_14173b000(...)` | Interno | 1 | pendente |
| `FUN_140903df0` | `FUN_140903df0(...)` | Interno | 1 | pendente |
| `FUN_1400b9c00` | `FUN_1400b9c00(...)` | Interno | 1 | pendente |
| `FUN_14089a140` | `FUN_14089a140(...)` | Interno | 1 | pendente |
| `FUN_1400459d0` | `FUN_1400459d0(...)` | Interno | 1 | pendente |
| `FUN_1408da9b0` | `FUN_1408da9b0(...)` | Interno | 1 | pendente |
| `FUN_1408da520` | `FUN_1408da520(...)` | Interno | 1 | pendente |
| `FUN_1417f08c0` | `FUN_1417f08c0(...)` | Interno | 1 | pendente |
| `FUN_14061d5f0` | `FUN_14061d5f0(...)` | Interno | 1 | pendente |
| `FUN_140654a50` | `FUN_140654a50(...)` | Interno | 1 | pendente |
| `FUN_141245390` | `FUN_141245390(...)` | Interno | 1 | pendente |
| `FUN_140591f90` | `FUN_140591f90(...)` | Interno | 1 | pendente |
| `FUN_1406230f0` | `FUN_1406230f0(...)` | Interno | 1 | pendente |
| `FUN_1401915e0` | `FUN_1401915e0(...)` | Interno | 1 | pendente |
| `FUN_1404e27a0` | `FUN_1404e27a0(...)` | Interno | 1 | pendente |
| `FUN_140064af0` | `FUN_140064af0(...)` | Interno | 1 | pendente |
| `FUN_14181ca20` | `FUN_14181ca20(...)` | Interno | 1 | pendente |
| `FUN_140ea7980` | `FUN_140ea7980(...)` | Interno | 1 | pendente |
| `FUN_1404d3ac0` | `FUN_1404d3ac0(...)` | Interno | 1 | pendente |
| `FUN_140eab910` | `FUN_140eab910(...)` | Interno | 1 | pendente |
| `FUN_14126a8c0` | `FUN_14126a8c0(...)` | Interno | 1 | pendente |
| `FUN_1412437d0` | `FUN_1412437d0(...)` | Interno | 1 | pendente |
| `FUN_140890720` | `FUN_140890720(...)` | Interno | 1 | pendente |
| `FUN_140689650` | `FUN_140689650(...)` | Interno | 1 | pendente |
| `FUN_1415feb50` | `FUN_1415feb50(...)` | Interno | 1 | pendente |
| `FUN_140ec6510` | `FUN_140ec6510(...)` | Interno | 1 | pendente |
| `FUN_1401044a0` | `FUN_1401044a0(...)` | Interno | 1 | pendente |
| `FUN_14031d8e0` | `FUN_14031d8e0(...)` | Interno | 1 | pendente |
| `FUN_14031d380` | `FUN_14031d380(...)` | Interno | 1 | pendente |
| `FUN_1400d3980` | `FUN_1400d3980(...)` | Interno | 1 | pendente |
| `FUN_14042fd60` | `FUN_14042fd60(...)` | Interno | 1 | pendente |
| `FUN_140a60430` | `FUN_140a60430(...)` | Interno | 1 | pendente |
| `FUN_140a601f0` | `FUN_140a601f0(...)` | Interno | 1 | pendente |
| `FUN_1410eb820` | `FUN_1410eb820(...)` | Interno | 1 | pendente |
| `FUN_1410eb750` | `FUN_1410eb750(...)` | Interno | 1 | pendente |
| `FUN_1410fb020` | `FUN_1410fb020(...)` | Interno | 1 | pendente |
| `FUN_140179440` | `FUN_140179440(...)` | Interno | 1 | pendente |
| `FUN_1415a5590` | `FUN_1415a5590(...)` | Interno | 1 | pendente |
| `FUN_140828100` | `FUN_140828100(...)` | Interno | 1 | pendente |
| `FUN_140fb9160` | `FUN_140fb9160(...)` | Interno | 1 | pendente |
| `FUN_14069a380` | `FUN_14069a380(...)` | Interno | 1 | pendente |
| `FUN_140fdcbf0` | `FUN_140fdcbf0(...)` | Interno | 1 | pendente |
| `FUN_1403d8800` | `FUN_1403d8800(...)` | Interno | 1 | pendente |
| `FUN_1408b5780` | `FUN_1408b5780(...)` | Interno | 1 | pendente |
| `FUN_1405196f0` | `FUN_1405196f0(...)` | Interno | 1 | pendente |
| `FUN_140047f10` | `FUN_140047f10(...)` | Interno | 1 | pendente |
| `FUN_1408822a0` | `FUN_1408822a0(...)` | Interno | 1 | pendente |
| `FUN_1407a8780` | `FUN_1407a8780(...)` | Interno | 1 | pendente |
| `FUN_1407a8570` | `FUN_1407a8570(...)` | Interno | 1 | pendente |
| `FUN_14158a880` | `FUN_14158a880(...)` | Interno | 1 | pendente |
| `FUN_1414b41b0` | `FUN_1414b41b0(...)` | Interno | 1 | pendente |
| `FUN_140081230` | `FUN_140081230(...)` | Interno | 1 | pendente |
| `FUN_141532630` | `FUN_141532630(...)` | Interno | 1 | pendente |
| `FUN_14025daf0` | `FUN_14025daf0(...)` | Interno | 1 | pendente |
| `FUN_141703d00` | `FUN_141703d00(...)` | Interno | 1 | pendente |
| `FUN_1408873b0` | `FUN_1408873b0(...)` | Interno | 1 | pendente |
| `FUN_1400eab30` | `FUN_1400eab30(...)` | Interno | 1 | pendente |
| `FUN_140286dd0` | `FUN_140286dd0(...)` | Interno | 1 | pendente |
| `FUN_140286bc0` | `FUN_140286bc0(...)` | Interno | 1 | pendente |
| `FUN_14169faf0` | `FUN_14169faf0(...)` | Interno | 1 | pendente |
| `FUN_140783d90` | `FUN_140783d90(...)` | Interno | 1 | pendente |
| `FUN_141365fe0` | `FUN_141365fe0(...)` | Interno | 1 | pendente |
| `FUN_140a58850` | `FUN_140a58850(...)` | Interno | 1 | pendente |
| `FUN_140a58620` | `FUN_140a58620(...)` | Interno | 1 | pendente |
| `FUN_1410429d0` | `FUN_1410429d0(...)` | Interno | 1 | pendente |
| `FUN_14012a750` | `FUN_14012a750(...)` | Interno | 1 | pendente |
| `FUN_1401bd260` | `FUN_1401bd260(...)` | Interno | 1 | pendente |
| `FUN_140a5e040` | `FUN_140a5e040(...)` | Interno | 1 | pendente |
| `FUN_1403562e0` | `FUN_1403562e0(...)` | Interno | 1 | pendente |
| `FUN_1400772a0` | `FUN_1400772a0(...)` | Interno | 1 | pendente |
| `FUN_140076e80` | `FUN_140076e80(...)` | Interno | 1 | pendente |
| `FUN_1405de8a0` | `FUN_1405de8a0(...)` | Interno | 1 | pendente |
| `FUN_1405de540` | `FUN_1405de540(...)` | Interno | 1 | pendente |
| `FUN_1418a4360` | `FUN_1418a4360(...)` | Interno | 1 | pendente |
| `FUN_140ab1b80` | `FUN_140ab1b80(...)` | Interno | 1 | pendente |
| `FUN_1414cc320` | `FUN_1414cc320(...)` | Interno | 1 | pendente |
| `FUN_1415dd590` | `FUN_1415dd590(...)` | Interno | 1 | pendente |
| `FUN_1408a9250` | `FUN_1408a9250(...)` | Interno | 1 | pendente |
| `FUN_1406428f0` | `FUN_1406428f0(...)` | Interno | 1 | pendente |
| `FUN_14053af90` | `FUN_14053af90(...)` | Interno | 1 | pendente |
| `FUN_1400c44d0` | `FUN_1400c44d0(...)` | Interno | 1 | pendente |
| `FUN_1400c4200` | `FUN_1400c4200(...)` | Interno | 1 | pendente |
| `FUN_1413e5d10` | `FUN_1413e5d10(...)` | Interno | 1 | pendente |
| `FUN_140777e60` | `FUN_140777e60(...)` | Interno | 1 | pendente |
| `FUN_140edc4f0` | `FUN_140edc4f0(...)` | Interno | 1 | pendente |
| `FUN_1404def30` | `FUN_1404def30(...)` | Interno | 1 | pendente |
| `FUN_140a36bb0` | `FUN_140a36bb0(...)` | Interno | 1 | pendente |
| `FUN_1414693a0` | `FUN_1414693a0(...)` | Interno | 1 | pendente |
| `FUN_141164bd0` | `FUN_141164bd0(...)` | Interno | 1 | pendente |
| `FUN_141164c80` | `FUN_141164c80(...)` | Interno | 1 | pendente |
| `FUN_141328ab0` | `FUN_141328ab0(...)` | Interno | 1 | pendente |
| `FUN_140972b00` | `FUN_140972b00(...)` | Interno | 1 | pendente |
| `FUN_1402d6f60` | `FUN_1402d6f60(...)` | Interno | 1 | pendente |
| `FUN_141744a40` | `FUN_141744a40(...)` | Interno | 1 | pendente |
| `FUN_14026d700` | `FUN_14026d700(...)` | Interno | 1 | pendente |
| `FUN_141661860` | `FUN_141661860(...)` | Interno | 1 | pendente |
| `FUN_140ae0bc0` | `FUN_140ae0bc0(...)` | Interno | 1 | pendente |
| `FUN_1400c6500` | `FUN_1400c6500(...)` | Interno | 1 | pendente |
| `FUN_141535610` | `FUN_141535610(...)` | Interno | 1 | pendente |
| `FUN_1406b3d40` | `FUN_1406b3d40(...)` | Interno | 1 | pendente |
| `FUN_140fc8f70` | `FUN_140fc8f70(...)` | Interno | 1 | pendente |
| `FUN_141616c70` | `FUN_141616c70(...)` | Interno | 1 | pendente |
| `FUN_141590340` | `FUN_141590340(...)` | Interno | 1 | pendente |
| `FUN_14158f2e0` | `FUN_14158f2e0(...)` | Interno | 1 | pendente |
| `FUN_14109bb00` | `FUN_14109bb00(...)` | Interno | 1 | pendente |
| `FUN_140ae5dc0` | `FUN_140ae5dc0(...)` | Interno | 1 | pendente |
| `FUN_140812da0` | `FUN_140812da0(...)` | Interno | 1 | pendente |
| `FUN_141108f00` | `FUN_141108f00(...)` | Interno | 1 | pendente |
| `FUN_14158bc50` | `FUN_14158bc50(...)` | Interno | 1 | pendente |
| `FUN_1409a9c60` | `FUN_1409a9c60(...)` | Interno | 1 | pendente |
| `FUN_1409a97d0` | `FUN_1409a97d0(...)` | Interno | 1 | pendente |
| `FUN_141719050` | `FUN_141719050(...)` | Interno | 1 | pendente |
| `FUN_140082530` | `FUN_140082530(...)` | Interno | 1 | pendente |
| `FUN_14069c600` | `FUN_14069c600(...)` | Interno | 1 | pendente |
| `FUN_14012bd80` | `FUN_14012bd80(...)` | Interno | 1 | pendente |
| `FUN_140352e60` | `FUN_140352e60(...)` | Interno | 1 | pendente |
| `FUN_140301e40` | `FUN_140301e40(...)` | Interno | 1 | pendente |
| `FUN_1407d7a60` | `FUN_1407d7a60(...)` | Interno | 1 | pendente |
| `FUN_141225d60` | `FUN_141225d60(...)` | Interno | 1 | pendente |
| `FUN_14169d610` | `FUN_14169d610(...)` | Interno | 1 | pendente |
| `FUN_1416bf620` | `FUN_1416bf620(...)` | Interno | 1 | pendente |
| `FUN_140322f10` | `FUN_140322f10(...)` | Interno | 1 | pendente |
| `FUN_1404d1f90` | `FUN_1404d1f90(...)` | Interno | 1 | pendente |
| `FUN_14127c2b0` | `FUN_14127c2b0(...)` | Interno | 1 | pendente |
| `FUN_140960290` | `FUN_140960290(...)` | Interno | 1 | pendente |
| `FUN_141191fe0` | `FUN_141191fe0(...)` | Interno | 1 | pendente |
| `FUN_140adf010` | `FUN_140adf010(...)` | Interno | 1 | pendente |
| `FUN_140adedd0` | `FUN_140adedd0(...)` | Interno | 1 | pendente |
| `FUN_140685050` | `FUN_140685050(...)` | Interno | 1 | pendente |
| `FUN_1404d2720` | `FUN_1404d2720(...)` | Interno | 1 | pendente |
| `FUN_1403cfca0` | `FUN_1403cfca0(...)` | Interno | 1 | pendente |
| `FUN_141033ad0` | `FUN_141033ad0(...)` | Interno | 1 | pendente |
| `FUN_141033a20` | `FUN_141033a20(...)` | Interno | 1 | pendente |
| `FUN_1414bfab0` | `FUN_1414bfab0(...)` | Interno | 1 | pendente |
| `FUN_1400e6780` | `FUN_1400e6780(...)` | Interno | 1 | pendente |
| `FUN_1401ff770` | `FUN_1401ff770(...)` | Interno | 1 | pendente |
| `FUN_1400d6480` | `FUN_1400d6480(...)` | Interno | 1 | pendente |
| `FUN_1408a2300` | `FUN_1408a2300(...)` | Interno | 1 | pendente |
| `FUN_1412d94e0` | `FUN_1412d94e0(...)` | Interno | 1 | pendente |
| `FUN_14076a850` | `FUN_14076a850(...)` | Interno | 1 | pendente |
| `FUN_1402c8c00` | `FUN_1402c8c00(...)` | Interno | 1 | pendente |
| `FUN_1403985c0` | `FUN_1403985c0(...)` | Interno | 1 | pendente |
| `FUN_140046eb0` | `FUN_140046eb0(...)` | Interno | 1 | pendente |
| `FUN_1417e3140` | `FUN_1417e3140(...)` | Interno | 1 | pendente |
| `FUN_1417e32d0` | `FUN_1417e32d0(...)` | Interno | 1 | pendente |
| `FUN_140b06450` | `FUN_140b06450(...)` | Interno | 1 | pendente |
| `FUN_140865f60` | `FUN_140865f60(...)` | Interno | 1 | pendente |
| `FUN_140911650` | `FUN_140911650(...)` | Interno | 1 | pendente |
| `FUN_140911420` | `FUN_140911420(...)` | Interno | 1 | pendente |
| `FUN_14028fa30` | `FUN_14028fa30(...)` | Interno | 1 | pendente |
| `FUN_14107ce10` | `FUN_14107ce10(...)` | Interno | 1 | pendente |
| `FUN_1409cec30` | `FUN_1409cec30(...)` | Interno | 1 | pendente |
| `FUN_1400e46d0` | `FUN_1400e46d0(...)` | Interno | 1 | pendente |
| `FUN_140fa32b0` | `FUN_140fa32b0(...)` | Interno | 1 | pendente |
| `FUN_14036f2c0` | `FUN_14036f2c0(...)` | Interno | 1 | pendente |
| `FUN_1413ff980` | `FUN_1413ff980(...)` | Interno | 1 | pendente |
| `FUN_140f25c10` | `FUN_140f25c10(...)` | Interno | 1 | pendente |
| `FUN_140197300` | `FUN_140197300(...)` | Interno | 1 | pendente |
| `FUN_140231e50` | `FUN_140231e50(...)` | Interno | 1 | pendente |
| `FUN_1407eae90` | `FUN_1407eae90(...)` | Interno | 1 | pendente |
| `FUN_1404963c0` | `FUN_1404963c0(...)` | Interno | 1 | pendente |
| `FUN_140e3aba0` | `FUN_140e3aba0(...)` | Interno | 1 | pendente |
| `FUN_1410c91c0` | `FUN_1410c91c0(...)` | Interno | 1 | pendente |
| `FUN_1404dd9d0` | `FUN_1404dd9d0(...)` | Interno | 1 | pendente |
| `FUN_1408883f0` | `FUN_1408883f0(...)` | Interno | 1 | pendente |

## Instâncias e Salas

Total de funções: **21** | Total de invocações: **8,230**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `Variable_Lookup` | `RValue* Variable_Lookup(CInstance* inst, int var_id, ...)` | Core / Runtime | 5197 | pendente |
| `Variable_GetGlobal` | `RValue* Variable_GetGlobal(int var_id)` | Core / Runtime | 2023 | pendente |
| `Variable_Global_Set` | `void Variable_Global_Set(int var_id, RValue* val)` | Core / Runtime | 341 | pendente |
| `instance_exists` | `bool instance_exists(int id)` | Core / Runtime | 307 | pendente |
| `instance_find` | `instance_find(... [2 args])` | Builtin GM | 266 | pendente |
| `instance_create_depth` | `instance_create_depth(... [4 args])` | Builtin GM | 40 | pendente |
| `place_meeting` | `place_meeting(... [3 args])` | Builtin GM | 12 | pendente |
| `collision_line` | `collision_line(... [7 args])` | Builtin GM | 7 | pendente |
| `instance_place` | `instance_place(... [3 args])` | Builtin GM | 7 | pendente |
| `room_get_name` | `room_get_name(... [1 args])` | Builtin GM | 6 | pendente |
| `collision_rectangle_list` | `collision_rectangle_list(... [9 args])` | Builtin GM | 5 | pendente |
| `instance_deactivate_object` | `instance_deactivate_object(... [1 args])` | Builtin GM | 3 | pendente |
| `collision_point` | `collision_point(... [5 args])` | Builtin GM | 3 | pendente |
| `collision_rectangle` | `collision_rectangle(... [7 args])` | Builtin GM | 3 | pendente |
| `instance_create_layer` | `instance_create_layer(... [4 args])` | Builtin GM | 2 | pendente |
| `instance_place_list` | `instance_place_list(... [5 args])` | Builtin GM | 2 | pendente |
| `object_get_name` | `object_get_name(... [1 args])` | Builtin GM | 2 | pendente |
| `collision_circle` | `collision_circle(... [6 args])` | Builtin GM | 1 | pendente |
| `room_goto` | `room_goto(... [1 args])` | Builtin GM | 1 | pendente |
| `collision_line_list` | `collision_line_list(... [9 args])` | Builtin GM | 1 | pendente |
| `object_get_sprite` | `object_get_sprite(... [1 args])` | Builtin GM | 1 | pendente |

## Geral

Total de funções: **30** | Total de invocações: **3,559**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `unknown_builtin_DAT_1424a5be8` | `unknown_builtin_DAT_1424a5be8(... [? args])` | Builtin GM | 2407 | pendente |
| `unknown_builtin_DAT_1424a5bd8` | `unknown_builtin_DAT_1424a5bd8(... [? args])` | Builtin GM | 658 | pendente |
| `unknown_builtin_DAT_1424a5d48` | `unknown_builtin_DAT_1424a5d48(... [? args])` | Builtin GM | 75 | pendente |
| `unknown_builtin_DAT_1424a5e08` | `unknown_builtin_DAT_1424a5e08(... [? args])` | Builtin GM | 65 | pendente |
| `unknown_builtin_DAT_1424a6078` | `unknown_builtin_DAT_1424a6078(... [? args])` | Builtin GM | 52 | pendente |
| `unknown_builtin_DAT_1424a6068` | `unknown_builtin_DAT_1424a6068(... [? args])` | Builtin GM | 42 | pendente |
| `unknown_builtin_DAT_1424a5f98` | `unknown_builtin_DAT_1424a5f98(... [? args])` | Builtin GM | 41 | pendente |
| `unknown_builtin_DAT_1424a5f88` | `unknown_builtin_DAT_1424a5f88(... [? args])` | Builtin GM | 38 | pendente |
| `unknown_builtin_DAT_1424a5c78` | `unknown_builtin_DAT_1424a5c78(... [? args])` | Builtin GM | 30 | pendente |
| `unknown_builtin_DAT_1424a6098` | `unknown_builtin_DAT_1424a6098(... [? args])` | Builtin GM | 28 | pendente |
| `unknown_builtin_DAT_1424a5d28` | `unknown_builtin_DAT_1424a5d28(... [? args])` | Builtin GM | 22 | pendente |
| `unknown_builtin_DAT_1424a5f48` | `unknown_builtin_DAT_1424a5f48(... [? args])` | Builtin GM | 17 | pendente |
| `unknown_builtin_DAT_1424a6018` | `unknown_builtin_DAT_1424a6018(... [? args])` | Builtin GM | 13 | pendente |
| `unknown_builtin_DAT_1424a6008` | `unknown_builtin_DAT_1424a6008(... [? args])` | Builtin GM | 10 | pendente |
| `ord` | `ord(... [? args])` | Builtin GM | 9 | pendente |
| `unknown_builtin_DAT_1424a5e48` | `unknown_builtin_DAT_1424a5e48(... [? args])` | Builtin GM | 8 | pendente |
| `unknown_builtin_DAT_1424a6128` | `unknown_builtin_DAT_1424a6128(... [? args])` | Builtin GM | 6 | pendente |
| `unknown_builtin_DAT_1424a60f8` | `unknown_builtin_DAT_1424a60f8(... [? args])` | Builtin GM | 6 | pendente |
| `unknown_builtin_DAT_1424a5f58` | `unknown_builtin_DAT_1424a5f58(... [? args])` | Builtin GM | 5 | pendente |
| `unknown_builtin_DAT_1424a5c68` | `unknown_builtin_DAT_1424a5c68(... [? args])` | Builtin GM | 5 | pendente |
| `chr` | `chr(... [? args])` | Builtin GM | 5 | pendente |
| `unknown_builtin_DAT_1424a5ca8` | `unknown_builtin_DAT_1424a5ca8(... [? args])` | Builtin GM | 4 | pendente |
| `unknown_builtin_DAT_1424a5ff8` | `unknown_builtin_DAT_1424a5ff8(... [? args])` | Builtin GM | 4 | pendente |
| `unknown_builtin_DAT_1424a5e38` | `unknown_builtin_DAT_1424a5e38(... [? args])` | Builtin GM | 2 | pendente |
| `unknown_builtin_DAT_1424a5d88` | `unknown_builtin_DAT_1424a5d88(... [? args])` | Builtin GM | 2 | pendente |
| `unknown_builtin_DAT_1424a5fd8` | `unknown_builtin_DAT_1424a5fd8(... [? args])` | Builtin GM | 1 | pendente |
| `logn` | `logn(... [? args])` | Builtin GM | 1 | pendente |
| `unknown_builtin_DAT_1424a5c98` | `unknown_builtin_DAT_1424a5c98(... [? args])` | Builtin GM | 1 | pendente |
| `unknown_builtin_DAT_1424a5c88` | `unknown_builtin_DAT_1424a5c88(... [? args])` | Builtin GM | 1 | pendente |
| `ln` | `ln(... [? args])` | Builtin GM | 1 | pendente |

## Matemática e Strings

Total de funções: **25** | Total de invocações: **2,024**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `String_Create` | `void String_Create(RValue* dst, const char* s)` | Core / Runtime | 593 | pendente |
| `random_range` | `double random_range(double min, double max)` | Core / Runtime | 421 | pendente |
| `sin` | `sin(... [? args])` | Builtin GM | 348 | pendente |
| `cos` | `double cos(double a)` | Core / Runtime | 199 | pendente |
| `sqrt` | `sqrt(... [? args])` | Builtin GM | 193 | pendente |
| `point_direction / atan2` | `double point_direction(double x1, double y1, double x2, double y2)` | Core / Runtime | 178 | pendente |
| `point_distance` | `point_distance(... [4 args])` | Builtin GM | 18 | pendente |
| `string_length` | `string_length(... [1 args])` | Builtin GM | 12 | pendente |
| `round` | `round(... [1 args])` | Builtin GM | 11 | pendente |
| `string_char_at` | `string_char_at(... [2 args])` | Builtin GM | 10 | pendente |
| `ceil` | `ceil(... [? args])` | Builtin GM | 6 | pendente |
| `string_delete` | `string_delete(... [3 args])` | Builtin GM | 6 | pendente |
| `string_replace_all` | `string_replace_all(... [3 args])` | Builtin GM | 5 | pendente |
| `string_copy` | `string_copy(... [3 args])` | Builtin GM | 3 | pendente |
| `point_in_rectangle` | `point_in_rectangle(... [6 args])` | Builtin GM | 3 | pendente |
| `string_byte_length` | `string_byte_length(... [1 args])` | Builtin GM | 3 | pendente |
| `string_upper` | `string_upper(... [1 args])` | Builtin GM | 3 | pendente |
| `lerp` | `lerp(... [? args])` | Builtin GM | 2 | pendente |
| `angle_difference` | `angle_difference(... [1 args])` | Builtin GM | 2 | pendente |
| `string_pos_ext` | `string_pos_ext(... [3 args])` | Builtin GM | 2 | pendente |
| `string_lower` | `string_lower(... [1 args])` | Builtin GM | 2 | pendente |
| `string_digits` | `string_digits(... [1 args])` | Builtin GM | 1 | pendente |
| `string_ord_at` | `string_ord_at(... [2 args])` | Builtin GM | 1 | pendente |
| `dot_product_3d` | `dot_product_3d(... [6 args])` | Builtin GM | 1 | pendente |
| `power` | `power(... [2 args])` | Builtin GM | 1 | pendente |

## Sistema

Total de funções: **6** | Total de invocações: **1,604**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `YYError` | `void YYError(const char* fmt, ...)` | Core / Runtime | 1591 | pendente |
| `path_start` | `path_start(... [4 args])` | Builtin GM | 5 | pendente |
| `asset_get_index` | `asset_get_index(... [1 args])` | Builtin GM | 3 | pendente |
| `path_end` | `path_end(... [0 args])` | Builtin GM | 2 | pendente |
| `show_debug_overlay` | `show_debug_overlay(... [1 args])` | Builtin GM | 2 | pendente |
| `asset_get_tags` | `asset_get_tags(... [1 args])` | Builtin GM | 1 | pendente |

## Gráficos

Total de funções: **92** | Total de invocações: **754**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `draw_sprite_ext` | `void draw_sprite_ext(int spr, float subimg, double x, double y, ...)` | Core / Runtime | 189 | pendente |
| `shader_get_uniform` | `shader_get_uniform(... [2 args])` | Builtin GM | 90 | pendente |
| `draw_text` | `draw_text(... [3 args])` | Builtin GM | 40 | pendente |
| `gpu_set_blendmode` | `gpu_set_blendmode(... [1 args])` | Builtin GM | 39 | pendente |
| `sprite_get_texture` | `sprite_get_texture(... [2 args])` | Builtin GM | 18 | pendente |
| `surface_exists` | `surface_exists(... [1 args])` | Builtin GM | 18 | pendente |
| `draw_sprite_stretched` | `draw_sprite_stretched(... [6 args])` | Builtin GM | 18 | pendente |
| `draw_text_color` | `draw_text_color(... [8 args])` | Builtin GM | 16 | pendente |
| `shader_get_sampler_index` | `shader_get_sampler_index(... [2 args])` | Builtin GM | 15 | pendente |
| `texture_set_stage` | `texture_set_stage(... [2 args])` | Builtin GM | 14 | pendente |
| `surface_free` | `surface_free(... [1 args])` | Builtin GM | 14 | pendente |
| `draw_vertex_texture_color` | `draw_vertex_texture_color(... [6 args])` | Builtin GM | 12 | pendente |
| `draw_sprite_stretched_ext` | `draw_sprite_stretched_ext(... [8 args])` | Builtin GM | 12 | pendente |
| `sprite_get_number` | `sprite_get_number(... [1 args])` | Builtin GM | 11 | pendente |
| `sprite_get_uvs` | `sprite_get_uvs(... [2 args])` | Builtin GM | 10 | pendente |
| `surface_create` | `surface_create(... [2 args])` | Builtin GM | 10 | pendente |
| `sprite_get_width` | `sprite_get_width(... [1 args])` | Builtin GM | 9 | pendente |
| `texture_get_texel_width` | `texture_get_texel_width(... [1 args])` | Builtin GM | 8 | pendente |
| `texture_get_texel_height` | `texture_get_texel_height(... [1 args])` | Builtin GM | 8 | pendente |
| `draw_vertex_color` | `draw_vertex_color(... [4 args])` | Builtin GM | 8 | pendente |
| `merge_color` | `merge_color(... [3 args])` | Builtin GM | 7 | pendente |
| `sprite_get_height` | `sprite_get_height(... [1 args])` | Builtin GM | 7 | pendente |
| `draw_line` | `draw_line(... [4 args])` | Builtin GM | 7 | pendente |
| `draw_primitive_end` | `draw_primitive_end(... [0 args])` | Builtin GM | 6 | pendente |
| `matrix_set` | `matrix_set(... [2 args])` | Builtin GM | 6 | pendente |
| `draw_circle_color` | `draw_circle_color(... [6 args])` | Builtin GM | 5 | pendente |
| `gpu_set_colorwriteenable` | `gpu_set_colorwriteenable(... [0xffffffff args])` | Builtin GM | 5 | pendente |
| `draw_circle` | `draw_circle(... [4 args])` | Builtin GM | 5 | pendente |
| `camera_get_view_width` | `camera_get_view_width(... [? args])` | Builtin GM | 5 | pendente |
| `camera_get_view_height` | `camera_get_view_height(... [? args])` | Builtin GM | 5 | pendente |
| `sprite_get_speed` | `sprite_get_speed(... [1 args])` | Builtin GM | 5 | pendente |
| `gpu_set_tex_filter` | `gpu_set_tex_filter(... [1 args])` | Builtin GM | 5 | pendente |
| `camera_get_view_x` | `camera_get_view_x(... [? args])` | Builtin GM | 5 | pendente |
| `camera_get_view_y` | `camera_get_view_y(... [? args])` | Builtin GM | 5 | pendente |
| `gpu_set_texfilter` | `gpu_set_texfilter(... [1 args])` | Builtin GM | 5 | pendente |
| `draw_rectangle_color` | `draw_rectangle_color(... [9 args])` | Builtin GM | 5 | pendente |
| `draw_clear_alpha` | `draw_clear_alpha(... [2 args])` | Builtin GM | 4 | pendente |
| `shader_set_uniform_f_array` | `shader_set_uniform_f_array(... [2 args])` | Builtin GM | 4 | pendente |
| `gpu_push_state` | `gpu_push_state(... [0 args])` | Builtin GM | 4 | pendente |
| `sprite_exists` | `sprite_exists(... [1 args])` | Builtin GM | 4 | pendente |
| `sprite_get_yoffset` | `sprite_get_yoffset(... [1 args])` | Builtin GM | 4 | pendente |
| `sprite_delete` | `sprite_delete(... [1 args])` | Builtin GM | 4 | pendente |
| `draw_vertex_texture` | `draw_vertex_texture(... [4 args])` | Builtin GM | 4 | pendente |
| `draw_ellipse_color` | `draw_ellipse_color(... [7 args])` | Builtin GM | 3 | pendente |
| `window_get_width` | `window_get_width(... [0 args])` | Builtin GM | 3 | pendente |
| `window_get_height` | `window_get_height(... [0 args])` | Builtin GM | 3 | pendente |
| `matrix_build_identity` | `matrix_build_identity(... [0 args])` | Builtin GM | 3 | pendente |
| `draw_primitive_begin` | `draw_primitive_begin(... [1 args])` | Builtin GM | 3 | pendente |
| `gpu_pop_state` | `gpu_pop_state(... [0 args])` | Builtin GM | 3 | pendente |
| `gpu_set_fog` | `gpu_set_fog(... [0xffffffff args])` | Builtin GM | 3 | pendente |
| `draw_primitive_begin_texture` | `draw_primitive_begin_texture(... [2 args])` | Builtin GM | 3 | pendente |
| `gpu_set_texfilter_ext` | `gpu_set_texfilter_ext(... [2 args])` | Builtin GM | 2 | pendente |
| `gpu_set_blendmode_ext` | `gpu_set_blendmode_ext(... [0xffffffff args])` | Builtin GM | 2 | pendente |
| `matrix_build_projection_perspective_fov` | `matrix_build_projection_perspective_fov(... [4 args])` | Builtin GM | 2 | pendente |
| `sprite_get_xoffset` | `sprite_get_xoffset(... [1 args])` | Builtin GM | 2 | pendente |
| `sprite_create_from_surface` | `sprite_create_from_surface(... [9 args])` | Builtin GM | 2 | pendente |
| `surface_save` | `surface_save(... [2 args])` | Builtin GM | 2 | pendente |
| `draw_tilemap` | `draw_tilemap(... [3 args])` | Builtin GM | 2 | pendente |
| `surface_get_texture` | `surface_get_texture(... [1 args])` | Builtin GM | 2 | pendente |
| `texture_get_uvs` | `texture_get_uvs(... [1 args])` | Builtin GM | 2 | pendente |
| `camera_destroy` | `camera_destroy(... [? args])` | Builtin GM | 2 | pendente |
| `matrix_build` | `matrix_build(... [9 args])` | Builtin GM | 2 | pendente |
| `window_get_x` | `window_get_x(... [0 args])` | Builtin GM | 2 | pendente |
| `window_get_y` | `window_get_y(... [0 args])` | Builtin GM | 2 | pendente |
| `gpu_set_blendenable` | `gpu_set_blendenable(... [1 args])` | Builtin GM | 2 | pendente |
| `draw_ellipse` | `draw_ellipse(... [5 args])` | Builtin GM | 1 | pendente |
| `merge_colour` | `merge_colour(... [3 args])` | Builtin GM | 1 | pendente |
| `gpu_set_tex_filter_ext` | `gpu_set_tex_filter_ext(... [2 args])` | Builtin GM | 1 | pendente |
| `gpu_set_alphatestenable` | `gpu_set_alphatestenable(... [1 args])` | Builtin GM | 1 | pendente |
| `display_get_width` | `display_get_width(... [0 args])` | Builtin GM | 1 | pendente |
| `display_get_height` | `display_get_height(... [0 args])` | Builtin GM | 1 | pendente |
| `display_get_gui_width` | `display_get_gui_width(... [0 args])` | Builtin GM | 1 | pendente |
| `display_get_gui_height` | `display_get_gui_height(... [0 args])` | Builtin GM | 1 | pendente |
| `surface_get_width` | `surface_get_width(... [1 args])` | Builtin GM | 1 | pendente |
| `surface_get_height` | `surface_get_height(... [1 args])` | Builtin GM | 1 | pendente |
| `matrix_build_lookat` | `matrix_build_lookat(... [9 args])` | Builtin GM | 1 | pendente |
| `camera_set_view_mat` | `camera_set_view_mat(... [? args])` | Builtin GM | 1 | pendente |
| `matrix_multiply` | `matrix_multiply(... [2 args])` | Builtin GM | 1 | pendente |
| `draw_line_width` | `draw_line_width(... [5 args])` | Builtin GM | 1 | pendente |
| `draw_surface` | `draw_surface(... [3 args])` | Builtin GM | 1 | pendente |
| `draw_surface_ext` | `draw_surface_ext(... [8 args])` | Builtin GM | 1 | pendente |
| `matrix_transform_vertex` | `matrix_transform_vertex(... [4 args])` | Builtin GM | 1 | pendente |
| `draw_text_ext` | `draw_text_ext(... [5 args])` | Builtin GM | 1 | pendente |
| `camera_set_view_pos` | `camera_set_view_pos(... [? args])` | Builtin GM | 1 | pendente |
| `display_mouse_get_x` | `display_mouse_get_x(... [0 args])` | Builtin GM | 1 | pendente |
| `display_mouse_get_y` | `display_mouse_get_y(... [0 args])` | Builtin GM | 1 | pendente |
| `camera_set_proj_mat` | `camera_set_proj_mat(... [? args])` | Builtin GM | 1 | pendente |
| `draw_rectangle_colour` | `draw_rectangle_colour(... [9 args])` | Builtin GM | 1 | pendente |
| `surface_resize` | `surface_resize(... [3 args])` | Builtin GM | 1 | pendente |
| `window_set_rectangle` | `window_set_rectangle(... [4 args])` | Builtin GM | 1 | pendente |
| `window_set_size` | `window_set_size(... [2 args])` | Builtin GM | 1 | pendente |
| `draw_line_color` | `draw_line_color(... [6 args])` | Builtin GM | 1 | pendente |

## YoYo/GML

Total de funções: **66** | Total de invocações: **121**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `tilemap_get` | `tilemap_get(... [3 args])` | Builtin GM | 16 | pendente |
| `layer_get_id` | `layer_get_id(... [1 args])` | Builtin GM | 12 | pendente |
| `layer_tilemap_get_id` | `layer_tilemap_get_id(... [1 args])` | Builtin GM | 7 | pendente |
| `layer_set_visible` | `layer_set_visible(... [2 args])` | Builtin GM | 6 | pendente |
| `layer_get_depth` | `layer_get_depth(... [1 args])` | Builtin GM | 6 | pendente |
| `layer_exists` | `layer_exists(... [1 args])` | Builtin GM | 4 | pendente |
| `layer_script_begin` | `layer_script_begin(... [2 args])` | Builtin GM | 3 | pendente |
| `layer_shader` | `layer_shader(... [2 args])` | Builtin GM | 3 | pendente |
| `psn_post_uds_event` | `psn_post_uds_event(... [3 args])` | Builtin GM | 2 | pendente |
| `layer_get_all_elements` | `layer_get_all_elements(... [1 args])` | Builtin GM | 2 | pendente |
| `tilemap_get_width` | `tilemap_get_width(... [1 args])` | Builtin GM | 2 | pendente |
| `tilemap_get_height` | `tilemap_get_height(... [1 args])` | Builtin GM | 2 | pendente |
| `tilemap_set` | `tilemap_set(... [4 args])` | Builtin GM | 2 | pendente |
| `tilemap_get_at_pixel` | `tilemap_get_at_pixel(... [3 args])` | Builtin GM | 2 | pendente |
| `sha1_string_utf8` | `sha1_string_utf8(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_get_all` | `layer_get_all(... [0 args])` | Builtin GM | 1 | pendente |
| `layer_get_visible` | `layer_get_visible(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_get_element_type` | `layer_get_element_type(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_instance_get_instance` | `layer_instance_get_instance(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_get_alpha` | `layer_sprite_get_alpha(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_get_blend` | `layer_sprite_get_blend(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_get_angle` | `layer_sprite_get_angle(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_get_yscale` | `layer_sprite_get_yscale(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_get_xscale` | `layer_sprite_get_xscale(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_get_y` | `layer_sprite_get_y(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_get_x` | `layer_sprite_get_x(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_get_index` | `layer_sprite_get_index(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_get_sprite` | `layer_sprite_get_sprite(... [1 args])` | Builtin GM | 1 | pendente |
| `tilemap_get_x` | `tilemap_get_x(... [1 args])` | Builtin GM | 1 | pendente |
| `tilemap_get_y` | `tilemap_get_y(... [1 args])` | Builtin GM | 1 | pendente |
| `ps5_gamepad_set_trigger_effect_vibration` | `ps5_gamepad_set_trigger_effect_vibration(... [5 args])` | Builtin GM | 1 | pendente |
| `os_get_language` | `os_get_language(... [0 args])` | Builtin GM | 1 | pendente |
| `os_get_region` | `os_get_region(... [0 args])` | Builtin GM | 1 | pendente |
| `layer_get_hspeed` | `layer_get_hspeed(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_get_vspeed` | `layer_get_vspeed(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_get_x` | `layer_get_x(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_get_y` | `layer_get_y(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_tilemap_create` | `layer_tilemap_create(... [6 args])` | Builtin GM | 1 | pendente |
| `ps5_gamepad_set_trigger_effect_feedback` | `ps5_gamepad_set_trigger_effect_feedback(... [3 args])` | Builtin GM | 1 | pendente |
| `os_get_config` | `os_get_config(... [0 args])` | Builtin GM | 1 | pendente |
| `os_is_paused` | `os_is_paused(... [0 args])` | Builtin GM | 1 | pendente |
| `ps5_gamepad_set_trigger_effect_weapon` | `ps5_gamepad_set_trigger_effect_weapon(... [5 args])` | Builtin GM | 1 | pendente |
| `ps5_gamepad_set_trigger_effect_off` | `ps5_gamepad_set_trigger_effect_off(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_background_get_id` | `layer_background_get_id(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_hspeed` | `layer_hspeed(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_vspeed` | `layer_vspeed(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_background_sprite` | `layer_background_sprite(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_background_index` | `layer_background_index(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_background_speed` | `layer_background_speed(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_background_xscale` | `layer_background_xscale(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_background_yscale` | `layer_background_yscale(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_background_alpha` | `layer_background_alpha(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_background_blend` | `layer_background_blend(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_background_htiled` | `layer_background_htiled(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_background_vtiled` | `layer_background_vtiled(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_background_stretch` | `layer_background_stretch(... [2 args])` | Builtin GM | 1 | pendente |
| `switch_get_operation_mode` | `switch_get_operation_mode(... [0 args])` | Builtin GM | 1 | pendente |
| `switch_controller_support_set_all` | `switch_controller_support_set_all(... [8 args])` | Builtin GM | 1 | pendente |
| `switch_controller_joycon_set_holdtype` | `switch_controller_joycon_set_holdtype(... [1 args])` | Builtin GM | 1 | pendente |
| `switch_controller_set_supported_styles` | `switch_controller_set_supported_styles(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_create` | `layer_create(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_create` | `layer_sprite_create(... [4 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_angle` | `layer_sprite_angle(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_index` | `layer_sprite_index(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_blend` | `layer_sprite_blend(... [2 args])` | Builtin GM | 1 | pendente |
| `layer_sprite_destroy` | `layer_sprite_destroy(... [1 args])` | Builtin GM | 1 | pendente |

## Partículas

Total de funções: **23** | Total de invocações: **107**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `part_emitter_region` | `part_emitter_region(... [8 args])` | Builtin GM | 18 | pendente |
| `part_emitter_destroy` | `part_emitter_destroy(... [2 args])` | Builtin GM | 13 | pendente |
| `part_emitter_clear` | `part_emitter_clear(... [2 args])` | Builtin GM | 9 | pendente |
| `part_emitter_create` | `part_emitter_create(... [1 args])` | Builtin GM | 7 | pendente |
| `part_system_destroy` | `part_system_destroy(... [1 args])` | Builtin GM | 6 | pendente |
| `part_type_speed` | `part_type_speed(... [5 args])` | Builtin GM | 6 | pendente |
| `part_type_destroy` | `part_type_destroy(... [1 args])` | Builtin GM | 5 | pendente |
| `part_type_size` | `part_type_size(... [5 args])` | Builtin GM | 4 | pendente |
| `part_type_alpha2` | `part_type_alpha2(... [3 args])` | Builtin GM | 4 | pendente |
| `part_type_direction` | `part_type_direction(... [5 args])` | Builtin GM | 4 | pendente |
| `part_type_blend` | `part_type_blend(... [2 args])` | Builtin GM | 4 | pendente |
| `part_emitter_burst` | `part_emitter_burst(... [4 args])` | Builtin GM | 4 | pendente |
| `part_type_create` | `part_type_create(... [0 args])` | Builtin GM | 4 | pendente |
| `part_system_create` | `part_system_create(... [0 args])` | Builtin GM | 3 | pendente |
| `part_type_sprite` | `part_type_sprite(... [5 args])` | Builtin GM | 3 | pendente |
| `part_type_life` | `part_type_life(... [3 args])` | Builtin GM | 3 | pendente |
| `part_emitter_stream` | `part_emitter_stream(... [4 args])` | Builtin GM | 2 | pendente |
| `part_particles_clear` | `part_particles_clear(... [1 args])` | Builtin GM | 2 | pendente |
| `part_type_gravity` | `part_type_gravity(... [3 args])` | Builtin GM | 2 | pendente |
| `part_system_depth` | `part_system_depth(... [2 args])` | Builtin GM | 1 | pendente |
| `part_type_shape` | `part_type_shape(... [2 args])` | Builtin GM | 1 | pendente |
| `part_type_color_hsv` | `part_type_color_hsv(... [7 args])` | Builtin GM | 1 | pendente |
| `part_system_exists` | `part_system_exists(... [1 args])` | Builtin GM | 1 | pendente |

## Arquivos e Buffers

Total de funções: **26** | Total de invocações: **74**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `buffer_create` | `buffer_create(... [3 args])` | Builtin GM | 8 | pendente |
| `buffer_async_group_option` | `buffer_async_group_option(... [2 args])` | Builtin GM | 8 | pendente |
| `buffer_tell` | `buffer_tell(... [1 args])` | Builtin GM | 7 | pendente |
| `buffer_delete` | `buffer_delete(... [1 args])` | Builtin GM | 7 | pendente |
| `buffer_peek` | `buffer_peek(... [3 args])` | Builtin GM | 7 | pendente |
| `buffer_seek` | `buffer_seek(... [3 args])` | Builtin GM | 6 | pendente |
| `buffer_poke` | `buffer_poke(... [4 args])` | Builtin GM | 4 | pendente |
| `buffer_get_size` | `buffer_get_size(... [1 args])` | Builtin GM | 3 | pendente |
| `buffer_async_group_begin` | `buffer_async_group_begin(... [1 args])` | Builtin GM | 2 | pendente |
| `buffer_async_group_end` | `buffer_async_group_end(... [0 args])` | Builtin GM | 2 | pendente |
| `buffer_sha1` | `buffer_sha1(... [3 args])` | Builtin GM | 2 | pendente |
| `directory_destroy` | `directory_destroy(... [1 args])` | Builtin GM | 2 | pendente |
| `file_text_close` | `file_text_close(... [1 args])` | Builtin GM | 2 | pendente |
| `buffer_copy` | `buffer_copy(... [5 args])` | Builtin GM | 2 | pendente |
| `buffer_load_async` | `buffer_load_async(... [4 args])` | Builtin GM | 1 | pendente |
| `buffer_get_surface` | `buffer_get_surface(... [3 args])` | Builtin GM | 1 | pendente |
| `buffer_save_async` | `buffer_save_async(... [4 args])` | Builtin GM | 1 | pendente |
| `buffer_exists` | `buffer_exists(... [1 args])` | Builtin GM | 1 | pendente |
| `file_exists` | `file_exists(... [1 args])` | Builtin GM | 1 | pendente |
| `file_text_open_write` | `file_text_open_write(... [1 args])` | Builtin GM | 1 | pendente |
| `file_text_write_string` | `file_text_write_string(... [2 args])` | Builtin GM | 1 | pendente |
| `buffer_create_from_vertex_buffer` | `buffer_create_from_vertex_buffer(... [3 args])` | Builtin GM | 1 | pendente |
| `buffer_resize` | `buffer_resize(... [2 args])` | Builtin GM | 1 | pendente |
| `file_text_open_read` | `file_text_open_read(... [1 args])` | Builtin GM | 1 | pendente |
| `file_text_eof` | `file_text_eof(... [1 args])` | Builtin GM | 1 | pendente |
| `file_text_readln` | `file_text_readln(... [1 args])` | Builtin GM | 1 | pendente |

## Áudio

Total de funções: **11** | Total de invocações: **29**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `audio_get_name` | `audio_get_name(... [1 args])` | Builtin GM | 7 | pendente |
| `audio_sound_gain` | `audio_sound_gain(... [3 args])` | Builtin GM | 5 | pendente |
| `audio_sound_pitch` | `audio_sound_pitch(... [2 args])` | Builtin GM | 3 | pendente |
| `audio_play_sound` | `audio_play_sound(... [0xffffffff args])` | Builtin GM | 3 | pendente |
| `audio_is_playing` | `audio_is_playing(... [1 args])` | Builtin GM | 2 | pendente |
| `audio_stop_sound` | `audio_stop_sound(... [1 args])` | Builtin GM | 2 | pendente |
| `audio_sound_set_track_position` | `audio_sound_set_track_position(... [2 args])` | Builtin GM | 2 | pendente |
| `audio_emitter_free` | `audio_emitter_free(... [1 args])` | Builtin GM | 2 | pendente |
| `audio_group_is_loaded` | `audio_group_is_loaded(... [1 args])` | Builtin GM | 1 | pendente |
| `audio_sound_length` | `audio_sound_length(... [1 args])` | Builtin GM | 1 | pendente |
| `audio_stop_all` | `audio_stop_all(... [0 args])` | Builtin GM | 1 | pendente |

## Recursos

Total de funções: **9** | Total de invocações: **26**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `animcurve_get_channel` | `animcurve_get_channel(... [2 args])` | Builtin GM | 9 | pendente |
| `animcurve_channel_evaluate` | `animcurve_channel_evaluate(... [2 args])` | Builtin GM | 8 | pendente |
| `path_exists` | `path_exists(... [1 args])` | Builtin GM | 2 | pendente |
| `layer_set_fx` | `layer_set_fx(... [2 args])` | Builtin GM | 2 | pendente |
| `fx_set_parameter` | `fx_set_parameter(... [0xffffffff args])` | Builtin GM | 1 | pendente |
| `path_delete` | `path_delete(... [1 args])` | Builtin GM | 1 | pendente |
| `path_get_length` | `path_get_length(... [1 args])` | Builtin GM | 1 | pendente |
| `layer_get_fx` | `layer_get_fx(... [1 args])` | Builtin GM | 1 | pendente |
| `fx_get_parameter` | `fx_get_parameter(... [2 args])` | Builtin GM | 1 | pendente |

## Input

Total de funções: **12** | Total de invocações: **21**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `device_mouse_check_button` | `device_mouse_check_button(... [2 args])` | Builtin GM | 6 | pendente |
| `gamepad_hat_value` | `gamepad_hat_value(... [2 args])` | Builtin GM | 3 | pendente |
| `keyboard_check_pressed` | `keyboard_check_pressed(... [1 args])` | Builtin GM | 3 | pendente |
| `keyboard_check_released` | `keyboard_check_released(... [1 args])` | Builtin GM | 1 | pendente |
| `device_mouse_raw_x` | `device_mouse_raw_x(... [1 args])` | Builtin GM | 1 | pendente |
| `device_mouse_raw_y` | `device_mouse_raw_y(... [1 args])` | Builtin GM | 1 | pendente |
| `mouse_check_button_pressed` | `mouse_check_button_pressed(... [1 args])` | Builtin GM | 1 | pendente |
| `mouse_wheel_up` | `mouse_wheel_up(... [0 args])` | Builtin GM | 1 | pendente |
| `mouse_wheel_down` | `mouse_wheel_down(... [0 args])` | Builtin GM | 1 | pendente |
| `gamepad_is_connected` | `gamepad_is_connected(... [1 args])` | Builtin GM | 1 | pendente |
| `keyboard_virtual_status` | `keyboard_virtual_status(... [0 args])` | Builtin GM | 1 | pendente |
| `keyboard_virtual_show` | `keyboard_virtual_show(... [4 args])` | Builtin GM | 1 | pendente |

## Shaders

Total de funções: **7** | Total de invocações: **20**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `vertex_format_begin` | `vertex_format_begin(... [0 args])` | Builtin GM | 4 | pendente |
| `vertex_format_add_position` | `vertex_format_add_position(... [0 args])` | Builtin GM | 4 | pendente |
| `vertex_format_add_texcoord` | `vertex_format_add_texcoord(... [0 args])` | Builtin GM | 4 | pendente |
| `vertex_format_end` | `vertex_format_end(... [0 args])` | Builtin GM | 4 | pendente |
| `vertex_format_add_colour` | `vertex_format_add_colour(... [0 args])` | Builtin GM | 2 | pendente |
| `vertex_format_add_normal` | `vertex_format_add_normal(... [0 args])` | Builtin GM | 1 | pendente |
| `vertex_format_add_color` | `vertex_format_add_color(... [0 args])` | Builtin GM | 1 | pendente |

## Matemática

Total de funções: **4** | Total de invocações: **18**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `is_undefined` | `is_undefined(... [1 args])` | Builtin GM | 12 | pendente |
| `instanceof` | `instanceof(... [1 args])` | Builtin GM | 4 | pendente |
| `is_nan` | `is_nan(... [1 args])` | Builtin GM | 1 | pendente |
| `typeof` | `typeof(... [1 args])` | Builtin GM | 1 | pendente |

## Buffers

Total de funções: **5** | Total de invocações: **18**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `vertex_delete_buffer` | `vertex_delete_buffer(... [1 args])` | Builtin GM | 9 | pendente |
| `vertex_submit` | `vertex_submit(... [3 args])` | Builtin GM | 5 | pendente |
| `vertex_create_buffer` | `vertex_create_buffer(... [0 args])` | Builtin GM | 2 | pendente |
| `vertex_freeze` | `vertex_freeze(... [1 args])` | Builtin GM | 1 | pendente |
| `vertex_create_buffer_from_buffer` | `vertex_create_buffer_from_buffer(... [2 args])` | Builtin GM | 1 | pendente |

## Interação

Total de funções: **2** | Total de invocações: **14**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `show_message` | `show_message(... [1 args])` | Builtin GM | 8 | pendente |
| `show_error` | `show_error(... [2 args])` | Builtin GM | 6 | pendente |

## Diversos

Total de funções: **1** | Total de invocações: **4**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `alarm_set` | `alarm_set(... [2 args])` | Builtin GM | 4 | pendente |

## Arquivos

Total de funções: **1** | Total de invocações: **1**

| Função | Assinatura / Papel | Tipo | Chamadas | Status |
| :--- | :--- | :---: | :---: | :---: |
| `json_parse` | `json_parse(... [1 args])` | Builtin GM | 1 | pendente |

