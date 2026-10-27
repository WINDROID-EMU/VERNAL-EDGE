#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* Storage Directory for Saves & Config */
static char s_save_directory[512] = "";
static char s_asset_directory[512] = "";

/* File Handles Pool */
static FILE* s_file_handles[MAX_FILE_HANDLES];

/* Buffer Structure */
typedef struct {
    int id;
    uint8_t* data;
    size_t size;
    size_t capacity;
    size_t alignment;
    int type;
    size_t seek_pos;
    bool in_use;
} BufferSlot;

static BufferSlot s_buffers[MAX_BUFFERS];
static int32_t s_async_counter = 1000;

/* -------------------------------------------------------------
 * Lifecycle & Path Resolution
 * ------------------------------------------------------------- */
void files_init(const char* save_dir) {
    if (save_dir && strlen(save_dir) > 0) {
        strncpy(s_save_directory, save_dir, sizeof(s_save_directory) - 1);
        /* Ensure trailing slash if missing */
        size_t len = strlen(s_save_directory);
        if (len > 0 && s_save_directory[len - 1] != '/' && len < sizeof(s_save_directory) - 2) {
            s_save_directory[len] = '/';
            s_save_directory[len + 1] = '\0';
        }
    } else if (s_save_directory[0] == '\0') {
        s_save_directory[0] = '\0';
    }

    memset(s_file_handles, 0, sizeof(s_file_handles));
    memset(s_buffers, 0, sizeof(s_buffers));
    s_async_counter = 1000;

    printf("[Files] Subsystem initialized. Save directory: '%s'\n",
           s_save_directory[0] ? s_save_directory : "./ (current directory)");
}

void files_shutdown(void) {
    /* Close any lingering open file handles */
    for (int i = 0; i < MAX_FILE_HANDLES; i++) {
        if (s_file_handles[i]) {
            fclose(s_file_handles[i]);
            s_file_handles[i] = NULL;
        }
    }

    /* Free all allocated buffers */
    for (int i = 0; i < MAX_BUFFERS; i++) {
        if (s_buffers[i].in_use && s_buffers[i].data) {
            free(s_buffers[i].data);
            s_buffers[i].data = NULL;
            s_buffers[i].in_use = false;
        }
    }

    printf("[Files] Subsystem shut down cleanly\n");
}

void files_set_save_directory(const char* dir) {
    files_init(dir);
}

const char* files_get_save_directory(void) {
    return s_save_directory;
}

void files_set_asset_directory(const char* dir) {
    if (dir && strlen(dir) > 0) {
        strncpy(s_asset_directory, dir, sizeof(s_asset_directory) - 1);
        size_t len = strlen(s_asset_directory);
        if (len > 0 && s_asset_directory[len - 1] != '/' && len < sizeof(s_asset_directory) - 2) {
            s_asset_directory[len] = '/';
            s_asset_directory[len + 1] = '\0';
        }
    } else {
        s_asset_directory[0] = '\0';
    }
}

const char* files_get_asset_directory(void) {
    return s_asset_directory;
}

void files_resolve_path(const char* in_path, char* out_path, size_t max_len) {
    if (!in_path || !out_path || max_len == 0) return;

    /* If path is already absolute, use as is */
    if (in_path[0] == '/') {
        strncpy(out_path, in_path, max_len - 1);
        out_path[max_len - 1] = '\0';
        return;
    }

    /* If save directory is configured, prefix it */
    if (s_save_directory[0] != '\0') {
        snprintf(out_path, max_len, "%s%s", s_save_directory, in_path);
    } else {
        strncpy(out_path, in_path, max_len - 1);
        out_path[max_len - 1] = '\0';
    }
}

/* -------------------------------------------------------------
 * File System Operations
 * ------------------------------------------------------------- */
bool file_exists(const char* fname) {
    if (!fname || !fname[0]) return false;

    char resolved[512];
    files_resolve_path(fname, resolved, sizeof(resolved));

    if (access(resolved, F_OK) == 0) return true;

    /* Fallback: check in current working directory */
    if (access(fname, F_OK) == 0) return true;

    return false;
}

