//
// Created by BZLZHH on 2025/4/19.
//

#include "../includes.h"
#include "../log.h"
#include <GL/gl.h>
#include "legacy_extensions.h"

#define DEBUG 0

extern "C" {
GLboolean glIsRenderbufferEXT(GLuint renderbuffer) {
     return GLES.glIsRenderbuffer(renderbuffer);
}

void glBindRenderbufferEXT(GLenum target, GLuint renderbuffer) {
    GLES.glBindRenderbuffer(target, renderbuffer);
}

void glDeleteRenderbuffersEXT(GLsizei n, const GLuint *renderbuffers) {
    GLES.glDeleteRenderbuffers(n, renderbuffers);
}

void glGenRenderbuffersEXT(GLsizei n, GLuint *renderbuffers) {
    GLES.glGenRenderbuffers(n, renderbuffers);
}

void glRenderbufferStorageEXT(GLenum target, GLenum internalformat,
                                     GLsizei width, GLsizei height) {
    glRenderbufferStorage(target, internalformat, width, height);
}

void glGetRenderbufferParameterivEXT(GLenum target, GLenum pname, GLint *params) {
    GLES.glGetRenderbufferParameteriv(target, pname, params);
}

GLboolean glIsFramebufferEXT(GLuint framebuffer) {
      return GLES.glIsFramebuffer(framebuffer);
}

void glBindFramebufferEXT(GLenum target, GLuint framebuffer) {
    glBindFramebuffer(target, framebuffer);
}

void glDeleteFramebuffersEXT(GLsizei n, const GLuint *framebuffers) {
    GLES.glDeleteFramebuffers(n, framebuffers);
}

void glGenFramebuffersEXT(GLsizei n, GLuint *framebuffers) {
    GLES.glGenFramebuffers(n, framebuffers);
}

GLenum glCheckFramebufferStatusEXT(GLenum target) {
     return glCheckFramebufferStatus(target);
}

void glFramebufferTexture2DEXT(GLenum target, GLenum attachment,
                                      GLenum textarget, GLuint texture, GLint level) {
    glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

void glFramebufferRenderbufferEXT(GLenum target, GLenum attachment,
                                         GLenum renderbuffertarget, GLuint renderbuffer) {
    GLES.glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

void glGetFramebufferAttachmentParameterivEXT(GLenum target, GLenum attachment,
                                                     GLenum pname, GLint *params) {
    GLES.glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
}

void glGenerateMipmapEXT(GLenum target) {
    GLES.glGenerateMipmap(target);
}

void glRenderbufferStorageMultisampleEXT(GLenum target, GLsizei samples,
                                                GLenum internalformat,
                                                GLsizei width, GLsizei height) {
    glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
}

void glBlitFramebufferEXT(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                 GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                                 GLbitfield mask, GLenum filter) {
    GLES.glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1,
                      dstX0, dstY0, dstX1, dstY1,
                      mask, filter);
}

void glBlitFramebufferLayersEXT(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                       GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                                       GLbitfield mask, GLenum filter) {
    GLES.glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1,
                            dstX0, dstY0, dstX1, dstY1,
                            mask, filter);
}

void glBlitFramebufferLayerEXT(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                      GLint srcLayer,
                                      GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                                      GLint dstLayer,
                                      GLbitfield mask, GLenum filter) {
    //glBlitFramebufferLayer(srcX0, srcY0, srcX1, srcY1,
    //                       srcLayer,
    //                       dstX0, dstY0, dstX1, dstY1,
    //                       dstLayer,
    //                       mask, filter);
}

//void glGenRenderbuffersARB(GLsizei n, GLuint *renderbuffers) {
//    glGenRenderbuffers(n, renderbuffers);
//}

void glRenderbufferStorageARB(GLenum target, GLenum internalformat,
                                     GLsizei width, GLsizei height) {
    glRenderbufferStorage(target, internalformat, width, height);
}

void glRenderbufferStorageMultisampleARB(GLenum target, GLsizei samples,
                                                GLenum internalformat,
                                                GLsizei width, GLsizei height) {
    GLES.glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
}

void glBindFramebufferARB(GLenum target, GLuint framebuffer) {
    glBindFramebuffer(target, framebuffer);
}

GLenum glCheckFramebufferStatusARB(GLenum target) {
      return glCheckFramebufferStatus(target);
}

void glFramebufferTexture1DARB(GLenum target, GLenum attachment,
                                      GLenum textarget, GLuint texture, GLint level) {
    //glFramebufferTexture1D(target, attachment, textarget, texture, level);
}

void glFramebufferTexture2DARB(GLenum target, GLenum attachment,
                                      GLenum textarget, GLuint texture, GLint level) {
    glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

void glFramebufferTexture3DARB(GLenum target, GLenum attachment,
                                      GLenum textarget, GLuint texture,
                                      GLint level, GLint layer) {
    //glFramebufferTexture3D(target, attachment, textarget, texture, level, layer);
}
void glDrawBuffersARB(GLsizei n, const GLenum *bufs) {
    glDrawBuffers(n, bufs);
}
void glFramebufferTexture1DEXT(GLenum target, GLenum attachment, GLenum textarget,
                                      GLuint texture, GLint level) {
    glFramebufferTexture2D(target, attachment, GL_TEXTURE_2D, texture, level);
}
void glFramebufferTexture3DEXT(GLenum target, GLenum attachment, GLenum textarget,
                                      GLuint texture, GLint level, GLint zoffset) {
    LOG()
    GLES.glFramebufferTexture3DOES(target, attachment, textarget, texture, level, zoffset);
}
    
void glDeleteObjectARB(GLhandleARB obj) {
    if (GLES.glIsProgram(obj)) {
        GLES.glDeleteProgram(obj);
    } else {
        GLES.glDeleteShader(obj);
    }
}

GLhandleARB glGetHandleARB(GLenum pname) {
    if (pname == GL_PROGRAM_OBJECT_ARB) {
        GLint currentProg;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProg);
        return (GLhandleARB)currentProg;
    }
    return 0;
}

void glDetachObjectARB(GLhandleARB containerObj, GLhandleARB attachedObj) {
    GLES.glDetachShader(containerObj, attachedObj);
}

GLhandleARB glCreateShaderObjectARB(GLenum shaderType) {
    return GLES.glCreateShader(shaderType);
}

void glShaderSourceARB(GLhandleARB shaderObj, GLsizei count,
                              const GLchar **string, const GLint *length) {
    glShaderSource(shaderObj, count, string, length);
}

GLhandleARB glCreateProgramObjectARB(void) {
    return GLES.glCreateProgram();
}

void glAttachObjectARB(GLhandleARB containerObj, GLhandleARB obj) {
    GLES.glAttachShader(containerObj, obj);
}

void glLinkProgramARB(GLhandleARB programObj) {
    glLinkProgram(programObj);
}

void glUseProgramObjectARB(GLhandleARB programObj) {
    GLES.glUseProgram(programObj);
}

void glGetObjectParameterfvARB(GLhandleARB obj, GLenum pname, GLfloat *params) {
    GLint iparam;
    if (GLES.glIsShader(obj)) {
        glGetShaderiv(obj, pname, &iparam);
    } else {
        glGetProgramiv(obj, pname, &iparam);
    }
    if (params) {
        params[0] = (GLfloat)iparam;
    }
}

void glGetObjectParameterivARB(GLhandleARB obj, GLenum pname, GLint *params) {
    if (GLES.glIsShader(obj)) {
        glGetShaderiv(obj, pname, params);
    } else {
        glGetProgramiv(obj, pname, params);
    }
}

//void glVertexAttrib1fARB(GLuint index, GLfloat v0) { glVertexAttrib1f(index, v0); }
void glVertexAttrib1sARB(GLuint index, GLshort v0) { glVertexAttrib1f(index, v0); }
void glVertexAttrib1dARB(GLuint index, GLdouble v0) { glVertexAttrib1f(index, v0); }
//void glVertexAttrib2fARB(GLuint index, GLfloat v0, GLfloat v1) { glVertexAttrib2f(index, v0, v1); }
void glVertexAttrib2sARB(GLuint index, GLshort v0, GLshort v1) { glVertexAttrib2f(index, v0, v1); }
void glVertexAttrib2dARB(GLuint index, GLdouble v0, GLdouble v1) { glVertexAttrib2f(index, v0, v1); }
//void glVertexAttrib3fARB(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2) { glVertexAttrib3f(index, v0, v1, v2); }
void glVertexAttrib3sARB(GLuint index, GLshort v0, GLshort v1, GLshort v2) { glVertexAttrib3f(index, v0, v1, v2); }
void glVertexAttrib3dARB(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2) { glVertexAttrib3f(index, v0, v1, v2); }
//void glVertexAttrib4fARB(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) { glVertexAttrib4f(index, v0, v1, v2, v3); }
void glVertexAttrib4sARB(GLuint index, GLshort v0, GLshort v1, GLshort v2, GLshort v3) { glVertexAttrib4f(index, v0, v1, v2, v3); }
void glVertexAttrib4dARB(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3) { glVertexAttrib4f(index, v0, v1, v2, v3); }

//void glVertexAttrib1fvARB(GLuint index, const GLfloat *v) { glVertexAttrib1fv(index, v); }
void glVertexAttrib1svARB(GLuint index, const GLshort *v) { glVertexAttrib1fv(index, (GLfloat*)v); }
void glVertexAttrib1dvARB(GLuint index, const GLdouble *v) { glVertexAttrib1fv(index, (GLfloat*)v); }
//void glVertexAttrib2fvARB(GLuint index, const GLfloat *v) { glVertexAttrib2fv(index, v); }
void glVertexAttrib2svARB(GLuint index, const GLshort *v) { glVertexAttrib2fv(index, (GLfloat*)v); }
void glVertexAttrib2dvARB(GLuint index, const GLdouble *v) { glVertexAttrib2fv(index, (GLfloat*)v); }
//void glVertexAttrib3fvARB(GLuint index, const GLfloat *v) { glVertexAttrib3fv(index, v); }
void glVertexAttrib3svARB(GLuint index, const GLshort *v) { glVertexAttrib3fv(index, (GLfloat*)v); }
void glVertexAttrib3dvARB(GLuint index, const GLdouble *v) { glVertexAttrib3fv(index, (GLfloat*)v); }
//void glVertexAttrib4fvARB(GLuint index, const GLfloat *v) { glVertexAttrib4fv(index, v); }
void glVertexAttrib4svARB(GLuint index, const GLshort *v) { glVertexAttrib4fv(index, (GLfloat*)v); }
void glVertexAttrib4dvARB(GLuint index, const GLdouble *v) { glVertexAttrib4fv(index, (GLfloat*)v); }
void glVertexAttrib4ivARB(GLuint index, const GLint *v) { glVertexAttrib4fv(index, (GLfloat*)v); }
void glVertexAttrib4bvARB(GLuint index, const GLbyte *v) { glVertexAttrib4fv(index, (GLfloat*)v); }
void glVertexAttrib4ubvARB(GLuint index, const GLubyte *v) { glVertexAttrib4fv(index, (GLfloat*)v); }
void glVertexAttrib4usvARB(GLuint index, const GLushort *v) { glVertexAttrib4fv(index, (GLfloat*)v); }
void glVertexAttrib4uivARB(GLuint index, const GLuint *v) { glVertexAttrib4fv(index, (GLfloat*)v); }

//void glCompileShaderARB(GLhandleARB shader) { glCompileShader(shader); }
//GLhandleARB glCreateShaderARB(GLenum shaderType) { return glCreateShader(shaderType); }

//void glVertexAttribPointerARB(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
//    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
//}

//void glEnableVertexAttribArrayARB(GLuint index) { glEnableVertexAttribArray(index); }
//void glDisableVertexAttribArrayARB(GLuint index) { glDisableVertexAttribArray(index); }

//void glBindAttribLocationARB(GLhandleARB programObj, GLuint index, const GLcharARB *name) {
//    glBindAttribLocation(programObj, index, name);
//}

//void glGetActiveAttribARB(GLhandleARB programObj, GLuint index, GLsizei maxLength,
//                                 GLsizei *length, GLint *size, GLenum *type, GLcharARB *name) {
//    glGetActiveAttrib(programObj, index, maxLength, length, size, type, name);
//}

//GLint glGetAttribLocationARB(GLhandleARB programObj, const GLcharARB *name) {
//    return glGetAttribLocation(programObj, name);
//}

void glGetVertexAttribdvARB(GLuint index, GLenum pname, GLdouble *params) { glGetVertexAttribfv(index, pname,(GLfloat*) params); }
//void glGetVertexAttribfvARB(GLuint index, GLenum pname, GLfloat *params) { glGetVertexAttribfv(index, pname, params); }
//void glGetVertexAttribivARB(GLuint index, GLenum pname, GLint *params) { glGetVertexAttribiv(index, pname, params); }
//void glGetVertexAttribPointervARB(GLuint index, GLenum pname, void **pointer) {
//    glGetVertexAttribPointerv(index, pname, pointer);
//}

void glGetActiveUniformName(GLuint program,
                          GLuint uniformIndex,
                          GLsizei bufSize,
                          GLsizei* length,
                          char* uniformName)
{
    GLint uniformSize;
    GLenum uniformType;
    char rawName[bufSize];

    GLES.glGetActiveUniform(program, uniformIndex, bufSize, length,
                       &uniformSize, &uniformType, rawName);

    char* bracketPos = strchr(rawName, '[');
    if (bracketPos != nullptr) {
        *bracketPos = '\0';
    }

    strncpy(uniformName, rawName, bufSize);
    if (length != nullptr && *length > bufSize) {
        *length = (GLsizei)strlen(rawName);
    }
}

void glGetActiveUniformNameARB(GLuint program,
                                      GLuint uniformIndex,
                                      GLsizei bufSize,
                                      GLsizei* length,
                                      char* uniformName) __attribute__((alias("glGetActiveUniformName")));

GLint glGetProgramResourceLocationIndex(GLuint program, GLenum programInterface, const char *name) {
    if (program == 0 || GLES.glIsProgram(program) == GL_FALSE) {
        return -1;
    }

    GLuint resourceIndex = GLES.glGetProgramResourceIndex(program, programInterface, name);
    if (resourceIndex == GL_INVALID_INDEX) {
        return -1;
    }

    switch (programInterface) {
        case GL_PROGRAM_OUTPUT: {
            GLint index = 0;
            GLenum props[] = { GL_INDEX };
            GLsizei length;
            GLES.glGetProgramResourceiv(program, programInterface, resourceIndex,
                                   1, props, 1, &length, &index);
            return index;
        }
        case GL_UNIFORM: {
            return -1;
        }
        default: {
            return -1;
        }
    }
}
void glGetProgramResourceLocationIndexARB(GLuint program, GLenum programInterface, const char *name) __attribute__((alias("glGetProgramResourceLocationIndex")));

void glGetAttachedObjectsARB(GLhandleARB program, GLsizei maxCount, GLsizei* count, GLhandleARB* objects) {
    if (program == 0 || GLES.glIsProgram(program) == GL_FALSE) {
        return;
    }
    GLES.glGetAttachedShaders(program, maxCount, count, (GLuint*)objects);
}

//GLhandleARB glCreateProgramARB() { return glCreateProgram(); }

}
