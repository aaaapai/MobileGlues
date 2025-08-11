//
// Created by Swung 0x48 on 2025-04-20.
//

#include "multidraw.h"
#include "../config/settings.h"

#define DEBUG 0

typedef void (*glMultiDrawElements_t)(GLenum, const GLsizei*, GLenum, const void* const*, GLsizei);

void glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount) {
    static glMultiDrawElements_t func_ptr = nullptr;

    if (func_ptr == nullptr) {
        switch (global_settings.multidraw_mode) {
            case multidraw_mode_t::PreferIndirect:
                func_ptr = mg_glMultiDrawElements_indirect;
                break;
            case multidraw_mode_t::PreferBaseVertex:
                func_ptr = mg_glMultiDrawElements_basevertex;
                break;
            case multidraw_mode_t::PreferMultidrawIndirect:
                func_ptr = mg_glMultiDrawElements_multiindirect;
                break;
            case multidraw_mode_t::DrawElements:
                func_ptr = mg_glMultiDrawElements_drawelements;
                break;
            case multidraw_mode_t::Compute:
                func_ptr = mg_glMultiDrawElements_compute;
                break;
            case multidraw_mode_t::DeepSeekOne:
                func_ptr = mg_glMultiDrawElements_deepseek_one;
                break;
            case multidraw_mode_t::DeepSeekTwo:
                func_ptr = mg_glMultiDrawElements_deepseek_two;
                break;
            case multidraw_mode_t::Native:
                GLES.glMultiDrawElementsEXT(mode, count, type, indices, primcount);
                return;
            default:
                func_ptr = mg_glMultiDrawElements_drawelements;
                break;
        }
    }
    func_ptr(mode, count, type, indices, primcount);
}

typedef void (*glMultiDrawElementsBaseVertex_t)(GLenum, GLsizei*, GLenum, const void* const*, GLsizei, const GLint*);

void glMultiDrawElementsBaseVertex(GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex) {
    static glMultiDrawElementsBaseVertex_t func_ptr = nullptr;

    if (func_ptr == nullptr) {
        switch (global_settings.multidraw_mode) {
            case multidraw_mode_t::PreferIndirect:
                func_ptr = mg_glMultiDrawElementsBaseVertex_indirect;
                break;
            case multidraw_mode_t::PreferBaseVertex:
                func_ptr = mg_glMultiDrawElementsBaseVertex_basevertex;
                break;
            case multidraw_mode_t::PreferMultidrawIndirect:
                func_ptr = mg_glMultiDrawElementsBaseVertex_multiindirect;
                break;
            case multidraw_mode_t::DrawElements:
                func_ptr = mg_glMultiDrawElementsBaseVertex_drawelements;
                break;
            case multidraw_mode_t::Compute:
                func_ptr = mg_glMultiDrawElementsBaseVertex_compute;
                break;
            case multidraw_mode_t::DeepSeekOne:
                func_ptr = mg_glMultiDrawElementsBaseVertex_deepseek_one;
                break;
            case multidraw_mode_t::DeepSeekTwo:
                func_ptr = mg_glMultiDrawElementsBaseVertex_deepseek_two;
                break;
            case multidraw_mode_t::Native:
                GLES.glMultiDrawElementsBaseVertexEXT(mode, counts, type, indices, primcount, basevertex);
                return;
            default:
                func_ptr = mg_glMultiDrawElementsBaseVertex_drawelements;
                break;
        }
    }

    func_ptr(mode, counts, type, indices, primcount, basevertex);
}

typedef void (*glMultiDrawElementsIndirect_t)(GLenum, GLenum, const void *, GLsizei, GLsizei);

