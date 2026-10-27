#include "input.h"
#include "graphics.h"
#include "room.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Compatibility memory block for engine */
uint8_t DAT_142613940[256];

/* Keyboard internal state */
static uint8_t s_key_current[256];
static uint8_t s_key_prev[256];
static uint8_t s_key_pressed[256];
static uint8_t s_key_released[256];
static int s_last_key = 0;

/* Mouse internal state */
static float s_mouse_raw_x = 0.0f;
static float s_mouse_raw_y = 0.0f;
static uint8_t s_mouse_btn[4];
static uint8_t s_mouse_btn_prev[4];
static uint8_t s_mouse_btn_pressed[4];
static uint8_t s_mouse_btn_released[4];
static int s_mouse_wheel = 0;

/* Multi-touch fingers */
typedef struct {
    int id;
    bool active;
    bool prev_active;
    bool pressed;
    bool released;
    float raw_x;
    float raw_y;
} TouchFinger;

static TouchFinger s_fingers[MAX_TOUCH_FINGERS];

/* Gamepad internal state */
typedef struct {
    bool connected;
    char description[64];
    uint8_t btn_current[16];
    uint8_t btn_prev[16];
    uint8_t btn_pressed[16];
    uint8_t btn_released[16];
    float axes[4]; /* 0: LH, 1: LV, 2: RH, 3: RV */
} GamepadState;

static GamepadState s_pads[MAX_GAMEPADS];

/* Virtual Touch Overlay */
static bool s_virtual_touch_enabled = false;
static VirtualStick s_vstick;
#define MAX_VBUTTONS 6
static VirtualButton s_vbuttons[MAX_VBUTTONS];
static int s_vbutton_count = 0;
static float s_screen_w = 1280.0f;
static float s_screen_h = 720.0f;

/* -------------------------------------------------------------
 * Lifecycle
 * ------------------------------------------------------------- */
void input_init(void) {
    memset(DAT_142613940, 0, sizeof(DAT_142613940));
    memset(s_key_current, 0, sizeof(s_key_current));
    memset(s_key_prev, 0, sizeof(s_key_prev));
    memset(s_key_pressed, 0, sizeof(s_key_pressed));
    memset(s_key_released, 0, sizeof(s_key_released));
    s_last_key = 0;

    memset(s_mouse_btn, 0, sizeof(s_mouse_btn));
    memset(s_mouse_btn_prev, 0, sizeof(s_mouse_btn_prev));
    memset(s_mouse_btn_pressed, 0, sizeof(s_mouse_btn_pressed));
    memset(s_mouse_btn_released, 0, sizeof(s_mouse_btn_released));
    s_mouse_raw_x = 0.0f;
    s_mouse_raw_y = 0.0f;
    s_mouse_wheel = 0;

    memset(s_fingers, 0, sizeof(s_fingers));
    for (int i = 0; i < MAX_TOUCH_FINGERS; i++) {
        s_fingers[i].id = -1;
    }

    memset(s_pads, 0, sizeof(s_pads));
    /* Gamepad 0 connected by default for seamless input detection */
    s_pads[0].connected = true;
    strncpy(s_pads[0].description, "Standard Gamepad (Virtual/SDL)", sizeof(s_pads[0].description) - 1);

    input_virtual_touch_configure(1280.0f, 720.0f);
    printf("[Input] Subsystem initialized (Keyboard, Mouse, Touch, %d Gamepads, Virtual Controls ready)\n", MAX_GAMEPADS);
}

void input_shutdown(void) {
    printf("[Input] Subsystem shut down\n");
}

