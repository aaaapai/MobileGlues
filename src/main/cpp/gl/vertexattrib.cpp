//
// Created by Swung 0x48 on 2025/2/19.
//

#include "vertexattrib.h"

#define DEBUG 0

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

/* glVertexAttribXs family */
void glVertexAttrib1s(GLuint index, GLshort v0) {
    LOG()
    GLES.glVertexAttrib1f(index, (GLfloat) v0);
}

void glVertexAttrib2s(GLuint index, GLshort v0, GLshort v1) {
    LOG()
    GLES.glVertexAttrib2f(index, (GLfloat) v0, (GLfloat) v1);
}
void glVertexAttrib3s( 	GLuint index,
                          GLshort v0,
                          GLshort v1,
                          GLshort v2) {
    LOG()
    GLES.glVertexAttrib3f(index, (GLfloat) v0, (GLfloat) v1, (GLfloat) v2);
}
void glVertexAttrib4s( 	GLuint index,
                          GLshort v0,
                          GLshort v1,
                          GLshort v2,
                          GLshort v3) {
    LOG()
    GLES.glVertexAttrib4f(index, (GLfloat) v0, (GLfloat) v1, (GLfloat) v2, (GLfloat) v3);
}

/* glVertexAttribXsv family */
void glVertexAttrib1sv( 	GLuint index, const GLshort *v) {
    LOG()
    glVertexAttrib1s(index, v[0]);
}
void glVertexAttrib2sv( 	GLuint index,
                           const GLshort *v) {
    LOG()
    glVertexAttrib2s(index, v[0], v[1]);
}
void glVertexAttrib3sv( 	GLuint index,
                           const GLshort *v) {
    LOG()
    glVertexAttrib3s(index, v[0], v[1], v[2]);
}
void glVertexAttrib4sv( 	GLuint index,
                           const GLshort *v) {
    LOG()
    glVertexAttrib4s(index, v[0], v[1], v[2], v[3]);
}

/* glVertexAttribXd family */
void glVertexAttrib1d( 	GLuint index,
                          GLdouble v0) {
    LOG()
    GLES.glVertexAttrib1f(index, (GLfloat) v0);
}

void glVertexAttrib2d( 	GLuint index,
                          GLdouble v0,
                          GLdouble v1) {
    LOG()
    GLES.glVertexAttrib2f(index, (GLfloat) v0, (GLfloat) v1);
}
void glVertexAttrib3d( 	GLuint index,
                          GLdouble v0,
                          GLdouble v1,
                          GLdouble v2) {
    LOG()
    GLES.glVertexAttrib3f(index, (GLfloat) v0, (GLfloat) v1, (GLfloat) v2);
}
void glVertexAttrib4d( 	GLuint index,
                          GLdouble v0,
                          GLdouble v1,
                          GLdouble v2,
                          GLdouble v3) {
    LOG()
    GLES.glVertexAttrib4f(index, (GLfloat) v0, (GLfloat) v1, (GLfloat) v2, (GLfloat) v3);
}

/* glVertexAttribXdv family */
void glVertexAttrib1dv( 	GLuint index,
                           const GLdouble *v) {
    LOG()
    glVertexAttrib1d(index, v[0]);
}
void glVertexAttrib2dv( 	GLuint index,
                           const GLdouble *v) {
    LOG()
    glVertexAttrib2d(index, v[0], v[1]);
}
void glVertexAttrib3dv( 	GLuint index,
                           const GLdouble *v) {
    LOG()
    glVertexAttrib3d(index, v[0], v[1], v[2]);
}
void glVertexAttrib4dv( 	GLuint index,
                           const GLdouble *v) {
    LOG()
    glVertexAttrib4d(index, v[0], v[1], v[2], v[3]);
}

