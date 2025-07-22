#version 320 es
precision highp float;
precision highp int;
uniform sampler2D Sampler0;
layout (location = 0) in vec4 vertexColor;
layout (location = 1) in vec2 texCoord0;
layout (location = 0) out vec4 FragColor;

void main() {
   vec4 color = texture(Sampler0, texCoord0) * vertexColor;
   if (color.a < 0.1) {
       discard;
   }
   FragColor = color;
}