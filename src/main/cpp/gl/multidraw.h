//
// Created by Swung 0x48 on 2025-04-20.
//

#ifndef MOBILEGLUES_MULTIDRAW_H
#define MOBILEGLUES_MULTIDRAW_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>
#include <mutex>
#include <cmath>
#include <cassert>
#include <arm_neon.h>
#include <thread>
#include <GLES/gl3.h>
#include "../includes.h"
#include <GL/gl.h>
#include <GL/glcorearb.h>
#include "log.h"
#include "../gles/loader.h"
#include "mg.h"

#ifdef __cplusplus
extern "C" {
#endif

struct draw_elements_indirect_command_t {
    GLuint  count;
    GLuint  instanceCount;
    GLuint  firstIndex;
    GLint   baseVertex;
    GLuint  baseInstance;
    GLuint  reservedMustBeZero;
};

struct drawcmd_compute_t {
    GLuint  firstIndex;
    GLint   baseVertex;
};

GLAPI GLAPIENTRY void glMultiDrawElementsBaseVertex(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_indirect(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_multiindirect(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_basevertex(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_drawelements(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_compute(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_deepseek_one(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_deepseek_two(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_native(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);

GLAPI GLAPIENTRY void glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_indirect(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_multiindirect(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_basevertex(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_drawelements(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_compute(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_deepseek_one(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_deepseek_two(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_native(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);

GLAPI GLAPIENTRY void glMultiDrawElementsIndirect(GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsIndirect_deepseek_one(GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride);

#ifdef __cplusplus
}
#endif

#endif //MOBILEGLUES_MULTIDRAW_H
