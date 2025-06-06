//
// Created by hanji on 2025/2/6.
//

#ifndef MOBILEGLUES_FRAMEBUFFER_H
#define MOBILEGLUES_FRAMEBUFFER_H

#include "GL/gl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    GLenum textarget;
    GLuint texture;
    GLint level;
} attachment_t;

typedef struct {
    GLenum current_target;
    attachment_t* draw_attachment;
    attachment_t* read_attachment;
} framebuffer_t;

GLint getMaxDrawBuffers();

GLAPI GLAPIENTRY void glBindFramebuffer(GLenum target, GLuint framebuffer);

GLAPI GLAPIENTRY void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

GLAPI GLAPIENTRY void glDrawBuffer(GLenum buf);

GLAPI GLAPIENTRY void glDrawBuffers(GLsizei n, const GLenum *bufs);

GLAPI GLAPIENTRY void glReadBuffer(GLenum src);

GLAPI GLAPIENTRY GLenum glCheckFramebufferStatus(GLenum target);

GLAPI GLAPIENTRY void glGenFramebuffers (GLsizei n, GLuint *framebuffers);

GLAPI GLAPIENTRY void glDeleteFramebuffers (GLsizei n, const GLuint *framebuffers);

#ifdef __cplusplus
}
#endif

#endif //MOBILEGLUES_FRAMEBUFFER_H
