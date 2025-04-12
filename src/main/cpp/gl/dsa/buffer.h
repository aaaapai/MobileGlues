//
// Created by Swung 0x48 on 2025-03-26.
//

#ifndef MOBILEGLUES_DSA_BUFFER_H
#define MOBILEGLUES_DSA_BUFFER_H

#include <vector>

#include "../gl.h"
#include "../buffer.h"
#include "../log.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI GLAPIENTRY void glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);

GLAPI GLAPIENTRY void glCreateBuffers(GLsizei n, GLuint* buffers);

GLAPI GLAPIENTRY void glFlushMappedNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length);

GLAPI GLAPIENTRY void glGetNamedBufferParameteriv(GLuint buffer, GLenum pname, GLint* params);

GLAPI GLAPIENTRY void glGetNamedBufferParameteri64v(GLuint buffer, GLenum pname, GLint64* params);

GLAPI GLAPIENTRY void glGetNamedBufferPointerv(GLuint buffer, GLenum pname, void* *params);

GLAPI GLAPIENTRY void glGetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data);

GLAPI GLAPIENTRY void* glMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);

#ifdef __cplusplus
}
#endif


#endif //MOBILEGLUES_DSA_BUFFER_H
