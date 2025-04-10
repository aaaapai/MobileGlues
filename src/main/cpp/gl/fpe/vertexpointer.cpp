//
// Created by Swung 0x48 on 2025/2/10.
//

#include "vertexpointer.h"
#include "list.h"

#define DEBUG 0

void glVertexPointer(GLint size,
                     GLenum type,
                     GLsizei stride,
                     const void * pointer) {
    LOG_D("glVertexPointer, size = %d, type = %s, stride = %d, pointer = 0x%x", size, glEnumToString(type), stride, pointer)
    auto& attr = g_glstate.fpe_state.vertexpointer_array.attributes[vp2idx(GL_VERTEX_ARRAY)];
    attr.size = size;
    attr.usage = GL_VERTEX_ARRAY;
    attr.type = type;
    attr.normalized = GL_FALSE;
    attr.stride = stride;
    attr.pointer = pointer;
    attr.varies = true;
    g_glstate.fpe_state.vertexpointer_array.dirty = true;
    g_glstate.fpe_state.vertexpointer_array.buffer_based = true;
}

void glNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer) {
    LOG_D("glNormalPointer, type = %s, stride = %d, pointer = 0x%x", glEnumToString(type), stride, pointer)
    g_glstate.fpe_state.vertexpointer_array.attributes[vp2idx(GL_NORMAL_ARRAY)] = {
            .size = 3,
            .usage = GL_NORMAL_ARRAY,
            .type = type,
            .normalized = GL_FALSE,
            .stride = stride,
            .pointer = pointer,
            .varies = true
    };
    g_glstate.fpe_state.vertexpointer_array.dirty = true;
}

void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {
    LOG_D("glColorPointer, size = %d, type = %s, stride = %d, pointer = 0x%x", size, glEnumToString(type), stride, pointer)
    g_glstate.fpe_state.vertexpointer_array.attributes[vp2idx(GL_COLOR_ARRAY)] = {
            .size = size,
            .usage = GL_COLOR_ARRAY,
            .type = type,
            .normalized = GL_TRUE,
            .stride = stride,
            .pointer = pointer,
            .varies = true
    };
    g_glstate.fpe_state.vertexpointer_array.dirty = true;
}

void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {
    LOG_D("glTexCoordPointer, size = %d, type = %s, stride = %d, pointer = 0x%x", size, glEnumToString(type), stride, pointer)
    g_glstate.fpe_state.vertexpointer_array.attributes[vp2idx(GL_TEXTURE_COORD_ARRAY)] = {
            .size = size,
            .usage = GL_TEXTURE_COORD_ARRAY + (g_glstate.fpe_state.client_active_texture - GL_TEXTURE0),
            .type = type,
            .normalized = GL_FALSE,
            .stride = stride,
            .pointer = pointer,
            .varies = true
    };
    g_glstate.fpe_state.vertexpointer_array.dirty = true;
}

void glIndexPointer(GLenum type, GLsizei stride, const GLvoid *pointer ) {
    LOG_D("glIndexPointer, size = %d, type = %s, stride = %d, pointer = 0x%x", glEnumToString(type), stride, pointer)
    g_glstate.fpe_state.vertexpointer_array.attributes[vp2idx(GL_INDEX_ARRAY)] = {
            .size = 1,
            .usage = GL_INDEX_ARRAY,
            .type = type,
            .normalized = GL_FALSE,
            .stride = stride,
            .pointer = pointer,
            .varies = true
    };
    g_glstate.fpe_state.vertexpointer_array.dirty = true;
}

void glEnableClientState(GLenum cap) {
    LOG_D("glEnableClientState, cap = %s", glEnumToString(cap))

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glEnableClientState>({}, cap);
        if (DisplayListManager::shouldFinish())
            return;
    }

    auto mask = vp_mask(cap);
    g_glstate.fpe_state.vertexpointer_array.enabled_pointers |= mask;
    LOG_D("Enabled Ptr: 0x%x", g_glstate.fpe_state.vertexpointer_array.enabled_pointers)
    g_glstate.fpe_state.vertexpointer_array.dirty = true;
}

void glDisableClientState(GLenum cap) {
    LOG_D("glDisableClientState, cap = %s", glEnumToString(cap))
    auto mask = vp_mask(cap);

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glDisableClientState>({}, cap);
        if (DisplayListManager::shouldFinish())
            return;
    }

    g_glstate.fpe_state.vertexpointer_array.enabled_pointers &= (~mask);
    LOG_D("Enabled Ptr: 0x%x", g_glstate.fpe_state.vertexpointer_array.enabled_pointers)
    g_glstate.fpe_state.vertexpointer_array.dirty = true;
}
