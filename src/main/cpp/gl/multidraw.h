//
// Created by Swung 0x48 on 2025-04-20.
//

#ifndef MOBILEGLUES_MULTIDRAW_H
#define MOBILEGLUES_MULTIDRAW_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <GLES3/gl3.h>
#include "../includes.h"
#include "GL/gl.h"
#include "GL/glcorearb.h"
#include "log.h"
#include "../gles/loader.h"
#include "mg.h"

struct draw_elements_indirect_command_t {
    GLuint  count;
    GLuint  instanceCount;
    GLuint  firstIndex;
    GLint   baseVertex;
    GLuint  reservedMustBeZero;
};

struct drawcmd_compute_t {
    GLuint  firstIndex;
    GLint   baseVertex;
};


typedef struct {
    bool ready;
    GLuint indirectRenderBuffer;
} ltw_basevertex_renderer_t;
typedef struct {
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLint baseVertex;
    GLuint reservedMustBeZero;
} indirect_pass_t;
GLint type_bytes(GLenum type);
GLuint multidraw_element_buffer;
#define MAX_BOUND_BUFFERS 9
GLuint bound_buffers[MAX_BOUND_BUFFERS];

GLAPI GLAPIENTRY void glMultiDrawElementsBaseVertex(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_indirect(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_multiindirect(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_basevertex(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_drawelements(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);
GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_compute(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex);

GLAPI GLAPIENTRY void glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_indirect(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_multiindirect(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_basevertex(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_drawelements(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_compute(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);
GLAPI GLAPIENTRY void mg_glMultiDrawElements_ltw(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount);

#endif //MOBILEGLUES_MULTIDRAW_H
