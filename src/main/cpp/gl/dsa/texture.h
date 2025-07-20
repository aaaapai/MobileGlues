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

GLAPI GLAPIENTRY void glClearTexSubImage(GLuint texture, GLint level, GLint xoffset, 
                       GLint yoffset, GLint zoffset, GLsizei width, 
                       GLsizei height, GLsizei depth, GLenum format, 
                       GLenum type, const void *data);
GLAPI GLAPIENTRY void glGetTextureParameterIuiv(GLuint texture, GLenum pname, GLuint *params);
GLAPI GLAPIENTRY void glGetTextureParameterIiv(GLuint texture, GLenum pname, GLint *params);
GLAPI GLAPIENTRY void glGetTextureParameterfv(GLuint texture, GLenum pname, GLfloat *params);
GLAPI GLAPIENTRY void glGetTextureParameteriv(GLuint texture, GLenum pname, GLint *params);
GLAPI GLAPIENTRY void glGetTextureLevelParameterfv(GLuint texture, GLint level, 
                                 GLenum pname, GLfloat *params);
GLAPI GLAPIENTRY void glGetTextureLevelParameteriv(GLuint texture, GLint level, 
                                 GLenum pname, GLint *params);
GLAPI GLAPIENTRY void glGetCompressedTextureImage(GLuint texture, GLint level, 
                                GLsizei bufSize, void *pixels);
GLAPI GLAPIENTRY void glGetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, 
                      GLsizei bufSize, void *pixels);
GLAPI GLAPIENTRY void glCompressedTextureSubImage1D(GLuint texture, GLint level,
                                  GLint xoffset, GLsizei width,
                                  GLenum format, GLsizei imageSize, const void *data);
GLAPI GLAPIENTRY void glCompressedTextureSubImage2D(GLuint texture, GLint level,
                                  GLint xoffset, GLint yoffset,
                                  GLsizei width, GLsizei height,
                                  GLenum format, GLsizei imageSize, const void *data);
GLAPI GLAPIENTRY void glCompressedTextureSubImage3D(GLuint texture, GLint level, 
                                  GLint xoffset, GLint yoffset, GLint zoffset,
                                  GLsizei width, GLsizei height, GLsizei depth,
                                  GLenum format, GLsizei imageSize, const void *data);
GLAPI GLAPIENTRY void glTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, 
                        GLsizei width, GLsizei height, GLsizei depth, 
                        GLenum format, GLenum type, const void *pixels);
GLAPI GLAPIENTRY void glTextureStorage3DMultisample(
    GLuint texture, GLsizei samples, GLenum internalformat,
    GLsizei width, GLsizei height, GLsizei depth,
    GLboolean fixedsamplelocations);
GLAPI GLAPIENTRY void glTextureStorage2DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, 
                                  GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
GLAPI GLAPIENTRY void glTextureStorage3D(GLuint texture, GLsizei levels, GLenum internalformat, 
                        GLsizei width, GLsizei height, GLsizei depth);
GLAPI GLAPIENTRY void glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, 
                        GLsizei width, GLsizei height);

#ifdef __cplusplus
}
#endif


#endif //MOBILEGLUES_DSA_TEXTURE_H