void input_begin_frame(void) {
    /* 1. Process Virtual Touch controls if enabled */
    if (s_virtual_touch_enabled) {
        input_virtual_touch_process();
    }

    /* 2. Update keyboard transition flags */
    for (int k = 0; k < 256; k++) {
        s_key_pressed[k] = (s_key_current[k] && !s_key_prev[k]) ? 1 : 0;
        s_key_released[k] = (!s_key_current[k] && s_key_prev[k]) ? 1 : 0;
        s_key_prev[k] = s_key_current[k];
        DAT_142613940[k] = s_key_current[k];
    }

    /* 3. Update mouse transition flags */
    for (int b = 1; b <= 3; b++) {
        s_mouse_btn_pressed[b] = (s_mouse_btn[b] && !s_mouse_btn_prev[b]) ? 1 : 0;
        s_mouse_btn_released[b] = (!s_mouse_btn[b] && s_mouse_btn_prev[b]) ? 1 : 0;
        s_mouse_btn_prev[b] = s_mouse_btn[b];
    }
    s_mouse_wheel = 0;

    /* 4. Update touch finger transitions */
    for (int i = 0; i < MAX_TOUCH_FINGERS; i++) {
        s_fingers[i].pressed = (s_fingers[i].active && !s_fingers[i].prev_active);
        s_fingers[i].released = (!s_fingers[i].active && s_fingers[i].prev_active);
        s_fingers[i].prev_active = s_fingers[i].active;
    }

    /* 5. Update gamepad button transition flags */
    for (int p = 0; p < MAX_GAMEPADS; p++) {
        if (!s_pads[p].connected) continue;
        for (int b = 0; b < 16; b++) {
            s_pads[p].btn_pressed[b] = (s_pads[p].btn_current[b] && !s_pads[p].btn_prev[b]) ? 1 : 0;
            s_pads[p].btn_released[b] = (!s_pads[p].btn_current[b] && s_pads[p].btn_prev[b]) ? 1 : 0;
            s_pads[p].btn_prev[b] = s_pads[p].btn_current[b];
        }
    }
}

/* -------------------------------------------------------------
 * Keyboard API
 * ------------------------------------------------------------- */
void input_keyboard_set(int vk_code, bool down) {
    if (vk_code >= 0 && vk_code < 256) {
        s_key_current[vk_code] = down ? 1 : 0;
        DAT_142613940[vk_code] = s_key_current[vk_code];
        if (down) {
            s_last_key = vk_code;
        }
    }
}

bool input_keyboard_check(int vk_code) {
    if (vk_code == GM_VK_NOKEY) {
        for (int i = 0; i < 256; i++) {
            if (s_key_current[i]) return false;
        }
        return true;
    }
    if (vk_code == GM_VK_ANYKEY) {
        for (int i = 0; i < 256; i++) {
            if (s_key_current[i]) return true;
        }
        return false;
    }
    if (vk_code >= 0 && vk_code < 256) {
        return s_key_current[vk_code] != 0;
    }
    return false;
}

bool input_keyboard_check_pressed(int vk_code) {
    if (vk_code >= 0 && vk_code < 256) {
        return s_key_pressed[vk_code] != 0;
    }
    return false;
}

bool input_keyboard_check_released(int vk_code) {
    if (vk_code >= 0 && vk_code < 256) {
        return s_key_released[vk_code] != 0;
    }
    return false;
}

void input_keyboard_clear(int vk_code) {
    if (vk_code >= 0 && vk_code < 256) {
        s_key_current[vk_code] = 0;
        s_key_prev[vk_code] = 0;
        s_key_pressed[vk_code] = 0;
        s_key_released[vk_code] = 0;
        DAT_142613940[vk_code] = 0;
    } else if (vk_code == -1) {
        memset(s_key_current, 0, sizeof(s_key_current));
        memset(s_key_prev, 0, sizeof(s_key_prev));
        memset(s_key_pressed, 0, sizeof(s_key_pressed));
        memset(s_key_released, 0, sizeof(s_key_released));
        memset(DAT_142613940, 0, sizeof(DAT_142613940));
    }
}

int input_keyboard_get_lastkey(void) {
    return s_last_key;
}

/* -------------------------------------------------------------
 * Mouse & Multi-touch API
 * ------------------------------------------------------------- */
void input_mouse_set_pos(float raw_x, float raw_y) {
    s_mouse_raw_x = raw_x;
    s_mouse_raw_y = raw_y;
}

void input_mouse_set_button(int button, bool down) {
    if (button >= 1 && button <= 3) {
        s_mouse_btn[button] = down ? 1 : 0;
    }
}

void input_mouse_set_wheel(int delta) {
    s_mouse_wheel = delta;
}