bool file_delete(const char* fname) {
    if (!fname || !fname[0]) return false;

    char resolved[512];
    files_resolve_path(fname, resolved, sizeof(resolved));
    return remove(resolved) == 0;
}

bool file_copy(const char* src, const char* dest) {
    if (!src || !dest) return false;

    char src_res[512], dest_res[512];
    files_resolve_path(src, src_res, sizeof(src_res));
    files_resolve_path(dest, dest_res, sizeof(dest_res));

    FILE* f_in = fopen(src_res, "rb");
    if (!f_in) {
        /* Try raw src */
        f_in = fopen(src, "rb");
        if (!f_in) return false;
    }

    FILE* f_out = fopen(dest_res, "wb");
    if (!f_out) {
        fclose(f_in);
        return false;
    }

    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f_in)) > 0) {
        fwrite(buf, 1, n, f_out);
    }

    fclose(f_in);
    fclose(f_out);
    return true;
}

/* -------------------------------------------------------------
 * Text File API
 * ------------------------------------------------------------- */
static int32_t AllocFileHandle(FILE* f) {
    for (int i = 0; i < MAX_FILE_HANDLES; i++) {
        if (!s_file_handles[i]) {
            s_file_handles[i] = f;
            return i + 1; /* 1-based handle */
        }
    }
    fclose(f);
    return -1;
}

static inline FILE* GetFileHandle(int32_t file_id) {
    if (file_id >= 1 && file_id <= MAX_FILE_HANDLES) {
        return s_file_handles[file_id - 1];
    }
    return NULL;
}

int32_t file_text_open_read(const char* fname) {
    if (!fname) return -1;
    char path[512];
    files_resolve_path(fname, path, sizeof(path));

    FILE* f = fopen(path, "r");
    if (!f) {
        f = fopen(fname, "r");
    }
    if (!f) return -1;
    return AllocFileHandle(f);
}

int32_t file_text_open_write(const char* fname) {
    if (!fname) return -1;
    char path[512];
    files_resolve_path(fname, path, sizeof(path));

    FILE* f = fopen(path, "w");
    if (!f) return -1;
    return AllocFileHandle(f);
}

int32_t file_text_open_append(const char* fname) {
    if (!fname) return -1;
    char path[512];
    files_resolve_path(fname, path, sizeof(path));

    FILE* f = fopen(path, "a");
    if (!f) return -1;
    return AllocFileHandle(f);
}

void file_text_close(int32_t file_id) {
    FILE* f = GetFileHandle(file_id);
    if (f) {
        fclose(f);
        s_file_handles[file_id - 1] = NULL;
    }
}

bool file_text_eof(int32_t file_id) {
    FILE* f = GetFileHandle(file_id);
    if (!f) return true;
    int c = fgetc(f);
    if (c == EOF) return true;
    ungetc(c, f);
    return false;
}

bool file_text_eoln(int32_t file_id) {
    FILE* f = GetFileHandle(file_id);
    if (!f) return true;
    int c = fgetc(f);
    if (c == EOF || c == '\n' || c == '\r') {
        if (c != EOF) ungetc(c, f);
        return true;
    }
    ungetc(c, f);
    return false;
}

void file_text_write_string(int32_t file_id, const char* str) {
    FILE* f = GetFileHandle(file_id);
    if (f && str) {
        fputs(str, f);
    }
}

void file_text_writeln(int32_t file_id) {
    FILE* f = GetFileHandle(file_id);
    if (f) {
        fputc('\n', f);
    }
}

void file_text_readln(int32_t file_id, char* out_buf, size_t max_len) {
    if (!out_buf || max_len == 0) return;
    out_buf[0] = '\0';
    FILE* f = GetFileHandle(file_id);
    if (!f) return;

    size_t idx = 0;
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\r') {
            /* Check next for \n */
            int next = fgetc(f);
            if (next != '\n' && next != EOF) ungetc(next, f);
            break;
        }
        if (c == '\n') break;

        if (idx + 1 < max_len) {
            out_buf[idx++] = (char)c;
        }
    }
    out_buf[idx] = '\0';
}

