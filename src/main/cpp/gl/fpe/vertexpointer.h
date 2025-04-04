//
// Created by Swung 0x48 on 2025/2/10.
//

#ifndef FOLD_CRAFT_LAUNCHER_VERTEXPOINTER_H
#define FOLD_CRAFT_LAUNCHER_VERTEXPOINTER_H

#include "../gl.h"
#include "../gles/loader.h"
#include "../gl/log.h"
#include "types.h"
#include "fpe.hpp"

int vp2idx(GLenum vp);
uint32_t vp_mask(GLenum vp);
GLenum idx2vp(int idx);

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

#endif //FOLD_CRAFT_LAUNCHER_VERTEXPOINTER_H
