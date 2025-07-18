//
// Created by Swung 0x48 on 2025/3/27.
//

#include "framebuffer.h"

#include "../framebuffer.h"
#include "../log.h"
#include "../config/settings.h"

#define DEBUG 0

extern struct framebuffer_t* bound_framebuffer;
extern GLint MAX_DRAW_BUFFERS;

void glCreateFramebuffers(GLsizei n, GLuint* framebuffers) {
    LOG()
    LOG_D("glCreateFramebuffers, n = %d", n)

    INIT_CHECK_GL_ERROR

    // do actual gen to ES driver
    GLES.glGenFramebuffers(n, framebuffers);
    CHECK_GL_ERROR_NO_INIT
}

void glNamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf) {
    LOG()
    
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glDrawBuffer(buf);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevFBO);
    
    CHECK_GL_ERROR
}

void glNamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum *bufs) {
    LOG()
    
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glDrawBuffers(n, bufs);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevFBO);
    
    CHECK_GL_ERROR
}

void glNamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level) {
    LOG()
    
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, level);
    glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    
    CHECK_GL_ERROR
}

void glNamedFramebufferTextureLayer(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer) {
    LOG()
    
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, texture, level, layer);
    glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    
    CHECK_GL_ERROR
}

void glNamedFramebufferReadBuffer(GLuint framebuffer, GLenum src) {
    LOG()
    
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glReadBuffer(src);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, prevFBO);
    
    CHECK_GL_ERROR
}

void glBlitNamedFramebuffer(GLuint readFramebuffer, GLuint drawFramebuffer, 
                           GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                           GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                           GLbitfield mask, GLenum filter) {
    LOG()
    
    // 保存当前绑定的帧缓冲
    GLint prevReadFBO, prevDrawFBO;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prevReadFBO);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevDrawFBO);
    
    // 绑定指定的帧缓冲
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);
    
    // 执行实际的像素复制操作
    GLES.glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1,
                          dstX0, dstY0, dstX1, dstY1,
                          mask, filter);
    
    // 恢复之前绑定的帧缓冲
    glBindFramebuffer(GL_READ_FRAMEBUFFER, prevReadFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevDrawFBO);
    
    CHECK_GL_ERROR
} //DeepSeek