void file_text_read_string(int32_t file_id, char* out_buf, size_t max_len) {
    file_text_readln(file_id, out_buf, max_len);
}

double file_text_read_real(int32_t file_id) {
    FILE* f = GetFileHandle(file_id);
    if (!f) return 0.0;
    double val = 0.0;
    if (fscanf(f, "%lf", &val) == 1) {
        return val;
    }
    return 0.0;
}

/* -------------------------------------------------------------
 * Buffer Subsystem
 * ------------------------------------------------------------- */
static inline BufferSlot* GetBuffer(int32_t id) {
    if (id >= 0 && id < MAX_BUFFERS && s_buffers[id].in_use) {
        return &s_buffers[id];
    }
    return NULL;
}

static bool BufferEnsureCapacity(BufferSlot* buf, size_t needed) {
    if (needed <= buf->capacity) return true;
    if (buf->type != BUFFER_GROW) return false;

    size_t new_cap = buf->capacity ? buf->capacity * 2 : 1024;
    while (new_cap < needed) new_cap *= 2;

    uint8_t* new_data = (uint8_t*)realloc(buf->data, new_cap);
    if (!new_data) return false;

    memset(new_data + buf->capacity, 0, new_cap - buf->capacity);
    buf->data = new_data;
    buf->capacity = new_cap;
    return true;
}

int32_t buffer_create(size_t size, int32_t type, size_t alignment) {
    for (int i = 0; i < MAX_BUFFERS; i++) {
        if (!s_buffers[i].in_use) {
            size_t cap = size > 0 ? size : 256;
            uint8_t* data = (uint8_t*)calloc(1, cap);
            if (!data) return -1;

            s_buffers[i].id = i;
            s_buffers[i].data = data;
            s_buffers[i].size = size;
            s_buffers[i].capacity = cap;
            s_buffers[i].alignment = alignment > 0 ? alignment : 1;
            s_buffers[i].type = type;
            s_buffers[i].seek_pos = 0;
            s_buffers[i].in_use = true;
            return i;
        }
    }
    return -1;
}

void buffer_delete(int32_t id) {
    BufferSlot* buf = GetBuffer(id);
    if (buf) {
        if (buf->data) free(buf->data);
        memset(buf, 0, sizeof(BufferSlot));
    }
}

bool buffer_exists(int32_t id) {
    return GetBuffer(id) != NULL;
}

size_t buffer_get_size(int32_t id) {
    BufferSlot* buf = GetBuffer(id);
    return buf ? buf->size : 0;
}

void buffer_resize(int32_t id, size_t new_size) {
    BufferSlot* buf = GetBuffer(id);
    if (!buf) return;

    if (new_size > buf->capacity) {
        uint8_t* nd = (uint8_t*)realloc(buf->data, new_size);
        if (!nd) return;
        memset(nd + buf->capacity, 0, new_size - buf->capacity);
        buf->data = nd;
        buf->capacity = new_size;
    }
    buf->size = new_size;
    if (buf->seek_pos > buf->size) buf->seek_pos = buf->size;
}

size_t buffer_seek(int32_t id, int32_t base, int64_t offset) {
    BufferSlot* buf = GetBuffer(id);
    if (!buf) return 0;

    int64_t target = 0;
    switch (base) {
        case BUFFER_SEEK_START:
            target = offset;
            break;
        case BUFFER_SEEK_RELATIVE:
            target = (int64_t)buf->seek_pos + offset;
            break;
        case BUFFER_SEEK_END:
            target = (int64_t)buf->size + offset;
            break;
        default:
            target = offset;
            break;
    }

    if (target < 0) target = 0;
    buf->seek_pos = (size_t)target;
    return buf->seek_pos;
}

size_t buffer_tell(int32_t id) {
    BufferSlot* buf = GetBuffer(id);
    return buf ? buf->seek_pos : 0;
}