/* glVertexAttribIXi family */
void glVertexAttribI1i( 	GLuint index,
                           GLint v0) {
    LOG()
    GLES.glVertexAttribI4i(index, v0, 0, 0, 1);
}
void glVertexAttribI2i( 	GLuint index,
                           GLint v0,
                           GLint v1) {
    LOG()
    GLES.glVertexAttribI4i(index, v0, v1, 0, 1);
}
void glVertexAttribI3i( 	GLuint index,
                           GLint v0,
                           GLint v1,
                           GLint v2) {
    LOG()
    GLES.glVertexAttribI4i(index, v0, v1, v2, 1);
}
// glVertexAttribI4i is natively implemented in OpenGL ES

/* glVertexAttribIXiv family */
void glVertexAttribI1iv( 	GLuint index,
                            const GLint *v) {
    LOG()
    glVertexAttribI1i(index, v[0]);
}
void glVertexAttribI2iv( 	GLuint index,
                            const GLint *v) {
    LOG()
    glVertexAttribI2i(index, v[0], v[1]);
}
void glVertexAttribI3iv( 	GLuint index,
                            const GLint *v) {
    LOG()
    glVertexAttribI3i(index, v[0], v[1], v[2]);
}
// glVertexAttribI4iv is natively implemented in OpenGL ES

/* glVertexAttribI4XYv family */
void glVertexAttribI4bv( 	GLuint index,
                            const GLbyte *v) {
    LOG()
    GLES.glVertexAttribI4i(index, (GLint)v[0], (GLint)v[1], (GLint)v[2], (GLint)v[3]);
}
void glVertexAttribI4ubv( 	GLuint index,
                            const GLubyte *v) {
    LOG()
    GLES.glVertexAttribI4ui(index, (GLuint)v[0], (GLuint)v[1], (GLuint)v[2], (GLuint)v[3]);
}
void glVertexAttribI4sv( 	GLuint index,
                            const GLshort *v) {
    LOG()
    GLES.glVertexAttribI4i(index, (GLint)v[0], (GLint)v[1], (GLint)v[2], (GLint)v[3]);
}
void glVertexAttribI4usv( 	GLuint index,
                             const GLushort *v) {
    LOG()
    GLES.glVertexAttribI4ui(index, (GLuint)v[0], (GLuint)v[1], (GLuint)v[2], (GLuint)v[3]);
}

/* glVertexAttribIXuiv family */
void glVertexAttribI1uiv( 	GLuint index,
                            const GLuint *v) {
    LOG()
    glVertexAttribI1ui(index, v[0]);
}
void glVertexAttribI2uiv( 	GLuint index,
                            const GLuint *v) {
    LOG()
    glVertexAttribI2ui(index, v[0], v[1]);
}
void glVertexAttribI3uiv( 	GLuint index,
                            const GLuint *v) {
    LOG()
    glVertexAttribI3ui(index, v[0], v[1], v[2]);
}
// glVertexAttribI4uiv is natively implemented in OpenGL ES

/* glVertexAttrib4N*** family */
void glVertexAttrib4Nub( 	GLuint index,
                            GLubyte v0,
                            GLubyte v1,
                            GLubyte v2,
                            GLubyte v3) {
    LOG()

    GLfloat fv0 = ((GLfloat)v0 / 255.0f);
    GLfloat fv1 = ((GLfloat)v1 / 255.0f);
    GLfloat fv2 = ((GLfloat)v2 / 255.0f);
    GLfloat fv3 = ((GLfloat)v3 / 255.0f);
    GLES.glVertexAttrib4f(index, fv0, fv1, fv2, fv3);
}

void glVertexAttrib4Nubv( 	GLuint index,
                             const GLubyte *v) {
    LOG()
    glVertexAttrib4Nub(index, v[0], v[1], v[2], v[3]);
}