void glMultiDrawElementsIndirect(GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride) {
    static glMultiDrawElementsIndirect_t func_ptr = nullptr;

    if (func_ptr == nullptr) {
        switch (global_settings.multidraw_mode) {
            case multidraw_mode_t::PreferIndirect:
                func_ptr = mg_glMultiDrawElementsIndirect_deepseek_one;
                break;
            case multidraw_mode_t::PreferBaseVertex:
                func_ptr = mg_glMultiDrawElementsIndirect_deepseek_one;
                break;
            case multidraw_mode_t::PreferMultidrawIndirect:
                GLES.glMultiDrawElementsIndirectEXT(mode, type, indirect, drawcount, stride);
                return;
            case multidraw_mode_t::DrawElements:
                func_ptr = mg_glMultiDrawElementsIndirect_deepseek_one;
                break;
            case multidraw_mode_t::Compute:
                func_ptr = mg_glMultiDrawElementsIndirect_deepseek_one;
                break;
            case multidraw_mode_t::DeepSeekOne:
                func_ptr = mg_glMultiDrawElementsIndirect_deepseek_one;
                break;
            case multidraw_mode_t::DeepSeekTwo:
                func_ptr = mg_glMultiDrawElementsIndirect_deepseek_one;
                break;
            case multidraw_mode_t::Native:
                GLES.glMultiDrawElementsIndirectEXT(mode, type, indirect, drawcount, stride);
                return;
            default:
                func_ptr = mg_glMultiDrawElementsIndirect_deepseek_one;
                break;
        }
    }

    func_ptr(mode, type, indirect, drawcount, stride);
}

static bool g_indirect_cmds_inited = false;
static GLsizei g_cmdbufsize = 0;
static GLuint g_indirectbuffer = 0;
static GLuint prevIndirectBuffer = 0;

static void prepare_indirect_buffer(const GLsizei *counts, GLenum type, const void *const *indices,
                             GLsizei primcount, const GLint *basevertex) {
	GLES.glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING, (GLint*)&prevIndirectBuffer);
    if (!g_indirect_cmds_inited) {
        GLES.glGenBuffers(1, &g_indirectbuffer);
        GLES.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectbuffer);
        g_cmdbufsize = 1;
        GLES.glBufferData(GL_DRAW_INDIRECT_BUFFER,
                          g_cmdbufsize * sizeof(draw_elements_indirect_command_t), NULL, GL_DYNAMIC_DRAW);

        g_indirect_cmds_inited = true;
    }
	GLES.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectbuffer);

    if (g_cmdbufsize < primcount) {
        size_t sz = g_cmdbufsize;

        LOG_D("Before resize: %d", sz)

        // 2-exponential to reduce reallocation
        while (sz < static_cast<size_t>(primcount))
            sz *= 2;

        GLES.glBufferData(GL_DRAW_INDIRECT_BUFFER,
                          sz * sizeof(draw_elements_indirect_command_t), NULL, GL_DYNAMIC_DRAW);
        g_cmdbufsize = sz;
    }

    LOG_D("After resize: %d", g_cmdbufsize)

    auto* pcmds = (draw_elements_indirect_command_t*)
            GLES.glMapBufferRange(GL_DRAW_INDIRECT_BUFFER,
                                  0, primcount * sizeof(draw_elements_indirect_command_t),
                                  GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    GLsizei elementSize;
    switch (type) {
        case GL_UNSIGNED_BYTE:
            elementSize = 1;
            break;
        case GL_UNSIGNED_SHORT:
            elementSize = 2;
            break;
        case GL_UNSIGNED_INT:
            elementSize = 4;
            break;
        default:
            elementSize = 4;
    }

    for (GLsizei i = 0; i < primcount; ++i) {
        auto byteOffset = reinterpret_cast<uintptr_t>(indices[i]);
        pcmds[i].firstIndex = static_cast<GLuint>(byteOffset / elementSize);
        pcmds[i].count = counts[i];
        pcmds[i].instanceCount = 1;
        pcmds[i].baseVertex = basevertex ? basevertex[i] : 0;
        pcmds[i].reservedMustBeZero = 0;
    }

    GLES.glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
}

