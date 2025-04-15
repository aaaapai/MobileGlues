//
// Created by hanji on 2025/2/3.A
//

#ifndef MOBILEGLUES_GL_MG_H
#define MOBILEGLUES_GL_MG_H

#include "gl.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI GLAPIENTRY void glClearDepth( GLclampd depth );
GLAPI GLAPIENTRY void glHint( GLenum target, GLenum mode );
GLAPI GLAPIENTRY void glDepthMask (GLboolean flag);
GLAPI GLAPIENTRY void glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
GLAPI GLAPIENTRY void glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI GLAPIENTRY void glClear (GLbitfield mask);
GLAPI GLAPIENTRY void glClearColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

#ifdef __cplusplus
}
#endif


#endif //MOBILEGLUES_GL_MG_H
