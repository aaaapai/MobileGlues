#include "PolygonMode.h"
#include <string>

namespace {
    PolygonMode g_currentPolygonMode = POLYGON_FILL;
    PolygonModeRenderer g_polygonRenderer;
}

PolygonModeRenderer::PolygonModeRenderer() : currentMode(POLYGON_FILL) {
    // 基本顶点着色器
    const char* vertexShaderSrc = R"(
        #version 320 es
        layout(location = 0) in vec3 aPosition;
        uniform mat4 uMVP;
        void main() {
            gl_Position = uMVP * vec4(aPosition, 1.0);
        }
    )";
    
    // 基本片段着色器
    const char* fragmentShaderSrc = R"(
        #version 320 es
        precision highp float;
        uniform vec4 uColor;
        out vec4 fragColor;
        void main() {
            fragColor = uColor;
        }
    )";
    
    // 创建填充模式着色器程序
    fillProgram = createShaderProgram(vertexShaderSrc, fragmentShaderSrc);
    
    // 创建线框模式着色器程序（使用几何着色器）
    const char* lineGeometryShader = getLineGeometryShader();
    lineProgram = createShaderProgram(vertexShaderSrc, fragmentShaderSrc, lineGeometryShader);
    
    // 创建点模式着色器程序（使用几何着色器）
    const char* pointGeometryShader = getPointGeometryShader();
    pointProgram = createShaderProgram(vertexShaderSrc, fragmentShaderSrc, pointGeometryShader);
}

PolygonModeRenderer::~PolygonModeRenderer() {
    GLES.glDeleteProgram(fillProgram);
    GLES.glDeleteProgram(lineProgram);
    GLES.glDeleteProgram(pointProgram);
}

void PolygonModeRenderer::setPolygonMode(PolygonMode mode) {
    currentMode = mode;
}

void PolygonModeRenderer::drawGeometry(GLenum primitiveType, 
                                     GLint first, 
                                     GLsizei count,
                                     GLfloat lineWidth,
                                     GLfloat pointSize) {
    switch(currentMode) {
        case POLYGON_FILL:
            GLES.glUseProgram(fillProgram);
            GLES.glDrawArrays(primitiveType, first, count);
            break;
            
        case POLYGON_LINE:
            GLES.glUseProgram(lineProgram);
            GLES.glLineWidth(lineWidth);
            // 对于非三角形图元，可能需要特殊处理
            if(primitiveType == GL_TRIANGLES) {
                GLES.glDrawArrays(GL_TRIANGLES, first, count);
            } else {
                // 其他图元类型的处理
                GLES.glDrawArrays(primitiveType, first, count);
            }
            break;
            
        case POLYGON_POINT:
            GLES.glUseProgram(pointProgram);
            glPointSize(pointSize);
            GLES.glDrawArrays(GL_POINTS, first, count);
            break;
    }
}

void PolygonModeRenderer::drawIndexedGeometry(GLenum primitiveType,
                                            GLsizei count,
                                            GLenum indicesType,
                                            const void* indices,
                                            GLfloat lineWidth,
                                            GLfloat pointSize) {
    switch(currentMode) {
        case POLYGON_FILL:
            GLES.glUseProgram(fillProgram);
            GLES.glDrawElements(primitiveType, count, indicesType, indices);
            break;
            
        case POLYGON_LINE:
            GLES.glUseProgram(lineProgram);
            GLES.glLineWidth(lineWidth);
            if(primitiveType == GL_TRIANGLES) {
                GLES.glDrawElements(GL_TRIANGLES, count, indicesType, indices);
            } else {
                GLES.glDrawElements(primitiveType, count, indicesType, indices);
            }
            break;
            
        case POLYGON_POINT:
            GLES.glUseProgram(pointProgram);
            glPointSize(pointSize);
            GLES.glDrawElements(GL_POINTS, count, indicesType, indices);
            break;
    }
}

GLuint PolygonModeRenderer::createShaderProgram(const char* vertexSrc, 
                                              const char* fragmentSrc,
                                              const char* geometrySrc) {
    GLuint program = GLES.glCreateProgram();
    GLuint vertexShader = GLES.glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = GLES.glCreateShader(GL_FRAGMENT_SHADER);
    
    GLES.glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    GLES.glCompileShader(vertexShader);
    
    GLES.glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    GLES.glCompileShader(fragmentShader);
    
    GLES.glAttachShader(program, vertexShader);
    GLES.glAttachShader(program, fragmentShader);
    
    if(geometrySrc != nullptr) {
        GLuint geometryShader = GLES.glCreateShader(GL_GEOMETRY_SHADER);
        GLES.glShaderSource(geometryShader, 1, &geometrySrc, nullptr);
        GLES.glCompileShader(geometryShader);
        GLES.glAttachShader(program, geometryShader);
        GLES.glDeleteShader(geometryShader);
    }
    
    GLES.glLinkProgram(program);
    
    GLES.glDeleteShader(vertexShader);
    GLES.glDeleteShader(fragmentShader);
    
    return program;
}

const char* PolygonModeRenderer::getLineGeometryShader() {
    return R"(
        #version 320 es
        layout(triangles) in;
        layout(line_strip, max_vertices = 4) out;
        
        void main() {
            for(int i = 0; i < 3; i++) {
                gl_Position = gl_in[i].gl_Position;
                EmitVertex();
            }
            
            gl_Position = gl_in[0].gl_Position;
            EmitVertex();
            EndPrimitive();
        }
    )";
}

const char* PolygonModeRenderer::getPointGeometryShader() {
    return R"(
        #version 320 es
        layout(triangles) in;
        layout(points, max_vertices = 3) out;
        
        void main() {
            for(int i = 0; i < 3; i++) {
                gl_Position = gl_in[i].gl_Position;
                EmitVertex();
                EndPrimitive();
            }
        }
    )";
}

void glPolygonMode(GLenum face, GLenum mode) {
    // OpenGL ES只支持FRONT_AND_BACK，忽略单独的面设置
    if(face != GL_FRONT_AND_BACK) {
        return;
    }
    
    switch(mode) {
        case GL_FILL:
            g_currentPolygonMode = POLYGON_FILL;
            break;
        case GL_LINE:
            g_currentPolygonMode = POLYGON_LINE;
            break;
        case GL_POINT:
            g_currentPolygonMode = POLYGON_POINT;
            break;
        default:
            // 无效模式，可以记录错误
            return;
    }
    
    g_polygonRenderer.setPolygonMode(g_currentPolygonMode);
}
