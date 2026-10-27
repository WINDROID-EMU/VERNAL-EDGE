#include "graphics.h"
#include "gles_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_BATCH_VERTICES 12000
#define MAX_SURFACES 64

static Vertex2D s_vertex_batch[MAX_BATCH_VERTICES];
static size_t s_vertex_count = 0;

static uint32_t s_current_tex = 0;
static uint32_t s_white_tex = 0;
static int s_current_blendmode = BM_NORMAL;

static GLuint s_default_program = 0;
static GLint s_loc_proj = -1;
static GLint s_loc_tex = -1;
static GLuint s_vbo = 0;

static float s_proj_matrix[16] = {0};
static float s_cam_x = 0.0f;
static float s_cam_y = 0.0f;
static float s_cam_w = 480.0f;
static float s_cam_h = 270.0f;

static uint32_t s_draw_color = 0xffffffff;
static double s_draw_alpha = 1.0;
static int32_t s_current_font = 1; /* Default to font_3 */

static Surface s_surfaces[MAX_SURFACES] = {0};
static int32_t s_current_surface = -1;
static uint32_t s_main_fbo = 0;
static int s_screen_w = 480;
static int s_screen_h = 270;

/* Primitive drawing state */
static int s_prim_type = 0;
static uint32_t s_prim_tex = 0;
static Vertex2D s_prim_vertices[256];
static size_t s_prim_vert_count = 0;

/* Default shader source */
static const char *s_default_vs =
    "attribute vec3 in_Position;\n"
    "attribute vec2 in_TexCoord;\n"
    "attribute vec4 in_Color;\n"
    "varying vec2 v_vTexCoord;\n"
    "varying vec4 v_vColour;\n"
    "uniform mat4 u_MatrixProjection;\n"
    "void main() {\n"
    "    gl_Position = u_MatrixProjection * vec4(in_Position, 1.0);\n"
    "    v_vTexCoord = in_TexCoord;\n"
    "    v_vColour = in_Color;\n"
    "}\n";

static const char *s_default_fs =
    "#ifdef GL_ES\n"
    "precision mediump float;\n"
    "#endif\n"
    "varying vec2 v_vTexCoord;\n"
    "varying vec4 v_vColour;\n"
    "uniform sampler2D gm_BaseTexture;\n"
    "void main() {\n"
    "    gl_FragColor = v_vColour * texture2D(gm_BaseTexture, v_vTexCoord);\n"
    "}\n";

static GLuint compile_shader(GLenum type, const char *source) {
    if (!glCreateShader) return 0;
    GLuint shd = glCreateShader(type);
    if (!shd) return 0;
    glShaderSource(shd, 1, &source, NULL);
    glCompileShader(shd);

    GLint ok = 0;
    glGetShaderiv(shd, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        memset(log, 0, sizeof(log));
        if (glGetShaderInfoLog) glGetShaderInfoLog(shd, sizeof(log) - 1, NULL, log);
        if (log[0] && (unsigned char)log[0] >= 32) {
            fprintf(stderr, "[Graphics] Shader compile error: %s\n", log);
        }
        if (glDeleteShader) glDeleteShader(shd);
        return 0;
    }
    return shd;
}

static void create_ortho_matrix(float *mat, float left, float right, float bottom, float top, float near_z, float far_z) {
    memset(mat, 0, 16 * sizeof(float));
    mat[0]  = 2.0f / (right - left);
    mat[5]  = 2.0f / (top - bottom);
    mat[10] = -2.0f / (far_z - near_z);
    mat[12] = -(right + left) / (right - left);
    mat[13] = -(top + bottom) / (top - bottom);
    mat[14] = -(far_z + near_z) / (far_z - near_z);
    mat[15] = 1.0f;
}

bool graphics_init(int screen_width, int screen_height) {
#ifndef __ANDROID__
    if (!gles_loader_init()) {
        fprintf(stderr, "[Graphics] gles_loader_init failed!\n");
        return false;
    }
#endif

    s_screen_w = screen_width;
    s_screen_h = screen_height;
    s_cam_w = (float)screen_width;
    s_cam_h = (float)screen_height;

    /* Build default 2D shader */
    GLuint vs = compile_shader(GL_VERTEX_SHADER, s_default_vs);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, s_default_fs);
    if (!vs || !fs) return false;

    s_default_program = glCreateProgram();
    glAttachShader(s_default_program, vs);
    glAttachShader(s_default_program, fs);
    glLinkProgram(s_default_program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    s_loc_proj = glGetUniformLocation(s_default_program, "u_MatrixProjection");
    s_loc_tex  = glGetUniformLocation(s_default_program, "gm_BaseTexture");

    /* Create dynamic VBO */
    glGenBuffers(1, &s_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(s_vertex_batch), NULL, GL_DYNAMIC_DRAW);

    /* Create 1x1 white fallback texture */
    glGenTextures(1, &s_white_tex);
    glBindTexture(GL_TEXTURE_2D, s_white_tex);
    uint32_t white_pixel = 0xffffffff;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white_pixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    s_current_tex = s_white_tex;

    /* Enable 2D blending */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    graphics_set_projection(0.0f, s_cam_w, s_cam_h, 0.0f);
    glViewport(0, 0, screen_width, screen_height);

    printf("[Graphics] Initialized 2D Renderer (Viewport: %dx%d)\n", screen_width, screen_height);
    return true;
}

