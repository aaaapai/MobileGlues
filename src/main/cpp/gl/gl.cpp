//
// Created by Swung0x48 on 2024/10/8.
//

#include "../includes.h"
#include "gl.h"
#include "log.h"
#include "../gles/loader.h"
#include "mg.h"
#include "gl_mg.h"

#define DEBUG 1

void glClearDepth(GLclampd depth) {
    LOG()
    GET_GL4ES_FUNC(void, glClearDepth, GLclampd depth)
    CALL_GL4ES_FUNC(glClearDepth, depth)
    GLES.glClearDepthf((float)depth);
    CHECK_GL_ERROR
}

void glHint(GLenum target, GLenum mode) {
    LOG()
    GET_GL4ES_FUNC(void, glClearDepth, GLenum target, GLenum mode)
    CALL_GL4ES_FUNC(glClearDepth, target, mode)
    LOG_D("glHint, target = %s, mode = %s", glEnumToString(target), glEnumToString(mode))
}
