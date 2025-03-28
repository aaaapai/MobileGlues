//
// Created by Swung 0x48 on 2025/2/8.
//

#include "fpe.hpp"
#include "glm/gtc/type_ptr.hpp"

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

    GLES.glGenVertexArrays(1, &g_glstate.fpe_state.vertexpointer_array.fpe_vao);
    CHECK_GL_ERROR_NO_INIT
    GLES.glGenBuffers(1, &g_glstate.fpe_state.vertexpointer_array.fpe_vbo);
    CHECK_GL_ERROR_NO_INIT
    GLES.glGenBuffers(1, &g_glstate.fpe_state.vertexpointer_array.fpe_ibo);
    CHECK_GL_ERROR_NO_INIT

    LOG_D("fpe_vao: %d", g_glstate.fpe_state.vertexpointer_array.fpe_vao)
    LOG_D("fpe_vbo: %d", g_glstate.fpe_state.vertexpointer_array.fpe_vbo)
    LOG_D("fpe_ibo: %d", g_glstate.fpe_state.vertexpointer_array.fpe_ibo)

    GLES.glBindVertexArray(g_glstate.fpe_state.vertexpointer_array.fpe_vao);
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

//    fpe_shader_generator gen(g_glstate.fpe_state);
//    program_t program = gen.generate_program();
//    LOG_D("Generated VS: \n%s", program.vs.c_str())
//    LOG_D("Generated FS: \n%s", program.fs.c_str())

    // TODO: Make a proper hash
    uint32_t key = g_glstate.fpe_state.vertexpointer_array.enabled_pointers;
    key |= ((g_glstate.fpe_state.fpe_bools.fog_enable & 1) << 31);
    if (g_glstate.fpe_programs.find(key)
        == g_glstate.fpe_programs.end()) {
        LOG_D("Generating new shader: 0x%x", key)
        fpe_shader_generator gen(g_glstate.fpe_state);
        g_glstate.fpe_programs[key] = gen.generate_program();
    }
    auto& prog = g_glstate.fpe_programs[key];
    int prog_id = prog.get_program();
    if (prog_id < 0)
        LOG_D("Error: FPE shader link failed!")
    GLES.glUseProgram(prog_id);
    CHECK_GL_ERROR_NO_INIT

    bool is_first = true;

    // All assuming tightly packed here...
    auto& vpa = g_glstate.fpe_state.vertexpointer_array;
//    GLES.glGenVertexArrays(1, &vpa.fpe_vao);
    LOG_D("fpe_vao: %d", g_glstate.fpe_state.vertexpointer_array.fpe_vao)
    GLES.glBindVertexArray(vpa.fpe_vao);
    CHECK_GL_ERROR_NO_INIT


    GLint prev_vbo = 0;
    GLES.glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev_vbo);
    CHECK_GL_ERROR_NO_INIT

    // Ugh...Why binding vbo is required BEFORE calling VertexAttrib* functions?
    if (prev_vbo == 0) {
        GLES.glBindBuffer(GL_ARRAY_BUFFER, g_glstate.fpe_state.vertexpointer_array.fpe_vbo);
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

                LOG_D("attrib #%d: type = %s, size = %d, stride = %d, usage = %s, ptr = 0x%x",
                      i, glEnumToString(vp.type), vp.size, vp.stride, glEnumToString(vp.usage))

                is_first = false;
            }
            else if (vpa.attributes[i].usage == GL_COLOR_ARRAY) {
                auto &vp = vpa.attributes[i];

                LOG_D("attrib #%d: type = %s, usage = %s, value = (%.2f, %.2f, %.2f, %.2f)",
                      i, glEnumToString(vp.type), glEnumToString(vp.usage),
                      vp.value[0], vp.value[1], vp.value[2], vp.value[3])

                GLES.glVertexAttrib4fv(i, glm::value_ptr(vp.value));
                CHECK_GL_ERROR_NO_INIT

                GLES.glDisableVertexAttribArray(i);
                CHECK_GL_ERROR_NO_INIT
            }
            else {
                GLES.glDisableVertexAttribArray(i);
                CHECK_GL_ERROR_NO_INIT
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
              vpa.starting_pointer, g_glstate.fpe_state.vertexpointer_array.fpe_vbo)

        GLES.glBufferData(GL_ARRAY_BUFFER, *count * vpa.stride, vpa.starting_pointer,
                          GL_DYNAMIC_DRAW);
        CHECK_GL_ERROR_NO_INIT
    } else {
        LOG_D("Using already bound VB")
    }

    if (*mode == GL_QUADS) {
        vpa.fpe_ibo_buffer = quad_to_triangle(*count);

        LOG_D("glBufferData: size = %d, data = 0x%x -> GL_ELEMENT_ARRAY_BUFFER (%d)",
              vpa.fpe_ibo_buffer.size() * sizeof(uint32_t), vpa.fpe_ibo_buffer.data(), g_glstate.fpe_state.vertexpointer_array.fpe_ibo)

        GLES.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_glstate.fpe_state.vertexpointer_array.fpe_ibo);
        CHECK_GL_ERROR_NO_INIT
        GLES.glBufferData(GL_ELEMENT_ARRAY_BUFFER, vpa.fpe_ibo_buffer.size() * sizeof(uint32_t),
                          vpa.fpe_ibo_buffer.data(), GL_DYNAMIC_DRAW);
        CHECK_GL_ERROR_NO_INIT
        *count = vpa.fpe_ibo_buffer.size();

        *mode = GL_TRIANGLES;
        ret = 1;
    }

    const auto& mv = g_glstate.fpe_uniform.transformation.matrices[matrix_idx(GL_MODELVIEW)];
    const auto& proj = g_glstate.fpe_uniform.transformation.matrices[matrix_idx(GL_PROJECTION)];

    LOG_D("GL_MODELVIEW: ")
    print_matrix(mv);
    LOG_D("GL_PROJECTION: ")
    print_matrix(proj);

    // TODO: detect change and only set dirty bits here
    GLES.glBindVertexArray(g_glstate.fpe_state.vertexpointer_array.fpe_vao);
    CHECK_GL_ERROR_NO_INIT
    GLint mvmat = GLES.glGetUniformLocation(prog_id, "ModelViewMat");
    CHECK_GL_ERROR_NO_INIT