bool buffer_peek(int32_t id, size_t offset, int32_t type, RValue* out_val) {
    BufferSlot* buf = GetBuffer(id);
    if (!buf || !out_val) return false;
    RValue_Init(out_val);

    if (offset >= buf->size && type != BUFFER_STRING && type != BUFFER_TEXT) {
        RValue_SetReal(out_val, 0.0);
        return false;
    }

    uint8_t* p = buf->data + offset;

    switch (type) {
        case BUFFER_U8:
            RValue_SetReal(out_val, (double)*p);
            return true;
        case BUFFER_S8:
            RValue_SetReal(out_val, (double)*(int8_t*)p);
            return true;
        case BUFFER_U16:
            if (offset + 2 <= buf->size) {
                uint16_t v; memcpy(&v, p, 2);
                RValue_SetReal(out_val, (double)v);
                return true;
            }
            break;
        case BUFFER_S16:
            if (offset + 2 <= buf->size) {
                int16_t v; memcpy(&v, p, 2);
                RValue_SetReal(out_val, (double)v);
                return true;
            }
            break;
        case BUFFER_U32:
            if (offset + 4 <= buf->size) {
                uint32_t v; memcpy(&v, p, 4);
                RValue_SetReal(out_val, (double)v);
                return true;
            }
            break;
        case BUFFER_S32:
            if (offset + 4 <= buf->size) {
                int32_t v; memcpy(&v, p, 4);
                RValue_SetReal(out_val, (double)v);
                return true;
            }
            break;
        case BUFFER_U64:
            if (offset + 8 <= buf->size) {
                uint64_t v; memcpy(&v, p, 8);
                RValue_SetInt64(out_val, (int64_t)v);
                return true;
            }
            break;
        case BUFFER_F32:
            if (offset + 4 <= buf->size) {
                float v; memcpy(&v, p, 4);
                RValue_SetReal(out_val, (double)v);
                return true;
            }
            break;
        case BUFFER_F64:
            if (offset + 8 <= buf->size) {
                double v; memcpy(&v, p, 8);
                RValue_SetReal(out_val, v);
                return true;
            }
            break;
        case BUFFER_BOOL:
            RValue_SetBool(out_val, *p != 0);
            return true;
        case BUFFER_STRING:
        case BUFFER_TEXT: {
            if (offset >= buf->size) {
                RValue_SetString(out_val, "");
                return true;
            }
            size_t max_s = buf->size - offset;
            char* tmp = (char*)malloc(max_s + 1);
            if (!tmp) return false;
            size_t len = 0;
            while (len < max_s && p[len] != '\0') {
                tmp[len] = (char)p[len];
                len++;
            }
            tmp[len] = '\0';
            RValue_SetString(out_val, tmp);
            free(tmp);
            return true;
        }
    }

    RValue_SetReal(out_val, 0.0);
    return false;
}

