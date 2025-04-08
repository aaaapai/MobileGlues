//
// Created by BZLZHH on 2025/1/29.
//

#include "drawing.h"
#include "buffer.h"
#include "framebuffer.h"
#include "fpe/fpe.hpp"

#define DEBUG 0

static bool g_indirect_cmds_inited = false;
static GLsizei g_cmdbufsize = 0;
GLuint g_indirectbuffer = 0;

void prepare_indirect_buffer(const GLsizei *counts, GLenum type, const void *const *indices,
                             GLsizei primcount, const GLint *basevertex) {
    if (!g_indirect_cmds_inited) {
        GLES.glGenBuffers(1, &g_indirectbuffer);
        GLES.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectbuffer);
        g_cmdbufsize = 1;
        GLES.glBufferData(GL_DRAW_INDIRECT_BUFFER,
                          g_cmdbufsize * sizeof(draw_elements_indirect_command_t), NULL, GL_DYNAMIC_DRAW);

        g_indirect_cmds_inited = true;
    }

    if (g_cmdbufsize < primcount) {
        size_t sz = g_cmdbufsize;

        LOG_D("Before resize: %d", sz)

        // 2-exponential to reduce reallocation
        while (sz < primcount)
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
        pcmds[i].baseVertex = basevertex[i];
        pcmds[i].reservedMustBeZero = 0;
    }

    GLES.glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
}

void mg_glMultiDrawElementsBaseVertex_drawelements(GLenum mode, GLsizei* counts, GLenum type, const void* const* indices, GLsizei primcount, const GLint* basevertex) {
    LOG()
    GET_GL4ES_FUNC(void, mg_glMultiDrawElementsBaseVertex_drawelements, GLenum mode, GLsizei* counts, GLenum type, const void* const* indices, GLsizei primcount, const GLint* basevertex)
    CALL_GL4ES_FUNC(mg_glMultiDrawElementsBaseVertex_drawelements, mode, counts, type, indices, primcount, basevertex)

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
    GET_GL4ES_FUNC(void, mg_glMultiDrawElementsBaseVertex_indirect, GLenum mode, GLsizei* counts, GLenum type, const void* const* indices, GLsizei primcount, const GLint* basevertex)
    CALL_GL4ES_FUNC(mg_glMultiDrawElementsBaseVertex_indirect, mode, counts, type, indices, primcount, basevertex)

    prepare_indirect_buffer(counts, type, indices, primcount, basevertex);

    // Draw indirect!
    for (GLsizei i = 0; i < primcount; ++i) {
        const GLvoid* offset = reinterpret_cast<GLvoid*>(i * sizeof(draw_elements_indirect_command_t));
        GLES.glDrawElementsIndirect(mode, type, offset);
    }

    CHECK_GL_ERROR
}

void mg_glMultiDrawElementsBaseVertex_multiindirect(GLenum mode, GLsizei* counts, GLenum type, const void* const* indices, GLsizei primcount, const GLint* basevertex) {
    LOG()
    GET_GL4ES_FUNC(void, mg_glMultiDrawElementsBaseVertex_multiindirect, GLenum mode, GLsizei* counts, GLenum type, const void* const* indices, GLsizei primcount, const GLint* basevertex)
    CALL_GL4ES_FUNC(mg_glMultiDrawElementsBaseVertex_multiindirect, mode, counts, type, indices, primcount, basevertex)

    prepare_indirect_buffer(counts, type, indices, primcount, basevertex);

    // Multi-draw indirect!
    GLES.glMultiDrawElementsIndirectEXT(mode, type, 0, primcount, 0);

    CHECK_GL_ERROR
}

