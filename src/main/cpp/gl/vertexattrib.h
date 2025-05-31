//
// Created by Swung 0x48 on 2025/2/19.
//

#ifndef MOBILEGLUES_VERTEXATTRIB_H
#define MOBILEGLUES_VERTEXATTRIB_H

#include "../includes.h"
#include "GL/gl.h"
#include "GL/glcorearb.h"
#include "log.h"
#include "../gles/loader.h"
#include "mg.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI GLAPIENTRY void glVertexAttribI1ui(GLuint index, GLuint x);
GLAPI GLAPIENTRY void glVertexAttribI2ui(GLuint index, GLuint x, GLuint y);
GLAPI GLAPIENTRY void glVertexAttribI3ui(GLuint index, GLuint x, GLuint y, GLuint z);

GLAPI GLAPIENTRY void glVertexAttrib1d (GLuint index, GLdouble x);
GLAPI GLAPIENTRY void glVertexAttrib1dv (GLuint index, const GLdouble *v);
GLAPI GLAPIENTRY void glVertexAttrib1f (GLuint index, GLfloat x);
GLAPI GLAPIENTRY void glVertexAttrib1fv (GLuint index, const GLfloat *v);
GLAPI GLAPIENTRY void glVertexAttrib1s (GLuint index, GLshort x);
GLAPI GLAPIENTRY void glVertexAttrib1sv (GLuint index, const GLshort *v);
GLAPI GLAPIENTRY void glVertexAttrib2d (GLuint index, GLdouble x, GLdouble y);
GLAPI GLAPIENTRY void glVertexAttrib2dv (GLuint index, const GLdouble *v);
GLAPI GLAPIENTRY void glVertexAttrib2f (GLuint index, GLfloat x, GLfloat y);
GLAPI GLAPIENTRY void glVertexAttrib2fv (GLuint index, const GLfloat *v);
GLAPI GLAPIENTRY void glVertexAttrib2s (GLuint index, GLshort x, GLshort y);
GLAPI GLAPIENTRY void glVertexAttrib2sv (GLuint index, const GLshort *v);
GLAPI GLAPIENTRY void glVertexAttrib3d (GLuint index, GLdouble x, GLdouble y, GLdouble z);
GLAPI GLAPIENTRY void glVertexAttrib3dv (GLuint index, const GLdouble *v);
GLAPI GLAPIENTRY void glVertexAttrib3f (GLuint index, GLfloat x, GLfloat y, GLfloat z);
GLAPI GLAPIENTRY void glVertexAttrib3fv (GLuint index, const GLfloat *v);
GLAPI GLAPIENTRY void glVertexAttrib3s (GLuint index, GLshort x, GLshort y, GLshort z);
GLAPI GLAPIENTRY void glVertexAttrib3sv (GLuint index, const GLshort *v);
GLAPI GLAPIENTRY void glVertexAttrib4Nbv (GLuint index, const GLbyte *v);
GLAPI GLAPIENTRY void glVertexAttrib4Niv (GLuint index, const GLint *v);
GLAPI GLAPIENTRY void glVertexAttrib4Nsv (GLuint index, const GLshort *v);
GLAPI GLAPIENTRY void glVertexAttrib4Nub (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
GLAPI GLAPIENTRY void glVertexAttrib4Nubv (GLuint index, const GLubyte *v);
GLAPI GLAPIENTRY void glVertexAttrib4Nuiv (GLuint index, const GLuint *v);
GLAPI GLAPIENTRY void glVertexAttrib4Nusv (GLuint index, const GLushort *v);
GLAPI GLAPIENTRY void glVertexAttrib4bv (GLuint index, const GLbyte *v);
GLAPI GLAPIENTRY void glVertexAttrib4d (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLAPI GLAPIENTRY void glVertexAttrib4dv (GLuint index, const GLdouble *v);
GLAPI GLAPIENTRY void glVertexAttrib4f (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLAPI GLAPIENTRY void glVertexAttrib4fv (GLuint index, const GLfloat *v);
GLAPI GLAPIENTRY void glVertexAttrib4iv (GLuint index, const GLint *v);
GLAPI GLAPIENTRY void glVertexAttrib4s (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
GLAPI GLAPIENTRY void glVertexAttrib4sv (GLuint index, const GLshort *v);
GLAPI GLAPIENTRY void glVertexAttrib4ubv (GLuint index, const GLubyte *v);
GLAPI GLAPIENTRY void glVertexAttrib4uiv (GLuint index, const GLuint *v);
GLAPI GLAPIENTRY void glVertexAttrib4usv (GLuint index, const GLushort *v);

#ifdef __cplusplus
}
#endif

#endif //MOBILEGLUES_VERTEXATTRIB_H
