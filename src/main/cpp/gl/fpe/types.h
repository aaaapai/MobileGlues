//
// Created by Swung 0x48 on 2025/2/11.
//

#ifndef MOBILEGLUES_TYPES_H
#define MOBILEGLUES_TYPES_H

#include "GL/gl.h"
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
#include "../version.h"

GLsizei type_size(GLenum type);

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
//    glm::vec4 value;
//    bool varying = true;
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

    // Split into starting pointer & offset into buffer per pointer
    void normalize() {
        int first_va_idx = -1;

        // Find starting pointer
        for (int i = 0; i < VERTEX_POINTER_COUNT; ++i) {
            bool enabled = ((enabled_pointers >> i) & 1);
            if (!enabled) continue;

            if (first_va_idx == -1) {
                first_va_idx = i;
                break;
            }
        }

        if (stride == 0)
            stride = attributes[first_va_idx].stride;

        // if not valid starting pointer
        if (!(stride != 0 && starting_pointer != 0 && starting_pointer > (void*)stride)) {
            starting_pointer = attributes[first_va_idx].pointer;
        }

        // stride==0 && stride in pointer == 0
        // => tightly packed, infer stride from offset below
        bool do_calc_stride = (stride == 0);

        // Adjust pointer offsets according to starting pointer
        // Getting actual stride if stride==0
        for (int i = 0; i < VERTEX_POINTER_COUNT; ++i) {
            bool enabled = ((enabled_pointers >> i) & 1);
            if (!enabled) continue;

            auto &vp = attributes[i];

            // check if pointer is a pointer rather than an offset
            if (stride > 0 && (uint64_t)vp.pointer > (uint64_t)stride)
                vp.pointer =
                        (const void*)((const uint64_t)vp.pointer - (const uint64_t)starting_pointer);

            if (do_calc_stride)
                stride = std::max((uint64_t)stride, (uint64_t)vp.pointer + vp.size * type_size(vp.type));
        }
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
    union {
        struct {
            GLint vertex_size = 0;
            GLint normal_size = 0;
            GLint color_size = 0;
            GLint index_size = 0;
            GLint edge_size = 0;
            GLint fog_size = 0;
            GLint secondary_color_size = 0;
            GLint placeholder_8th = 0; // data[7]
            GLint texcoord_size[MAX_TEX] = {0};
        };
        GLint data[VERTEX_POINTER_COUNT];
    };
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

    void reset();

    void advance();

    void compile_vertexattrib(vertex_pointer_array_t& va) const;
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
    struct fixed_function_draw_state_t fpe_draw;
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
    template <typename K, typename V>
    using unordered_map = std::unordered_map<K, V>;

    // States that can led to layout change / shader recompile
    struct fixed_function_state_t fpe_state;
    struct fixed_function_uniform_t fpe_uniform;

//    GLuint fpe_vtx_shader = 0;
//    GLuint fpe_frag_shader = 0;
//    GLuint fpe_program = 0;

    // enabled_vertexpointers - program
    // TODO: using vp as key is bad! Try to hash the whole fpe_state
    unordered_map<uint64_t, program_t> fpe_programs;
    unordered_map<uint64_t, GLuint> fpe_vaos;

    static constexpr uint32_t s_hash_seed = VERSION_NUM;

    const char* fpe_vtx_shader_src;
    const char* fpe_frag_shader_src;

    static glstate_t& get_instance();

    void send_uniforms(int program);

    uint32_t program_hash();

    uint32_t vertex_attrib_hash();

    program_t& get_or_generate_program(const uint64_t key);

    bool get_vao(const uint64_t key, GLuint* vao);

    void save_vao(const uint64_t key, const GLuint vao);

    void send_vertex_attributes();
};

#endif //MOBILEGLUES_TYPES_H
