#ifndef MOBILEGLUES_DSA_TEXTURE_H
#define MOBILEGLUES_DSA_TEXTURE_H

#include <vector>

#include "GL/gl.h"
#include "../log.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI GLAPIENTRY void glCreateTextures (GLenum target, GLsizei n, GLuint *textures);

GLAPI GLAPIENTRY void glTextureParameteri (GLuint texture, GLenum pname, GLint param);
GLAPI GLAPIENTRY void glTextureParameterIiv (GLuint texture, GLenum pname, const GLint *params);
GLAPI GLAPIENTRY void glTextureParameterIuiv (GLuint texture, GLenum pname, const GLuint *params);

GLAPI GLAPIENTRY void glBindTextureUnit (GLuint unit, GLuint texture);

GLAPI GLAPIENTRY void glBindSamplers (GLuint first, GLsizei count, const GLuint *samplers);
GLAPI GLAPIENTRY void glCopyTextureSubImage1D (GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
GLAPI GLAPIENTRY void glCopyTextureSubImage2D (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI GLAPIENTRY void glCopyTextureSubImage3D (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI GLAPIENTRY void glTextureParameteriv (GLuint texture, GLenum pname, const GLint *param);

GLAPI GLAPIENTRY void glTextureSubImage2D (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);

#ifdef __cplusplus
}
#endif


#endif //MOBILEGLUES_DSA_TEXTURE_H
