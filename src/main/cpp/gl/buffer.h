//
// Created by BZLZHH on 2025/1/28.
//

#ifndef MOBILEGLUES_BUFFER_H

#include "../includes.h"
#include "glcorearb.h"
#include "log.h"
#include "../gles/loader.h"
#include "mg.h"

#include <vector>

typedef struct {
    GLenum target;
    GLuint buffer_id;
    void *mapped_ptr;
#if GLOBAL_DEBUG || DEBUG
    void *client_ptr;
#endif
    GLsizeiptr size;
    GLbitfield flags;
    GLboolean is_dirty;
} BufferMapping;

#ifdef __cplusplus
extern "C" {
#endif

GLuint gen_buffer();

GLboolean has_buffer(GLuint key);

void modify_buffer(GLuint key, GLuint value);

void remove_buffer(GLuint key);

GLuint find_real_buffer(GLuint key);

void bind_buffer(GLenum target, GLuint buffer);

GLuint find_buffer(GLenum target);

void real_bind_buffer(GLenum target, GLuint buffer);

GLuint gen_array();

GLboolean has_array(GLuint key);

void modify_array(GLuint key, GLuint value);

void remove_array(GLuint key);

GLuint find_real_array(GLuint key);

static GLenum get_binding_query(GLenum target);

GLAPI APIENTRY void glGenBuffers(GLsizei n, GLuint *buffers);

GLAPI APIENTRY void glDeleteBuffers(GLsizei n, const GLuint *buffers);

GLAPI APIENTRY GLboolean glIsBuffer(GLuint buffer);

GLAPI APIENTRY void glBindBuffer(GLenum target, GLuint buffer);

GLAPI APIENTRY GLboolean glUnmapBuffer(GLenum target);

GLAPI APIENTRY void *glMapBuffer(GLenum target, GLenum access);

GLAPI APIENTRY void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage);

GLAPI APIENTRY void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);

GLAPI APIENTRY void glBufferStorage(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags);

GLAPI APIENTRY void glGenVertexArrays(GLsizei n, GLuint *arrays);

GLAPI APIENTRY void glDeleteVertexArrays(GLsizei n, const GLuint *arrays);

GLAPI APIENTRY GLboolean glIsVertexArray(GLuint array);

GLAPI APIENTRY void glBindVertexArray(GLuint array);

#ifdef __cplusplus
}
#endif

#define MOBILEGLUES_BUFFER_H

#endif //MOBILEGLUES_BUFFER_H
