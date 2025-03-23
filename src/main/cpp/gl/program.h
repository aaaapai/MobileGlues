//
// Created by hanji on 2025/2/3.
//

#ifndef MOBILEGLUES_PROGRAM_H
#define MOBILEGLUES_PROGRAM_H

#include "glcorearb.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI APIENTRY void glBindFragDataLocation(GLuint program, GLuint color, const GLchar *name);

GLAPI APIENTRY void glLinkProgram(GLuint program);

GLAPI APIENTRY void glGetProgramiv(GLuint program, GLenum pname, GLint *params);

#ifdef __cplusplus
}
#endif


#endif //MOBILEGLUES_PROGRAM_H
