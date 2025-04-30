//
// Created by BZLZHH on 2025/4/19.
//

#include "../includes.h"
#include "../log.h"
#include <GL/gl.h>

#define DEBUG 1

extern "C" {
GLAPI GLAPIENTRY GLboolean glIsRenderbufferEXT(GLuint renderbuffer) {
return glIsRenderbuffer(renderbuffer);
}

GLAPI GLAPIENTRY void glBindRenderbufferEXT(GLenum target, GLuint renderbuffer) {
    glBindRenderbuffer(target, renderbuffer);
}

GLAPI GLAPIENTRY void glDeleteRenderbuffersEXT(GLsizei n, const GLuint *renderbuffers) {
    glDeleteRenderbuffers(n, renderbuffers);
}

GLAPI GLAPIENTRY void glGenRenderbuffersEXT(GLsizei n, GLuint *renderbuffers) {
    glGenRenderbuffers(n, renderbuffers);
}

GLAPI GLAPIENTRY void glRenderbufferStorageEXT(GLenum target, GLenum internalformat,
                                     GLsizei width, GLsizei height) {
    glRenderbufferStorage(target, internalformat, width, height);
}

GLAPI GLAPIENTRY void glGetRenderbufferParameterivEXT(GLenum target, GLenum pname, GLint *params) {
    glGetRenderbufferParameteriv(target, pname, params);
}

GLAPI GLAPIENTRY GLboolean glIsFramebufferEXT(GLuint framebuffer) {
return glIsFramebuffer(framebuffer);
}

GLAPI GLAPIENTRY void glBindFramebufferEXT(GLenum target, GLuint framebuffer) {
    glBindFramebuffer(target, framebuffer);
}

GLAPI GLAPIENTRY void glDeleteFramebuffersEXT(GLsizei n, const GLuint *framebuffers) {
    glDeleteFramebuffers(n, framebuffers);
}

GLAPI GLAPIENTRY void glGenFramebuffersEXT(GLsizei n, GLuint *framebuffers) {
    glGenFramebuffers(n, framebuffers);
}

GLAPI GLAPIENTRY GLenum glCheckFramebufferStatusEXT(GLenum target) {
return glCheckFramebufferStatus(target);
}

GLAPI GLAPIENTRY void glFramebufferTexture2DEXT(GLenum target, GLenum attachment,
                                      GLenum textarget, GLuint texture, GLint level) {
    glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

GLAPI GLAPIENTRY void glFramebufferRenderbufferEXT(GLenum target, GLenum attachment,
                                         GLenum renderbuffertarget, GLuint renderbuffer) {
    glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

GLAPI GLAPIENTRY void glGetFramebufferAttachmentParameterivEXT(GLenum target, GLenum attachment,
                                                     GLenum pname, GLint *params) {
    glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
}

GLAPI GLAPIENTRY void glGenerateMipmapEXT(GLenum target) {
    glGenerateMipmap(target);
}

GLAPI GLAPIENTRY void glRenderbufferStorageMultisampleEXT(GLenum target, GLsizei samples,
                                                GLenum internalformat,
                                                GLsizei width, GLsizei height) {
    glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
}

GLAPI GLAPIENTRY void glBlitFramebufferEXT(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                 GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                                 GLbitfield mask, GLenum filter) {
    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1,
                      dstX0, dstY0, dstX1, dstY1,
                      mask, filter);
}

GLAPI GLAPIENTRY void glBlitFramebufferLayersEXT(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                       GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                                       GLbitfield mask, GLenum filter) {
    //glBlitFramebufferLayers(srcX0, srcY0, srcX1, srcY1,
    //                        dstX0, dstY0, dstX1, dstY1,
    //                        mask, filter);
}

GLAPI GLAPIENTRY void glBlitFramebufferLayerEXT(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
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

//GLAPI GLAPIENTRY void glGenRenderbuffersARB(GLsizei n, GLuint *renderbuffers) {
//    glGenRenderbuffers(n, renderbuffers);
//}

GLAPI GLAPIENTRY void glRenderbufferStorageARB(GLenum target, GLenum internalformat,
                                     GLsizei width, GLsizei height) {
    glRenderbufferStorage(target, internalformat, width, height);
}

GLAPI GLAPIENTRY void glRenderbufferStorageMultisampleARB(GLenum target, GLsizei samples,
                                                GLenum internalformat,
                                                GLsizei width, GLsizei height) {
    glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
}

GLAPI GLAPIENTRY void glBindFramebufferARB(GLenum target, GLuint framebuffer) {
    glBindFramebuffer(target, framebuffer);
}

GLAPI GLAPIENTRY GLenum glCheckFramebufferStatusARB(GLenum target) {
return glCheckFramebufferStatus(target);
}

GLAPI GLAPIENTRY void glFramebufferTexture1DARB(GLenum target, GLenum attachment,
                                      GLenum textarget, GLuint texture, GLint level) {
    //glFramebufferTexture1D(target, attachment, textarget, texture, level);
}

GLAPI GLAPIENTRY void glFramebufferTexture2DARB(GLenum target, GLenum attachment,
                                      GLenum textarget, GLuint texture, GLint level) {
    glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

GLAPI GLAPIENTRY void glFramebufferTexture3DARB(GLenum target, GLenum attachment,
                                      GLenum textarget, GLuint texture,
                                      GLint level, GLint layer) {
    //glFramebufferTexture3D(target, attachment, textarget, texture, level, layer);
}
GLAPI GLAPIENTRY void glDrawBuffersARB(GLsizei n, const GLenum *bufs) {
    glDrawBuffers(n, bufs);
}
GLAPI GLAPIENTRY void glFramebufferTexture1DEXT(GLenum target, GLenum attachment, GLenum textarget,
                                      GLuint texture, GLint level) {
    glFramebufferTexture2D(target, attachment, GL_TEXTURE_2D, texture, level);
}
GLAPI GLAPIENTRY void glFramebufferTexture3DEXT(GLenum target, GLenum attachment, GLenum textarget,
                                      GLuint texture, GLint level, GLint zoffset) {
    LOG()
    GLES.glFramebufferTexture3DOES(target, attachment, textarget, texture, level, zoffset);
}
    
GLAPI GLAPIENTRY void glDeleteObjectARB(GLhandleARB obj) {
    if (glIsProgram(obj)) {
        glDeleteProgram(obj);
    } else {
        glDeleteShader(obj);
    }
}

GLAPI GLAPIENTRY GLhandleARB glGetHandleARB(GLenum pname) {
    if (pname == GL_PROGRAM_OBJECT_ARB) {
        GLint currentProg;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProg);
        return (GLhandleARB)currentProg;
    }
    return 0;
}

GLAPI GLAPIENTRY void glDetachObjectARB(GLhandleARB containerObj, GLhandleARB attachedObj) {
    glDetachShader(containerObj, attachedObj);
}

GLAPI GLAPIENTRY GLhandleARB glCreateShaderObjectARB(GLenum shaderType) {
    return glCreateShader(shaderType);
}

GLAPI GLAPIENTRY void glShaderSourceARB(GLhandleARB shaderObj, GLsizei count,
                              const GLchar **string, const GLint *length) {
    glShaderSource(shaderObj, count, string, length);
}

GLAPI GLAPIENTRY GLhandleARB glCreateProgramObjectARB(void) {
    return glCreateProgram();
}

GLAPI GLAPIENTRY void glAttachObjectARB(GLhandleARB containerObj, GLhandleARB obj) {
    glAttachShader(containerObj, obj);
}

GLAPI GLAPIENTRY void glLinkProgramARB(GLhandleARB programObj) {
    glLinkProgram(programObj);
}

GLAPI GLAPIENTRY void glUseProgramObjectARB(GLhandleARB programObj) {
    glUseProgram(programObj);
}

GLAPI GLAPIENTRY void glGetObjectParameterfvARB(GLhandleARB obj, GLenum pname, GLfloat *params) {
    GLint iparam;
    if (glIsShader(obj)) {
        glGetShaderiv(obj, pname, &iparam);
    } else {
        glGetProgramiv(obj, pname, &iparam);
    }
    if (params) {
        params[0] = (GLfloat)iparam;
    }
}

GLAPI GLAPIENTRY void glGetObjectParameterivARB(GLhandleARB obj, GLenum pname, GLint *params) {
    if (glIsShader(obj)) {
        glGetShaderiv(obj, pname, params);
    } else {
        glGetProgramiv(obj, pname, params);
    }
}

//GLAPI GLAPIENTRY void glVertexAttrib1fARB(GLuint index, GLfloat v0) { glVertexAttrib1f(index, v0); }
GLAPI GLAPIENTRY void glVertexAttrib1sARB(GLuint index, GLshort v0) { glVertexAttrib1f(index, v0); }
GLAPI GLAPIENTRY void glVertexAttrib1dARB(GLuint index, GLdouble v0) { glVertexAttrib1f(index, v0); }
//GLAPI GLAPIENTRY void glVertexAttrib2fARB(GLuint index, GLfloat v0, GLfloat v1) { glVertexAttrib2f(index, v0, v1); }
GLAPI GLAPIENTRY void glVertexAttrib2sARB(GLuint index, GLshort v0, GLshort v1) { glVertexAttrib2f(index, v0, v1); }
GLAPI GLAPIENTRY void glVertexAttrib2dARB(GLuint index, GLdouble v0, GLdouble v1) { glVertexAttrib2f(index, v0, v1); }
//GLAPI GLAPIENTRY void glVertexAttrib3fARB(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2) { glVertexAttrib3f(index, v0, v1, v2); }
GLAPI GLAPIENTRY void glVertexAttrib3sARB(GLuint index, GLshort v0, GLshort v1, GLshort v2) { glVertexAttrib3f(index, v0, v1, v2); }
GLAPI GLAPIENTRY void glVertexAttrib3dARB(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2) { glVertexAttrib3f(index, v0, v1, v2); }
//GLAPI GLAPIENTRY void glVertexAttrib4fARB(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) { glVertexAttrib4f(index, v0, v1, v2, v3); }
GLAPI GLAPIENTRY void glVertexAttrib4sARB(GLuint index, GLshort v0, GLshort v1, GLshort v2, GLshort v3) { glVertexAttrib4f(index, v0, v1, v2, v3); }
GLAPI GLAPIENTRY void glVertexAttrib4dARB(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3) { glVertexAttrib4f(index, v0, v1, v2, v3); }

//GLAPI GLAPIENTRY void glVertexAttrib1fvARB(GLuint index, const GLfloat *v) { glVertexAttrib1fv(index, v); }
GLAPI GLAPIENTRY void glVertexAttrib1svARB(GLuint index, const GLshort *v) { glVertexAttrib1fv(index, (GLfloat*)v); }
GLAPI GLAPIENTRY void glVertexAttrib1dvARB(GLuint index, const GLdouble *v) { glVertexAttrib1fv(index, (GLfloat*)v); }
//GLAPI GLAPIENTRY void glVertexAttrib2fvARB(GLuint index, const GLfloat *v) { glVertexAttrib2fv(index, v); }
GLAPI GLAPIENTRY void glVertexAttrib2svARB(GLuint index, const GLshort *v) { glVertexAttrib2fv(index, (GLfloat*)v); }
GLAPI GLAPIENTRY void glVertexAttrib2dvARB(GLuint index, const GLdouble *v) { glVertexAttrib2fv(index, (GLfloat*)v); }
//GLAPI GLAPIENTRY void glVertexAttrib3fvARB(GLuint index, const GLfloat *v) { glVertexAttrib3fv(index, v); }
GLAPI GLAPIENTRY void glVertexAttrib3svARB(GLuint index, const GLshort *v) { glVertexAttrib3fv(index, (GLfloat*)v); }
GLAPI GLAPIENTRY void glVertexAttrib3dvARB(GLuint index, const GLdouble *v) { glVertexAttrib3fv(index, (GLfloat*)v); }
//GLAPI GLAPIENTRY void glVertexAttrib4fvARB(GLuint index, const GLfloat *v) { glVertexAttrib4fv(index, v); }
GLAPI GLAPIENTRY void glVertexAttrib4svARB(GLuint index, const GLshort *v) { glVertexAttrib4fv(index, (GLfloat*)v); }
GLAPI GLAPIENTRY void glVertexAttrib4dvARB(GLuint index, const GLdouble *v) { glVertexAttrib4fv(index, (GLfloat*)v); }
GLAPI GLAPIENTRY void glVertexAttrib4ivARB(GLuint index, const GLint *v) { glVertexAttrib4fv(index, (GLfloat*)v); }
GLAPI GLAPIENTRY void glVertexAttrib4bvARB(GLuint index, const GLbyte *v) { glVertexAttrib4fv(index, (GLfloat*)v); }
GLAPI GLAPIENTRY void glVertexAttrib4ubvARB(GLuint index, const GLubyte *v) { glVertexAttrib4fv(index, (GLfloat*)v); }
GLAPI GLAPIENTRY void glVertexAttrib4usvARB(GLuint index, const GLushort *v) { glVertexAttrib4fv(index, (GLfloat*)v); }
GLAPI GLAPIENTRY void glVertexAttrib4uivARB(GLuint index, const GLuint *v) { glVertexAttrib4fv(index, (GLfloat*)v); }

//GLAPI GLAPIENTRY void glCompileShaderARB(GLhandleARB shader) { glCompileShader(shader); }
//GLAPI GLAPIENTRY GLhandleARB glCreateShaderARB(GLenum shaderType) { return glCreateShader(shaderType); }

//GLAPI GLAPIENTRY void glVertexAttribPointerARB(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
//    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
//}

//GLAPI GLAPIENTRY void glEnableVertexAttribArrayARB(GLuint index) { glEnableVertexAttribArray(index); }
//GLAPI GLAPIENTRY void glDisableVertexAttribArrayARB(GLuint index) { glDisableVertexAttribArray(index); }

//GLAPI GLAPIENTRY void glBindAttribLocationARB(GLhandleARB programObj, GLuint index, const GLcharARB *name) {
//    glBindAttribLocation(programObj, index, name);
//}

//GLAPI GLAPIENTRY void glGetActiveAttribARB(GLhandleARB programObj, GLuint index, GLsizei maxLength,
//                                 GLsizei *length, GLint *size, GLenum *type, GLcharARB *name) {
//    glGetActiveAttrib(programObj, index, maxLength, length, size, type, name);
//}

//GLAPI GLAPIENTRY GLint glGetAttribLocationARB(GLhandleARB programObj, const GLcharARB *name) {
//    return glGetAttribLocation(programObj, name);
//}

GLAPI GLAPIENTRY void glGetVertexAttribdvARB(GLuint index, GLenum pname, GLdouble *params) { glGetVertexAttribfv(index, pname,(GLfloat*) params); }
//GLAPI GLAPIENTRY void glGetVertexAttribfvARB(GLuint index, GLenum pname, GLfloat *params) { glGetVertexAttribfv(index, pname, params); }
//GLAPI GLAPIENTRY void glGetVertexAttribivARB(GLuint index, GLenum pname, GLint *params) { glGetVertexAttribiv(index, pname, params); }
//GLAPI GLAPIENTRY void glGetVertexAttribPointervARB(GLuint index, GLenum pname, void **pointer) {
//    glGetVertexAttribPointerv(index, pname, pointer);
//}

GLAPI GLAPIENTRY void glGetActiveUniformName(GLuint program,
                          GLuint uniformIndex,
                          GLsizei bufSize,
                          GLsizei* length,
                          char* uniformName)
{
    GLint uniformSize;
    GLenum uniformType;
    char rawName[bufSize];

    glGetActiveUniform(program, uniformIndex, bufSize, length,
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

GLAPI GLAPIENTRY void glGetActiveUniformNameARB(GLuint program,
                                      GLuint uniformIndex,
                                      GLsizei bufSize,
                                      GLsizei* length,
                                      char* uniformName) __attribute__((alias("glGetActiveUniformName")));

GLAPI GLAPIENTRY GLint glGetProgramResourceLocationIndex(GLuint program, GLenum programInterface, const char *name) {
    if (program == 0 || glIsProgram(program) == GL_FALSE) {
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
GLAPI GLAPIENTRY void glGetProgramResourceLocationIndexARB(GLuint program, GLenum programInterface, const char *name) __attribute__((alias("glGetProgramResourceLocationIndex")));

GLAPI GLAPIENTRY void glGetAttachedObjectsARB(GLhandleARB program, GLsizei maxCount, GLsizei* count, GLhandleARB* objects) {
    if (program == 0 || glIsProgram(program) == GL_FALSE) {
        return;
    }
    glGetAttachedShaders(program, maxCount, count, (GLuint*)objects);
}

//GLAPI GLAPIENTRY GLhandleARB glCreateProgramARB() { return glCreateProgram(); }

}
