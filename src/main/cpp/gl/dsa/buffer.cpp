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
    LOG_D("glGetNamedBufferSubData, buffer: %u, offset: %lld, size: %lld, data: %p", 
          buffer, (long long)offset, (long long)size, data)

    INIT_CHECK_GL_ERROR

    // 查找当前绑定的缓冲区
    GLenum target = 0;
    for (const auto& pair : g_bound_buffers) {
        if (pair.second == buffer) {
            target = pair.first;
            break;
        }
    }

    // 保存当前绑定状态
    GLint prevbuf = 0;
    GLenum bindingQuery = get_binding_query(target);
    if (bindingQuery != 0) {
        glGetIntegerv(bindingQuery, &prevbuf);
        CHECK_GL_ERROR_NO_INIT
    }

    // 绑定目标缓冲区
    glBindBuffer(target, buffer);
    CHECK_GL_ERROR_NO_INIT

    // 使用glGetBufferSubData的替代方法，因为GLES.glGetBufferSubData不可用
    // 替代方案: 使用映射缓冲区的方式读取数据
    void* mappedData = GLES.glMapBufferRange(target, offset, size, GL_MAP_READ_BIT);
    if (mappedData) {
        memcpy(data, mappedData, size);
        GLES.glUnmapBuffer(target);
        CHECK_GL_ERROR_NO_INIT
    } else {
        LOG_E("ERROR: Failed to map buffer %u for reading", buffer)
    }

    // 恢复之前的绑定状态
    if (bindingQuery != 0) {
        glBindBuffer(target, prevbuf);
        CHECK_GL_ERROR_NO_INIT
    }
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
    LOG_D("glClearBufferData(target=%s, internalformat=%s, format=%s, type=%s, data=%p)",
          glEnumToString(target), glEnumToString(internalformat),
          glEnumToString(format), glEnumToString(type), data)

    // Find the currently bound buffer for this target
    GLuint buffer = find_bound_buffer(get_binding_query(target));
    if (!buffer) {
        LOG_E("ERROR: No buffer bound to target %s", glEnumToString(target))
        return;
    }

    // Get the real buffer ID from our mapping
    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        LOG_E("ERROR: Buffer %d not found in mapping", buffer)
        return;
    }

    // Get buffer size
    GLint size;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
    if (size <= 0) {
        LOG_E("ERROR: Invalid buffer size: %d", size)
        return;
    }

    // Map the buffer with write access
    void *ptr = GLES.glMapBufferRange(target, 0, size, 
                                     GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    if (!ptr) {
        LOG_E("ERROR: Failed to map buffer")
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
            LOG_E("ERROR: Unsupported type: %s", glEnumToString(type))
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
    LOG_D("glClearNamedBufferData(buffer=%u, internalformat=%s, format=%s, type=%s, data=%p)",
          buffer, glEnumToString(internalformat), 
          glEnumToString(format), glEnumToString(type), data);

    // 直接使用 glClearBufferData 的 GLES3 等效实现
    GLint prev_binding = 0;
    glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING, &prev_binding);
    
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffer);
    
    // GLES3 使用 glBufferData 来清除缓冲区
    GLint buffer_size = 0;
    GLES.glGetBufferParameteriv(GL_COPY_WRITE_BUFFER, GL_BUFFER_SIZE, &buffer_size);
    
    if (buffer_size > 0) {
        if (data) {
            // 计算清除数据的大小
            size_t elem_size = 0;
            switch(type) {
                case GL_BYTE:
                case GL_UNSIGNED_BYTE: elem_size = 1; break;
                case GL_SHORT:
                case GL_UNSIGNED_SHORT:
                case GL_HALF_FLOAT:    elem_size = 2; break;
                case GL_INT:
                case GL_UNSIGNED_INT:
                case GL_FLOAT:
                case GL_FIXED:         elem_size = 4; break;
            }
            
            if (elem_size > 0) {
                // 创建填充数据缓冲区
                size_t data_size = buffer_size / elem_size;
                void *fill_data = malloc(buffer_size);
                if (fill_data) {
                    for (size_t i = 0; i < data_size; i++) {
                        memcpy((char*)fill_data + i * elem_size, data, elem_size);
                    }
                    glBufferData(GL_COPY_WRITE_BUFFER, buffer_size, fill_data, GL_STATIC_DRAW);
                    free(fill_data);
                }
            }
        } else {
            // 如果没有提供数据，则用0填充
            glBufferData(GL_COPY_WRITE_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);
        }
    }
    
    glBindBuffer(GL_COPY_WRITE_BUFFER, prev_binding);
}

void glClearBufferSubData(GLenum target, GLenum internalformat, GLintptr offset,
                         GLsizeiptr size, GLenum format, GLenum type, const void* data) {
    LOG_D("glClearBufferSubData, target: %u, offset: %lld, size: %lld, data: %p",
          target, (long long)offset, (long long)size, data);

    INIT_CHECK_GL_ERROR

    GLenum binding = get_binding_query(target);
    if (!binding || g_active_mappings.count(g_bound_buffers[target])) return;

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER);
    
    // Create temp buffer with desired data
    GLuint tempBuf;
    GLES.glGenBuffers(1, &tempBuf);
    GLES.glBufferData(GL_COPY_WRITE_BUFFER, size, data ? data : calloc(1, size), GL_STATIC_DRAW);
    
    // Copy to target
    GLES.glCopyBufferSubData(GL_COPY_WRITE_BUFFER, target, 0, offset, size);
    
    // Cleanup
    if (!data) free(const_cast<void*>(GLES.glMapBufferRange(GL_COPY_WRITE_BUFFER, 0, size, GL_MAP_READ_BIT)));
    GLES.glDeleteBuffers(1, &tempBuf);
    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER);
} //dk

void glClearNamedBufferSubData(GLuint buffer, GLenum internalformat, 
                             GLintptr offset, GLsizeiptr size,
                             GLenum format, GLenum type, const void *data) {
    LOG()
    LOG_D("glClearNamedBufferSubData(buffer=%u, internalformat=%s, offset=%ld, size=%zd, format=%s, type=%s, data=%p)",
          buffer, glEnumToString(internalformat), 
          offset, size, glEnumToString(format), glEnumToString(type), data)

    INIT_CHECK_GL_ERROR

    // 1. 验证缓冲区存在性
    if (!has_buffer(buffer)) {
        LOG_E("Buffer %u does not exist", buffer)
        return;
    }

    // 2. 获取真实缓冲区ID
    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        real_buffer = buffer;
        LOG_D("Using buffer %u directly (not in mapping table)", real_buffer)
    }

    // 3. 保存当前绑定状态
    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    // 4. 执行清除操作
    if (data) {
        GLES.glBufferSubData(GL_COPY_WRITE_BUFFER, offset, size, data);
    } else {
        // 如果data为NULL，使用0填充
        std::vector<char> zero_data(size, 0);
        GLES.glBufferSubData(GL_COPY_WRITE_BUFFER, offset, size, zero_data.data());
    }

    // 5. 恢复状态
    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    CHECK_GL_ERROR_NO_INIT
} //DeepSeek*2
