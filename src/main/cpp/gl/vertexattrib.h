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

GLAPI void APIENTRY glVertexAttrib1d (GLuint index, GLdouble x);
GLAPI void APIENTRY glVertexAttrib1dv (GLuint index, const GLdouble *v);
GLAPI void APIENTRY glVertexAttrib1f (GLuint index, GLfloat x);
GLAPI void APIENTRY glVertexAttrib1fv (GLuint index, const GLfloat *v);
GLAPI void APIENTRY glVertexAttrib1s (GLuint index, GLshort x);
GLAPI void APIENTRY glVertexAttrib1sv (GLuint index, const GLshort *v);
GLAPI void APIENTRY glVertexAttrib2d (GLuint index, GLdouble x, GLdouble y);
GLAPI void APIENTRY glVertexAttrib2dv (GLuint index, const GLdouble *v);
GLAPI void APIENTRY glVertexAttrib2f (GLuint index, GLfloat x, GLfloat y);
GLAPI void APIENTRY glVertexAttrib2fv (GLuint index, const GLfloat *v);
GLAPI void APIENTRY glVertexAttrib2s (GLuint index, GLshort x, GLshort y);
GLAPI void APIENTRY glVertexAttrib2sv (GLuint index, const GLshort *v);
GLAPI void APIENTRY glVertexAttrib3d (GLuint index, GLdouble x, GLdouble y, GLdouble z);
GLAPI void APIENTRY glVertexAttrib3dv (GLuint index, const GLdouble *v);
GLAPI void APIENTRY glVertexAttrib3f (GLuint index, GLfloat x, GLfloat y, GLfloat z);
GLAPI void APIENTRY glVertexAttrib3fv (GLuint index, const GLfloat *v);
GLAPI void APIENTRY glVertexAttrib3s (GLuint index, GLshort x, GLshort y, GLshort z);
GLAPI void APIENTRY glVertexAttrib3sv (GLuint index, const GLshort *v);
GLAPI void APIENTRY glVertexAttrib4Nbv (GLuint index, const GLbyte *v);
GLAPI void APIENTRY glVertexAttrib4Niv (GLuint index, const GLint *v);
GLAPI void APIENTRY glVertexAttrib4Nsv (GLuint index, const GLshort *v);
GLAPI void APIENTRY glVertexAttrib4Nub (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
GLAPI void APIENTRY glVertexAttrib4Nubv (GLuint index, const GLubyte *v);
GLAPI void APIENTRY glVertexAttrib4Nuiv (GLuint index, const GLuint *v);
GLAPI void APIENTRY glVertexAttrib4Nusv (GLuint index, const GLushort *v);
GLAPI void APIENTRY glVertexAttrib4bv (GLuint index, const GLbyte *v);
GLAPI void APIENTRY glVertexAttrib4d (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLAPI void APIENTRY glVertexAttrib4dv (GLuint index, const GLdouble *v);
GLAPI void APIENTRY glVertexAttrib4f (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLAPI void APIENTRY glVertexAttrib4fv (GLuint index, const GLfloat *v);
GLAPI void APIENTRY glVertexAttrib4iv (GLuint index, const GLint *v);
GLAPI void APIENTRY glVertexAttrib4s (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
GLAPI void APIENTRY glVertexAttrib4sv (GLuint index, const GLshort *v);
GLAPI void APIENTRY glVertexAttrib4ubv (GLuint index, const GLubyte *v);
GLAPI void APIENTRY glVertexAttrib4uiv (GLuint index, const GLuint *v);
GLAPI void APIENTRY glVertexAttrib4usv (GLuint index, const GLushort *v);
GLAPI void APIENTRY glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

#ifdef __cplusplus
}
#endif

#endif //MOBILEGLUES_VERTEXATTRIB_H
