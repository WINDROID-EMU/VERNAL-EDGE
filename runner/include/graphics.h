#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "data_win.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Blend mode constants matching GameMaker */
#define BM_NORMAL    0
#define BM_ADD       1
#define BM_MAX       2
#define BM_SUBTRACT  3

/* Vertex format for 2D quad batching */
typedef struct {
    float x, y, z;
    float u, v;
    uint8_t r, g, b, a;
} Vertex2D;

/* Surface descriptor (Framebuffer Object) */
typedef struct {
    int32_t id;
    int width;
    int height;
    uint32_t fbo;
    uint32_t gl_texture;
    bool in_use;
} Surface;

/* System lifecycle */
bool graphics_init(int screen_width, int screen_height);
void graphics_shutdown(void);
void graphics_begin_frame(void);
void graphics_end_frame(void);
void graphics_flush(void);

/* View & Camera */
void graphics_set_projection(float left, float right, float bottom, float top);
void graphics_set_viewport(int x, int y, int width, int height);
void camera_get_view_size(float *w, float *h);
void camera_get_view_pos(float *x, float *y);
void camera_set_view_pos(float x, float y);
void camera_set_view_size(float w, float h);

/* Sprites */
void draw_sprite_ext(int32_t spr_idx, float subimg, double x, double y,
                     double xscale, double yscale, double rot,
                     uint32_t col, double alpha);
void draw_sprite(int32_t spr_idx, float subimg, double x, double y);
void draw_sprite_stretched(int32_t spr_idx, float subimg, double x, double y, double w, double h);
void draw_sprite_stretched_ext(int32_t spr_idx, float subimg, double x, double y,
                               double w, double h, uint32_t col, double alpha);

/* Shapes & Primitives */
void draw_rectangle(double x1, double y1, double x2, double y2, bool outline);
void draw_rectangle_color(double x1, double y1, double x2, double y2,
                          uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4, bool outline);
void draw_circle(double x, double y, double r, bool outline);
void draw_circle_color(double x, double y, double r, uint32_t col1, uint32_t col2, bool outline);
void draw_line(double x1, double y1, double x2, double y2);
void draw_line_width(double x1, double y1, double x2, double y2, double w);
void draw_line_color(double x1, double y1, double x2, double y2, uint32_t c1, uint32_t c2);
void draw_clear_alpha(uint32_t col, double alpha);

/* Primitives API */
void draw_primitive_begin(int type);
void draw_primitive_begin_texture(int type, uint32_t tex);
void draw_vertex(double x, double y);
void draw_vertex_color(double x, double y, uint32_t col, double alpha);
void draw_vertex_texture(double x, double y, double u, double v);
void draw_vertex_texture_color(double x, double y, double u, double v, uint32_t col, double alpha);
void draw_primitive_end(void);

/* Draw State */
void draw_set_color(uint32_t col);
void draw_set_alpha(double alpha);
uint32_t draw_get_color(void);
double draw_get_alpha(void);

/* GPU State */
void gpu_set_blendmode(int bm);
void gpu_set_blendmode_ext(int src, int dst);
void gpu_set_blendenable(bool enable);
void gpu_set_colorwriteenable(bool r, bool g, bool b, bool a);
void gpu_push_state(void);
void gpu_pop_state(void);

/* Surfaces */
int32_t surface_create(int w, int h);
void surface_free(int32_t surf_id);
bool surface_exists(int32_t surf_id);
void surface_set_target(int32_t surf_id);
void surface_reset_target(void);
uint32_t surface_get_texture(int32_t surf_id);
int surface_get_width(int32_t surf_id);
int surface_get_height(int32_t surf_id);
void surface_resize(int32_t surf_id, int w, int h);
void draw_surface(int32_t surf_id, double x, double y);
void draw_surface_ext(int32_t surf_id, double x, double y,
                      double xscale, double yscale, double rot,
                      uint32_t col, double alpha);

/* Shaders */
void shader_set(int32_t shader_idx);
void shader_reset(void);
int32_t shader_get_uniform(int32_t shader_idx, const char *name);
int32_t shader_get_sampler_index(int32_t shader_idx, const char *name);
void shader_set_uniform_f(int32_t uniform_loc, float v0);
void shader_set_uniform_f_array(int32_t uniform_loc, const float *vals, int count);

/* Texture management */
uint32_t graphics_get_or_upload_texture(uint32_t tex_idx);

/* Tiles */
void graphics_draw_tile(int32_t tileset_idx, uint32_t tile_raw, float x, float y);

/* Fonts & Text */
void draw_set_font(int32_t font_idx);
int32_t draw_get_font(void);
void draw_text(double x, double y, const char *str);
void draw_text_color(double x, double y, const char *str, uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4, double alpha);
void draw_text_transformed(double x, double y, const char *str, double xscale, double yscale, double angle);
void draw_text_transformed_color(double x, double y, const char *str, double xscale, double yscale, double angle,
                                  uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4, double alpha);
float string_width(const char *str);
float string_height(const char *str);

#ifdef __cplusplus
}
#endif

#endif /* GRAPHICS_H */
