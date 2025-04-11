//
// Created by Swung 0x48 on 2025/2/8.
//

#include "fpe.hpp"
#include <glm/glm/gtc/type_ptr.hpp>

#define DEBUG 0

glstate_t& glstate_t::get_instance() {
    static glstate_t s_glstate;
    return s_glstate;
}

GLsizei type_size(GLenum type) {
    switch (type) {
        case GL_SHORT:
            return 2;
        case GL_INT:
            return 4;
        case GL_FLOAT:
            return 4;
        case GL_DOUBLE:
            return 8;
        default:
            LOG_D("%s: unknown type: %s", __FUNCTION__, glEnumToString(type))
            return 0;
    }
}

std::vector<uint32_t> quad_to_triangle(int n) {
    int num_quads = n / 4;
    int num_indices = num_quads * 6;

    std::vector<uint32_t> indices(num_indices, 0);

    for (int i = 0; i < num_quads; i++) {
        int base_index = i * 4;

        indices[i * 6 + 0] = base_index + 0;
        indices[i * 6 + 1] = base_index + 1;
        indices[i * 6 + 2] = base_index + 2;

        indices[i * 6 + 3] = base_index + 2;
        indices[i * 6 + 4] = base_index + 3;
        indices[i * 6 + 5] = base_index + 0;
    }

    return indices;
}

#if DEBUG || GLOBAL_DEBUG
void log_vtx_attrib_data(const void* ptr, GLenum type, int size, int stride, int offset, int idx) {
    const char* p = (const char*) ptr + idx * stride + offset;
    switch (type) {
        case GL_FLOAT: {
            LOG_D_N("(GL_FLOAT): (")
            const GLfloat *p_data = (const GLfloat *) p;
            for (int i = 0; i < size; ++i) {
                LOG_D_N("%.2f, ", p_data[i]);
            }
            LOG_D_N(") ")
            break;
        }
        case GL_UNSIGNED_BYTE: {
            LOG_D_N("(GL_UNSIGNED_BYTE): (")
            const GLubyte *p_data = (const GLubyte *) p;
            for (int i = 0; i < size; ++i) {
                LOG_D_N("%hhu, ", p_data[i]);
            }
            LOG_D_N(") ")
        }
    }
}
#endif

