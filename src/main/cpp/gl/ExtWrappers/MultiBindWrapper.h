#pragma once
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <regex.h>
#include "../log.h"
#include "../shader.h"
#include "../program.h"
#include "../buffer.h"
#include <regex>
#include <cstring>
#include <iostream>
#include "../../config/settings.h"
#include <ankerl/unordered_dense.h>
#include "../drawing.h"

extern "C" {
    GLAPI GLAPIENTRY void glBindTextures(GLuint first, GLsizei count, const GLuint* textures);
    GLAPI GLAPIENTRY void glBindSamplers(GLuint first, GLsizei count, const GLuint* samplers);
    GLAPI GLAPIENTRY void glBindImageTextures(GLuint first, GLsizei count, const GLuint* textures);
    GLAPI GLAPIENTRY void glBindVertexBuffers(GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides);
}
