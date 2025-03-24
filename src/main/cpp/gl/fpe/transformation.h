//
// Created by Swung 0x48 on 2025/2/8.
//

#ifndef FOLD_CRAFT_LAUNCHER_TRANSFORMATION_H
#define FOLD_CRAFT_LAUNCHER_TRANSFORMATION_H

#include "../gl.h"
#include "../gles/loader.h"
#include "../gl/log.h"
#include "fpe.hpp"
#include <glm/glm.hpp>

int matrix_idx(GLenum matrix_mode);

void print_matrix(const glm::mat4& mat);

#ifdef __cplusplus
extern "C" {
#endif

GLAPI GLAPIENTRY void glMatrixMode( GLenum mode );
GLAPI GLAPIENTRY void glLoadIdentity();
GLAPI GLAPIENTRY void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
GLAPI GLAPIENTRY void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near_val, GLfloat far_val);
GLAPI GLAPIENTRY void glScalef( GLfloat x, GLfloat y, GLfloat z );
GLAPI GLAPIENTRY void glTranslatef( GLfloat x, GLfloat y, GLfloat z );
GLAPI GLAPIENTRY void glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z );

GLAPI GLAPIENTRY void glMultMatrixf(const GLfloat *m);

GLAPI GLAPIENTRY void glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z );
GLAPI GLAPIENTRY void glScaled(GLdouble x, GLdouble y, GLdouble z );
GLAPI GLAPIENTRY void glTranslated(GLdouble x, GLdouble y, GLdouble z );

GLAPI GLAPIENTRY void glPushMatrix( void );
GLAPI GLAPIENTRY void glPopMatrix( void );

#ifdef __cplusplus
}
#endif

#endif //FOLD_CRAFT_LAUNCHER_TRANSFORMATION_H
