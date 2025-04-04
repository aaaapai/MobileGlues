//
// Created by Swung 0x48 on 2025/3/27.
//

#include "framebuffer.h"
#define DEBUG 0

void glCreateFramebuffers(GLsizei n, GLuint* framebuffers) {
    LOG()
    LOG_D("glCreateFramebuffers, n = %d", n)

    INIT_CHECK_GL_ERROR

    // do actual gen to ES driver
    GLES.glGenFramebuffers(n, framebuffers);
    CHECK_GL_ERROR_NO_INIT
}

//GLAPI GLAPIENTRY void glNamedFramebufferRenderbuffer(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
//    LOG()
//    LOG_D("glNamedFramebufferRenderbuffer, framebuffer = %d, attachment = 0x%x, renderbuffertarget = 0x%x, renderbuffer = %d",
//          framebuffer, glEnumToString(attachment), glEnumToString(renderbuffer), renderbuffer)
//
//    INIT_CHECK_GL_ERROR
//
////    GLES.glFramebufferRenderbuffer()
//}
