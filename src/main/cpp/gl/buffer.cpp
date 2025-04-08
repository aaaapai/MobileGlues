//
// Created by BZLZHH on 2025/1/28.
//

#include "buffer.h"
#include <unordered_map>
#include <vector>
#include <cstdlib>

#define DEBUG 0

GLint maxBufferId = 0;
GLint maxArrayId = 0;

// Map: MG buffer ID -> GLES real buffer ID
unordered_map<GLuint, GLuint> g_gen_buffers;
// Map: MG array ID -> GLES real array ID
unordered_map<GLuint, GLuint> g_gen_arrays;

// Map: MG buffer ID -> GL4ES buffer ID
unordered_map<GLuint, GLuint> g_buffer_map_mg_gl4es;
// Map: MG array ID -> GL4ES array ID
unordered_map<GLuint, GLuint> g_va_map_mg_gl4es;

// Map: GLenum target -> currently bound MG buffer ID
unordered_map<GLenum, GLuint> g_bound_buffers;
// Currently bound MG array ID
GLuint bound_array = 0;

// Map: GLES buffer ID -> Active mapping info (for glMapBuffer/glUnmapBuffer emulation)
unordered_map<GLuint, BufferMapping> g_active_mappings;

GLuint gen_buffer() {
    LOG_D("gen_buffer: Generating a new buffer. Previous maxBufferId was %d", maxBufferId)
    maxBufferId++;
    g_gen_buffers[maxBufferId] = 0;
    return maxBufferId;
}

GLboolean has_buffer(GLuint key) {
    GLboolean result = g_gen_buffers.count(key);
    LOG_D("has_buffer: Checking if buffer %u exists. Result: %s", key, result ? "true" : "false")
    return result;
}

void modify_buffer(GLuint key, GLuint value) {
    if (g_gen_buffers.count(key)) {
        g_gen_buffers[key] = value;
        LOG_D("modify_buffer: Modified buffer %u. New GLES ID: %u", key, value)
    }
}

void remove_buffer(GLuint key) {
    LOG_D("remove_buffer: Removing buffer %u", key)
    g_gen_buffers.erase(key);
    g_buffer_map_mg_gl4es.erase(key);
}

GLuint find_real_buffer(GLuint key) {
    LOG_D("find_real_buffer: Finding GLES ID for buffer %u", key)
    auto it = g_gen_buffers.find(key);
    GLuint result = (it != g_gen_buffers.end()) ? it->second : 0;
    LOG_D("find_real_buffer: Result for buffer %u: GLES ID %u", key, result)
    return result;
}

GLuint find_gl4es_buffer(GLuint mg_buffer_id) {
    LOG_D("find_gl4es_buffer: Finding GL4ES ID for MG buffer %u", mg_buffer_id)
    if (mg_buffer_id == 0) return 0;
    auto it = g_buffer_map_mg_gl4es.find(mg_buffer_id);
    GLuint result = (it != g_buffer_map_mg_gl4es.end()) ? it->second : 0;
    LOG_D("find_gl4es_buffer: Result for buffer %u: GL4ES ID %u", mg_buffer_id, result)
    return result;
}

GLuint find_bound_buffer(GLenum key) {
    GLenum target = 0;
    switch (key) {
        case GL_ARRAY_BUFFER_BINDING:               target = GL_ARRAY_BUFFER; break;
        case GL_ATOMIC_COUNTER_BUFFER_BINDING:      target = GL_ATOMIC_COUNTER_BUFFER; break;
        case GL_COPY_READ_BUFFER_BINDING:           target = GL_COPY_READ_BUFFER; break;
        case GL_COPY_WRITE_BUFFER_BINDING:          target = GL_COPY_WRITE_BUFFER; break;
        case GL_DRAW_INDIRECT_BUFFER_BINDING:       target = GL_DRAW_INDIRECT_BUFFER; break;
        case GL_DISPATCH_INDIRECT_BUFFER_BINDING:   target = GL_DISPATCH_INDIRECT_BUFFER; break;
        case GL_ELEMENT_ARRAY_BUFFER_BINDING:       target = GL_ELEMENT_ARRAY_BUFFER; break;
        case GL_PIXEL_PACK_BUFFER_BINDING:          target = GL_PIXEL_PACK_BUFFER; break;
        case GL_PIXEL_UNPACK_BUFFER_BINDING:        target = GL_PIXEL_UNPACK_BUFFER; break;
        case GL_SHADER_STORAGE_BUFFER_BINDING:      target = GL_SHADER_STORAGE_BUFFER; break;
        case GL_TRANSFORM_FEEDBACK_BUFFER_BINDING:  target = GL_TRANSFORM_FEEDBACK_BUFFER; break;
        case GL_UNIFORM_BUFFER_BINDING:             target = GL_UNIFORM_BUFFER; break;
        default:                                    target = 0; break;
    }
    auto it = g_bound_buffers.find(target);
    return (it != g_bound_buffers.end()) ? it->second : 0;
}

