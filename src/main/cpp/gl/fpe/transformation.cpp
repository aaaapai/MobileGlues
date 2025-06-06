//
// Created by Swung 0x48 on 2025/2/8.
//

#include "transformation.h"
#include <glm/glm/gtc/type_ptr.hpp>
#include "list.h"
#include "pointer_utils.h"

#include <glm/glm/ext/matrix_clip_space.hpp>
#include <glm/glm/ext/matrix_relational.hpp>
#include <glm/glm/ext/matrix_transform.hpp>
#include <glm/glm/ext/matrix_float4x4.hpp>
#include <glm/glm/ext/vector_relational.hpp>
#include <glm/glm/ext/vector_float4.hpp>
#include <glm/glm/ext/vector_float3.hpp>
#define DEBUG 0

int matrix_idx(GLenum matrix_mode) {
    switch (matrix_mode) {
        case GL_MODELVIEW:
            return 0;
        case GL_PROJECTION:
            return 1;
        case GL_TEXTURE:
            return 2;
        case GL_COLOR:
            return 3;
    }
    LOG_E("Error: 1282");
    return 0;
}

void print_matrix(const glm::mat4& mat)
{
#if DEBUG || GLOBAL_DEBUG
    auto *pmat = (const float*)glm::value_ptr(mat);
    LOG_D_N("[")
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            int idx = j + i * 4;
            LOG_D_N("%.2f ", pmat[idx])
        }
        LOG_D("")
    }
    LOG_D("]")
#endif
}

void glMatrixMode( GLenum mode ) {
    LOG()
    LOG_D("glMatrixMode(%s)", glEnumToString(mode))

    LIST_RECORD(glMatrixMode, {}, mode)

    auto& transformation = g_glstate.fpe_uniform.transformation;

    switch (mode) {
        case GL_MODELVIEW:
        case GL_PROJECTION:
        case GL_TEXTURE:
        case GL_COLOR:
            transformation.matrix_mode = mode;
        default:
            break;
    }
}

void glLoadIdentity() {
    LOG()
    LOG_D("glLoadIdentity")

    LIST_RECORD(glLoadIdentity, {})

    auto& transformation = g_glstate.fpe_uniform.transformation;

    transformation.matrices[matrix_idx(transformation.matrix_mode)] = glm::mat4(1.0);

    LOG_D("Matrix %s:", glEnumToString(transformation.matrix_mode))
    print_matrix(transformation.matrices[matrix_idx(transformation.matrix_mode)]);
}

void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val) {
    LOG()
    LOG_D("glOrtho(%f, %f, %f, %f, %f, %f)", left, right, bottom, top, near_val, far_val)

    LIST_RECORD(glOrtho, {}, left, right, bottom, top, near_val, far_val)

    // TODO: precision loss?
    SELF_CALL(glOrthof, left, right, bottom, top, near_val, far_val)
}

void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) {
    LOG()
    LOG_D("glOrthof(%f, %f, %f, %f, %f, %f)", left, right, bottom, top, zNear, zFar)

    LIST_RECORD(glOrthof, {}, left, right, bottom, top, zNear, zFar)

    auto& transformation = g_glstate.fpe_uniform.transformation;

    transformation.matrices[matrix_idx(transformation.matrix_mode)] *= glm::ortho(left, right, bottom, top, zNear, zFar);
    LOG_D("Matrix %s:", glEnumToString(transformation.matrix_mode))
    print_matrix(transformation.matrices[matrix_idx(transformation.matrix_mode)]);
}

void glScalef( GLfloat x, GLfloat y, GLfloat z ) {
    LOG()
    LOG_D("glScalef(%f, %f, %f)", x, y, z)

    LIST_RECORD(glScalef, {}, x, y, z)

    auto& transformation = g_glstate.fpe_uniform.transformation;

    transformation.matrices[matrix_idx(transformation.matrix_mode)] =
            glm::scale(transformation.matrices[matrix_idx(transformation.matrix_mode)], glm::vec3(x, y, z));
    LOG_D("Matrix %s:", glEnumToString(transformation.matrix_mode))
    print_matrix(transformation.matrices[matrix_idx(transformation.matrix_mode)]);
}

