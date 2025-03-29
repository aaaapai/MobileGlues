//
// Created by Swung 0x48 on 2025-03-26.
//

#ifndef MOBILEGLUES_DSA_BUFFER_H
#define MOBILEGLUES_DSA_BUFFER_H

#include <vector>

#include "../glcorearb.h"
#include "../buffer.h"
#include "../log.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI APIENTRY void glNamedBufferSubData (GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data);

GLAPI APIENTRY void glCreateBuffers(GLsizei n, GLuint* buffers);

GLAPI APIENTRY void glFlushMappedNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length);

GLAPI APIENTRY void glGetNamedBufferParameteriv(GLuint buffer, GLenum pname, GLint* params);

GLAPI APIENTRY void glGetNamedBufferParameteri64v(GLuint buffer, GLenum pname, GLint64* params);

GLAPI APIENTRY void glGetNamedBufferPointerv(GLuint buffer, GLenum pname, void* *params);

GLAPI APIENTRY void glGetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data);

GLAPI APIENTRY void* glMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);

#ifdef __cplusplus
}
#endif


#endif //MOBILEGLUES_DSA_BUFFER_H
