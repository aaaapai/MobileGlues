//
// Created by Swung 0x48 on 2025/2/8.
//

#ifndef MOBILEGLUES_FPE_HPP
#define MOBILEGLUES_FPE_HPP

#include "../gl.h"
#include "../log.h"
#include "transformation.h"
#include "state.h"
#include "vertexpointer.h"

#define g_glstate glstate_t::get_instance()

// 0 - keep DrawArray, 1 - switch to DrawElements
int commit_fpe_state_on_draw(GLenum* mode, GLint* first, GLsizei* count);

#endif //MOBILEGLUES_FPE_HPP
