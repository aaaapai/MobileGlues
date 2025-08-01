#ifndef MOBILEGLUES_TEXTURE_HPP
#define MOBILEGLUES_TEXTURE_HPP


#include <GL/gl.h>
#include "ankerl/unordered_dense.h"
#include <iostream>
#include <memory>

template<typename T>
void readDataComponents(const void* data, GLenum type, T* out, size_t maxComponents);

enum class TextureTarget : unsigned int {
    TEXTURE_1D = 0,
    PROXY_TEXTURE_1D,
    TEXTURE_1D_ARRAY,
    PROXY_TEXTURE_1D_ARRAY,
    TEXTURE_2D,
    PROXY_TEXTURE_2D,
    TEXTURE_2D_ARRAY,
    PROXY_TEXTURE_2D_ARRAY,
    TEXTURE_2D_MULTISAMPLE,
    PROXY_TEXTURE_2D_MULTISAMPLE,
    TEXTURE_2D_MULTISAMPLE_ARRAY,
    PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY,
    TEXTURE_3D,
    PROXY_TEXTURE_3D,
    TEXTURE_RECTANGLE,
    PROXY_TEXTURE_RECTANGLE,
    TEXTURE_CUBE_MAP,
    PROXY_TEXTURE_CUBE_MAP,
    // TEXTURE_CUBE_MAP_POSITIVE_X,
    // TEXTURE_CUBE_MAP_NEGATIVE_X,
    // TEXTURE_CUBE_MAP_POSITIVE_Y,
    // TEXTURE_CUBE_MAP_NEGATIVE_Y,
    // TEXTURE_CUBE_MAP_POSITIVE_Z,
    // TEXTURE_CUBE_MAP_NEGATIVE_Z,
    TEXTURE_CUBE_MAP_ARRAY,
    PROXY_TEXTURE_CUBE_MAP_ARRAY,
    TEXTURE_BUFFER,
    TEXTURES_COUNT
};

GLenum ConvertTextureTargetToGLEnum(TextureTarget target);
TextureTarget ConvertGLEnumToTextureTarget(GLenum target);

class TextureObject { // TODO: Make this a more standard class
public:
    TextureTarget target;
    GLuint texture;
    GLenum internal_format;
    GLenum format;
    GLint swizzle_param[4];
    GLsizei width;
    GLsizei height;
    GLsizei depth;
};

std::shared_ptr<TextureObject> mgGetTexObjectByTarget(GLenum target);
std::shared_ptr<TextureObject> mgGetTexObjectByID(unsigned texture);

#endif