void graphics_shutdown(void) {
    graphics_flush();
    if (s_white_tex) { glDeleteTextures(1, &s_white_tex); s_white_tex = 0; }
    if (s_vbo) { glDeleteBuffers(1, &s_vbo); s_vbo = 0; }
    if (s_default_program) { glDeleteProgram(s_default_program); s_default_program = 0; }
}

void graphics_set_projection(float left, float right, float bottom, float top) {
    graphics_flush();
    create_ortho_matrix(s_proj_matrix, left, right, bottom, top, -1000.0f, 1000.0f);
}

void graphics_set_viewport(int x, int y, int width, int height) {
    graphics_flush();
    glViewport(x, y, width, height);
}

void camera_get_view_size(float *w, float *h) {
    if (w) *w = s_cam_w;
    if (h) *h = s_cam_h;
}

void camera_get_view_pos(float *x, float *y) {
    if (x) *x = s_cam_x;
    if (y) *y = s_cam_y;
}

void camera_set_view_pos(float x, float y) {
    s_cam_x = x;
    s_cam_y = y;
    graphics_set_projection(x, x + s_cam_w, y + s_cam_h, y);
}

void camera_set_view_size(float w, float h) {
    s_cam_w = w;
    s_cam_h = h;
    graphics_set_projection(s_cam_x, s_cam_x + w, s_cam_y + h, s_cam_y);
}

uint32_t graphics_get_or_upload_texture(uint32_t tex_idx) {
    if (tex_idx >= g_DataWin.texture_count) return s_white_tex;
    TexturePage *tp = &g_DataWin.textures[tex_idx];
    if (tp->gl_texture_id != 0) return tp->gl_texture_id;

    uint8_t *rgba = data_win_decode_texture(&g_DataWin, tex_idx);
    if (!rgba) return s_white_tex;

    GLuint gl_id = 0;
    glGenTextures(1, &gl_id);
    glBindTexture(GL_TEXTURE_2D, gl_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)tp->width, (GLsizei)tp->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    tp->gl_texture_id = gl_id ? gl_id : (tex_idx + 1);
    printf("[Graphics] Decoded and uploaded Texture Page %u (%ux%u) to GL tex %u\n",
           tex_idx, tp->width, tp->height, tp->gl_texture_id);
    return tp->gl_texture_id;
}

void graphics_flush(void) {
    if (s_vertex_count == 0) return;
    if (!glDrawArrays || s_default_program == 0) {
        s_vertex_count = 0;
        return;
    }

    glUseProgram(s_default_program);
    glUniformMatrix4fv(s_loc_proj, 1, GL_FALSE, s_proj_matrix);
    glUniform1i(s_loc_tex, 0);

    glBindTexture(GL_TEXTURE_2D, s_current_tex);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(s_vertex_count * sizeof(Vertex2D)), s_vertex_batch);

    /* Attributes */
    GLint pos_loc = 0; // in_Position
    GLint uv_loc  = 1; // in_TexCoord
    GLint col_loc = 2; // in_Color

    glEnableVertexAttribArray(pos_loc);
    glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, x));

    glEnableVertexAttribArray(uv_loc);
    glVertexAttribPointer(uv_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, u));

    glEnableVertexAttribArray(col_loc);
    glVertexAttribPointer(col_loc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, r));

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)s_vertex_count);

    glDisableVertexAttribArray(pos_loc);
    glDisableVertexAttribArray(uv_loc);
    glDisableVertexAttribArray(col_loc);

    s_vertex_count = 0;
}

static inline void batch_vertex(float x, float y, float z, float u, float v, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (s_vertex_count >= MAX_BATCH_VERTICES) {
        graphics_flush();
    }
    Vertex2D *vtx = &s_vertex_batch[s_vertex_count++];
    vtx->x = x; vtx->y = y; vtx->z = z;
    vtx->u = u; vtx->v = v;
    vtx->r = r; vtx->g = g; vtx->b = b; vtx->a = a;
}

