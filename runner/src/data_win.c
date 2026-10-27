#include "data_win.h"
#include "qoi.h"
#include "files.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dlfcn.h>

#include "bzlib.h"

/* Global DataWin instance */
DataWin g_DataWin = {0};

static inline uint32_t read_u32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static inline int32_t read_i32(const uint8_t *p) {
    return (int32_t)read_u32(p);
}

static inline uint16_t read_u16(const uint8_t *p) {
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static inline int16_t read_i16(const uint8_t *p) {
    return (int16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static inline float read_f32(const uint8_t *p) {
    union { uint32_t u; float f; } val;
    val.u = read_u32(p);
    return val.f;
}

const char *data_win_get_string(const DataWin *dw, uint32_t offset) {
    if (!dw || !dw->file_data || offset >= dw->file_size) return "";
    return (const char *)(dw->file_data + offset);
}

static const uint8_t *find_chunk(const DataWin *dw, const char *fourcc, uint32_t *out_size) {
    if (!dw || !dw->file_data || dw->file_size < 8) return NULL;
    if (memcmp(dw->file_data, "FORM", 4) != 0) return NULL;

    size_t pos = 8;
    while (pos + 8 <= dw->file_size) {
        const char *cname = (const char *)(dw->file_data + pos);
        uint32_t csize = read_u32(dw->file_data + pos + 4);
        if (memcmp(cname, fourcc, 4) == 0) {
            if (out_size) *out_size = csize;
            return dw->file_data + pos + 8;
        }
        pos += 8 + csize;
    }
    return NULL;
}

bool data_win_load(DataWin *dw, const char *filepath) {
    if (!dw || !filepath) return false;
    memset(dw, 0, sizeof(DataWin));

    int fd = open(filepath, O_RDONLY);
    char resolved_path[512] = "";
    if (fd >= 0) {
        strncpy(resolved_path, filepath, sizeof(resolved_path) - 1);
    } else if (filepath[0] != '/') {
        char fallback[512];
        const char *cand_dirs[] = {
            files_get_asset_directory(),
            files_get_save_directory(),
            "/sdcard/VernalEdge/",
            "/storage/emulated/0/VernalEdge/",
            "/sdcard/Android/data/com.vernaledge/files/",
            "/storage/emulated/0/Android/data/com.vernaledge/files/",
            "./"
        };
        for (size_t c = 0; c < sizeof(cand_dirs)/sizeof(cand_dirs[0]); c++) {
            const char *cdir = cand_dirs[c];
            if (!cdir || !cdir[0]) continue;
            snprintf(fallback, sizeof(fallback), "%s%s", cdir, filepath);
            fd = open(fallback, O_RDONLY);
            if (fd >= 0) {
                strncpy(resolved_path, fallback, sizeof(resolved_path) - 1);
                break;
            }
        }
    }
    if (fd < 0) {
        fprintf(stderr, "[DataWin] Failed to open %s in any searched directory\n", filepath);
        return false;
    }
    fprintf(stdout, "[DataWin] Successfully opened %s (fd %d)\n", resolved_path, fd);

    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        return false;
    }

    dw->file_size = (size_t)st.st_size;
    dw->file_data = (uint8_t *)mmap(NULL, dw->file_size, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);

    if (dw->file_data == MAP_FAILED) {
        fprintf(stderr, "[DataWin] Failed to mmap %s (%zu bytes)\n", filepath, dw->file_size);
        dw->file_data = NULL;
        return false;
    }

    /* 1. Parse TPAG chunk */
    uint32_t tpag_chunk_size = 0;
    const uint8_t *tpag_ptr = find_chunk(dw, "TPAG", &tpag_chunk_size);
    if (tpag_ptr) {
        dw->tpag_count = read_u32(tpag_ptr);
        dw->tpags = (TPAGItem *)calloc(dw->tpag_count, sizeof(TPAGItem));
        const uint32_t *tpag_offsets = (const uint32_t *)(tpag_ptr + 4);

        for (uint32_t i = 0; i < dw->tpag_count; i++) {
            uint32_t off = tpag_offsets[i];
            if (off + 22 <= dw->file_size) {
                const uint8_t *p = dw->file_data + off;
                dw->tpags[i].x        = read_u16(p + 0);
                dw->tpags[i].y        = read_u16(p + 2);
                dw->tpags[i].w        = read_u16(p + 4);
                dw->tpags[i].h        = read_u16(p + 6);
                dw->tpags[i].target_x = read_u16(p + 8);
                dw->tpags[i].target_y = read_u16(p + 10);
                dw->tpags[i].target_w = read_u16(p + 12);
                dw->tpags[i].target_h = read_u16(p + 14);
                dw->tpags[i].bound_w  = read_u16(p + 16);
                dw->tpags[i].bound_h  = read_u16(p + 18);
                dw->tpags[i].tex_id   = read_u16(p + 20);
            }
        }
        printf("[DataWin] Loaded %u TPAG items\n", dw->tpag_count);
    }

    /* 2. Parse TXTR chunk */
    uint32_t txtr_chunk_size = 0;
    const uint8_t *txtr_ptr = find_chunk(dw, "TXTR", &txtr_chunk_size);
    if (txtr_ptr) {
        dw->texture_count = read_u32(txtr_ptr);
        dw->textures = (TexturePage *)calloc(dw->texture_count, sizeof(TexturePage));
        const uint32_t *txtr_offsets = (const uint32_t *)(txtr_ptr + 4);

        for (uint32_t i = 0; i < dw->texture_count; i++) {
            uint32_t off = txtr_offsets[i];
            if (off + 28 <= dw->file_size) {
                const uint8_t *p = dw->file_data + off;
                dw->textures[i].width       = read_u32(p + 12);
                dw->textures[i].height      = read_u32(p + 16);
                dw->textures[i].data_offset = read_u32(p + 24);

                /* Read BZ2 len from texture data header */
                if (dw->textures[i].data_offset + 8 <= dw->file_size) {
                    const uint8_t *dp = dw->file_data + dw->textures[i].data_offset;
                    dw->textures[i].bz_len = read_u32(dp + 8);
                }
            }
        }
        printf("[DataWin] Loaded %u texture pages\n", dw->texture_count);
    }

    /* 3. Parse SPRT chunk */
    uint32_t sprt_chunk_size = 0;
    const uint8_t *sprt_ptr = find_chunk(dw, "SPRT", &sprt_chunk_size);
    if (sprt_ptr) {
        dw->sprite_count = read_u32(sprt_ptr);
        dw->sprites = (Sprite *)calloc(dw->sprite_count, sizeof(Sprite));
        const uint32_t *sprt_offsets = (const uint32_t *)(sprt_ptr + 4);

        for (uint32_t i = 0; i < dw->sprite_count; i++) {
            uint32_t off = sprt_offsets[i];
            if (off + 88 <= dw->file_size) {
                const uint8_t *p = dw->file_data + off;
                uint32_t name_ptr = read_u32(p + 0);
                dw->sprites[i].name     = data_win_get_string(dw, name_ptr);
                dw->sprites[i].width    = read_u32(p + 4);
                dw->sprites[i].height   = read_u32(p + 8);
                dw->sprites[i].origin_x = read_i32(p + 48);
                dw->sprites[i].origin_y = read_i32(p + 52);

                /* In GMS 2.3+: check version flag at offset 56 */
                int32_t v_flag = read_i32(p + 56);
                if (v_flag == -1) {
                    /* Version 3 */
                    dw->sprites[i].playback_speed = read_f32(p + 68);
                    dw->sprites[i].subimage_count = read_u32(p + 84);

                    if (dw->sprites[i].subimage_count > 0 && dw->sprites[i].subimage_count < 10000) {
                        dw->sprites[i].tpag_offsets = (uint32_t *)malloc(dw->sprites[i].subimage_count * sizeof(uint32_t));
                        const uint32_t *sub_ptrs = (const uint32_t *)(p + 88);
                        for (uint32_t s = 0; s < dw->sprites[i].subimage_count; s++) {
                            dw->sprites[i].tpag_offsets[s] = sub_ptrs[s];
                        }
                    }
                } else {
                    /* Legacy GMS format */
                    dw->sprites[i].subimage_count = read_u32(p + 56);
                    if (dw->sprites[i].subimage_count > 0 && dw->sprites[i].subimage_count < 10000) {
                        dw->sprites[i].tpag_offsets = (uint32_t *)malloc(dw->sprites[i].subimage_count * sizeof(uint32_t));
                        const uint32_t *sub_ptrs = (const uint32_t *)(p + 60);
                        for (uint32_t s = 0; s < dw->sprites[i].subimage_count; s++) {
                            dw->sprites[i].tpag_offsets[s] = sub_ptrs[s];
                        }
                    }
                }
            }
        }
        printf("[DataWin] Loaded %u sprites\n", dw->sprite_count);
    }

    /* 4. Parse OBJT chunk */
    uint32_t objt_chunk_size = 0;
    const uint8_t *objt_ptr = find_chunk(dw, "OBJT", &objt_chunk_size);
    if (objt_ptr) {
        dw->object_count = read_u32(objt_ptr);
        dw->objects = (GameObjectDef *)calloc(dw->object_count, sizeof(GameObjectDef));
        const uint32_t *objt_offsets = (const uint32_t *)(objt_ptr + 4);

        for (uint32_t i = 0; i < dw->object_count; i++) {
            uint32_t off = objt_offsets[i];
            if (off + 32 <= dw->file_size) {
                const uint8_t *p = dw->file_data + off;
                uint32_t name_ptr = read_u32(p + 0);
                dw->objects[i].name         = data_win_get_string(dw, name_ptr);
                dw->objects[i].sprite_index = read_i32(p + 4);
                dw->objects[i].visible      = (uint8_t)read_u32(p + 8);
                dw->objects[i].solid        = (uint8_t)read_u32(p + 12);
                dw->objects[i].depth        = read_i32(p + 16);
                dw->objects[i].persistent   = (uint8_t)read_u32(p + 20);
                dw->objects[i].parent_index = read_i32(p + 24);
                dw->objects[i].mask_index   = read_i32(p + 28);
            }
        }
        printf("[DataWin] Loaded %u game objects\n", dw->object_count);
    }

    /* 5. Parse ROOM chunk */
    uint32_t room_chunk_size = 0;
    const uint8_t *room_ptr = find_chunk(dw, "ROOM", &room_chunk_size);
    if (room_ptr) {
        dw->room_count = read_u32(room_ptr);
        dw->rooms = (DataWinRoomDef *)calloc(dw->room_count, sizeof(DataWinRoomDef));
        const uint32_t *room_offsets = (const uint32_t *)(room_ptr + 4);

        for (uint32_t i = 0; i < dw->room_count; i++) {
            uint32_t off = room_offsets[i];
            if (off + 56 <= dw->file_size) {
                const uint8_t *p = dw->file_data + off;
                uint32_t name_ptr = read_u32(p + 0);
                dw->rooms[i].name       = data_win_get_string(dw, name_ptr);
                dw->rooms[i].width      = read_u32(p + 8);
                dw->rooms[i].height     = read_u32(p + 12);
                dw->rooms[i].speed      = read_u32(p + 16);
                dw->rooms[i].persistent = (uint8_t)read_u32(p + 20);
                dw->rooms[i].bg_color   = read_u32(p + 24);

                /* GameObjects pointer in room header */
                uint32_t gameobjs_ptr = read_u32(p + 48);
                if (gameobjs_ptr + 4 <= dw->file_size) {
                    const uint8_t *gop = dw->file_data + gameobjs_ptr;
                    uint32_t obj_count = read_u32(gop);
                    dw->rooms[i].instance_count = obj_count;

                    if (obj_count > 0 && obj_count < 100000) {
                        dw->rooms[i].instances = (RoomInstDef *)calloc(obj_count, sizeof(RoomInstDef));
                        const uint32_t *inst_ptrs = (const uint32_t *)(gop + 4);

                        for (uint32_t o = 0; o < obj_count; o++) {
                            uint32_t i_off = inst_ptrs[o];
                            if (i_off + 36 <= dw->file_size) {
                                const uint8_t *ip = dw->file_data + i_off;
                                dw->rooms[i].instances[o].x                = read_i32(ip + 0);
                                dw->rooms[i].instances[o].y                = read_i32(ip + 4);
                                dw->rooms[i].instances[o].object_index     = read_i32(ip + 8);
                                dw->rooms[i].instances[o].instance_id      = read_i32(ip + 12);
                                dw->rooms[i].instances[o].creation_code_id = read_i32(ip + 16);
                                dw->rooms[i].instances[o].scale_x          = read_f32(ip + 20);
                                dw->rooms[i].instances[o].scale_y          = read_f32(ip + 24);
                                dw->rooms[i].instances[o].color            = read_u32(ip + 28);
                                dw->rooms[i].instances[o].rotation         = read_f32(ip + 32);
                            }
                        }
                    }
                }
                /* Layers pointer at offset +88 */
                if (off + 92 <= dw->file_size) {
                    uint32_t layers_ptr = read_u32(p + 88);
                    if (layers_ptr > 0 && layers_ptr + 4 <= dw->file_size) {
                        const uint8_t *lp_list = dw->file_data + layers_ptr;
                        uint32_t l_count = read_u32(lp_list);
                        if (l_count > 0 && l_count < 1000 && layers_ptr + 4 + l_count * 4 <= dw->file_size) {
                            dw->rooms[i].layer_count = l_count;
                            dw->rooms[i].layers = (RoomLayerDef *)calloc(l_count, sizeof(RoomLayerDef));
                            const uint32_t *l_offsets = (const uint32_t *)(lp_list + 4);
                            for (uint32_t l = 0; l < l_count; l++) {
                                uint32_t lp = l_offsets[l];
                                if (lp + 48 <= dw->file_size) {
                                    const uint8_t *lay = dw->file_data + lp;
                                    RoomLayerDef *rld = &dw->rooms[i].layers[l];
                                    rld->name       = data_win_get_string(dw, read_u32(lay + 0));
                                    rld->layer_id   = read_u32(lay + 4);
                                    rld->type       = (RoomLayerType)read_u32(lay + 8);
                                    rld->depth      = read_i32(lay + 12);
                                    rld->x_offset   = read_f32(lay + 16);
                                    rld->y_offset   = read_f32(lay + 20);
                                    rld->hspeed     = read_f32(lay + 24);
                                    rld->vspeed     = read_f32(lay + 28);
                                    rld->visible    = (uint8_t)read_u32(lay + 32);

                                    const uint8_t *dp = lay + 48;
                                    if (rld->type == LAYER_TYPE_BACKGROUND && lp + 48 + 28 <= dw->file_size) {
                                        rld->visible         = (uint8_t)read_u32(dp + 0);
                                        rld->bg_sprite_index = read_i32(dp + 8);
                                        rld->bg_tiled_h      = (uint8_t)read_u32(dp + 12);
                                        rld->bg_tiled_v      = (uint8_t)read_u32(dp + 16);
                                        rld->bg_stretch      = (uint8_t)read_u32(dp + 20);
                                        rld->bg_color        = read_u32(dp + 24);
                                    } else if (rld->type == LAYER_TYPE_INSTANCES && lp + 48 + 4 <= dw->file_size) {
                                        uint32_t inst_cnt = read_u32(dp + 0);
                                        if (inst_cnt > 0 && inst_cnt < 10000 && lp + 48 + 4 + inst_cnt * 4 <= dw->file_size) {
                                            const uint32_t *inst_ids = (const uint32_t *)(dp + 4);
                                            for (uint32_t ii = 0; ii < inst_cnt; ii++) {
                                                uint32_t target_id = inst_ids[ii];
                                                for (uint32_t o = 0; o < dw->rooms[i].instance_count; o++) {
                                                    if ((uint32_t)dw->rooms[i].instances[o].instance_id == target_id) {
                                                        dw->rooms[i].instances[o].depth = rld->depth;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    } else if (rld->type == LAYER_TYPE_ASSETS && lp + 48 + 8 <= dw->file_size) {
                                        uint32_t sprites_ptr = read_u32(dp + 4);
                                        if (sprites_ptr > 0 && sprites_ptr + 4 <= dw->file_size) {
                                            uint32_t as_cnt = read_u32(dw->file_data + sprites_ptr);
                                            if (as_cnt > 0 && as_cnt < 10000 && sprites_ptr + 4 + as_cnt * 4 <= dw->file_size) {
                                                rld->asset_sprite_count = as_cnt;
                                                rld->asset_sprites = (RoomAssetSprite *)calloc(as_cnt, sizeof(RoomAssetSprite));
                                                const uint32_t *as_ptrs = (const uint32_t *)(dw->file_data + sprites_ptr + 4);
                                                for (uint32_t s = 0; s < as_cnt; s++) {
                                                    uint32_t sp = as_ptrs[s];
                                                    if (sp + 44 <= dw->file_size) {
                                                        const uint8_t *asp = dw->file_data + sp;
                                                        rld->asset_sprites[s].name         = data_win_get_string(dw, read_u32(asp + 0));
                                                        rld->asset_sprites[s].sprite_index = read_i32(asp + 4);
                                                        rld->asset_sprites[s].x            = read_i32(asp + 8);
                                                        rld->asset_sprites[s].y            = read_i32(asp + 12);
                                                        rld->asset_sprites[s].scale_x      = read_f32(asp + 16);
                                                        rld->asset_sprites[s].scale_y      = read_f32(asp + 20);
                                                        rld->asset_sprites[s].color        = read_u32(asp + 24);
                                                        rld->asset_sprites[s].frame_index  = read_f32(asp + 36);
                                                        rld->asset_sprites[s].rotation     = read_f32(asp + 40);
                                                    }
                                                }
                                            }
                                        }
                                    } else if (rld->type == LAYER_TYPE_TILES && lp + 48 + 12 <= dw->file_size) {
                                        rld->tileset_index = read_i32(dp + 0);
                                        rld->tiles_x       = read_u32(dp + 4);
                                        rld->tiles_y       = read_u32(dp + 8);
                                        uint32_t total_tiles = rld->tiles_x * rld->tiles_y;
                                        if (total_tiles > 0 && total_tiles < 1000000 && lp + 48 + 12 + total_tiles * 4 <= dw->file_size) {
                                            rld->tile_data = (uint32_t *)malloc(total_tiles * sizeof(uint32_t));
                                            if (rld->tile_data) {
                                                memcpy(rld->tile_data, dp + 12, total_tiles * sizeof(uint32_t));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        printf("[DataWin] Loaded %u rooms\n", dw->room_count);
    }

    /* 5b. Parse BGND chunk (Tilesets) */
    uint32_t bgnd_chunk_size = 0;
    const uint8_t *bgnd_ptr = find_chunk(dw, "BGND", &bgnd_chunk_size);
    if (bgnd_ptr) {
        dw->tileset_count = read_u32(bgnd_ptr);
        dw->tilesets = (TilesetDef *)calloc(dw->tileset_count, sizeof(TilesetDef));
        const uint32_t *offsets = (const uint32_t *)(bgnd_ptr + 4);
        for (uint32_t i = 0; i < dw->tileset_count; i++) {
            uint32_t off = offsets[i];
            if (off + 52 <= dw->file_size) {
                const uint8_t *p = dw->file_data + off;
                dw->tilesets[i].name         = data_win_get_string(dw, read_u32(p + 0));
                dw->tilesets[i].tpag_offset  = read_u32(p + 16);
                dw->tilesets[i].tile_width   = read_u32(p + 24);
                dw->tilesets[i].tile_height  = read_u32(p + 28);
                dw->tilesets[i].border_x     = read_u32(p + 32);
                dw->tilesets[i].border_y     = read_u32(p + 36);
                dw->tilesets[i].tile_columns = read_u32(p + 40);
                dw->tilesets[i].tile_count   = read_u32(p + 48);
            }
        }
        printf("[DataWin] Loaded %u tilesets (BGND)\n", dw->tileset_count);
    }

    /* 5c. Parse FONT chunk */
    uint32_t font_chunk_size = 0;
    const uint8_t *font_ptr = find_chunk(dw, "FONT", &font_chunk_size);
    if (font_ptr) {
        dw->font_count = read_u32(font_ptr);
        dw->fonts = (FontDef *)calloc(dw->font_count, sizeof(FontDef));
        const uint32_t *offsets = (const uint32_t *)(font_ptr + 4);
        for (uint32_t i = 0; i < dw->font_count; i++) {
            uint32_t off = offsets[i];
            if (off + 52 <= dw->file_size) {
                const uint8_t *p = dw->file_data + off;
                dw->fonts[i].name        = data_win_get_string(dw, read_u32(p + 0));
                dw->fonts[i].tpag_offset = read_u32(p + 28);
                dw->fonts[i].scale_x     = read_f32(p + 32);
                dw->fonts[i].scale_y     = read_f32(p + 36);

                uint32_t glyphs_list_ptr = off + 48;
                if (glyphs_list_ptr + 4 <= dw->file_size) {
                    uint32_t g_count = read_u32(dw->file_data + glyphs_list_ptr);
                    dw->fonts[i].glyph_count = g_count;
                    if (g_count > 0 && g_count < 20000 && glyphs_list_ptr + 4 + g_count * 4 <= dw->file_size) {
                        dw->fonts[i].glyphs = (FontGlyph *)calloc(g_count, sizeof(FontGlyph));
                        const uint32_t *g_ptrs = (const uint32_t *)(dw->file_data + glyphs_list_ptr + 4);
                        for (uint32_t g = 0; g < g_count; g++) {
                            uint32_t gp = g_ptrs[g];
                            if (gp + 14 <= dw->file_size) {
                                const uint8_t *gp_data = dw->file_data + gp;
                                dw->fonts[i].glyphs[g].character = read_u16(gp_data + 0);
                                dw->fonts[i].glyphs[g].src_x     = read_u16(gp_data + 2);
                                dw->fonts[i].glyphs[g].src_y     = read_u16(gp_data + 4);
                                dw->fonts[i].glyphs[g].src_w     = read_u16(gp_data + 6);
                                dw->fonts[i].glyphs[g].src_h     = read_u16(gp_data + 8);
                                dw->fonts[i].glyphs[g].shift     = read_i16(gp_data + 10);
                                dw->fonts[i].glyphs[g].offset    = read_i16(gp_data + 12);
                            }
                        }
                    }
                }
            }
        }
        printf("[DataWin] Loaded %u fonts (FONT)\n", dw->font_count);
    }

    /* 6. Parse SHDR chunk */
    uint32_t shdr_chunk_size = 0;
    const uint8_t *shdr_ptr = find_chunk(dw, "SHDR", &shdr_chunk_size);
    if (shdr_ptr) {
        dw->shader_count = read_u32(shdr_ptr);
        dw->shaders = (ShaderDef *)calloc(dw->shader_count, sizeof(ShaderDef));
        const uint32_t *shdr_offsets = (const uint32_t *)(shdr_ptr + 4);

        for (uint32_t i = 0; i < dw->shader_count; i++) {
            uint32_t off = shdr_offsets[i];
            if (off + 20 <= dw->file_size) {
                const uint8_t *p = dw->file_data + off;
                dw->shaders[i].name        = data_win_get_string(dw, read_u32(p + 0));
                dw->shaders[i].vertex_code = data_win_get_string(dw, read_u32(p + 8));
                dw->shaders[i].pixel_code  = data_win_get_string(dw, read_u32(p + 12));
            }
        }
        printf("[DataWin] Loaded %u shaders\n", dw->shader_count);
    }

    /* 7. Parse AUDO chunk */
    uint32_t audo_chunk_size = 0;
    const uint8_t *audo_ptr = find_chunk(dw, "AUDO", &audo_chunk_size);
    if (audo_ptr) {
        dw->audio_count = read_u32(audo_ptr);
        dw->audio_entries = (EmbeddedAudio *)calloc(dw->audio_count, sizeof(EmbeddedAudio));
        const uint32_t *audo_offsets = (const uint32_t *)(audo_ptr + 4);

        for (uint32_t i = 0; i < dw->audio_count; i++) {
            uint32_t off = audo_offsets[i];
            if (off + 4 <= dw->file_size) {
                uint32_t len = read_u32(dw->file_data + off);
                dw->audio_entries[i].file_offset = off + 4;
                dw->audio_entries[i].data_len = len;
                dw->audio_entries[i].riff_data = dw->file_data + off + 4;
            }
        }
        printf("[DataWin] Loaded %u embedded audio entries (AUDO)\n", dw->audio_count);
    }

    /* 8. Parse SOND chunk */
    uint32_t sond_chunk_size = 0;
    const uint8_t *sond_ptr = find_chunk(dw, "SOND", &sond_chunk_size);
    if (sond_ptr) {
        dw->sound_count = read_u32(sond_ptr);
        dw->sounds = (SoundDef *)calloc(dw->sound_count, sizeof(SoundDef));
        const uint32_t *sond_offsets = (const uint32_t *)(sond_ptr + 4);

        for (uint32_t i = 0; i < dw->sound_count; i++) {
            uint32_t off = sond_offsets[i];
            if (off + 36 <= dw->file_size) {
                const uint8_t *p = dw->file_data + off;
                dw->sounds[i].name        = data_win_get_string(dw, read_u32(p + 0));
                dw->sounds[i].flags       = read_u32(p + 4);
                dw->sounds[i].file_name   = data_win_get_string(dw, read_u32(p + 12));
                dw->sounds[i].volume      = read_f32(p + 20);
                dw->sounds[i].pitch       = read_f32(p + 24);
                dw->sounds[i].group_id    = read_i32(p + 28);
                dw->sounds[i].audio_index = read_i32(p + 32);

                if (dw->sounds[i].file_name && strstr(dw->sounds[i].file_name, ".ogg")) {
                    dw->sounds[i].is_streamed = true;
                } else {
                    dw->sounds[i].is_streamed = false;
                }
            }
        }
        printf("[DataWin] Loaded %u sound definitions (SOND)\n", dw->sound_count);
    }

    /* Default starting room */
    dw->starting_room_index = 0; /* realinit */
    return true;
}

void data_win_free(DataWin *dw) {
    if (!dw) return;

    if (dw->textures) {
        for (uint32_t i = 0; i < dw->texture_count; i++) {
            if (dw->textures[i].rgba_pixels) {
                free(dw->textures[i].rgba_pixels);
            }
        }
        free(dw->textures);
    }

    if (dw->tpags) free(dw->tpags);

    if (dw->sprites) {
        for (uint32_t i = 0; i < dw->sprite_count; i++) {
            if (dw->sprites[i].tpag_offsets) {
                free(dw->sprites[i].tpag_offsets);
            }
        }
        free(dw->sprites);
    }

    if (dw->objects) free(dw->objects);

    if (dw->rooms) {
        for (uint32_t i = 0; i < dw->room_count; i++) {
            if (dw->rooms[i].instances) {
                free(dw->rooms[i].instances);
            }
            if (dw->rooms[i].layers) {
                for (uint32_t l = 0; l < dw->rooms[i].layer_count; l++) {
                    if (dw->rooms[i].layers[l].asset_sprites) {
                        free(dw->rooms[i].layers[l].asset_sprites);
                    }
                    if (dw->rooms[i].layers[l].tile_data) {
                        free(dw->rooms[i].layers[l].tile_data);
                    }
                }
                free(dw->rooms[i].layers);
            }
        }
        free(dw->rooms);
    }

    if (dw->tilesets) free(dw->tilesets);

    if (dw->fonts) {
        for (uint32_t i = 0; i < dw->font_count; i++) {
            if (dw->fonts[i].glyphs) {
                free(dw->fonts[i].glyphs);
            }
        }
        free(dw->fonts);
    }

    if (dw->shaders) free(dw->shaders);
    if (dw->sounds) free(dw->sounds);
    if (dw->audio_entries) free(dw->audio_entries);

    if (dw->file_data && dw->file_size > 0) {
        munmap(dw->file_data, dw->file_size);
    }

    memset(dw, 0, sizeof(DataWin));
}

uint8_t *data_win_decode_texture(DataWin *dw, uint32_t tex_idx) {
    if (!dw || tex_idx >= dw->texture_count) return NULL;
    TexturePage *tp = &dw->textures[tex_idx];
    if (tp->rgba_pixels) return tp->rgba_pixels;

    uint32_t data_off = tp->data_offset;
    if (data_off + 12 + tp->bz_len > dw->file_size) {
        fprintf(stderr, "[DataWin] Texture %u offset out of bounds\n", tex_idx);
        return NULL;
    }

    /* Texture header: magic (4), w (2), h (2), bz_len (4), bz2_data... */
    const uint8_t *bz_data = dw->file_data + data_off + 12;
    uint32_t bz_len = tp->bz_len;

    /* QOI buffer: max 2048x2048x5 */
    unsigned int qoi_max_size = 2048 * 2048 * 5;
    char *qoi_buf = (char *)malloc(qoi_max_size);
    if (!qoi_buf) return NULL;

    unsigned int qoi_size = qoi_max_size;
    int res = BZ2_bzBuffToBuffDecompress(qoi_buf, &qoi_size, (char *)bz_data, bz_len, 0, 0);
    if (res != 0) {
        fprintf(stderr, "[DataWin] BZ2 decompression failed for texture %u (err %d)\n", tex_idx, res);
        free(qoi_buf);
        return NULL;
    }

    gm_qoi_desc desc;
    uint8_t *rgba = gm_qoi_decode((const uint8_t *)qoi_buf, (size_t)qoi_size, &desc);
    free(qoi_buf);

    if (!rgba) {
        fprintf(stderr, "[DataWin] GM QOI decode failed for texture %u\n", tex_idx);
        return NULL;
    }

    tp->rgba_pixels = rgba;
    return tp->rgba_pixels;
}

Sprite *data_win_get_sprite(DataWin *dw, int32_t sprite_idx) {
    if (!dw || sprite_idx < 0 || (uint32_t)sprite_idx >= dw->sprite_count) return NULL;
    return &dw->sprites[sprite_idx];
}

GameObjectDef *data_win_get_object(DataWin *dw, int32_t obj_idx) {
    if (!dw || obj_idx < 0 || (uint32_t)obj_idx >= dw->object_count) return NULL;
    return &dw->objects[obj_idx];
}

DataWinRoomDef *data_win_get_room(DataWin *dw, int32_t room_idx) {
    if (!dw || room_idx < 0 || (uint32_t)room_idx >= dw->room_count) return NULL;
    return &dw->rooms[room_idx];
}

SoundDef *data_win_get_sound(DataWin *dw, int32_t sound_idx) {
    if (!dw || sound_idx < 0 || (uint32_t)sound_idx >= dw->sound_count) return NULL;
    return &dw->sounds[sound_idx];
}

int32_t data_win_find_room_by_name(DataWin *dw, const char *name) {
    if (!dw || !name) return -1;
    for (uint32_t i = 0; i < dw->room_count; i++) {
        if (dw->rooms[i].name && strcmp(dw->rooms[i].name, name) == 0) {
            return (int32_t)i;
        }
    }
    return -1;
}

int32_t data_win_find_object_by_name(DataWin *dw, const char *name) {
    if (!dw || !name) return -1;
    for (uint32_t i = 0; i < dw->object_count; i++) {
        if (dw->objects[i].name && strcmp(dw->objects[i].name, name) == 0) {
            return (int32_t)i;
        }
    }
    return -1;
}

int32_t data_win_find_sprite_by_name(DataWin *dw, const char *name) {
    if (!dw || !name) return -1;
    for (uint32_t i = 0; i < dw->sprite_count; i++) {
        if (dw->sprites[i].name && strcmp(dw->sprites[i].name, name) == 0) {
            return (int32_t)i;
        }
    }
    return -1;
}

int32_t data_win_find_sound_by_name(DataWin *dw, const char *name) {
    if (!dw || !name) return -1;
    for (uint32_t i = 0; i < dw->sound_count; i++) {
        if (dw->sounds[i].name && strcmp(dw->sounds[i].name, name) == 0) {
            return (int32_t)i;
        }
    }
    return -1;
}

const uint8_t *data_win_get_sound_pcm(DataWin *dw, int32_t sound_idx, size_t *out_len, int *out_channels, int *out_sample_rate) {
    if (!dw || sound_idx < 0 || (uint32_t)sound_idx >= dw->sound_count) return NULL;
    SoundDef *snd = &dw->sounds[sound_idx];
    if (snd->is_streamed || snd->audio_index < 0 || (uint32_t)snd->audio_index >= dw->audio_count) return NULL;

    EmbeddedAudio *ea = &dw->audio_entries[snd->audio_index];
    if (!ea->riff_data || ea->data_len < 44) return NULL;

    const uint8_t *riff = ea->riff_data;
    if (memcmp(riff, "RIFF", 4) != 0 || memcmp(riff + 8, "WAVE", 4) != 0) return NULL;

    size_t p = 12;
    int channels = 2;
    int sample_rate = 44100;
    const uint8_t *pcm_data = NULL;
    size_t pcm_len = 0;

    while (p + 8 <= ea->data_len) {
        const char *chunk_id = (const char *)(riff + p);
        uint32_t chunk_sz = read_u32(riff + p + 4);
        p += 8;

        if (memcmp(chunk_id, "fmt ", 4) == 0 && chunk_sz >= 16) {
            channels = (int)read_u16(riff + p + 2);
            sample_rate = (int)read_u32(riff + p + 4);
        } else if (memcmp(chunk_id, "data", 4) == 0) {
            pcm_data = riff + p;
            pcm_len = (size_t)chunk_sz;
            break;
        }
        p += chunk_sz;
    }

    if (!pcm_data) return NULL;

    if (out_len) *out_len = pcm_len;
    if (out_channels) *out_channels = channels;
    if (out_sample_rate) *out_sample_rate = sample_rate;
    return pcm_data;
}

TilesetDef *data_win_get_tileset(DataWin *dw, int32_t tileset_idx) {
    if (!dw || tileset_idx < 0 || (uint32_t)tileset_idx >= dw->tileset_count) return NULL;
    return &dw->tilesets[tileset_idx];
}

FontDef *data_win_get_font(DataWin *dw, int32_t font_idx) {
    if (!dw || font_idx < 0 || (uint32_t)font_idx >= dw->font_count) return NULL;
    return &dw->fonts[font_idx];
}

int32_t data_win_find_font_by_name(DataWin *dw, const char *name) {
    if (!dw || !name) return -1;
    for (uint32_t i = 0; i < dw->font_count; i++) {
        if (dw->fonts[i].name && strcmp(dw->fonts[i].name, name) == 0) {
            return (int32_t)i;
        }
    }
    return -1;
}

TPAGItem data_win_read_tpag(const DataWin *dw, uint32_t off) {
    TPAGItem item = {0};
    if (dw && dw->file_data && off + 22 <= dw->file_size) {
        const uint8_t *p = dw->file_data + off;
        item.x        = read_u16(p + 0);
        item.y        = read_u16(p + 2);
        item.w        = read_u16(p + 4);
        item.h        = read_u16(p + 6);
        item.target_x = read_u16(p + 8);
        item.target_y = read_u16(p + 10);
        item.target_w = read_u16(p + 12);
        item.target_h = read_u16(p + 14);
        item.bound_w  = read_u16(p + 16);
        item.bound_h  = read_u16(p + 18);
        item.tex_id   = read_u16(p + 20);
    }
    return item;
}
