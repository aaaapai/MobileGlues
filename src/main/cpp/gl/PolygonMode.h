#ifndef POLYGON_MODE_H
#define POLYGON_MODE_H

#include "../includes.h"
#include "GL/gl.h"
#include "GL/glcorearb.h"
#include "log.h"
#include "../gles/loader.h"
#include "mg.h"

#include <GLES3/gl3.h>

enum PolygonMode {
    POLYGON_FILL = 0,
    POLYGON_LINE,
    POLYGON_POINT
};

class PolygonModeRenderer {
public:
    PolygonModeRenderer();
    ~PolygonModeRenderer();
    
    // 设置多边形模式
    void setPolygonMode(PolygonMode mode);
    
    // 绘制几何体
    void drawGeometry(GLenum primitiveType, 
                    GLint first, 
                    GLsizei count,
                    GLfloat lineWidth = 1.0f,
                    GLfloat pointSize = 1.0f);
    
    // 使用索引绘制几何体
    void drawIndexedGeometry(GLenum primitiveType,
                           GLsizei count,
                           GLenum indicesType,
                           const void* indices,
                           GLfloat lineWidth = 1.0f,
                           GLfloat pointSize = 1.0f);
    
    static PolygonMode GetGlobalPolygonMode();
    static void SetGlobalPolygonMode(PolygonMode mode);
    
    GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc, const char* geometrySrc);
private:
    PolygonMode currentMode;
    GLuint fillProgram;
    GLuint lineProgram;
    GLuint pointProgram;
    
    // 几何着色器源
    const char* getfillGeometryShader();
    const char* getLineGeometryShader();
    const char* getPointGeometryShader();
    
    static PolygonMode s_globalMode;

};

//DeepSeek
#ifdef __cplusplus
extern "C" {
#endif

GLAPI GLAPIENTRY void glPolygonMode(GLenum face, GLenum mode);

#ifdef __cplusplus
}
#endif

#endif // POLYGON_MODE_H
