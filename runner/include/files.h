#ifndef RUNNER_FILES_H
#define RUNNER_FILES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "rvalue.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Buffer Types */
#define BUFFER_FIXED    0
#define BUFFER_GROW     1
#define BUFFER_WRAP     2
#define BUFFER_FAST     3
#define BUFFER_VBUFFER  4

/* Buffer Data Types */
#define BUFFER_U8       1
#define BUFFER_S8       2
#define BUFFER_U16      3
#define BUFFER_S16      4
#define BUFFER_U32      5
#define BUFFER_S32      6
#define BUFFER_U64      7
#define BUFFER_F16      8
#define BUFFER_F32      9
#define BUFFER_F64      10
#define BUFFER_BOOL     11
#define BUFFER_STRING   12
#define BUFFER_TEXT     13

/* Buffer Seek Constants */
#define BUFFER_SEEK_START     0
#define BUFFER_SEEK_RELATIVE  1
#define BUFFER_SEEK_END       2

#define MAX_BUFFERS      256
#define MAX_FILE_HANDLES 32

/* Lifecycle */
void files_init(const char* save_dir);
void files_shutdown(void);
void files_set_save_directory(const char* dir);
const char* files_get_save_directory(void);
void files_set_asset_directory(const char* dir);
const char* files_get_asset_directory(void);
void files_resolve_path(const char* in_path, char* out_path, size_t max_len);

/* File Operations */
bool file_exists(const char* fname);
bool file_delete(const char* fname);
bool file_copy(const char* src, const char* dest);

/* Text File API */
int32_t file_text_open_read(const char* fname);
int32_t file_text_open_write(const char* fname);
int32_t file_text_open_append(const char* fname);
void    file_text_close(int32_t file_id);
bool    file_text_eof(int32_t file_id);
bool    file_text_eoln(int32_t file_id);
void    file_text_write_string(int32_t file_id, const char* str);
void    file_text_writeln(int32_t file_id);
void    file_text_readln(int32_t file_id, char* out_buf, size_t max_len);
void    file_text_read_string(int32_t file_id, char* out_buf, size_t max_len);
double  file_text_read_real(int32_t file_id);

/* Buffer API */
int32_t buffer_create(size_t size, int32_t type, size_t alignment);
void    buffer_delete(int32_t id);
bool    buffer_exists(int32_t id);
size_t  buffer_get_size(int32_t id);
void    buffer_resize(int32_t id, size_t new_size);
size_t  buffer_seek(int32_t id, int32_t base, int64_t offset);
size_t  buffer_tell(int32_t id);
bool    buffer_peek(int32_t id, size_t offset, int32_t type, RValue* out_val);
bool    buffer_poke(int32_t id, size_t offset, int32_t type, const RValue* val);
bool    buffer_read(int32_t id, int32_t type, RValue* out_val);
int32_t buffer_write(int32_t id, int32_t type, const RValue* val);
void    buffer_copy(int32_t src_id, size_t src_offset, size_t size, int32_t dest_id, size_t dest_offset);
bool    buffer_save(int32_t id, const char* filename);
int32_t buffer_load(const char* filename);

/* Async Buffer Operations */
int32_t buffer_save_async(int32_t id, const char* filename, size_t offset, size_t size);
int32_t buffer_load_async(int32_t id, const char* filename, size_t offset, size_t size);
void    buffer_async_group_begin(const char* group_name);
void    buffer_async_group_option(const char* option, const char* value);
int32_t buffer_async_group_end(void);

/* Checksums */
void    buffer_sha1(int32_t id, size_t offset, size_t size, char out_sha1[41]);
uint32_t buffer_crc32(int32_t id, size_t offset, size_t size);

/* Engine Compatibility Hook */
int32_t FUN_1418884f0(int32_t buffer_id, int32_t type, const RValue* val);

#ifdef __cplusplus
}
#endif

#endif /* RUNNER_FILES_H */
