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

// 状态结构
static struct {
    GLenum front = GL_FILL;
    GLenum back = GL_FILL;
    GLuint currentProgram = 0;
    GLuint wireframeProgram = 0;
} s_polyState;

// 内部立即应用函数
static void applyPolygonMode() {
    if (s_polyState.front == GL_LINE || s_polyState.back == GL_LINE) {
        if (s_polyState.wireframeProgram == 0) {
            // 惰性初始化线框着色器
            const char* vs = "#version 300 es\nlayout(location=0) in vec4 aPos; void main() { gl_Position = aPos; }";
            const char* fs = "#version 300 es\nout vec4 FragColor; void main() { FragColor = vec4(1.0); }";
            s_polyState.wireframeProgram = createProgram(vs, fs);
        }
        GLES.glUseProgram(s_polyState.wireframeProgram);
    } else {
        // 恢复原始程序
        GLES.glUseProgram(s_polyState.currentProgram);
    }
}

void glPolygonMode(GLenum face, GLenum mode) {
    // 参数验证
    if ((face != GL_FRONT) && (face != GL_BACK) && (face != GL_FRONT_AND_BACK)) return;
    if ((mode != GL_POINT) && (mode != GL_LINE) && (mode != GL_FILL)) return;

    // 保存当前程序（首次调用时）
    if (s_polyState.currentProgram == 0) {
        GLES.glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&s_polyState.currentProgram);
    }

    // 更新状态
    switch (face) {
        case GL_FRONT:  s_polyState.front = mode; break;
        case GL_BACK:   s_polyState.back = mode; break;
        case GL_FRONT_AND_BACK: 
            s_polyState.front = s_polyState.back = mode; 
            break;
    }

    // 立即应用改变
    applyPolygonMode();
} //DeepSeek
