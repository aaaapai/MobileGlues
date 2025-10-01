//
// Created by BZLZHH on 2025/1/29.
//

#include "drawing.h"
#include "buffer.h"
#include "framebuffer.h"
#include "mg.h"
#include "texture.h"
#include "fpe/fpe.hpp"
#include "fpe/list.h"
#include <ankerl/unordered_dense.h>
#include <string_view>
#include <span>
#include <format>
#include <memory>
#include <array>
#include <optional>
#include <bit>
#include <utility>
#include <type_traits>
#include <algorithm>
#include <ranges>

#define DEBUG 0

GLuint bufSampelerProg;
GLuint bufSampelerLoc;
std::string bufSampelerName;

extern UnorderedMap<GLuint, bool> program_map_is_sampler_buffer_emulated;
extern UnorderedMap<GLuint, bool> program_map_is_atomic_counter_emulated;

UnorderedMap<GLuint, SamplerInfo> g_samplerCacheForSamplerBuffer;

void setupBufferTextureUniforms(GLuint program) {
    LOG_D("setupBufferTextureUniforms, program: {}", program);

    if (auto it = program_map_is_sampler_buffer_emulated.find(program); 
        it == program_map_is_sampler_buffer_emulated.end() || !it->second) {
        return;
    }

    if (auto [it, inserted] = g_samplerCacheForSamplerBuffer.try_emplace(program); inserted) {
        auto& progSamplerInfo = it->second;
        GLint locWidth = GLES.glGetUniformLocation(program, "u_BufferTexWidth");
        GLint locHeight = GLES.glGetUniformLocation(program, "u_BufferTexHeight");
        if (locWidth == -1) {
            LOG_W("u_BufferTexWidth uniform not found in program {}", program);
            return;
        }

        progSamplerInfo = SamplerInfo{
            .locWidth = locWidth,
            .locHeight = locHeight,
            .samplers = {}
        };

        GLint numUniforms = 0;
        GLES.glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);
        LOG_D("Program {} has {} active uniforms", program, numUniforms);

        progSamplerInfo.samplers.reserve(static_cast<size_t>(numUniforms));
        for (GLint i = 0; i < numUniforms; ++i) {
            constexpr GLsizei bufSize = 256;
            std::array<GLchar, bufSize> name{};
            GLsizei length = 0;
            GLint size = 0;
            GLenum type = 0;
            GLES.glGetActiveUniform(program, i, bufSize, &length, &size, &type, name.data());

            if (type == GL_SAMPLER_2D || type == GL_INT_SAMPLER_2D) {
                GLint locSampler = GLES.glGetUniformLocation(program, name.data());
                if (locSampler != -1) {
                    progSamplerInfo.samplers.push_back(locSampler);
                }
            }
        }
    }

    auto& progSamplerInfo = g_samplerCacheForSamplerBuffer[program];

    GLint locWidth = progSamplerInfo.locWidth;
    GLint locHeight = progSamplerInfo.locHeight;

    for (auto locSampler : progSamplerInfo.samplers | std::views::filter([](GLint loc) { return loc >= 0; })) {
        GLuint prev_unit = gl_state->current_tex_unit;
        constexpr GLint unit = 15;

        GLES.glActiveTexture(GL_TEXTURE0 + unit);
        GLint texId = 0;
        GLES.glGetIntegerv(GL_TEXTURE_BINDING_2D, &texId);
        if (texId == 0) {
            GLES.glActiveTexture(GL_TEXTURE0 + prev_unit);
            continue;
        }

        auto texObject = mgGetTexObjectByID(texId);
        if (!texObject) {
            GLES.glActiveTexture(GL_TEXTURE0 + prev_unit);
            continue;
        }

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

void glDrawArrays(GLenum mode, GLint first, GLsizei count) {
    LOG()
    LOG_D("glDrawArrays(), mode = {}, first = {}, count = {}", glEnumToString(mode), first, count)

    LIST_RECORD(glDrawArrays, {}, mode, first, count)

    // TODO: deal with draw in list later
    if (DisplayListManager::isCalling()) {
        return;
    }

    INIT_CHECK_GL_ERROR

    CHECK_GL_ERROR_NO_INIT
    GET_PREV_PROGRAM
    int do_draw_element = commit_fpe_state_on_draw(&mode, &first, &count);
    if (do_draw_element) {
        LOG_D("Switch to glDrawElements(), mode = {}, count = {}", glEnumToString(mode), count)

        GLES.glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
    } else
        GLES.glDrawArrays(mode, first, count);

    SET_PREV_PROGRAM
    GLES.glBindVertexArray(0);
    CHECK_GL_ERROR_NO_INIT
}

void glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount) {
    LOG()
    LOG_D("glDrawElementsInstanced, mode: {}, count: {}, type: {}, indices: {}, primcount: {}", mode, count, type,
          indices, primcount)
    prepareForDraw();
    GLES.glDrawElementsInstanced(mode, count, type, indices, primcount);
    CHECK_GL_ERROR
}

