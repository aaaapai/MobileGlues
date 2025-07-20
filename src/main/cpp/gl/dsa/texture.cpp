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

    // 保存当前绑定状态
    GLint prevBinding = 0;
    GLenum target = GL_TEXTURE_2D;
    
    // 尝试检测纹理类型
    GLenum targets[] = {
        GL_TEXTURE_2D,
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_2D_ARRAY,
        GL_TEXTURE_3D
    };
    
    for (size_t i = 0; i < sizeof(targets)/sizeof(targets[0]); i++) {
        GLint bindingParam;
        switch (targets[i]) {
            case GL_TEXTURE_2D:
                bindingParam = GL_TEXTURE_BINDING_2D;
                break;
            case GL_TEXTURE_CUBE_MAP:
                bindingParam = GL_TEXTURE_BINDING_CUBE_MAP;
                break;
            case GL_TEXTURE_2D_ARRAY:
                bindingParam = GL_TEXTURE_BINDING_2D_ARRAY;
                break;
            case GL_TEXTURE_3D:
                bindingParam = GL_TEXTURE_BINDING_3D;
                break;
            default:
                continue;
        }
        
        GLES.glGetIntegerv(bindingParam, &prevBinding);
        
        // 尝试绑定纹理
        GLES.glBindTexture(targets[i], texture);
        
        GLint newBinding;
        GLES.glGetIntegerv(bindingParam, &newBinding);
        
        // 检查绑定是否成功
        if (static_cast<GLuint>(newBinding) == texture) {
            target = targets[i];
            break;
        }
        
        // 恢复原始绑定
        GLES.glBindTexture(targets[i], static_cast<GLuint>(prevBinding));
    }

    // 设置纹理参数
    GLES.glTexParameteri(target, pname, param);
    
    // 恢复原始绑定状态
    GLES.glBindTexture(target, static_cast<GLuint>(prevBinding));
    
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


GLenum GetTextureTarget(GLuint texture) {

    // 尝试查询纹理绑定的目标类型
    GLint currentBinding = 0;
    GLenum possibleTargets[] = {
        GL_TEXTURE_2D,
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_2D_ARRAY,
        GL_TEXTURE_3D
    };

    for (GLenum target : possibleTargets) {
        GLint bindingParam;
        switch (target) {
            case GL_TEXTURE_2D:
                bindingParam = GL_TEXTURE_BINDING_2D;
                break;
            case GL_TEXTURE_CUBE_MAP:
                bindingParam = GL_TEXTURE_BINDING_CUBE_MAP;
                break;
            case GL_TEXTURE_2D_ARRAY:
                bindingParam = GL_TEXTURE_BINDING_2D_ARRAY;
                break;
            case GL_TEXTURE_3D:
                bindingParam = GL_TEXTURE_BINDING_3D;
                break;
            default:
                continue;
        }

        GLES.glGetIntegerv(bindingParam, &currentBinding);
        if (static_cast<GLuint>(currentBinding) == texture) {
            return target;
        }
    }

    return GL_TEXTURE_2D; // 默认回退到 GL_TEXTURE_2D
}
void glBindTextureUnit(GLuint unit, GLuint texture) {

    LOG()
    LOG_D("glBindTextureUnit, unit: %u, texture: %u", unit, texture)

    if (texture != 0) {
        GLenum target = GetTextureTarget(texture);
        GLES.glActiveTexture(GL_TEXTURE0 + unit);
        GLES.glBindTexture(target, texture);
    } else {
        // 解绑当前单元的所有可能目标
        GLES.glActiveTexture(GL_TEXTURE0 + unit);
        GLES.glBindTexture(GL_TEXTURE_2D, 0);
        GLES.glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        GLES.glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        GLES.glBindTexture(GL_TEXTURE_3D, 0);
    }
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

void glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, 
                        GLsizei width, GLsizei height) {

    LOG()
    
    // 保存当前绑定的纹理
    GLint prevTexture;
    GLES.glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    
    // 绑定指定纹理
    GLES.glBindTexture(GL_TEXTURE_2D, texture);
    
    // 创建不可变存储
    GLES.glTexStorage2D(GL_TEXTURE_2D, levels, internalformat, width, height);
    
    // 恢复之前绑定的纹理
    GLES.glBindTexture(GL_TEXTURE_2D, (GLuint)prevTexture);
}