bool fpe_inited = false;
int init_fpe() {
    LOG_I("Initializing fixed-function pipeline...")

    char compile_info[1024];

    INIT_CHECK_GL_ERROR

//    g_glstate.fpe_vtx_shader_src =
//            "#version 320 es\n"
//            "precision highp float;\n"
//            "precision highp int;\n"
//            "uniform mat4 ModelViewMat;\n"
//            "uniform mat4 ProjMat;\n"
//            "layout (location = 0) in vec3 Pos;\n"
//            "layout (location = 1) in vec3 Normal;\n"
//            "layout (location = 2) in vec4 Color;\n"
//            "layout (location = 7) in vec2 UV0;\n"
//            "out vec4 vertexColor;\n"
//            "out vec2 texCoord0;\n"
//            "void main() {\n"
//            "   gl_Position = ProjMat * ModelViewMat * vec4(Pos, 1.0);\n"
//            "   vertexColor = Color;\n"
//            "   texCoord0 = UV0;\n"
//            "}\n";
//
//    g_glstate.fpe_frag_shader_src =
//            "#version 320 es\n"
//            "precision highp float;\n"
//            "precision highp int;\n"
//            "uniform sampler2D Sampler0;\n"
//            "in vec4 vertexColor;\n"
//            "in vec2 texCoord0;\n"
//            "out vec4 FragColor;\n"
//            "\n"
//            "void main() {\n"
//            "   vec4 color = texture(Sampler0, texCoord0) * vertexColor;"
//            "   if (color.a < 0.1) {\n"
//            "       discard;\n"
//            "   }\n"
//            "   FragColor = color;\n"
//            "}";
//
//    g_glstate.fpe_vtx_shader = GLES.glCreateShader(GL_VERTEX_SHADER);
//    CHECK_GL_ERROR_NO_INIT
//    GLES.glShaderSource(g_glstate.fpe_vtx_shader, 1, &g_glstate.fpe_vtx_shader_src, NULL);
//    CHECK_GL_ERROR_NO_INIT
//    GLES.glCompileShader(g_glstate.fpe_vtx_shader);
//    CHECK_GL_ERROR_NO_INIT
//    int success = 0;
//    GLES.glGetShaderiv(g_glstate.fpe_vtx_shader, GL_COMPILE_STATUS, &success);
//    CHECK_GL_ERROR_NO_INIT
//    if (!success) {
//        GLES.glGetShaderInfoLog(g_glstate.fpe_vtx_shader, 1024, NULL, compile_info);
//        CHECK_GL_ERROR_NO_INIT
//        LOG_E("fpe vertex shader compile error: %s", compile_info);
//        return -1;
//    }
//
//    g_glstate.fpe_frag_shader = GLES.glCreateShader(GL_FRAGMENT_SHADER);
//    CHECK_GL_ERROR_NO_INIT
//    GLES.glShaderSource(g_glstate.fpe_frag_shader, 1, &g_glstate.fpe_frag_shader_src, NULL);
//    CHECK_GL_ERROR_NO_INIT
//
//    GLES.glCompileShader(g_glstate.fpe_frag_shader);
//    CHECK_GL_ERROR_NO_INIT
//    GLES.glGetShaderiv(g_glstate.fpe_frag_shader, GL_COMPILE_STATUS, &success);
//    CHECK_GL_ERROR_NO_INIT
//    if (!success) {
//        GLES.glGetShaderInfoLog(g_glstate.fpe_frag_shader, 1024, NULL, compile_info);
//        CHECK_GL_ERROR_NO_INIT
//        LOG_E("fpe fragment shader compile error: %s", compile_info);
//        return -1;
//    }
//
//
//    g_glstate.fpe_program = GLES.glCreateProgram();
//    CHECK_GL_ERROR_NO_INIT
//    GLES.glAttachShader(g_glstate.fpe_program, g_glstate.fpe_vtx_shader);
//    CHECK_GL_ERROR_NO_INIT
//    GLES.glAttachShader(g_glstate.fpe_program, g_glstate.fpe_frag_shader);
//    CHECK_GL_ERROR_NO_INIT
//    GLES.glLinkProgram(g_glstate.fpe_program);
//    CHECK_GL_ERROR_NO_INIT
//    GLES.glGetProgramiv(g_glstate.fpe_program, GL_LINK_STATUS, &success);
//    CHECK_GL_ERROR_NO_INIT
//    if(!success) {
//        glGetProgramInfoLog(g_glstate.fpe_program, 1024, NULL, compile_info);
//        CHECK_GL_ERROR_NO_INIT
//        LOG_E("fpe program link error: %s", compile_info);
//        return -1;
//    }

    GLES.glGenVertexArrays(1, &g_glstate.fpe_state.fpe_vao);
    CHECK_GL_ERROR_NO_INIT
    GLES.glGenBuffers(1, &g_glstate.fpe_state.fpe_vbo);
    CHECK_GL_ERROR_NO_INIT
    GLES.glGenBuffers(1, &g_glstate.fpe_state.fpe_ibo);
    CHECK_GL_ERROR_NO_INIT

    LOG_D("fpe_vao: %d", g_glstate.fpe_state.fpe_vao)
    LOG_D("fpe_vbo: %d", g_glstate.fpe_state.fpe_vbo)
    LOG_D("fpe_ibo: %d", g_glstate.fpe_state.fpe_ibo)

    GLES.glBindVertexArray(g_glstate.fpe_state.fpe_vao);
    CHECK_GL_ERROR_NO_INIT
    GLES.glBindVertexArray(0);
    CHECK_GL_ERROR_NO_INIT

    return 0;
}

