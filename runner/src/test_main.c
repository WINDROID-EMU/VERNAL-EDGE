#include "runner.h"
#include "graphics.h"
#include "data_win.h"
#include "audio.h"
#include "input.h"
#include "files.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

int main(void) {
    printf("========================================================\n");
    printf(" Vernal Edge Runner - Fase 7: Arquivos, Saves & Buffers\n");
    printf("========================================================\n\n");

    RunnerConfig cfg = {
        .window_width = 480,
        .window_height = 270,
        .target_fps = 60,
        .vsync = true,
        .title = "Vernal Edge Runner"
    };

    Runner_Init(&cfg);

    // =========================================================
    // 1. Audio Regression Check (Fase 5)
    // =========================================================
    printf("\n[Test 1] Audio Subsystem Quick Verification...\n");
    int32_t sfx_idx = data_win_find_sound_by_name(&g_DataWin, "sfx_voice_vernal_hurt_05");
    assert(sfx_idx >= 0);
    int32_t mus_idx = data_win_find_sound_by_name(&g_DataWin, "mus_title_theme");
    assert(mus_idx >= 0);
    int32_t voice_sfx = audio_play_sound(sfx_idx, 5, false);
    assert(voice_sfx > 0);
    assert(audio_is_playing(voice_sfx));
    audio_stop_all();
    assert(!audio_is_playing(voice_sfx));
    printf("  -> Audio Subsystem: PASSED\n");

    // =========================================================
    // 2. Keyboard & Engine Binary Compatibility Test (Fase 6)
    // =========================================================
    printf("\n[Test 2] Testing Keyboard State, Transitions & Binary Hooks...\n");
    assert(input_keyboard_check(GM_VK_NOKEY) == true);
    input_keyboard_set(GM_VK_SPACE, true);
    input_keyboard_set(GM_VK_LEFT, true);
    assert(DAT_142613940[GM_VK_SPACE] == 1);
    assert(FUN_14192ec30(GM_VK_SPACE) == 1);
    assert(FUN_14188b3e0(GM_VK_SPACE) == 1);

    input_begin_frame();
    assert(input_keyboard_check_pressed(GM_VK_SPACE) == true);

    RValue res, arg;
    RValue_Init(&res);
    RValue_Init(&arg);
    RValue_SetInt32(&arg, GM_VK_SPACE);
    RValue* argv[1] = { &arg };
    FUN_14183b9b0(NULL, NULL, &res, 1, 0x1424a6388, argv);
    assert(RValue_AsBool(&res) == true);

    input_keyboard_clear(-1);
    assert(input_keyboard_check(GM_VK_NOKEY) == true);
    printf("  -> Keyboard States & Engine Dispatch: PASSED\n");

    // =========================================================
    // 3. Mouse & Gamepad Subsystem (Fase 6)
    // =========================================================
    printf("\n[Test 3] Testing Mouse, Multi-Touch & Gamepad Subsystem...\n");
    input_mouse_set_pos(320.0f, 180.0f);
    input_mouse_set_button(GM_MB_LEFT, true);
    input_begin_frame();
    assert(input_mouse_check_button(GM_MB_LEFT) == true);
    assert(input_mouse_check_button_pressed(GM_MB_LEFT) == true);

    input_mouse_set_wheel(1);
    FUN_14183b9b0(NULL, NULL, &res, 0, 0x1424a6398, NULL);
    assert(RValue_AsBool(&res) == true);

    input_gamepad_set_button(0, GM_GP_FACE1, true);
    input_gamepad_set_axis(0, GM_GP_AXISLH, 0.75f);
    input_begin_frame();
    assert(input_gamepad_button_check(0, GM_GP_FACE1) == true);
    assert(fabsf(input_gamepad_axis_value(0, GM_GP_AXISLH) - 0.75f) < 0.001f);

    input_gamepad_set_button(0, GM_GP_FACE1, false);
    input_mouse_set_button(GM_MB_LEFT, false);
    printf("  -> Mouse & Gamepad Subsystem: PASSED\n");

    // =========================================================
    // 4. File Operations & Text File API (Fase 7)
    // =========================================================
    printf("\n[Test 4] Testing Text File Persistence & Path Resolution...\n");

    // Configure dedicated save directory (matching Android externalFilesDir)
    files_set_save_directory("runner/scratch/savedata");
    assert(strcmp(files_get_save_directory(), "runner/scratch/savedata/") == 0);

    const char* test_file = "slot1_save.json";

    // 4.1 Write text file using dispatcher (0x1424a6448, 0x1424a6458, 0x1424a6468)
    RValue fname_arg, content_arg;
    RValue_Init(&fname_arg);
    RValue_Init(&content_arg);
    RValue_SetString(&fname_arg, test_file);
    RValue* open_w_argv[1] = { &fname_arg };

    FUN_14183b9b0(NULL, NULL, &res, 1, 0x1424a6448, open_w_argv);
    int32_t handle = RValue_AsInt32(&res);
    assert(handle > 0);

    const char* sample_json = "{\"slot\": 1, \"health\": 100, \"room\": \"r_intro\", \"shard_count\": 12}";
    RValue handle_arg;
    RValue_Init(&handle_arg);
    RValue_SetInt32(&handle_arg, handle);
    RValue_SetString(&content_arg, sample_json);
    RValue* write_argv[2] = { &handle_arg, &content_arg };

    FUN_14183b9b0(NULL, NULL, &res, 2, 0x1424a6458, write_argv);

    // Close file
    RValue* close_argv[1] = { &handle_arg };
    FUN_14183b9b0(NULL, NULL, &res, 1, 0x1424a6468, close_argv);

    // 4.2 Verify file exists on disk via dispatcher (0x1424a7968)
    FUN_14183b9b0(NULL, NULL, &res, 1, 0x1424a7968, open_w_argv);
    assert(RValue_AsBool(&res) == true);
    assert(file_exists(test_file) == true);

    // 4.3 Read back text file using dispatcher (0x1424a7cc8, 0x1424a7cd8, 0x1424a7ce8)
    FUN_14183b9b0(NULL, NULL, &res, 1, 0x1424a7cc8, open_w_argv);
    int32_t r_handle = RValue_AsInt32(&res);
    assert(r_handle > 0);

    RValue r_handle_arg;
    RValue_Init(&r_handle_arg);
    RValue_SetInt32(&r_handle_arg, r_handle);
    RValue* read_argv[1] = { &r_handle_arg };

    FUN_14183b9b0(NULL, NULL, &res, 1, 0x1424a7cd8, read_argv);
    assert(RValue_AsBool(&res) == false); // not EOF yet

    FUN_14183b9b0(NULL, NULL, &res, 1, 0x1424a7ce8, read_argv);
    const char* read_str = RValue_AsString(&res);
    assert(read_str != NULL);
    assert(strcmp(read_str, sample_json) == 0);
    printf("  -> Written & read back JSON: '%s'\n", read_str);

    file_text_close(r_handle);

    // 4.4 Test file_copy and file_delete
    const char* backup_file = "slot1_save_backup.json";
    assert(file_copy(test_file, backup_file) == true);
    assert(file_exists(backup_file) == true);
    assert(file_delete(backup_file) == true);
    assert(file_exists(backup_file) == false);
    file_delete(test_file);
    assert(file_exists(test_file) == false);

    printf("  -> Text File Read/Write, File Copy & Delete: PASSED\n");

    // =========================================================
    // 5. Buffer Subsystem & Binary Serialization (Fase 7)
    // =========================================================
    printf("\n[Test 5] Testing Buffer Creation, Seek/Tell, Peek/Poke & Binary Writes...\n");

    // 5.1 Create a grow buffer via dispatcher (0x1424a6b28)
    RValue sz_arg, type_arg, align_arg;
    RValue_Init(&sz_arg);
    RValue_Init(&type_arg);
    RValue_Init(&align_arg);
    RValue_SetReal(&sz_arg, 16.0);
    RValue_SetInt32(&type_arg, BUFFER_GROW);
    RValue_SetReal(&align_arg, 1.0);
    RValue* buf_create_argv[3] = { &sz_arg, &type_arg, &align_arg };

    FUN_14183b9b0(NULL, NULL, &res, 3, 0x1424a6b28, buf_create_argv);
    int32_t buf_id = RValue_AsInt32(&res);
    assert(buf_id >= 0);
    assert(buffer_exists(buf_id) == true);

    // 5.2 Write data types via buffer_write / FUN_1418884f0 (Engine hook)
    RValue val_u8, val_u16, val_f32, val_str;
    RValue_Init(&val_u8);
    RValue_Init(&val_u16);
    RValue_Init(&val_f32);
    RValue_Init(&val_str);

    RValue_SetReal(&val_u8, 0xFE);
    RValue_SetReal(&val_u16, 0xCAFE);
    RValue_SetReal(&val_f32, 3.14159f);
    RValue_SetString(&val_str, "Vernal Edge Save Data");

    assert(FUN_1418884f0(buf_id, BUFFER_U8, &val_u8) == 1);
    assert(FUN_1418884f0(buf_id, BUFFER_U16, &val_u16) == 2);
    assert(FUN_1418884f0(buf_id, BUFFER_F32, &val_f32) == 4);
    assert(FUN_1418884f0(buf_id, BUFFER_STRING, &val_str) == (int32_t)strlen("Vernal Edge Save Data") + 1);

    size_t current_sz = buffer_get_size(buf_id);
    assert(current_sz == 1 + 2 + 4 + strlen("Vernal Edge Save Data") + 1);
    printf("  -> Total buffer serialized size: %zu bytes\n", current_sz);

    // 5.3 Test buffer_seek to start and read back sequentially
    buffer_seek(buf_id, BUFFER_SEEK_START, 0);
    assert(buffer_tell(buf_id) == 0);

    RValue out_u8, out_u16, out_f32, out_str;
    assert(buffer_read(buf_id, BUFFER_U8, &out_u8));
    assert(RValue_AsInt32(&out_u8) == 0xFE);

    assert(buffer_read(buf_id, BUFFER_U16, &out_u16));
    assert(RValue_AsInt32(&out_u16) == 0xCAFE);

    assert(buffer_read(buf_id, BUFFER_F32, &out_f32));
    assert(fabsf((float)RValue_AsReal(&out_f32) - 3.14159f) < 0.0001f);

    assert(buffer_read(buf_id, BUFFER_STRING, &out_str));
    assert(strcmp(RValue_AsString(&out_str), "Vernal Edge Save Data") == 0);
    printf("  -> Sequentially deserialized buffer matching exactly: '%s'\n", RValue_AsString(&out_str));

    // 5.4 Test buffer_sha1 and buffer_crc32
    char sha1[41];
    buffer_sha1(buf_id, 0, current_sz, sha1);
    assert(strlen(sha1) == 40);
    uint32_t crc = buffer_crc32(buf_id, 0, current_sz);
    assert(crc != 0);
    printf("  -> Buffer SHA-1: %s | CRC32: 0x%08X\n", sha1, crc);

    // 5.5 Test binary file save & load (buffer_save & buffer_load)
    const char* bin_save = "player_state.sav";
    assert(buffer_save(buf_id, bin_save) == true);
    assert(file_exists(bin_save) == true);

    int32_t loaded_buf = buffer_load(bin_save);
    assert(loaded_buf >= 0);
    assert(buffer_get_size(loaded_buf) == current_sz);

    char sha1_loaded[41];
    buffer_sha1(loaded_buf, 0, buffer_get_size(loaded_buf), sha1_loaded);
    assert(strcmp(sha1, sha1_loaded) == 0);
    printf("  -> Binary file saved and reloaded with identical SHA-1! PASSED\n");

    // 5.6 Test async group save simulation (0x1424a7038, 0x1424a7058, 0x1424a7078)
    RValue group_name, save_name, off_arg, count_arg;
    RValue_Init(&group_name);
    RValue_Init(&save_name);
    RValue_Init(&off_arg);
    RValue_Init(&count_arg);
    RValue_SetString(&group_name, "VernalSaveGroup");
    RValue_SetString(&save_name, "async_save.dat");
    RValue_SetReal(&off_arg, 0.0);
    RValue_SetReal(&count_arg, (double)current_sz);

    RValue* grp_argv[1] = { &group_name };
    FUN_14183b9b0(NULL, NULL, &res, 1, 0x1424a7038, grp_argv); // group begin

    RValue b_arg;
    RValue_Init(&b_arg);
    RValue_SetInt32(&b_arg, buf_id);
    RValue* async_save_argv[4] = { &b_arg, &save_name, &off_arg, &count_arg };
    FUN_14183b9b0(NULL, NULL, &res, 4, 0x1424a7058, async_save_argv); // save async
    int32_t async_id = RValue_AsInt32(&res);
    assert(async_id > 0);

    FUN_14183b9b0(NULL, NULL, &res, 0, 0x1424a7078, NULL); // group end
    assert(file_exists("async_save.dat") == true);

    // Clean up buffers and files
    buffer_delete(buf_id);
    buffer_delete(loaded_buf);
    file_delete(bin_save);
    file_delete("async_save.dat");

    assert(!buffer_exists(buf_id));
    assert(!buffer_exists(loaded_buf));
    printf("  -> Buffer Subsystem & Async Group Persistence: PASSED\n");

    // =========================================================
    // 6. Integrated 60-Frame Simulation (Full Engine Active)
    // =========================================================
    printf("\n[Test 6] Running 60-Frame Simulation with Audio, Video, Input & File State...\n");
    int32_t title_room_idx = data_win_find_room_by_name(&g_DataWin, "titletemp");
    assert(title_room_idx >= 0);
    room_goto(title_room_idx);

    int32_t bgm = audio_play_sound(mus_idx, 10, true);
    assert(bgm > 0);

    for (int frame = 0; frame < 60; frame++) {
        if (frame == 10) {
            input_keyboard_set(GM_VK_SPACE, true);
        } else if (frame == 25) {
            input_keyboard_set(GM_VK_SPACE, false);
        }

        Runner_Step();
        Runner_Draw();

        float frame_audio[735 * 2];
        audio_mix_stereo_f32(frame_audio, 735);
    }
    printf("  -> 60 frames simulated with synchronized Input, Audio, Graphics and Files! PASSED\n");

    Runner_Shutdown();
    printf("\n========================================================\n");
    printf(" FASE 7: ARQUIVOS, SAVES E BUFFERS 100%% CONCLUÍDA!\n");
    printf("========================================================\n");
    return 0;
}
