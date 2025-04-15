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

    INIT_CHECK_GL_ERROR

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

    auto key = g_glstate.program_hash();
    LOG_D("%s: key=0x%x", __func__, key)
    auto& prog = g_glstate.get_or_generate_program(key);
    int prog_id = prog.get_program();
    if (prog_id < 0)
        LOG_D("Error: FPE shader link failed!")
    GLES.glUseProgram(prog_id);
    CHECK_GL_ERROR_NO_INIT

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

    LOG_D("starting_ptr = %p", vpa.starting_pointer)
    LOG_D("stride = %d", vpa.stride)
//    vpa.normalize();
    g_glstate.send_vertex_attributes();

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
    vpa.reset();
//    vpa.starting_pointer = 0;
//    vpa.stride = 0;
    return ret;
}
