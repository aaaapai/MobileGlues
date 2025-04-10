//
// Created by Swung 0x48 on 2025/2/11.
//

#ifndef MOBILEGLUES_TYPES_H
#define MOBILEGLUES_TYPES_H

#include "../gl.h"
#include "../gles/loader.h"
#include "../gl/log.h"
#include "defines.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <vector>
#include <sstream>
#include "fpe_shadergen.h"
#include "vertexpointer_utils.h"

struct transformation_t {
    glm::mat4 matrices[4];
    std::vector<glm::mat4> matrices_stack[4];
    GLenum matrix_mode = GL_MODELVIEW;
};

struct vertexattribute_t {
    GLint size;
    GLenum usage;
    GLenum type;
    GLenum normalized;
    GLsizei stride;
    const void *pointer;
    glm::vec4 value;
    bool varies = true;
};

#define VERTEX_POINTER_COUNT (8 + MAX_TEX)
struct vertex_pointer_array_t {
    const void* starting_pointer = NULL;
    GLsizei stride = 0;

    struct vertexattribute_t attributes[VERTEX_POINTER_COUNT];
    uint32_t enabled_pointers = 0;
    bool dirty = false;
    bool buffer_based = false;

    void reset() {
        starting_pointer = NULL;
        stride = 0;
        enabled_pointers = 0;
        dirty = false;
        buffer_based = false;
        memset(&attributes, 0, sizeof(attributes));
    }
};

struct fixed_function_bool_t { // glEnable/glDisable
    bool fog_enable = false; // GL_FOG
    bool lighting_enable = false; // GL_LIGHTING
    bool light_enable[MAX_LIGHTS] = {false};
};

struct light_t {
    glm::vec4 ambient = {0, 0, 0, 1};
    glm::vec4 diffuse = {1, 1, 1, 1};
    glm::vec4 specular = {0, 0, 0, 1};
    glm::vec4 position = {0, 0, 1, 0};
    GLfloat constant_attenuation = 1.;
    GLfloat linear_attenuation = 0.;
    GLfloat quadratic_attenuation = 0.;
    glm::vec3 spot_direction = {0, 0, -1};
    GLfloat spot_exp = 0.;
    GLfloat spot_cutoff = 180.; // 0-90, 180
};

// size = 0 means disabled
struct fixed_function_draw_size_t {
    GLint normal_size = 3;
    GLint color_size = 0;
    GLint vertex_size = 0;
    GLint texcoord_size[MAX_TEX] = {0};
};

struct fixed_function_draw_data_t {
    glm::vec4 vertex = {0, 0, 0, 1};
    glm::vec3 normal = {0, 0, 1};
    glm::vec4 color = {1, 1, 1, 1};
    glm::vec4 texcoord[MAX_TEX];

    fixed_function_draw_size_t sizes;
};

struct fixed_function_draw_state_t {
    GLenum primitive = GL_NONE;

    fixed_function_draw_data_t current_data;

    std::stringstream vb;

    size_t vertex_count = 0;

#define DEBUG 0
    inline void reset() {
        primitive = GL_NONE;
        vertex_count = 0;
        vb.str(std::string()); // clearing vb stringstream
    }

    inline void advance() {
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

    inline void compile_vertexattrib(vertex_pointer_array_t& va) const {
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
                    .varies = true
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
                    .varies = true
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
                    .varies = true
            };
            offset += sizes.color_size * sizeof(GLfloat);
        }

        // texcoord
        for (GLint i = 0; i < MAX_TEX; ++i)
        {
            if (sizes.texcoord_size[i] > 0) {
                LOG_D("texcoord_size[%d] = %d", i, sizes.texcoord_size[i])
                // TODO: fix vp_mask()/vp2idx(), make it adapt to here
                va.enabled_pointers |= (1 << (7 + i));
                va.attributes[7 + i] = {
                        .size = sizes.texcoord_size[i],
                        .usage = GL_TEXTURE_COORD_ARRAY,
                        .type = GL_FLOAT,
                        .normalized = GL_FALSE,
                        .stride = 0,
                        .pointer = (const void*)offset,
                        .varies = true
                };
                offset += sizes.texcoord_size[i] * sizeof(GLfloat);
            }
        }

        va.stride = offset;
    }

#undef DEBUG
};

struct fixed_function_state_t {
    GLenum client_active_texture = GL_TEXTURE0; // glClientActiveTexture, specifies active texcood
    GLenum alpha_func = GL_ALWAYS; // glAlphaFunc
    GLenum fog_mode = GL_EXP; // glFogi(GL_FOG_MODE)
    GLint fog_index = 0; // glFogi(GL_FOG_INDEX)
    GLenum fog_coord_src = GL_FRAGMENT_DEPTH; // glFogi(GL_FOG_COORD_SRC)
    GLenum shade_model = GL_SMOOTH; // glShadeModel
    GLenum light_model_color_ctrl = GL_SINGLE_COLOR; // glLightModel(GL_LIGHT_MODEL_COLOR_CONTROL)
    int light_model_local_viewer = 0; // glLightModel(GL_LIGHT_MODEL_LOCAL_VIEWER)
    int light_model_two_side = 0; // glLightModel(GL_LIGHT_MODEL_TWO_SIDE)

    // Fixed-function VAO
    // Reserve a vao purely for fpe, so that
    // it won't mess up with other states in
    // programmable pipeline.
    GLuint fpe_vao = 0;

    GLuint fpe_vbo = 0;

    GLuint fpe_ibo = 0;

    std::vector<uint32_t> fpe_ib;

    struct vertex_pointer_array_t vertexpointer_array;
    struct fixed_function_bool_t fpe_bools;
};

struct fixed_function_uniform_t {
    // glAlphaFunc
    GLclampf alpha_ref = 0.0f;

    // glFogf
    GLfloat fog_density = 1.f;
    GLfloat fog_start = 0.f;
    GLfloat fog_end = 1.f;
    // glFogfv/iv
    glm::vec4 fog_color = { 0., 0., 0., 0. };

    // glLightModel
    glm::vec4 light_model_ambient = {0.2, 0.2, 0.2, 1.0};

    // glMatrix*
    struct transformation_t transformation;

    // glLightf/i/fv/iv
    light_t lights[MAX_LIGHTS];
};

struct program_t {
    std::string vs;
    std::string fs;

    int get_program();
private:
    static int compile_shader(GLenum shader_type, const char* src);
    static int link_program(GLuint vs, GLuint fs);
    int program = -1;
};

struct glstate_t {
    struct fixed_function_state_t fpe_state;
    struct fixed_function_uniform_t fpe_uniform;

    struct fixed_function_draw_state_t fpe_draw;

    GLuint fpe_vtx_shader = 0;
    GLuint fpe_frag_shader = 0;
//    GLuint fpe_program = 0;

    // enabled_vertexpointers - program
    // TODO: using vp as key is bad! Try to hash the whole fpe_state
    std::unordered_map<uint32_t, program_t> fpe_programs;

    const char* fpe_vtx_shader_src;
    const char* fpe_frag_shader_src;

    static glstate_t& get_instance();
};

#endif //MOBILEGLUES_TYPES_H