//    GLint projmat = GLES.glGetUniformLocation(prog_id, "ProjMat");
//    CHECK_GL_ERROR_NO_INIT
    GLint mat_id = GLES.glGetUniformLocation(prog_id, "ModelViewProjMat");
    CHECK_GL_ERROR_NO_INIT
    const auto mat = proj * mv;
    GLES.glUniformMatrix4fv(mvmat, 1, GL_FALSE, glm::value_ptr(g_glstate.fpe_uniform.transformation.matrices[matrix_idx(GL_MODELVIEW)]));
    CHECK_GL_ERROR_NO_INIT
//    GLES.glUniformMatrix4fv(projmat, 1, GL_FALSE, glm::value_ptr(g_glstate.fpe_uniform.transformation.matrices[matrix_idx(GL_PROJECTION)]));
//    CHECK_GL_ERROR_NO_INIT
    GLES.glUniformMatrix4fv(mat_id, 1, GL_FALSE, glm::value_ptr(mat));
    CHECK_GL_ERROR_NO_INIT
    GLES.glUniform1i(GLES.glGetUniformLocation(prog_id, "Sampler0"), 0);

    if (g_glstate.fpe_state.fpe_bools.fog_enable) {
        GLint fogcolor_id = GLES.glGetUniformLocation(prog_id, "fogParam.color");
        CHECK_GL_ERROR_NO_INIT
        LOG_D("fogcolor_id = %d", fogcolor_id)
        GLES.glUniform4fv(fogcolor_id, 1, glm::value_ptr(g_glstate.fpe_uniform.fog_color));
        CHECK_GL_ERROR_NO_INIT
        GLint fogdensity_id = GLES.glGetUniformLocation(prog_id, "fogParam.density");
        CHECK_GL_ERROR_NO_INIT
        GLES.glUniform1f(fogdensity_id, g_glstate.fpe_uniform.fog_density);
        CHECK_GL_ERROR_NO_INIT
        GLint fogstart_id = GLES.glGetUniformLocation(prog_id, "fogParam.start");
        CHECK_GL_ERROR_NO_INIT
        GLES.glUniform1f(fogstart_id, g_glstate.fpe_uniform.fog_start);
        CHECK_GL_ERROR_NO_INIT
        GLint fogend_id = GLES.glGetUniformLocation(prog_id, "fogParam.end");
        CHECK_GL_ERROR_NO_INIT
        GLES.glUniform1f(fogend_id, g_glstate.fpe_uniform.fog_end);
        CHECK_GL_ERROR_NO_INIT
    }

    return ret;
}