GLuint gen_array() {
    LOG_D("gen_array: Generating a new array. Previous maxArrayId was %d", maxArrayId);
    maxArrayId++;
    g_gen_arrays[maxArrayId] = 0;
    return maxArrayId;
}

GLboolean has_array(GLuint key) {
    GLboolean result = g_gen_arrays.count(key);
    LOG_D("has_array: Checking if array %u exists. Result: %s", key, result ? "true" : "false");
    return result;
}

void modify_array(GLuint key, GLuint value) {
    if (g_gen_arrays.count(key)) {
        g_gen_arrays[key] = value;
        LOG_D("modify_array: Modified array %u. New GLES ID: %u", key, value);
    }
}

void remove_array(GLuint key) {
    LOG_D("remove_array: Removing array %u", key);
    g_gen_arrays.erase(key);
    g_va_map_mg_gl4es.erase(key);
}

GLuint find_real_array(GLuint key) {
    LOG_D("find_real_array: Finding GLES ID for array %u", key);
    auto it = g_gen_arrays.find(key);
    GLuint result = (it != g_gen_arrays.end()) ? it->second : 0;
    LOG_D("find_real_array: Result for array %u: GLES ID %u", key, result);
    return result;
}

GLuint find_gl4es_array(GLuint mg_array_id) {
    LOG_D("find_gl4es_array: Finding GL4ES ID for MG array %u", mg_array_id);
    if (mg_array_id == 0) return 0;
    auto it = g_va_map_mg_gl4es.find(mg_array_id);
    return (it != g_va_map_mg_gl4es.end()) ? it->second : 0;
}


GLuint find_bound_array() {
    return bound_array;
}

static GLenum get_binding_query(GLenum target) {
    switch(target) {
        case GL_ARRAY_BUFFER:           return GL_ARRAY_BUFFER_BINDING;
        case GL_ELEMENT_ARRAY_BUFFER:   return GL_ELEMENT_ARRAY_BUFFER_BINDING;
        case GL_PIXEL_PACK_BUFFER:      return GL_PIXEL_PACK_BUFFER_BINDING;
        case GL_PIXEL_UNPACK_BUFFER:    return GL_PIXEL_UNPACK_BUFFER_BINDING;
        default:                        return 0;
    }
}

void glGenBuffers(GLsizei n, GLuint *buffers) {
    LOG()
    LOG_D("glGenBuffers(%i, %p)", n, buffers)

    if (n <= 0) return;

    auto gl4es_buffers = (GLuint*)malloc(sizeof(GLuint) * n);
    if (!gl4es_buffers) return;

    GET_GL4ES_FUNC(void, glGenBuffers, GLsizei n, GLuint* buffers)
    CALL_GL4ES_FUNC(glGenBuffers, n, gl4es_buffers)

    for (int i = 0; i < n; ++i) {
        GLuint mg_buffer_id = gen_buffer();
        buffers[i] = mg_buffer_id;
        g_buffer_map_mg_gl4es[mg_buffer_id] = gl4es_buffers[i];
    }

    free(gl4es_buffers);
}

void glGenBuffersARB(GLsizei n, GLuint *buffers) {
    glGenBuffers(n, buffers);
}

