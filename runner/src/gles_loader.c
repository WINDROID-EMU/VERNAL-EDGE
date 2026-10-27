#include "gles_loader.h"

#ifndef __ANDROID__

#include <stdio.h>
#include <dlfcn.h>

void (*glViewport)(GLint x, GLint y, GLsizei width, GLsizei height) = NULL;
void (*glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = NULL;
void (*glClear)(GLbitfield mask) = NULL;
void (*glEnable)(GLenum cap) = NULL;
void (*glDisable)(GLenum cap) = NULL;
void (*glBlendFunc)(GLenum sfactor, GLenum dfactor) = NULL;
void (*glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) = NULL;
void (*glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) = NULL;
void (*glGenTextures)(GLsizei n, GLuint *textures) = NULL;
void (*glDeleteTextures)(GLsizei n, const GLuint *textures) = NULL;
void (*glBindTexture)(GLenum target, GLuint texture) = NULL;
void (*glTexParameteri)(GLenum target, GLenum pname, GLint param) = NULL;
void (*glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) = NULL;
void (*glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels) = NULL;
void (*glGenBuffers)(GLsizei n, GLuint *buffers) = NULL;
void (*glDeleteBuffers)(GLsizei n, const GLuint *buffers) = NULL;
void (*glBindBuffer)(GLenum target, GLuint buffer) = NULL;
void (*glBufferData)(GLenum target, GLsizeiptr size, const void *data, GLenum usage) = NULL;
void (*glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data) = NULL;
void (*glEnableVertexAttribArray)(GLuint index) = NULL;
void (*glDisableVertexAttribArray)(GLuint index) = NULL;
void (*glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) = NULL;
void (*glDrawArrays)(GLenum mode, GLint first, GLsizei count) = NULL;
GLuint (*glCreateShader)(GLenum type) = NULL;
void (*glDeleteShader)(GLuint shader) = NULL;
void (*glShaderSource)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length) = NULL;
void (*glCompileShader)(GLuint shader) = NULL;
void (*glGetShaderiv)(GLuint shader, GLenum pname, GLint *params) = NULL;
void (*glGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) = NULL;
GLuint (*glCreateProgram)(void) = NULL;
void (*glDeleteProgram)(GLuint program) = NULL;
void (*glAttachShader)(GLuint program, GLuint shader) = NULL;
void (*glLinkProgram)(GLuint program) = NULL;
void (*glGetProgramiv)(GLuint program, GLenum pname, GLint *params) = NULL;
void (*glGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) = NULL;
void (*glUseProgram)(GLuint program) = NULL;
GLint (*glGetUniformLocation)(GLuint program, const GLchar *name) = NULL;
GLint (*glGetAttribLocation)(GLuint program, const GLchar *name) = NULL;
void (*glUniform1f)(GLint location, GLfloat v0) = NULL;
void (*glUniform1i)(GLint location, GLint v0) = NULL;
void (*glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) = NULL;
void (*glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) = NULL;
void (*glGenFramebuffers)(GLsizei n, GLuint *framebuffers) = NULL;
void (*glDeleteFramebuffers)(GLsizei n, const GLuint *framebuffers) = NULL;
void (*glBindFramebuffer)(GLenum target, GLuint framebuffer) = NULL;
void (*glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) = NULL;
GLenum (*glCheckFramebufferStatus)(GLenum target) = NULL;

static void *s_gl_handle = NULL;

#define LOAD_PROC(fn) do { \
    fn = dlsym(s_gl_handle, #fn); \
    if (!fn) fn = dlsym(RTLD_DEFAULT, #fn); \
} while (0)

bool gles_loader_init(void) {
    if (s_gl_handle) return true;

    s_gl_handle = dlopen("libGLESv2.so.2", RTLD_LAZY | RTLD_GLOBAL);
    if (!s_gl_handle) s_gl_handle = dlopen("libGLESv2.so", RTLD_LAZY | RTLD_GLOBAL);
    if (!s_gl_handle) s_gl_handle = dlopen("/usr/lib/x86_64-linux-gnu/libGLESv2.so.2", RTLD_LAZY | RTLD_GLOBAL);
    if (!s_gl_handle) s_gl_handle = dlopen("libGL.so.1", RTLD_LAZY | RTLD_GLOBAL);
    if (!s_gl_handle) s_gl_handle = dlopen("libGL.so", RTLD_LAZY | RTLD_GLOBAL);

    if (!s_gl_handle) {
        fprintf(stderr, "[GLES Loader] Failed to load GLES / GL library\n");
        return false;
    }

    LOAD_PROC(glViewport);
    LOAD_PROC(glClearColor);
    LOAD_PROC(glClear);
    LOAD_PROC(glEnable);
    LOAD_PROC(glDisable);
    LOAD_PROC(glBlendFunc);
    LOAD_PROC(glBlendFuncSeparate);
    LOAD_PROC(glColorMask);
    LOAD_PROC(glGenTextures);
    LOAD_PROC(glDeleteTextures);
    LOAD_PROC(glBindTexture);
    LOAD_PROC(glTexParameteri);
    LOAD_PROC(glTexImage2D);
    LOAD_PROC(glTexSubImage2D);
    LOAD_PROC(glGenBuffers);
    LOAD_PROC(glDeleteBuffers);
    LOAD_PROC(glBindBuffer);
    LOAD_PROC(glBufferData);
    LOAD_PROC(glBufferSubData);
    LOAD_PROC(glEnableVertexAttribArray);
    LOAD_PROC(glDisableVertexAttribArray);
    LOAD_PROC(glVertexAttribPointer);
    LOAD_PROC(glDrawArrays);
    LOAD_PROC(glCreateShader);
    LOAD_PROC(glDeleteShader);
    LOAD_PROC(glShaderSource);
    LOAD_PROC(glCompileShader);
    LOAD_PROC(glGetShaderiv);
    LOAD_PROC(glGetShaderInfoLog);
    LOAD_PROC(glCreateProgram);
    LOAD_PROC(glDeleteProgram);
    LOAD_PROC(glAttachShader);
    LOAD_PROC(glLinkProgram);
    LOAD_PROC(glGetProgramiv);
    LOAD_PROC(glGetProgramInfoLog);
    LOAD_PROC(glUseProgram);
    LOAD_PROC(glGetUniformLocation);
    LOAD_PROC(glGetAttribLocation);
    LOAD_PROC(glUniform1f);
    LOAD_PROC(glUniform1i);
    LOAD_PROC(glUniform4f);
    LOAD_PROC(glUniformMatrix4fv);
    LOAD_PROC(glGenFramebuffers);
    LOAD_PROC(glDeleteFramebuffers);
    LOAD_PROC(glBindFramebuffer);
    LOAD_PROC(glFramebufferTexture2D);
    LOAD_PROC(glCheckFramebufferStatus);

    return true;
}

#endif /* !__ANDROID__ */
