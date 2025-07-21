//
// Created by Swung 0x48 on 2025/3/27.
//

#include "framebuffer.h"

#include "../log.h"
#include "../config/settings.h"
#include <ankerl/unordered_dense.h>

#define DEBUG 1

extern GLint MAX_DRAW_BUFFERS;
extern void rebind_framebuffer(GLenum old_attachment, GLenum target_attachment);
extern GLint getMaxDrawBuffers();

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
   LOG_D("glNamedFramebufferDrawBuffer, framebuffer = %u, buf = 0x%x", framebuffer, buf)

    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glDrawBuffer(buf);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevFBO);
    
    CHECK_GL_ERROR
}

void glNamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum *bufs) {

    LOG()
    LOG_D("glNamedFramebufferDrawBuffers, framebuffer: %u, n: %d, bufs: %p", framebuffer, n, bufs)

    // 保存当前绑定的帧缓冲区
    GLint prevFramebuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFramebuffer);
    
    // 绑定目标帧缓冲区
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    
    // 设置绘制缓冲区
    glDrawBuffers(n, bufs);
    
    // 恢复之前绑定的帧缓冲区
    glBindFramebuffer(GL_FRAMEBUFFER, prevFramebuffer);

}

void glNamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level) {

    LOG()
    LOG_D("glNamedFramebufferTexture, framebuffer: %u, attachment: 0x%04X, texture: %u, level: %d", 
          framebuffer, attachment, texture, level)

    // 验证 attachment 参数是否合法
    if (attachment >= GL_COLOR_ATTACHMENT0 && attachment < GL_COLOR_ATTACHMENT0 + getMaxDrawBuffers()) {
        // 有效的颜色附件
    } else if (attachment == GL_DEPTH_ATTACHMENT || 
               attachment == GL_STENCIL_ATTACHMENT || 
               attachment == GL_DEPTH_STENCIL_ATTACHMENT) {
        // 有效的深度/模板附件
    } else {
        LOG_E("ERROR: Invalid attachment parameter: 0x%04X", attachment);
        return;
    }

    // 检查 framebuffer 是否为 0（默认帧缓冲区）
    if (framebuffer == 0) {
        LOG_D("Default framebuffer (0) cannot be modified with glNamedFramebufferTexture");
        return;
    }

    // 绑定帧缓冲区到当前目标（假设 GL_DRAW_FRAMEBUFFER 为最常见目标）
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

    // 更新帧缓冲区附件信息
    struct framebuffer_t* fb = bound_framebuffer;  // 显式使用 struct 前缀
    if (fb) {
        if (attachment >= GL_COLOR_ATTACHMENT0 && attachment < GL_COLOR_ATTACHMENT0 + MAX_DRAW_BUFFERS) {
            // 处理颜色附件
            GLuint index = attachment - GL_COLOR_ATTACHMENT0;
            if (index >= MAX_DRAW_BUFFERS) {
                LOG_D("Color attachment index out of bounds: %u", index);
                return;
            }

            if (!fb->draw_attachment) {
                fb->draw_attachment = new struct attachment_t[MAX_DRAW_BUFFERS];  // 同样显式使用 struct
            }

            fb->draw_attachment[index].textarget = texture ? GL_TEXTURE_2D : GL_NONE;
            fb->draw_attachment[index].texture = texture;
            fb->draw_attachment[index].level = level;
        } else {
            // 处理深度/模板附件
            if (!fb->read_attachment) {
                fb->read_attachment = new struct attachment_t;  // 显式使用 struct
            }

            fb->read_attachment->textarget = texture ? GL_TEXTURE_2D : GL_NONE;
            fb->read_attachment->texture = texture;
            fb->read_attachment->level = level;
        }
    }

    // 实际调用 GLES 函数
    GLES.glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, level);

    // 检查帧缓冲区完整性
    GLenum status = GLES.glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        LOG_D("Framebuffer not complete after attachment: 0x%04X", status);
    }
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

void glNamedFramebufferRenderbuffer(GLuint framebuffer, GLenum attachment, 
                                      GLenum renderbuffertarget, GLuint renderbuffer) {

    LOG()
    
    // 保存当前绑定的Framebuffer
    GLint prevFramebuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFramebuffer);
    
    // 绑定指定的Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    // 附加Renderbuffer
    GLES.glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, renderbuffertarget, renderbuffer);
    
    // 恢复之前绑定的Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFramebuffer);
}

void glNamedFramebufferParameteri(GLuint framebuffer, GLenum pname, GLint param) {

    LOG()
    
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO); // 保存当前绑定的 FBO
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);   // 绑定目标 FBO
    GLES.glFramebufferParameteri(GL_FRAMEBUFFER, pname, param); // 设置参数
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO);    // 恢复绑定
}

void glGetNamedFramebufferAttachmentParameteriv(GLuint framebuffer, GLenum attachment, GLenum pname, GLint* params) {

    LOG()
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO); // 保存当前绑定的 FBO
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);  // 绑定目标 FBO
    GLES.glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment, pname, params); // 查询参数
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO); // 恢复绑定
}

void glGetNamedFramebufferParameteriv(GLuint framebuffer, GLenum pname, GLint* param) {

    LOG()
    
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO); // 保存当前绑定的 FBO
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);  // 绑定目标 FBO
    GLES.glGetFramebufferParameteriv(GL_FRAMEBUFFER, pname, param); // 查询参数
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO); // 恢复绑定
}

GLenum glCheckNamedFramebufferStatus(GLuint framebuffer, GLenum target) {

    LOG()
    
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO); // 保存当前绑定的 FBO
    glBindFramebuffer(target, framebuffer);         // 绑定目标 FBO
    GLenum status = glCheckFramebufferStatus(target); // 检查状态
    glBindFramebuffer(target, (GLuint)prevFBO);      // 恢复绑定
    return status;
}

void glClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value) {

    LOG()
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    GLES.glClearBufferfv(buffer, drawbuffer, value);
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO);
}

void glClearNamedFramebufferiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value) {

    LOG()
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    GLES.glClearBufferiv(buffer, drawbuffer, value);
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO);
}

void glClearNamedFramebufferuiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value) {

    LOG()
    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    GLES.glClearBufferuiv(buffer, drawbuffer, value);
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO);
} //Depk