void glDeleteBuffers(GLsizei n, const GLuint *buffers) {
    LOG()
    LOG_D("glDeleteBuffers(%i, %p)", n, buffers)

    if (n <= 0 || !buffers) return;

    std::vector<GLuint> gl4es_buffers_to_delete;
    gl4es_buffers_to_delete.reserve(n);

    std::vector<GLuint> gles_buffers_to_delete;
    gles_buffers_to_delete.reserve(n);


    for (int i = 0; i < n; ++i) {
        GLuint mg_buffer_id = buffers[i];
        if (mg_buffer_id == 0) continue;

        GLuint gl4es_buffer = find_gl4es_buffer(mg_buffer_id);
        if (gl4es_buffer != 0) {
            gl4es_buffers_to_delete.push_back(gl4es_buffer);
        } else {
            LOG_D("glDeleteBuffers warning: GL4ES buffer ID not found for MG buffer %u", mg_buffer_id);
        }

        GLuint real_buff = find_real_buffer(mg_buffer_id);
        if (real_buff != 0) {
            gles_buffers_to_delete.push_back(real_buff);
        }

        remove_buffer(mg_buffer_id);

        for (auto it = g_bound_buffers.begin(); it != g_bound_buffers.end(); ) {
            if (it->second == mg_buffer_id) {
                it = g_bound_buffers.erase(it);
            } else {
                ++it;
            }
        }
    }

    if (!gl4es_buffers_to_delete.empty()) {
        GET_GL4ES_FUNC(void, glDeleteBuffers, GLsizei n, const GLuint *buffers)
        CALL_GL4ES_FUNC(glDeleteBuffers, gl4es_buffers_to_delete.size(), gl4es_buffers_to_delete.data())
    }

    if (!gles_buffers_to_delete.empty()) {
        GLES.glDeleteBuffers(gles_buffers_to_delete.size(), gles_buffers_to_delete.data());
        CHECK_GL_ERROR
    }
}

void glDeleteBuffersARB(GLsizei n, const GLuint *buffers) {
    glDeleteBuffers(n, buffers);
}

GLboolean glIsBuffer(GLuint buffer) {
    LOG()
    LOG_D("glIsBuffer, buffer = %d", buffer)
    return has_buffer(buffer);
}

GLboolean glIsBufferARB(GLuint buffer) {
    glIsBuffer(buffer);
}

void glBindBuffer(GLenum target, GLuint buffer) {
    LOG()
    LOG_D("glBindBuffer, target = %s, buffer = %d", glEnumToString(target), buffer)

    GLuint gl4es_buffer = find_gl4es_buffer(buffer);
    GET_GL4ES_FUNC(void, glBindBuffer, GLenum target, GLuint buffer)
    CALL_GL4ES_FUNC(glBindBuffer, target, gl4es_buffer)

    g_bound_buffers[target] = buffer;

    if (buffer == 0) {
        GLES.glBindBuffer(target, 0);
        CHECK_GL_ERROR
        return;
    }

    if (!has_buffer(buffer)) {
        LOG_D("glBindBuffer error: Attempting to bind unknown buffer ID %u", buffer)
        GLES.glBindBuffer(target, 0);
        CHECK_GL_ERROR
        return;
    }

    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        GLES.glGenBuffers(1, &real_buffer);
        CHECK_GL_ERROR
        if (real_buffer != 0) {
            modify_buffer(buffer, real_buffer);
        } else {
            LOG_E("glBindBuffer error: GLES glGenBuffers failed")
            GLES.glBindBuffer(target, 0);
            CHECK_GL_ERROR
            return;
        }
    }
    GLES.glBindBuffer(target, real_buffer);
    CHECK_GL_ERROR
}

void glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) {
    LOG()
    LOG_D("glBindBufferRange, target = %s, index = %d, buffer = %d, offset = %p, size = %zi", glEnumToString(target), index, buffer, (void*) offset, size)

    GLuint gl4es_buffer = find_gl4es_buffer(buffer);
    GET_GL4ES_FUNC(void, glBindBufferRange, GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
    CALL_GL4ES_FUNC(glBindBufferRange, target, index, gl4es_buffer, offset, size)

    g_bound_buffers[target] = buffer;

    if (buffer == 0) {
        GLES.glBindBufferRange(target, index, 0, offset, size);
        CHECK_GL_ERROR
        return;
    }

    if (!has_buffer(buffer)) {
        LOG_D("glBindBufferRange error: Unknown buffer ID %u", buffer)
        GLES.glBindBufferRange(target, index, 0, offset, size);
        CHECK_GL_ERROR
        return;
    }

    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        GLES.glGenBuffers(1, &real_buffer);
        CHECK_GL_ERROR
        if (real_buffer != 0) {
            modify_buffer(buffer, real_buffer);
        } else {
            LOG_D("glBindBufferRange error: GLES glGenBuffers failed")
            GLES.glBindBufferRange(target, index, 0, offset, size);
            CHECK_GL_ERROR
            return;
        }
    }
    GLES.glBindBufferRange(target, index, real_buffer, offset, size);
    CHECK_GL_ERROR
}

void glBindBufferBase(GLenum target, GLuint index, GLuint buffer) {
    LOG()
    LOG_D("glBindBufferBase, target = %s, index = %d, buffer = %d", glEnumToString(target), index, buffer)

    GLuint gl4es_buffer = find_gl4es_buffer(buffer);
    GET_GL4ES_FUNC(void, glBindBufferBase, GLenum target, GLuint index, GLuint buffer)
    CALL_GL4ES_FUNC(glBindBufferBase, target, index, gl4es_buffer)

    g_bound_buffers[target] = buffer;

    if (buffer == 0) {
        GLES.glBindBufferBase(target, index, 0);
        CHECK_GL_ERROR
        return;
    }

    if (!has_buffer(buffer)) {
        LOG_D("glBindBufferBase error: Unknown buffer ID %u", buffer);
        GLES.glBindBufferBase(target, index, 0);
        CHECK_GL_ERROR
        return;
    }

    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        GLES.glGenBuffers(1, &real_buffer);
        CHECK_GL_ERROR
        if (real_buffer != 0) {
            modify_buffer(buffer, real_buffer);
        } else {
            LOG_D("glBindBufferBase error: GLES glGenBuffers failed");
            GLES.glBindBufferBase(target, index, 0);
            CHECK_GL_ERROR
            return;
        }
    }
    GLES.glBindBufferBase(target, index, real_buffer);
    CHECK_GL_ERROR
}

void glBindVertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) {
    LOG()
    LOG_D("glBindVertexBuffer, bindingindex = %d, buffer = %d, offset = %p, stride = %i", bindingindex, buffer, (void*)offset, stride)

    GLuint gl4es_buffer = find_gl4es_buffer(buffer);
    GET_GL4ES_FUNC(void, glBindVertexBuffer, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride)
    CALL_GL4ES_FUNC(glBindVertexBuffer, bindingindex, gl4es_buffer, offset, stride)

    if (buffer == 0) {
        GLES.glBindVertexBuffer(bindingindex, 0, offset, stride);
        CHECK_GL_ERROR
        return;
    }

    if (!has_buffer(buffer)) {
        LOG_D("glBindVertexBuffer error: Unknown buffer ID %u", buffer)
        GLES.glBindVertexBuffer(bindingindex, 0, offset, stride);
        CHECK_GL_ERROR
        return;
    }

    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        GLES.glGenBuffers(1, &real_buffer);
        CHECK_GL_ERROR
        if (real_buffer != 0) {
            modify_buffer(buffer, real_buffer);
        } else {
            LOG_D("glBindVertexBuffer error: GLES glGenBuffers failed");
            GLES.glBindVertexBuffer(bindingindex, 0, offset, stride);
            CHECK_GL_ERROR
            return;
        }
    }
    GLES.glBindVertexBuffer(bindingindex, real_buffer, offset, stride);
    CHECK_GL_ERROR
}