bool input_mouse_check_button(int button) {
    if (button >= 1 && button <= 3) return s_mouse_btn[button] != 0;
    return false;
}

bool input_mouse_check_button_pressed(int button) {
    if (button >= 1 && button <= 3) return s_mouse_btn_pressed[button] != 0;
    return false;
}

bool input_mouse_check_button_released(int button) {
    if (button >= 1 && button <= 3) return s_mouse_btn_released[button] != 0;
    return false;
}

float input_mouse_get_x(void) {
    float vx = 0.0f, vy = 0.0f;
    camera_get_view_pos(&vx, &vy);
    return s_mouse_raw_x + vx;
}

float input_mouse_get_y(void) {
    float vx = 0.0f, vy = 0.0f;
    camera_get_view_pos(&vx, &vy);
    return s_mouse_raw_y + vy;
}

int input_mouse_get_wheel_delta(void) {
    return s_mouse_wheel;
}

void input_touch_down(int finger_id, float raw_x, float raw_y) {
    for (int i = 0; i < MAX_TOUCH_FINGERS; i++) {
        if (!s_fingers[i].active || s_fingers[i].id == finger_id) {
            s_fingers[i].id = finger_id;
            s_fingers[i].active = true;
            s_fingers[i].pressed = true;
            s_fingers[i].raw_x = raw_x;
            s_fingers[i].raw_y = raw_y;
            break;
        }
    }
    /* Finger 0 also updates mouse position */
    if (finger_id == 0 || s_fingers[0].id == finger_id) {
        s_mouse_raw_x = raw_x;
        s_mouse_raw_y = raw_y;
        s_mouse_btn[1] = 1;
        s_mouse_btn_pressed[1] = 1;
    }
}

void input_touch_move(int finger_id, float raw_x, float raw_y) {
    for (int i = 0; i < MAX_TOUCH_FINGERS; i++) {
        if (s_fingers[i].active && s_fingers[i].id == finger_id) {
            s_fingers[i].raw_x = raw_x;
            s_fingers[i].raw_y = raw_y;
            break;
        }
    }
    if (finger_id == 0 || s_fingers[0].id == finger_id) {
        s_mouse_raw_x = raw_x;
        s_mouse_raw_y = raw_y;
    }
}

void input_touch_up(int finger_id) {
    for (int i = 0; i < MAX_TOUCH_FINGERS; i++) {
        if (s_fingers[i].active && s_fingers[i].id == finger_id) {
            s_fingers[i].active = false;
            s_fingers[i].released = true;
            s_fingers[i].id = -1;
            break;
        }
    }
    if (finger_id == 0 || s_fingers[0].id == -1) {
        s_mouse_btn[1] = 0;
        s_mouse_btn_released[1] = 1;
    }
}

bool input_device_mouse_check_button(int device, int button) {
    if (device >= 0 && device < MAX_TOUCH_FINGERS) {
        if (button == GM_MB_LEFT) return s_fingers[device].active;
    }
    return false;
}

bool input_device_mouse_check_button_pressed(int device, int button) {
    if (device >= 0 && device < MAX_TOUCH_FINGERS) {
        if (button == GM_MB_LEFT) return s_fingers[device].pressed;
    }
    return false;
}

bool input_device_mouse_check_button_released(int device, int button) {
    if (device >= 0 && device < MAX_TOUCH_FINGERS) {
        if (button == GM_MB_LEFT) return s_fingers[device].released;
    }
    return false;
}

float input_device_mouse_raw_x(int device) {
    if (device >= 0 && device < MAX_TOUCH_FINGERS && s_fingers[device].active) {
        return s_fingers[device].raw_x;
    }
    return s_mouse_raw_x;
}

float input_device_mouse_raw_y(int device) {
    if (device >= 0 && device < MAX_TOUCH_FINGERS && s_fingers[device].active) {
        return s_fingers[device].raw_y;
    }
    return s_mouse_raw_y;
}

float input_device_mouse_x(int device) {
    float vx = 0.0f, vy = 0.0f;
    camera_get_view_pos(&vx, &vy);
    return input_device_mouse_raw_x(device) + vx;
}

