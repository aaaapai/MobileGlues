//
// Created by Swung 0x48 on 2025-03-26.
//

#include "buffer.h"

#include "../buffer.h"
#include "ankerl/unordered_dense.h"

#define DEBUG 0

template <typename K, typename V>
using unordered_map = ankerl::unordered_dense::map<K, V>;

extern GLint maxBufferId;
extern GLint maxArrayId;

extern unordered_map<GLuint, GLuint> g_gen_buffers;
extern unordered_map<GLuint, GLuint> g_gen_arrays;

extern unordered_map<GLenum, GLuint> g_bound_buffers;
extern GLuint bound_array;
// fake array - fake ibo
extern unordered_map<GLuint, GLuint> g_element_array_buffer_per_vao;

extern unordered_map<GLuint, BufferMapping> g_active_mappings;

static GLenum get_binding_query(GLenum target) {
    switch(target) {
        case GL_ARRAY_BUFFER:          return GL_ARRAY_BUFFER_BINDING;
        case GL_ELEMENT_ARRAY_BUFFER:  return GL_ELEMENT_ARRAY_BUFFER_BINDING;
        case GL_PIXEL_PACK_BUFFER:     return GL_PIXEL_PACK_BUFFER_BINDING;
        case GL_PIXEL_UNPACK_BUFFER:   return GL_PIXEL_UNPACK_BUFFER_BINDING;
        case GL_COPY_WRITE_BUFFER:     return GL_COPY_WRITE_BUFFER_BINDING;
        case GL_COPY_READ_BUFFER:      return GL_COPY_READ_BUFFER_BINDING;
        case GL_UNIFORM_BUFFER:        return GL_UNIFORM_BUFFER_BINDING;
        case GL_SHADER_STORAGE_BUFFER: return GL_SHADER_STORAGE_BUFFER_BINDING;
        case GL_TRANSFORM_FEEDBACK_BUFFER: return GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
        case GL_ATOMIC_COUNTER_BUFFER: return GL_ATOMIC_COUNTER_BUFFER_BINDING;
        case GL_DRAW_INDIRECT_BUFFER:  return GL_DRAW_INDIRECT_BUFFER_BINDING;
        case GL_DISPATCH_INDIRECT_BUFFER: return GL_DISPATCH_INDIRECT_BUFFER_BINDING;
        default:                       return 0;
    }
}

