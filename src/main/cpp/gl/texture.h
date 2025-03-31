//
// Created by BZLZHH on 2025/1/27.
//

#ifndef MOBILEGLUES_TEXTURE_H
#define MOBILEGLUES_TEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "glcorearb.h"

struct texture_t {
    GLenum target;
    GLuint texture;
    GLenum internal_format;
    GLenum format;
    GLint swizzle_param[4];
};

GLAPI APIENTRY void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
GLAPI APIENTRY void glTexParameteri(GLenum target, GLenum pname, GLint param);
GLAPI APIENTRY void glTexImage1D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
GLAPI APIENTRY void glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
GLAPI APIENTRY void glTexImage3D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
GLAPI APIENTRY void glTexStorage1D(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
GLAPI APIENTRY void glTexStorage2D(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
GLAPI APIENTRY void glTexStorage3D(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);
GLAPI APIENTRY void glCopyTexImage1D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border);
GLAPI APIENTRY void glCopyTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
GLAPI APIENTRY void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI APIENTRY void glRenderbufferStorage(GLenum target, GLenum internalFormat, GLsizei width, GLsizei height);
GLAPI APIENTRY void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height);
GLAPI APIENTRY void glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params);
GLAPI APIENTRY void glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params);
GLAPI APIENTRY void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
GLAPI APIENTRY void glTexParameteriv(GLenum target, GLenum pname, const GLint* params);
GLAPI APIENTRY void glGenerateTextureMipmap(GLuint texture);
GLAPI APIENTRY void glBindTexture(GLenum target, GLuint texture);
GLAPI APIENTRY void glDeleteTextures(GLsizei n, const GLuint *textures);
GLAPI APIENTRY void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void* pixels);
GLAPI APIENTRY void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
GLAPI APIENTRY void glTexParameteri(GLenum target, GLenum pname, GLint param);
GLAPI APIENTRY void glClearTexImage(GLuint texture, GLint level, GLenum format, GLenum type, const void* data);
GLAPI APIENTRY void glPixelStorei(GLenum pname, GLint param);

#ifdef __cplusplus
}
#endif

#endif
