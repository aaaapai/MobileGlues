//
// Created by Swung 0x48 on 2025/2/19.
//

#ifndef MOBILEGLUES_VERTEXATTRIB_H
#define MOBILEGLUES_VERTEXATTRIB_H

#include "../includes.h"
#include "glcorearb.h"
#include "log.h"
#include "../gles/loader.h"
#include "mg.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI APIENTRY void glVertexAttribI1ui(GLuint index, GLuint x);
GLAPI APIENTRY void glVertexAttribI2ui(GLuint index, GLuint x, GLuint y);
GLAPI APIENTRY void glVertexAttribI3ui(GLuint index, GLuint x, GLuint y, GLuint z);

#ifdef __cplusplus
}
#endif

#endif //MOBILEGLUES_VERTEXATTRIB_H