int commit_fpe_state_on_draw(GLenum* mode, GLint* first, GLsizei* count) {
    LOG()

    INIT_CHECK_GL_ERROR

    if (!fpe_inited) {
        if (init_fpe() != 0)
            abort();
        else
            fpe_inited = true;
    }

    auto& prog = g_glstate.get_or_generate_program();
    int prog_id = prog.get_program();
    if (prog_id < 0)
        LOG_D("Error: FPE shader link failed!")
    GLES.glUseProgram(prog_id);
    CHECK_GL_ERROR_NO_INIT

    bool is_first = true;

    // All assuming tightly packed here...
    auto& vpa = g_glstate.fpe_state.vertexpointer_array;
//    GLES.glGenVertexArrays(1, &vpa.fpe_vao);
    LOG_D("fpe_vao: %d", g_glstate.fpe_state.fpe_vao)
    GLES.glBindVertexArray(g_glstate.fpe_state.fpe_vao);
    CHECK_GL_ERROR_NO_INIT


    GLint prev_vbo = 0;
    GLES.glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev_vbo);
    CHECK_GL_ERROR_NO_INIT

    // Ugh...Why binding vbo is required BEFORE calling VertexAttrib* functions?
    if (prev_vbo == 0) {
        GLES.glBindBuffer(GL_ARRAY_BUFFER, g_glstate.fpe_state.fpe_vbo);
        CHECK_GL_ERROR_NO_INIT
    }

    if (vpa.dirty) {
        vpa.dirty = false;

        for (int i = 0; i < VERTEX_POINTER_COUNT; ++i) {
            bool enabled = ((vpa.enabled_pointers >> i) & 1);

            if (enabled) {
                auto &vp = vpa.attributes[i];

                if (is_first) {
                    vpa.starting_pointer = vp.pointer;
                    vpa.stride = vp.stride; // TODO: stride == 0?
                }

                const void* offset = (const void*)((const char*)vp.pointer - (const char*)vpa.starting_pointer);
                GLES.glVertexAttribPointer(i, vp.size, vp.type, vp.normalized, vp.stride, offset);
                CHECK_GL_ERROR_NO_INIT

                GLES.glEnableVertexAttribArray(i);
                CHECK_GL_ERROR_NO_INIT

                LOG_D("attrib #%d: type = %s, size = %d, stride = %d, usage = %s, ptr = %p",
                      i, glEnumToString(vp.type), vp.size, vp.stride, glEnumToString(vp.usage), vp.pointer)

                is_first = false;
            }
            else if (vpa.attributes[i].usage == GL_COLOR_ARRAY) {
                auto &vp = vpa.attributes[i];

                if (g_glstate.fpe_draw.current_data.sizes.color_size > 0) {
                    const auto& color = g_glstate.fpe_draw.current_data.color;
                    LOG_D("attrib #%d: type = %s, usage = %s, value = (%.2f, %.2f, %.2f, %.2f)",
                          i, glEnumToString(vp.type), glEnumToString(vp.usage),
                          color[0], color[1], color[2], color[3])

                    GLES.glVertexAttrib4fv(i, glm::value_ptr(color));
                    CHECK_GL_ERROR_NO_INIT
                }

                GLES.glDisableVertexAttribArray(i);
                CHECK_GL_ERROR_NO_INIT
            }
            else {
                GLES.glDisableVertexAttribArray(i);
                CLEAR_GL_ERROR_NO_INIT
                //CHECK_GL_ERROR_NO_INIT
            }
        }
    }

    int ret = 0;

    // Making sure it is a valid pointer rather than an offset into the buffer
    if (vpa.starting_pointer != nullptr && vpa.starting_pointer > (void*)vpa.stride) {
        LOG_D("VB @ 0x%x, size = %d * %d = %d", vpa.starting_pointer, *count, vpa.stride, *count * vpa.stride)

#if DEBUG || GLOBAL_DEBUG
            //    for (int j = 0; j < *count; ++j) {
//        for (int i = 0; i < VERTEX_POINTER_COUNT; ++i) {
//            bool enabled = ((vpa.enabled_pointers >> i) & 1);
//
//            if (!enabled)
//                continue;
//
//            auto &vp = vpa.pointers[i];
//
//            // const void* ptr, GLenum type, int size, int stride, int offset, int i
//            log_vtx_attrib_data(vpa.starting_pointer, vp.type, vp.size, vp.stride,
//                                (const char*)vp.pointer - (const char*)vpa.starting_pointer, j);
//
//        }
//        LOG_D("")
//    }
#endif

        LOG_D("glBufferData: size = %d, data = 0x%x -> GL_ARRAY_BUFFER (%d)", *count * vpa.stride,
              vpa.starting_pointer, g_glstate.fpe_state.fpe_vbo)

        GLES.glBufferData(GL_ARRAY_BUFFER, *count * vpa.stride, vpa.starting_pointer,
                          GL_DYNAMIC_DRAW);
        CHECK_GL_ERROR_NO_INIT
    } else {
        LOG_D("Using already bound VB")
    }

    if (*mode == GL_QUADS) {
        g_glstate.fpe_state.fpe_ib = quad_to_triangle(*count);

        LOG_D("glBufferData: size = %d, data = 0x%x -> GL_ELEMENT_ARRAY_BUFFER (%d)",
              g_glstate.fpe_state.fpe_ib.size() * sizeof(uint32_t), g_glstate.fpe_state.fpe_ib.data(), g_glstate.fpe_state.fpe_ibo)

        GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_glstate.fpe_state.fpe_ibo);
        CHECK_GL_ERROR_NO_INIT
        GLES.glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_glstate.fpe_state.fpe_ib.size() * sizeof(uint32_t),
                          g_glstate.fpe_state.fpe_ib.data(), GL_DYNAMIC_DRAW);
        CHECK_GL_ERROR_NO_INIT
        *count = g_glstate.fpe_state.fpe_ib.size();

        *mode = GL_TRIANGLES;
        ret = 1;
    }

    g_glstate.send_uniforms(prog_id);
    return ret;
}
