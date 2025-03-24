//
// Created by BZLZHH on 2025/1/28.
//

#include "../includes.h"
#include "glcorearb.h"
#include "log.h"
#include "../gles/loader.h"
#include "mg.h"
#include "../version.h"

#ifndef MOBILEGLUES_GETTER_H
#define MOBILEGLUES_GETTER_H

#ifdef __cplusplus
extern "C" {
#endif

GLAPI APIENTRY const GLubyte *glGetString(GLenum name);
GLAPI APIENTRY const GLubyte *glGetStringi(GLenum name, GLuint index);
GLAPI APIENTRY GLenum glGetError();
GLAPI APIENTRY void glGetIntegerv(GLenum pname, GLint *params);
GLAPI APIENTRY void glGetQueryObjectiv(GLuint id, GLenum pname, GLint* params);
GLAPI APIENTRY void glGetQueryObjecti64v(GLuint id, GLenum pname, GLint64* params);

void AppendExtension(const char* ext);
void InitGLESBaseExtensions();
void set_es_version();

#ifdef __cplusplus
}
#endif

#endif //MOBILEGLUES_GETTER_H
