#version 320 es
precision highp float;
precision highp int;
layout(location = 0) uniform mat4 ModelViewMat;
layout(location = 1) uniform mat4 ProjMat;
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec4 Color;
layout (location = 7) in vec2 UV0;
layout (location = 0) out vec4 vertexColor;
layout (location = 1) out vec2 texCoord0;

void main() {
   gl_Position = ProjMat * ModelViewMat * vec4(Pos, 1.0);
   vertexColor = Color;
   texCoord0 = UV0;
}