void glTexBuffer(GLenum target, GLenum internalformat, GLuint buffer) {
    LOG()
    LOG_D("glTexBuffer, target = %s, internalformat = %s, buffer = %d", glEnumToString(target), glEnumToString(internalformat), buffer)

    GLuint gl4es_buffer = find_gl4es_buffer(buffer);
    GET_GL4ES_FUNC(void, glTexBuffer, GLenum target, GLenum internalformat, GLuint buffer)
    CALL_GL4ES_FUNC(glTexBuffer, target, internalformat, gl4es_buffer)

    if (buffer == 0) {
        GLES.glTexBuffer(target, internalformat, 0);
        CHECK_GL_ERROR
        return;
    }

    if (!has_buffer(buffer)) {
        LOG_D("glTexBuffer error: Unknown buffer ID %u", buffer)
        GLES.glTexBuffer(target, internalformat, 0);
        CHECK_GL_ERROR
        return;
    }

    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        GLES.glGenBuffers(1, &real_buffer);
        CHECK_GL_ERROR
        if (real_buffer != 0) {
            modify_buffer(buffer, real_buffer);
        } else {
            LOG_D("glTexBuffer error: GLES glGenBuffers failed");
            GLES.glTexBuffer(target, internalformat, 0);
            CHECK_GL_ERROR
            return;
        }
    }
    GLES.glTexBuffer(target, internalformat, real_buffer);
    CHECK_GL_ERROR
}

void glTexBufferRange(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) {
    LOG()
    LOG_D("glTexBufferRange, target = %s, internalformat = %s, buffer = %d, offset = %p, size = %zi", glEnumToString(target), glEnumToString(internalformat), buffer, (void*) offset, size)

    GLuint gl4es_buffer = find_gl4es_buffer(buffer);
    GET_GL4ES_FUNC(void, glTexBufferRange, GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size)
    CALL_GL4ES_FUNC(glTexBufferRange, target, internalformat, gl4es_buffer, offset, size)

    if (buffer == 0) {
        GLES.glTexBufferRange(target, internalformat, 0, offset, size);
        CHECK_GL_ERROR
        return;
    }

    if (!has_buffer(buffer)) {
        LOG_D("glTexBufferRange error: Unknown buffer ID %u", buffer)
        GLES.glTexBufferRange(target, internalformat, 0, offset, size);
        CHECK_GL_ERROR
        return;
    }

    GLuint real_buffer = find_real_buffer(buffer);
    if (!real_buffer) {
        GLES.glGenBuffers(1, &real_buffer);
        CHECK_GL_ERROR
        if (real_buffer != 0) {
            modify_buffer(buffer, real_buffer);
        } else {
            LOG_D("glTexBufferRange error: GLES glGenBuffers failed");
            GLES.glTexBufferRange(target, internalformat, 0, offset, size);
            CHECK_GL_ERROR
            return;
        }
    }
    GLES.glTexBufferRange(target, internalformat, real_buffer, offset, size);
    CHECK_GL_ERROR
}


void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage) {
    LOG()
    LOG_D("glBufferData, target = %s, size = %d, data = %p, usage = %s",
          glEnumToString(target), size, data, glEnumToString(usage))
          
    GLuint mg_buffer = find_bound_buffer(get_binding_query(target));
    GLuint real_buffer = find_real_buffer(mg_buffer);
    gl4es_glBufferData(target, size, data, usage, real_buffer);

    GLES.glBufferData(target, size, data, usage);
    CHECK_GL_ERROR
}