float input_device_mouse_y(int device) {
    float vx = 0.0f, vy = 0.0f;
    camera_get_view_pos(&vx, &vy);
    return input_device_mouse_raw_y(device) + vy;
}

/* -------------------------------------------------------------
 * Gamepad API
 * ------------------------------------------------------------- */
static inline int GamepadButtonToIndex(int btn) {
    if (btn >= GM_GP_FACE1 && btn <= GM_GP_PADR) {
        return btn - GM_GP_FACE1;
    }
    return -1;
}

int input_gamepad_get_device_count(void) {
    return MAX_GAMEPADS;
}

bool input_gamepad_is_connected(int device) {
    if (device >= 0 && device < MAX_GAMEPADS) {
        return s_pads[device].connected;
    }
    return false;
}

void input_gamepad_set_connected(int device, bool connected, const char* desc) {
    if (device >= 0 && device < MAX_GAMEPADS) {
        s_pads[device].connected = connected;
        if (desc) {
            strncpy(s_pads[device].description, desc, sizeof(s_pads[device].description) - 1);
        }
    }
}

const char* input_gamepad_get_description(int device) {
    if (device >= 0 && device < MAX_GAMEPADS && s_pads[device].connected) {
        return s_pads[device].description;
    }
    return "";
}

void input_gamepad_set_button(int device, int btn, bool down) {
    if (device >= 0 && device < MAX_GAMEPADS) {
        int idx = GamepadButtonToIndex(btn);
        if (idx >= 0 && idx < 16) {
            s_pads[device].btn_current[idx] = down ? 1 : 0;
        }
    }
}

void input_gamepad_set_axis(int device, int axis, float value) {
    if (device >= 0 && device < MAX_GAMEPADS) {
        if (axis >= GM_GP_AXISLH && axis <= GM_GP_AXISRV) {
            int a = axis - GM_GP_AXISLH;
            if (a >= 0 && a < 4) {
                /* Clamp [-1.0, 1.0] */
                if (value < -1.0f) value = -1.0f;
                if (value > 1.0f)  value = 1.0f;
                s_pads[device].axes[a] = value;
            }
        }
    }
}

bool input_gamepad_button_check(int device, int btn) {
    if (device >= 0 && device < MAX_GAMEPADS && s_pads[device].connected) {
        int idx = GamepadButtonToIndex(btn);
        if (idx >= 0 && idx < 16) {
            return s_pads[device].btn_current[idx] != 0;
        }
    }
    return false;
}

bool input_gamepad_button_check_pressed(int device, int btn) {
    if (device >= 0 && device < MAX_GAMEPADS && s_pads[device].connected) {
        int idx = GamepadButtonToIndex(btn);
        if (idx >= 0 && idx < 16) {
            return s_pads[device].btn_pressed[idx] != 0;
        }
    }
    return false;
}

bool input_gamepad_button_check_released(int device, int btn) {
    if (device >= 0 && device < MAX_GAMEPADS && s_pads[device].connected) {
        int idx = GamepadButtonToIndex(btn);
        if (idx >= 0 && idx < 16) {
            return s_pads[device].btn_released[idx] != 0;
        }
    }
    return false;
}

float input_gamepad_axis_value(int device, int axis) {
    if (device >= 0 && device < MAX_GAMEPADS && s_pads[device].connected) {
        if (axis >= GM_GP_AXISLH && axis <= GM_GP_AXISRV) {
            int a = axis - GM_GP_AXISLH;
            if (a >= 0 && a < 4) {
                return s_pads[device].axes[a];
            }
        }
    }
    return 0.0f;
}

float input_gamepad_hat_value(int device, int hat) {
    (void)hat;
    if (device >= 0 && device < MAX_GAMEPADS && s_pads[device].connected) {
        bool u = input_gamepad_button_check(device, GM_GP_PADU);
        bool r = input_gamepad_button_check(device, GM_GP_PADR);
        bool d = input_gamepad_button_check(device, GM_GP_PADD);
        bool l = input_gamepad_button_check(device, GM_GP_PADL);
        int val = 0;
        if (u) val |= 1;
        if (r) val |= 2;
        if (d) val |= 4;
        if (l) val |= 8;
        return (float)val;
    }
    return 0.0f;
}

