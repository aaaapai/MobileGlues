//
// Created by Swung 0x48 on 2025-04-08.
//

#ifndef MOBILEGLUES_VERTEXPOINTER_UTILS_H
#define MOBILEGLUES_VERTEXPOINTER_UTILS_H

#include "GL/gl.h"
#include "../gl/log.h"

int vp2idx(GLenum vp);
uint32_t vp_mask(GLenum vp);
GLenum idx2vp(int idx);

#endif //MOBILEGLUES_VERTEXPOINTER_UTILS_H
