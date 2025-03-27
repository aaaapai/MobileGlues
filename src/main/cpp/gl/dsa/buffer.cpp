//
// Created by Swung 0x48 on 2025-03-26.
//

#include "buffer.h"

#define DEBUG 0

void glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data) {
    LOG()
    LOG_D("glNamedBufferSubData, buffer = %d, offset = %d, size = %d, data = 0x%x",
          buffer, offset, size, data)

    INIT_CHECK_GL_ERROR

    GLint prevbuf = 0;
    glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING, &prevbuf);
    CHECK_GL_ERROR_NO_INIT
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffer);
    CHECK_GL_ERROR_NO_INIT
    glBufferSubData(GL_COPY_WRITE_BUFFER, offset, size, data);
    CHECK_GL_ERROR_NO_INIT
    glBindBuffer(GL_COPY_WRITE_BUFFER, prevbuf);
    CHECK_GL_ERROR_NO_INIT
}