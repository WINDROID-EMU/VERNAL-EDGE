#include <jni.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include <android/log.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include <aaudio/AAudio.h>

#include "runner.h"
#include "graphics.h"
#include "audio.h"
#include "input.h"
#include "files.h"

#define LOG_TAG "VernalEdge"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#include <pthread.h>
static int s_pfd[2];
static pthread_t s_log_thread;

static void* LogcatLoggerThread(void* arg) {
    (void)arg;
    ssize_t rdsz;
    char buf[1024];
    while ((rdsz = read(s_pfd[0], buf, sizeof(buf) - 1)) > 0) {
        if (rdsz > 0 && buf[rdsz - 1] == '\n') --rdsz;
        buf[rdsz] = 0;
        __android_log_print(ANDROID_LOG_INFO, "VernalEdge", "%s", buf);
    }
    return NULL;
}

static void RedirectStdioToLogcat(void) {
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);
    pipe(s_pfd);
    dup2(s_pfd[1], 1);
    dup2(s_pfd[1], 2);
    pthread_create(&s_log_thread, 0, LogcatLoggerThread, 0);
    pthread_detach(s_log_thread);
}

/* EGL and Graphics state */
typedef struct {
    struct android_app* app;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    bool animating;
    bool has_surface;
    AAudioStream* audio_stream;
} EngineState;

static EngineState s_engine;

/* -------------------------------------------------------------
 * AAudio Real-time Audio Stream
 * ------------------------------------------------------------- */
static aaudio_data_callback_result_t AudioCallback(
    AAudioStream* stream, void* userData, void* audioData, int32_t numFrames) {
    (void)stream; (void)userData;
    audio_mix_stereo_f32((float*)audioData, (size_t)numFrames);
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

static void InitAAudio(EngineState* engine) {
    AAudioStreamBuilder* builder = NULL;
    aaudio_result_t res = AAudio_createStreamBuilder(&builder);
    if (res != AAUDIO_OK) {
        LOGE("Failed to create AAudioStreamBuilder: %s", AAudio_convertResultToText(res));
        return;
    }

    AAudioStreamBuilder_setFormat(builder, AAUDIO_FORMAT_PCM_FLOAT);
    AAudioStreamBuilder_setChannelCount(builder, 2);
    AAudioStreamBuilder_setSampleRate(builder, 44100);
    AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setSharingMode(builder, AAUDIO_SHARING_MODE_SHARED);
    AAudioStreamBuilder_setDataCallback(builder, AudioCallback, engine);

    res = AAudioStreamBuilder_openStream(builder, &engine->audio_stream);
    AAudioStreamBuilder_delete(builder);

    if (res == AAUDIO_OK && engine->audio_stream) {
        AAudioStream_requestStart(engine->audio_stream);
        LOGI("AAudio stream active (44100 Hz, Stereo Float, Low Latency)");
    } else {
        LOGE("Failed to open AAudio stream: %s", AAudio_convertResultToText(res));
        engine->audio_stream = NULL;
    }
}

static void StopAAudio(EngineState* engine) {
    if (engine->audio_stream) {
        AAudioStream_requestStop(engine->audio_stream);
        AAudioStream_close(engine->audio_stream);
        engine->audio_stream = NULL;
        LOGI("AAudio stream stopped");
    }
}

/* -------------------------------------------------------------
 * EGL Display & Context Management
 * ------------------------------------------------------------- */
static int InitDisplay(EngineState* engine) {
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };

    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, NULL, NULL);

    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    EGLSurface surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGE("Unable to eglMakeCurrent");
        return -1;
    }

    EGLint w, h;
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;
    engine->has_surface = true;

    LOGI("EGL Initialized: Resolution %dx%d, GLES 3.0", w, h);

    /* Initialize Game Runner */
    RunnerConfig cfg = {
        .window_width = w,
        .window_height = h,
        .target_fps = 60,
        .vsync = true,
        .title = "Vernal Edge"
    };

    /* Set Save Directory to internal storage */
    if (engine->app->activity->internalDataPath) {
        files_set_save_directory(engine->app->activity->internalDataPath);
        LOGI("Save directory set to: %s", engine->app->activity->internalDataPath);
    }
    if (engine->app->activity->externalDataPath) {
        files_set_asset_directory(engine->app->activity->externalDataPath);
        LOGI("Asset directory set to: %s", engine->app->activity->externalDataPath);
    }

    Runner_Init(&cfg);

    /* Enable and configure on-screen virtual controls for mobile */
    input_virtual_touch_set_enabled(true);
    input_virtual_touch_configure((float)w, (float)h);
    LOGI("Virtual touch overlay configured for %fx%f", (float)w, (float)h);

    /* Start audio engine */
    InitAAudio(engine);

    return 0;
}