bool buffer_poke(int32_t id, size_t offset, int32_t type, const RValue* val) {
    BufferSlot* buf = GetBuffer(id);
    if (!buf || !val) return false;

    size_t req_size = offset;
    switch (type) {
        case BUFFER_U8:
        case BUFFER_S8:
        case BUFFER_BOOL:
            req_size += 1;
            break;
        case BUFFER_U16:
        case BUFFER_S16:
        case BUFFER_F16:
            req_size += 2;
            break;
        case BUFFER_U32:
        case BUFFER_S32:
        case BUFFER_F32:
            req_size += 4;
            break;
        case BUFFER_U64:
        case BUFFER_F64:
            req_size += 8;
            break;
        case BUFFER_STRING: {
            const char* s = RValue_AsString(val);
            req_size += (s ? strlen(s) + 1 : 1);
            break;
        }
        case BUFFER_TEXT: {
            const char* s = RValue_AsString(val);
            req_size += (s ? strlen(s) : 0);
            break;
        }
        default:
            req_size += 1;
            break;
    }

    if (!BufferEnsureCapacity(buf, req_size)) return false;
    if (req_size > buf->size) buf->size = req_size;

    uint8_t* p = buf->data + offset;
    switch (type) {
        case BUFFER_U8:
            *p = (uint8_t)RValue_AsReal(val);
            break;
        case BUFFER_S8:
            *(int8_t*)p = (int8_t)RValue_AsReal(val);
            break;
        case BUFFER_BOOL:
            *p = RValue_AsBool(val) ? 1 : 0;
            break;
        case BUFFER_U16: {
            uint16_t v = (uint16_t)RValue_AsReal(val);
            memcpy(p, &v, 2);
            break;
        }
        case BUFFER_S16: {
            int16_t v = (int16_t)RValue_AsReal(val);
            memcpy(p, &v, 2);
            break;
        }
        case BUFFER_U32: {
            uint32_t v = (uint32_t)RValue_AsReal(val);
            memcpy(p, &v, 4);
            break;
        }
        case BUFFER_S32: {
            int32_t v = (int32_t)RValue_AsReal(val);
            memcpy(p, &v, 4);
            break;
        }
        case BUFFER_U64: {
            int64_t v = RValue_AsInt64(val);
            memcpy(p, &v, 8);
            break;
        }
        case BUFFER_F32: {
            float v = (float)RValue_AsReal(val);
            memcpy(p, &v, 4);
            break;
        }
        case BUFFER_F64: {
            double v = RValue_AsReal(val);
            memcpy(p, &v, 8);
            break;
        }
        case BUFFER_STRING: {
            const char* s = RValue_AsString(val);
            if (s) {
                size_t l = strlen(s);
                memcpy(p, s, l + 1);
            } else {
                *p = '\0';
            }
            break;
        }
        case BUFFER_TEXT: {
            const char* s = RValue_AsString(val);
            if (s) {
                size_t l = strlen(s);
                memcpy(p, s, l);
            }
            break;
        }
    }
    return true;
}

bool buffer_read(int32_t id, int32_t type, RValue* out_val) {
    BufferSlot* buf = GetBuffer(id);
    if (!buf) return false;

    bool ok = buffer_peek(id, buf->seek_pos, type, out_val);
    if (ok) {
        switch (type) {
            case BUFFER_U8:
            case BUFFER_S8:
            case BUFFER_BOOL:
                buf->seek_pos += 1;
                break;
            case BUFFER_U16:
            case BUFFER_S16:
            case BUFFER_F16:
                buf->seek_pos += 2;
                break;
            case BUFFER_U32:
            case BUFFER_S32:
            case BUFFER_F32:
                buf->seek_pos += 4;
                break;
            case BUFFER_U64:
            case BUFFER_F64:
                buf->seek_pos += 8;
                break;
            case BUFFER_STRING: {
                const char* s = RValue_AsString(out_val);
                buf->seek_pos += (s ? strlen(s) + 1 : 1);
                break;
            }
            case BUFFER_TEXT: {
                const char* s = RValue_AsString(out_val);
                buf->seek_pos += (s ? strlen(s) : 0);
                break;
            }
            default:
                buf->seek_pos += 1;
                break;
        }
    }
    return ok;
}

int32_t buffer_write(int32_t id, int32_t type, const RValue* val) {
    BufferSlot* buf = GetBuffer(id);
    if (!buf || !val) return 0;

    size_t start_pos = buf->seek_pos;
    if (!buffer_poke(id, start_pos, type, val)) return 0;

    size_t bytes_written = 0;
    switch (type) {
        case BUFFER_U8:
        case BUFFER_S8:
        case BUFFER_BOOL:
            bytes_written = 1;
            break;
        case BUFFER_U16:
        case BUFFER_S16:
        case BUFFER_F16:
            bytes_written = 2;
            break;
        case BUFFER_U32:
        case BUFFER_S32:
        case BUFFER_F32:
            bytes_written = 4;
            break;
        case BUFFER_U64:
        case BUFFER_F64:
            bytes_written = 8;
            break;
        case BUFFER_STRING: {
            const char* s = RValue_AsString(val);
            bytes_written = s ? strlen(s) + 1 : 1;
            break;
        }
        case BUFFER_TEXT: {
            const char* s = RValue_AsString(val);
            bytes_written = s ? strlen(s) : 0;
            break;
        }
        default:
            bytes_written = 1;
            break;
    }

    buf->seek_pos += bytes_written;
    if (buf->seek_pos > buf->size) {
        buf->size = buf->seek_pos;
    }
    return (int32_t)bytes_written;
}