void mg_glMultiDrawElementsBaseVertex_drawelements(GLenum mode, GLsizei* counts, GLenum type, const void* const* indices, GLsizei primcount, const GLint* basevertex) {
    LOG()
    prepareForDraw();
	
    GLint prevElementBuffer;
    GLES.glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &prevElementBuffer);

    for (GLsizei i = 0; i < primcount; ++i) {
        if (counts[i] <= 0) continue;

        GLsizei currentCount = counts[i];
        const GLvoid *currentIndices = indices[i];
        GLint currentBaseVertex = basevertex[i];

        size_t indexSize;
        switch (type) {
            case GL_UNSIGNED_INT:  indexSize = sizeof(GLuint);   break;
            case GL_UNSIGNED_SHORT: indexSize = sizeof(GLushort); break;
            case GL_UNSIGNED_BYTE:  indexSize = sizeof(GLubyte);  break;
            default: return;
        }

        GLuint tempBuffer;
        GLES.glGenBuffers(1, &tempBuffer);
        GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempBuffer);

        void *srcData = nullptr;
        void *tempIndices = malloc(currentCount * indexSize);
        if (!tempIndices) {
            GLES.glDeleteBuffers(1, &tempBuffer);
            continue;
        }

        if (prevElementBuffer != 0) {
            GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prevElementBuffer);
            srcData = GLES.glMapBufferRange(
                    GL_ELEMENT_ARRAY_BUFFER,
                    (GLintptr)currentIndices,
                    currentCount * indexSize,
                    GL_MAP_READ_BIT
            );

            if (!srcData) {
                free(tempIndices);
                GLES.glDeleteBuffers(1, &tempBuffer);
                continue;
            }
        } else {
            srcData = (void*)currentIndices;
        }

        switch (type) {
            case GL_UNSIGNED_INT:
                for (int j = 0; j < currentCount; ++j) {
                    ((GLuint *)tempIndices)[j] = ((GLuint *)srcData)[j] + currentBaseVertex;
                }
                break;
            case GL_UNSIGNED_SHORT:
                for (int j = 0; j < currentCount; ++j) {
                    ((GLushort *)tempIndices)[j] = ((GLushort *)srcData)[j] + currentBaseVertex;
                }
                break;
            case GL_UNSIGNED_BYTE:
                for (int j = 0; j < currentCount; ++j) {
                    ((GLubyte *)tempIndices)[j] = ((GLubyte *)srcData)[j] + currentBaseVertex;
                }
                break;
        }

        if (prevElementBuffer != 0) {
            GLES.glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        }

        GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempBuffer);
        GLES.glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentCount * indexSize, tempIndices, GL_STREAM_DRAW);
        free(tempIndices);
        GLES.glDrawElements(mode, currentCount, type, 0);

        GLES.glDeleteBuffers(1, &tempBuffer);
    }

    GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prevElementBuffer);

    CHECK_GL_ERROR
}

void mg_glMultiDrawElementsBaseVertex_indirect(GLenum mode, GLsizei* counts, GLenum type, const void* const* indices, GLsizei primcount, const GLint* basevertex) {
    LOG()

	prepareForDraw();

    prepare_indirect_buffer(counts, type, indices, primcount, basevertex);

    // Draw indirect!
    for (GLsizei i = 0; i < primcount; ++i) {
        const GLvoid* offset = reinterpret_cast<GLvoid*>(i * sizeof(draw_elements_indirect_command_t));
        GLES.glDrawElementsIndirect(mode, type, offset);
    }

	GLES.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, prevIndirectBuffer);

    CHECK_GL_ERROR
}

void mg_glMultiDrawElementsBaseVertex_multiindirect(GLenum mode, GLsizei* counts, GLenum type, const void* const* indices, GLsizei primcount, const GLint* basevertex) {
    LOG()
    prepareForDraw();

    prepare_indirect_buffer(counts, type, indices, primcount, basevertex);

    // Multi-draw indirect!
    GLES.glMultiDrawElementsIndirectEXT(mode, type, 0, primcount, 0);

    GLES.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, prevIndirectBuffer);

    CHECK_GL_ERROR
}

