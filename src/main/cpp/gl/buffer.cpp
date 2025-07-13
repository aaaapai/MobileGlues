//
// Created by BZLZHH on 2025/1/28.
//

#include "buffer.h"
#include "ankerl/unordered_dense.h"

template <typename K, typename V>
using unordered_map = ankerl::unordered_dense::map<K, V>;
//using unordered_map = std::unordered_map<K, V>;

#define DEBUG 0

GLint maxBufferId = 0;
GLint maxArrayId = 0;

unordered_map<GLuint, GLuint> g_gen_buffers;
unordered_map<GLuint, GLuint> g_gen_arrays;

unordered_map<GLenum, GLuint> g_bound_buffers;
GLuint bound_array = 0;
// fake array - fake ibo
unordered_map<GLuint, GLuint> g_element_array_buffer_per_vao;

unordered_map<GLuint, BufferMapping> g_active_mappings;

GLuint gen_buffer(GLuint realid) {
    maxBufferId++;
    g_gen_buffers[maxBufferId] = realid;
    return maxBufferId;
}

GLboolean has_buffer(GLuint key) {
    auto it = g_gen_buffers.find(key);
    return it != g_gen_buffers.end();
}

void modify_buffer(GLuint key, GLuint value) {
    g_gen_buffers[key] = value;
}

void remove_buffer(GLuint key) {
    if (g_gen_buffers.find(key) != g_gen_buffers.end())
        g_gen_buffers.erase(key);
}

GLuint find_real_buffer(GLuint key) {
    auto it = g_gen_buffers.find(key);
    if (it != g_gen_buffers.end())
        return it->second;
    else
        return 0;
}

GLuint get_ibo_by_vao(GLuint vao) {
    return g_element_array_buffer_per_vao[vao];
}

GLuint find_bound_array() {
    return bound_array;
}

void update_vao_ibo_binding(GLuint vao, GLuint ibo) {
    g_element_array_buffer_per_vao[vao] = ibo;
}

GLuint find_bound_buffer(GLenum key) {
    GLenum target = 0;
    switch (key) {
        case GL_ARRAY_BUFFER_BINDING:
            target = GL_ARRAY_BUFFER;
            break;
        case GL_QUERY_BUFFER_BINDING:
            target = GL_QUERY_BUFFER;
            break;
        case GL_ATOMIC_COUNTER_BUFFER_BINDING:
            target = GL_ATOMIC_COUNTER_BUFFER;
            break;
        case GL_COPY_READ_BUFFER_BINDING:
            target = GL_COPY_READ_BUFFER;
            break;
        case GL_COPY_WRITE_BUFFER_BINDING:
            target = GL_COPY_WRITE_BUFFER;
            break;
        case GL_DRAW_INDIRECT_BUFFER_BINDING:
            target = GL_DRAW_INDIRECT_BUFFER;
            break;
        case GL_DISPATCH_INDIRECT_BUFFER_BINDING:
            target = GL_DISPATCH_INDIRECT_BUFFER;
            break;
        case GL_ELEMENT_ARRAY_BUFFER_BINDING:
            target = GL_ELEMENT_ARRAY_BUFFER;
            break;
        case GL_PIXEL_PACK_BUFFER_BINDING:
            target = GL_PIXEL_PACK_BUFFER;
            break;
        case GL_PIXEL_UNPACK_BUFFER_BINDING:
            target = GL_PIXEL_UNPACK_BUFFER;
            break;
        case GL_SHADER_STORAGE_BUFFER_BINDING:
            target = GL_SHADER_STORAGE_BUFFER;
            break;
        case GL_TRANSFORM_FEEDBACK_BUFFER_BINDING:
            target = GL_TRANSFORM_FEEDBACK_BUFFER;
            break;
        case GL_UNIFORM_BUFFER_BINDING:
            target = GL_UNIFORM_BUFFER;
            break;
        default:
            target = 0;
            break;
    }
    if (target == GL_ELEMENT_ARRAY_BUFFER) {
        return get_ibo_by_vao(find_bound_array());
    }

    auto it = g_bound_buffers.find(target);
    if (it != g_bound_buffers.end())
        return it->second;
    else
        return 0;
}

GLuint gen_array() {
    maxArrayId++;
    g_gen_arrays[maxArrayId] = 0;
    return maxArrayId;
}

GLboolean has_array(GLuint key) {
    auto it = g_gen_arrays.find(key);
    return it != g_gen_arrays.end();
}

void modify_array(GLuint key, GLuint value) {
    g_gen_arrays[key] = value;
}