/* -------------------------------------------------------------
 * Virtual Touch Controls (Android Overlay)
 * ------------------------------------------------------------- */
void input_virtual_touch_set_enabled(bool enabled) {
    s_virtual_touch_enabled = enabled;
}

bool input_virtual_touch_is_enabled(void) {
    return s_virtual_touch_enabled;
}

void input_virtual_touch_configure(float screen_width, float screen_height) {
    s_screen_w = screen_width;
    s_screen_h = screen_height;

    /* Analog stick positioned on bottom left */
    s_vstick.center_x = screen_width * 0.15f;
    s_vstick.center_y = screen_height * 0.75f;
    s_vstick.base_radius = screen_height * 0.18f;
    s_vstick.knob_radius = screen_height * 0.08f;
    s_vstick.current_x = s_vstick.center_x;
    s_vstick.current_y = s_vstick.center_y;
    s_vstick.is_active = false;
    s_vstick.touch_id = -1;

    /* Action buttons arranged comfortably on bottom right for mobile ergonomics */
    s_vbutton_count = 0;
    float btn_r = screen_height * 0.075f;

    /* 1. Jump Button (A) */
    s_vbuttons[s_vbutton_count++] = (VirtualButton){
        .x = screen_width * 0.85f,
        .y = screen_height * 0.82f,
        .radius = btn_r,
        .mapped_vk = GM_VK_SPACE,
        .mapped_gp = GM_GP_FACE1,
        .label = "JUMP",
        .is_pressed = false
    };

    /* 2. Attack Button (X / Z) */
    s_vbuttons[s_vbutton_count++] = (VirtualButton){
        .x = screen_width * 0.77f,
        .y = screen_height * 0.72f,
        .radius = btn_r,
        .mapped_vk = 'Z',
        .mapped_gp = GM_GP_FACE3,
        .label = "ATK",
        .is_pressed = false
    };

    /* 3. Spell / Heavy Button (Y / X) */
    s_vbuttons[s_vbutton_count++] = (VirtualButton){
        .x = screen_width * 0.85f,
        .y = screen_height * 0.62f,
        .radius = btn_r,
        .mapped_vk = 'X',
        .mapped_gp = GM_GP_FACE4,
        .label = "SPELL",
        .is_pressed = false
    };

    /* 4. Dash Button (B / C) */
    s_vbuttons[s_vbutton_count++] = (VirtualButton){
        .x = screen_width * 0.93f,
        .y = screen_height * 0.72f,
        .radius = btn_r,
        .mapped_vk = 'C',
        .mapped_gp = GM_GP_SHOULDERLB,
        .label = "DASH",
        .is_pressed = false
    };

    /* 5. Pulse / Parry Button (L / A) */
    s_vbuttons[s_vbutton_count++] = (VirtualButton){
        .x = screen_width * 0.93f,
        .y = screen_height * 0.52f,
        .radius = btn_r * 0.85f,
        .mapped_vk = 'A',
        .mapped_gp = GM_GP_SHOULDERL,
        .label = "PULSE",
        .is_pressed = false
    };

    /* 6. Pause / Menu Button (Start / ESC) */
    s_vbuttons[s_vbutton_count++] = (VirtualButton){
        .x = screen_width * 0.94f,
        .y = screen_height * 0.08f,
        .radius = btn_r * 0.70f,
        .mapped_vk = GM_VK_ESCAPE,
        .mapped_gp = GM_GP_START,
        .label = "PAUSE",
        .is_pressed = false
    };
}