void* glMapBuffer(GLenum target, GLenum access) {
    LOG()

    GET_GL4ES_FUNC(void*, glMapBuffer, GLenum target, GLenum access)
    CALL_GL4ES_FUNC(glMapBuffer, target, access)

    LOG_D("glMapBuffer, target = %s, access = %s", glEnumToString(target), glEnumToString(access))

    if(g_gles_caps.GL_OES_mapbuffer) {
        return GLES.glMapBufferOES(target, access);
    }

    GLenum binding_query = get_binding_query(target);
    if (binding_query == 0) {
        LOG_D("glMapBuffer error: Invalid target %s", glEnumToString(target))
        return nullptr;
    }

    GLint current_gles_buffer;
    GLES.glGetIntegerv(binding_query, &current_gles_buffer);
    CHECK_GL_ERROR

    if (current_gles_buffer == 0) {
        LOG_D("glMapBuffer error: No buffer bound to target %s", glEnumToString(target))
        return nullptr;
    }

    if (g_active_mappings.count(current_gles_buffer)) {
        LOG_D("glMapBuffer error: Buffer %d already mapped", current_gles_buffer)
        return nullptr;
    }

    GLint buffer_size;
    GLES.glGetBufferParameteriv(target, GL_BUFFER_SIZE, &buffer_size);
    if (glGetError() != GL_NO_ERROR || buffer_size <= 0) {
        LOG_D("glMapBuffer error: Failed to get buffer size or size is 0 for buffer %d", current_gles_buffer)
        return nullptr;
    }

    GLbitfield flags = 0;
    switch (access) {
        case GL_READ_ONLY:  flags = GL_MAP_READ_BIT; break;
        case GL_WRITE_ONLY: flags = GL_MAP_WRITE_BIT /*| GL_MAP_INVALIDATE_BUFFER_BIT*/; break;
        case GL_READ_WRITE: flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT; break;
        default:
            LOG_D("glMapBuffer error: Invalid access mode %s", glEnumToString(access))
            return nullptr;
    }

    void* ptr = GLES.glMapBufferRange(target, 0, buffer_size, flags);
    CHECK_GL_ERROR_NO_INIT

    if (!ptr) {
        LOG_D("glMapBuffer: GLES glMapBufferRange failed for buffer %d", current_gles_buffer)
        return nullptr;
    }

    BufferMapping mapping;
    mapping.target = target;
    mapping.buffer_id = (GLuint)current_gles_buffer;
    mapping.mapped_ptr = ptr;
#if GLOBAL_DEBUG || DEBUG
    if (target == GL_PIXEL_UNPACK_BUFFER) {
        mapping.client_ptr = malloc(buffer_size);
        if (mapping.client_ptr) {
            memset(mapping.client_ptr, 0xFF, buffer_size);
        } else {
            GLES.glUnmapBuffer(target);
            g_active_mappings.erase(current_gles_buffer);
            return nullptr;
        }
    } else {
        mapping.client_ptr = nullptr;
    }
#endif
    mapping.size = buffer_size;
    mapping.flags = flags;
    mapping.is_dirty = (flags & GL_MAP_WRITE_BIT) ? GL_TRUE : GL_FALSE;
    g_active_mappings[current_gles_buffer] = mapping;

#if GLOBAL_DEBUG || DEBUG
    if (target == GL_PIXEL_UNPACK_BUFFER && mapping.client_ptr)
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

extern "C" {
GLAPI GLAPIENTRY void *glMapBufferARB(GLenum target, GLenum access) __attribute__((alias("glMapBuffer")));
GLAPI GLAPIENTRY void *glBufferDataARB(GLenum target, GLenum access) __attribute__((alias("glBufferData")));
GLAPI GLAPIENTRY GLboolean glUnmapBufferARB(GLenum target) __attribute__((alias("glUnmapBuffer")));
GLAPI GLAPIENTRY void glBufferStorageARB(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags) __attribute__((alias("glBufferStorage")));
GLAPI GLAPIENTRY void glBindBufferARB(GLenum target, GLuint buffer) __attribute__((alias("glBindBuffer")));
}

GLboolean glUnmapBuffer(GLenum target) {
    LOG()
    GET_GL4ES_FUNC(GLboolean, glUnmapBuffer, GLenum target)
    CALL_GL4ES_FUNC(glUnmapBuffer, target)
    if(g_gles_caps.GL_OES_mapbuffer)
        return GLES.glUnmapBuffer(target);

    GLint buffer;
    GLenum binding_query = get_binding_query(target);
    GLES.glGetIntegerv(binding_query, &buffer);

    if (buffer == 0)
        return GL_FALSE;

#if GLOBAL_DEBUG || DEBUG
    // Blit data from client side to OpenGL here
    if (target == GL_PIXEL_UNPACK_BUFFER) {
        auto &mapping = g_active_mappings[buffer];

        std::fstream fs(std::string(BIN_FILE_PREFIX) + "buf" + std::to_string(buffer) + ".bin", std::ios::out | std::ios::binary | std::ios::trunc);
        fs.write((const char*)mapping.client_ptr, mapping.size);
        fs.close();

        // memset(mapping.mapped_ptr, 0xFF, mapping.size);
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
    
    GET_GL4ES_FUNC(void, glBufferStorage, GLenum target, GLsizeiptr size, const void* data, GLbitfield flags)
    CALL_GL4ES_FUNC(glBufferStorage, target, size, data, flags)

    if(GLES.glBufferStorageEXT) {
        GLES.glBufferStorageEXT(target,size,data,flags);
    }
    CHECK_GL_ERROR
}

void glGenVertexArrays(GLsizei n, GLuint *arrays) {
    LOG()
    LOG_D("glGenVertexArrays(%i, %p)", n, arrays)

    if (n <= 0) return;

    auto gl4es_vas = (GLuint*)malloc(sizeof(GLuint) * n);
    if (!gl4es_vas) return;

    GET_GL4ES_FUNC(void, glGenVertexArrays, GLsizei n, GLuint *arrays)
    CALL_GL4ES_FUNC(glGenVertexArrays, n, gl4es_vas)

    for (int i = 0; i < n; ++i) {
        GLuint mg_array_id = gen_array();
        arrays[i] = mg_array_id;
        g_va_map_mg_gl4es[mg_array_id] = gl4es_vas[i];
    }

    free(gl4es_vas);
}

void glDeleteVertexArrays(GLsizei n, const GLuint *arrays) {
    LOG()
    LOG_D("glDeleteVertexArrays(%i, %p)", n, arrays)

    if (n <= 0 || !arrays) return;

    std::vector<GLuint> gl4es_vas_to_delete;
    gl4es_vas_to_delete.reserve(n);

    std::vector<GLuint> gles_vas_to_delete;
    gles_vas_to_delete.reserve(n);

    for (int i = 0; i < n; ++i) {
        GLuint mg_array_id = arrays[i];
        if (mg_array_id == 0) continue;

        GLuint gl4es_va = find_gl4es_array(mg_array_id);
        if (gl4es_va != 0) {
            gl4es_vas_to_delete.push_back(gl4es_va);
        } else {
            LOG_D("glDeleteVertexArrays warning: GL4ES VA ID not found for MG VA %u", mg_array_id);
        }

        GLuint real_array = find_real_array(mg_array_id);
        if (real_array != 0) {
            gles_vas_to_delete.push_back(real_array);
        }

        if (bound_array == mg_array_id) {
            bound_array = 0;
        }

        remove_array(mg_array_id);
    }

    if (!gl4es_vas_to_delete.empty()) {
        GET_GL4ES_FUNC(void, glDeleteVertexArrays, GLsizei n, const GLuint *arrays)
        CALL_GL4ES_FUNC(glDeleteVertexArrays, gl4es_vas_to_delete.size(), gl4es_vas_to_delete.data())
    }

    if (!gles_vas_to_delete.empty()) {
        GLES.glDeleteVertexArrays(gles_vas_to_delete.size(), gles_vas_to_delete.data());
        CHECK_GL_ERROR
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

    GLuint gl4es_va = find_gl4es_array(array);
    GET_GL4ES_FUNC(void, glBindVertexArray, GLuint array)
    CALL_GL4ES_FUNC(glBindVertexArray, gl4es_va)

    bound_array = array;

    if (array == 0) {
        GLES.glBindVertexArray(0);
        CHECK_GL_ERROR
        return;
    }

    if (!has_array(array)) {
        LOG_D("glBindVertexArray error: Attempting to bind unknown VA ID %u", array)
        GLES.glBindVertexArray(0);
        CHECK_GL_ERROR
        return;
    }

    GLuint real_array = find_real_array(array);
    if (!real_array) {
        GLES.glGenVertexArrays(1, &real_array);
        CHECK_GL_ERROR
        if (real_array != 0) {
            modify_array(array, real_array);
        } else {
            LOG_D("glBindVertexArray error: GLES glGenVertexArrays failed");
            GLES.glBindVertexArray(0);
            CHECK_GL_ERROR
            return;
        }
    }
    GLES.glBindVertexArray(real_array);
    CHECK_GL_ERROR
}
