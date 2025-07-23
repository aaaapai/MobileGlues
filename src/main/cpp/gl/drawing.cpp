//
// Created by BZLZHH on 2025/1/29.
//

#include "drawing.h"
#include "buffer.h"
#include "framebuffer.h"
#include "fpe/fpe.hpp"
#include "fpe/list.h"

#include "mg.h"
#include "texture.h"
#include <ankerl/unordered_dense.h>


#define DEBUG 0

template <typename Key, typename Value>
using unordered_map = ankerl::unordered_dense::map<Key, Value>;

GLuint bufSampelerProg;
GLuint bufSampelerLoc;
std::string bufSampelerName;

extern unordered_map<GLuint, bool> program_map_is_sampler_buffer_emulated;

unordered_map<GLuint, SamplerInfo> g_samplerCacheForSamplerBuffer;

void setupBufferTextureUniforms(GLuint program) {
    LOG_D("setupBufferTextureUniforms, program: %d", program);

    if (!program_map_is_sampler_buffer_emulated[program])
        return;

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
        
        GLint width = g_textures[texId].width;
        
        GLES.glUniform1i(locSampler, unit);
        GLES.glUniform1i(locWidth, g_textures[texId].width);
        GLES.glUniform1i(locHeight, g_textures[texId].height);
        
        GLES.glActiveTexture(GL_TEXTURE0 + prev_unit);
    }
}

void prepareForDraw() {
	LOG_D("prepareForDraw...")
    if (hardware->emulate_texture_buffer)
    {	
        setupBufferTextureUniforms(gl_state->current_program);
    }
}

#define DEBUG 0

void glDrawArrays(GLenum mode, GLint first, GLsizei count) {
    LOG()
    LOG_D("glDrawArrays(), mode = %s, first = %d, count = %u", glEnumToString(mode), first, count)

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
        LOG_D("Switch to glDrawElements(), mode = %s, count = %u", glEnumToString(mode), count)

        GLES.glDrawElements(mode, count, GL_UNSIGNED_INT, (void *) 0);
    } else
        GLES.glDrawArrays(mode, first, count);

    SET_PREV_PROGRAM
    GLES.glBindVertexArray(0);
    CHECK_GL_ERROR_NO_INIT
}

///*_Thread_local*/ static bool unexpected_error = false; // solve the crash error for ANGLE
// Why thread local here? We've never PRETEND we are thread safe.

// solve the crash error for ANGLE, but it will make Derivative Main with Optifine not work!

//_Thread_local static bool unexpected_error = false;

void glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount) {
    LOG()
    LOG_D("glDrawElementsInstanced, mode: %d, count: %d, type: %d, indices: %p, primcount: %d",
          mode, count, type, indices, primcount)
    prepareForDraw();
    GLES.glDrawElementsInstanced(mode, count, type, indices, primcount);
    CHECK_GL_ERROR
}

void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) {
    LOG()
    LOG_D("glDrawElements, mode: %d, count: %d, type: %d, indices: %p", mode, count, type, indices)
    prepareForDraw();
    //LOAD_GLES_FUNC(glGetError)
    //GLenum pre_err = GLES.glGetError();
    //if(pre_err != GL_NO_ERROR) {
    //    LOG_D("Skipping due to prior error: 0x%04X", pre_err)
    //    return;
    //}
    //if (!unexpected_error) {
    //    LOG_D("es_glDrawElements, mode: %d, count: %d, type: %d, indices: %p", mode, count, type, indices)
    GLES.glDrawElements(mode, count, type, indices);
    CHECK_GL_ERROR
    //} else {
    //    unexpected_error = false;
    //}
}

void glBindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) {

    LOG()
    LOG_D("glBindImageTexture, unit: %d, texture: %d, level: %d, layered: %d, layer: %d, access: %d, format: %d",
          unit, texture, level, layered, layer, access, format)
    //LOAD_GLES_FUNC(glGetError)
    GLES.glBindImageTexture(unit, texture, level, layered, layer, access, format);
    CHECK_GL_ERROR
    //GLenum err;
    //while((err = GLES.glGetError()) != GL_NO_ERROR) {
    //    LOG_D("GL Error: 0x%04X", err)
    //    unexpected_error = true;
    //}
}

void glUniform1i(GLint location, GLint v0) {
    LOG()
    LOG_D("glUniform1i, location: %d, v0: %d", location, v0)
    //LOAD_GLES_FUNC(glGetError)
    GLES.glUniform1i(location, v0);
    CHECK_GL_ERROR
    //GLenum err;
    //while((err = GLES.glGetError()) != GL_NO_ERROR) {
    //    LOG_D("GL Error: 0x%04X", err)
    //    unexpected_error = true;
    //}
}

void glDispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) {
    LOG()
    LOG_D("glDispatchCompute, num_groups_x: %d, num_groups_y: %d, num_groups_z: %d",
          num_groups_x, num_groups_y, num_groups_z)
    //LOAD_GLES_FUNC(glGetError)
    //GLenum pre_err = GLES.glGetError();
    //if(pre_err != GL_NO_ERROR) {
    //    LOG_D("Skipping due to prior error: 0x%04X", pre_err)
    //    return;
    //}
    //if (!unexpected_error) {
    //    LOG_D("es_glDispatchCompute, num_groups_x: %d, num_groups_y: %d, num_groups_z: %d",
    //          num_groups_x, num_groups_y, num_groups_z)
    GLES.glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    CHECK_GL_ERROR
    //} else {
    //    unexpected_error = false;
    //}
}

void glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const void* indices, GLint basevertex) {
    LOG()
    LOG_D("glDrawElementsBaseVertex, mode: %d, count: %d, type: %d, indices: %p, basevertex: %d",
        mode, count, type, indices, basevertex);
    prepareForDraw();
    if (hardware->es_version < 320) {
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
        case GL_UNSIGNED_INT:  indexSize = sizeof(GLuint);   break;
        case GL_UNSIGNED_SHORT: indexSize = sizeof(GLushort); break;
        case GL_UNSIGNED_BYTE:  indexSize = sizeof(GLubyte);  break;
        default: return;
        }

        void* tempIndices = malloc(count * indexSize);
        if (!tempIndices) {
            return;
        }

        if (prevElementBuffer != 0) {
            GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prevElementBuffer);
            void* srcData = GLES.glMapBufferRange(
                GL_ELEMENT_ARRAY_BUFFER,
                (GLintptr)indices,
                count * indexSize,
                GL_MAP_READ_BIT
            );

            if (srcData) {
                memcpy(tempIndices, srcData, count * indexSize);
                GLES.glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
            }
            else {
                free(tempIndices);
                return;
            }
        }
        else {
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