static void TermDisplay(EngineState* engine) {
    StopAAudio(engine);

    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->has_surface = false;
    engine->animating = false;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

/* -------------------------------------------------------------
 * Input Event Handling (Touch & Gamepad)
 * ------------------------------------------------------------- */
static int32_t HandleInput(struct android_app* app, AInputEvent* event) {
    (void)app;
    int32_t type = AInputEvent_getType(event);

    if (type == AINPUT_EVENT_TYPE_MOTION) {
        int32_t action_data = AMotionEvent_getAction(event);
        int32_t action = action_data & AMOTION_EVENT_ACTION_MASK;
        size_t pointer_idx = (action_data & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                             >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        size_t pointer_count = AMotionEvent_getPointerCount(event);

        if (action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_POINTER_DOWN) {
            int id = AMotionEvent_getPointerId(event, pointer_idx);
            float x = AMotionEvent_getX(event, pointer_idx);
            float y = AMotionEvent_getY(event, pointer_idx);
            input_touch_down(id, x, y);
            return 1;
        } else if (action == AMOTION_EVENT_ACTION_MOVE) {
            for (size_t i = 0; i < pointer_count; i++) {
                int id = AMotionEvent_getPointerId(event, i);
                float x = AMotionEvent_getX(event, i);
                float y = AMotionEvent_getY(event, i);
                input_touch_move(id, x, y);
            }
            return 1;
        } else if (action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_POINTER_UP
                   || action == AMOTION_EVENT_ACTION_CANCEL) {
            int id = AMotionEvent_getPointerId(event, pointer_idx);
            input_touch_up(id);
            return 1;
        }
    } else if (type == AINPUT_EVENT_TYPE_KEY) {
        int32_t key_action = AKeyEvent_getAction(event);
        int32_t key_code = AKeyEvent_getKeyCode(event);
        bool is_down = (key_action == AKEY_EVENT_ACTION_DOWN);

        switch (key_code) {
            case AKEYCODE_BACK:
                input_keyboard_set(GM_VK_ESCAPE, is_down);
                input_gamepad_set_button(0, GM_GP_START, is_down);
                return 1;
            case AKEYCODE_BUTTON_A:
                input_keyboard_set(GM_VK_SPACE, is_down);
                input_gamepad_set_button(0, GM_GP_FACE1, is_down);
                return 1;
            case AKEYCODE_BUTTON_B:
                input_keyboard_set('C', is_down);
                input_gamepad_set_button(0, GM_GP_FACE2, is_down);
                return 1;
            case AKEYCODE_BUTTON_X:
                input_keyboard_set('Z', is_down);
                input_gamepad_set_button(0, GM_GP_FACE3, is_down);
                return 1;
            case AKEYCODE_BUTTON_Y:
                input_keyboard_set('X', is_down);
                input_gamepad_set_button(0, GM_GP_FACE4, is_down);
                return 1;
            case AKEYCODE_BUTTON_L1:
                input_keyboard_set('A', is_down);
                input_gamepad_set_button(0, GM_GP_SHOULDERL, is_down);
                return 1;
            case AKEYCODE_BUTTON_R1:
                input_gamepad_set_button(0, GM_GP_SHOULDERR, is_down);
                return 1;
            case AKEYCODE_BUTTON_START:
                input_keyboard_set(GM_VK_ESCAPE, is_down);
                input_gamepad_set_button(0, GM_GP_START, is_down);
                return 1;
            case AKEYCODE_DPAD_LEFT:
                input_keyboard_set(GM_VK_LEFT, is_down);
                input_gamepad_set_button(0, GM_GP_PADL, is_down);
                return 1;
            case AKEYCODE_DPAD_RIGHT:
                input_keyboard_set(GM_VK_RIGHT, is_down);
                input_gamepad_set_button(0, GM_GP_PADR, is_down);
                return 1;
            case AKEYCODE_DPAD_UP:
                input_keyboard_set(GM_VK_UP, is_down);
                input_gamepad_set_button(0, GM_GP_PADU, is_down);
                return 1;
            case AKEYCODE_DPAD_DOWN:
                input_keyboard_set(GM_VK_DOWN, is_down);
                input_gamepad_set_button(0, GM_GP_PADD, is_down);
                return 1;
            default:
                break;
        }
    }
    return 0;
}

/* -------------------------------------------------------------
 * Application Lifecycle Command Handling
 * ------------------------------------------------------------- */
static void HandleCmd(struct android_app* app, int32_t cmd) {
    EngineState* engine = (EngineState*)app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (engine->app->window != NULL) {
                InitDisplay(engine);
                engine->animating = true;
            }
            break;
        case APP_CMD_TERM_WINDOW:
            TermDisplay(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            engine->animating = true;
            break;
        case APP_CMD_LOST_FOCUS:
            engine->animating = false;
            break;
        case APP_CMD_PAUSE:
            engine->animating = false;
            break;
        case APP_CMD_DESTROY:
            Runner_Shutdown();
            break;
    }
}

/* -------------------------------------------------------------
 * android_main (NativeActivity Entry Point)
 * ------------------------------------------------------------- */
void android_main(struct android_app* state) {
    memset(&s_engine, 0, sizeof(EngineState));
    state->userData = &s_engine;
    state->onAppCmd = HandleCmd;
    state->onInputEvent = HandleInput;
    s_engine.app = state;

    RedirectStdioToLogcat();
    LOGI("Vernal Edge NativeActivity starting...");

    while (1) {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident = ALooper_pollOnce(s_engine.animating ? 0 : -1, NULL, &events, (void**)&source)) >= 0) {
            if (source != NULL) {
                source->process(state, source);
            }
            if (state->destroyRequested != 0) {
                LOGI("Destroy requested by Android OS");
                TermDisplay(&s_engine);
                return;
            }
        }

        if (s_engine.animating && s_engine.has_surface) {
            Runner_Step();
            Runner_Draw();
            eglSwapBuffers(s_engine.display, s_engine.surface);
        }
    }
}
