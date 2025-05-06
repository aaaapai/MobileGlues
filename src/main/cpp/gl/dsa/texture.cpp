#include "texture.h"
#include "../texture.h"

#define DEBUG 0

void glCreateTextures(GLenum target, GLsizei n, GLuint *textures) {
     glGenTextures(n, textures);
}
