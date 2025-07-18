#include "texture.h"
#include "../texture.h"
#include <cstring>
#include <vector>
#include <cstdlib>

#ifndef __APPLE__
#include <malloc.h>
#include <android/log.h>
#endif

#include "ankerl/unordered_dense.h"

#include "GL/gl.h"
#include "../gles/gles.h"
#include "log.h"
#include "../gles/loader.h"
#include "../includes.h"
#include "glsl/glsl_for_es.h"
#include "mg.h"
#include "framebuffer.h"
#include "pixel.h"

#define DEBUG 0

void glCreateTextures(GLenum target, GLsizei n, GLuint *textures) {
    LOG()
    
    GLES.glGenTextures(n, textures);
    
    for (GLsizei i = 0; i < n; i++) {
        GLES.glBindTexture(target, textures[i]);
        // Set default texture parameters
        GLES.glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLES.glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLES.glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GLES.glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    
    CHECK_GL_ERROR
}

void glTextureParameteri(GLuint texture, GLenum pname, GLint param) {
    LOG()
    
    GLint prevTexture;
    GLES.glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    
    GLES.glBindTexture(GL_TEXTURE_2D, texture);
    GLES.glTexParameteri(GL_TEXTURE_2D, pname, param);
    GLES.glBindTexture(GL_TEXTURE_2D, prevTexture);
    
    CHECK_GL_ERROR
}

void glTextureParameterIiv(GLuint texture, GLenum pname, const GLint *params) {
    LOG()
    
    GLint prevTexture;
    GLES.glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    
    GLES.glBindTexture(GL_TEXTURE_2D, texture);
    GLES.glTexParameterIiv(GL_TEXTURE_2D, pname, params);
    GLES.glBindTexture(GL_TEXTURE_2D, prevTexture);
    
    CHECK_GL_ERROR
}

void glTextureParameterIuiv(GLuint texture, GLenum pname, const GLuint *params) {
    LOG()
    
    GLint prevTexture;
    GLES.glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    
    GLES.glBindTexture(GL_TEXTURE_2D, texture);
    GLES.glTexParameterIuiv(GL_TEXTURE_2D, pname, params);
    GLES.glBindTexture(GL_TEXTURE_2D, prevTexture);
    
    CHECK_GL_ERROR
}

void glBindTextureUnit(GLuint unit, GLuint texture) {
    LOG()
    LOG_D("glBindTextureUnit, unit: %d, texture: %d", unit, texture)
    INIT_CHECK_GL_ERROR
    
    // First bind the texture to the specified texture unit
    GLES.glActiveTexture(GL_TEXTURE0 + unit);
    CHECK_GL_ERROR_NO_INIT
    
    // Then bind the texture to the currently bound target
    if (texture != 0) {
        auto it = g_textures.find(texture);
        if (it != g_textures.end()) {
            GLES.glBindTexture(it->second.target, texture);
            bound_texture = texture;
        } else {
            // If texture not found in our tracking, bind to GL_TEXTURE_2D by default
            GLES.glBindTexture(GL_TEXTURE_2D, texture);
            bound_texture = texture;
            // Add to our texture tracking
            g_textures[texture] = {
                .target = GL_TEXTURE_2D,
                .texture = texture,
                .format = 0,
                .swizzle_param = {0}
            };
        }
    } else {
        // If texture is 0, unbind current texture
        GLES.glBindTexture(GL_TEXTURE_2D, 0);
        bound_texture = 0;
    }
    
    CHECK_GL_ERROR_NO_INIT
}

void glTextureParameteriv(GLuint texture, GLenum pname, const GLint *param) {
    LOG()
    LOG_D("glTextureParameteriv, texture: %d, pname: %s, param: %d", 
          texture, glEnumToString(pname), param ? *param : 0)
    
    GLint prevTexture;
    GLenum target = GL_TEXTURE_2D; // 默认目标
    
    // 从纹理跟踪器中获取实际目标类型
    auto it = g_textures.find(texture);
    if (it != g_textures.end()) {
        target = it->second.target;
    }
    
    glGetIntegerv(get_binding_for_target(target), &prevTexture);
    glBindTexture(target, texture);
    
    if (pname == GL_TEXTURE_SWIZZLE_RGBA) {
        // 特殊处理swizzle参数
        if (param) {
            glTexParameteriv(target, GL_TEXTURE_SWIZZLE_R, &param[0]);
            glTexParameteriv(target, GL_TEXTURE_SWIZZLE_G, &param[1]);
            glTexParameteriv(target, GL_TEXTURE_SWIZZLE_B, &param[2]);
            glTexParameteriv(target, GL_TEXTURE_SWIZZLE_A, &param[3]);
            
            // 更新纹理状态
            g_textures[texture].swizzle_param[0] = param[0];
            g_textures[texture].swizzle_param[1] = param[1];
            g_textures[texture].swizzle_param[2] = param[2];
            g_textures[texture].swizzle_param[3] = param[3];
        }
    } else {
        glTexParameteriv(target, pname, param);
    }
    
    glBindTexture(target, prevTexture);
    CHECK_GL_ERROR
} //DeepSeek

void glCopyTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, 
                            GLint yoffset, GLint x, GLint y, 
                            GLsizei width, GLsizei height) {
    LOG()
    LOG_D("glCopyTextureSubImage2D, tex: %d, level: %d, xoff: %d, yoff: %d", 
          texture, level, xoffset, yoffset);
    
    GLint prevTexture;
    GLenum target = GL_TEXTURE_2D;
    
    // 获取纹理实际目标类型
    auto it = g_textures.find(texture);
    if (it != g_textures.end()) {
        target = it->second.target;
    }
    
    glGetIntegerv(get_binding_for_target(target), &prevTexture);
    glBindTexture(target, texture);
    
    // 直接使用GLES的拷贝函数
    GLES.glCopyTexSubImage2D(target, level, xoffset, yoffset, 
                            x, y, width, height);
    
    glBindTexture(target, prevTexture);
    CHECK_GL_ERROR
} //DeepSeek

