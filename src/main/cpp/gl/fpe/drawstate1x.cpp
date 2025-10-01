//
// Created by Swung 0x48 on 2025/4/10.
//
#include "types.h"

#define DEBUG 0

void fixed_function_draw_state_t::reset() {
    primitive = GL_NONE;
    vertex_count = 0;
    vb.str(std::string()); // clearing vb stringstream
}

void fixed_function_draw_state_t::advance() {
    ++vertex_count;

    const auto& sizes = current_data.sizes;

    // vertex
    if (sizes.vertex_size > 0)
    {
        vb.write(
                (const char*)glm::value_ptr(current_data.vertex),
                sizeof(GLfloat) * sizes.vertex_size);
    }

    // normal
    if (sizes.normal_size > 0)
    {
        vb.write(
                (const char*)glm::value_ptr(current_data.normal),
                sizeof(GLfloat) * sizes.normal_size);
    }

    // color
    if (sizes.color_size > 0)
    {
        vb.write(
                (const char*)glm::value_ptr(current_data.color),
                sizeof(GLfloat) * sizes.color_size);
    }

    // texcoord
    for (GLint i = 0; i < MAX_TEX; ++i)
    {
        if (sizes.texcoord_size[i] > 0) {
            vb.write(
                    (const char*)glm::value_ptr(current_data.texcoord[i]),
                    sizeof(GLfloat) * sizes.texcoord_size[i]);
        }
    }

    LOG_D("advance(): vertexcount = %d, vbsize = %d", vertex_count, vb.str().size())
}

void fixed_function_draw_state_t::compile_vertexattrib(vertex_pointer_array_t& va) const {
    va.reset();

    va.dirty = true;
    va.buffer_based = false;

    const auto& sizes = current_data.sizes;
    GLsizei offset = 0;

    // vertex
    if (sizes.vertex_size > 0)
    {
        va.enabled_pointers |= vp_mask(GL_VERTEX_ARRAY);

        va.attributes[vp2idx(GL_VERTEX_ARRAY)] = {
                .size = sizes.vertex_size,
                .usage = GL_VERTEX_ARRAY,
                .type = GL_FLOAT,
                .normalized = GL_FALSE,
                .stride = 0,
                .pointer = (const void*)offset,
//                .varying = true
        };
        offset += sizes.vertex_size * sizeof(GLfloat);
    }

    // normal
    if (sizes.normal_size > 0) {
        va.enabled_pointers |= vp_mask(GL_NORMAL_ARRAY);

        va.attributes[vp2idx(GL_NORMAL_ARRAY)] = {
                .size = sizes.normal_size,
                .usage = GL_NORMAL_ARRAY,
                .type = GL_FLOAT,
                .normalized = GL_FALSE,
                .stride = 0,
                .pointer = (const void*)offset,
//                .varying = true
        };
        offset += sizes.normal_size * sizeof(GLfloat);
    }

    // color
    if (sizes.color_size > 0)
    {
        va.enabled_pointers |= vp_mask(GL_COLOR_ARRAY);
        va.attributes[vp2idx(GL_COLOR_ARRAY)] = {
                .size = sizes.color_size,
                .usage = GL_COLOR_ARRAY,
                .type = GL_FLOAT,
                .normalized = GL_FALSE,
                .stride = 0,
                .pointer = (const void*)offset,
//                .varying = true
        };
        offset += sizes.color_size * sizeof(GLfloat);
    }

    // texcoord
    for (GLint i = 0; i < MAX_TEX; ++i)
    {
        if (sizes.texcoord_size[i] > 0) {
            // TODO: fix vp_mask()/vp2idx(), make it adapt to here
            va.enabled_pointers |= (1 << (7 + i));
            va.attributes[7 + i] = {
                    .size = sizes.texcoord_size[i],
                    .usage = GL_TEXTURE_COORD_ARRAY,
                    .type = GL_FLOAT,
                    .normalized = GL_FALSE,
                    .stride = 0,
                    .pointer = (const void*)offset,
//                    .varying = true
            };
            offset += sizes.texcoord_size[i] * sizeof(GLfloat);
        }
    }

    va.stride = offset;
}