void mg_glMultiDrawElementsBaseVertex_basevertex(GLenum mode, GLsizei* counts, GLenum type, const void* const* indices, GLsizei primcount, const GLint* basevertex) {
    LOG()
    prepareForDraw();

    for (GLsizei i = 0; i < primcount; ++i) {
        const GLsizei count = counts[i];
        if (count > 0) {
            LOG_D("GLES.glDrawElementsBaseVertex, mode = %s, count = %d, type = %s, indices[i] = 0x%x, basevertex[i] = %d",
                  glEnumToString(mode), count, glEnumToString(type), indices[i], basevertex[i])
            GLES.glDrawElementsBaseVertex(mode, count, type, indices[i], basevertex[i]);
        }
    }
    CHECK_GL_ERROR
}

void mg_glMultiDrawElements_indirect(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount) {
    LOG()
    prepareForDraw();

    prepare_indirect_buffer(count, type, indices, primcount, 0);
    // Draw indirect!
    for (GLsizei i = 0; i < primcount; ++i) {
        const GLvoid* offset = reinterpret_cast<GLvoid*>(i * sizeof(draw_elements_indirect_command_t));
        GLES.glDrawElementsIndirect(mode, type, offset);
    }

    GLES.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, prevIndirectBuffer);
    CHECK_GL_ERROR
}

void mg_glMultiDrawElements_drawelements(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount) {
    LOG()
    prepareForDraw();

    for (GLsizei i = 0; i < primcount; ++i) {
        const GLsizei c = count[i];
        if (c > 0) {
            GLES.glDrawElements(mode, c, type, indices[i]);
        }
    }

    CHECK_GL_ERROR
}

void mg_glMultiDrawElements_compute(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount) {
    LOG()
    prepareForDraw();

    for (GLsizei i = 0; i < primcount; ++i) {
        const GLsizei c = count[i];
        if (c > 0) {
            GLES.glDrawElements(mode, c, type, indices[i]);
        }
    }

    CHECK_GL_ERROR
}

void mg_glMultiDrawElements_multiindirect(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount) {
    LOG()
    prepareForDraw();

    prepare_indirect_buffer(count, type, indices, primcount, 0);

    // Multi-draw indirect!
    GLES.glMultiDrawElementsIndirectEXT(mode, type, 0, primcount, 0);

    GLES.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, prevIndirectBuffer);

    CHECK_GL_ERROR
}

void mg_glMultiDrawElements_basevertex(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount) {
    LOG()
    prepareForDraw();

    for (GLsizei i = 0; i < primcount; ++i) {
        const GLsizei c = count[i];
        if (c > 0) {
            GLES.glDrawElements(mode, c, type, indices[i]);
        }
    }

    CHECK_GL_ERROR
}

static const std::string multidraw_comp_shader =
R"(#version 320 es

layout(local_size_x = 64) in;

layout(std430, binding = 0) readonly buffer Input { uint in_indices[]; };
layout(std430, binding = 1) readonly buffer FirstIndex { uint firstIndex[]; };
layout(std430, binding = 2) readonly buffer BaseVertex { int baseVertex[]; };
layout(std430, binding = 3) readonly buffer Prefix { uint prefixSums[]; };
layout(std430, binding = 4) writeonly buffer Output { uint out_indices[]; };

void main() {
    uint outIdx = gl_GlobalInvocationID.x;
    if (outIdx >= prefixSums[prefixSums.length() - 1])
    return;

    int low = 0;
    int high = prefixSums.length() - 1;
    while (low < high) {
        int mid = low + (high - low) / 2;
        if (prefixSums[mid] > outIdx) {
            high = mid; // next [low, mid)
        }
        else {
            low = mid + 1; // next [mid + 1, high)
        }
    }

    // figure out which index to take
    //DrawCommand cmd = draws[low];
    uint localIdx = outIdx - ((low == 0) ? 0u : (prefixSums[low - 1]));
    uint inIndex = localIdx + firstIndex[low] / 4u; // elementSize == 4

    // Write out
    out_indices[outIdx] = uint(int(in_indices[inIndex]) + baseVertex[low]);
}

)";

