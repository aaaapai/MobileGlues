//
// Created by Swung 0x48 on 2025/2/19.
//

#include "vertexattrib.h"
#include "buffer.h"

#define DEBUG 0

void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
    LOG()
    LOG_D("glVertexAttribPointer(%d, %d, %s, %d, %d, %p)", index, size, glEnumToString(type), normalized, stride, pointer)
    if (find_buffer(GL_ARRAY_BUFFER)) {
        GLuint real_buff = find_real_buffer(find_buffer(GL_ARRAY_BUFFER));
        if (!real_buff) {
            GLES.glGenBuffers(1, &real_buff);
            modify_buffer(find_buffer(GL_ARRAY_BUFFER), real_buff);
            CHECK_GL_ERROR
        }
        if (real_buff) {
            real_bind_buffer(GL_ARRAY_BUFFER, real_buff);
            GLES.glVertexAttribPointer(index, size, type, normalized, stride, pointer);
            CHECK_GL_ERROR
        } else {
            LOG_E("real buffer is null!")
        }
    } else {
        LOG_E("No binded GL_ARRAY_BUFFER!")
    }
    CHECK_GL_ERROR
}

void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer) {
    LOG()
    LOG_D("glVertexAttribIPointer(%d, %d, %s, %d, %p)", index, size, glEnumToString(type), stride, pointer)
    if (find_buffer(GL_ARRAY_BUFFER)) {
        GLuint real_buff = find_real_buffer(find_buffer(GL_ARRAY_BUFFER));
        if (!real_buff) {
            GLES.glGenBuffers(1, &real_buff);
            modify_buffer(find_buffer(GL_ARRAY_BUFFER), real_buff);
            CHECK_GL_ERROR
        }
        if (real_buff) {
            real_bind_buffer(GL_ARRAY_BUFFER, real_buff);
            GLES.glVertexAttribIPointer(index, size, type, stride, pointer);
            CHECK_GL_ERROR
        } else {
            LOG_E("real buffer is null!")
        }
    } else {
        LOG_E("No binded GL_ARRAY_BUFFER!")
    }
    CHECK_GL_ERROR
}

void glVertexAttribI1ui(GLuint index, GLuint x) {
    LOG()
    GLES.glVertexAttribI4ui(index, x, 0, 0, 0);
}

void glVertexAttribI2ui(GLuint index, GLuint x, GLuint y) {
    LOG()
    GLES.glVertexAttribI4ui(index, x, y, 0, 0);
}

void glVertexAttribI3ui(GLuint index, GLuint x, GLuint y, GLuint z) {
    LOG()
    GLES.glVertexAttribI4ui(index, x, y, z, 0);
}