#ifndef RUNNER_INPUT_H
#define RUNNER_INPUT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* GameMaker Virtual Key Codes */
#define GM_VK_NOKEY       0
#define GM_VK_ANYKEY      1
#define GM_VK_BACKSPACE   8
#define GM_VK_TAB         9
#define GM_VK_ENTER       13
#define GM_VK_RETURN      13
#define GM_VK_SHIFT       16
#define GM_VK_CONTROL     17
#define GM_VK_ALT         18
#define GM_VK_PAUSE       19
#define GM_VK_ESCAPE      27
#define GM_VK_SPACE       32
#define GM_VK_PAGEUP      33
#define GM_VK_PAGEDOWN    34
#define GM_VK_END         35
#define GM_VK_HOME        36
#define GM_VK_LEFT        37
#define GM_VK_UP          38
#define GM_VK_RIGHT       39
#define GM_VK_DOWN        40
#define GM_VK_PRINTSCREEN 44
#define GM_VK_INSERT      45
#define GM_VK_DELETE      46
#define GM_VK_NUMPAD0     96
#define GM_VK_NUMPAD1     97
#define GM_VK_NUMPAD2     98
#define GM_VK_NUMPAD3     99
#define GM_VK_NUMPAD4     100
#define GM_VK_NUMPAD5     101
#define GM_VK_NUMPAD6     102
#define GM_VK_NUMPAD7     103
#define GM_VK_NUMPAD8     104
#define GM_VK_NUMPAD9     105
#define GM_VK_MULTIPLY    106
#define GM_VK_ADD         107
#define GM_VK_SUBTRACT    109
#define GM_VK_DECIMAL     110
#define GM_VK_DIVIDE      111
#define GM_VK_F1          112
#define GM_VK_F2          113
#define GM_VK_F3          114
#define GM_VK_F4          115
#define GM_VK_F5          116
#define GM_VK_F6          117
#define GM_VK_F7          118
#define GM_VK_F8          119
#define GM_VK_F9          120
#define GM_VK_F10         121
#define GM_VK_F11         122
#define GM_VK_F12         123
#define GM_VK_LSHIFT      160
#define GM_VK_RSHIFT      161
#define GM_VK_LCONTROL    162
#define GM_VK_RCONTROL    163
#define GM_VK_LALT        164
#define GM_VK_RALT        165

/* GameMaker Gamepad Constants */
#define GM_GP_FACE1      32769 /* A / Cross */
#define GM_GP_FACE2      32770 /* B / Circle */
#define GM_GP_FACE3      32771 /* X / Square */
#define GM_GP_FACE4      32772 /* Y / Triangle */
#define GM_GP_SHOULDERL  32773 /* LB / L1 */
#define GM_GP_SHOULDERR  32774 /* RB / R1 */
#define GM_GP_SHOULDERLB 32775 /* LT / L2 */
#define GM_GP_SHOULDERRB 32776 /* RT / R2 */
#define GM_GP_SELECT     32777 /* Select / Back */
#define GM_GP_START      32778 /* Start / Menu */
#define GM_GP_STICKL     32779 /* Left Stick click */
#define GM_GP_STICKR     32780 /* Right Stick click */
#define GM_GP_PADU       32781 /* Dpad Up */
#define GM_GP_PADD       32782 /* Dpad Down */
#define GM_GP_PADL       32783 /* Dpad Left */
#define GM_GP_PADR       32784 /* Dpad Right */
#define GM_GP_AXISLH     32785 /* Left Stick H (-1.0 to 1.0) */
#define GM_GP_AXISLV     32786 /* Left Stick V (-1.0 to 1.0) */
#define GM_GP_AXISRH     32787 /* Right Stick H (-1.0 to 1.0) */
#define GM_GP_AXISRV     32788 /* Right Stick V (-1.0 to 1.0) */

/* Mouse Constants */
#define GM_MB_NONE       0
#define GM_MB_ANY       -1
#define GM_MB_LEFT       1
#define GM_MB_RIGHT      2
#define GM_MB_MIDDLE     3

#define MAX_TOUCH_FINGERS 10
#define MAX_GAMEPADS      4

/* System Lifecycle */
void input_init(void);
void input_shutdown(void);
void input_begin_frame(void);

/* Keyboard API */
void input_keyboard_set(int vk_code, bool down);
bool input_keyboard_check(int vk_code);
bool input_keyboard_check_pressed(int vk_code);
bool input_keyboard_check_released(int vk_code);
void input_keyboard_clear(int vk_code);
int  input_keyboard_get_lastkey(void);

/* Mouse & Multi-touch API */
void input_mouse_set_pos(float raw_x, float raw_y);
void input_mouse_set_button(int button, bool down);
void input_mouse_set_wheel(int delta);
bool input_mouse_check_button(int button);
bool input_mouse_check_button_pressed(int button);
bool input_mouse_check_button_released(int button);
float input_mouse_get_x(void);
float input_mouse_get_y(void);
int  input_mouse_get_wheel_delta(void);

void input_touch_down(int finger_id, float raw_x, float raw_y);
void input_touch_move(int finger_id, float raw_x, float raw_y);
void input_touch_up(int finger_id);
bool input_device_mouse_check_button(int device, int button);
bool input_device_mouse_check_button_pressed(int device, int button);
bool input_device_mouse_check_button_released(int device, int button);
float input_device_mouse_raw_x(int device);
float input_device_mouse_raw_y(int device);
float input_device_mouse_x(int device);
float input_device_mouse_y(int device);

/* Gamepad API */
int  input_gamepad_get_device_count(void);
bool input_gamepad_is_connected(int device);
void input_gamepad_set_connected(int device, bool connected, const char* desc);
const char* input_gamepad_get_description(int device);
void input_gamepad_set_button(int device, int btn, bool down);
void input_gamepad_set_axis(int device, int axis, float value);
bool input_gamepad_button_check(int device, int btn);
bool input_gamepad_button_check_pressed(int device, int btn);
bool input_gamepad_button_check_released(int device, int btn);
float input_gamepad_axis_value(int device, int axis);
float input_gamepad_hat_value(int device, int hat);

/* On-Screen Virtual Touch Controls (Android Overlay) */
typedef struct {
    float x, y, radius;
    int mapped_vk;
    int mapped_gp;
    const char *label;
    bool is_pressed;
} VirtualButton;

typedef struct {
    float center_x, center_y;
    float base_radius, knob_radius;
    float current_x, current_y;
    bool is_active;
    int touch_id;
} VirtualStick;

void input_virtual_touch_set_enabled(bool enabled);
bool input_virtual_touch_is_enabled(void);
void input_virtual_touch_configure(float screen_width, float screen_height);
void input_virtual_touch_process(void);
void input_virtual_touch_draw(void);

/* Binary / Engine Compatibility symbols */
extern uint8_t DAT_142613940[256];
uint64_t FUN_14192ec30(uint32_t param_1);
uint8_t  FUN_14188b3e0(uint32_t param_1);

#ifdef __cplusplus
}
#endif

#endif /* RUNNER_INPUT_H */
