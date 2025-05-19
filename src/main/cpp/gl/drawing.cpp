//
// Created by BZLZHH on 2025/1/29.
//

#include "drawing.h"
#include "buffer.h"
#include "framebuffer.h"
#include "fpe/fpe.hpp"
#include "fpe/list.h"

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

void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) {
    LOG()
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