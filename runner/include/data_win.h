#ifndef DATA_WIN_H
#define DATA_WIN_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint16_t target_x;
    uint16_t target_y;
    uint16_t target_w;
    uint16_t target_h;
    uint16_t bound_w;
    uint16_t bound_h;
    uint16_t tex_id;
} TPAGItem;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t data_offset;
    uint32_t bz_len;
    uint32_t gl_texture_id;
    uint8_t *rgba_pixels;
} TexturePage;

typedef struct {
    const char *name;
    uint32_t width;
    uint32_t height;
    int32_t origin_x;
    int32_t origin_y;
    uint32_t subimage_count;
    uint32_t *tpag_offsets;
    float playback_speed;
} Sprite;

typedef struct {
    const char *name;
    int32_t sprite_index;
    int32_t parent_index;
    int32_t mask_index;
    int32_t depth;
    uint8_t visible;
    uint8_t solid;
    uint8_t persistent;
} GameObjectDef;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t object_index;
    int32_t instance_id;
    int32_t creation_code_id;
    float scale_x;
    float scale_y;
    float rotation;
    uint32_t color;
    int32_t depth;
} RoomInstDef;

typedef struct {
    const char *name;
    uint32_t tpag_offset;
    uint32_t tile_width;
    uint32_t tile_height;
    uint32_t border_x;
    uint32_t border_y;
    uint32_t tile_columns;
    uint32_t tile_count;
} TilesetDef;

typedef struct {
    uint16_t character;
    uint16_t src_x;
    uint16_t src_y;
    uint16_t src_w;
    uint16_t src_h;
    int16_t shift;
    int16_t offset;
} FontGlyph;

typedef struct {
    const char *name;
    uint32_t tpag_offset;
    float scale_x;
    float scale_y;
    uint32_t glyph_count;
    FontGlyph *glyphs;
} FontDef;

typedef enum {
    LAYER_TYPE_BACKGROUND = 1,
    LAYER_TYPE_INSTANCES  = 2,
    LAYER_TYPE_ASSETS     = 3,
    LAYER_TYPE_TILES      = 4,
    LAYER_TYPE_EFFECT     = 5
} RoomLayerType;

typedef struct {
    const char *name;
    int32_t sprite_index;
    int32_t x;
    int32_t y;
    float scale_x;
    float scale_y;
    uint32_t color;
    float rotation;
    float frame_index;
} RoomAssetSprite;

typedef struct {
    const char *name;
    uint32_t layer_id;
    RoomLayerType type;
    int32_t depth;
    float x_offset;
    float y_offset;
    float hspeed;
    float vspeed;
    uint8_t visible;

    // Type 1: Background
    int32_t bg_sprite_index;
    uint8_t bg_tiled_h;
    uint8_t bg_tiled_v;
    uint8_t bg_stretch;
    uint32_t bg_color;

    // Type 3: Assets
    uint32_t asset_sprite_count;
    RoomAssetSprite *asset_sprites;

    // Type 4: Tiles
    int32_t tileset_index;
    uint32_t tiles_x;
    uint32_t tiles_y;
    uint32_t *tile_data;
} RoomLayerDef;

typedef struct {
    const char *name;
    uint32_t width;
    uint32_t height;
    uint32_t speed;
    uint8_t persistent;
    uint32_t bg_color;
    uint32_t instance_count;
    RoomInstDef *instances;
    uint32_t layer_count;
    RoomLayerDef *layers;
} DataWinRoomDef;

typedef struct {
    const char *name;
    const char *vertex_code;
    const char *pixel_code;
    uint32_t gl_program;
} ShaderDef;

typedef struct {
    const char *name;
    const char *file_name;
    uint32_t flags;
    float volume;
    float pitch;
    int32_t group_id;
    int32_t audio_index;
    bool is_streamed;
} SoundDef;

typedef struct {
    uint32_t file_offset;
    uint32_t data_len;
    const uint8_t *riff_data;
} EmbeddedAudio;

typedef struct {
    uint8_t *file_data;
    size_t file_size;

    uint32_t texture_count;
    TexturePage *textures;

    uint32_t tpag_count;
    TPAGItem *tpags;

    uint32_t sprite_count;
    Sprite *sprites;

    uint32_t object_count;
    GameObjectDef *objects;

    uint32_t room_count;
    DataWinRoomDef *rooms;

    uint32_t shader_count;
    ShaderDef *shaders;

    uint32_t sound_count;
    SoundDef *sounds;

    uint32_t audio_count;
    EmbeddedAudio *audio_entries;

    uint32_t tileset_count;
    TilesetDef *tilesets;

    uint32_t font_count;
    FontDef *fonts;

    int32_t starting_room_index;
} DataWin;

/* Global instance of data.win */
extern DataWin g_DataWin;

/* Lifecycle */
bool data_win_load(DataWin *dw, const char *filepath);
void data_win_free(DataWin *dw);

/* Accessors */
const char *data_win_get_string(const DataWin *dw, uint32_t offset);
uint8_t *data_win_decode_texture(DataWin *dw, uint32_t tex_idx);
Sprite *data_win_get_sprite(DataWin *dw, int32_t sprite_idx);
GameObjectDef *data_win_get_object(DataWin *dw, int32_t obj_idx);
DataWinRoomDef *data_win_get_room(DataWin *dw, int32_t room_idx);
SoundDef *data_win_get_sound(DataWin *dw, int32_t sound_idx);
TilesetDef *data_win_get_tileset(DataWin *dw, int32_t tileset_idx);
FontDef *data_win_get_font(DataWin *dw, int32_t font_idx);
TPAGItem data_win_read_tpag(const DataWin *dw, uint32_t off);

int32_t data_win_find_room_by_name(DataWin *dw, const char *name);
int32_t data_win_find_object_by_name(DataWin *dw, const char *name);
int32_t data_win_find_sprite_by_name(DataWin *dw, const char *name);
int32_t data_win_find_sound_by_name(DataWin *dw, const char *name);
int32_t data_win_find_font_by_name(DataWin *dw, const char *name);

const uint8_t *data_win_get_sound_pcm(DataWin *dw, int32_t sound_idx, size_t *out_len, int *out_channels, int *out_sample_rate);

#ifdef __cplusplus
}
#endif

#endif /* DATA_WIN_H */
