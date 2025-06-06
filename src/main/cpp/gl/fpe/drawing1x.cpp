//
// Created by Swung 0x48 on 2025/4/1.
//

#include <glm/glm/gtc/type_ptr.hpp>
#include "drawing1x.h"
#include "list.h"
#include <bit>

#define DEBUG 0

void glBegin( GLenum mode ) {
    LOG()
    LOG_D("glBegin(%s)", glEnumToString(mode))

    LIST_RECORD(glBegin, {}, mode)

    if (!fpe_inited) {
        if (init_fpe() != 0)
            abort();
        else
            fpe_inited = true;
    }

    auto& s = g_glstate.fpe_state.fpe_draw;

    if (s.primitive != GL_NONE) {
        LOG_D("glBegin() nested! \nLast mode: %s", glEnumToString(s.primitive))
        return;
    }

    s.primitive = mode;
}

void glEnd() {
    LOG()
    LOG_D("glEnd()")

    LIST_RECORD(glEnd, {})

    INIT_CHECK_GL_ERROR

    auto& s = g_glstate.fpe_state.fpe_draw;
    auto& raw_va = g_glstate.fpe_state.vertexpointer_array;
//    auto& vb = g_glstate.fpe_state.fpe_vb;
    auto& vb = g_glstate.fpe_state.fpe_draw.vb;

    if (s.primitive == GL_NONE) {
        LOG_E("glEnd() without effect: already ended");
        return;
    }
    
    GET_PREV_PROGRAM
    
    // actual assembly work, and draw!
    {
        // Vertex Pointer State Machine Update
        g_glstate.fpe_state.fpe_draw.compile_vertexattrib(raw_va);

        auto& va = g_glstate.fpe_state.normalized_vpa;
        va = raw_va.normalize();
        // Need to generate_compressed_index first (shadergen will use that)
        va.generate_compressed_index(g_glstate.fpe_state.fpe_draw.current_data.sizes.data);

        auto key = g_glstate.program_hash();

        // Program
        auto& prog = g_glstate.get_or_generate_program(key);

        int prog_id = prog.get_program();
        if (prog_id < 0) {
            LOG_D("Error: FPE shader link failed!")
        }
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

        // Vertex Pointer to ES
        g_glstate.send_vertex_attributes(va);

        // Uniform
        {
            g_glstate.send_uniforms(prog_id);
        }

        // Draw
        LOG_D("glEnd: glDrawArrays(%s, %d, %d), vb = %d, vb size = %d", glEnumToString(s.primitive), 0, s.vertex_count, g_glstate.fpe_state.fpe_vbo, vbbuf.size())
        GLES.glDrawArrays(s.primitive, 0, s.vertex_count);
        CHECK_GL_ERROR_NO_INIT
    }

    SET_PREV_PROGRAM
    
    // resetting draw state
    s.reset();
    raw_va.reset();
}

void glNormal3f( GLfloat nx, GLfloat ny, GLfloat nz ) {
    LOG()
    LOG_D("glNormal3f(%.2f, %.2f, %.2f)", nx, ny, nz)

    LIST_RECORD(glNormal3f, {}, nx, ny, nz)

    mglNormal<GLfloat, 3>({nx, ny, nz});
}

void glTexCoord2f( GLfloat s, GLfloat t ) {
    LOG()
    LOG_D("glTexCoord2f(%.2f, %.2f)", s, t)

    LIST_RECORD(glTexCoord2f, {}, s, t)

    mglTexCoord<GLfloat, 2>({s, t}, 0);
}

void glTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q ) {
    LOG()
    LOG_D("glTexCoord4f(%.2f, %.2f, %.2f, %.2f)", s, t, r, q)

    LIST_RECORD(glTexCoord4f, {}, s, t, r, q)

    mglTexCoord<GLfloat, 4>({s, t, r, q}, 0);
}

void glMultiTexCoord2f( GLenum target, GLfloat s, GLfloat t ) {
    LOG()
    LOG_D("glMultiTexCoord2f(%s, %.2f, %.2f)", glEnumToString(target), s, t)

    LIST_RECORD(glMultiTexCoord2f, {}, target, s, t)

    assert(target - GL_TEXTURE0 < MAX_TEX);
    mglTexCoord<GLfloat, 2>({s, t}, target - GL_TEXTURE0);
}

void glMultiTexCoord4f( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q ) {
    LOG()
    LOG_D("glMultiTexCoord4f(%s, %.2f, %.2f, %.2f, %.2f)", glEnumToString(target), s, t, r, q)

    LIST_RECORD(glMultiTexCoord4f, {}, target, s, t, r, q)

    assert(target - GL_TEXTURE0 < MAX_TEX);
    mglTexCoord<GLfloat, 4>({s, t, r, q}, target - GL_TEXTURE0);
}

void glVertex3f( GLfloat x, GLfloat y, GLfloat z ) {
    LOG()
    LOG_D("glVertex3f(%.2f, %.2f, %.2f)", x, y, z)

    LIST_RECORD(glVertex3f, {}, x, y, z)

    mglVertex<GLfloat, 3>({x, y, z});
}

void glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
    LOG()
    LOG_D("glVertex4f(%.2f, %.2f, %.2f, %.2f)", x, y, z, w)

    LIST_RECORD(glVertex4f, {}, x, y, z, w)

    mglVertex<GLfloat, 4>({x, y, z, w});
}

void glColor3f( GLfloat red, GLfloat green, GLfloat blue ) {
    LOG()
    LOG_D("glColor3f(%f, %f, %f)", red, green, blue)

    LIST_RECORD(glColor3f, {}, red, green, blue)

    mglColor<GLfloat, 3>({red, green, blue});
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

    LIST_RECORD(glColor4f, {}, red, green, blue, alpha)

//    auto& attr = g_glstate.fpe_state.vertexpointer_array.attributes[vp2idx(GL_COLOR_ARRAY)];
//    auto& vpa = g_glstate.fpe_state.vertexpointer_array;
//    if (vpa.buffer_based) {
//        attr.size = 4;
//        attr.usage = GL_COLOR_ARRAY;
//        attr.type = GL_FLOAT;
//        attr.normalized = GL_FALSE;
//        attr.stride = 0;
//        attr.pointer = 0;
//        attr.value = glm::vec4(red, green, blue, alpha);
//        attr.varying = false;
//    }
    mglColor<GLfloat, 4>({red, green, blue, alpha});
}