void remove_array(GLuint key) {
    if (g_gen_arrays.find(key) != g_gen_arrays.end())
        g_gen_arrays.erase(key);
    g_element_array_buffer_per_vao.erase(key);
}

GLuint find_real_array(GLuint key) {
    auto it = g_gen_arrays.find(key);
    if (it != g_gen_arrays.end())
        return it->second;
    else
        return 0;
}

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

void glGenBuffers(GLsizei n, GLuint *buffers) {
    LOG()
    LOG_D("glGenBuffers(%i, %p)", n, buffers)

    GLuint realid = 0;
    for (int i = 0; i < n; ++i) {
        buffers[i] = gen_buffer(realid);
    }
}

void glDeleteBuffers(GLsizei n, const GLuint *buffers) {
    LOG()
    LOG_D("glDeleteBuffers(%i, %p)", n, buffers)
    for (int i = 0; i < n; ++i) {
        if (find_real_buffer(buffers[i])) {
            GLuint real_buff = find_real_buffer(buffers[i]);
            GLES.glDeleteBuffers(1, &real_buff);
            CHECK_GL_ERROR
        }
        remove_buffer(buffers[i]);
    }
}

GLboolean glIsBuffer(GLuint buffer) {
    LOG()
    LOG_D("glIsBuffer, buffer = %d", buffer)
    return has_buffer(buffer);
}

void glBindBuffer(GLenum target, GLuint buffer) {
    LOG()
    LOG_D("glBindBuffer, target = %s, buffer = %d", glEnumToString(target), buffer)
    g_bound_buffers[target] = buffer;
    // save ibo binding to vao
    if (target == GL_ELEMENT_ARRAY_BUFFER) {
        update_vao_ibo_binding(find_bound_array(), buffer);
    }

    if (!has_buffer(buffer) || buffer == 0) {
        GLES.glBindBuffer(target, buffer);
        CHECK_GL_ERROR
        return;
    }
    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        GLES.glGenBuffers(1, &real_buffer);
        modify_buffer(buffer, real_buffer);
        CHECK_GL_ERROR
    }
    LOG_D("glBindBuffer: %d -> %d", buffer, real_buffer)
    GLES.glBindBuffer(target, real_buffer);
    CHECK_GL_ERROR
}

void glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) {
    LOG()
    LOG_D("glBindBufferRange, target = %s, index = %d, buffer = %d, offset = %p, size = %zi", glEnumToString(target), index, buffer, (void*) offset, size)
    g_bound_buffers[target] = buffer;
    // save ibo binding to vao
    if (target == GL_ELEMENT_ARRAY_BUFFER) {
        update_vao_ibo_binding(find_bound_array(), buffer);
    }

    if (!has_buffer(buffer) || buffer == 0) {
        GLES.glBindBufferRange(target, index, buffer, offset, size);
        CHECK_GL_ERROR
        return;
    }
    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        GLES.glGenBuffers(1, &real_buffer);
        modify_buffer(buffer, real_buffer);
        CHECK_GL_ERROR
    }
    GLES.glBindBufferRange(target, index, real_buffer, offset, size);
    CHECK_GL_ERROR
}

void glBindBufferBase(GLenum target, GLuint index, GLuint buffer) {
    LOG()
    LOG_D("glBindBufferBase, target = %s, index = %d, buffer = %d", glEnumToString(target), index, buffer)
    g_bound_buffers[target] = buffer;
    // save ibo binding to vao
    if (target == GL_ELEMENT_ARRAY_BUFFER) {
        update_vao_ibo_binding(find_bound_array(), buffer);
    }

    if (!has_buffer(buffer) || buffer == 0) {
        GLES.glBindBufferBase(target, index, buffer);
        CHECK_GL_ERROR
        return;
    }
    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        GLES.glGenBuffers(1, &real_buffer);
        modify_buffer(buffer, real_buffer);
        CHECK_GL_ERROR
    }
    GLES.glBindBufferBase(target, index, real_buffer);
    CHECK_GL_ERROR
}

void glBindVertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) {
    LOG()
    LOG_D("glBindVertexBuffer, bindingindex = %d, buffer = %d, offset = %p, stride = %i", bindingindex, buffer, offset, stride)
    // Todo: should record fake buffer binding here, when glGetVertexArrayIntegeri_v is called, should return fake buffer id
    if (!has_buffer(buffer) || buffer == 0) {
        GLES.glBindVertexBuffer(bindingindex, buffer, offset, stride);
        CHECK_GL_ERROR
        return;
    }
    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        GLES.glGenBuffers(1, &real_buffer);
        modify_buffer(buffer, real_buffer);
        CHECK_GL_ERROR
    }
    GLES.glBindVertexBuffer(bindingindex, real_buffer, offset, stride);
    CHECK_GL_ERROR
}

