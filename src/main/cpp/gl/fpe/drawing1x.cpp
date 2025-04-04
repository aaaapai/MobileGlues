//
// Created by Swung 0x48 on 2025/4/1.
//

#include <glm/gtc/type_ptr.hpp>
#include "drawing1x.h"
#include "fpe.hpp"
#include "list.h"

#define DEBUG 0

// act a bit like gl*Pointer
void populate_vertex_pointer(GLenum array) {
    auto& va = g_glstate.fpe_state.vertexpointer_array;
    switch (array) {
        case GL_ARRAY_BUFFER:
            va.attributes[vp2idx(GL_ARRAY_BUFFER)] = {
                    .size = 4,
                    .usage = GL_ARRAY_BUFFER,
                    .type = GL_FLOAT,
                    .normalized = GL_FALSE,
                    .stride = 0,
                    .pointer = 0,
                    .varies = true
            };
            va.stride += 4 * sizeof(GLfloat);
            break;
        case GL_NORMAL_ARRAY:
            va.attributes[vp2idx(GL_NORMAL_ARRAY)] = {
                    .size = 4,
                    .usage = GL_NORMAL_ARRAY,
                    .type = GL_FLOAT,
                    .normalized = GL_FALSE,
                    .stride = 0,
                    .pointer = 0,
                    .varies = true
            };
            va.stride += 4 * sizeof(GLfloat);
            break;
        case GL_TEXTURE_COORD_ARRAY:
            va.attributes[vp2idx(GL_TEXTURE_COORD_ARRAY)] = {
                    .size = 4,
                    .usage = GL_TEXTURE_COORD_ARRAY + (g_glstate.fpe_state.client_active_texture - GL_TEXTURE0),
                    .type = GL_FLOAT,
                    .normalized = GL_FALSE,
                    .stride = 0,
                    .pointer = 0,
                    .varies = true
            };
            va.stride += 4 * sizeof(GLfloat);
            break;
    }
    g_glstate.fpe_state.vertexpointer_array.dirty = true;
}

void glBegin( GLenum mode ) {
    LOG()
    LOG_D("glBegin(%s)", glEnumToString(mode))

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glBegin>({}, mode);
        if (DisplayListManager::shouldFinish())
            return;
    }

//    if (!fpe_inited) {
//        if (init_fpe() != 0)
//            abort();
//        else
//            fpe_inited = true;
//    }

    auto& s = g_glstate.fpe_draw;

    if (s.primitive != GL_NONE) {
        LOG_D("glBegin() nested! \nLast mode: %s", glEnumToString(s.primitive))
        return;
    }

    s.primitive = mode;
}

void glEnd() {
    LOG()
    LOG_D("glEnd()")

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glEnd>({});
        if (DisplayListManager::shouldFinish())
            return;
    }

    INIT_CHECK_GL_ERROR

    auto& s = g_glstate.fpe_draw;
    auto& va = g_glstate.fpe_state.vertexpointer_array;
    auto& vb = g_glstate.fpe_state.fpe_vb;

    if (s.primitive == GL_NONE) {
        LOG_E("glEnd() without effect: already ended");
        return;
    }
    
    GET_PREV_PROGRAM
    
    // actual assembly work, and draw!
    {
        // Program
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

        // VAO, VB
        GLES.glBindVertexArray(g_glstate.fpe_state.fpe_vao);
        CHECK_GL_ERROR_NO_INIT
        GLES.glBindBuffer(GL_ARRAY_BUFFER, g_glstate.fpe_state.fpe_vbo);
        CHECK_GL_ERROR_NO_INIT
        auto vbbuf = vb.str();
        GLES.glBufferData(GL_ARRAY_BUFFER, vbbuf.size(), vbbuf.c_str(), GL_DYNAMIC_DRAW);
        CHECK_GL_ERROR_NO_INIT

        // Vertex Pointer
        if (va.dirty) {
            va.dirty = false;

            size_t offset = 0;
            for (int i = 0; i < VERTEX_POINTER_COUNT; ++i) {
                bool enabled = ((va.enabled_pointers >> i) & 1);

                if (enabled) {
                    auto &vp = va.attributes[i];

                    GLES.glVertexAttribPointer(i, vp.size, vp.type, vp.normalized, vp.stride, (const void*)offset);
                    CHECK_GL_ERROR_NO_INIT

                    GLES.glEnableVertexAttribArray(i);
                    CHECK_GL_ERROR_NO_INIT

                    LOG_D("attrib #%d: type = %s, size = %d, stride = %d, usage = %s, ptr = 0x%x",
                          i, glEnumToString(vp.type), vp.size, vp.stride, glEnumToString(vp.usage))

                    offset += (vp.size * type_size(vp.type));
                }
                else {
                    GLES.glDisableVertexAttribArray(i);
                    CHECK_GL_ERROR_NO_INIT
                }
            }
        }

        // Uniform
//        update_fpe_uniforms(prog_id);
        {
            const auto& mv = g_glstate.fpe_uniform.transformation.matrices[matrix_idx(GL_MODELVIEW)];
            const auto& proj = g_glstate.fpe_uniform.transformation.matrices[matrix_idx(GL_PROJECTION)];

            //    LOG_D("GL_MODELVIEW: ")
            //    print_matrix(mv);
            //    LOG_D("GL_PROJECTION: ")
            //    print_matrix(proj);

            // TODO: detect change and only set dirty bits here
            GLES.glBindVertexArray(g_glstate.fpe_state.fpe_vao);
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
        }

        // Draw
        GLES.glDrawArrays(s.primitive, 0, s.vertex_count);
        CHECK_GL_ERROR_NO_INIT
    }

    SET_PREV_PROGRAM
    
    // resetting draw state
    s.reset();
    va.enabled_pointers = 0;
    vb.clear();
}