void mg_glMultiDrawElementsBaseVertex_basevertex(GLenum mode, GLsizei* counts, GLenum type, const void* const* indices, GLsizei primcount, const GLint* basevertex) {
    LOG()
    GET_GL4ES_FUNC(void, mg_glMultiDrawElementsBaseVertex_basevertex, GLenum mode, GLsizei* counts, GLenum type, const void* const* indices, GLsizei primcount, const GLint* basevertex)
    CALL_GL4ES_FUNC(mg_glMultiDrawElementsBaseVertex_basevertex, mode, counts, type, indices, primcount, basevertex)

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
    GET_GL4ES_FUNC(void, mg_glMultiDrawElements_indirect, GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount)
    CALL_GL4ES_FUNC(mg_glMultiDrawElements_indirect, mode, count, type, indices, primcount)

    prepare_indirect_buffer(count, type, indices, primcount, 0);
    // Draw indirect!
    for (GLsizei i = 0; i < primcount; ++i) {
        const GLvoid* offset = reinterpret_cast<GLvoid*>(i * sizeof(draw_elements_indirect_command_t));
        GLES.glDrawElementsIndirect(mode, type, offset);
    }
    CHECK_GL_ERROR
}

void mg_glMultiDrawElements_drawelements(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount) {
    LOG()
    GET_GL4ES_FUNC(void, mg_glMultiDrawElements_drawelements, GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount)
    CALL_GL4ES_FUNC(mg_glMultiDrawElements_drawelements, mode, count, type, indices, primcount)

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
    GET_GL4ES_FUNC(void, mg_glMultiDrawElements_multiindirect, GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount)
    CALL_GL4ES_FUNC(mg_glMultiDrawElements_multiindirect, mode, count, type, indices, primcount)

    prepare_indirect_buffer(count, type, indices, primcount, 0);

    // Multi-draw indirect!
    GLES.glMultiDrawElementsIndirectEXT(mode, type, 0, primcount, 0);

    CHECK_GL_ERROR
}

void mg_glMultiDrawElements_basevertex(GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount) {
    LOG()
    GET_GL4ES_FUNC(void, mg_glMultiDrawElements_basevertex, GLenum mode, const GLsizei *count, GLenum type, const void *const *indices, GLsizei primcount)
    CALL_GL4ES_FUNC(mg_glMultiDrawElements_basevertex, mode, count, type, indices, primcount)

    for (GLsizei i = 0; i < primcount; ++i) {
        const GLsizei c = count[i];
        if (c > 0) {
            GLES.glDrawElements(mode, c, type, indices[i]);
        }
    }

    CHECK_GL_ERROR
}

void glMultiDrawElementsBaseVertex(GLenum mode, GLsizei* counts, GLenum type, const void* const* indices, GLsizei primcount, const GLint* basevertex) {
    LOG()
    GET_GL4ES_FUNC(void, glMultiDrawElementsBaseVertex, GLenum mode, GLsizei* counts, GLenum type, const void* const* indices, GLsizei primcount, const GLint* basevertex)
    CALL_GL4ES_FUNC(glMultiDrawElementsBaseVertex, mode, counts, type, indices, primcount, basevertex)

    if (!g_indirect_cmds_inited) {
        GLES.glGenBuffers(1, &g_indirectbuffer);
        GLES.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectbuffer);
        g_cmdbufsize = 1;
        GLES.glBufferData(GL_DRAW_INDIRECT_BUFFER,
                          g_cmdbufsize * sizeof(draw_elements_indirect_command_t), NULL, GL_DYNAMIC_DRAW);

        g_indirect_cmds_inited = true;
    }

    if (g_cmdbufsize < primcount) {
        size_t sz = g_cmdbufsize;

        LOG_D("Before resize: %d", sz)

        // 2-exponential to reduce reallocation
        while (sz < primcount)
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
        pcmds[i].baseVertex = basevertex[i];
        pcmds[i].reservedMustBeZero = 0;
    }

    GLES.glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);

    // Draw indirect!
    for (GLsizei i = 0; i < primcount; ++i) {
        const GLvoid* offset = reinterpret_cast<GLvoid*>(i * sizeof(draw_elements_indirect_command_t));
        GLES.glDrawElementsIndirect(mode, type, offset);
    }

    CHECK_GL_ERROR
}

