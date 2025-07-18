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
    LOG_D("glNamedFramebufferDrawBuffers, framebuffer = %u, n = %d", framebuffer, n)

    INIT_CHECK_GL_ERROR

    // 检查draw buffer数量是否超过硬件限制
    if (n > MAX_DRAW_BUFFERS) {
        LOG_E("ERROR: Number of draw buffers (%d) exceeds MAX_DRAW_BUFFERS (%d)", n, MAX_DRAW_BUFFERS)
        return;
    }

    // 获取或创建framebuffer对象
    framebuffer_t* fb = bound_framebuffer;
    if (!fb || fb->id != framebuffer) {
        // 这里简化处理，实际应该有个framebuffer管理表
        fb = (framebuffer_t*)malloc(sizeof(framebuffer_t));
        memset(fb, 0, sizeof(framebuffer_t));
        fb->id = framebuffer;
        bound_framebuffer = fb;
    }

    // 更新draw attachments
    if (!fb->draw_attachment) {
        fb->draw_attachment = (attachment_t*)malloc(sizeof(attachment_t) * n);
        memset(fb->draw_attachment, 0, sizeof(attachment_t) * n);
    }

    // 保存当前绑定状态
    GLint prevFBO;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevFBO);

    // 绑定并设置draw buffers
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glDrawBuffers(n, bufs);

    // 更新attachment信息
    for (GLsizei i = 0; i < n; ++i) {
        fb->draw_attachment[i].target = GL_DRAW_FRAMEBUFFER;
        fb->draw_attachment[i].attachment = bufs[i];
    }

    // 恢复之前的绑定
    if (prevFBO != (GLint)framebuffer) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevFBO);
    }

    CHECK_GL_ERROR
} //DeepSeek*2

void glNamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level) {
    LOG()
    LOG_D("glNamedFramebufferTexture: fb=%u, attach=0x%X, tex=%u, level=%d", 
          framebuffer, attachment, texture, level)

    INIT_CHECK_GL_ERROR

    // 获取或创建framebuffer对象
    framebuffer_t* fb = bound_framebuffer;
    if (!fb || fb->id != framebuffer) {
        fb = (framebuffer_t*)malloc(sizeof(framebuffer_t));
        memset(fb, 0, sizeof(framebuffer_t));
        fb->id = framebuffer;
        bound_framebuffer = fb;
    }

    // 确定attachment类型(READ/DRAW)
    GLenum target = (attachment == GL_DEPTH_ATTACHMENT || 
                    attachment == GL_STENCIL_ATTACHMENT ||
                    attachment == GL_DEPTH_STENCIL_ATTACHMENT) 
                   ? GL_DRAW_FRAMEBUFFER : GL_READ_FRAMEBUFFER;

    // 更新attachment结构
    attachment_t* attach = (target == GL_DRAW_FRAMEBUFFER) ? 
                          &fb->draw_attachment : &fb->read_attachment;
    if (!attach) {
        attach = (attachment_t*)malloc(sizeof(attachment_t));
        memset(attach, 0, sizeof(attachment_t));
        if (target == GL_DRAW_FRAMEBUFFER) {
            fb->draw_attachment = attach;
        } else {
            fb->read_attachment = attach;
        }
    }

    attach->textarget = GL_TEXTURE_2D;
    attach->texture = texture;
    attach->level = level;
    attach->attachment = attachment;

    // 使用rebind_framebuffer管理状态
    rebind_framebuffer(fb, target);

    // 执行实际GL操作
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, level);

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