void glVertexAttrib4Nusv( 	GLuint index,
                             const GLushort *v) {
    LOG()
    GLfloat fv0 = ((GLfloat)v[0] / 32767.0f);
    GLfloat fv1 = ((GLfloat)v[1] / 32767.0f);
    GLfloat fv2 = ((GLfloat)v[2] / 32767.0f);
    GLfloat fv3 = ((GLfloat)v[3] / 32767.0f);
    GLES.glVertexAttrib4f(index, fv0, fv1, fv2, fv3);
}

void glVertexAttrib4Nuiv( 	GLuint index,
                             const GLuint *v) {
    LOG()

    // GLfloat has a precision limit of 24 bits, so truncate the input integers to it.
    GLfloat fv0 = ((GLfloat)v[0] / 4294967295.0f);
    GLfloat fv1 = ((GLfloat)v[1] / 4294967295.0f);
    GLfloat fv2 = ((GLfloat)v[2] / 4294967295.0f);
    GLfloat fv3 = ((GLfloat)v[3] / 4294967295.0f);
    GLES.glVertexAttrib4f(index, fv0, fv1, fv2, fv3);
}

static float bnormalize(GLbyte x) {
    return x < 0 ? (GLfloat)x/128.0f : (GLfloat)x/127.0f;
}
static float snormalize(GLshort x) {
    return x < 0 ? (GLfloat) x / 32768.0f : (GLfloat) x / 32767.0f;
}
static float inormalize(GLint x) {
    return x < 0 ? (GLfloat)x/2147483648.0f : (GLfloat)x/2147483647.0f;
}

void glVertexAttrib4Nbv( 	GLuint index,
                            const GLbyte *v) {
    LOG()
    GLES.glVertexAttrib4f(
            index,
            bnormalize(v[0]),
            bnormalize(v[1]),
            bnormalize(v[2]),
            bnormalize(v[3])
    );
}

void glVertexAttrib4Nsv( 	GLuint index,
                            const GLshort *v) {
    LOG()
    GLES.glVertexAttrib4f(
            index,
            snormalize(v[0]),
            snormalize(v[1]),
            snormalize(v[2]),
            snormalize(v[3])
    );
}

void glVertexAttrib4Niv( 	GLuint index,
                            const GLint *v) {
    LOG()
    GLES.glVertexAttrib4f(
            index,
            inormalize(v[0]),
            inormalize(v[1]),
            inormalize(v[2]),
            inormalize(v[3])
    );
}

void glVertexAttrib4iv(GLuint index, const GLint *v) {
    LOG()
    GLES.glVertexAttrib4f(
            index,
            inormalize(v[0]),
            inormalize(v[1]),
            inormalize(v[2]),
            inormalize(v[3])
    );
}

void glVertexAttrib4bv(GLuint index, const GLbyte *v) {
    LOG()
    GLES.glVertexAttrib4f(
            index,
            bnormalize(v[0]),
            bnormalize(v[1]),
            bnormalize(v[2]),
            bnormalize(v[3])
    );
}

void glVertexAttrib4ubv(GLuint index, const GLubyte *v) {
      LOG()
      glVertexAttrib4Nub(index, v[0], v[1], v[2], v[3]);
}

void glVertexAttrib4usv (GLuint index, const GLushort *v) {
    LOG()
    GLfloat fv0 = ((GLfloat)v[0] / 32767.0f);
    GLfloat fv1 = ((GLfloat)v[1] / 32767.0f);
    GLfloat fv2 = ((GLfloat)v[2] / 32767.0f);
    GLfloat fv3 = ((GLfloat)v[3] / 32767.0f);
    GLES.glVertexAttrib4f(index, fv0, fv1, fv2, fv3);
}

void glVertexAttrib4uiv (GLuint index, const GLuint *v) {
    LOG()
    // GLfloat has a precision limit of 24 bits, so truncate the input integers to it.
    GLfloat fv0 = ((GLfloat)v[0] / 4294967295.0f);
    GLfloat fv1 = ((GLfloat)v[1] / 4294967295.0f);
    GLfloat fv2 = ((GLfloat)v[2] / 4294967295.0f);
    GLfloat fv3 = ((GLfloat)v[3] / 4294967295.0f);
    GLES.glVertexAttrib4f(index, fv0, fv1, fv2, fv3);
}