void buffer_copy(int32_t src_id, size_t src_offset, size_t size, int32_t dest_id, size_t dest_offset) {
    BufferSlot* src = GetBuffer(src_id);
    BufferSlot* dst = GetBuffer(dest_id);
    if (!src || !dst || size == 0) return;

    if (src_offset >= src->size) return;
    if (src_offset + size > src->size) size = src->size - src_offset;

    size_t needed = dest_offset + size;
    if (!BufferEnsureCapacity(dst, needed)) return;
    if (needed > dst->size) dst->size = needed;

    memcpy(dst->data + dest_offset, src->data + src_offset, size);
}

bool buffer_save(int32_t id, const char* filename) {
    BufferSlot* buf = GetBuffer(id);
    if (!buf || !filename) return false;

    char path[512];
    files_resolve_path(filename, path, sizeof(path));

    FILE* f = fopen(path, "wb");
    if (!f) return false;

    if (buf->size > 0 && buf->data) {
        fwrite(buf->data, 1, buf->size, f);
    }
    fclose(f);
    return true;
}

int32_t buffer_load(const char* filename) {
    if (!filename) return -1;

    char path[512];
    files_resolve_path(filename, path, sizeof(path));

    FILE* f = fopen(path, "rb");
    if (!f) {
        f = fopen(filename, "rb");
    }
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fsize < 0) {
        fclose(f);
        return -1;
    }

    int32_t id = buffer_create((size_t)fsize, BUFFER_GROW, 1);
    if (id >= 0) {
        BufferSlot* buf = GetBuffer(id);
        if (buf && fsize > 0) {
            fread(buf->data, 1, (size_t)fsize, f);
        }
    }
    fclose(f);
    return id;
}

/* -------------------------------------------------------------
 * Async Buffer Operations
 * ------------------------------------------------------------- */
int32_t buffer_save_async(int32_t id, const char* filename, size_t offset, size_t size) {
    BufferSlot* buf = GetBuffer(id);
    if (!buf || !filename) return -1;

    char path[512];
    files_resolve_path(filename, path, sizeof(path));

    FILE* f = fopen(path, "wb");
    if (!f) return -1;

    if (offset < buf->size) {
        size_t available = buf->size - offset;
        size_t to_write = size > 0 && size < available ? size : available;
        fwrite(buf->data + offset, 1, to_write, f);
    }
    fclose(f);
    return s_async_counter++;
}

int32_t buffer_load_async(int32_t id, const char* filename, size_t offset, size_t size) {
    BufferSlot* buf = GetBuffer(id);
    if (!buf || !filename) return -1;

    char path[512];
    files_resolve_path(filename, path, sizeof(path));

    FILE* f = fopen(path, "rb");
    if (!f) f = fopen(filename, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fsize > 0) {
        size_t needed = offset + (size_t)fsize;
        if (BufferEnsureCapacity(buf, needed)) {
            fread(buf->data + offset, 1, (size_t)fsize, f);
            if (needed > buf->size) buf->size = needed;
        }
    }
    fclose(f);
    return s_async_counter++;
}

void buffer_async_group_begin(const char* group_name) {
    (void)group_name;
}

void buffer_async_group_option(const char* option, const char* value) {
    (void)option; (void)value;
}

int32_t buffer_async_group_end(void) {
    return s_async_counter++;
}

/* -------------------------------------------------------------
 * Checksums: SHA-1 & CRC32 (Self-contained)
 * ------------------------------------------------------------- */
typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    uint8_t buffer[64];
} SHA1_CTX;