static void batch_quad(uint32_t tex,
                       float x0, float y0, float u0, float v0,
                       float x1, float y1, float u1, float v1,
                       float x2, float y2, float u2, float v2,
                       float x3, float y3, float u3, float v3,
                       uint32_t color, float alpha) {
    if (tex != s_current_tex) {
        graphics_flush();
        s_current_tex = tex;
    }

    if (s_vertex_count + 6 > MAX_BATCH_VERTICES) {
        graphics_flush();
    }

    uint8_t cr = (uint8_t)(color & 0xff);
    uint8_t cg = (uint8_t)((color >> 8) & 0xff);
    uint8_t cb = (uint8_t)((color >> 16) & 0xff);
    uint8_t ca = (uint8_t)(alpha * 255.0f);

    /* Triangle 1: 0, 1, 2 */
    batch_vertex(x0, y0, 0.0f, u0, v0, cr, cg, cb, ca);
    batch_vertex(x1, y1, 0.0f, u1, v1, cr, cg, cb, ca);
    batch_vertex(x2, y2, 0.0f, u2, v2, cr, cg, cb, ca);

    /* Triangle 2: 2, 3, 0 */
    batch_vertex(x2, y2, 0.0f, u2, v2, cr, cg, cb, ca);
    batch_vertex(x3, y3, 0.0f, u3, v3, cr, cg, cb, ca);
    batch_vertex(x0, y0, 0.0f, u0, v0, cr, cg, cb, ca);
}

void graphics_begin_frame(void) {
    s_current_tex = s_white_tex;
    s_vertex_count = 0;
}

void graphics_end_frame(void) {
    graphics_flush();
}

/* ================= GameMaker Drawing Builtins ================= */

void draw_set_color(uint32_t col) {
    s_draw_color = col;
}

void draw_set_alpha(double alpha) {
    s_draw_alpha = alpha;
}

uint32_t draw_get_color(void) {
    return s_draw_color;
}

double draw_get_alpha(void) {
    return s_draw_alpha;
}

