//
// Created by Swung 0x48 on 2025/2/10.
//

#ifndef MOBILEGLUES_VERTEXPOINTER_H
#define MOBILEGLUES_VERTEXPOINTER_H

#include "GL/gl.h"
#include "../gles/loader.h"
#include "../gl/log.h"
#include "types.h"
#include "fpe.hpp"
#include "vertexpointer_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI GLAPIENTRY void glVertexPointer(GLint size,
                                      GLenum type,
                                      GLsizei stride,
                                      const void *pointer);
GLAPI GLAPIENTRY void glNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer);
GLAPI GLAPIENTRY void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
GLAPI GLAPIENTRY void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
GLAPI GLAPIENTRY void glIndexPointer(GLenum type, GLsizei stride, const GLvoid *ptr );

GLAPI GLAPIENTRY void glEnableClientState(GLenum cap);
GLAPI GLAPIENTRY void glDisableClientState(GLenum cap);

#ifdef __cplusplus
}
#endif

#endif //MOBILEGLUES_VERTEXPOINTER_H
