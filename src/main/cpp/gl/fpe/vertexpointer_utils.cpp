//
// Created by Swung 0x48 on 2025-04-08.
//

#include "vertexpointer_utils.h"
#include "fpe.hpp"

#define DEBUG 0

int vp2idx(GLenum vp) {
    switch (vp) {
        case GL_VERTEX_ARRAY:
            return 0;
        case GL_NORMAL_ARRAY:
            return 1;
        case GL_COLOR_ARRAY:
            return 2;
        case GL_INDEX_ARRAY:
            return 3;
        case GL_EDGE_FLAG_ARRAY:
            return 4;
        case GL_FOG_COORD_ARRAY:
            return 5;
        case GL_SECONDARY_COLOR_ARRAY:
            return 6;
        case GL_TEXTURE_COORD_ARRAY:
            return 7 + (g_glstate.fpe_state.client_active_texture - GL_TEXTURE0);
    }
    LOG_E("ERROR: 1280")
    return -1;
}

uint32_t vp_mask(GLenum vp) {
    switch (vp) {
        case GL_VERTEX_ARRAY:
        case GL_NORMAL_ARRAY:
        case GL_COLOR_ARRAY:
        case GL_INDEX_ARRAY:
        case GL_TEXTURE_COORD_ARRAY:
        case GL_EDGE_FLAG_ARRAY:
        case GL_FOG_COORD_ARRAY:
        case GL_SECONDARY_COLOR_ARRAY:
            return (1 << vp2idx(vp));
    }
    LOG_E("ERROR: 1280")
    return 0;
}

GLenum idx2vp(int idx) {
    switch (idx) {
        case 0:
            return GL_VERTEX_ARRAY;
        case 1:
            return GL_NORMAL_ARRAY;
        case 2:
            return GL_COLOR_ARRAY;
        case 3:
            return GL_INDEX_ARRAY;
        case 4:
            return GL_TEXTURE_COORD_ARRAY;
        case 5:
            return GL_EDGE_FLAG_ARRAY;
        case 6:
            return GL_FOG_COORD_ARRAY;
        case 7:
            return GL_SECONDARY_COLOR_ARRAY;
    }
    LOG_E("ERROR: 1280")

    return 0;
}