void draw_clear_alpha(uint32_t col, double alpha) {
    graphics_flush();
    float r = (float)(col & 0xff) / 255.0f;
    float g = (float)((col >> 8) & 0xff) / 255.0f;
    float b = (float)((col >> 16) & 0xff) / 255.0f;
    float a = (float)alpha;
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void draw_sprite_ext(int32_t spr_idx, float subimg, double x, double y,
                     double xscale, double yscale, double rot,
                     uint32_t col, double alpha) {
    Sprite *spr = data_win_get_sprite(&g_DataWin, spr_idx);
    if (!spr || spr->subimage_count == 0 || !spr->tpag_offsets) return;

    int sub = (int)subimg;
    if (sub < 0) sub = 0;
    sub %= (int)spr->subimage_count;

    uint32_t tpag_off = spr->tpag_offsets[sub];
    if (tpag_off + 22 > g_DataWin.file_size) return;

    const uint8_t *p = g_DataWin.file_data + tpag_off;
    uint16_t tp_x  = (uint16_t)p[0] | ((uint16_t)p[1] << 8);
    uint16_t tp_y  = (uint16_t)p[2] | ((uint16_t)p[3] << 8);
    uint16_t tp_w  = (uint16_t)p[4] | ((uint16_t)p[5] << 8);
    uint16_t tp_h  = (uint16_t)p[6] | ((uint16_t)p[7] << 8);
    uint16_t tgt_x = (uint16_t)p[8] | ((uint16_t)p[9] << 8);
    uint16_t tgt_y = (uint16_t)p[10] | ((uint16_t)p[11] << 8);
    uint16_t tgt_w = (uint16_t)p[12] | ((uint16_t)p[13] << 8);
    uint16_t tgt_h = (uint16_t)p[14] | ((uint16_t)p[15] << 8);
    uint16_t tex_id = (uint16_t)p[20] | ((uint16_t)p[21] << 8);

    uint32_t gl_tex = graphics_get_or_upload_texture(tex_id);
    if (tex_id >= g_DataWin.texture_count) return;

    float tex_w = (float)g_DataWin.textures[tex_id].width;
    float tex_h = (float)g_DataWin.textures[tex_id].height;
    if (tex_w <= 0.0f || tex_h <= 0.0f) return;

    /* UV coordinates */
    float u0 = (float)tp_x / tex_w;
    float v0 = (float)tp_y / tex_h;
    float u1 = (float)(tp_x + tp_w) / tex_w;
    float v1 = (float)(tp_y + tp_h) / tex_h;

    /* Quad relative to origin */
    float lx0 = (float)tgt_x - (float)spr->origin_x;
    float ly0 = (float)tgt_y - (float)spr->origin_y;
    float lx1 = lx0 + (float)tgt_w;
    float ly1 = ly0 + (float)tgt_h;

    /* Corners: 0=(lx0, ly0), 1=(lx1, ly0), 2=(lx1, ly1), 3=(lx0, ly1) */
    float rad = (float)rot * (3.14159265f / 180.0f);
    float cos_r = cosf(rad);
    float sin_r = sinf(rad);

    float xs = (float)xscale;
    float ys = (float)yscale;
    float bx = (float)x;
    float by = (float)y;

    #define TRANSFORM_X(lx, ly) (bx + ((lx) * xs * cos_r + (ly) * ys * sin_r))
    #define TRANSFORM_Y(lx, ly) (by + (-(lx) * xs * sin_r + (ly) * ys * cos_r))

    float x0 = TRANSFORM_X(lx0, ly0); float y0 = TRANSFORM_Y(lx0, ly0);
    float x1 = TRANSFORM_X(lx1, ly0); float y1 = TRANSFORM_Y(lx1, ly0);
    float x2 = TRANSFORM_X(lx1, ly1); float y2 = TRANSFORM_Y(lx1, ly1);
    float x3 = TRANSFORM_X(lx0, ly1); float y3 = TRANSFORM_Y(lx0, ly1);

    #undef TRANSFORM_X
    #undef TRANSFORM_Y

    batch_quad(gl_tex, x0, y0, u0, v0, x1, y1, u1, v0, x2, y2, u1, v1, x3, y3, u0, v1, col, (float)alpha);
}

void draw_sprite(int32_t spr_idx, float subimg, double x, double y) {
    draw_sprite_ext(spr_idx, subimg, x, y, 1.0, 1.0, 0.0, s_draw_color, s_draw_alpha);
}

void draw_sprite_stretched(int32_t spr_idx, float subimg, double x, double y, double w, double h) {
    Sprite *spr = data_win_get_sprite(&g_DataWin, spr_idx);
    if (!spr || spr->width == 0 || spr->height == 0) return;
    double xs = w / (double)spr->width;
    double ys = h / (double)spr->height;
    draw_sprite_ext(spr_idx, subimg, x, y, xs, ys, 0.0, s_draw_color, s_draw_alpha);
}

void draw_sprite_stretched_ext(int32_t spr_idx, float subimg, double x, double y,
                               double w, double h, uint32_t col, double alpha) {
    Sprite *spr = data_win_get_sprite(&g_DataWin, spr_idx);
    if (!spr || spr->width == 0 || spr->height == 0) return;
    double xs = w / (double)spr->width;
    double ys = h / (double)spr->height;
    draw_sprite_ext(spr_idx, subimg, x, y, xs, ys, 0.0, col, alpha);
}

/* YYC direct function FUN_14186cd80 implementation */
void FUN_14186cd80(void *ctx, int32_t spr_idx, float subimg, double x, double y,
                   double xscale, double yscale, double rot, uint32_t col, double alpha) {
    (void)ctx;
    draw_sprite_ext(spr_idx, subimg, x, y, xscale, yscale, rot, col, alpha);
}

void draw_rectangle(double x1, double y1, double x2, double y2, bool outline) {
    draw_rectangle_color(x1, y1, x2, y2, s_draw_color, s_draw_color, s_draw_color, s_draw_color, outline);
}

void draw_rectangle_color(double x1, double y1, double x2, double y2,
                          uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4, bool outline) {
    (void)c2; (void)c3; (void)c4;
    float fx1 = (float)x1; float fy1 = (float)y1;
    float fx2 = (float)x2; float fy2 = (float)y2;

    if (!outline) {
        batch_quad(s_white_tex, fx1, fy1, 0, 0, fx2, fy1, 0, 0, fx2, fy2, 0, 0, fx1, fy2, 0, 0, c1, (float)s_draw_alpha);
    } else {
        draw_line_color(x1, y1, x2, y1, c1, c1);
        draw_line_color(x2, y1, x2, y2, c1, c1);
        draw_line_color(x2, y2, x1, y2, c1, c1);
        draw_line_color(x1, y2, x1, y1, c1, c1);
    }
}

void draw_line(double x1, double y1, double x2, double y2) {
    draw_line_color(x1, y1, x2, y2, s_draw_color, s_draw_color);
}

void draw_line_color(double x1, double y1, double x2, double y2, uint32_t c1, uint32_t c2) {
    draw_line_width(x1, y1, x2, y2, 1.0);
}

void draw_line_width(double x1, double y1, double x2, double y2, double w) {
    float dx = (float)(x2 - x1);
    float dy = (float)(y2 - y1);
    float len = sqrtf(dx * dx + dy * dy);
    if (len <= 0.0001f) return;

    float nx = -dy / len * ((float)w * 0.5f);
    float ny =  dx / len * ((float)w * 0.5f);

    float fx1 = (float)x1; float fy1 = (float)y1;
    float fx2 = (float)x2; float fy2 = (float)y2;

    batch_quad(s_white_tex,
               fx1 + nx, fy1 + ny, 0, 0,
               fx2 + nx, fy2 + ny, 0, 0,
               fx2 - nx, fy2 - ny, 0, 0,
               fx1 - nx, fy1 - ny, 0, 0,
               s_draw_color, (float)s_draw_alpha);
}

void draw_circle(double x, double y, double r, bool outline) {
    draw_circle_color(x, y, r, s_draw_color, s_draw_color, outline);
}

void draw_circle_color(double x, double y, double r, uint32_t col1, uint32_t col2, bool outline) {
    (void)col2;
    int segments = 32;
    float step = (2.0f * 3.14159265f) / (float)segments;
    float bx = (float)x; float by = (float)y; float fr = (float)r;

    if (!outline) {
        for (int i = 0; i < segments; i++) {
            float a1 = (float)i * step;
            float a2 = (float)(i + 1) * step;
            float x1 = bx + cosf(a1) * fr; float y1 = by + sinf(a1) * fr;
            float x2 = bx + cosf(a2) * fr; float y2 = by + sinf(a2) * fr;

            if (s_current_tex != s_white_tex) { graphics_flush(); s_current_tex = s_white_tex; }
            uint8_t cr = (uint8_t)(col1 & 0xff);
            uint8_t cg = (uint8_t)((col1 >> 8) & 0xff);
            uint8_t cb = (uint8_t)((col1 >> 16) & 0xff);
            uint8_t ca = (uint8_t)((col1 >> 24) & 0xff);
            if (ca == 0) ca = (uint8_t)(s_draw_alpha * 255.0);

            batch_vertex(bx, by, 0, 0, 0, cr, cg, cb, ca);
            batch_vertex(x1, y1, 0, 0, 0, cr, cg, cb, ca);
            batch_vertex(x2, y2, 0, 0, 0, cr, cg, cb, ca);
        }
    } else {
        for (int i = 0; i < segments; i++) {
            float a1 = (float)i * step;
            float a2 = (float)(i + 1) * step;
            draw_line(bx + cosf(a1) * fr, by + sinf(a1) * fr,
                      bx + cosf(a2) * fr, by + sinf(a2) * fr);
        }
    }
}

/* ================= Primitive Drawing ================= */

void draw_primitive_begin(int type) {
    s_prim_type = type;
    s_prim_tex = s_white_tex;
    s_prim_vert_count = 0;
}

void draw_primitive_begin_texture(int type, uint32_t tex) {
    s_prim_type = type;
    s_prim_tex = tex ? tex : s_white_tex;
    s_prim_vert_count = 0;
}

void draw_vertex(double x, double y) {
    draw_vertex_texture_color(x, y, 0.0, 0.0, s_draw_color, s_draw_alpha);
}

void draw_vertex_color(double x, double y, uint32_t col, double alpha) {
    draw_vertex_texture_color(x, y, 0.0, 0.0, col, alpha);
}

void draw_vertex_texture(double x, double y, double u, double v) {
    draw_vertex_texture_color(x, y, u, v, s_draw_color, s_draw_alpha);
}

void draw_vertex_texture_color(double x, double y, double u, double v, uint32_t col, double alpha) {
    if (s_prim_vert_count >= 256) return;
    Vertex2D *vtx = &s_prim_vertices[s_prim_vert_count++];
    vtx->x = (float)x; vtx->y = (float)y; vtx->z = 0.0f;
    vtx->u = (float)u; vtx->v = (float)v;
    vtx->r = (uint8_t)(col & 0xff);
    vtx->g = (uint8_t)((col >> 8) & 0xff);
    vtx->b = (uint8_t)((col >> 16) & 0xff);
    vtx->a = (uint8_t)(alpha * 255.0);
}

void draw_primitive_end(void) {
    if (s_prim_vert_count < 3) return;
    if (s_current_tex != s_prim_tex) { graphics_flush(); s_current_tex = s_prim_tex; }

    /* Fan into triangles */
    for (size_t i = 1; i + 1 < s_prim_vert_count; i++) {
        const Vertex2D *v0 = &s_prim_vertices[0];
        const Vertex2D *v1 = &s_prim_vertices[i];
        const Vertex2D *v2 = &s_prim_vertices[i + 1];

        batch_vertex(v0->x, v0->y, 0, v0->u, v0->v, v0->r, v0->g, v0->b, v0->a);
        batch_vertex(v1->x, v1->y, 0, v1->u, v1->v, v1->r, v1->g, v1->b, v1->a);
        batch_vertex(v2->x, v2->y, 0, v2->u, v2->v, v2->r, v2->g, v2->b, v2->a);
    }
}

/* ================= GPU State ================= */

void gpu_set_blendmode(int bm) {
    if (s_current_blendmode == bm) return;
    graphics_flush();
    s_current_blendmode = bm;
    switch (bm) {
        case BM_NORMAL:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BM_ADD:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        case BM_MAX:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
            break;
        case BM_SUBTRACT:
            glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
            break;
        default:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
    }
}

void gpu_set_blendmode_ext(int src, int dst) {
    graphics_flush();
    glBlendFunc((GLenum)src, (GLenum)dst);
}

void gpu_set_blendenable(bool enable) {
    graphics_flush();
    if (enable) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
}

void gpu_set_colorwriteenable(bool r, bool g, bool b, bool a) {
    graphics_flush();
    glColorMask((GLboolean)r, (GLboolean)g, (GLboolean)b, (GLboolean)a);
}

void gpu_push_state(void) {
    /* Push GPU state onto stack if needed */
}

void gpu_pop_state(void) {
    /* Pop GPU state */
}

/* ================= Surfaces ================= */

int32_t surface_create(int w, int h) {
    for (int32_t i = 0; i < MAX_SURFACES; i++) {
        if (!s_surfaces[i].in_use) {
            Surface *surf = &s_surfaces[i];
            surf->id = i;
            surf->width = w;
            surf->height = h;
            surf->in_use = true;

            glGenTextures(1, &surf->gl_texture);
            glBindTexture(GL_TEXTURE_2D, surf->gl_texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glGenFramebuffers(1, &surf->fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, surf->fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, surf->gl_texture, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, s_main_fbo);
            return i;
        }
    }
    return -1;
}

void surface_free(int32_t surf_id) {
    if (surf_id < 0 || surf_id >= MAX_SURFACES || !s_surfaces[surf_id].in_use) return;
    graphics_flush();
    Surface *surf = &s_surfaces[surf_id];
    if (surf->fbo) glDeleteFramebuffers(1, &surf->fbo);
    if (surf->gl_texture) glDeleteTextures(1, &surf->gl_texture);
    memset(surf, 0, sizeof(Surface));
}

bool surface_exists(int32_t surf_id) {
    if (surf_id < 0 || surf_id >= MAX_SURFACES) return false;
    return s_surfaces[surf_id].in_use;
}

void surface_set_target(int32_t surf_id) {
    if (surf_id < 0 || surf_id >= MAX_SURFACES || !s_surfaces[surf_id].in_use) return;
    graphics_flush();
    s_current_surface = surf_id;
    Surface *surf = &s_surfaces[surf_id];
    glBindFramebuffer(GL_FRAMEBUFFER, surf->fbo);
    glViewport(0, 0, surf->width, surf->height);
    graphics_set_projection(0.0f, (float)surf->width, (float)surf->height, 0.0f);
}

void surface_reset_target(void) {
    graphics_flush();
    s_current_surface = -1;
    glBindFramebuffer(GL_FRAMEBUFFER, s_main_fbo);
    glViewport(0, 0, s_screen_w, s_screen_h);
    graphics_set_projection(s_cam_x, s_cam_x + s_cam_w, s_cam_y + s_cam_h, s_cam_y);
}

uint32_t surface_get_texture(int32_t surf_id) {
    if (surf_id < 0 || surf_id >= MAX_SURFACES || !s_surfaces[surf_id].in_use) return 0;
    return s_surfaces[surf_id].gl_texture;
}

int surface_get_width(int32_t surf_id) {
    if (surf_id < 0 || surf_id >= MAX_SURFACES || !s_surfaces[surf_id].in_use) return 0;
    return s_surfaces[surf_id].width;
}

int surface_get_height(int32_t surf_id) {
    if (surf_id < 0 || surf_id >= MAX_SURFACES || !s_surfaces[surf_id].in_use) return 0;
    return s_surfaces[surf_id].height;
}

void surface_resize(int32_t surf_id, int w, int h) {
    if (!surface_exists(surf_id)) return;
    surface_free(surf_id);
    surface_create(w, h);
}

void draw_surface(int32_t surf_id, double x, double y) {
    draw_surface_ext(surf_id, x, y, 1.0, 1.0, 0.0, s_draw_color, s_draw_alpha);
}

void draw_surface_ext(int32_t surf_id, double x, double y,
                      double xscale, double yscale, double rot,
                      uint32_t col, double alpha) {
    if (!surface_exists(surf_id)) return;
    Surface *surf = &s_surfaces[surf_id];
    float w = (float)surf->width;
    float h = (float)surf->height;

    float x0 = (float)x; float y0 = (float)y;
    float x1 = x0 + w * (float)xscale;
    float y1 = y0 + h * (float)yscale;

    /* In OpenGL, FBO textures are right-side-up, so v0=0, v1=1 (or inverted if GM) */
    batch_quad(surf->gl_texture, x0, y0, 0, 0, x1, y0, 1, 0, x1, y1, 1, 1, x0, y1, 0, 1, col, (float)alpha);
}

/* ================= Shaders ================= */

void shader_set(int32_t shader_idx) {
    graphics_flush();
    if (shader_idx < 0 || (uint32_t)shader_idx >= g_DataWin.shader_count) {
        glUseProgram(s_default_program);
        return;
    }
    ShaderDef *shd = &g_DataWin.shaders[shader_idx];
    if (shd->gl_program == 0) {
        /* Compile shader on demand */
        GLuint vs = compile_shader(GL_VERTEX_SHADER, shd->vertex_code);
        GLuint fs = compile_shader(GL_FRAGMENT_SHADER, shd->pixel_code);
        if (vs && fs) {
            shd->gl_program = glCreateProgram();
            glAttachShader(shd->gl_program, vs);
            glAttachShader(shd->gl_program, fs);
            glLinkProgram(shd->gl_program);
            glDeleteShader(vs);
            glDeleteShader(fs);
        }
    }
    if (shd->gl_program != 0) {
        glUseProgram(shd->gl_program);
    }
}

void shader_reset(void) {
    graphics_flush();
    glUseProgram(s_default_program);
}

int32_t shader_get_uniform(int32_t shader_idx, const char *name) {
    if (shader_idx < 0 || (uint32_t)shader_idx >= g_DataWin.shader_count) return -1;
    ShaderDef *shd = &g_DataWin.shaders[shader_idx];
    if (shd->gl_program == 0) shader_set(shader_idx);
    if (shd->gl_program == 0) return -1;
    return (int32_t)glGetUniformLocation(shd->gl_program, name);
}

int32_t shader_get_sampler_index(int32_t shader_idx, const char *name) {
    return shader_get_uniform(shader_idx, name);
}

void shader_set_uniform_f(int32_t uniform_loc, float v0) {
    if (uniform_loc < 0) return;
    graphics_flush();
    glUniform1f(uniform_loc, v0);
}

void shader_set_uniform_f_array(int32_t uniform_loc, const float *vals, int count) {
    if (uniform_loc < 0 || !vals || count <= 0) return;
    graphics_flush();
    if (count == 1) glUniform1f(uniform_loc, vals[0]);
    else if (count == 4) glUniform4f(uniform_loc, vals[0], vals[1], vals[2], vals[3]);
}

/* ================= Tiles ================= */

void graphics_draw_tile(int32_t tileset_idx, uint32_t tile_raw, float x, float y) {
    uint32_t tile_id = tile_raw & 0x07ffffff;
    if (tile_id == 0) return; /* 0 = empty / transparent */

    TilesetDef *ts = data_win_get_tileset(&g_DataWin, tileset_idx);
    if (!ts || ts->tile_columns == 0 || ts->tile_width == 0 || ts->tile_height == 0) return;

    TPAGItem tpag = data_win_read_tpag(&g_DataWin, ts->tpag_offset);
    uint32_t gl_tex = graphics_get_or_upload_texture(tpag.tex_id);
    if (!gl_tex) return;

    float tex_w = (float)g_DataWin.textures[tpag.tex_id].width;
    float tex_h = (float)g_DataWin.textures[tpag.tex_id].height;
    if (tex_w <= 0.0f || tex_h <= 0.0f) return;

    uint32_t col = tile_id % ts->tile_columns;
    uint32_t row = tile_id / ts->tile_columns;
    uint32_t cell_w = ts->tile_width + 2 * ts->border_x;
    uint32_t cell_h = ts->tile_height + 2 * ts->border_y;

    float src_x = (float)(tpag.x + col * cell_w + ts->border_x);
    float src_y = (float)(tpag.y + row * cell_h + ts->border_y);
    float tw = (float)ts->tile_width;
    float th = (float)ts->tile_height;

    float u0 = src_x / tex_w;
    float v0 = src_y / tex_h;
    float u1 = (src_x + tw) / tex_w;
    float v1 = (src_y + th) / tex_h;

    bool flip_h = (tile_raw & 0x10000000) != 0;
    bool flip_v = (tile_raw & 0x20000000) != 0;
    if (flip_h) { float tmp = u0; u0 = u1; u1 = tmp; }
    if (flip_v) { float tmp = v0; v0 = v1; v1 = tmp; }

    batch_quad(gl_tex, x, y, u0, v0, x + tw, y, u1, v0, x + tw, y + th, u1, v1, x, y + th, u0, v1, 0xffffffff, 1.0f);
}

/* ================= Fonts & Text ================= */

void draw_set_font(int32_t font_idx) {
    s_current_font = font_idx;
}

int32_t draw_get_font(void) {
    return s_current_font;
}

static const FontGlyph *find_glyph(const FontDef *fnt, uint16_t ch) {
    if (!fnt || !fnt->glyphs || fnt->glyph_count == 0) return NULL;
    int left = 0;
    int right = (int)fnt->glyph_count - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (fnt->glyphs[mid].character == ch) {
            return &fnt->glyphs[mid];
        }
        if (fnt->glyphs[mid].character < ch) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return NULL;
}

void draw_text_transformed_color(double x, double y, const char *str, double xscale, double yscale, double angle,
                                  uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4, double alpha) {
    (void)angle; (void)c2; (void)c3; (void)c4;
    if (!str) return;

    int font_id = s_current_font >= 0 ? s_current_font : 1;
    FontDef *fnt = data_win_get_font(&g_DataWin, font_id);
    if (!fnt) fnt = data_win_get_font(&g_DataWin, 1);
    if (!fnt || !fnt->glyphs || fnt->glyph_count == 0) return;

    TPAGItem tpag = data_win_read_tpag(&g_DataWin, fnt->tpag_offset);
    uint32_t gl_tex = graphics_get_or_upload_texture(tpag.tex_id);
    if (!gl_tex) return;

    float tex_w = (float)g_DataWin.textures[tpag.tex_id].width;
    float tex_h = (float)g_DataWin.textures[tpag.tex_id].height;
    if (tex_w <= 0.0f || tex_h <= 0.0f) return;

    float cur_x = (float)x;
    float cur_y = (float)y;
    float xs = (float)xscale;
    float ys = (float)yscale;

    for (const char *p = str; *p; p++) {
        uint16_t ch = (uint16_t)(unsigned char)*p;
        if (ch == '\n') {
            cur_x = (float)x;
            cur_y += 18.0f * ys;
            continue;
        }

        const FontGlyph *glyph = find_glyph(fnt, ch);
        if (!glyph) glyph = find_glyph(fnt, (uint16_t)'?');
        if (!glyph) {
            cur_x += 8.0f * xs;
            continue;
        }

        if (glyph->src_w > 0 && glyph->src_h > 0) {
            float gx = (float)(tpag.x + glyph->src_x);
            float gy = (float)(tpag.y + glyph->src_y);
            float gw = (float)glyph->src_w;
            float gh = (float)glyph->src_h;

            float u0 = gx / tex_w;
            float v0 = gy / tex_h;
            float u1 = (gx + gw) / tex_w;
            float v1 = (gy + gh) / tex_h;

            float qx = cur_x + (float)glyph->offset * xs;
            float qy = cur_y;
            float qw = gw * xs;
            float qh = gh * ys;

            batch_quad(gl_tex, qx, qy, u0, v0, qx + qw, qy, u1, v0, qx + qw, qy + qh, u1, v1, qx, qy + qh, u0, v1, c1, (float)alpha);
        }
        cur_x += (float)glyph->shift * xs;
    }
}

void draw_text(double x, double y, const char *str) {
    draw_text_color(x, y, str, s_draw_color, s_draw_color, s_draw_color, s_draw_color, s_draw_alpha);
}

void draw_text_color(double x, double y, const char *str, uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4, double alpha) {
    draw_text_transformed_color(x, y, str, 1.0, 1.0, 0.0, c1, c2, c3, c4, alpha);
}

void draw_text_transformed(double x, double y, const char *str, double xscale, double yscale, double angle) {
    draw_text_transformed_color(x, y, str, xscale, yscale, angle, s_draw_color, s_draw_color, s_draw_color, s_draw_color, s_draw_alpha);
}

float string_width(const char *str) {
    if (!str) return 0.0f;
    int font_id = s_current_font >= 0 ? s_current_font : 1;
    FontDef *fnt = data_win_get_font(&g_DataWin, font_id);
    if (!fnt) fnt = data_win_get_font(&g_DataWin, 1);
    if (!fnt) return (float)strlen(str) * 8.0f;

    float max_w = 0.0f;
    float cur_w = 0.0f;
    for (const char *p = str; *p; p++) {
        uint16_t ch = (uint16_t)(unsigned char)*p;
        if (ch == '\n') {
            if (cur_w > max_w) max_w = cur_w;
            cur_w = 0.0f;
            continue;
        }
        const FontGlyph *glyph = find_glyph(fnt, ch);
        cur_w += glyph ? (float)glyph->shift : 8.0f;
    }
    return cur_w > max_w ? cur_w : max_w;
}

float string_height(const char *str) {
    if (!str) return 0.0f;
    int lines = 1;
    for (const char *p = str; *p; p++) {
        if (*p == '\n') lines++;
    }
    return (float)lines * 18.0f;
}
