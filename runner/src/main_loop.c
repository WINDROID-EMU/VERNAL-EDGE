#include "runner.h"
#include "graphics.h"
#include "data_win.h"
#include "audio.h"
#include "room.h"
#include "files.h"
#include "input.h"
#include <stdio.h>
#include <time.h>

static bool s_running = false;
static uint64_t s_frame_count = 0;
static RunnerConfig s_config;

void Runner_Init(const RunnerConfig* config) {
    if (config) {
        s_config = *config;
    } else {
        s_config.window_width = 1280;
        s_config.window_height = 720;
        s_config.target_fps = 60;
        s_config.vsync = true;
        s_config.title = "Vernal Edge";
    }

    printf("[Runner] Initializing Core Systems...\n");
    MemoryManager_Init();
    DS_Init();
    InstanceManager_Init();
    RoomManager_Init();
    BuiltinsDispatcher_Init();

    /* Try loading data.win if not loaded yet */
    if (!g_DataWin.file_data) {
        data_win_load(&g_DataWin, "data.win");
    }

    /* Initialize 2D Graphics */
    graphics_init(s_config.window_width, s_config.window_height);

    /* Initialize Audio Subsystem */
    audio_init();

    /* Initialize Input Subsystem */
    input_init();

    /* Initialize Files & Buffers Subsystem */
    files_init(NULL);

    /* Start Initial Room & Title Music */
    if (g_DataWin.file_data) {
        int32_t first_room = data_win_find_room_by_name(&g_DataWin, "titletemp");
        if (first_room < 0 && g_DataWin.room_count > 0) {
            first_room = 0;
        }
        if (first_room >= 0) {
            printf("[Runner] Transitioning to initial room: %d ('%s')\n",
                   first_room, g_DataWin.rooms[first_room].name);
            room_goto(first_room);
        }

        int32_t mus_idx = data_win_find_sound_by_name(&g_DataWin, "mus_title_theme");
        if (mus_idx >= 0) {
            printf("[Runner] Starting background music: mus_title_theme (id %d)\n", mus_idx);
            audio_play_sound(mus_idx, 10, true);
        }
    }

    s_running = true;
    s_frame_count = 0;
    printf("[Runner] Core Initialized. Target FPS: %d\n", s_config.target_fps);
}

void Runner_Step(void) {
    if (!s_running) return;
    s_frame_count++;
    input_begin_frame();
    RoomManager_Step();
}

void Runner_Draw(void) {
    if (!s_running) return;
    RoomManager_Draw();

    /* Render Mobile Virtual Touch Controls Overlay in Screen GUI space */
    graphics_set_projection(0.0f, (float)s_config.window_width, (float)s_config.window_height, 0.0f);
    input_virtual_touch_draw();
    graphics_end_frame();
}

void Runner_Shutdown(void) {
    printf("[Runner] Shutting down (Total frames: %lu)...\n", (unsigned long)s_frame_count);
    s_running = false;
    files_shutdown();
    input_shutdown();
    audio_shutdown();
    graphics_shutdown();
    data_win_free(&g_DataWin);
    InstanceManager_Reset();
    DS_Reset();
    MemoryManager_Clean();
}

bool Runner_IsRunning(void) {
    return s_running;
}

const RunnerConfig* Runner_GetConfig(void) {
    return &s_config;
}