void glTranslatef( GLfloat x, GLfloat y, GLfloat z ) {
    LOG()
    LOG_D("glTranslatef(%f, %f, %f)", x, y, z)

    LIST_RECORD(glTranslatef, {}, x, y, z)

    auto& transformation = g_glstate.fpe_uniform.transformation;

    transformation.matrices[matrix_idx(transformation.matrix_mode)] =
            glm::translate(transformation.matrices[matrix_idx(transformation.matrix_mode)], glm::vec3(x, y, z));
    LOG_D("Matrix %s:", glEnumToString(transformation.matrix_mode))
    print_matrix(transformation.matrices[matrix_idx(transformation.matrix_mode)]);
}

void glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z ) {
    LOG()
    LOG_D("glRotatef, angle = %.2f, x = %.2f, y = %.2f, z = %.2f", angle, x, y, z)

    LIST_RECORD(glRotatef, {}, angle, x, y, z)

    auto& transformation = g_glstate.fpe_uniform.transformation;

    transformation.matrices[matrix_idx(transformation.matrix_mode)] =
            glm::rotate(transformation.matrices[matrix_idx(transformation.matrix_mode)], (GLfloat)(angle * M_PI / 180.f), glm::vec3(x, y, z));
    LOG_D("Matrix %s:", glEnumToString(transformation.matrix_mode))
    print_matrix(transformation.matrices[matrix_idx(transformation.matrix_mode)]);
}

void glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z ) {
    LOG()
    LOG_D("glRotated(%f, %f, %f, %f)", angle, x, y, z)

    LIST_RECORD(glRotated, {}, angle, x, y, z)

    // TODO: precision loss?
    SELF_CALL(glRotatef, angle, x, y, z)
}

void glScaled(GLdouble x, GLdouble y, GLdouble z ) {
    LOG()
    LOG_D("glScaled(%f, %f, %f)", x, y, z)

    LIST_RECORD(glScaled, {}, x, y, z)

    // TODO: precision loss?
    SELF_CALL(glScalef, x, y, z)
}

void glTranslated(GLdouble x, GLdouble y, GLdouble z ) {
    LOG()
    LOG_D("glTranslated(%f, %f, %f)", x, y, z)

    LIST_RECORD(glTranslated, {}, x, y, z)

    // TODO: precision loss?
    SELF_CALL(glTranslatef, x, y, z);
}

void glMultMatrixf(const GLfloat *m) {
    LOG()
    LOG_D("glMultMatrixf(%p)", m)

    LIST_RECORD(glMultMatrixf, {{0, sizeof(GLfloat) * 16}}, m)

    auto& transformation = g_glstate.fpe_uniform.transformation;

    LOG_D("Matrix %s:", glEnumToString(transformation.matrix_mode))
    print_matrix(transformation.matrices[matrix_idx(transformation.matrix_mode)]);
    LOG_D("*")
    auto mat = glm::make_mat4(m);
    print_matrix(mat);

    transformation.matrices[matrix_idx(transformation.matrix_mode)] *= mat;
    LOG_D("=")

    print_matrix(transformation.matrices[matrix_idx(transformation.matrix_mode)]);
}

void glPushMatrix( void ) {
    LOG()
    LOG_D("glPushMatrix()")

    LIST_RECORD(glPushMatrix, {})

    auto& transformation = g_glstate.fpe_uniform.transformation;

    auto idx = matrix_idx(transformation.matrix_mode);
    auto& mat = transformation.matrices[idx];
    transformation.matrices_stack[idx].push_back(mat);

    LOG_D("Matrix %s:", glEnumToString(transformation.matrix_mode))
    print_matrix(transformation.matrices[matrix_idx(transformation.matrix_mode)]);
}

void glPopMatrix( void ) {
    LOG()
    LOG_D("glPopMatrix()")

    LIST_RECORD(glPopMatrix, {})

    auto& transformation = g_glstate.fpe_uniform.transformation;

    auto idx = matrix_idx(transformation.matrix_mode);
    auto& mat = transformation.matrices[idx];
    mat = transformation.matrices_stack[idx].back();
    transformation.matrices_stack[idx].pop_back();

    LOG_D("Matrix %s:", glEnumToString(transformation.matrix_mode))
    print_matrix(transformation.matrices[matrix_idx(transformation.matrix_mode)]);
}
