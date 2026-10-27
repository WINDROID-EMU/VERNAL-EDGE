#include "room.h"
#include "graphics.h"
#include "data_win.h"
#include "input.h"
#include "runner.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define MAX_ROOMS 1024

static RoomDef s_rooms[MAX_ROOMS];
static int32_t s_current_room = -1;
static int32_t s_target_room = -1;

void RoomManager_Init(void) {
    memset(s_rooms, 0, sizeof(s_rooms));
    s_current_room = -1;
    s_target_room = -1;
}

void RoomManager_Register(int32_t room_id, const char* name, int32_t width, int32_t height, RoomCreateFunc create_code) {
    if (room_id >= 0 && room_id < MAX_ROOMS) {
        s_rooms[room_id].id = room_id;
        s_rooms[room_id].name = name;
        s_rooms[room_id].width = width > 0 ? width : 1280;
        s_rooms[room_id].height = height > 0 ? height : 720;
        s_rooms[room_id].speed = 60.0;
        s_rooms[room_id].create_code = create_code;
    }
}

void room_goto(int32_t room_id) {
    if (room_id >= 0 && room_id < MAX_ROOMS) {
        s_target_room = room_id;
    }
}

void room_restart(void) {
    s_target_room = s_current_room;
}

int32_t room_get_current(void) {
    return s_current_room;
}

int32_t room_get_width(void) {
    if (s_current_room >= 0 && (uint32_t)s_current_room < g_DataWin.room_count) {
        return (int32_t)g_DataWin.rooms[s_current_room].width;
    }
    if (s_current_room >= 0 && s_current_room < MAX_ROOMS && s_rooms[s_current_room].width > 0) {
        return s_rooms[s_current_room].width;
    }
    return 480;
}

int32_t room_get_height(void) {
    if (s_current_room >= 0 && (uint32_t)s_current_room < g_DataWin.room_count) {
        return (int32_t)g_DataWin.rooms[s_current_room].height;
    }
    if (s_current_room >= 0 && s_current_room < MAX_ROOMS && s_rooms[s_current_room].height > 0) {
        return s_rooms[s_current_room].height;
    }
    return 270;
}

double room_get_speed(void) {
    if (s_current_room >= 0 && s_current_room < MAX_ROOMS) return s_rooms[s_current_room].speed;
    return 60.0;
}

bool RoomManager_IsTitleRoom(void) {
    return (s_current_room >= 0 && (uint32_t)s_current_room < g_DataWin.room_count &&
            strstr(g_DataWin.rooms[s_current_room].name, "title") != NULL);
}

static void PerformRoomSwitch(void) {
    if (s_target_room < 0 || s_target_room >= MAX_ROOMS) return;

    // Destroy non-persistent instances
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        CInstance* inst = Instance_GetById(INSTANCE_BASE_ID + i);
        if (inst && inst->active && !inst->persistent) {
            instance_destroy(inst->id);
        }
    }

    s_current_room = s_target_room;
    s_target_room = -1;

    // Set camera view for the room
    int rw = room_get_width();
    int rh = room_get_height();
    camera_set_view_pos(0, 0);
    camera_set_view_size((float)rw, (float)rh);

    // Spawn instances defined in data.win
    if (g_DataWin.room_count > 0 && (uint32_t)s_current_room < g_DataWin.room_count) {
        DataWinRoomDef *dw_r = &g_DataWin.rooms[s_current_room];
        printf("[Room] Switching to '%s' (%ux%u, spawning %u instances)\n",
               dw_r->name, dw_r->width, dw_r->height, dw_r->instance_count);

        for (uint32_t i = 0; i < dw_r->instance_count; i++) {
            RoomInstDef *ri = &dw_r->instances[i];
            CInstance *inst = instance_create_depth((double)ri->x, (double)ri->y, (double)ri->depth, ri->object_index);
            if (inst) {
                inst->id = ri->instance_id;
                inst->image_xscale = (double)ri->scale_x;
                inst->image_yscale = (double)ri->scale_y;
                inst->image_angle  = (double)ri->rotation;
                inst->image_blend  = (ri->color == 0 || ri->color == 0x3F800000) ? 0xFFFFFFFF : ri->color;
                inst->depth        = (double)ri->depth;
                if ((uint32_t)ri->object_index < g_DataWin.object_count) {
                    GameObjectDef *god = &g_DataWin.objects[ri->object_index];
                    inst->sprite_index = god->sprite_index;
                }
            }
        }
    }

    // Run Room Creation Code if registered
    if (s_rooms[s_current_room].create_code) {
        s_rooms[s_current_room].create_code();
    }
}

