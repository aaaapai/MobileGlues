//
// Created by Swung 0x48 on 2025/3/27.
//

#include "framebuffer.h"

#include "../log.h"
#include "../config/settings.h"
#include <ankerl/unordered_dense.h>

#define DEBUG 1

extern GLint MAX_DRAW_BUFFERS;
extern ankerl::unordered_dense::map<GLuint, GLenum> g_textureTargetMap;
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

    // 1. 验证参数合法性
    if (framebuffer == 0) {
        LOG_E("ERROR: Cannot modify default framebuffer (0) with glNamedFramebufferTexture");
        return;
    }

    // 2. 检查附件类型是否有效
    bool isColorAttachment = (attachment >= GL_COLOR_ATTACHMENT0 && 
                             attachment < GL_COLOR_ATTACHMENT0 + getMaxDrawBuffers());
    bool isDepthStencil = (attachment == GL_DEPTH_ATTACHMENT || 
                          attachment == GL_STENCIL_ATTACHMENT || 
                          attachment == GL_DEPTH_STENCIL_ATTACHMENT);

    if (!isColorAttachment && !isDepthStencil) {
        LOG_E("ERROR: Invalid attachment parameter: 0x%04X", attachment);
        return;
    }

    GLint prevFramebuffer;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevFramebuffer);

    // 3. 获取纹理目标类型（默认为GL_TEXTURE_2D）
    GLenum target = GL_TEXTURE_2D;
    if (texture != 0 && g_textureTargetMap.find(texture) != g_textureTargetMap.end()) {
        auto it = g_textureTargetMap.find(texture);
        if (it != g_textureTargetMap.end()) {
            target = it->second;
        } else {
            LOG_W("WARNING: Texture %u target unknown, defaulting to GL_TEXTURE_2D", texture);
        }
    }

    // 4. 绑定帧缓冲区并附加纹理
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

     // 修改：正确处理分层纹理
    if (isLayeredTarget(target)) {
        // 分层纹理 - 使用 glFramebufferTexture
        glFramebufferTexture(GL_DRAW_FRAMEBUFFER, attachment, texture, level);
        LOG_D("Attached layered texture (target=0x%04X)", target);
    } else {
        // 非分层纹理
        // 标准2D纹理或立方体贴图面
        GLES.glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, target, texture, level);
    }

    // 5. 更新内部帧缓冲区状态（示例结构体）
    struct FramebufferState {
        std::vector<GLuint> colorAttachments;
        GLuint depthAttachment = 0;
        GLuint stencilAttachment = 0;
        std::vector<bool> isLayeredColor; // 记录颜色附件分层状态
        bool isLayeredDepth = false;
        bool isLayeredStencil = false;
    };
    static ankerl::unordered_dense::map<GLuint, FramebufferState> fbStates;

    FramebufferState& state = fbStates[framebuffer];
    if (isColorAttachment) {
        size_t index = attachment - GL_COLOR_ATTACHMENT0;
        if (state.colorAttachments.size() <= index) {
            state.colorAttachments.resize(index + 1, 0);
        }
        state.colorAttachments[index] = texture;
        // 记录分层状态
        if (state.isLayeredColor.size() <= index) {
            state.isLayeredColor.resize(index + 1, false);
        }
    } else if (attachment == GL_DEPTH_ATTACHMENT) {
        state.depthAttachment = texture;
        state.isLayeredDepth = isLayeredTarget(target);
    } else if (attachment == GL_STENCIL_ATTACHMENT) {
        state.stencilAttachment = texture;
        state.isLayeredStencil = isLayeredTarget(target);
    }

    // 新增：分层一致性检查
    bool hasLayeredAttachment = false;
    bool hasNonLayeredAttachment = false;
    
    for (bool layered : state.isLayeredColor) {
        if (layered) hasLayeredAttachment = true;
        else if (!layered && state.colorAttachments[i] != 0) 
            hasNonLayeredAttachment = true;
    }
    
    if (state.depthAttachment != 0) {
        if (state.isLayeredDepth) hasLayeredAttachment = true;
        else hasNonLayeredAttachment = true;
    }
    
    if (state.stencilAttachment != 0) {
        if (state.isLayeredStencil) hasLayeredAttachment = true;
        else hasNonLayeredAttachment = true;
    }
    
    // 混合使用分层和非分层附件会导致错误
    if (hasLayeredAttachment && hasNonLayeredAttachment) {
        LOG_E("ERROR: Mixed layered and non-layered attachments in FBO %u", framebuffer);
        
        // 记录详细附件信息
        for (size_t i = 0; i < state.colorAttachments.size(); ++i) {
            if (state.colorAttachments[i] != 0) {
                LOG_E("  Color attachment %d: texture %u, layered: %s", 
                      i, state.colorAttachments[i], state.isLayeredColor[i] ? "yes" : "no");
            }
        }
    }

    // 6. 检查帧缓冲区完整性
    GLenum status = GLES.glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        const char* errorMsg = "Unknown";
        switch (status) {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: errorMsg = "INCOMPLETE_ATTACHMENT"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: errorMsg = "MISSING_ATTACHMENT"; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: errorMsg = "UNSUPPORTED_FORMAT"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: errorMsg = "INCOMPLETE_LAYER_TARGETS"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: errorMsg = "INCONSISTENT_DIMENSIONS"; break;
        }
        LOG_E("Framebuffer %u incomplete: %s (0x%04X)", framebuffer, errorMsg, status);
    }

    // 7. 恢复原始绑定
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, (GLuint)prevFramebuffer);
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