static void SHA1Transform(uint32_t state[5], const uint8_t buffer[64]) {
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];
    uint32_t w[80];

    for (int i = 0; i < 16; i++) {
        w[i] = ((uint32_t)buffer[i * 4] << 24) |
               ((uint32_t)buffer[i * 4 + 1] << 16) |
               ((uint32_t)buffer[i * 4 + 2] << 8) |
               ((uint32_t)buffer[i * 4 + 3]);
    }
    for (int i = 16; i < 80; i++) {
        uint32_t v = w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16];
        w[i] = (v << 1) | (v >> 31);
    }

    for (int i = 0; i < 80; i++) {
        uint32_t f, k;
        if (i < 20) {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999;
        } else if (i < 40) {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
        } else if (i < 60) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        } else {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }
        uint32_t temp = ((a << 5) | (a >> 27)) + f + e + k + w[i];
        e = d;
        d = c;
        c = (b << 30) | (b >> 2);
        b = a;
        a = temp;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

static void SHA1Init(SHA1_CTX* ctx) {
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
    ctx->count[0] = ctx->count[1] = 0;
}

static void SHA1Update(SHA1_CTX* ctx, const uint8_t* data, size_t len) {
    uint32_t i, j;
    j = (ctx->count[0] >> 3) & 63;
    if ((ctx->count[0] += (uint32_t)(len << 3)) < (len << 3)) ctx->count[1]++;
    ctx->count[1] += (uint32_t)(len >> 29);
    if ((j + len) > 63) {
        memcpy(&ctx->buffer[j], data, (i = 64 - j));
        SHA1Transform(ctx->state, ctx->buffer);
        for (; i + 63 < len; i += 64) {
            SHA1Transform(ctx->state, &data[i]);
        }
        j = 0;
    } else i = 0;
    memcpy(&ctx->buffer[j], &data[i], len - i);
}

static void SHA1Final(uint8_t digest[20], SHA1_CTX* ctx) {
    uint8_t finalcount[8];
    for (int i = 0; i < 8; i++) {
        finalcount[i] = (uint8_t)((ctx->count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);
    }
    SHA1Update(ctx, (const uint8_t*)"\x80", 1);
    while ((ctx->count[0] & 504) != 448) {
        SHA1Update(ctx, (const uint8_t*)"\0", 1);
    }
    SHA1Update(ctx, finalcount, 8);
    for (int i = 0; i < 20; i++) {
        digest[i] = (uint8_t)((ctx->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
    }
}

void buffer_sha1(int32_t id, size_t offset, size_t size, char out_sha1[41]) {
    BufferSlot* buf = GetBuffer(id);
    if (!buf || !out_sha1) {
        if (out_sha1) out_sha1[0] = '\0';
        return;
    }

    if (offset >= buf->size) {
        out_sha1[0] = '\0';
        return;
    }
    if (offset + size > buf->size || size == 0) {
        size = buf->size - offset;
    }

    SHA1_CTX ctx;
    SHA1Init(&ctx);
    SHA1Update(&ctx, buf->data + offset, size);
    uint8_t digest[20];
    SHA1Final(digest, &ctx);

    for (int i = 0; i < 20; i++) {
        sprintf(out_sha1 + (i * 2), "%02x", digest[i]);
    }
    out_sha1[40] = '\0';
}

uint32_t buffer_crc32(int32_t id, size_t offset, size_t size) {
    BufferSlot* buf = GetBuffer(id);
    if (!buf || offset >= buf->size) return 0;
    if (offset + size > buf->size || size == 0) {
        size = buf->size - offset;
    }

    uint32_t crc = 0xFFFFFFFF;
    const uint8_t* p = buf->data + offset;
    for (size_t i = 0; i < size; i++) {
        crc ^= p[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & (-(int32_t)(crc & 1)));
        }
    }
    return ~crc;
}

/* -------------------------------------------------------------
 * Engine Compatibility Hook
 * ------------------------------------------------------------- */
int32_t FUN_1418884f0(int32_t buffer_id, int32_t type, const RValue* val) {
    return buffer_write(buffer_id, type, val);
}
