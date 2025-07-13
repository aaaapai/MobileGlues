//
// Created by hanji on 2025/2/6.
//

#ifndef MOBILEGLUES_FRAMEBUFFER_H
#define MOBILEGLUES_FRAMEBUFFER_H

#include "GL/gl.h"

#ifdef __cplusplus
extern "C" {
#endif

struct attachment_t {
    GLenum textarget;
    GLuint texture;
    GLint level;
};

struct framebuffer_t {
    GLenum current_target;
    struct attachment_t* draw_attachment;
    struct attachment_t* read_attachment;
};

GLint getMaxDrawBuffers();

GLAPI GLAPIENTRY void glBindFramebuffer(GLenum target, GLuint framebuffer);

GLAPI GLAPIENTRY void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

GLAPI GLAPIENTRY void glDrawBuffer(GLenum buf);

GLAPI GLAPIENTRY void glDrawBuffers(GLsizei n, const GLenum *bufs);

GLAPI GLAPIENTRY void glReadBuffer(GLenum src);

GLAPI GLAPIENTRY GLenum glCheckFramebufferStatus(GLenum target);

GLAPI GLAPIENTRY void glGenFramebuffers (GLsizei n, GLuint *framebuffers);

GLAPI GLAPIENTRY void glDeleteFramebuffers (GLsizei n, const GLuint *framebuffers);

GLAPI GLAPIENTRY void glNamedFramebufferDrawBuffer (GLuint framebuffer, GLenum buf);

GLAPI GLAPIENTRY void glNamedFramebufferDrawBuffers (GLuint framebuffer, GLsizei n, const GLenum *bufs);

GLAPI GLAPIENTRY void glNamedFramebufferTexture (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);

GLAPI GLAPIENTRY void glNamedFramebufferTextureLayer (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);

GLAPI GLAPIENTRY void glNamedFramebufferReadBuffer (GLuint framebuffer, GLenum src);

GLAPI GLAPIENTRY void glBlitNamedFramebuffer (GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);

#ifdef __cplusplus
}
#endif

#endif //MOBILEGLUES_FRAMEBUFFER_H
