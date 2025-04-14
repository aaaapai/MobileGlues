//
// Created by Swung 0x48 on 2025/4/11.
//
#include "types.h"
#include "transformation.h"
#include "xxhash32.h"

#define DEBUG 0

void glstate_t::send_uniforms(int program) {
    LOG()
    INIT_CHECK_GL_ERROR
    const auto& mv = fpe_uniform.transformation.matrices[matrix_idx(GL_MODELVIEW)];
    const auto& proj = fpe_uniform.transformation.matrices[matrix_idx(GL_PROJECTION)];

    LOG_D("GL_MODELVIEW: ")
    print_matrix(mv);
    LOG_D("GL_PROJECTION: ")
    print_matrix(proj);

    // TODO: detect change and only set dirty bits here
    GLES.glBindVertexArray(fpe_state.fpe_vao);
    CHECK_GL_ERROR_NO_INIT
    GLint mvmat = GLES.glGetUniformLocation(program, "ModelViewMat");
    CHECK_GL_ERROR_NO_INIT
//    GLint projmat = GLES.glGetUniformLocation(program, "ProjMat");
//    CHECK_GL_ERROR_NO_INIT
    GLint mat_id = GLES.glGetUniformLocation(program, "ModelViewProjMat");
    CHECK_GL_ERROR_NO_INIT
    const auto mat = proj * mv;
    GLES.glUniformMatrix4fv(mvmat, 1, GL_FALSE, glm::value_ptr(fpe_uniform.transformation.matrices[matrix_idx(GL_MODELVIEW)]));
    CHECK_GL_ERROR_NO_INIT
//    GLES.glUniformMatrix4fv(projmat, 1, GL_FALSE, glm::value_ptr(fpe_uniform.transformation.matrices[matrix_idx(GL_PROJECTION)]));
//    CHECK_GL_ERROR_NO_INIT
    GLES.glUniformMatrix4fv(mat_id, 1, GL_FALSE, glm::value_ptr(mat));
    CHECK_GL_ERROR_NO_INIT
    GLES.glUniform1i(GLES.glGetUniformLocation(program, "Sampler0"), 0);
    CHECK_GL_ERROR_NO_INIT

    if (fpe_state.fpe_bools.fog_enable) {
        GLint fogcolor_id = GLES.glGetUniformLocation(program, "fogParam.color");
        CHECK_GL_ERROR_NO_INIT
        LOG_D("fogcolor_id = %d", fogcolor_id)
        GLES.glUniform4fv(fogcolor_id, 1, glm::value_ptr(fpe_uniform.fog_color));
        CHECK_GL_ERROR_NO_INIT
        GLint fogdensity_id = GLES.glGetUniformLocation(program, "fogParam.density");
        CHECK_GL_ERROR_NO_INIT
        GLES.glUniform1f(fogdensity_id, fpe_uniform.fog_density);
        CHECK_GL_ERROR_NO_INIT
        GLint fogstart_id = GLES.glGetUniformLocation(program, "fogParam.start");
        CHECK_GL_ERROR_NO_INIT
        GLES.glUniform1f(fogstart_id, fpe_uniform.fog_start);
        CHECK_GL_ERROR_NO_INIT
        GLint fogend_id = GLES.glGetUniformLocation(program, "fogParam.end");
        CHECK_GL_ERROR_NO_INIT
        GLES.glUniform1f(fogend_id, fpe_uniform.fog_end);
        CHECK_GL_ERROR_NO_INIT
    }
}

uint32_t glstate_t::program_hash() {
    uint64_t key = 0;
    key |= vertex_attrib_hash();

    XXHash32 hash(s_hash_seed);
    hash.add(&fpe_state.client_active_texture, sizeof(fpe_state.client_active_texture));
    hash.add(&fpe_state.alpha_func, sizeof(fpe_state.alpha_func));
    hash.add(&fpe_state.fog_mode, sizeof(fpe_state.fog_mode));
    hash.add(&fpe_state.fog_index, sizeof(fpe_state.fog_index));
    hash.add(&fpe_state.fog_coord_src, sizeof(fpe_state.fog_coord_src));
    hash.add(&fpe_state.shade_model, sizeof(fpe_state.shade_model));
    hash.add(&fpe_state.light_model_color_ctrl, sizeof(fpe_state.light_model_color_ctrl));
    hash.add(&fpe_state.light_model_local_viewer, sizeof(fpe_state.light_model_local_viewer));
    hash.add(&fpe_state.light_model_two_side, sizeof(fpe_state.light_model_two_side));

    key |= (((uint64_t)hash.hash()) << 32);

    return key;
}

