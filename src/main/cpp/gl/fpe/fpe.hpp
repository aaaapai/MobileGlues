//
// Created by Swung 0x48 on 2025/2/8.
//

#ifndef MOBILEGLUES_FPE_HPP
#define MOBILEGLUES_FPE_HPP

#include "GL/gl.h"
#include "../log.h"
#include "transformation.h"
#include "state.h"
#include "vertexpointer.h"

#define GET_PREV_PROGRAM GLint m_prev_program; GLES.glGetIntegerv(GL_CURRENT_PROGRAM, &m_prev_program);
#define SET_PREV_PROGRAM if (m_prev_program) GLES.glUseProgram(m_prev_program);

#define g_glstate glstate_t::get_instance()

extern bool fpe_inited;

int init_fpe();

// 0 - keep DrawArray, 1 - switch to DrawElements
int commit_fpe_state_on_draw(GLenum* mode, GLint* first, GLsizei* count);

#endif //MOBILEGLUES_FPE_HPP