// 模拟顶点属性格式函数的核心实现
static void vertex_attrib_format_helper(GLuint vaobj, GLuint attribindex, 
                                      GLint size, GLenum type, 
                                      GLboolean normalized, 
                                      GLboolean integer,
                                      GLuint relativeoffset) {
    // 参数验证
    GLint max_attribs;
    GLES.glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
    if (attribindex >= (GLuint)max_attribs) {
        glGetError(); // 触发GL_INVALID_VALUE
        return;
    }

    // 检查size有效性
    if (size != 1 && size != 2 && size != 3 && size != 4) {
        glGetError(); // 触发GL_INVALID_VALUE
        return;
    }

    // 检查type有效性
    switch(type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_FLOAT:
        case GL_HALF_FLOAT:
        case GL_FIXED:
            break;
        default:
            glGetError(); // 触发GL_INVALID_ENUM
            return;
    }

    // 保存当前VAO绑定状态
    GLint prev_vao;
    GLES.glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prev_vao);
    
    // 绑定目标VAO
    if (vaobj != 0) {
        GLES.glBindVertexArray(vaobj);
    }

    // 设置顶点属性指针
    if (integer) {
        GLES.glVertexAttribIPointer(attribindex, size, type, 0, 
                             (const void*)(intptr_t)relativeoffset);
    } else {
        GLES.glVertexAttribPointer(attribindex, size, type, normalized, 0,
                            (const void*)(intptr_t)relativeoffset);
    }

    // 恢复之前的VAO绑定
    if (vaobj != 0) {
        GLES.glBindVertexArray(prev_vao);
    }
}

// 标准浮点顶点属性格式
void glVertexAttribFormat(GLuint attribindex, GLint size, GLenum type,
                         GLboolean normalized, GLuint relativeoffset) {
    vertex_attrib_format_helper(0, attribindex, size, type, 
                               normalized, GL_FALSE, relativeoffset);
}

// 整数顶点属性格式
void glVertexAttribIFormat(GLuint attribindex, GLint size, GLenum type,
                          GLuint relativeoffset) {
    vertex_attrib_format_helper(0, attribindex, size, type, 
                              GL_FALSE, GL_TRUE, relativeoffset);
}

// VAO版本的浮点格式
void glVertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size,
                              GLenum type, GLboolean normalized,
                              GLuint relativeoffset) {
    vertex_attrib_format_helper(vaobj, attribindex, size, type,
                              normalized, GL_FALSE, relativeoffset);
}

// VAO版本的整数格式
void glVertexArrayAttribIFormat(GLuint vaobj, GLuint attribindex, GLint size,
                               GLenum type, GLuint relativeoffset) {
    vertex_attrib_format_helper(vaobj, attribindex, size, type,
                              GL_FALSE, GL_TRUE, relativeoffset);
}

// 64位双精度模拟(使用float模拟)
void glVertexAttribLFormat(GLuint attribindex, GLint size, GLenum type,
                          GLuint relativeoffset) {
    if (type != GL_DOUBLE) {
        glGetError(); // 触发GL_INVALID_ENUM
        return;
    }
    vertex_attrib_format_helper(0, attribindex, size, GL_FLOAT,
                              GL_FALSE, GL_FALSE, relativeoffset);
}

// VAO版本的64位双精度模拟
void glVertexArrayAttribLFormat(GLuint vaobj, GLuint attribindex, GLint size,
                               GLenum type, GLuint relativeoffset) {
    if (type != GL_DOUBLE) {
        glGetError(); // 触发GL_INVALID_ENUM
        return;
    }
    vertex_attrib_format_helper(vaobj, attribindex, size, GL_FLOAT,
                              GL_FALSE, GL_FALSE, relativeoffset);
}
