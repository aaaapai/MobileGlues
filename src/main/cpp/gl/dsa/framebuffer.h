//
// Created by Swung 0x48 on 2025/3/27.
//

#ifndef MOBILEGLUES_FRAMEBUFFER_H
#define MOBILEGLUES_FRAMEBUFFER_H

#include <vector>

#include "GL/gl.h"
#include "../log.h"
#include "../framebuffer.h"

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

struct framebuffer_t* bound_framebuffer;
extern GLint MAX_DRAW_BUFFERS;
void rebind_framebuffer(GLenum old_attachment, GLenum target_attachment);
GLint getMaxDrawBuffers();

GLAPI GLAPIENTRY void glCreateFramebuffers(GLsizei n, GLuint* framebuffers);

GLAPI GLAPIENTRY void glNamedFramebufferRenderbuffer(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);

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