// Todo: any glGet* related to this function?
void glTexBuffer(GLenum target, GLenum internalformat, GLuint buffer) {
    LOG()
    LOG_D("glTexBuffer, target = %s, internalformat = %s, buffer = %d", glEnumToString(target), glEnumToString(internalformat), buffer)
    if (!has_buffer(buffer) || buffer == 0) {
        GLES.glTexBuffer(target, internalformat, buffer);
        CHECK_GL_ERROR
        return;
    }
    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        GLES.glGenBuffers(1, &real_buffer);
        modify_buffer(buffer, real_buffer);
        CHECK_GL_ERROR
    }
    GLES.glTexBuffer(target, internalformat, real_buffer);
    CHECK_GL_ERROR
}

void glTexBufferRange(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) {
    LOG()
    LOG_D("glTexBufferRange, target = %s, internalformat = %s, buffer = %d, offset = %p, size = %zi", glEnumToString(target), glEnumToString(internalformat), buffer, (void*) offset, size)
    if (!has_buffer(buffer) || buffer == 0) {
        GLES.glTexBufferRange(target, internalformat, buffer, offset, size);
        CHECK_GL_ERROR
        return;
    }
    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        GLES.glGenBuffers(1, &real_buffer);
        modify_buffer(buffer, real_buffer);
        CHECK_GL_ERROR
    }
    GLES.glTexBufferRange(target, internalformat, real_buffer, offset, size);
    CHECK_GL_ERROR
}

void glTexBufferRangeARB(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) {
    glTexBufferRange(target, internalformat, buffer, offset, size);
}

void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage) {
    LOG()
    LOG_D("glBufferData, target = %s, size = %d, data = 0x%x, usage = %s",
          glEnumToString(target), size, data, glEnumToString(usage))
    GLES.glBufferData(target, size, data, usage);
    CHECK_GL_ERROR
}

void* glMapBuffer(GLenum target, GLenum access) {
    LOG()
    LOG_D("glMapBuffer, target = %s, access = %s", glEnumToString(target), glEnumToString(access))
    if(g_gles_caps.GL_OES_mapbuffer) {
        return GLES.glMapBufferOES(target, access);
    }
    if (get_binding_query(target) == 0) {
        return nullptr;
    }
    GLint current_buffer;
    GLES.glGetIntegerv(get_binding_query(target), &current_buffer);
    if (current_buffer == 0) {
        return nullptr;
    }
    if (g_active_mappings[current_buffer].mapped_ptr != nullptr) {
        return nullptr;
    }
    GLint buffer_size;
    GLES.glGetBufferParameteriv(target, GL_BUFFER_SIZE, &buffer_size);
    if (buffer_size <= 0 || glGetError() != GL_NO_ERROR) {
        return nullptr;
    }
    GLbitfield flags = 0;
    switch (access) {
        case GL_READ_ONLY:  flags = GL_MAP_READ_BIT; break;
        case GL_WRITE_ONLY: flags = GL_MAP_WRITE_BIT /*| GL_MAP_INVALIDATE_BUFFER_BIT*/; break;
        case GL_READ_WRITE: flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT; break;
        default:
            return nullptr;
    }
    void* ptr = GLES.glMapBufferRange(target, 0, buffer_size, flags);
    if (!ptr) return nullptr;
    BufferMapping mapping;
    mapping.target = target;
    mapping.buffer_id = (GLuint)current_buffer;
    mapping.mapped_ptr = ptr;
#if GLOBAL_DEBUG || DEBUG
    if (target == GL_PIXEL_UNPACK_BUFFER) {
        mapping.client_ptr = malloc(buffer_size);
        memset(mapping.client_ptr, 0xFF, buffer_size);
    }
#endif
    mapping.size = buffer_size;
    mapping.flags = flags;
    mapping.is_dirty = (flags & GL_MAP_WRITE_BIT) ? GL_TRUE : GL_FALSE;
    g_active_mappings[current_buffer] = mapping;
    CHECK_GL_ERROR
#if GLOBAL_DEBUG || DEBUG
    if (target == GL_PIXEL_UNPACK_BUFFER)
        return mapping.client_ptr;
    else
        return ptr;
#else
    return ptr;
#endif
}

#if GLOBAL_DEBUG || DEBUG
#include <fstream>
#define BIN_FILE_PREFIX "/sdcard/MG/buf/"
#endif

