# Port de Vernal Edge para Android

Guia técnico para portar o código decompilado (pastas `gml/` e `engine/`) de um jogo GameMaker compilado com YYC para Android arm64. Uso pessoal, a partir da sua própria cópia legítima do jogo.

## 1. Visão geral

O jogo foi decompilado em C pelo Ghidra e a saída ficou em duas pastas:

- **`gml/`**: a lógica do jogo (objetos, scripts, eventos). Em builds YYC, o GML virou código nativo dentro do exe. É a parte que você quer manter.
- **`engine/`**: o runner do GameMaker (janela, renderização, áudio, input, arquivos). É a parte que precisa ser substituída por uma camada própria para Android.

**Ideia central:** não portar o `engine/` linha por linha. Descobrir quais funções do runner o `gml/` usa e reimplementar só essas, sobre SDL3, GLES 3 (ou Vulkan) e AAudio.

**Aviso importante:** o C gerado pelo Ghidra é pseudo-código. Ele não compila de primeira, tem tipos errados e nomes genéricos. Espere uma fase longa de limpeza antes de ver qualquer coisa na tela.

## 2. Pré-requisitos

- Android Studio com **NDK** (versão LTS recente) e CMake.
- Dispositivo ARM64 (arm64-v8a) com depuração USB.
- **SDL3** (como submódulo ou via FetchContent no CMake).
- Ghidra com o MCP que você já usa, para renomear símbolos e consultar o binário.
- **UndertaleModTool** (ou biblioteca equivalente) como referência do formato `data.win`.
- A cópia do jogo instalada, com `data.win` e arquivos de áudio externos.
- Um repositório Git com três partes separadas: `gml/`, `runner/` (seu código novo) e `android/` (projeto Gradle).

## 3. Fase 0: confirmar o tipo de build

1. Verifique se o `data.win` contém bytecode de GML (build VM) ou apenas assets (YYC).
2. Se for **VM**, pare aqui: não precisa recompilar. Use um runner GameMaker para Android que carregue o `data.win`.
3. Se for **YYC**, continue. O `data.win` ainda guarda sprites, sons, fontes, salas e metadados, mas a lógica está no exe.

### 4. Fase 1: inventário da API usada pelo `gml/`

Objetivo: obter a lista exata de funções do runner de que o código do jogo depende.

1. Liste todas as funções chamadas dentro de `gml/` que não são definidas em `gml/` (use um script no Ghidra ou grep sobre os símbolos).
2. Agrupe por categoria: gráficos (`draw_*`, `surface_*`, `shader_*`), áudio (`audio_*`), input (`keyboard_*`, `gamepad_*`, `mouse_*`), estruturas (`ds_list_*`, `ds_map_*`, `array_*`), arquivos (`file_*`, `ini_*`, `buffer_*`), sistema (`room_*`, `instance_*`, `game_*`).
3. Conte a frequência de cada função. As mais usadas entram primeiro.
4. Crie uma planilha ou arquivo `api.md` com: nome, assinatura inferida, categoria, número de chamadas, status (pendente, parcial, pronto).

Essa lista é o seu contrato com o novo runner.

## 5. Fase 2: limpeza do `gml/`

1. Renomeie em massa pelo MCP: padrões `gml_Object_<nome>_<evento>` e `gml_Script_<nome>` já indicam o papel de cada função.
2. Substitua tipos genéricos (`undefined4`, `longlong`) por tipos reais, principalmente o valor dinâmico do GameMaker (um struct tipo `RValue` com tipo, flags e união de dados).
3. Defina em um cabeçalho seu (`gml_types.h`) as structs de **instância**, **objeto** e **valor**. Todo o código do `gml/` vai depender delas.
4. Compile arquivo por arquivo com `clang` apontando para um cabeçalho de stubs. Corrija os erros por lotes.
5. Meta da fase: todo o `gml/` compila como biblioteca estática arm64, mesmo que as chamadas ao runner sejam funções vazias.

## 6. Fase 3: o núcleo do runner

Implemente o mínimo para o jogo existir:

- **Loop principal**: passo fixo (o GameMaker usa o room speed, normalmente 60 por segundo), com eventos na ordem Create, Begin Step, Step, End Step, Draw.
- **Gerenciador de salas**: carregar a sala, criar instâncias, ordenar por profundidade.
- **Instâncias e objetos**: tabela de objetos com herança (parent), lista de instâncias vivas, variáveis por instância (mapa nome para valor).
- **Valor dinâmico**: números (double), strings, arrays, structs, ponteiros, undefined. Referência contada para strings e arrays.
- **Estruturas de dados**: `ds_list`, `ds_map`, `ds_grid`, `ds_stack`, `ds_queue`, `ds_priority`.

Dica: implemente primeiro o que o inventário da Fase 1 mostrar como mais frequente.

## 7. Fase 4: gráficos

1. Crie a janela e o contexto com SDL3 (GLES 3.0 para começar; Vulkan depois, se precisar).
2. Carregue as **texture pages** do `data.win` (o formato guarda sprites em atlas, em PNG ou BZ2/QOI conforme a versão). Decodifique para texturas GPU.
3. Implemente `draw_sprite`, `draw_sprite_ext`, `draw_sprite_part`, `draw_rectangle`, `draw_text` e `draw_set_*`.
4. Use um **batcher** de quads (um buffer dinâmico, uma chamada por troca de textura ou estado) para manter o desempenho.
5. Câmeras e views: matriz de projeção por view, com escala para a tela do celular (manter a proporção, barras pretas ou crop).
6. **Surfaces** (`surface_create`, `surface_set_target`) viram framebuffers. Muitos efeitos dependem disso.
7. **Shaders**: o GameMaker usa GLSL ES nos shaders do usuário. Extraia os shaders do `data.win` e adapte para GLES 3 se necessário.
8. Blend modes (`gpu_set_blendmode`) mapeiam direto para o blending do GL.

