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

static GLenum get_binding_query(GLenum target);

GLAPI APIENTRY GLboolean glUnmapBuffer(GLenum target);

GLAPI APIENTRY void *glMapBuffer(GLenum target, GLenum access);

GLAPI APIENTRY void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage);

GLAPI APIENTRY void glBufferStorage(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags);

GLAPI APIENTRY void glBindBuffer(GLenum target, GLuint buffer);

#ifdef __cplusplus
}
#endif

#define MOBILEGLUES_BUFFER_H

#endif //MOBILEGLUES_BUFFER_H
