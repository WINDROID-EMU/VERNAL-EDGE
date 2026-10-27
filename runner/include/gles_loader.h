#ifndef GLES_LOADER_H
#define GLES_LOADER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __ANDROID__
  #include <GLES3/gl3.h>
  #include <EGL/egl.h>
#else

/* Desktop Linux / Host minimal GLES declarations */
typedef unsigned int   GLenum;
typedef unsigned char  GLboolean;
typedef unsigned int   GLbitfield;
typedef void           GLvoid;
typedef signed char    GLbyte;
typedef short          GLshort;
typedef int            GLint;
typedef int            GLsizei;
typedef unsigned char  GLubyte;
typedef unsigned short GLushort;
typedef unsigned int   GLuint;
typedef float          GLfloat;
typedef float          GLclampf;
typedef int32_t        GLintptr;
typedef int32_t        GLsizeiptr;
typedef char           GLchar;

#define GL_FALSE                          0
#define GL_TRUE                           1
#define GL_TRIANGLES                      0x0004
#define GL_LINES                          0x0001
#define GL_LINE_STRIP                     0x0003
#define GL_LINE_LOOP                      0x0002
#define GL_DEPTH_TEST                     0x0B71
#define GL_BLEND                          0x0BE2
#define GL_TEXTURE_2D                     0x0DE1
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_FLOAT                          0x1406
#define GL_RGBA                           0x1908
#define GL_RGB                            0x1907
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_REPEAT                         0x2901
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_ONE                            1
#define GL_ZERO                           0
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307
#define GL_ARRAY_BUFFER                   0x8892
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_STATIC_DRAW                    0x88E4
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_FRAMEBUFFER                    0x8D40
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5

/* Dynamic GL function pointers */
extern void (*glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
extern void (*glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern void (*glClear)(GLbitfield mask);
extern void (*glEnable)(GLenum cap);
extern void (*glDisable)(GLenum cap);
extern void (*glBlendFunc)(GLenum sfactor, GLenum dfactor);
extern void (*glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
extern void (*glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
extern void (*glGenTextures)(GLsizei n, GLuint *textures);
extern void (*glDeleteTextures)(GLsizei n, const GLuint *textures);
extern void (*glBindTexture)(GLenum target, GLuint texture);
extern void (*glTexParameteri)(GLenum target, GLenum pname, GLint param);
extern void (*glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
extern void (*glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
extern void (*glGenBuffers)(GLsizei n, GLuint *buffers);
extern void (*glDeleteBuffers)(GLsizei n, const GLuint *buffers);
extern void (*glBindBuffer)(GLenum target, GLuint buffer);
extern void (*glBufferData)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
extern void (*glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
extern void (*glEnableVertexAttribArray)(GLuint index);
extern void (*glDisableVertexAttribArray)(GLuint index);
extern void (*glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
extern void (*glDrawArrays)(GLenum mode, GLint first, GLsizei count);
extern GLuint (*glCreateShader)(GLenum type);
extern void (*glDeleteShader)(GLuint shader);
extern void (*glShaderSource)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
extern void (*glCompileShader)(GLuint shader);
extern void (*glGetShaderiv)(GLuint shader, GLenum pname, GLint *params);
extern void (*glGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
extern GLuint (*glCreateProgram)(void);
extern void (*glDeleteProgram)(GLuint program);
extern void (*glAttachShader)(GLuint program, GLuint shader);
extern void (*glLinkProgram)(GLuint program);
extern void (*glGetProgramiv)(GLuint program, GLenum pname, GLint *params);
extern void (*glGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
extern void (*glUseProgram)(GLuint program);
extern GLint (*glGetUniformLocation)(GLuint program, const GLchar *name);
extern GLint (*glGetAttribLocation)(GLuint program, const GLchar *name);
extern void (*glUniform1f)(GLint location, GLfloat v0);
extern void (*glUniform1i)(GLint location, GLint v0);
extern void (*glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
extern void (*glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
extern void (*glGenFramebuffers)(GLsizei n, GLuint *framebuffers);
extern void (*glDeleteFramebuffers)(GLsizei n, const GLuint *framebuffers);
extern void (*glBindFramebuffer)(GLenum target, GLuint framebuffer);
extern void (*glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
extern GLenum (*glCheckFramebufferStatus)(GLenum target);

bool gles_loader_init(void);

#endif /* !__ANDROID__ */

#endif /* GLES_LOADER_H */