// -------------------------------------------------------------
// Step Execution Order (GameMaker Specification)
// -------------------------------------------------------------
void RoomManager_Step(void) {
    if (s_target_room != -1) {
        PerformRoomSwitch();
    }

    // 1. Begin Step
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        CInstance* inst = Instance_GetById(INSTANCE_BASE_ID + i);
        if (inst && inst->active && !inst->marked_for_destroy) {
            CObjectDef* obj = Object_Get(inst->object_index);
            if (obj && obj->ev_step_begin) obj->ev_step_begin(inst, NULL);
        }
    }

    // 2. Alarms
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        CInstance* inst = Instance_GetById(INSTANCE_BASE_ID + i);
        if (inst && inst->active && !inst->marked_for_destroy) {
            CObjectDef* obj = Object_Get(inst->object_index);
            for (int a = 0; a < MAX_ALARMS; a++) {
                if (inst->alarm[a] > 0) {
                    inst->alarm[a]--;
                    if (inst->alarm[a] == 0) {
                        if (obj && obj->ev_alarm[a]) obj->ev_alarm[a](inst, NULL);
                    }
                }
            }
        }
    }

    // 3. Step Event
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        CInstance* inst = Instance_GetById(INSTANCE_BASE_ID + i);
        if (inst && inst->active && !inst->marked_for_destroy) {
            CObjectDef* obj = Object_Get(inst->object_index);
            if (obj && obj->ev_step) obj->ev_step(inst, NULL);
        }
    }

    // 4. Movement / Physics update
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        CInstance* inst = Instance_GetById(INSTANCE_BASE_ID + i);
        if (inst && inst->active && !inst->marked_for_destroy) {
            inst->xprevious = inst->x;
            inst->yprevious = inst->y;
            inst->x += inst->hspeed;
            inst->y += inst->vspeed;

            // Advance animation
            if (inst->image_speed != 0.0) {
                inst->image_index += inst->image_speed;
            }

            // Player control & Camera follow for o_vernal in gameplay rooms
            bool is_player = (inst->object_index == 304 || (inst->sprite_index >= 14 && inst->sprite_index <= 30));
            if (is_player) {
                float move_h = 0.0f;
                if (input_keyboard_check(GM_VK_LEFT) || input_gamepad_button_check(0, GM_GP_PADL)) move_h -= 1.0f;
                if (input_keyboard_check(GM_VK_RIGHT) || input_gamepad_button_check(0, GM_GP_PADR)) move_h += 1.0f;
                float stick_h = input_gamepad_axis_value(0, GM_GP_AXISLH);
                if (fabsf(stick_h) > 0.25f) move_h = stick_h;

                if (move_h != 0.0f) {
                    inst->x += (double)(move_h * 3.5f);
                    inst->image_xscale = (move_h > 0.0f) ? 1.0 : -1.0;
                }

                int rw = room_get_width();
                int rh = room_get_height();
                bool is_title = (s_current_room >= 0 && (uint32_t)s_current_room < g_DataWin.room_count &&
                                 strstr(g_DataWin.rooms[s_current_room].name, "title") != NULL);
                if (!is_title) {
                    float target_cx = (float)inst->x - (float)rw * 0.5f;
                    float target_cy = (float)inst->y - (float)rh * 0.5f;
                    int r_w = (s_current_room >= 0 && (uint32_t)s_current_room < g_DataWin.room_count) ? (int)g_DataWin.rooms[s_current_room].width : rw;
                    int r_h = (s_current_room >= 0 && (uint32_t)s_current_room < g_DataWin.room_count) ? (int)g_DataWin.rooms[s_current_room].height : rh;
                    if (target_cx < 0) target_cx = 0;
                    if (target_cy < 0) target_cy = 0;
                    if (target_cx > (float)(r_w - rw)) target_cx = (float)(r_w - rw);
                    if (target_cy > (float)(r_h - rh)) target_cy = (float)(r_h - rh);
                    camera_set_view_pos(target_cx, target_cy);
                }
            }
        }
    }

    // 5. End Step
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        CInstance* inst = Instance_GetById(INSTANCE_BASE_ID + i);
        if (inst && inst->active && !inst->marked_for_destroy) {
            CObjectDef* obj = Object_Get(inst->object_index);
            if (obj && obj->ev_step_end) obj->ev_step_end(inst, NULL);
        }
    }

    // 6. Process Destroy / Cleanup
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        CInstance* inst = Instance_GetById(INSTANCE_BASE_ID + i);
        if (inst && inst->active && inst->marked_for_destroy) {
            CObjectDef* obj = Object_Get(inst->object_index);
            if (obj && obj->ev_destroy) obj->ev_destroy(inst, NULL);
            if (obj && obj->ev_cleanup) obj->ev_cleanup(inst, NULL);
            inst->active = false;
            inst->marked_for_destroy = false;
        }
    }
}

