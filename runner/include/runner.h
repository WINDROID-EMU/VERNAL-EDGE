#ifndef RUNNER_CORE_H
#define RUNNER_CORE_H

#include "rvalue.h"
#include "ds.h"
#include "instance.h"
#include "room.h"
#include "builtins_dispatcher.h"
#include "input.h"
#include "files.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RunnerConfig {
    int32_t window_width;
    int32_t window_height;
    int32_t target_fps;
    bool vsync;
    const char* title;
} RunnerConfig;

// Core Runner API
void Runner_Init(const RunnerConfig* config);
void Runner_Step(void);
void Runner_Draw(void);
void Runner_Shutdown(void);
bool Runner_IsRunning(void);
const RunnerConfig* Runner_GetConfig(void);

#ifdef __cplusplus
}
#endif

#endif // RUNNER_CORE_H
