//
// Created by Swung0x48 on 2024/10/8.
//

#include "../includes.h"
#include "GL/gl.h"
#include "GL/glcorearb.h"
#include "log.h"
#include "../gles/loader.h"
#include "../config/settings.h"
#include "mg.h"
#include "gl.h"

#define DEBUG 1

static GLclampd currentDepthValue;

void glClearDepth(GLclampd depth) {
    LOG()
    currentDepthValue = depth;
    GLES.glClearDepthf((float)depth);
    CHECK_GL_ERROR
}

static GLuint g_depthClearProgram = 0;
static GLuint g_depthClearVAO = 0;
static GLuint g_depthClearVBO = 0;

static const GLfloat kFullScreenTri[3][2] = {
    { -1.0f, -1.0f },
    {  3.0f, -1.0f },
    { -1.0f,  3.0f }
};

static const char* kDepthClearVS = R"glsl(
    #version 300 es
    layout(location = 0) in vec2 aPos;
    void main() {
        // Write far‐plane depth
        gl_Position = vec4(aPos, 1.0, 1.0);
    }
)glsl";
static const char* kDepthClearFS = R"glsl(
    #version 300 es
    precision mediump float;
    out vec4 fragColor;
    void main() {
        // Empty—color writes will be disabled
        fragColor = vec4(0.0);
    }
)glsl";

void InitDepthClearCoreProfile() {
    if (g_depthClearProgram) return;

    auto compile = [&](GLenum type, const char* src) {
        GLuint s = GLES.glCreateShader(type);
        GLES.glShaderSource(s, 1, &src, nullptr);
        GLES.glCompileShader(s);
        return s;
        };
    GLuint vs = compile(GL_VERTEX_SHADER, kDepthClearVS);
    GLuint fs = compile(GL_FRAGMENT_SHADER, kDepthClearFS);

    g_depthClearProgram = GLES.glCreateProgram();
    GLES.glAttachShader(g_depthClearProgram, vs);
    GLES.glAttachShader(g_depthClearProgram, fs);
    GLES.glLinkProgram(g_depthClearProgram);
    GLES.glDeleteShader(vs);
    GLES.glDeleteShader(fs);

    GLES.glGenVertexArrays(1, &g_depthClearVAO);
    GLES.glGenBuffers(1, &g_depthClearVBO);

    GLES.glBindVertexArray(g_depthClearVAO);
    GLES.glBindBuffer(GL_ARRAY_BUFFER, g_depthClearVBO);
    GLES.glBufferData(GL_ARRAY_BUFFER, sizeof(kFullScreenTri), kFullScreenTri, GL_STATIC_DRAW);

    GLES.glEnableVertexAttribArray(0);
    GLES.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    GLES.glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLES.glBindVertexArray(0);
}

void DrawDepthClearTri() {
    InitDepthClearCoreProfile();

    GLboolean prevColorMask[4];
    GLES.glGetBooleanv(GL_COLOR_WRITEMASK, prevColorMask);
    GLboolean prevDepthMask;
    GLES.glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);
    GLint prevDepthFunc;
    GLES.glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);

    GLES.glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    GLES.glDepthMask(GL_TRUE);
    GLES.glDepthFunc(GL_ALWAYS);
    
    GLES.glUseProgram(g_depthClearProgram);
    GLES.glBindVertexArray(g_depthClearVAO);
    GLES.glDrawArrays(GL_TRIANGLES, 0, 3);
    GLES.glBindVertexArray(0);
    GLES.glUseProgram(0);

    GLES.glDepthFunc(prevDepthFunc);
    GLES.glDepthMask(prevDepthMask);
    GLES.glColorMask(prevColorMask[0], prevColorMask[1], prevColorMask[2], prevColorMask[3]);
}

void glHint(GLenum target, GLenum mode) {
    LOG()
    LOG_D("glHint, target = %s, mode = %s", glEnumToString(target), glEnumToString(mode))
    GLES.glHint(target, mode);
}

// 全局状态记录
static struct {
    GLenum front = GL_FILL;
    GLenum back = GL_FILL;
} s_polygonMode;

void glPolygonMode(GLenum face, GLenum mode) {

    LOG()

    // 参数验证
    if (face != GL_FRONT && face != GL_BACK && face != GL_FRONT_AND_BACK) {
        LOG_E("Invalid face: 0x%04X", face);
        return;
    }
    if (mode != GL_POINT && mode != GL_LINE && mode != GL_FILL) {
        LOG_E("Invalid mode: 0x%04X", mode);
        return;
    }

    // 更新状态
    switch (face) {
        case GL_FRONT: 
            s_polygonMode.front = mode;
            break;
        case GL_BACK:
            s_polygonMode.back = mode;
            break;
        case GL_FRONT_AND_BACK:
            s_polygonMode.front = s_polygonMode.back = mode;
            break;
    }

    if (s_polygonMode.front == s_polygonMode.back) {
            GLES.glPolygonModeNV(GL_FRONT_AND_BACK, s_polygonMode.front);
    } else {
            GLES.glPolygonModeNV(GL_FRONT, s_polygonMode.front);
            GLES.glPolygonModeNV(GL_BACK, s_polygonMode.back);
    }

}
