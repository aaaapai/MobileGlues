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

GLuint gen_buffer(GLuint realid = 0);

GLboolean has_buffer(GLuint key);

void modify_buffer(GLuint key, GLuint value);

void remove_buffer(GLuint key);

GLuint find_real_buffer(GLuint key);

GLuint find_bound_buffer(GLenum key);

GLuint gen_array();

GLboolean has_array(GLuint key);

void modify_array(GLuint key, GLuint value);

void remove_array(GLuint key);

GLuint find_real_array(GLuint key);

GLuint find_bound_array();

static GLenum get_binding_query(GLenum target);

GLAPI APIENTRY void glGenBuffers(GLsizei n, GLuint *buffers);

GLAPI APIENTRY void glGenBuffersARB(GLsizei n, GLuint *buffers); 

GLAPI APIENTRY void glDeleteBuffers(GLsizei n, const GLuint *buffers);

GLAPI APIENTRY void glDeleteBuffersARB(GLsizei n, const GLuint *buffers);

GLAPI APIENTRY GLboolean glIsBuffer(GLuint buffer);

GLAPI APIENTRY GLboolean glIsBufferARB(GLuint buffer);

GLAPI APIENTRY void glBindBuffer(GLenum target, GLuint buffer);

GLAPI APIENTRY void glTexBuffer(GLenum target, GLenum internalformat, GLuint buffer);

GLAPI APIENTRY void glTexBufferRange(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);

GLAPI APIENTRY GLboolean glUnmapBuffer(GLenum target);

GLAPI APIENTRY void glTexBufferRange(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);

GLAPI APIENTRY GLboolean glUnmapBuffer(GLenum target);

GLAPI APIENTRY void glBindBufferBase(GLenum target, GLuint index, GLuint buffer);

GLAPI APIENTRY void glBindVertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);

GLAPI APIENTRY GLboolean glUnmapBuffer(GLenum target);

GLAPI APIENTRY void *glMapBuffer(GLenum target, GLenum access);

GLAPI APIENTRY void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage);

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
