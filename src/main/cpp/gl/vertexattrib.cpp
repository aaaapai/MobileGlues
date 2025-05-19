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


void glVertexAttrib2s(GLuint index, GLshort v0, GLshort v1) {
    glVertexAttrib2f(index, (GLfloat) v0, (GLfloat) v1);
}