GLboolean glUnmapBuffer(GLenum target) {
    LOG()
    LOG_D("%s(%s)", __func__, glEnumToString(target));
    if(g_gles_caps.GL_OES_mapbuffer)
        return GLES.glUnmapBuffer(target);

    GLint buffer;
    GLenum binding_query = get_binding_query(target);
    GLES.glGetIntegerv(binding_query, &buffer);

    if (buffer == 0)
        return GL_FALSE;

#if GLOBAL_DEBUG || DEBUG
//     Blit data from client side to OpenGL here
    if (target == GL_PIXEL_UNPACK_BUFFER) {
        auto &mapping = g_active_mappings[buffer];

        std::fstream fs(std::string(BIN_FILE_PREFIX) + "buf" + std::to_string(buffer) + ".bin", std::ios::out | std::ios::binary | std::ios::trunc);
        fs.write((const char*)mapping.client_ptr, mapping.size);
        fs.close();

//        memset(mapping.mapped_ptr, 0xFF, mapping.size);
        memcpy(mapping.mapped_ptr, mapping.client_ptr, mapping.size);
        free(mapping.client_ptr);
        mapping.client_ptr = nullptr;
    }
#endif

    GLboolean result = GLES.glUnmapBuffer(target);
    g_active_mappings.erase(buffer);
    CHECK_GL_ERROR
    return result;
}

void glBufferStorage(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags) {
    LOG()
    if(GLES.glBufferStorageEXT)
        GLES.glBufferStorageEXT(target,size,data,flags);
    CHECK_GL_ERROR
}

void glGenVertexArrays(GLsizei n, GLuint *arrays) {
    LOG()
    LOG_D("glGenVertexArrays(%i, %p)", n, arrays)
    for (int i = 0; i < n; ++i) {
        arrays[i] = gen_array();
    }
}

void glDeleteVertexArrays(GLsizei n, const GLuint *arrays) {
    LOG()
    LOG_D("glDeleteVertexArrays(%i, %p)", n, arrays)
    for (int i = 0; i < n; ++i) {
        if (find_real_array(arrays[i])) {
            GLuint real_array = find_real_array(arrays[i]);
            GLES.glDeleteVertexArrays(1, &real_array);
            CHECK_GL_ERROR
        }
        remove_array(arrays[i]);
    }
}

GLboolean glIsVertexArray(GLuint array) {
    LOG()
    LOG_D("glIsVertexArray(%d)", array)
    return has_array(array);
}

void glBindVertexArray(GLuint array) {
    LOG()
    LOG_D("glBindVertexArray(%d)", array)
    bound_array = array;

    // update bound ibo
    g_bound_buffers[GL_ELEMENT_ARRAY_BUFFER] = get_ibo_by_vao(array);

    if (!has_array(array) || array == 0) {
        LOG_D("Does not have va=%d found!", array)
        GLES.glBindVertexArray(array);
        CHECK_GL_ERROR
        return;
    }

    GLuint real_array = find_real_array(array);
    if (!real_array) {
        LOG_D("va=%d not initialized, initializing...", array)
        GLES.glGenVertexArrays(1, &real_array);
        modify_array(array, real_array);
        CHECK_GL_ERROR
    }
    LOG_D("glBindVertexArray: %d -> %d", array, real_array)
    GLES.glBindVertexArray(real_array);
    CHECK_GL_ERROR
}




