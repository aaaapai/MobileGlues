//
// Created by Swung 0x48 on 2025-03-26.
//

#include "buffer.h"

#define DEBUG 0

#define SAVE_BUFFER_CTX(target) \
    GLint prevbuf = 0; \
    glGetIntegerv(target##_BINDING, &prevbuf); \
    CHECK_GL_ERROR_NO_INIT \
    glBindBuffer(target, buffer); \
    CHECK_GL_ERROR_NO_INIT

#define RESTORE_BUFFER_CTX(target) \
    glBindBuffer(GL_COPY_WRITE_BUFFER, prevbuf); \
    CHECK_GL_ERROR_NO_INIT

void glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data) {
    LOG()
    LOG_D("glNamedBufferSubData, buffer = %d, offset = %d, size = %d, data = 0x%x",
          buffer, offset, size, data)

    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glBufferSubData(GL_COPY_WRITE_BUFFER, offset, size, data);

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glCreateBuffers(GLsizei n, GLuint* buffers) {
    LOG()
    LOG_D("glCreateBuffers, n = %d", n)

    INIT_CHECK_GL_ERROR

    // do actual gen to ES driver
    GLES.glGenBuffers(n, buffers);
    CHECK_GL_ERROR_NO_INIT
    // save to renaming table
    for (int i = 0; i < n; ++i) {
        buffers[i] = gen_buffer(buffers[i]);
    }
}

void glFlushMappedNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length) {
    LOG()
    LOG_D("glFlushMappedNamedBufferRange, buffer = %d, offset = %d, length = %d", buffer, offset, length)
    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glFlushMappedBufferRange(GL_COPY_WRITE_BUFFER, offset, length);
    CHECK_GL_ERROR_NO_INIT

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glGetNamedBufferParameteriv(GLuint buffer, GLenum pname, GLint* params) {
    LOG()
    LOG_D("glGetNamedBufferParameteriv, buffer = %d, pname = %s", buffer, glEnumToString(pname))
    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glGetBufferParameteriv(GL_COPY_WRITE_BUFFER, pname, params);
    CHECK_GL_ERROR_NO_INIT

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glGetNamedBufferParameteri64v(GLuint buffer, GLenum pname, GLint64* params) {
    LOG()
    LOG_D("glGetNamedBufferParameteri64v, buffer = %d, pname = %s", buffer, glEnumToString(pname))
    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glGetBufferParameteri64v(GL_COPY_WRITE_BUFFER, pname, params);
    CHECK_GL_ERROR_NO_INIT

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glGetNamedBufferPointerv(GLuint buffer, GLenum pname, void* *params) {
    LOG()
    LOG_D("glGetNamedBufferPointerv, buffer = %d, pname = %s", buffer, glEnumToString(pname))
    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glGetBufferPointerv(GL_COPY_WRITE_BUFFER, pname, params);
    CHECK_GL_ERROR_NO_INIT

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glGetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data) {
    LOG()
    LOG_D("glGetNamedBufferSubData, buffer = %d, offset = %d, size = %d, data = 0x%x",
          buffer, offset, size, data)
    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_READ_BUFFER)

    // TODO...

    RESTORE_BUFFER_CTX(GL_COPY_READ_BUFFER)
}

void* glMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access) {
    LOG()
    LOG_D("glMapNamedBufferRange, buffer = %d, offset = %d, length = %d, access = 0x%x",
          buffer, offset, length, access)
    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_READ_BUFFER)

    void* ptr = GLES.glMapBufferRange(GL_COPY_READ_BUFFER, offset, length, access);

    RESTORE_BUFFER_CTX(GL_COPY_READ_BUFFER)

    return ptr;
}
