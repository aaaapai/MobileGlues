#ifndef MOBILEGLUES_DSA_TEXTURE_H
#define MOBILEGLUES_DSA_TEXTURE_H

#include <vector>

#include "GL/gl.h"
#include "../log.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI void APIENTRY glCreateTextures(GLenum target, GLsizei n, GLuint *textures);

#ifdef __cplusplus
}
#endif


#endif //MOBILEGLUES_DSA_TEXTURE_H
