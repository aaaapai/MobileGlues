//
// Created by Swung 0x48 on 2025/3/27.
//

#ifndef MOBILEGLUES_FRAMEBUFFER_H
#define MOBILEGLUES_FRAMEBUFFER_H

#include <vector>

#include "../gl.h"
#include "../log.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI GLAPIENTRY void glCreateFramebuffers(GLsizei n, GLuint* framebuffers);

GLAPI GLAPIENTRY void glNamedFramebufferRenderbuffer(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);

#ifdef __cplusplus
}
#endif

#endif //MOBILEGLUES_FRAMEBUFFER_H
