//
// Created by Swung 0x48 on 2025/3/27.
//

#include "framebuffer.h"

#include "../log.h"
#include "../config/settings.h"
#include "../../config/settings.h"
#include <ankerl/unordered_dense.h>

#define DEBUG 0

extern GLint MAX_DRAW_BUFFERS;
extern void rebind_framebuffer(GLenum old_attachment, GLenum target_attachment);
extern GLint getMaxDrawBuffers();

void glCreateFramebuffers(GLsizei n, GLuint* framebuffers) {
    LOG()
    LOG_D("glCreateFramebuffers, n = %d", n)

    if (!global_settings.ext_dsa) {
        return;
    }

    INIT_CHECK_GL_ERROR

    // do actual gen to ES driver
    GLES.glGenFramebuffers(n, framebuffers);
    CHECK_GL_ERROR_NO_INIT
}

void glNamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf) {
    LOG()
   LOG_D("glNamedFramebufferDrawBuffer, framebuffer = %u, buf = 0x%x", framebuffer, buf)

    if (!global_settings.ext_dsa) {
        return;
    }

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

    if (!global_settings.ext_dsa) {
        return;
    }

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

    if (!global_settings.ext_dsa) {
        return;
    }

    GLint prevFBO;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevFBO);

    // 获取实际纹理目标
    GLenum textarget = GL_TEXTURE_2D;
    if (texture != 0) {
        GLint prevTex;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTex);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        GLint real_target = 0;
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_TARGET, &real_target);
        if (real_target != 0) {
            textarget = static_cast<GLenum>(real_target);
        }
        glBindTexture(GL_TEXTURE_2D, prevTex);
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    
    struct framebuffer_t* fb = bound_framebuffer;  // 显式使用 struct 前缀
    GLuint index = attachment - GL_COLOR_ATTACHMENT0;
    if (fb) {
        if (attachment >= GL_COLOR_ATTACHMENT0 && attachment < GL_COLOR_ATTACHMENT0 + MAX_DRAW_BUFFERS) {
            if (!fb->draw_attachment) {
                fb->draw_attachment = new struct attachment_t[MAX_DRAW_BUFFERS];
            }
            fb->draw_attachment[index].textarget = textarget;
            fb->draw_attachment[index].texture = texture;
            fb->draw_attachment[index].level = level;
        } else if (attachment == GL_DEPTH_ATTACHMENT) {
            if (!fb->read_attachment) fb->read_attachment = new attachment_t;
            fb->read_attachment->textarget = textarget;
            fb->read_attachment[index]->texture = texture;
            fb->read_attachment[index]->level = level;
        } else if (attachment == GL_STENCIL_ATTACHMENT) {
            if (!fb->read_attachment) fb->read_attachment = new attachment_t;
            fb->read_attachment->textarget = textarget;
            fb->read_attachment[index]->texture = texture;
            fb->read_attachment[index]->level = level;
        }
    }

    // 使用实际纹理目标
    GLES.glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, textarget, texture, level);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevFBO);

    CHECK_GL_ERROR
}

void glNamedFramebufferTextureLayer(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer) {
    LOG()
    LOG_D("glNamedFramebufferTextureLayer: fb=%u, attach=0x%04X, tex=%u, level=%d, layer=%d", 
          framebuffer, attachment, texture, level, layer)


    if (!global_settings.ext_dsa) {
        return;
    }

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


    if (!global_settings.ext_dsa) {
        return;
    }

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


    if (!global_settings.ext_dsa) {
        return;
    }

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

    if (!global_settings.ext_dsa) {
        return;
    }

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

    if (!global_settings.ext_dsa) {
        return;
    }

    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO); // 保存当前绑定的 FBO
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);   // 绑定目标 FBO
    GLES.glFramebufferParameteri(GL_FRAMEBUFFER, pname, param); // 设置参数
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO);    // 恢复绑定
}

void glGetNamedFramebufferAttachmentParameteriv(GLuint framebuffer, GLenum attachment, GLenum pname, GLint* params) {

    LOG()

    if (!global_settings.ext_dsa) {
        return;
    }

    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO); // 保存当前绑定的 FBO
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);  // 绑定目标 FBO
    GLES.glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment, pname, params); // 查询参数
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO); // 恢复绑定
}

void glGetNamedFramebufferParameteriv(GLuint framebuffer, GLenum pname, GLint* param) {

    LOG()

    if (!global_settings.ext_dsa) {
        return;
    }

    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO); // 保存当前绑定的 FBO
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);  // 绑定目标 FBO
    GLES.glGetFramebufferParameteriv(GL_FRAMEBUFFER, pname, param); // 查询参数
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO); // 恢复绑定
}

GLenum glCheckNamedFramebufferStatus(GLuint framebuffer, GLenum target) {

    LOG()

    if (!global_settings.ext_dsa) {
        return;
    }

    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO); // 保存当前绑定的 FBO
    glBindFramebuffer(target, framebuffer);         // 绑定目标 FBO
    GLenum status = glCheckFramebufferStatus(target); // 检查状态
    glBindFramebuffer(target, (GLuint)prevFBO);      // 恢复绑定
    return status;
}

void glClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value) {

    LOG()

    if (!global_settings.ext_dsa) {
        return;
    }

    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    GLES.glClearBufferfv(buffer, drawbuffer, value);
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO);
}

void glClearNamedFramebufferiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value) {

    LOG()

    if (!global_settings.ext_dsa) {
        return;
    }

    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    GLES.glClearBufferiv(buffer, drawbuffer, value);
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO);
}

void glClearNamedFramebufferuiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value) {

    LOG()

    if (!global_settings.ext_dsa) {
        return;
    }

    GLint prevFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    GLES.glClearBufferuiv(buffer, drawbuffer, value);
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO);
} //Depk

void glInvalidateNamedFramebufferData(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments) {

    LOG()

    if (!global_settings.ext_dsa) {
        return;
    }

    // Save the currently bound framebuffer
    GLint prevFramebuffer;
    GLES.glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFramebuffer);
    
    // Bind our target framebuffer
    GLES.glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    // Use the core GLES function to invalidate
    GLES.glInvalidateFramebuffer(GL_FRAMEBUFFER, numAttachments, attachments);
    
    // Restore the previous framebuffer binding
    GLES.glBindFramebuffer(GL_FRAMEBUFFER, prevFramebuffer);
} //dick
