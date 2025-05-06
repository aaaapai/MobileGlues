#include "texture.h"
#include "../texture.h"

#define DEBUG 0

void glCreateTextures(GLenum target, GLsizei n, GLuint *textures) {
     glBindTexture(target, textures);
     glGenTextures(n, textures);
}