void glTextureStorage3D(GLuint texture, GLsizei levels, GLenum internalformat, 
                        GLsizei width, GLsizei height, GLsizei depth) {

    LOG()
    // 保存当前绑定的纹理
    GLint prevTexture;
    GLES.glGetIntegerv(GL_TEXTURE_BINDING_3D, &prevTexture);
    
    // 绑定指定纹理
    GLRS.glBindTexture(GL_TEXTURE_3D, texture);
    
    // 创建不可变存储
    GLES.glTexStorage3D(GL_TEXTURE_3D, levels, internalformat, width, height, depth);
    
    // 恢复之前绑定的纹理
    GLRS.glBindTexture(GL_TEXTURE_3D, (GLuint)prevTexture);
}

void glTextureStorage2DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, 
                                  GLsizei width, GLsizei height, GLboolean fixedsamplelocations) {

    LOG()
    // 绑定纹理目标为GL_TEXTURE_2D_MULTISAMPLE
    GLES.glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
    
    // 使用GLES的TexStorage2DMultisample函数
    GLES.glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalformat, 
                                  width, height, fixedsamplelocations);
    
    // 解绑纹理
    GLES.glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}

void glTextureStorage3DMultisample(
    GLuint texture, GLsizei samples, GLenum internalformat,
    GLsizei width, GLsizei height, GLsizei depth,
    GLboolean fixedsamplelocations) 
{

    LOG()
    
    // 使用 2D 纹理数组模拟 3D 纹理，每个 slice 存储一个样本
    GLES.glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    
    // 分配存储：depth * samples 个 2D 纹理切片
    GLES.glTexStorage3D(
        GL_TEXTURE_2D_ARRAY,
        1,                  // 1 mipmap level
        internalformat,
        width,
        height,
        depth * samples     // 每个 depth 层有 samples 个样本
    );
    
    // 设置纹理参数（根据需要调整）
    GLES.glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GLES.glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLES.glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GLES.glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    GLES.glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

// 实现 glTextureSubImage3D
void glTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, 
                        GLsizei width, GLsizei height, GLsizei depth, 
                        GLenum format, GLenum type, const void *pixels) {

    LOG()
    // 保存当前错误状态
    GLenum prevError = GLES.GetError();
    
    // 绑定纹理
    GLES.BindTexture(GL_TEXTURE_3D, texture);
    
    // 调用 GLES 等效函数
    GLES.TexSubImage3D(GL_TEXTURE_3D, level, xoffset, yoffset, zoffset, 
                      width, height, depth, format, type, pixels);
    
    // 检查错误
    GLenum error = GLES.GetError();
    if (error != GL_NO_ERROR && prevError == GL_NO_ERROR) {
        // 可以在这里记录或处理错误
    }
}

// 实现 glCompressedTextureSubImage3D
void glCompressedTextureSubImage3D(GLuint texture, GLint level, 
                                  GLint xoffset, GLint yoffset, GLint zoffset,
                                  GLsizei width, GLsizei height, GLsizei depth,
                                  GLenum format, GLsizei imageSize, const void *data) {

    LOG()
    GLenum prevError = GLES.GetError();
    
    GLES.BindTexture(GL_TEXTURE_3D, texture);
    GLES.CompressedTexSubImage3D(GL_TEXTURE_3D, level, xoffset, yoffset, zoffset,
                                width, height, depth, format, imageSize, data);
    
    GLenum error = GLES.GetError();
    if (error != GL_NO_ERROR && prevError == GL_NO_ERROR) {
        // 错误处理
    }
}

// 实现 glCompressedTextureSubImage2D
void glCompressedTextureSubImage2D(GLuint texture, GLint level,
                                  GLint xoffset, GLint yoffset,
                                  GLsizei width, GLsizei height,
                                  GLenum format, GLsizei imageSize, const void *data) {

    LOG()
    GLenum prevError = GLES.GetError();
    
    GLES.BindTexture(GL_TEXTURE_2D, texture);
    GLES.CompressedTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset,
                                width, height, format, imageSize, data);
    
    GLenum error = GLES.GetError();
    if (error != GL_NO_ERROR && prevError == GL_NO_ERROR) {
        // 错误处理
    }
}

// 实现 glCompressedTextureSubImage1D
void glCompressedTextureSubImage1D(GLuint texture, GLint level,
                                  GLint xoffset, GLsizei width,
                                  GLenum format, GLsizei imageSize, const void *data) {

    LOG()
    GLenum prevError = GLES.GetError();
    
    // 注意: OpenGL ES 不直接支持 1D 纹理，通常使用 2D 纹理模拟
    GLES.BindTexture(GL_TEXTURE_2D, texture);
    
    // 将 1D 上传转换为 2D 上传，高度为 1
    GLES.CompressedTexSubImage2D(GL_TEXTURE_2D, level, xoffset, 0,
                                width, 1, format, imageSize, data);
    
    GLenum error = GLES.GetError();
    if (error != GL_NO_ERROR && prevError == GL_NO_ERROR) {
        // 错误处理
    }
}
