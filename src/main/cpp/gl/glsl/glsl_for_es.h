#ifndef GLSL_FOR_ES
#define GLSL_FOR_ES
#include <stdio.h>
#include "../includes.h"
#include "GL/gl.h"
#include "GL/glcorearb.h"
#include "../log.h"
#include "../gles/loader.h"
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
#include "../mg.h"
#ifdef __cplusplus
}
#endif

std::string getCachedESSL(const char* glsl_code, uint essl_version);
std::string GLSLtoGLSLES(const char *glsl_code, GLenum glsl_type, uint esversion, uint glsl_version);
std::string GLSLtoGLSLES_1(const char *glsl_code, GLenum glsl_type, uint esversion, int& return_code);
std::string GLSLtoGLSLES_2(const char *glsl_code, GLenum glsl_type, uint essl_version, int& return_code);
int getGLSLVersion(const char* glsl_code);

extern char* (*MesaConvertShader)(const char *src, GLenum type, unsigned int glsl, unsigned int essl);

#endif
