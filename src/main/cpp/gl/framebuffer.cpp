//
// Created by hanji on 2025/2/6.
//

#include "framebuffer.h"
#include "log.h"
#include "../config/settings.h"

#define DEBUG 0

struct framebuffer_t* bound_framebuffer;

GLint MAX_DRAW_BUFFERS = 0;

GLint getMaxDrawBuffers() {
    if (!MAX_DRAW_BUFFERS) {
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &MAX_DRAW_BUFFERS);
    }
    return MAX_DRAW_BUFFERS;
}

void rebind_framebuffer(GLenum old_attachment, GLenum target_attachment) {
    if (!bound_framebuffer)
        return;

    struct attachment_t* attach;
    if (bound_framebuffer->current_target == GL_DRAW_FRAMEBUFFER)
        attach = bound_framebuffer->draw_attachment;
    else
        attach = bound_framebuffer->read_attachment;

    if (!attach)
        return;

    struct attachment_t attachment = attach[old_attachment - GL_COLOR_ATTACHMENT0];

    GLES.glFramebufferTexture2D(bound_framebuffer->current_target, target_attachment, attachment.textarget, attachment.texture, attachment.level);
}

void glBindFramebuffer(GLenum target, GLuint framebuffer) {
    LOG()

    INIT_CHECK_GL_ERROR

    LOG_D("glBindFramebuffer(0x%x, %d)", target, framebuffer)

    GLES.glBindFramebuffer(target, framebuffer);
    CHECK_GL_ERROR_NO_INIT

    if (!bound_framebuffer) {
        bound_framebuffer = (struct framebuffer_t*)malloc(sizeof(struct framebuffer_t));
        memset(bound_framebuffer, 0, sizeof(struct framebuffer_t));
    }

    switch (target) {
        case GL_DRAW_FRAMEBUFFER:
            free(bound_framebuffer->draw_attachment);
            bound_framebuffer->draw_attachment = (struct attachment_t*)malloc(getMaxDrawBuffers() * sizeof(struct attachment_t));
            break;
        case GL_READ_FRAMEBUFFER:
            free(bound_framebuffer->read_attachment);
            bound_framebuffer->read_attachment = (struct attachment_t*)malloc(getMaxDrawBuffers() * sizeof(struct attachment_t));
            break;
        case GL_FRAMEBUFFER:
            free(bound_framebuffer->draw_attachment);
            bound_framebuffer->draw_attachment = (struct attachment_t*)malloc(getMaxDrawBuffers() * sizeof(struct attachment_t));
            free(bound_framebuffer->read_attachment);
            bound_framebuffer->read_attachment = (struct attachment_t*)malloc(getMaxDrawBuffers() * sizeof(struct attachment_t));
            break;
        default:
            break;
    }

    CHECK_GL_ERROR_NO_INIT
}

void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
    LOG()

    LOG_D("glFramebufferTexture2D(0x%x, 0x%x, 0x%x, %d, %d)", target, attachment, textarget, texture, level)

    if (bound_framebuffer && attachment - GL_COLOR_ATTACHMENT0 <= getMaxDrawBuffers()) {
        struct attachment_t* attach;
        if (target == GL_DRAW_FRAMEBUFFER)
            attach = bound_framebuffer->draw_attachment;
        else
            attach = bound_framebuffer->read_attachment;

        if (attach) {
            attach[attachment - GL_COLOR_ATTACHMENT0].textarget = textarget;
            attach[attachment - GL_COLOR_ATTACHMENT0].texture = texture;
            attach[attachment - GL_COLOR_ATTACHMENT0].level = level;
        }

        bound_framebuffer->current_target = target;
    }

    GLES.glFramebufferTexture2D(target, attachment, textarget, texture, level);

        // 检查是否失败（例如 GL_RGBA32F 不支持）
    GLenum error = GLES.glGetError();
    if (error == GL_INVALID_OPERATION) {
        LOG_W("Falling back to GL_RGBA16F due to GL_RGBA32F not supported");

        // 获取当前纹理格式（假设可以查询）
        GLint internalFormat;
        GLES.glBindTexture(textarget, texture);
        GLES.glGetTexLevelParameteriv(textarget, level, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
        
        // 如果是 GL_RGBA32F，尝试降级到 GL_RGBA16F
        if (internalFormat == GL_RGBA32F) {

            struct attachment_t* attach;
            // 重新创建 16F 纹理
            GLuint fallbackTex;
            GLES.glGenTextures(1, &fallbackTex);
            GLES.glBindTexture(textarget, fallbackTex);
            
            // 获取原纹理尺寸
            GLint width, height;
            GLES.glGetTexLevelParameteriv(textarget, level, GL_TEXTURE_WIDTH, &width);
            GLES.glGetTexLevelParameteriv(textarget, level, GL_TEXTURE_HEIGHT, &height);
            
            // 改用 GL_RGBA16F
            GLES.glTexImage2D(
                textarget, 0, GL_RGBA16F, width, height, 0,
                GL_RGBA, GL_FLOAT, nullptr
            );
            
            // 重新绑定降级后的纹理
            GLES.glFramebufferTexture2D(target, attachment, textarget, fallbackTex, level);
            
            // 检查是否成功
            if (glGetError() == GL_NO_ERROR) {
                LOG_W("Fallback to GL_RGBA16F succeeded");
                // 更新绑定的纹理（可选）
                if (bound_framebuffer && attach) {
                    attach[attachment - GL_COLOR_ATTACHMENT0].texture = fallbackTex;
                }
            } else {
                LOG_E("Fallback to GL_RGBA16F failed");
                GLES.glDeleteTextures(1, &fallbackTex);
            }
        }
    } //DeepSeek

    CHECK_GL_ERROR
}

