//
// Created by aaaapai on 2025-04-30.
//

#ifndef MOBILEGLUES_LEGACY_EXTRNSIONS_H
#define MOBILEGLUES_LEGACY_EXTRNSIONS_H

#include "GL/gl.h"

#ifdef __cplusplus
extern "C" {
#endif

GLAPI GLAPIENTRY GLboolean glIsRenderbufferEXT(GLuint renderbuffer);
GLAPI GLAPIENTRY void glBindRenderbufferEXT(GLenum target, GLuint renderbuffer);
GLAPI GLAPIENTRY void glDeleteRenderbuffersEXT(GLsizei n, const GLuint *renderbuffers);
GLAPI GLAPIENTRY void glGenRenderbuffersEXT(GLsizei n, GLuint *renderbuffers);
GLAPI GLAPIENTRY void glRenderbufferStorageEXT(GLenum target, GLenum internalformat,
                                     GLsizei width, GLsizei height);
GLAPI GLAPIENTRY void glGetRenderbufferParameterivEXT(GLenum target, GLenum pname, GLint *params);
GLAPI GLAPIENTRY GLboolean glIsFramebufferEXT(GLuint framebuffer);
GLAPI GLAPIENTRY void glBindFramebufferEXT(GLenum target, GLuint framebuffer);
GLAPI GLAPIENTRY void glDeleteFramebuffersEXT(GLsizei n, const GLuint *framebuffers);
GLAPI GLAPIENTRY void glGenFramebuffersEXT(GLsizei n, GLuint *framebuffers);
GLenum glCheckFramebufferStatusEXT(GLenum target);
GLAPI GLAPIENTRY void glFramebufferTexture2DEXT(GLenum target, GLenum attachment,
                                      GLenum textarget, GLuint texture, GLint level);
GLAPI GLAPIENTRY void glFramebufferRenderbufferEXT(GLenum target, GLenum attachment,
                                         GLenum renderbuffertarget, GLuint renderbuffer);
GLAPI GLAPIENTRY void glGetFramebufferAttachmentParameterivEXT(GLenum target, GLenum attachment,
                                                     GLenum pname, GLint *params);
GLAPI GLAPIENTRY void glGenerateMipmapEXT(GLenum target
GLAPI GLAPIENTRY void glRenderbufferStorageMultisampleEXT(GLenum target, GLsizei samples,
                                                GLenum internalformat,
                                                GLsizei width, GLsizei height
GLAPI GLAPIENTRY void glBlitFramebufferEXT(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                 GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                                 GLbitfield mask, GLenum filter
GLAPI GLAPIENTRY void glBlitFramebufferLayersEXT(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                       GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                                       GLbitfield mask, GLenum filter
GLAPI GLAPIENTRY void glBlitFramebufferLayerEXT(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                      GLint srcLayer,
                                      GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                                      GLint dstLayer,
                                      GLbitfield mask, GLenum filter);

GLAPI GLAPIENTRY void glRenderbufferStorageARB(GLenum target, GLenum internalformat,
                                     GLsizei width, GLsizei height);
GLAPI GLAPIENTRY void glRenderbufferStorageMultisampleARB(GLenum target, GLsizei samples,
                                                GLenum internalformat,
                                                GLsizei width, GLsizei height);
GLAPI GLAPIENTRY void glBindFramebufferARB(GLenum target, GLuint framebuffer);
GLenum glCheckFramebufferStatusARB(GLenum target);
GLAPI GLAPIENTRY void glFramebufferTexture1DARB(GLenum target, GLenum attachment,
                                      GLenum textarget, GLuint texture, GLint level);
GLAPI GLAPIENTRY void glFramebufferTexture2DARB(GLenum target, GLenum attachment,
                                      GLenum textarget, GLuint texture, GLint level);
GLAPI GLAPIENTRY void glFramebufferTexture3DARB(GLenum target, GLenum attachment,
                                      GLenum textarget, GLuint texture,
                                      GLint level, GLint layer);

GLAPI GLAPIENTRY void glDrawBuffersARB(GLsizei n, const GLenum *bufs);

GLAPI GLAPIENTRY void glFramebufferTexture3DEXT(GLenum target, GLenum attachment, GLenum textarget,
                                      GLuint texture, GLint level, GLint zoffset);
                                      
GLAPI GLAPIENTRY void glFramebufferTexture1DEXT(GLenum target, GLenum attachment, GLenum textarget,
                                      GLuint texture, GLint level);
GLAPI GLAPIENTRY void glDeleteObjectARB(GLhandleARB obj);
GLhandleARB glGetHandleARB(GLenum pname);
GLAPI GLAPIENTRY void glDetachObjectARB(GLhandleARB containerObj, GLhandleARB attachedObj);
GLhandleARB glCreateShaderObjectARB(GLenum shaderType);

GLhandleARB glCreateProgramObjectARB(void);

GLAPI GLAPIENTRY void glAttachObjectARB(GLhandleARB containerObj, GLhandleARB obj);
GLAPI GLAPIENTRY void glLinkProgramARB(GLhandleARB programObj);
GLAPI GLAPIENTRY void glUseProgramObjectARB(GLhandleARB programObj);
GLAPI GLAPIENTRY void glGetObjectParameterfvARB(GLhandleARB obj, GLenum pname, GLfloat *params);
GLAPI GLAPIENTRY void glGetObjectParameterivARB(GLhandleARB obj, GLenum pname, GLint *params);
GLAPI GLAPIENTRY void glVertexAttrib1sARB(GLuint index, GLshort v0);
GLAPI GLAPIENTRY void glVertexAttrib1dARB(GLuint index, GLdouble v0);
GLAPI GLAPIENTRY void glVertexAttrib2sARB(GLuint index, GLshort v0, GLshort v1);
GLAPI GLAPIENTRY void glVertexAttrib2dARB(GLuint index, GLdouble v0, GLdouble v1);
GLAPI GLAPIENTRY void glVertexAttrib3sARB(GLuint index, GLshort v0, GLshort v1, GLshort v2
GLAPI GLAPIENTRY void glVertexAttrib3dARB(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2);
GLAPI GLAPIENTRY void glVertexAttrib4sARB(GLuint index, GLshort v0, GLshort v1, GLshort v2, GLshort v3);
GLAPI GLAPIENTRY void glVertexAttrib4dARB(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
GLAPI GLAPIENTRY void glVertexAttrib1svARB(GLuint index, const GLshort *v);
GLAPI GLAPIENTRY void glVertexAttrib1dvARB(GLuint index, const GLdouble *v);
GLAPI GLAPIENTRY void glVertexAttrib2svARB(GLuint index, const GLshort *v);
GLAPI GLAPIENTRY void glVertexAttrib2dvARB(GLuint index, const GLdouble *v); 
GLAPI GLAPIENTRY void glVertexAttrib3svARB(GLuint index, const GLshort *v);
GLAPI GLAPIENTRY void glVertexAttrib3dvARB(GLuint index, const GLdouble *v
GLAPI GLAPIENTRY void glVertexAttrib4svARB(GLuint index, const GLshort *v);
GLAPI GLAPIENTRY void glVertexAttrib4dvARB(GLuint index, const GLdouble *v);
GLAPI GLAPIENTRY void glVertexAttrib4ivARB(GLuint index, const GLint *v);
GLAPI GLAPIENTRY void glVertexAttrib4bvARB(GLuint index, const GLbyte *v);
GLAPI GLAPIENTRY void glVertexAttrib4ubvARB(GLuint index, const GLubyte *v);
GLAPI GLAPIENTRY void glVertexAttrib4usvARB(GLuint index, const GLushort *v);
GLAPI GLAPIENTRY void glVertexAttrib4uivARB(GLuint index, const GLuint *v);
GLAPI GLAPIENTRY void glGetVertexAttribdvARB(GLuint index, GLenum pname, GLdouble *params);
GLAPI GLAPIENTRY void glGetActiveUniformName(GLuint program,
                          GLuint uniformIndex,
                          GLsizei bufSize,
                          GLsizei* length,
                          char* uniformName);
                          
GLAPI GLAPIENTRY void glShaderSourceARB(GLhandleARB shaderObj, GLsizei count,
                              const GLchar **string, const GLint *length);
                              
GLAPI GLAPIENTRY void glGetActiveUniformNameARB(GLuint program,
                                      GLuint uniformIndex,
                                      GLsizei bufSize,
                                      GLsizei* length,
                                      char* uniformName);
                                      
GLAPI GLAPIENTRY GLint glGetProgramResourceLocationIndex(GLuint program, GLenum programInterface, const char *name);

GLAPI GLAPIENTRY void glGetProgramResourceLocationIndexARB(GLuint program, GLenum programInterface, const char *name);

GLAPI GLAPIENTRY void glGetAttachedObjectsARB(GLhandleARB program, GLsizei maxCount, GLsizei* count, GLhandleARB* objects);

#ifdef __cplusplus
}
#endif


#endif //MOBILEGLUES_LEGACY_EXTRNSIONS_H