void glDrawArrays(GLenum mode, GLint first, GLsizei count) {
    LOG()
    
    GET_GL4ES_FUNC(void, glDrawArrays, GLenum mode, GLint first, GLsizei count)
    CALL_GL4ES_FUNC(glDrawArrays, mode, first, count)
    LOG_D("glDrawArrays, mode: %d, first: %d, count: %d", mode, first, count)
    switch (mode) {
        case GL_TRIANGLES:
        case GL_TRIANGLE_STRIP:
        case GL_TRIANGLE_FAN:
        case GL_POINTS:
        case GL_LINES:
        case GL_LINE_STRIP:
        case GL_LINE_LOOP:
            GLES.glDrawArrays(mode, first, count);
        default:
            LOG_D("Unknown mode, only do glDrawArrays in gl4es")
    }
    CHECK_GL_ERROR
}

void glMultiDrawElements(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei primcount) {
    LOG()
    GET_GL4ES_FUNC(void, glMultiDrawElements, GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei primcount)
    CALL_GL4ES_FUNC(glMultiDrawElements, mode, count, type, indices, primcount)

    if (!g_indirect_cmds_inited) {
        GLES.glGenBuffers(1, &g_indirectbuffer);
        GLES.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectbuffer);
        g_cmdbufsize = 1;
        GLES.glBufferData(GL_DRAW_INDIRECT_BUFFER,
                          g_cmdbufsize * sizeof(draw_elements_indirect_command_t), NULL, GL_DYNAMIC_DRAW);

        g_indirect_cmds_inited = true;
    }

    if (g_cmdbufsize < primcount) {
        size_t sz = g_cmdbufsize;

        LOG_D("Before resize: %d", sz)

        // 2-exponential to reduce reallocation
        while (sz < primcount)
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
        pcmds[i].count = count[i];
        pcmds[i].instanceCount = 1;
        pcmds[i].baseVertex = 0;
        pcmds[i].reservedMustBeZero = 0;
    }

    GLES.glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);

    // Draw indirect!
    for (GLsizei i = 0; i < primcount; ++i) {
        const GLvoid* offset = reinterpret_cast<GLvoid*>(i * sizeof(draw_elements_indirect_command_t));
        GLES.glDrawElementsIndirect(mode, type, offset);
    }
}

///*_Thread_local*/ static bool unexpected_error = false; // solve the crash error for ANGLE
// Why thread local here? We've never PRETEND we are thread safe.

// solve the crash error for ANGLE, but it will make Derivative Main with Optifine not work!

//_Thread_local static bool unexpected_error = false;

void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) {
    LOG()
    GET_GL4ES_FUNC(void, glDrawElements, GLenum mode, GLsizei count, GLenum type, const void* indices)
    CALL_GL4ES_FUNC(glDrawElements, mode, count, type, indices)
    LOG_D("glDrawElements, mode: %d, count: %d, type: %d, indices: %p", mode, count, type, indices)
    GLES.glDrawElements(mode, count, type, indices);
    CHECK_GL_ERROR
}

void glBindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) {
    LOG()
    GET_GL4ES_FUNC(void, glBindImageTexture, GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)
    CALL_GL4ES_FUNC(glBindImageTexture, unit, texture, level, layered, layer, access, format)
    LOG_D("glBindImageTexture, unit: %d, texture: %d, level: %d, layered: %d, layer: %d, access: %d, format: %d",
          unit, texture, level, layered, layer, access, format)
    GLES.glBindImageTexture(unit, texture, level, layered, layer, access, format);
    CHECK_GL_ERROR
}

void glUniform1i(GLint location, GLint v0) {
    LOG()
    GET_GL4ES_FUNC(void, glUniform1i, GLint location, GLint v0)
    CALL_GL4ES_FUNC(glUniform1i, location, v0)
    LOG_D("glUniform1i, location: %d, v0: %d", location, v0)
    GLES.glUniform1i(location, v0);
    CHECK_GL_ERROR
}

void glDispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) {
    LOG()
    GET_GL4ES_FUNC(void, glDispatchCompute, GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z)
    CALL_GL4ES_FUNC(glDispatchCompute, num_groups_x, num_groups_y, num_groups_z)
    LOG_D("glDispatchCompute, num_groups_x: %d, num_groups_y: %d, num_groups_z: %d",
          num_groups_x, num_groups_y, num_groups_z)
    GLES.glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    CHECK_GL_ERROR
}

