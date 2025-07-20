#include "texture.h"
#include "../texture.h"
#include <cstring>
#include <vector>
#include <cstdlib>

#ifndef __APPLE__
#include <malloc.h>
#include <android/log.h>
#endif

#include <ankerl/unordered_dense.h>

#include <GL/gl.h>
#include "../../gles/gles.h"
#include "../log.h"
#include "../../gles/loader.h"
#include "../../includes.h"
#include "../glsl/glsl_for_es.h"
#include "../mg.h"
#include "../framebuffer.h"
#include "../pixel.h"

#define DEBUG 1

extern ankerl::unordered_dense::map<GLuint, texture_t> g_textures;
extern GLuint bound_texture;
static int is_depth_format(GLenum format) {
    switch(format) {
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32F:
            return 1;
        default:
            return 0;
    }
}
static GLenum get_binding_for_target(GLenum target) {
    switch(target) {
        case GL_TEXTURE_2D: return GL_TEXTURE_BINDING_2D;
        case GL_TEXTURE_2D_MULTISAMPLE: return GL_TEXTURE_BINDING_2D_MULTISAMPLE;
        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        case GL_TEXTURE_2D_ARRAY: return GL_TEXTURE_BINDING_2D_ARRAY;
        case GL_TEXTURE_CUBE_MAP_ARRAY: return GL_TEXTURE_BINDING_CUBE_MAP_ARRAY;
        case GL_TEXTURE_BUFFER: return GL_TEXTURE_BUFFER_BINDING;
        case GL_TEXTURE_CUBE_MAP:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            return GL_TEXTURE_BINDING_CUBE_MAP;
        default: return 0;
    }
}

void glCreateTextures(GLenum target, GLsizei n, GLuint *textures) {
    LOG()
    LOG_D("glCreateTextures, target = 0x%x, n = %d, textures = %p", target, n, textures)

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
    LOG_D("glTextureParameteri, texture = %u, pname = 0x%x, param = %d", texture, pname, param)

    GLint prevTexture;
    GLES.glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    
    GLES.glBindTexture(GL_TEXTURE_2D, texture);
    GLES.glTexParameteri(GL_TEXTURE_2D, pname, param);
    GLES.glBindTexture(GL_TEXTURE_2D, prevTexture);
    
    CHECK_GL_ERROR
}

void glTextureParameterIiv(GLuint texture, GLenum pname, const GLint *params) {
    LOG()
    LOG_D("glTextureParameterIiv, texture = %u, pname = 0x%x, params = %p", texture, pname, params)

    GLint prevTexture;
    GLES.glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    
    GLES.glBindTexture(GL_TEXTURE_2D, texture);
    GLES.glTexParameterIiv(GL_TEXTURE_2D, pname, params);
    GLES.glBindTexture(GL_TEXTURE_2D, prevTexture);
    
    CHECK_GL_ERROR
}

void glTextureParameterIuiv(GLuint texture, GLenum pname, const GLuint *params) {
    LOG()
    LOG_D("glTextureParameterIuiv, texture = %u, pname = 0x%x, params = %p", texture, pname, params)

    GLint prevTexture;
    GLES.glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    
    GLES.glBindTexture(GL_TEXTURE_2D, texture);
    GLES.glTexParameterIuiv(GL_TEXTURE_2D, pname, params);
    GLES.glBindTexture(GL_TEXTURE_2D, prevTexture);
    
    CHECK_GL_ERROR
}

void glBindTextureUnit(GLuint unit, GLuint texture) {
    LOG()
    LOG_D("glBindTextureUnit, unit: %u, texture: %u", unit, texture)
    
    // 激活纹理单元
    GLES.glActiveTexture(GL_TEXTURE0 + unit);
    
    // 处理纹理绑定
    if (texture != 0) {
        GLenum target = GL_TEXTURE_2D; // 默认目标
        
        // 检查已知纹理类型
        auto it = g_textures.find(texture);
        if (it != g_textures.end()) {
            target = it->second.target;
        } else {
            // 未知纹理自动注册为2D纹理
            g_textures[texture] = {GL_TEXTURE_2D, texture, 0, {0}};
        }
        
        glBindTexture(target, texture);
    } else {
        // 解绑纹理
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    bound_texture = texture; // 更新绑定状态
    CHECK_GL_ERROR
} //DeepSeek

void glTextureParameteriv(GLuint texture, GLenum pname, const GLint *param) {

    LOG()
    LOG_D("glTextureParameteriv, tex: %u, pname: %s, param: %d", 
          texture, glEnumToString(pname), param ? *param : 0)

    // 获取纹理目标类型（默认为2D纹理）
    GLenum target = GL_TEXTURE_2D;
    auto texIt = g_textures.find(texture);
    if (texIt != g_textures.end()) {
        target = texIt->second.target;
    }

    // 保存并绑定纹理
    GLint prevTex;
    GLES.glGetIntegerv(get_binding_for_target(target), &prevTex);
    GLES.glBindTexture(target, texture);

    // 设置参数
    if (pname == GL_TEXTURE_SWIZZLE_RGBA && param) {
        GLES.glTexParameteriv(target, pname, param);
        memcpy(g_textures[texture].swizzle_param, param, 4*sizeof(GLint));
    } else {
        GLES.glTexParameteriv(target, pname, param);
    }

    // 恢复纹理绑定
    GLES.glBindTexture(target, prevTex);
    CHECK_GL_ERROR;
}


void glCopyTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, 
                            GLint yoffset, GLint x, GLint y, 
                            GLsizei width, GLsizei height) {
    LOG()
    LOG_D("glCopyTextureSubImage2D, tex: %d, level: %d, xoff: %d, yoff: %d", 
          texture, level, xoffset, yoffset)
    
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
          texture, level, zoffset)
    
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
          texture, level, xoffset, width)

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

static struct {
    GLint activeUnit = 0;  // 使用GLint避免类型转换
    GLuint boundSamplers[GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS] = {0};
} s_samplerState;
void glBindSamplers(GLuint first, GLsizei count, const GLuint* samplers) {

    LOG()
    LOG_D("glBindSamplers, first: %u, count: %d, samplers: %p", first, count, samplers)

    // 快速参数检查
    if (count < 0 || first + count > GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS) {
        LOG_E("ERROR: Invalid sampler binding range: first=%u count=%d", first, count)
        return;
    }

    // 批量绑定采样器
    for (GLsizei i = 0; i < count; ++i) {
        const GLuint unit = first + i;
        const GLuint sampler = samplers ? samplers[i] : 0;
        
        if (s_samplerState.boundSamplers[unit] != sampler) {
            if (s_samplerState.activeUnit != unit) {
                GLES.glActiveTexture(GL_TEXTURE0 + unit);
                s_samplerState.activeUnit = unit;
            }
            GLES.glBindSampler(unit, sampler);
            s_samplerState.boundSamplers[unit] = sampler;
        }
    }

    CHECK_GL_ERROR
} //DeepSeek*2

void glTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, 
                        GLsizei width, GLsizei height, GLenum format, 
                        GLenum type, const void *pixels) {

    LOG()
    LOG_D("glTextureSubImage2D, texture = %u, level = %d, xoffset = %d, yoffset = %d, width = %d, height = %d, format = 0x%x, type = 0x%x, pixels = %p", texture, level, xoffset, yoffset, width, height, format, type, pixels)
    
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
