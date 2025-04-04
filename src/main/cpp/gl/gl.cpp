//
// Created by Swung0x48 on 2024/10/8.
//

#include "../includes.h"
#include "gl.h"
#include "log.h"
#include "../gles/loader.h"
#include "mg.h"

#define DEBUG 0

GLAPI void GLAPIENTRY glClearDepth(GLclampd depth) {
    LOG()
    GLES.glClearDepthf((GLfloat)depth);
    CHECK_GL_ERROR
}

GLAPI void GLAPIENTRY glHint(GLenum target, GLenum mode) {
    LOG()
    LOG_D("glHint, target = %s, mode = %s", glEnumToString(target), glEnumToString(mode))
}

GLAPI void GLAPIENTRY glPolygonMode(GLenum face, GLenum mode) {
	if (face == GL_FRONT)
		face = GL_FRONT_AND_BACK;   //TODO, better handle all this
	if (face == GL_BACK)
		return;		//TODO, handle face enum for polygon mode != GL_FILL
	switch(mode) {
		case GL_LINE:
		case GL_POINT:
			gl_state->polygon_mode = mode;
			break;
		case GL_FILL:
			gl_state->polygon_mode = 0;
			break;
		default:
			gl_state->polygon_mode = 0;
	}
}
