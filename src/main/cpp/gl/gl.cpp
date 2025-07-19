//
// Created by Swung0x48 on 2024/10/8.
//

#include "../egl/loader.h"
#include "../gles/loader.h"
#include <EGL/egl.h>
#include "../includes.h"
#include "GL/gl.h"
#include "GL/glcorearb.h"
#include "log.h"
#include "../gles/loader.h"
#include "../config/settings.h"
#include "mg.h"
#include "gl.h"

#define DEBUG 0

static GLclampd currentDepthValue;

void glClearDepth(GLclampd depth) {
    LOG()
    currentDepthValue = depth;
    GLES.glClearDepthf((float)depth);
    CHECK_GL_ERROR
}

void glDepthMask(GLboolean flag) {
    LOG()
    LOG_D("glDepthMask(%d)", flag)
    
    GLES.glDepthMask(flag);
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
    #version 320 es
    layout(location = 0) in vec2 aPos;
    void main() {
        // Write far‐plane depth
        gl_Position = vec4(aPos, 1.0, 1.0);
    }
)glsl";
static const char* kDepthClearFS = R"glsl(
    #version 320 es
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

static EGLDisplay eglDisplay = EGL_NO_DISPLAY;
static EGLSurface eglSurface = EGL_NO_SURFACE;
void glClear(GLbitfield mask) {
    LOG()
    LOG_D("glClear, mask = 0x%x", mask)

    /*if (global_settings.angle == AngleMode::Enabled &&
        mask == GL_DEPTH_BUFFER_BIT && 
        fabs(currentDepthValue - 1.0f) <= 0.001f) {
        if (global_settings.angle_depth_clear_fix_mode == AngleDepthClearFixMode::Mode1)
            // Workaround for ANGLE depth-clear bug: if depth≈1.0, draw a fullscreen triangle at z=1.0 to force actual depth buffer write.
            DrawDepthClearTri();
        else if (global_settings.angle_depth_clear_fix_mode == AngleDepthClearFixMode::Mode2) {
            // Or just explicitly clear depth buffer and see what's happened
            const GLfloat clear_depth_value = 1.0f;
            GLES.glClearBufferfv(GL_DEPTH, 0, &clear_depth_value);
        }
        // Clear again
        GLES.glClear(mask);
    } else {*/
        LOAD_EGL(eglSurfaceAttrib);
        if (mask == (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)) {
           if (eglDisplay != EGL_NO_DISPLAY && eglSurface != EGL_NO_SURFACE) {
              egl_eglSurfaceAttrib(eglDisplay, eglSurface, 
                           EGL_SWAP_BEHAVIOR, EGL_BUFFER_DESTROYED);
              return;
           }
        }
        GLES.glClear(mask);

        if (mask & GL_DEPTH_BUFFER_BIT) {
           GLES.glEnable(GL_DEPTH_TEST);
           GLES.glDepthFunc(GL_LEQUAL);
        }
    //}

    CHECK_GL_ERROR
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
    GLuint pointProgram = 0;
} s_polyState;

// 内部着色器创建函数
static GLuint createShader(GLenum type, const char* source) {
    GLuint shader = GLES.glCreateShader(type);
    GLES.glShaderSource(shader, 1, &source, NULL);
    GLES.glCompileShader(shader);
    
    GLint success;
    GLES.glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        LOG_E("Shader compilation failed: %s", infoLog);
    }
    return shader;
}

// 内部程序创建函数
static GLuint createProgram(const char* vsSource, const char* fsSource) {
    GLuint program = GLES.glCreateProgram();
    GLuint vs = createShader(GL_VERTEX_SHADER, vsSource);
    GLuint fs = createShader(GL_FRAGMENT_SHADER, fsSource);
    
    GLES.glAttachShader(program, vs);
    GLES.glAttachShader(program, fs);
    GLES.glLinkProgram(program);
    
    GLint success;
    GLES.glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        LOG_E("Program linking failed: %s", infoLog);
    }
    
    GLES.glDeleteShader(vs);
    GLES.glDeleteShader(fs);
    return program;
}

// 内部立即应用函数
static void applyPolygonMode() {
    if (s_polyState.currentProgram == 0) {
        GLES.glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&s_polyState.currentProgram);
    }

    if (s_polyState.front == GL_LINE || s_polyState.back == GL_LINE) {
        if (s_polyState.wireframeProgram == 0) {
            const char* vs = R"glsl(
                #version 300 es
                layout(location=0) in vec4 aPos;
                void main() { gl_Position = aPos; }
            )glsl";
            const char* fs = R"glsl(
                #version 300 es
                precision highp float;
                out vec4 FragColor;
                void main() { FragColor = vec4(1.0); }
            )glsl";
            s_polyState.wireframeProgram = createProgram(vs, fs);
        }
        GLES.glUseProgram(s_polyState.wireframeProgram);
    }
    else if (s_polyState.front == GL_POINT || s_polyState.back == GL_POINT) {
        if (s_polyState.pointProgram == 0) {
            const char* vs = R"glsl(
                #version 300 es
                layout(location=0) in vec4 aPos;
                void main() { 
                    gl_Position = aPos; 
                    gl_PointSize = 4.0;
                }
            )glsl";
            const char* fs = R"glsl(
                #version 300 es
                precision highp float;
                out vec4 FragColor;
                void main() { FragColor = vec4(1.0); }
            )glsl";
            s_polyState.pointProgram = createProgram(vs, fs);
        }
        GLES.glEnable(GL_PROGRAM_POINT_SIZE);
        GLES.glUseProgram(s_polyState.pointProgram);
    }
    else {
        GLES.glUseProgram(s_polyState.currentProgram);
    }
}

void glPolygonMode(GLenum face, GLenum mode) {

    LOG()
    // 参数验证
    if ((face != GL_FRONT) && (face != GL_BACK) && (face != GL_FRONT_AND_BACK)) return;
    if ((mode != GL_POINT) && (mode != GL_LINE) && (mode != GL_FILL)) return;

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