// -------------------------------------------------------------
// Draw Pipeline
// -------------------------------------------------------------
typedef struct {
    int type; // 0 = instance, 1 = layer
    int32_t depth;
    int32_t id;
    void *ptr;
} RenderItem;

static int render_item_comparator(const void *a, const void *b) {
    const RenderItem *ia = (const RenderItem *)a;
    const RenderItem *ib = (const RenderItem *)b;
    if (ia->depth > ib->depth) return -1;
    if (ia->depth < ib->depth) return 1;
    return ia->id - ib->id;
}

void RoomManager_Draw(void) {
    static RenderItem render_items[MAX_INSTANCES + 128];
    int32_t item_count = 0;

    graphics_begin_frame();

    // Set camera / room projection
    float cx = 0, cy = 0;
    camera_get_view_pos(&cx, &cy);
    int rw = room_get_width();
    int rh = room_get_height();
    graphics_set_projection(cx, cx + (float)rw, cy + (float)rh, cy);

    // Clear background
    uint32_t bg_col = 0xd89858; // Vernal Edge sky blue
    if (g_DataWin.room_count > 0 && s_current_room >= 0 && (uint32_t)s_current_room < g_DataWin.room_count) {
        if (g_DataWin.rooms[s_current_room].bg_color != 0) {
            bg_col = g_DataWin.rooms[s_current_room].bg_color;
        }
    }
    draw_clear_alpha(bg_col, 1.0);

    // 1. Gather all active visible instances
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        CInstance* inst = Instance_GetById(INSTANCE_BASE_ID + i);
        if (inst && inst->active && inst->visible && !inst->marked_for_destroy) {
            if (item_count < (int32_t)(sizeof(render_items) / sizeof(render_items[0]))) {
                render_items[item_count].type = 0;
                render_items[item_count].depth = (int32_t)inst->depth;
                render_items[item_count].id = inst->id;
                render_items[item_count].ptr = inst;
                item_count++;
            }
        }
    }

    // 2. Gather room layers (Backgrounds, Tilemaps, Asset sprites)
    if (g_DataWin.room_count > 0 && s_current_room >= 0 && (uint32_t)s_current_room < g_DataWin.room_count) {
        DataWinRoomDef *dw_r = &g_DataWin.rooms[s_current_room];
        for (uint32_t l = 0; l < dw_r->layer_count; l++) {
            RoomLayerDef *lay = &dw_r->layers[l];
            // Respect layer visibility flag
            if (!lay->visible) continue;
            // Skip instance layers (instances already added)
            if (lay->type == LAYER_TYPE_INSTANCES) continue;
            // Skip invisible collision tile layers
            if (lay->type == LAYER_TYPE_TILES && (strstr(lay->name, "collision") || strstr(lay->name, "Collision"))) continue;

            if (item_count < (int32_t)(sizeof(render_items) / sizeof(render_items[0]))) {
                render_items[item_count].type = 1;
                render_items[item_count].depth = lay->depth;
                render_items[item_count].id = (int32_t)lay->layer_id;
                render_items[item_count].ptr = lay;
                item_count++;
            }
        }
    }

    // 3. Sort all renderables by depth descending (background -> foreground)
    if (item_count > 1) {
        qsort(render_items, item_count, sizeof(RenderItem), render_item_comparator);
    }

    // 4. Render layers and instances in depth order
    for (int32_t i = 0; i < item_count; i++) {
        RenderItem *item = &render_items[i];
        if (item->type == 1) {
            // Room Layer
            RoomLayerDef *lay = (RoomLayerDef *)item->ptr;
            if (lay->type == LAYER_TYPE_BACKGROUND) {
                if (lay->bg_sprite_index >= 0) {
                    Sprite *spr = data_win_get_sprite(&g_DataWin, lay->bg_sprite_index);
                    if (spr && spr->width > 0 && spr->height > 0) {
                        lay->x_offset += lay->hspeed * (1.0f / 60.0f);
                        lay->y_offset += lay->vspeed * (1.0f / 60.0f);
                        if (lay->bg_tiled_h) {
                            float sw = (float)spr->width;
                            float start_x = fmodf(lay->x_offset, sw);
                            if (start_x > 0.0f) start_x -= sw;
                            for (float bx = start_x; bx < (float)rw + sw; bx += sw) {
                                draw_sprite_ext(lay->bg_sprite_index, 0.0f, (double)bx, (double)lay->y_offset,
                                                1.0, 1.0, 0.0, 0xFFFFFFFF, 1.0);
                            }
                        } else {
                            float dx = lay->x_offset;
                            float dy = lay->y_offset;
                            // Clean placement for title logo on Backgrounds_4 layer
                            if (strstr(lay->name, "Backgrounds_4") || (spr->name && strstr(spr->name, "title"))) {
                                dx = 24.0f;
                                dy = 16.0f;
                            }
                            draw_sprite_ext(lay->bg_sprite_index, 0.0f, (double)dx, (double)dy,
                                            1.0, 1.0, 0.0, 0xFFFFFFFF, 1.0);
                        }
                    }
                } else if (lay->bg_color != 0 && lay->depth >= 1900) {
                    draw_rectangle_color(0.0, 0.0, (double)rw, (double)rh,
                                         lay->bg_color, lay->bg_color, lay->bg_color, lay->bg_color, false);
                }
            } else if (lay->type == LAYER_TYPE_TILES) {
                if (lay->tileset_index >= 0 && lay->tile_data) {
                    TilesetDef *ts = data_win_get_tileset(&g_DataWin, lay->tileset_index);
                    if (ts) {
                        for (uint32_t ty = 0; ty < lay->tiles_y; ty++) {
                            for (uint32_t tx = 0; tx < lay->tiles_x; tx++) {
                                uint32_t t_raw = lay->tile_data[ty * lay->tiles_x + tx];
                                if ((t_raw & 0x07ffffff) > 0) {
                                    float px = (float)tx * (float)ts->tile_width + lay->x_offset;
                                    float py = (float)ty * (float)ts->tile_height + lay->y_offset;
                                    graphics_draw_tile(lay->tileset_index, t_raw, px, py);
                                }
                            }
                        }
                    }
                }
            } else if (lay->type == LAYER_TYPE_ASSETS) {
                for (uint32_t s = 0; s < lay->asset_sprite_count; s++) {
                    RoomAssetSprite *as = &lay->asset_sprites[s];
                    uint32_t col = (as->color == 0 || as->color == 0x3F800000) ? 0xFFFFFFFF : as->color;
                    draw_sprite_ext(as->sprite_index, as->frame_index,
                                    (double)as->x + (double)lay->x_offset, (double)as->y + (double)lay->y_offset,
                                    (double)as->scale_x, (double)as->scale_y, (double)as->rotation, col, 1.0);
                }
            }
        } else {
            // Instance
            CInstance* inst = (CInstance *)item->ptr;
            CObjectDef* obj = Object_Get(inst->object_index);
            if (obj && obj->ev_draw) {
                obj->ev_draw(inst, NULL);
            } else if (inst->sprite_index >= 0) {
                draw_sprite_ext(inst->sprite_index, (float)inst->image_index,
                                inst->x, inst->y, inst->image_xscale, inst->image_yscale,
                                inst->image_angle, inst->image_blend, inst->image_alpha);
            }
        }
    }

    // 5. Title screen overlay: Interactive Menu & Options
    bool is_title_room = (s_current_room >= 0 && (uint32_t)s_current_room < g_DataWin.room_count &&
                          strstr(g_DataWin.rooms[s_current_room].name, "title") != NULL);
    if (is_title_room) {
        static const char *menu_options[4] = { "Continue", "New Game", "Settings", "Quit" };
        static int s_menu_sel = 1; // Default to 'New Game'
        static int s_debounce = 0;
        if (s_debounce > 0) s_debounce--;

        bool nav_up = input_keyboard_check(GM_VK_UP) || input_keyboard_check('W') ||
                      input_gamepad_button_check(0, GM_GP_PADU) ||
                      (input_gamepad_axis_value(0, GM_GP_AXISLV) < -0.35f);

        bool nav_down = input_keyboard_check(GM_VK_DOWN) || input_keyboard_check('S') ||
                        input_gamepad_button_check(0, GM_GP_PADD) ||
                        (input_gamepad_axis_value(0, GM_GP_AXISLV) > 0.35f);

        bool btn_confirm = input_keyboard_check_pressed(GM_VK_SPACE) ||
                           input_keyboard_check_pressed(GM_VK_ENTER) ||
                           input_keyboard_check_pressed('Z') ||
                           input_gamepad_button_check_pressed(0, GM_GP_FACE1) ||
                           input_gamepad_button_check_pressed(0, GM_GP_FACE3) ||
                           input_gamepad_button_check_pressed(0, GM_GP_START);

        if (s_debounce == 0) {
            if (nav_up) {
                s_menu_sel = (s_menu_sel - 1 + 4) % 4;
                s_debounce = 12;
            } else if (nav_down) {
                s_menu_sel = (s_menu_sel + 1) % 4;
                s_debounce = 12;
            }
        }

        float opt_ys[4] = { 160.0f, 184.0f, 208.0f, 232.0f };

        // Direct Touch detection on menu options
        const RunnerConfig *cfg = Runner_GetConfig();
        float win_w = (cfg && cfg->window_width > 0) ? (float)cfg->window_width : 1280.0f;
        float win_h = (cfg && cfg->window_height > 0) ? (float)cfg->window_height : 720.0f;

        for (int f = 0; f < MAX_TOUCH_FINGERS; f++) {
            if (input_device_mouse_check_button_pressed(f, GM_MB_LEFT)) {
                float mx = input_device_mouse_raw_x(f);
                float my = input_device_mouse_raw_y(f);
                float rx = mx * ((float)rw / win_w);
                float ry = my * ((float)rh / win_h);

                if (rx >= 260.0f && rx <= 480.0f) {
                    for (int o = 0; o < 4; o++) {
                        float oy = opt_ys[o];
                        if (ry >= oy - 4.0f && ry <= oy + 26.0f) {
                            if (s_menu_sel == o) {
                                btn_confirm = true;
                            } else {
                                s_menu_sel = o;
                            }
                            s_debounce = 12;
                            break;
                        }
                    }
                }
            }
        }

        // Handle Confirmation / Action
        if (btn_confirm) {
            printf("[Menu] Confirmed selection %d: '%s'\n", s_menu_sel, menu_options[s_menu_sel]);
            if (s_menu_sel == 0 || s_menu_sel == 1) {
                // Enter opening gameplay room 'rm_foggy_entrance'
                int32_t play_room = data_win_find_room_by_name(&g_DataWin, "rm_foggy_entrance");
                if (play_room < 0) play_room = 8;
                if (play_room >= 0 && (uint32_t)play_room < g_DataWin.room_count) {
                    printf("[Menu] Transitioning to room %d ('%s')\n", play_room, g_DataWin.rooms[play_room].name);
                    room_goto(play_room);
                }
            }
        }

        // Render option highlights and text on the menu bars
        for (int o = 0; o < 4; o++) {
            float oy = opt_ys[o];
            bool sel = (o == s_menu_sel);
            uint32_t txt_col = sel ? 0x00FFFFFF : 0x00A0A0A0;
            if (sel) {
                draw_rectangle_color(318.0, oy + 2.0, 460.0, oy + 22.0,
                                     0x4000D8FF, 0x4000D8FF, 0x4000D8FF, 0x4000D8FF, false);
                draw_rectangle_color(318.0, oy + 2.0, 460.0, oy + 22.0,
                                     0xD000D8FF, 0xD000D8FF, 0xD000D8FF, 0xD000D8FF, true);
                draw_text_transformed_color(320.0 + 8.0, oy + 4.0, ">", 1.0, 1.0, 0.0,
                                            0x0000D8FF, 0x0000D8FF, 0x0000D8FF, 0x0000D8FF, 1.0);
            }
            draw_text_transformed_color(320.0 + 26.0, oy + 4.0, menu_options[o], 1.0, 1.0, 0.0,
                                        txt_col, txt_col, txt_col, txt_col, 1.0);
        }
    }

    // 6. Draw GUI Event (64)
    for (int32_t i = 0; i < MAX_INSTANCES; i++) {
        CInstance* inst = Instance_GetById(INSTANCE_BASE_ID + i);
        if (inst && inst->active && inst->visible && !inst->marked_for_destroy) {
            CObjectDef* obj = Object_Get(inst->object_index);
            if (obj && obj->ev_draw_gui) {
                obj->ev_draw_gui(inst, NULL);
            }
        }
    }

    graphics_end_frame();
}