void input_virtual_touch_process(void) {
    /* Reset button states */
    for (int b = 0; b < s_vbutton_count; b++) {
        s_vbuttons[b].is_pressed = false;
    }

    bool stick_touch_found = false;

    /* Check all active fingers against controls */
    for (int i = 0; i < MAX_TOUCH_FINGERS; i++) {
        if (!s_fingers[i].active) continue;
        float tx = s_fingers[i].raw_x;
        float ty = s_fingers[i].raw_y;

        /* Floating Dynamic Joystick on left half */
        if (tx < s_screen_w * 0.48f) {
            /* If stick is not yet active, dynamically anchor under player's thumb */
            if (!s_vstick.is_active && ty > s_screen_h * 0.20f) {
                s_vstick.center_x = tx;
                s_vstick.center_y = ty;
                s_vstick.current_x = tx;
                s_vstick.current_y = ty;
                s_vstick.is_active = true;
                s_vstick.touch_id = s_fingers[i].id;
            }

            float dx = tx - s_vstick.center_x;
            float dy = ty - s_vstick.center_y;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist <= s_vstick.base_radius * 2.0f || s_vstick.touch_id == s_fingers[i].id) {
                stick_touch_found = true;
                s_vstick.is_active = true;
                s_vstick.touch_id = s_fingers[i].id;

                /* Clamp knob to base radius */
                if (dist > s_vstick.base_radius) {
                    float factor = s_vstick.base_radius / dist;
                    dx *= factor;
                    dy *= factor;
                }
                s_vstick.current_x = s_vstick.center_x + dx;
                s_vstick.current_y = s_vstick.center_y + dy;

                /* Normalize axis [-1.0, 1.0] */
                float norm_x = dx / s_vstick.base_radius;
                float norm_y = dy / s_vstick.base_radius;
                input_gamepad_set_axis(0, GM_GP_AXISLH, norm_x);
                input_gamepad_set_axis(0, GM_GP_AXISLV, norm_y);

                /* Inject D-Pad and Keyboard keys with responsive 0.25f threshold */
                float threshold = 0.25f;
                input_keyboard_set(GM_VK_LEFT,  norm_x < -threshold);
                input_keyboard_set(GM_VK_RIGHT, norm_x > threshold);
                input_keyboard_set(GM_VK_UP,    norm_y < -threshold);
                input_keyboard_set(GM_VK_DOWN,  norm_y > threshold);

                input_gamepad_set_button(0, GM_GP_PADL, norm_x < -threshold);
                input_gamepad_set_button(0, GM_GP_PADR, norm_x > threshold);
                input_gamepad_set_button(0, GM_GP_PADU, norm_y < -threshold);
                input_gamepad_set_button(0, GM_GP_PADD, norm_y > threshold);
            }
        }

        /* Check action buttons on right half (only in gameplay) */
        if (!RoomManager_IsTitleRoom()) {
            for (int b = 0; b < s_vbutton_count; b++) {
                float bdx = tx - s_vbuttons[b].x;
                float bdy = ty - s_vbuttons[b].y;
                float bdist = sqrtf(bdx * bdx + bdy * bdy);
                if (bdist <= s_vbuttons[b].radius * 1.25f) {
                    s_vbuttons[b].is_pressed = true;
                }
            }
        }
    }

    if (!stick_touch_found) {
        s_vstick.is_active = false;
        s_vstick.touch_id = -1;
        s_vstick.center_x = s_screen_w * 0.15f;
        s_vstick.center_y = s_screen_h * 0.75f;
        s_vstick.current_x = s_vstick.center_x;
        s_vstick.current_y = s_vstick.center_y;
        input_gamepad_set_axis(0, GM_GP_AXISLH, 0.0f);
        input_gamepad_set_axis(0, GM_GP_AXISLV, 0.0f);
        input_keyboard_set(GM_VK_LEFT, false);
        input_keyboard_set(GM_VK_RIGHT, false);
        input_keyboard_set(GM_VK_UP, false);
        input_keyboard_set(GM_VK_DOWN, false);
        input_gamepad_set_button(0, GM_GP_PADL, false);
        input_gamepad_set_button(0, GM_GP_PADR, false);
        input_gamepad_set_button(0, GM_GP_PADU, false);
        input_gamepad_set_button(0, GM_GP_PADD, false);
    }

    /* Apply action button inputs */
    for (int b = 0; b < s_vbutton_count; b++) {
        input_keyboard_set(s_vbuttons[b].mapped_vk, s_vbuttons[b].is_pressed);
        input_gamepad_set_button(0, s_vbuttons[b].mapped_gp, s_vbuttons[b].is_pressed);
        if (s_vbuttons[b].mapped_gp == GM_GP_FACE1) {
            input_keyboard_set(GM_VK_ENTER, s_vbuttons[b].is_pressed);
        }
    }
}