static bool g_compute_inited = false;
static std::vector<GLuint> g_prefix_sum(1);
static GLuint g_prefixsumbuffer = 0;
static GLuint g_firstidx_ssbo = 0;
static GLuint g_basevtx_ssbo = 0;
static GLuint g_outputibo = 0;
static GLuint g_compute_program = 0;
static char g_compile_info[1024];

static GLuint compile_compute_program(const std::string& src) {
    INIT_CHECK_GL_ERROR
    auto program = GLES.glCreateProgram();
    CHECK_GL_ERROR_NO_INIT
    GLuint shader = GLES.glCreateShader(GL_COMPUTE_SHADER);
    CHECK_GL_ERROR_NO_INIT
    const char* s[] = { src.c_str() };
    const GLint length[] = { static_cast<GLint>(src.length()) };
    GLES.glShaderSource(shader, 1, s, length);
    CHECK_GL_ERROR_NO_INIT
    GLES.glCompileShader(shader);
    CHECK_GL_ERROR_NO_INIT
    int success = 0;
    GLES.glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    CHECK_GL_ERROR_NO_INIT
    if (!success) {
        GLES.glGetShaderInfoLog(shader, 1024, NULL, g_compile_info);
        CHECK_GL_ERROR_NO_INIT
        LOG_E("%s: %s shader compile error: %s\nsrc:\n%s",
              __func__,
              "compute",
              g_compile_info,
              src.c_str());
#if DEBUG || GLOBAL_DEBUG
        abort();
#endif
        return -1;
    }

    GLES.glAttachShader(program, shader);
    CHECK_GL_ERROR_NO_INIT
    GLES.glLinkProgram(program);
    CHECK_GL_ERROR_NO_INIT

    GLES.glGetProgramiv(program, GL_LINK_STATUS, &success);
    CHECK_GL_ERROR_NO_INIT
    if(!success) {
        GLES.glGetProgramInfoLog(program, 1024, NULL, g_compile_info);
        CHECK_GL_ERROR_NO_INIT
        LOG_E("program link error: %s", g_compile_info);
#if DEBUG || GLOBAL_DEBUG
        abort();
#endif
        return -1;
    }

    return program;
}