#define SAVE_BUFFER_CTX(target) \
    GLint prevbuf = 0; \
    glGetIntegerv(target##_BINDING, &prevbuf); \
    CHECK_GL_ERROR_NO_INIT \
    glBindBuffer(target, buffer); \
    CHECK_GL_ERROR_NO_INIT

#define RESTORE_BUFFER_CTX(target) \
    glBindBuffer(GL_COPY_WRITE_BUFFER, prevbuf); \
    CHECK_GL_ERROR_NO_INIT

void glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data) {
    LOG()
    LOG_D("glNamedBufferSubData, buffer = %d, offset = %d, size = %d, data = 0x%x",
          buffer, offset, size, data)

    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glBufferSubData(GL_COPY_WRITE_BUFFER, offset, size, data);

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glCreateBuffers(GLsizei n, GLuint* buffers) {
    LOG()
    LOG_D("glCreateBuffers, n = %d", n)

    INIT_CHECK_GL_ERROR

    // do actual gen to ES driver
    GLES.glGenBuffers(n, buffers);
    CHECK_GL_ERROR_NO_INIT
    // save to renaming table
    for (int i = 0; i < n; ++i) {
        buffers[i] = gen_buffer(buffers[i]);
    }
}

void glFlushMappedNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length) {
    LOG()
    LOG_D("glFlushMappedNamedBufferRange, buffer = %d, offset = %d, length = %d", buffer, offset, length)
    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glFlushMappedBufferRange(GL_COPY_WRITE_BUFFER, offset, length);
    CHECK_GL_ERROR_NO_INIT

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glGetNamedBufferParameteriv(GLuint buffer, GLenum pname, GLint* params) {
    LOG()
    LOG_D("glGetNamedBufferParameteriv, buffer = %d, pname = %s", buffer, glEnumToString(pname))
    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glGetBufferParameteriv(GL_COPY_WRITE_BUFFER, pname, params);
    CHECK_GL_ERROR_NO_INIT

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glGetNamedBufferParameteri64v(GLuint buffer, GLenum pname, GLint64* params) {
    LOG()
    LOG_D("glGetNamedBufferParameteri64v, buffer = %d, pname = %s", buffer, glEnumToString(pname))
    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glGetBufferParameteri64v(GL_COPY_WRITE_BUFFER, pname, params);
    CHECK_GL_ERROR_NO_INIT

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glGetNamedBufferPointerv(GLuint buffer, GLenum pname, void* *params) {
    LOG()
    LOG_D("glGetNamedBufferPointerv, buffer = %d, pname = %s", buffer, glEnumToString(pname))
    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glGetBufferPointerv(GL_COPY_WRITE_BUFFER, pname, params);
    CHECK_GL_ERROR_NO_INIT

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glGetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data) {
    LOG()
    LOG_D("glGetNamedBufferSubData, buffer = %d, offset = %d, size = %d, data = 0x%x",
          buffer, offset, size, data)
    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_READ_BUFFER)

    // TODO...

    RESTORE_BUFFER_CTX(GL_COPY_READ_BUFFER)
}

void* glMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access) {
    LOG()
    LOG_D("glMapNamedBufferRange, buffer = %d, offset = %d, length = %d, access = 0x%x",
          buffer, offset, length, access)
    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_READ_BUFFER)

    void* ptr = GLES.glMapBufferRange(GL_COPY_READ_BUFFER, offset, length, access);

    RESTORE_BUFFER_CTX(GL_COPY_READ_BUFFER)

    return ptr;
}

void glClearBufferData(GLenum target, GLenum internalformat,
                      GLenum format, GLenum type, const void *data) {
    LOG()
    LOG_W("glClearBufferData(target=%s, internalformat=%s, format=%s, type=%s, data=%p)",
          glEnumToString(target), glEnumToString(internalformat),
          glEnumToString(format), glEnumToString(type), data);

    // Find the currently bound buffer for this target
    GLuint buffer = find_bound_buffer(get_binding_query(target));
    if (!buffer) {
        LOG_E("ERROR: No buffer bound to target %s", glEnumToString(target));
        return;
    }

    // Get the real buffer ID from our mapping
    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        LOG_E("ERROR: Buffer %d not found in mapping", buffer);
        return;
    }

    // Get buffer size
    GLint size;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
    if (size <= 0) {
        LOG_E("ERROR: Invalid buffer size: %d", size);
        return;
    }

    // Map the buffer with write access
    void *ptr = GLES.glMapBufferRange(target, 0, size, 
                                     GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    if (!ptr) {
        LOG_E("ERROR: Failed to map buffer");
        return;
    }

    // Determine element size based on type
    size_t elem_size = 0;
    switch (type) {
        case GL_UNSIGNED_BYTE:
        case GL_BYTE:
            elem_size = 1;
            break;
        case GL_UNSIGNED_SHORT:
        case GL_SHORT:
            elem_size = 2;
            break;
        case GL_UNSIGNED_INT:
        case GL_INT:
        case GL_FLOAT:
            elem_size = 4;
            break;
        default:
            LOG_E("ERROR: Unsupported type: %s", glEnumToString(type));
            GLES.glUnmapBuffer(target);
            return;
    }

    // Fill the buffer with the pattern
    if (data) {
        for (size_t i = 0; i < size; i += elem_size) {
            memcpy((char*)ptr + i, data, elem_size);
        }
    } else {
        // If data is NULL, use 0 as the pattern
        memset(ptr, 0, size);
    }

    GLES.glUnmapBuffer(target);
    CHECK_GL_ERROR
} //DeepSeek

