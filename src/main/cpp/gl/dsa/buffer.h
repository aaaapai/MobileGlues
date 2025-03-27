//
// Created by Swung 0x48 on 2025-03-26.
//

#ifndef MOBILEGLUES_DSA_BUFFER_H
#define MOBILEGLUES_DSA_BUFFER_H

#include <vector>

#include "../gl.h"
#include "../buffer.h"
#include "../log.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI GLAPIENTRY void glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);

#ifdef __cplusplus
}
#endif


#endif //MOBILEGLUES_DSA_BUFFER_H