GLAPI GLAPIENTRY void mg_glMultiDrawElementsBaseVertex_compute(
        GLenum mode, GLsizei *counts, GLenum type, const void *const *indices, GLsizei primcount, const GLint *basevertex) {
    LOG()
    prepareForDraw();

    INIT_CHECK_GL_ERROR

    if (primcount <= 0)
        return;

    // TODO: support `types` other than GL_UNSIGNED_INT

    // Align compute shader input format with standard OpenGL indirect-draw format
//    prepare_indirect_buffer(counts, type, indices, primcount, basevertex);
//    prepare_compute_drawcmd_ssbo(counts, type, indices, primcount, basevertex);

    // Init compute buffers
    if (!g_compute_inited) {
        LOG_D("Initializing multidraw compute pipeline...")
        GLES.glGenBuffers(1, &g_prefixsumbuffer);
        GLES.glGenBuffers(1, &g_firstidx_ssbo);
        GLES.glGenBuffers(1, &g_basevtx_ssbo);
        GLES.glGenBuffers(1, &g_outputibo);

        g_compute_program = compile_compute_program(multidraw_comp_shader);

        g_compute_inited = true;
    }

    // Resize prefix sum buffer if needed
    size_t sz = g_prefix_sum.empty() ? 1 : g_prefix_sum.size();
    while (sz < static_cast<size_t>(primcount))
        sz *= 2;
    g_prefix_sum.resize(sz);

    // Calculate prefix sum
    g_prefix_sum[0] = counts[0];
    for (GLsizei i = 1; i < primcount; ++i) {
        g_prefix_sum[i] = g_prefix_sum[i - 1] + counts[i];
    }

    // Fill in the data
    GLES.glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_firstidx_ssbo);
    CHECK_GL_ERROR_NO_INIT
    GLES.glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * primcount, indices, GL_DYNAMIC_DRAW);
    CHECK_GL_ERROR_NO_INIT

    GLES.glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_basevtx_ssbo);
    CHECK_GL_ERROR_NO_INIT
    GLES.glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLint) * primcount, basevertex, GL_DYNAMIC_DRAW);
    CHECK_GL_ERROR_NO_INIT

    GLES.glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_prefixsumbuffer);
    CHECK_GL_ERROR_NO_INIT
    GLES.glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * primcount, g_prefix_sum.data(), GL_DYNAMIC_DRAW);
    CHECK_GL_ERROR_NO_INIT

    // Allocate output buffer
    auto total_indices = g_prefix_sum[primcount - 1];
    GLES.glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_outputibo);
    CHECK_GL_ERROR_NO_INIT
    GLES.glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * total_indices, nullptr, GL_DYNAMIC_DRAW);
    CHECK_GL_ERROR_NO_INIT


    GLES.glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    CHECK_GL_ERROR_NO_INIT

    GLint ibo = 0;
    GLES.glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ibo);
    CHECK_GL_ERROR_NO_INIT

    // Bind buffers
//    GLES.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ibo);
//    CHECK_GL_ERROR_NO_INIT
//    GLES.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, g_drawssbo);
//    CHECK_GL_ERROR_NO_INIT
//    GLES.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, g_prefixsumbuffer);
//    CHECK_GL_ERROR_NO_INIT
//    GLES.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, g_outputibo);
//    CHECK_GL_ERROR_NO_INIT

    GLES.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ibo);
    CHECK_GL_ERROR_NO_INIT
    GLES.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, g_firstidx_ssbo);
    CHECK_GL_ERROR_NO_INIT
    GLES.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, g_basevtx_ssbo);
    CHECK_GL_ERROR_NO_INIT
    GLES.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, g_prefixsumbuffer);
    CHECK_GL_ERROR_NO_INIT
    GLES.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, g_outputibo);
    CHECK_GL_ERROR_NO_INIT

    // Save states
    GLint prev_program = 0;
    GLES.glGetIntegerv(GL_CURRENT_PROGRAM, &prev_program);
    CHECK_GL_ERROR_NO_INIT
    GLint prev_vb = 0;
    GLES.glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev_vb);
    CHECK_GL_ERROR_NO_INIT

    // Dispatch compute
    LOG_D("Using compute program = %d", g_compute_program)
    GLES.glUseProgram(g_compute_program);
    CHECK_GL_ERROR_NO_INIT
    LOG_D("Dispatch compute")
    GLES.glDispatchCompute((total_indices + 63) / 64, 1, 1);
    CHECK_GL_ERROR_NO_INIT

    // Wait for compute to complete
    LOG_D("memory barrier")
    GLES.glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    CHECK_GL_ERROR_NO_INIT

    // Bind index buffer and do draw
    LOG_D("draw")
    GLES.glUseProgram(prev_program);
    CHECK_GL_ERROR_NO_INIT
    GLES.glBindBuffer(GL_ARRAY_BUFFER, prev_vb);
    CHECK_GL_ERROR_NO_INIT
    GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_outputibo);
    CHECK_GL_ERROR_NO_INIT
    GLES.glDrawElements(mode, total_indices, type, 0);

    // Restore states
    GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    CHECK_GL_ERROR_NO_INIT
}