void glClearNamedBufferData(GLuint buffer, GLenum internalformat,
                          GLenum format, GLenum type, const void *data) {
    LOG()
    LOG_W("glClearNamedBufferData(buffer=%d, internalformat=%s, format=%s, type=%s, data=%p)",
          buffer, glEnumToString(internalformat), 
          glEnumToString(format), glEnumToString(type), data);

    if (!has_buffer(buffer)) {
        LOG_E("ERROR: Buffer %d does not exist", buffer);
        return;
    }

    // We need to determine the target type of the buffer
    // This is tricky since OpenGL doesn't provide a direct query for it
    // We'll try to find which binding point this buffer is bound to
    
    GLenum target = 0;
    for (const auto& pair : g_bound_buffers) {
        if (pair.second == buffer) {
            target = pair.first;
            break;
        }
    }
    
    // If not found in current bindings, default to ARRAY_BUFFER
    if (target == 0) {
        target = GL_ARRAY_BUFFER;
        LOG_W("Could not determine buffer target for %d, defaulting to GL_ARRAY_BUFFER", buffer);
    }

    // Save current binding
    GLint prev_buffer;
    glGetIntegerv(get_binding_query(target), &prev_buffer);
    
    // Bind our buffer and delegate to glClearBufferData
    GLuint real_buffer = find_real_buffer(buffer);
    glBindBuffer(target, real_buffer);
    glClearBufferData(target, internalformat, format, type, data);
    
    // Restore previous binding
    glBindBuffer(target, prev_buffer);
    CHECK_GL_ERROR
}  //DeepSeek

void glClearBufferSubData(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data) {
    LOG()
    LOG_W("glClearBufferSubData(target=%s, internalformat=%s, offset=%p, size=%zi, format=%s, type=%s, data=%p)",
          glEnumToString(target), glEnumToString(internalformat), (void*)offset, size, glEnumToString(format), glEnumToString(type), data)

    // Get the currently bound buffer for this target
    GLint current_buffer = 0;
    glGetIntegerv(get_binding_query(target), &current_buffer);
    
    if (current_buffer == 0) {
        // No buffer bound to this target
        return;
    }

    // Find the real buffer ID from our mapping (if it exists)
    GLuint real_buffer = find_real_buffer(current_buffer);
    if (!real_buffer) {
        // If not found in our mapping, assume it's already a real buffer ID
        real_buffer = current_buffer;
    }

    // Save current buffer binding
    GLint prev_binding = 0;
    glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING, &prev_binding);
    
    // Bind our buffer to COPY_WRITE_BUFFER target
    glBindBuffer(GL_COPY_WRITE_BUFFER, real_buffer);
    
    // Use glBufferSubData to clear the buffer range
    glBufferSubData(GL_COPY_WRITE_BUFFER, offset, size, data);
    
    // Restore previous binding
    glBindBuffer(GL_COPY_WRITE_BUFFER, prev_binding);
    
    CHECK_GL_ERROR
} //DeepSeek

void glClearNamedBufferSubData(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data) {
    LOG()
    LOG_W("glClearNamedBufferSubData(buffer=%u, internalformat=%s, offset=%p, size=%zi, format=%s, type=%s, data=%p)",
          buffer, glEnumToString(internalformat), (void*)offset, size, glEnumToString(format), glEnumToString(type), data)

    // First find the real buffer ID from our mapping
    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        // If not found in our mapping, assume it's already a real buffer ID
        real_buffer = buffer;
    }

    // Save current buffer binding
    GLint prev_binding = 0;
    glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING, &prev_binding);
    
    // Bind our buffer to COPY_WRITE_BUFFER target (since we can't bind by name directly in GLES)
    glBindBuffer(GL_COPY_WRITE_BUFFER, real_buffer);
    
    // Use glBufferSubData to clear the buffer range (GLES doesn't have glClearBufferSubData)
    // Note: This isn't exactly the same as clear, but closest we can get in GLES
    glBufferSubData(GL_COPY_WRITE_BUFFER, offset, size, data);
    
    // Restore previous binding
    glBindBuffer(GL_COPY_WRITE_BUFFER, prev_binding);
    
    CHECK_GL_ERROR
} //DeepSeek