void glDrawBuffer(GLenum buffer) {
    LOG()

    GLint currentFBO;
    GLES.glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO);

    if (currentFBO == 0) {
        GLenum buffers[1] = {GL_NONE};
        switch (buffer) {
            case GL_FRONT:
            case GL_BACK:
            case GL_NONE:
                buffers[0] = buffer;
                GLES.glDrawBuffers(1, buffers);
                break;
            default:
                break;
        }
    } else {
        GLint maxAttachments;
        GLES.glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttachments);

        if (buffer == GL_NONE) {
            auto *buffers = (GLenum *)alloca(maxAttachments * sizeof(GLenum));
            for (int i = 0; i < maxAttachments; i++) {
                buffers[i] = GL_NONE;
            }
            GLES.glDrawBuffers(maxAttachments, buffers);
        } else if (buffer >= GL_COLOR_ATTACHMENT0 &&
                   buffer < GL_COLOR_ATTACHMENT0 + maxAttachments) {
            auto *buffers = (GLenum *)alloca(maxAttachments * sizeof(GLenum));
            for (int i = 0; i < maxAttachments; i++) {
                buffers[i] = (i == (buffer - GL_COLOR_ATTACHMENT0)) ? buffer : GL_NONE;
            }
            GLES.glDrawBuffers(maxAttachments, buffers);
        }
    }
}

void glDrawBuffers(GLsizei n, const GLenum *bufs) {
    LOG()

    LOG_D("glDrawBuffers(%d, %p), [0]=0x%x", n, bufs, n ? bufs[0] : 0)

    GLenum new_bufs[n];

    for (int i = 0; i < n; i++) {
        if (bufs[i] >= GL_COLOR_ATTACHMENT0 && bufs[i] <= GL_COLOR_ATTACHMENT0 + getMaxDrawBuffers()) {
            GLenum target_attachment = GL_COLOR_ATTACHMENT0 + i;
            new_bufs[i] = target_attachment;
            rebind_framebuffer(bufs[i], target_attachment);
        } else {
            new_bufs[i] = bufs[i];
        }
    }

    GLES.glDrawBuffers(n, new_bufs);

    CHECK_GL_ERROR
}

void glReadBuffer(GLenum src) {
    LOG()
    LOG_D("glReadBuffer, src = %s", glEnumToString(src))

    GLES.glReadBuffer(src);
}

GLenum glCheckFramebufferStatus(GLenum target) {
    LOG()
    GLenum status = GLES.glCheckFramebufferStatus(target);
    if(global_settings.ignore_error == IgnoreErrorLevel::Full && status != GL_FRAMEBUFFER_COMPLETE) {
        LOG_W_FORCE("Framebuffer %d isn't GL_FRAMEBUFFER_COMPLETE: %d", target, status)
        LOG_W_FORCE("Now try to cheat.")
        return GL_FRAMEBUFFER_COMPLETE;
    }
    return status;
    CHECK_GL_ERROR
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