void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) {
    LOG()
    LOG_D("glDrawElements, mode: {}, count: {}, type: {}, indices: {}", mode, count, type, indices)
    prepareForDraw();
    GLES.glDrawElements(mode, count, type, indices);
    CHECK_GL_ERROR
}

void glBindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access,
                        GLenum format) {
    LOG()
    LOG_D("glBindImageTexture, unit: {}, texture: {}, level: {}, layered: {}, layer: {}, access: {}, format: {}", unit,
          texture, level, layered, layer, access, format)
    GLES.glBindImageTexture(unit, texture, level, layered, layer, access, format);
    CHECK_GL_ERROR
}

void glUniform1i(GLint location, GLint v0) {
    LOG()
    LOG_D("glUniform1i, location: {}, v0: {}", location, v0)
    GLES.glUniform1i(location, v0);
    CHECK_GL_ERROR
}

void bindAllAtomicCounterAsSSBO();
void glDispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) {
    LOG()
    LOG_D("glDispatchCompute, num_groups_x: {}, num_groups_y: {}, num_groups_z: {}", num_groups_x, num_groups_y,
          num_groups_z)
    if (auto it = program_map_is_atomic_counter_emulated.find(gl_state->current_program);
        it != program_map_is_atomic_counter_emulated.end() && it->second) {
        bindAllAtomicCounterAsSSBO();
        LOG_D("Atomic counters bound as SSBOs for program {}", gl_state->current_program);
    } else {
        LOG_D("No atomic counters bound as SSBOs for program {}", gl_state->current_program);
    }
    GLES.glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    CHECK_GL_ERROR
}

void glMemoryBarrier(GLbitfield barriers) {
    LOG()
    LOG_D("glMemoryBarrier, barriers: {}", barriers)
    if (auto it = program_map_is_atomic_counter_emulated.find(gl_state->current_program);
        it != program_map_is_atomic_counter_emulated.end() && it->second) {
        barriers |= GL_ATOMIC_COUNTER_BARRIER_BIT;
        barriers |= GL_SHADER_STORAGE_BARRIER_BIT;
    }
    GLES.glMemoryBarrier(barriers);
    CHECK_GL_ERROR
}

void glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const void* indices, GLint basevertex) {
    LOG()
    LOG_D("glDrawElementsBaseVertex, mode: {}, count: {}, type: {}, indices: {}, basevertex: {}", mode, count, type,
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

        std::optional<size_t> indexSize;
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

        const auto bufferSize = count * *indexSize;
        auto tempIndices = std::make_unique<std::byte[]>(bufferSize);

        if (prevElementBuffer != 0) {
            GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prevElementBuffer);
            void* srcData =
                GLES.glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, reinterpret_cast<GLintptr>(indices), bufferSize, GL_MAP_READ_BIT);

            if (srcData) {
                std::memcpy(tempIndices.get(), srcData, bufferSize);
                GLES.glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
            } else {
                return;
            }
        } else {
            std::memcpy(tempIndices.get(), indices, bufferSize);
        }

        // 使用 lambda 避免重复代码
        auto addBaseVertex = [basevertex](auto* data, size_t count) {
            std::for_each_n(data, count, [basevertex](auto& index) {
                index += static_cast<std::remove_reference_t<decltype(index)>>(basevertex);
            });
        };

        switch (type) {
        case GL_UNSIGNED_INT: {
            addBaseVertex(reinterpret_cast<GLuint*>(tempIndices.get()), count);
            break;
        }
        case GL_UNSIGNED_SHORT: {
            addBaseVertex(reinterpret_cast<GLushort*>(tempIndices.get()), count);
            break;
        }
        case GL_UNSIGNED_BYTE: {
            addBaseVertex(reinterpret_cast<GLubyte*>(tempIndices.get()), count);
            break;
        }
        }

        GLuint tempBuffer;
        GLES.glGenBuffers(1, &tempBuffer);
        GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempBuffer);
        GLES.glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, tempIndices.get(), GL_STREAM_DRAW);

        GLES.glDrawElements(mode, count, type, nullptr);

        GLES.glDeleteBuffers(1, &tempBuffer);
        GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prevElementBuffer);

        CHECK_GL_ERROR
    } else {
        GLES.glDrawElementsBaseVertex(mode, count, type, indices, basevertex);
    }
    CHECK_GL_ERROR
}
