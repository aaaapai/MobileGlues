//
// Created by BZLZHH on 2025/1/28.
//

#include "buffer.h"
#include <unordered_map>

#define DEBUG 0

GLint maxBufferId = 0;
GLint maxArrayId = 0;

std::unordered_map<GLuint, GLuint> g_gen_buffers;
std::unordered_map<GLenum, GLuint> g_bound_buffers;

std::unordered_map<GLuint, GLuint> g_gen_arrays;

std::unordered_map<GLuint, BufferMapping> g_active_mappings;

GLuint gen_buffer() {
    maxBufferId++;
    g_gen_buffers[maxBufferId] = 0;
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

void bind_buffer(GLenum target, GLuint buffer) {
    g_bound_buffers[target] = buffer;
}

GLuint find_buffer(GLenum target) {
    auto it = g_bound_buffers.find(target);
    if (it != g_bound_buffers.end())
        return it->second;
    else
        return 0;
}

void real_bind_buffer(GLenum target, GLuint buffer) {
    LOG()
    LOG_D("real_bind_buffer, target = %s, buffer = %d", glEnumToString(target), buffer)
    GLES.glBindBuffer(target, buffer);
    CHECK_GL_ERROR
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
        default:                       return 0;
    }
}

void glGenBuffers(GLsizei n, GLuint *buffers) {
    LOG()
    LOG_D("glGenBuffers(%i, %p)", n, buffers)
    for (int i = 0; i < n; ++i) {
        buffers[i] = gen_buffer();
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
    bind_buffer(target, buffer);
    if (find_real_buffer(buffer))
        real_bind_buffer(target, find_real_buffer(buffer));
}

void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage) {
    LOG()
    LOG_D("glBufferData, target = %s, size = %d, data = 0x%x, usage = %s",
          glEnumToString(target), size, data, glEnumToString(usage))
    GLuint real_buff = find_real_buffer(find_buffer(target));
    if (!real_buff) {
        GLES.glGenBuffers(1, &real_buff);
        modify_buffer(find_buffer(target), real_buff);
        CHECK_GL_ERROR
    }
    if (real_buff) {
        real_bind_buffer(target, real_buff);
        GLES.glBufferData(target, size, data, usage);
        CHECK_GL_ERROR
    } else {
        LOG_E("real buffer is null!")
    }
}

void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data) {
    LOG()
    LOG_D("glBufferSubData, target = %s, offset = %p, size = %d, data = 0x%x",
          glEnumToString(target), (void*) offset, size, data)
    GLuint real_buff = find_real_buffer(find_buffer(target));
    if (real_buff) {
        real_bind_buffer(target, real_buff);
        GLES.glBufferSubData(target, offset, size, data);
        CHECK_GL_ERROR
    } else {
        LOG_E("real buffer is null!")
    }
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

#ifdef __cplusplus
extern "C" {
#endif
GLAPI APIENTRY void *glMapBufferARB(GLenum target, GLenum access) __attribute__((alias("glMapBuffer")));
GLAPI APIENTRY void *glBufferDataARB(GLenum target, GLenum access) __attribute__((alias("glBufferData")));
GLAPI APIENTRY GLboolean glUnmapBufferARB(GLenum target) __attribute__((alias("glUnmapBuffer")));
GLAPI APIENTRY void glBufferStorageARB(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags) __attribute__((alias("glBufferStorage")));
GLAPI APIENTRY void glBindBufferARB(GLenum target, GLuint buffer) __attribute__((alias("glBindBuffer")));

#ifdef __cplusplus
}
#endif

GLboolean glUnmapBuffer(GLenum target) {
    LOG()
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
    GLuint real_buff = find_real_buffer(find_buffer(target));
    if (!real_buff) {
        GLES.glGenBuffers(1, &real_buff);
        modify_buffer(find_buffer(target), real_buff);
        CHECK_GL_ERROR
    }
    if (real_buff) {
        real_bind_buffer(target, real_buff);
        if(GLES.glBufferStorageEXT)
            GLES.glBufferStorageEXT(target,size,data,flags);
        CHECK_GL_ERROR
    } else {
        LOG_E("real buffer is null!")
    }
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
    GLuint real_array = find_real_array(array);
    if (!real_array) {
        GLES.glGenVertexArrays(1, &real_array);
        modify_array(array, real_array);
        CHECK_GL_ERROR
    }
    GLES.glBindVertexArray(real_array);
    CHECK_GL_ERROR
}