void mg_glMultiDrawElements_deepseek_one(GLenum mode, const GLsizei *count, 
                                      GLenum type, const void *const *indices, 
                                      GLsizei primcount) {
    LOG();
    prepareForDraw();

    // Process 4 elements at a time using NEON
    GLsizei i = 0;
    for (; i + 3 < primcount; i += 4) {
        // Load 4 counts at once
        int32x4_t counts = vld1q_s32(&count[i]);
        
        // Create mask for non-zero counts (0xFFFFFFFF for true, 0 for false)
        uint32x4_t mask = vcgtq_s32(counts, vdupq_n_s32(0));
        
        // Check if any of the 4 counts are > 0
        if (vmaxvq_u32(mask) != 0) {
            // Process each of the 4 elements
            for (int j = 0; j < 4; j++) {
                if (count[i + j] > 0) {
                    GLES.glDrawElements(mode, count[i + j], type, indices[i + j]);
                }
            }
        }
    }
    
    // Process remaining elements
    for (; i < primcount; ++i) {
        const GLsizei c = count[i];
        if (c > 0) {
            GLES.glDrawElements(mode, c, type, indices[i]);
        }
    }
    
    CHECK_GL_ERROR
}

void mg_glMultiDrawElementsBaseVertex_deepseek_one(GLenum mode, GLsizei* counts, 
                                                GLenum type, const void* const* indices, 
                                                GLsizei primcount, const GLint* basevertex) {
    LOG()

	prepareForDraw();

    // Process 4 elements at a time using NEON
    GLsizei i = 0;
    for (; i + 3 < primcount; i += 4) {
        // Load 4 counts at once
        int32x4_t count_vec = vld1q_s32((const int32_t*)&counts[i]);
        
        // Create mask for counts > 0
        uint32x4_t mask = vcgtq_s32(count_vec, vdupq_n_s32(0));
        
        // Check each element of the mask
        if (vgetq_lane_u32(mask, 0)) {
            GLES.glDrawElementsBaseVertex(mode, counts[i], type, indices[i], basevertex[i]);
        }
        if (vgetq_lane_u32(mask, 1)) {
            GLES.glDrawElementsBaseVertex(mode, counts[i+1], type, indices[i+1], basevertex[i+1]);
        }
        if (vgetq_lane_u32(mask, 2)) {
            GLES.glDrawElementsBaseVertex(mode, counts[i+2], type, indices[i+2], basevertex[i+2]);
        }
        if (vgetq_lane_u32(mask, 3)) {
            GLES.glDrawElementsBaseVertex(mode, counts[i+3], type, indices[i+3], basevertex[i+3]);
        }
    }

    // Process remaining elements
    for (; i < primcount; ++i) {
        const GLsizei count = counts[i];
        if (count > 0) {
            LOG_D("GLES.glDrawElementsBaseVertex, mode = %s, count = %d, type = %s, indices[i] = 0x%x, basevertex[i] = %d",
                 glEnumToString(mode), count, glEnumToString(type), indices[i], basevertex[i]);
            GLES.glDrawElementsBaseVertex(mode, count, type, indices[i], basevertex[i]);
        }
    }

    CHECK_GL_ERROR
}

