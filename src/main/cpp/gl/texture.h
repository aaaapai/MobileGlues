//
// Created by BZLZHH on 2025/1/27.
//

#ifndef MOBILEGLUES_TEXTURE_H
#define MOBILEGLUES_TEXTURE_H

#include "GL/gl.h"
#include "ankerl/unordered_dense.h"

template<typename T>
void readDataComponents(const void* data, GLenum type, T* out, size_t maxComponents);

#ifdef __cplusplus
extern "C" {
#endif

struct texture_t {
    GLenum target;
    GLuint texture;
    GLenum internal_format;
    GLenum format;
    GLint swizzle_param[4];
};

void internal_convert(GLenum* internal_format, GLenum* type, GLenum* format);
int nlevel(int size, int level);
static int is_depth_format(GLenum format);
static GLenum get_binding_for_target(GLenum target);


GLAPI GLAPIENTRY void glGenTextures( GLsizei n, GLuint *textures );
GLAPI GLAPIENTRY void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
GLAPI GLAPIENTRY void glTexParameteri(GLenum target, GLenum pname, GLint param);
GLAPI GLAPIENTRY void glTexImage1D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
GLAPI GLAPIENTRY void glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
GLAPI GLAPIENTRY void glTexImage3D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
GLAPI GLAPIENTRY void glTexStorage1D(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
GLAPI GLAPIENTRY void glTexStorage2D(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
GLAPI GLAPIENTRY void glTexStorage3D(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);
GLAPI GLAPIENTRY void glCopyTexImage1D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border);
GLAPI GLAPIENTRY void glCopyTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
GLAPI GLAPIENTRY void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI GLAPIENTRY void glRenderbufferStorage(GLenum target, GLenum internalFormat, GLsizei width, GLsizei height);
GLAPI GLAPIENTRY void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height);
GLAPI GLAPIENTRY void glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params);
GLAPI GLAPIENTRY void glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params);
GLAPI GLAPIENTRY void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
GLAPI GLAPIENTRY void glTexParameteriv(GLenum target, GLenum pname, const GLint* params);
GLAPI GLAPIENTRY void glGenerateTextureMipmap(GLuint texture);
GLAPI GLAPIENTRY void glBindTexture(GLenum target, GLuint texture);
GLAPI GLAPIENTRY void glDeleteTextures(GLsizei n, const GLuint *textures);
GLAPI GLAPIENTRY void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void* pixels);
GLAPI GLAPIENTRY void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
GLAPI GLAPIENTRY void glTexParameteri(GLenum target, GLenum pname, GLint param);
GLAPI GLAPIENTRY void glClearTexImage(GLuint texture, GLint level, GLenum format, GLenum type, const void* data);
GLAPI GLAPIENTRY void glPixelStorei(GLenum pname, GLint param);
GLAPI GLAPIENTRY void glGetCompressedTexImage(GLenum target, GLint level, void* pixels);
GLAPI GLAPIENTRY void glGetnCompressedTexImage(GLenum target, GLint level, GLsizei bufSize, void* pixels);

#ifdef __cplusplus
}
#endif

#endif
