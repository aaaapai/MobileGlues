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

    // 检查draw buffer数量是否超过硬件限制（ES 3.0+才支持多渲染目标）
    if (n > MAX_DRAW_BUFFERS) {
        LOG_E("ERROR: Number of draw buffers (%d) exceeds MAX_DRAW_BUFFERS (%d)", n, MAX_DRAW_BUFFERS)
        return;
    }

    // 保存当前绑定状态（使用ES兼容的查询方式）
    GLint prevFBO = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

    // 绑定并设置（ES需要显式绑定，无法跳过）
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glDrawBuffers(n, bufs);

    // 恢复绑定（避免冗余操作）
    if (prevFBO != (GLint)framebuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, prevFBO); // 注意：ES中恢复时用GL_FRAMEBUFFER
    }

    CHECK_GL_ERROR_NO_INIT
} //DeepSeek*4

void glNamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level) {
    LOG()
    LOG_D("glNamedFramebufferTexture: framebuffer=%u, attachment=0x%04X, texture=%u, level=%d", framebuffer, attachment, texture, level)

    // 保存当前绑定的FBO
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    
    // 更新bound_framebuffer状态
    if (!bound_framebuffer || bound_framebuffer->id != framebuffer) {
        if (bound_framebuffer) free(bound_framebuffer);
        struct bound_framebuffer = (struct framebuffer_t*)malloc(sizeof(struct framebuffer_t));
        memset(bound_framebuffer, 0, sizeof(framebuffer_t));
        struct bound_framebuffer->id = framebuffer;
    }
    
    // 确定attachment类型
    GLenum target = (attachment >= GL_COLOR_ATTACHMENT0 && attachment <= GL_COLOR_ATTACHMENT0 + MAX_DRAW_BUFFERS) 
                   ? GL_DRAW_FRAMEBUFFER : GL_READ_FRAMEBUFFER;
    
    // 更新对应的attachment结构
    struct attachment_t** target_attach = (target == GL_DRAW_FRAMEBUFFER) 
                                 ? &bound_framebuffer->draw_attachment 
                                 : &bound_framebuffer->read_attachment;
    
    if (!*target_attach) {
        *target_attach = (struct attachment_t*)malloc(sizeof(struct attachment_t));
        memset(*target_attach, 0, sizeof(struct attachment_t));
    }
    
    (*target_attach)->textarget = GL_TEXTURE_2D;
    (*target_attach)->texture = texture;
    (*target_attach)->level = level;
    
    // 执行绑定和纹理附加
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    struct bound_framebuffer->current_target = GL_FRAMEBUFFER;
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, level);
    
    // 恢复之前的绑定
    glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    if (bound_framebuffer) {
        struct bound_framebuffer->current_target = (prevFBO == framebuffer) ? GL_FRAMEBUFFER : 0;
    }
    
    CHECK_GL_ERROR
}

void glNamedFramebufferTextureLayer(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer) {
    LOG()
    LOG_D("glNamedFramebufferTextureLayer: fb=%u, attach=0x%04X, tex=%u, level=%d, layer=%d", 
          framebuffer, attachment, texture, level, layer)

    INIT_CHECK_GL_ERROR

    // 保存当前绑定状态（兼容GLES）
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

    // 仅当framebuffer不同时才重新绑定
    if (prevFBO != (GLint)framebuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }

    // 执行纹理层附加
    glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, texture, level, layer);

    // 恢复绑定（仅当需要时）
    if (prevFBO != (GLint)framebuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    }

    CHECK_GL_ERROR_NO_INIT
}

void glNamedFramebufferReadBuffer(GLuint framebuffer, GLenum src) {
    LOG()
    LOG_D("glNamedFramebufferReadBuffer: fb=%u, src=0x%04X", framebuffer, src)

    INIT_CHECK_GL_ERROR

    // 保存当前READ_FRAMEBUFFER绑定状态
    GLint prevFBO;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prevFBO);

    // 仅当framebuffer不同时才重新绑定
    if (prevFBO != (GLint)framebuffer) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    }

    // 设置读取缓冲区
    glReadBuffer(src);

    // 恢复绑定（仅当需要时）
    if (prevFBO != (GLint)framebuffer) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, prevFBO);
    }

    CHECK_GL_ERROR_NO_INIT
}

void glBlitNamedFramebuffer(GLuint readFramebuffer, GLuint drawFramebuffer,
                           GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                           GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                           GLbitfield mask, GLenum filter) {
    LOG()
    LOG_D("glBlitNamedFramebuffer: readFB=%u, drawFB=%u, srcRect=(%d,%d)-(%d,%d), dstRect=(%d,%d)-(%d,%d), mask=0x%X, filter=0x%X",
          readFramebuffer, drawFramebuffer,
          srcX0, srcY0, srcX1, srcY1,
          dstX0, dstY0, dstX1, dstY1,
          mask, filter)

    INIT_CHECK_GL_ERROR

    // 保存当前绑定状态（兼容GLES方式）
    GLint prevReadFBO = 0, prevDrawFBO = 0;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prevReadFBO);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevDrawFBO);

    // 优化绑定：仅在不同时才重新绑定
    if (prevReadFBO != (GLint)readFramebuffer) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);
    }
    if (prevDrawFBO != (GLint)drawFramebuffer) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);
    }

    // 执行blit操作
    GLES.glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1,
                         dstX0, dstY0, dstX1, dstY1,
                         mask, filter);

    // 恢复绑定（仅当需要时）
    if (prevReadFBO != (GLint)readFramebuffer) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, prevReadFBO);
    }
    if (prevDrawFBO != (GLint)drawFramebuffer) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevDrawFBO);
    }

    CHECK_GL_ERROR_NO_INIT
}
