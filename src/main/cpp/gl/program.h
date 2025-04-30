//
// Created by hanji on 2025/2/3.A
//

#ifndef MOBILEGLUES_PROGRAM_H
#define MOBILEGLUES_PROGRAM_H

#include "GL/gl.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI GLAPIENTRY void glBindFragDataLocation(GLuint program, GLuint color, const GLchar *name);

GLAPI GLAPIENTRY void glLinkProgram(GLuint program);

GLAPI GLAPIENTRY void glGetProgramiv(GLuint program, GLenum pname, GLint *params);

//GLAPI GLAPIENTRY GLint glGetUniformLocation(GLuint program, const GLchar *name);

#ifdef __cplusplus
}
#endif


#endif //MOBILEGLUES_PROGRAM_H