void input_virtual_touch_draw(void) {
    if (!s_virtual_touch_enabled) return;

    /* Draw Joystick Base and Knob */
    uint32_t base_col = s_vstick.is_active ? 0x50ffffff : 0x22ffffff;
    uint32_t ring_col = s_vstick.is_active ? 0xa0ffffff : 0x45ffffff;
    uint32_t knob_col = s_vstick.is_active ? 0xccffffff : 0x60ffffff;

    draw_circle_color(s_vstick.center_x, s_vstick.center_y, s_vstick.base_radius, base_col, base_col, false);
    draw_circle_color(s_vstick.center_x, s_vstick.center_y, s_vstick.base_radius, ring_col, ring_col, true);
    draw_circle_color(s_vstick.current_x, s_vstick.current_y, s_vstick.knob_radius, knob_col, knob_col, false);
    draw_circle_color(s_vstick.current_x, s_vstick.current_y, s_vstick.knob_radius, 0xffffffff, 0xffffffff, true);

    /* Draw D-Pad Directional Indicators */
    float arrow_dist = s_vstick.base_radius * 0.72f;
    draw_text_transformed_color(s_vstick.center_x - 6.0, s_vstick.center_y - arrow_dist - 10.0, "^", 2.0, 2.0, 0.0,
                                ring_col, ring_col, ring_col, ring_col, 0.75);
    draw_text_transformed_color(s_vstick.center_x - 6.0, s_vstick.center_y + arrow_dist - 14.0, "v", 2.0, 2.0, 0.0,
                                ring_col, ring_col, ring_col, ring_col, 0.75);
    draw_text_transformed_color(s_vstick.center_x - arrow_dist - 12.0, s_vstick.center_y - 12.0, "<", 2.0, 2.0, 0.0,
                                ring_col, ring_col, ring_col, ring_col, 0.75);
    draw_text_transformed_color(s_vstick.center_x + arrow_dist - 6.0, s_vstick.center_y - 12.0, ">", 2.0, 2.0, 0.0,
                                ring_col, ring_col, ring_col, ring_col, 0.75);

    /* Draw Action Buttons with Labels (only in gameplay rooms) */
    if (!RoomManager_IsTitleRoom()) {
        float scale = 2.0f;
        for (int b = 0; b < s_vbutton_count; b++) {
            uint32_t col_inner = s_vbuttons[b].is_pressed ? 0x904080ff : 0x25ffffff;
            uint32_t col_border = s_vbuttons[b].is_pressed ? 0xffffffff : 0x60ffffff;
            draw_circle_color(s_vbuttons[b].x, s_vbuttons[b].y, s_vbuttons[b].radius, col_inner, col_inner, false);
            draw_circle_color(s_vbuttons[b].x, s_vbuttons[b].y, s_vbuttons[b].radius, col_border, col_border, true);

            if (s_vbuttons[b].label) {
                float tw = string_width(s_vbuttons[b].label) * scale;
                float th = string_height(s_vbuttons[b].label) * scale;
                float tx = s_vbuttons[b].x - (tw * 0.5f);
                float ty = s_vbuttons[b].y - (th * 0.5f);
                uint32_t txt_col = s_vbuttons[b].is_pressed ? 0x00FFFFFF : 0x00E0E0E0;
                draw_text_transformed_color((double)tx, (double)ty, s_vbuttons[b].label,
                                            scale, scale, 0.0, txt_col, txt_col, txt_col, txt_col, 0.95);
            }
        }
    }
}

/* -------------------------------------------------------------
 * Binary / Engine Compatibility
 * ------------------------------------------------------------- */
uint64_t FUN_14192ec30(uint32_t param_1) {
    if (param_1 < 256) {
        return (uint64_t)DAT_142613940[param_1];
    }
    return 0;
}

uint8_t FUN_14188b3e0(uint32_t param_1) {
    return input_keyboard_check((int)param_1) ? 1 : 0;
}
