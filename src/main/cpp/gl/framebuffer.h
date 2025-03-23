//
// Created by hanji on 2025/2/6.
//

#ifndef MOBILEGLUES_FRAMEBUFFER_H
#define MOBILEGLUES_FRAMEBUFFER_H

#include "glcorearb.h"

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

GLAPI APIENTRY void glBindFramebuffer(GLenum target, GLuint framebuffer);

GLAPI APIENTRY void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

GLAPI APIENTRY void glDrawBuffer(GLenum buf);

GLAPI APIENTRY void glDrawBuffers(GLsizei n, const GLenum *bufs);

GLAPI APIENTRY void glReadBuffer(GLenum src);

GLAPI APIENTRY GLenum glCheckFramebufferStatus(GLenum target);

#ifdef __cplusplus
}
#endif

#endif //MOBILEGLUES_FRAMEBUFFER_H