void glCopyTextureSubImage3D(GLuint texture, GLint level, GLint xoffset,
                            GLint yoffset, GLint zoffset, GLint x, GLint y,
                            GLsizei width, GLsizei height) {
    LOG()
    LOG_D("glCopyTextureSubImage3D, tex: %d, level: %d, zoff: %d",
          texture, level, zoffset);
    
    GLint prevTexture;
    GLES.glGetIntegerv(GL_TEXTURE_BINDING_3D, &prevTexture);
    GLES.glBindTexture(GL_TEXTURE_3D, texture);
    
    // 使用GLES扩展函数
    GLES.glCopyTexSubImage3D(GL_TEXTURE_3D, level, xoffset, yoffset, zoffset,
                            x, y, width, height);
    
    GLES.glBindTexture(GL_TEXTURE_3D, prevTexture);
    CHECK_GL_ERROR
} //DeepSeek

void glCopyTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, 
                            GLint x, GLint y, GLsizei width) {
    LOG()
    LOG_D("glCopyTextureSubImage1D, tex: %d, level: %d, xoff: %d, width: %d",
          texture, level, xoffset, width);

    GLint prevTexture;
    GLenum target = GL_TEXTURE_1D;
    
    auto it = g_textures.find(texture);
    if (it != g_textures.end()) {
        target = it->second.target;
    }

    GLES.glGetIntegerv(get_binding_for_target(target), &prevTexture);
    GLES.glBindTexture(target, texture);

    GLES.glCopyTexSubImage2D(GL_TEXTURE_2D, level, xoffset, 0, x, y, width, 1);

    GLES.glBindTexture(target, prevTexture);
    CHECK_GL_ERROR
}

// 全局状态缓存
static struct {
    GLuint activeUnit = 0;
    GLuint boundSamplers[32] = {0}; // 假设最大32个纹理单元
} s_samplerState;

void glBindSamplers(GLuint first, GLsizei count, const GLuint* samplers) {
    // 参数检查
    if (count < 0) {
        LOG_E("Invalid count: %d", count);
        return;
    }

    // 保存当前活跃纹理单元
    GLint prevActiveUnit;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &prevActiveUnit);
    prevActiveUnit -= GL_TEXTURE0; // 转换为索引值

    // 绑定采样器
    for (GLsizei i = 0; i < count; ++i) {
        const GLuint unit = first + i;
        const GLuint sampler = samplers ? samplers[i] : 0;

        // 只有状态变化时才执行绑定
        if (s_samplerState.boundSamplers[unit] != sampler) {
            if (s_samplerState.activeUnit != unit) {
                GLES.glActiveTexture(GL_TEXTURE0 + unit);
                s_samplerState.activeUnit = unit;
            }
            
            GLES.glBindSampler(unit, sampler);
            s_samplerState.boundSamplers[unit] = sampler;
        }
    }

    // 恢复之前活跃的纹理单元
    if (s_samplerState.activeUnit != prevActiveUnit) {
        GLES.glActiveTexture(GL_TEXTURE0 + prevActiveUnit);
        s_samplerState.activeUnit = prevActiveUnit;
    }

    CHECK_GL_ERROR;
} //DeepSeek

void glTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, 
                        GLsizei width, GLsizei height, GLenum format, 
                        GLenum type, const void *pixels) {
    // 保存当前绑定的纹理以便后续恢复
    GLint prevTexture;
    GLES.glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    
    // 绑定目标纹理
    GLES.glBindTexture(GL_TEXTURE_2D, texture);
    
    // 调用实际的纹理更新函数
    GLES.glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, 
                   width, height, format, type, pixels);
    
    // 恢复之前绑定的纹理
    GLES.glBindTexture(GL_TEXTURE_2D, (GLuint)prevTexture);
}
