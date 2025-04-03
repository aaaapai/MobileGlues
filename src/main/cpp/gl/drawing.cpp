//
// Created by BZLZHH on 2025/1/29.
//

#include "drawing.h"
#include "buffer.h"
#include "framebuffer.h"

#define DEBUG 0

static bool g_indirect_cmds_inited = false;
static GLsizei g_cmdbufsize = 0;
GLuint g_indirectbuffer = 0;

#define DRAW_INDIRECT

void glMultiDrawElementsBaseVertex(GLenum mode,
                                   const GLsizei *count,
                                   GLenum type,
                                   const GLvoid *const *indices,
                                   GLsizei drawcount,
                                   const GLint *basevertex) {
    LOG_D("glMultiDrawElementsBaseVertex, mode: %s, count: %d, type: %s, drawcount: %d, basevertex[0]: %d",
          glEnumToString(mode), count, glEnumToString(type), drawcount, basevertex[0])

    if (hardware->es_version < 310) {
        GLint prevElementBuffer;
        GLES.glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &prevElementBuffer);

        for (GLsizei i = 0; i < drawcount; ++i) {
            if (count[i] <= 0) continue;

            GLsizei currentCount = count[i];
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
        return;
    }
    
#ifdef DRAW_INDIRECT

    LOG_D("DRAW_INDIRECT")
    if (!g_indirect_cmds_inited) {
        LOG_D("DRAW_INDIRECT init")
        GLES.glGenBuffers(1, &g_indirectbuffer);
        GLES.glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectbuffer);
        g_cmdbufsize = 1;
        GLES.glBufferData(GL_DRAW_INDIRECT_BUFFER,
                          g_cmdbufsize * sizeof(draw_elements_indirect_command_t), NULL, GL_DYNAMIC_DRAW);

        g_indirect_cmds_inited = true;
        LOG_D("DRAW_INDIRECT init end")
    }

    if (g_cmdbufsize < drawcount || drawcount <= 0) {
        size_t sz = g_cmdbufsize;

        LOG_D("Before resize: %d", sz)

        // 2-exponential to reduce reallocation
        while (sz < drawcount)
            sz *= 2;

        GLES.glBufferData(GL_DRAW_INDIRECT_BUFFER,
                          sz * sizeof(draw_elements_indirect_command_t), NULL, GL_DYNAMIC_DRAW);
        g_cmdbufsize = sz;
    }

    LOG_D("After resize: %d", g_cmdbufsize)

    LOG_D("GLES.glMapBufferRange")
    //auto* pcmds = (draw_elements_indirect_command_t*)
    //        GLES.glMapBufferRange(GL_DRAW_INDIRECT_BUFFER,
    //                                    0, drawcount * sizeof(draw_elements_indirect_command_t),
    //                              GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);

    auto* pcmds = (draw_elements_indirect_command_t*)
            GLES.glMapBufferOES(GL_DRAW_INDIRECT_BUFFER, GL_WRITE_ONLY);

    GLenum ERR;
    while ((ERR = GLES.glGetError()) != GL_NO_ERROR) {
        LOG_E("err: %s", glEnumToString(ERR))
    }

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

    LOG_D("pcmds, %p", pcmds)
    for (GLsizei i = 0; i < drawcount; ++i) {
        LOG_D("pcmds 1")
        auto byteOffset = reinterpret_cast<uintptr_t>(indices[i]);
        LOG_D("pcmds 2")
        pcmds[i].firstIndex = static_cast<GLuint>(byteOffset / elementSize);
        LOG_D("pcmds 3")
        pcmds[i].count = count[i];
        LOG_D("pcmds 4")
        pcmds[i].instanceCount = 1;
        LOG_D("pcmds 5")
        pcmds[i].baseVertex = basevertex[i];
        LOG_D("pcmds 6")
        pcmds[i].reservedMustBeZero = 0;
        LOG_D("pcmds 7")
    }
    if(!pcmds) {
        GLenum err = GLES.glGetError();
        LOG_E("glMapBufferRange failed! Error: 0x%X", err);
        return;
    }
    
    LOG_D("GLES.glUnmapBuffer")
    GLES.glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);

    // Draw indirect!
    for (GLsizei i = 0; i < drawcount; ++i) {
        LOG_D("for...")
        const GLvoid* offset = reinterpret_cast<GLvoid*>(i * sizeof(draw_elements_indirect_command_t));
        LOG_D("GLES.glDrawElementsIndirect")
        GLES.glDrawElementsIndirect(mode, type, offset);
        LOG_D("GLES.glDrawElementsIndirect end")
    }


#else

    for (GLsizei i = 0; i < primcount; ++i) {
        const GLsizei count = counts[i];
        if (count > 0) {
            LOG_D("GLES.glDrawElementsBaseVertex, mode = %s, count = %d, type = %s, indices[i] = 0x%x, basevertex[i] = %d",
                  glEnumToString(mode), count, glEnumToString(type), indices[i], basevertex[i])
            GLES.glDrawElementsBaseVertex(mode, count, type, indices[i], basevertex[i]);
        }
    }
#endif

    CHECK_GL_ERROR_NO_INIT
}

void glMultiDrawElements(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei primcount) {
    LOG()

#ifdef DRAW_INDIRECT
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

#else
    
    for (GLsizei i = 0; i < primcount; ++i) {
        const GLsizei c = count[i];
        if (c > 0) {
            GLES.glDrawElements(mode, c, type, indices[i]);
        }
    }
    
#endif
}

// solve the crash error for ANGLE, but it will make Derivative Main with Optifine not work!

//_Thread_local static bool unexpected_error = false; 

void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) {
    LOG_D("glDrawElements, mode: %d, count: %d, type: %d, indices: %p", mode, count, type, indices)
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