//
// Created by Swung 0x48 on 2025-03-26.
//

#ifndef MOBILEGLUES_DSA_BUFFER_H
#define MOBILEGLUES_DSA_BUFFER_H

#include <vector>

#include "GL/gl.h"
#include "../buffer.h"
#include "../log.h"

#ifdef __cplusplus
extern "C" {
#endif

static GLenum get_binding_query(GLenum target);


GLAPI GLAPIENTRY void glNamedBufferData(GLuint buffer, GLsizeiptr size, const void *data, GLenum usage);

GLAPI GLAPIENTRY void glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);

GLAPI GLAPIENTRY void glCreateBuffers(GLsizei n, GLuint* buffers);

GLAPI GLAPIENTRY void glFlushMappedNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length);

GLAPI GLAPIENTRY void glGetNamedBufferParameteriv(GLuint buffer, GLenum pname, GLint* params);

GLAPI GLAPIENTRY void glGetNamedBufferParameteri64v(GLuint buffer, GLenum pname, GLint64* params);

GLAPI GLAPIENTRY void glGetNamedBufferPointerv(GLuint buffer, GLenum pname, void* *params);

GLAPI GLAPIENTRY void glGetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data);

GLAPI GLAPIENTRY void *glMapNamedBuffer (GLuint buffer, GLenum access);

GLAPI GLAPIENTRY void* glMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);

GLAPI GLAPIENTRY void glClearBufferData(GLenum target, GLenum internalformat, GLenum format, GLenum type, const void *data);

GLAPI GLAPIENTRY void glClearNamedBufferData(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void *data);

GLAPI GLAPIENTRY void glClearBufferSubData (GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);

GLAPI GLAPIENTRY void glClearNamedBufferSubData (GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);

GLAPI GLAPIENTRY void glNamedBufferStorage (GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags);

GLAPI GLAPIENTRY void glCopyNamedBufferSubData (GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);

#ifdef __cplusplus
}
#endif


#endif //MOBILEGLUES_DSA_BUFFER_H
