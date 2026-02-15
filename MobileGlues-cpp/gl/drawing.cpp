// MobileGlues - gl/drawing.cpp
// Copyright (c) 2025-2026 MobileGL-Dev
// Licensed under the GNU Lesser General Public License v2.1:
//   https://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt
// SPDX-License-Identifier: LGPL-2.1-only
// End of Source File Header

#include "drawing.h"
#include "buffer.h"
#include "framebuffer.h"
#include "mg.h"
#include "texture.h"
#include <ankerl/unordered_dense.h>

#define DEBUG 0

GLuint bufSampelerProg;
GLuint bufSampelerLoc;
std::string bufSampelerName;

extern UnorderedMap<GLuint, bool> program_map_is_sampler_buffer_emulated;
extern UnorderedMap<GLuint, bool> program_map_is_atomic_counter_emulated;

UnorderedMap<GLuint, SamplerInfo> g_samplerCacheForSamplerBuffer;

void setupBufferTextureUniforms(GLuint program) {
    LOG_D("setupBufferTextureUniforms, program: %d", program);

    if (!program_map_is_sampler_buffer_emulated[program]) return;

    if (g_samplerCacheForSamplerBuffer.find(program) == g_samplerCacheForSamplerBuffer.end()) {
        auto& progSamplerInfo = g_samplerCacheForSamplerBuffer[program];
        GLint locWidth = GLES.glGetUniformLocation(program, "u_BufferTexWidth");
        GLint locHeight = GLES.glGetUniformLocation(program, "u_BufferTexHeight");
        if (locWidth == -1) {
            LOG_W("u_BufferTexWidth uniform not found in program %d", program);
            return;
        }

        progSamplerInfo.locHeight = locHeight;
        progSamplerInfo.locWidth = locWidth;
        progSamplerInfo.samplers.clear();

        GLint numUniforms = 0;
        GLES.glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);
        LOG_D("Program %d has %d active uniforms", program, numUniforms);

        for (GLint i = 0; i < numUniforms; ++i) {
            const GLsizei bufSize = 256;
            GLchar name[bufSize];
            GLsizei length = 0;
            GLint size = 0;
            GLenum type = 0;
            GLES.glGetActiveUniform(program, i, bufSize, &length, &size, &type, name);

            if (type == GL_SAMPLER_2D || type == GL_INT_SAMPLER_2D) {
                GLint locSampler = GLES.glGetUniformLocation(program, name);
                progSamplerInfo.samplers.push_back(locSampler);
            }
        }
    }

    auto& progSamplerInfo = g_samplerCacheForSamplerBuffer[program];

    GLint locWidth = progSamplerInfo.locWidth;
    GLint locHeight = progSamplerInfo.locHeight;

    for (auto locSampler : progSamplerInfo.samplers) {
        if (locSampler < 0) {
            continue;
        }

        GLuint prev_unit = gl_state->current_tex_unit;
        const GLint unit = 15;

        GLES.glActiveTexture(GL_TEXTURE0 + unit);
        GLint texId = 0;
        GLES.glGetIntegerv(GL_TEXTURE_BINDING_2D, &texId);
        if (texId == 0) {
            GLES.glActiveTexture(GL_TEXTURE0 + prev_unit);
            continue;
        }

        auto texObject = mgGetTexObjectByID(texId);

        GLES.glUniform1i(locSampler, unit);
        GLES.glUniform1i(locWidth, texObject->width);
        GLES.glUniform1i(locHeight, texObject->height);

        GLES.glActiveTexture(GL_TEXTURE0 + prev_unit);
    }
}

void prepareForDraw() {
    LOG_D("prepareForDraw...")
    if (hardware->emulate_texture_buffer) {
        setupBufferTextureUniforms(gl_state->current_program);
    }
}

void glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount) {
    LOG()
    LOG_D("glDrawElementsInstanced, mode: %d, count: %d, type: %d, indices: %p, primcount: %d", mode, count, type,
          indices, primcount)
    prepareForDraw();
    GLES.glDrawElementsInstanced(mode, count, type, indices, primcount);
    CHECK_GL_ERROR
}

void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) {
    LOG()
    LOG_D("glDrawElements, mode: %d, count: %d, type: %d, indices: %p", mode, count, type, indices)
    prepareForDraw();
    GLES.glDrawElements(mode, count, type, indices);
    CHECK_GL_ERROR
}

void glBindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access,
                        GLenum format) {
    LOG()
    LOG_D("glBindImageTexture, unit: %d, texture: %d, level: %d, layered: %d, layer: %d, access: %d, format: %d", unit,
          texture, level, layered, layer, access, format)
    GLES.glBindImageTexture(unit, texture, level, layered, layer, access, format);
    CHECK_GL_ERROR
}

void glUniform1i(GLint location, GLint v0) {
    LOG()
    LOG_D("glUniform1i, location: %d, v0: %d", location, v0)
    GLES.glUniform1i(location, v0);
    CHECK_GL_ERROR
}

void bindAllAtomicCounterAsSSBO();
void glDispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) {
    LOG()
    LOG_D("glDispatchCompute, num_groups_x: %d, num_groups_y: %d, num_groups_z: %d", num_groups_x, num_groups_y,
          num_groups_z)
    if (program_map_is_atomic_counter_emulated[gl_state->current_program]) {
        bindAllAtomicCounterAsSSBO();
        LOG_D("Atomic counters bound as SSBOs for program %d", gl_state->current_program);
    } else {
        LOG_D("No atomic counters bound as SSBOs for program %d", gl_state->current_program);
    }
    GLES.glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    CHECK_GL_ERROR
}

GLbitfield glGetSupportedMemoryBarrierBits() {
    GLbitfield supported = 0;
    supported |= GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT;
    supported |= GL_ELEMENT_ARRAY_BARRIER_BIT;
    supported |= GL_UNIFORM_BARRIER_BIT;
    supported |= GL_TEXTURE_FETCH_BARRIER_BIT;
    supported |= GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
    supported |= GL_COMMAND_BARRIER_BIT;
    supported |= GL_PIXEL_BUFFER_BARRIER_BIT;
    supported |= GL_TEXTURE_UPDATE_BARRIER_BIT;
    supported |= GL_BUFFER_UPDATE_BARRIER_BIT;
    supported |= GL_FRAMEBUFFER_BARRIER_BIT;
    supported |= GL_TRANSFORM_FEEDBACK_BARRIER_BIT;
    supported |= GL_ATOMIC_COUNTER_BARRIER_BIT;
    supported |= GL_SHADER_STORAGE_BARRIER_BIT;
    
    return supported;
}
/*void glMemoryBarrier(GLbitfield barriers) {
    LOG()
    LOG_D("glMemoryBarrier, barriers: %d", barriers)
    if (program_map_is_atomic_counter_emulated[gl_state->current_program]) {
        barriers |= GL_ATOMIC_COUNTER_BARRIER_BIT;
        barriers |= GL_SHADER_STORAGE_BARRIER_BIT;
    }
    GLES.glMemoryBarrier(barriers);
    CHECK_GL_ERROR
}*/
void glMemoryBarrier(GLbitfield barriers) {
    LOG()
    LOG_D("glMemoryBarrier, barriers: 0x%x", barriers)

    // OpenGL ES 3.2 支持的 barrier bits
    const GLbitfield ES_SUPPORTED_BARRIERS = 
        GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT |
        GL_ELEMENT_ARRAY_BARRIER_BIT |
        GL_UNIFORM_BARRIER_BIT |
        GL_TEXTURE_FETCH_BARRIER_BIT |
        GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
        GL_COMMAND_BARRIER_BIT |
        GL_PIXEL_BUFFER_BARRIER_BIT |
        GL_TEXTURE_UPDATE_BARRIER_BIT |
        GL_BUFFER_UPDATE_BARRIER_BIT |
        GL_FRAMEBUFFER_BARRIER_BIT |
        GL_TRANSFORM_FEEDBACK_BARRIER_BIT |
        GL_ATOMIC_COUNTER_BARRIER_BIT |
        GL_SHADER_STORAGE_BARRIER_BIT;

    // 需要过滤或转换的 barrier bits
    GLbitfield filtered_barriers = barriers & ES_SUPPORTED_BARRIERS;
    
    // 记录不支持的 bits
    GLbitfield unsupported = barriers & ~ES_SUPPORTED_BARRIERS;
    if (unsupported != 0) {
        LOG_W("glMemoryBarrier: Unsupported barriers in ES 3.2: 0x%x", unsupported);
        
        // 尝试转换一些常见的桌面 GL barrier 到 ES 等效项
        if (unsupported & GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT) {
            LOG_D("Converting GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT to GL_BUFFER_UPDATE_BARRIER_BIT");
            filtered_barriers |= GL_BUFFER_UPDATE_BARRIER_BIT;
        }
        
        // GL_QUERY_BUFFER_BARRIER_BIT 没有直接等效项，可能需要完全同步
        if (unsupported & GL_QUERY_BUFFER_BARRIER_BIT) {
            LOG_D("GL_QUERY_BUFFER_BARRIER_BIT not supported, forcing full barrier");
            // 在某些情况下，可能需要更保守的同步
            filtered_barriers |= (GL_TEXTURE_UPDATE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
        }
    }

    // 处理原子计数器模拟
    if (program_map_is_atomic_counter_emulated[gl_state->current_program]) {
        LOG_D("Atomic counter emulation active, adding ATOMIC_COUNTER and SHADER_STORAGE barriers");
        filtered_barriers |= GL_ATOMIC_COUNTER_BARRIER_BIT;
        filtered_barriers |= GL_SHADER_STORAGE_BARRIER_BIT;
    }

    // 如果过滤后没有 barriers，可能需要一个完整的屏障
    if (filtered_barriers == 0 && barriers != 0) {
        LOG_W("glMemoryBarrier: All barriers filtered out, using full barrier");
        // 在某些实现中，可以使用 glFinish() 作为最后手段
        GLES.glFinish();
        return;
    }

    GLES.glMemoryBarrier(filtered_barriers);
    
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        LOG_E("glMemoryBarrier failed with error: 0x%x, barriers: 0x%x", err, filtered_barriers);
        
        LOG_W("Falling back to glFinish()");
        GLES.glFinish();
    } else {
        LOG_D("glMemoryBarrier succeeded with barriers: 0x%x", filtered_barriers);
    }
    
    CHECK_GL_ERROR
}

void glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const void* indices, GLint basevertex) {
    LOG()
    LOG_D("glDrawElementsBaseVertex, mode: %d, count: %d, type: %d, indices: %p, basevertex: %d", mode, count, type,
          indices, basevertex);
    prepareForDraw();
    if (hardware->es_version < 320 && !g_gles_caps.GL_EXT_draw_elements_base_vertex &&
        !g_gles_caps.GL_OES_draw_elements_base_vertex) {
        // TODO: use indirect drawing for GLES 3.1
        LOG_D("Emulating glDrawElementsBaseVertex")
        GLint prevElementBuffer;
        GLES.glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &prevElementBuffer);

        if (basevertex == 0) {
            GLES.glDrawElements(mode, count, type, indices);
            return;
        }

        size_t indexSize;
        switch (type) {
        case GL_UNSIGNED_INT:
            indexSize = sizeof(GLuint);
            break;
        case GL_UNSIGNED_SHORT:
            indexSize = sizeof(GLushort);
            break;
        case GL_UNSIGNED_BYTE:
            indexSize = sizeof(GLubyte);
            break;
        default:
            return;
        }

        void* tempIndices = malloc(count * indexSize);
        if (!tempIndices) {
            return;
        }

        if (prevElementBuffer != 0) {
            GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prevElementBuffer);
            void* srcData =
                GLES.glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, (GLintptr)indices, count * indexSize, GL_MAP_READ_BIT);

            if (srcData) {
                memcpy(tempIndices, srcData, count * indexSize);
                GLES.glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
            } else {
                free(tempIndices);
                return;
            }
        } else {
            memcpy(tempIndices, indices, count * indexSize);
        }

        switch (type) {
        case GL_UNSIGNED_INT:
            for (int j = 0; j < count; ++j) {
                ((GLuint*)tempIndices)[j] += basevertex;
            }
            break;
        case GL_UNSIGNED_SHORT:
            for (int j = 0; j < count; ++j) {
                ((GLushort*)tempIndices)[j] += basevertex;
            }
            break;
        case GL_UNSIGNED_BYTE:
            for (int j = 0; j < count; ++j) {
                ((GLubyte*)tempIndices)[j] += basevertex;
            }
            break;
        }

        GLuint tempBuffer;
        GLES.glGenBuffers(1, &tempBuffer);
        GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempBuffer);
        GLES.glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * indexSize, tempIndices, GL_STREAM_DRAW);
        free(tempIndices);

        GLES.glDrawElements(mode, count, type, 0);

        GLES.glDeleteBuffers(1, &tempBuffer);
        GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prevElementBuffer);

        CHECK_GL_ERROR
    } else {
        GLES.glDrawElementsBaseVertex(mode, count, type, indices, basevertex);
    }
    CHECK_GL_ERROR
}
