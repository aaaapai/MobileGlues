// MobileGlues - gl/shader.cpp
// Copyright (c) 2025-2026 MobileGL-Dev
// Licensed under the GNU Lesser General Public License v2.1:
//   https://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt
// SPDX-License-Identifier: LGPL-2.1-only
// End of Source File Header

#include <cctype>
#include "shader.h"

#include <GL/gl.h>
#include "log.h"
#include "program.h"
#include "../gles/loader.h"
#include "../includes.h"
#include "glsl/glsl_for_es.h"
#include "../config/settings.h"
#include "FSR1/FSR1.h"

#define DEBUG 0

struct shader_t shaderInfo;

UnorderedMap<GLuint, bool> shader_map_is_sampler_buffer_emulated;

bool can_run_essl3(unsigned int esversion, const char* glsl) {
    if (strncmp(glsl, "#version 100", 12) == 0) {
        return true;
    }

    unsigned int glsl_version = 0;
    if (strncmp(glsl, "#version 300 es", 15) == 0) {
        glsl_version = 300;
    } else if (strncmp(glsl, "#version 310 es", 15) == 0) {
        glsl_version = 310;
    } else if (strncmp(glsl, "#version 320 es", 15) == 0) {
        glsl_version = 320;
    } else {
        return false;
    }
    return esversion >= glsl_version;
}

bool is_direct_shader(const char* glsl) {
    bool es3_ability = can_run_essl3(hardware->es_version, glsl);
    return es3_ability;
}

bool check_if_sampler_buffer_used(std::string str) {
    return str.find("samplerBuffer") != std::string::npos;
}

void glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length) {
    LOG()

    // 合并所有输入字符串为一个完整的源码
    size_t total_len = 0;
    for (int i = 0; i < count; i++) {
        total_len += (length && length[i] >= 0) ? length[i] : strlen(string[i]);
    }
    std::string glsl_src;
    glsl_src.reserve(total_len + 1);
    if (length) {
        for (int i = 0; i < count; i++) {
            if (length[i] >= 0)
                glsl_src.append(string[i], length[i]);
            else
                glsl_src += string[i];
        }
    } else {
        for (int i = 0; i < count; i++) {
            glsl_src += string[i];
        }
    }

    // 检查是否需要对 texture buffer 进行模拟（基于原始源码）
    bool is_sampler_buffer_emulated = hardware->emulate_texture_buffer &&
                                      check_if_sampler_buffer_used(glsl_src);

    std::string final_src;
    bool conversion_success = false;

    if (is_direct_shader(glsl_src.c_str())) {
        LOG_D("[INFO] [Shader] Direct shader source: ")
        LOG_D("%s", glsl_src.c_str())
        final_src = glsl_src;
        conversion_success = true;  // 直接使用，视为成功
    } else {
        LOG_D("[INFO] [Shader] Shader source: ")
        LOG_D("%s", glsl_src.c_str())

        GLint shaderType;
        GLES.glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);
        int return_code = 0;
        std::string essl_src = GLSLtoGLSLES(glsl_src.c_str(), shaderType,
                                            hardware->es_version, glsl_version, return_code);

        if (!essl_src.empty()) {
            LOG_D("\n[INFO] [Shader] Converted Shader source: \n%s", essl_src.c_str())
            final_src = essl_src;
            conversion_success = true;
        } else {
            LOG_E("Failed to convert glsl, falling back to original source.")
            final_src = glsl_src;   // 转换失败，使用原始源码
            conversion_success = false;
        }
    }

    // 记录 shader 信息（无论成功与否）
    shaderInfo.id = shader;
    shaderInfo.converted = final_src;
    shaderInfo.frag_data_changed_converted.clear();
    shaderInfo.frag_data_changed = 0;

    // 总是将最终源码传递给驱动（此时合并为一个字符串）
    const char* src_ptr = final_src.c_str();
    GLES.glShaderSource(shader, 1, &src_ptr, nullptr);   // 修复：count 固定为 1

    // 若开启了 texture buffer 模拟，记录该 shader 是否需要模拟
    if (hardware->emulate_texture_buffer) {
        shader_map_is_sampler_buffer_emulated[shader] = is_sampler_buffer_emulated;
    }

    CHECK_GL_ERROR
}

void glGetShaderiv(GLuint shader, GLenum pname, GLint* params) {
    LOG()
    GLES.glGetShaderiv(shader, pname, params);
    if (global_settings.ignore_error >= IgnoreErrorLevel::Partial && pname == GL_COMPILE_STATUS && !*params) {
        GLchar infoLog[512];
        GLES.glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        LOG_W_FORCE("Shader %d compilation failed: \n%s", shader, infoLog)
        LOG_W_FORCE("Now try to cheat.")
        *params = GL_TRUE;
    }
    CHECK_GL_ERROR
}

GLuint glCreateShader(GLenum shaderType) {
    if (global_settings.fsr1_setting != FSR1_Quality_Preset::Disabled && !fsrInitialized) {
        InitFSRResources();
    }

    LOG()
    LOG_D("glCreateShader(%s)", glEnumToString(shaderType))
    GLuint shader = GLES.glCreateShader(shaderType);
    if (shader != 0 && hardware->emulate_texture_buffer) shader_map_is_sampler_buffer_emulated[shader] = false;
    CHECK_GL_ERROR
    return shader;
}