void mg_glMultiDrawElementsIndirect_deepseek_one(GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride) {
    LOG()

    prepareForDraw();

    // 保存当前绑定的间接绘制缓冲区
    GLuint prevIndirectBuffer = 0;
    GLES.glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING, (GLint*)&prevIndirectBuffer);
    
    // 如果没有绑定缓冲区，使用客户端内存模式
    if (prevIndirectBuffer == 0 && indirect) {
        // 使用现有的缓冲区管理代码
        if (!g_indirect_cmds_inited) {
            GLES.glGenBuffers(1, &g_indirectbuffer);
            g_indirect_cmds_inited = true;
        }
        
        // 绑定我们的间接缓冲区
        GLES.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectbuffer);
        
        // 确保缓冲区足够大
        size_t requiredSize = drawcount * (stride ? stride : sizeof(draw_elements_indirect_command_t));
        if (g_cmdbufsize * sizeof(draw_elements_indirect_command_t) < requiredSize) {
            size_t newSize = g_cmdbufsize;
            while (newSize * sizeof(draw_elements_indirect_command_t) < requiredSize) {
                newSize *= 2;
            }
            
            GLES.glBufferData(GL_DRAW_INDIRECT_BUFFER, 
                            newSize * sizeof(draw_elements_indirect_command_t),
                            NULL, GL_DYNAMIC_DRAW);
            g_cmdbufsize = newSize;
        }
        
        // 上传数据到缓冲区
        GLES.glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, requiredSize, indirect);
    }
    
    const GLubyte *ptr = (const GLubyte *)indirect;
    
    // 计算实际步长
    size_t effectiveStride = stride;
    if (stride == 0) {
        effectiveStride = sizeof(draw_elements_indirect_command_t);
    }
    
    // 执行间接绘制
    for (GLsizei i = 0; i < drawcount; i++) {
        const void *currentIndirect = ptr + i * effectiveStride;
        GLES.glDrawElementsIndirect(mode, type, currentIndirect);
    }
    
    // 恢复之前绑定的缓冲区
    if (prevIndirectBuffer == 0 && indirect) {
        GLES.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, prevIndirectBuffer);
    }
    
    CHECK_GL_ERROR
}

void mg_glMultiDrawElements_deepseek_two(GLenum mode,
                      const GLsizei *counts,
                      GLenum type,
                      const GLvoid *const *indices,
                      GLsizei drawcount)
{

	LOG()

    prepareForDraw();

    // Fallback to emulated implementation
    GLint currentProgram = 0;
    GLboolean hasDrawID = GL_FALSE;
    GLint drawIDLoc = GLES.glGetUniformLocation(currentProgram, "drawID");

    // Get current program and check for drawID uniform
    GLES.glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != 0)
    {
        hasDrawID = (drawIDLoc != -1);
    }

    if (hasDrawID)
    {
        for (GLsizei drawID = 0; drawID < drawcount; ++drawID)
        {
            if (counts[drawID] <= 0)
            {
                continue;
            }
            GLES.glUniform1i(drawIDLoc, drawID);
            GLES.glDrawElements(mode, counts[drawID], type, indices[drawID]);
        }
    }
    else
    {
        for (GLsizei drawID = 0; drawID < drawcount; ++drawID)
        {
            if (counts[drawID] <= 0)
            {
                continue;
            }
            GLES.glDrawElements(mode, counts[drawID], type, indices[drawID]);
        }
    }

}

void mg_glMultiDrawElementsBaseVertex_deepseek_two(GLenum mode,
                                const GLsizei *counts,
                                GLenum type,
                                const GLvoid *const *indices,
                                const GLint *baseVertices,
                                GLsizei drawcount)
{

	LOG()

    prepareForDraw();

    GLint currentProgram = 0;
    GLboolean hasDrawID = GL_FALSE;
    GLint drawIDLoc = -1;
    
    // Get current program and check for drawID uniform
    GLES.glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != 0)
    {
        drawIDLoc = GLES.glGetUniformLocation(currentProgram, "drawID");
        hasDrawID = (drawIDLoc != -1);
    }

    if (hasDrawID)
    {
        for (GLsizei drawID = 0; drawID < drawcount; ++drawID)
        {
            if (counts[drawID] <= 0)
            {
                continue;
            }
            GLES.glUniform1i(drawIDLoc, drawID);
            GLES.glDrawElementsBaseVertex(mode, counts[drawID], type, indices[drawID], baseVertices[drawID]);
        }
    }
    else
    {
        for (GLsizei drawID = 0; drawID < drawcount; ++drawID)
        {
            if (counts[drawID] <= 0)
            {
                continue;
            }
            GLES.glDrawElementsBaseVertex(mode, counts[drawID], type, indices[drawID], baseVertices[drawID]);
        }
    }

}