void glClearBufferData(GLenum target, GLenum internalformat,
                      GLenum format, GLenum type, const void *data) {
    LOG()
    LOG_D("glClearBufferData(target=%s, internalformat=%s, format=%s, type=%s, data=%p)",
          glEnumToString(target), glEnumToString(internalformat),
          glEnumToString(format), glEnumToString(type), data);

    // Find the currently bound buffer for this target
    GLuint buffer = find_bound_buffer(get_binding_query(target));
    if (!buffer) {
        LOG_E("No buffer bound to target %s", glEnumToString(target));
        return;
    }

    // Get the real buffer ID from our mapping
    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        LOG_E("Buffer %d not found in mapping", buffer);
        return;
    }

    // Get buffer size
    GLint size;
    GLES.glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
    if (size <= 0) {
        LOG_E("Invalid buffer size: %d", size);
        return;
    }

    // Map the buffer with write access
    void *ptr = GLES.glMapBufferRange(target, 0, size, 
                                     GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    if (!ptr) {
        LOG_E("Failed to map buffer");
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
            LOG_E("Unsupported type: %s", glEnumToString(type));
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
    LOG_D("glClearNamedBufferData(buffer=%d, internalformat=%s, format=%s, type=%s, data=%p)",
          buffer, glEnumToString(internalformat), 
          glEnumToString(format), glEnumToString(type), data);

    if (!has_buffer(buffer)) {
        LOG_E("Buffer %d does not exist", buffer);
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
    GLES.glGetIntegerv(get_binding_query(target), &prev_buffer);
    
    // Bind our buffer and delegate to glClearBufferData
    GLuint real_buffer = find_real_buffer(buffer);
    GLES.glBindBuffer(target, real_buffer);
    glClearBufferData(target, internalformat, format, type, data);
    
    // Restore previous binding
    GLES.glBindBuffer(target, prev_buffer);
    CHECK_GL_ERROR
}  //DeepSeek

void APIENTRY glClearBufferSubData(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data) {
    LOG()
    LOG_D("glClearBufferSubData(target=%s, internalformat=%s, offset=%p, size=%zi, format=%s, type=%s, data=%p)",
          glEnumToString(target), glEnumToString(internalformat), (void*)offset, size, glEnumToString(format), glEnumToString(type), data)

    // Get the currently bound buffer for this target
    GLint current_buffer = 0;
    GLES.glGetIntegerv(get_binding_query(target), &current_buffer);
    
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
    GLES.glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING, &prev_binding);
    
    // Bind our buffer to COPY_WRITE_BUFFER target
    GLES.glBindBuffer(GL_COPY_WRITE_BUFFER, real_buffer);
    
    // Use glBufferSubData to clear the buffer range
    GLES.glBufferSubData(GL_COPY_WRITE_BUFFER, offset, size, data);
    
    // Restore previous binding
    GLES.glBindBuffer(GL_COPY_WRITE_BUFFER, prev_binding);
    
    CHECK_GL_ERROR
} //DeepSeek

void glClearNamedBufferSubData(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data) {
    LOG()
    LOG_D("glClearNamedBufferSubData(buffer=%u, internalformat=%s, offset=%p, size=%zi, format=%s, type=%s, data=%p)",
          buffer, glEnumToString(internalformat), (void*)offset, size, glEnumToString(format), glEnumToString(type), data)

    // First find the real buffer ID from our mapping
    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        // If not found in our mapping, assume it's already a real buffer ID
        real_buffer = buffer;
    }

    // Save current buffer binding
    GLint prev_binding = 0;
    GLES.glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING, &prev_binding);
    
    // Bind our buffer to COPY_WRITE_BUFFER target (since we can't bind by name directly in GLES)
    GLES.glBindBuffer(GL_COPY_WRITE_BUFFER, real_buffer);
    
    // Use glBufferSubData to clear the buffer range (GLES doesn't have glClearBufferSubData)
    // Note: This isn't exactly the same as clear, but closest we can get in GLES
    GLES.glBufferSubData(GL_COPY_WRITE_BUFFER, offset, size, data);
    
    // Restore previous binding
    GLES.glBindBuffer(GL_COPY_WRITE_BUFFER, prev_binding);
    
    CHECK_GL_ERROR
} //DeepSeek

extern "C" {
GLAPI GLAPIENTRY void *glMapBufferARB(GLenum target, GLenum access) __attribute__((alias("glMapBuffer")));
GLAPI GLAPIENTRY void *glBufferDataARB(GLenum target, GLenum access) __attribute__((alias("glBufferData")));
GLAPI GLAPIENTRY GLboolean glUnmapBufferARB(GLenum target) __attribute__((alias("glUnmapBuffer")));
GLAPI GLAPIENTRY void glBufferStorageARB(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags) __attribute__((alias("glBufferStorage")));
GLAPI GLAPIENTRY void glBindBufferARB(GLenum target, GLuint buffer) __attribute__((alias("glBindBuffer")));
GLAPI GLAPIENTRY void glBindBufferRangeARB(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) __attribute__((alias("glBindBufferRange")));
GLAPI GLAPIENTRY void glBindBufferBaseARB(GLenum target, GLuint index, GLuint buffer) __attribute__((alias("glBindBufferBase")));
GLAPI GLAPIENTRY void glDeleteBuffersARB(GLsizei n, const GLuint *buffers) __attribute__((alias("glDeleteBuffers")));
GLAPI GLAPIENTRY void glGenBuffersARB(GLsizei n, GLuint *buffers) __attribute__((alias("glGenBuffers")));
GLAPI GLAPIENTRY GLboolean glIsBufferARB(GLuint buffer) __attribute__((alias("glIsBuffer")));
}