void glNormal3f( GLfloat nx, GLfloat ny, GLfloat nz ) {
    LOG()
    LOG_D("glNormal3f(%.2f, %.2f, %.2f)", nx, ny, nz)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glNormal3f>({}, nx, ny, nz);
        if (DisplayListManager::shouldFinish())
            return;
    }

    auto& state = g_glstate.fpe_draw;
    auto& va = g_glstate.fpe_state.vertexpointer_array;

    state.normal = glm::vec3(nx, ny, nz);
    auto mask = vp_mask(GL_NORMAL_ARRAY);
    auto index = vp2idx(GL_NORMAL_ARRAY);
    if (!(va.enabled_pointers & mask)) {
        va.enabled_pointers |= mask;
        populate_vertex_pointer(GL_NORMAL_ARRAY);
    }
}

void glTexCoord2f( GLfloat s, GLfloat t ) {
    LOG()
    LOG_D("glTexCoord2f(%.2f, %.2f)", s, t)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glTexCoord2f>({}, s, t);
        if (DisplayListManager::shouldFinish())
            return;
    }

    SELF_CALL(glTexCoord4f, s, t, 0, 1);
}

void glTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q ) {
    LOG()
    LOG_D("glTexCoord4f(%.2f, %.2f, %.2f, %.2f)", s, t, r, q)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glTexCoord4f>({}, s, t, r, q);
        if (DisplayListManager::shouldFinish())
            return;
    }

    auto& state = g_glstate.fpe_draw;
    auto& va = g_glstate.fpe_state.vertexpointer_array;

    state.texcoord[0] = glm::vec4(s, t, r, q);
    auto mask = vp_mask(GL_TEXTURE_COORD_ARRAY);
    auto index = vp2idx(GL_TEXTURE_COORD_ARRAY);
    if (!(va.enabled_pointers & mask)) {
        va.enabled_pointers |= mask;
        populate_vertex_pointer(GL_TEXTURE_COORD_ARRAY);
    }
}

void glMultiTexCoord2f( GLenum target, GLfloat s, GLfloat t ) {
    LOG()
    LOG_D("glMultiTexCoord2f(%s, %.2f, %.2f)", glEnumToString(target), s, t)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glMultiTexCoord2f>({}, target, s, t);
        if (DisplayListManager::shouldFinish())
            return;
    }

    SELF_CALL(glMultiTexCoord4f, target, s, t, 0, 1);
}

// Todo: target - GL_TEXTURE0
void glMultiTexCoord4f( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q ) {
    LOG()
    LOG_D("glMultiTexCoord4f(%s, %.2f, %.2f, %.2f, %.2f)", glEnumToString(target), s, t, r, q)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glMultiTexCoord4f>({}, target, s, t, r, q);
        if (DisplayListManager::shouldFinish())
            return;
    }

    auto& state = g_glstate.fpe_draw;
    auto& va = g_glstate.fpe_state.vertexpointer_array;

    state.texcoord[target - GL_TEXTURE0] = glm::vec4(s, t, r, q);
    auto mask = vp_mask(GL_TEXTURE_COORD_ARRAY);
    auto index = vp2idx(GL_TEXTURE_COORD_ARRAY);
    if (!(va.enabled_pointers & mask)) {
        va.enabled_pointers |= mask;
        populate_vertex_pointer(GL_TEXTURE_COORD_ARRAY);
    }
}