## 8. Fase 5: áudio

1. Use SDL3 audio ou AAudio direto, com um mixer próprio (soma de vozes em float, resample para 48 kHz).
2. Decodifique OGG (stb\_vorbis ou libvorbis) e WAV. Faça streaming para músicas longas.
3. Implemente `audio_play_sound`, `audio_stop_sound`, `audio_sound_gain`, `audio_sound_pitch`, loops e prioridade de voz.
4. Os sons ficam no chunk de áudio do `data.win` ou em arquivos externos ao lado dele. Confirme qual é o caso.

## 9. Fase 6: input

- **Teclado**: mapeie teclas virtuais para as `vk_*` do GameMaker (`vk_left`, `vk_space` etc.).
- **Touch**: crie controles na tela (direcional e botões) que injetam eventos de teclado, ajustando posição e tamanho.
- **Gamepad**: SDL3 gamepad com mapeamento padrão, exposto como `gamepad_*`. Para um metroidvania de ação, suporte a controle é o ideal.
- Cuidado com a latência: processe os eventos no início de cada passo.

## 10. Fase 7: arquivos, saves e configurações

- Aponte o diretório de trabalho do jogo para o armazenamento do app (`getExternalFilesDir`).
- Implemente `file_text_*`, `file_bin_*`, `ini_*` e `buffer_*` sobre `fopen`.
- Os saves devem ir para um diretório gravável e persistente do app.
- Os assets (`data.win` e áudio) ficam em armazenamento acessível: copie na primeira execução ou leia via SAF ou OBB.

## 11. Fase 8: projeto Android

1. Crie um projeto com `NativeActivity` ou uma Activity do SDL3 e CMake com `arm64-v8a`.
2. Compile `gml/` (estática), `runner/` e SDL3 em uma única `.so`.
3. Use flags de otimização (`-O2`) e, durante a depuração, símbolos e sanitizers (`-fsanitize=address` com cuidado).
4. Permissões: só armazenamento do app, sem permissões desnecessárias.
5. Mantenha a tela ligada, trate pause e resume (salvar estado e liberar o contexto GL).

## 12. Fase 9: testes e depuração

Ordem sugerida de marcos:

1. A `.so` carrega e abre uma janela preta.
2. Mostra um sprite estático da sala inicial.
3. Tela de título com input funcionando.
4. Música e efeitos sonoros.
5. Primeira sala jogável com movimento e colisão.
6. Inimigos, bosses, câmera e HUD.
7. Salvar e carregar.
8. Jogo completo sem travar.

Ferramentas: `logcat`, `adb`, `gdb` ou `lldb` para o NDK, RenderDoc ou o Android GPU Inspector para problemas gráficos.

**Truque útil:** compare o comportamento com o exe original rodando no PC (ou no Windroid). Faça logs de eventos e posições nos dois e compare.

## 13. Alternativa mais rápida para validar

Se você só quer jogar no Android, rode o exe original no **Windroid** (Wine com Box64). É bem mais simples e serve como referência do comportamento esperado enquanto o port nativo não fica pronto.

## 14. Riscos e pontos de atenção

- O volume de limpeza do `gml/` é grande: dedique tempo a automatizar renomeação e correção de tipos.
- O comportamento exato do GameMaker (ordem de eventos, colisão, profundidade, arredondamento) precisa ser reproduzido com cuidado para o jogo se comportar igual.
- Funções do runner pouco usadas podem aparecer só em fases avançadas: faça stubs que registram no log quando forem chamados.
- Desempenho: o batcher de quads e o uso eficiente de texturas são decisivos em GPUs móveis.

## 15. Cuidados legais

- Use apenas a sua cópia legítima do jogo.
- Não distribua o código decompilado, o runner derivado dele, o `data.win` nem os assets.
- Se quiser compartilhar algo, publique somente código seu, original, sem trechos copiados do jogo.

## 16. Checklist final

- [x] Tipo de build confirmado (VM ou YYC) — Confirmado: YYC.
- [x] Lista de funções do runner usadas pelo `gml/` — Gerado em api.md.
- [x] `gml/` limpo e compilando — Validado via tools/build_gml_library.py.
- [x] Tipos de valor, instância e objeto definidos — runner/include/gml_types.h.
- [x] Loop principal e gerenciador de salas — runner/src/main_loop.c e runner/src/room.c.
- [x] Carregador do `data.win` — runner/src/data_win.c e runner/include/qoi.h (descompactador qoz2/QOI + BZ2).
- [x] Gráficos: sprites, texto, surfaces e shaders — runner/src/graphics.c e runner/src/gles_loader.c.
- [x] Áudio: música e efeitos — runner/src/audio.c e runner/include/audio.h (stb_vorbis + mixer estéreo + SOND/AUDO).
- [x] Input: teclado, toque e controle — runner/src/input.c e runner/include/input.h (Teclado, Mouse/Touch, Gamepad e Overlay Virtual com Multi-touch).
- [x] Saves e arquivos — runner/src/files.c e runner/include/files.h (file_text_*, buffer_*, SHA-1, CRC32, async groups e diretório configurável de save).
- [x] Projeto Android com CMake e NativeActivity — `arm64-v8a`, GLES 3.0, AAudio Low Latency, APK assinado e pipeline em `tools/build_apk.sh`.
- [x] Testes por marcos e execução validada no dispositivo — Instalado no Xiaomi 12 (`cec895ed`), EGL 2294x1080 ativo a 90 FPS, `data.win` (322 MB) e 58 faixas de áudio carregados, AAudio executando `mus_title_theme.ogg` em tempo real, overlay virtual touch funcional na tela.