uint32_t glstate_t::vertex_attrib_hash() {
    XXHash32 hash(s_hash_seed);

    auto& va = fpe_state.vertexpointer_array;

    hash.add(&va.starting_pointer, sizeof(va.starting_pointer));
    for (int i = 0; i < VERTEX_POINTER_COUNT; ++i) {
        bool enabled = ((va.enabled_pointers >> i) & 1);
        if (enabled || fpe_state.fpe_draw.current_data.sizes.data[i] > 0) {
            hash.add(&i, sizeof(i));
            hash.add(&enabled, sizeof(enabled));
            auto &attr = va.attributes[i];

            if (enabled)
                hash.add(&attr.size, sizeof(attr.size));
            else
                hash.add(&fpe_state.fpe_draw.current_data.sizes.data[i], sizeof(fpe_state.fpe_draw.current_data.sizes.data[i]));

            hash.add(&attr.usage, sizeof(attr.usage));

            if (enabled) {
                hash.add(&attr.type, sizeof(attr.type));
                hash.add(&attr.normalized, sizeof(attr.normalized));
//                hash.add(&attr.stride, sizeof(attr.stride));
                hash.add(&attr.pointer, sizeof(attr.pointer));
            }
            else {
                const GLenum t = GL_FLOAT;
                hash.add(&t, sizeof(t));
            }
        }
    }

    uint32_t result = hash.hash();
    return result;
}

program_t& glstate_t::get_or_generate_program(const uint64_t key) {
    LOG()
    if (fpe_programs.find(key)
        == fpe_programs.end()) {
        LOG_D("Generating new shader: 0x%x", key)
        fpe_shader_generator gen(fpe_state);
        program_t program = gen.generate_program();
        int prog_id = program.get_program();
        if (prog_id > 0)
            fpe_programs[key] = program;
        else {
            LOG_D("Error: FPE shader link failed!")
            // reserve key==0 as null program for failure
            return fpe_programs[0];
        }
    }

    auto& prog = fpe_programs[key];
    return prog;
}

int glstate_t::get_vao(const uint64_t key) {
    LOG()
    if (fpe_vaos.find(key)
        == fpe_vaos.end()) {
        return -1;
    }

    return fpe_vaos[key];
}

void glstate_t::save_vao(const uint64_t key, const GLuint vao) {
    LOG()
    fpe_vaos[key] = vao;
}

void glstate_t::send_vertex_attributes() {
    LOG()
    INIT_CHECK_GL_ERROR
    auto& va = fpe_state.vertexpointer_array;
    if (!va.dirty) return;

    va.dirty = false;

    va.normalize();

    for (int i = 0; i < VERTEX_POINTER_COUNT; ++i) {
        bool enabled = ((va.enabled_pointers >> i) & 1);

        auto &vp = va.attributes[i];
        if (enabled) {
            vp.stride = va.stride;

            GLES.glVertexAttribPointer(
                    i, vp.size, vp.type, vp.normalized, vp.stride, vp.pointer);
            CHECK_GL_ERROR_NO_INIT

            GLES.glEnableVertexAttribArray(i);
            CHECK_GL_ERROR_NO_INIT

            LOG_D("attrib #%d: type = %s, size = %d, stride = %d, usage = %s, ptr = %p",
                  i, glEnumToString(vp.type), vp.size, vp.stride, glEnumToString(vp.usage), vp.pointer)
        }
        else {
            switch (idx2vp(i)) {
                case GL_COLOR_ARRAY:
                    if (fpe_state.fpe_draw.current_data.sizes.color_size > 0) {
                        const auto& v = fpe_state.fpe_draw.current_data.color;
                        LOG_D("attrib #%d: type = %s, usage = %s, value = (%.2f, %.2f, %.2f, %.2f) (disabled)",
                              i, glEnumToString(vp.type), glEnumToString(vp.usage),
                              v[0], v[1], v[2], v[3])

                        GLES.glVertexAttrib4fv(i, glm::value_ptr(v));
                        CHECK_GL_ERROR_NO_INIT
                    }
                    break;
                case GL_NORMAL_ARRAY:
                    if (fpe_state.fpe_draw.current_data.sizes.normal_size > 0) {
                        const auto& v = fpe_state.fpe_draw.current_data.normal;
                        LOG_D("attrib #%d: type = %s, usage = %s, value = (%.2f, %.2f, %.2f, %.2f) (disabled)",
                              i, glEnumToString(vp.type), glEnumToString(vp.usage),
                              v[0], v[1], v[2], v[3])

                        GLES.glVertexAttrib4fv(i, glm::value_ptr(v));
                        CHECK_GL_ERROR_NO_INIT
                    }
                    break;
                default:
                    LOG_D("attrib #%d: (disabled)", i)
                    break;
            }

            GLES.glDisableVertexAttribArray(i);
            CHECK_GL_ERROR_NO_INIT
        }
    }
}