void glVertex3f( GLfloat x, GLfloat y, GLfloat z ) {
    LOG()
    LOG_D("glVertex3f(%.2f, %.2f, %.2f)", x, y, z)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glVertex3f>({}, x, y, z);
        if (DisplayListManager::shouldFinish())
            return;
    }

    SELF_CALL(glVertex4f, x, y, z, 1.f);
}

void glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
    LOG()
    LOG_D("glVertex4f(%.2f, %.2f, %.2f, %.2f)", x, y, z, w)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glVertex4f>({}, x, y, z, w);
        if (DisplayListManager::shouldFinish())
            return;
    }

    auto& drawstate = g_glstate.fpe_draw;

    drawstate.vertex_count++;

    // assuming vertex layout won't change since here
    auto& va = g_glstate.fpe_state.vertexpointer_array;
    auto& vb = g_glstate.fpe_state.fpe_vb;

    auto mask = vp_mask(GL_VERTEX_ARRAY);
    if (!(va.enabled_pointers & mask)) {
        va.enabled_pointers |= vp_mask(GL_VERTEX_ARRAY);
        populate_vertex_pointer(GL_VERTEX_ARRAY);
    }

    // Put in 1 vertex (with attributes)
    for (int i = 0; i < VERTEX_POINTER_COUNT; ++i) {
        bool enabled = ((va.enabled_pointers >> i) & 1);

        if (enabled) {
            auto &vp = va.attributes[i];

            // Fill in data of this attribute
            switch (vp.usage) {
                case GL_VERTEX_ARRAY:
                    vb.write((const char*)&x, sizeof(GLfloat));
                    vb.write((const char*)&y, sizeof(GLfloat));
                    vb.write((const char*)&z, sizeof(GLfloat));
                    vb.write((const char*)&w, sizeof(GLfloat));
                    break;
                case GL_TEXTURE_COORD_ARRAY + 0:
                case GL_TEXTURE_COORD_ARRAY + 1:
                case GL_TEXTURE_COORD_ARRAY + 2:
                case GL_TEXTURE_COORD_ARRAY + 3:
                case GL_TEXTURE_COORD_ARRAY + 4:
                case GL_TEXTURE_COORD_ARRAY + 5:
                case GL_TEXTURE_COORD_ARRAY + 6:
                case GL_TEXTURE_COORD_ARRAY + 7: {
                    auto& texcoord = drawstate.texcoord[vp.usage - GL_TEXTURE_COORD_ARRAY];
                    vb.write((const char*)glm::value_ptr(texcoord), sizeof(GLfloat) * 4);
                }
            }
        }
    }
}

void glColor3f( GLfloat red, GLfloat green, GLfloat blue ) {
    LOG()
    LOG_D("glColor3f(%f, %f, %f)", red, green, blue)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glColor3f>({}, red, green, blue);
        if (DisplayListManager::shouldFinish())
            return;
    }

    SELF_CALL(glColor4f, red, green, blue, 1)
}

void glColor4f( GLfloat red, GLfloat green,
                GLfloat blue, GLfloat alpha ) {
    LOG()
    /* TODO: This can cause problem if per-vertex color is used,
     *  (using multiple glVertex*() calls in conjunction with
     *  multiple glColor4f() calls, rather than a single glColor4f()
     *  call and some gl*Pointer() call)
     *  Fix for that situation later.
    */
    LOG_D("glColor4f(%f, %f, %f, %f)", red, green, blue, alpha)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glColor4f>({}, red, green, blue, alpha);
        if (DisplayListManager::shouldFinish())
            return;
    }

    auto& attr = g_glstate.fpe_state.vertexpointer_array.attributes[vp2idx(GL_COLOR_ARRAY)];
    auto& vpa = g_glstate.fpe_state.vertexpointer_array;
    vpa.enabled_pointers;
    if (vpa.buffer_based) {
        attr.size = 4;
        attr.usage = GL_COLOR_ARRAY;
        attr.type = GL_FLOAT;
        attr.normalized = GL_FALSE;
        attr.stride = 0;
        attr.pointer = 0;
        attr.value = glm::vec4(red, green, blue, alpha);
        attr.varies = false;
    } else {
        LOG_D("Not implemented!")
    }
}

