// MobileGlues - gl/buffer.cpp
// Copyright (c) 2025-2026 MobileGL-Dev
// Licensed under the GNU Lesser General Public License v2.1:
//   https://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt
// SPDX-License-Identifier: LGPL-2.1-only
// End of Source File Header

#include "buffer.h"
#include "ankerl/unordered_dense.h"
#include "texture.h"

#define DEBUG 0

#define GL_MAP_PERSISTENT_BIT_EXT 0x0040
#define GL_MAP_COHERENT_BIT_EXT 0x0080

static void depth_to_format(float depth, GLenum internalformat, GLubyte* output);
static GLhalf floatToHalf(float f);
static void convert_components_to_internal_format(const float* components, int count, GLenum internalformat, GLubyte* output);

GLuint bound_array;
static GLint maxBufferId = 0;
static GLint maxArrayId = 0;

static std::vector<GLuint> g_gen_buffers;
static std::vector<char> g_gen_buffer_exists;
static std::vector<GLuint> g_free_buffer_ids;

static std::vector<GLuint> g_gen_arrays;
static std::vector<char> g_gen_array_exists;
static std::vector<GLuint> g_free_array_ids;

static std::vector<size_t> g_buffer_datasize;

static std::vector<GLuint> g_element_array_buffer_per_vao;

enum BindingIndex : int {
    BI_ARRAY_BUFFER = 0,
    BI_ATOMIC_COUNTER,
    BI_COPY_READ,
    BI_COPY_WRITE,
    BI_DRAW_INDIRECT,
    BI_DISPATCH_INDIRECT,
    BI_ELEMENT_ARRAY,
    BI_PIXEL_PACK,
    BI_PIXEL_UNPACK,
    BI_SHADER_STORAGE,
    BI_TRANSFORM_FEEDBACK,
    BI_UNIFORM_BUFFER,
    BINDING_COUNT
};
static std::array<GLuint, BINDING_COUNT> g_bound_buffers_arr = {0};

static inline int ensure_buffer_capacity(GLuint id) {
    if ((int)g_gen_buffers.size() <= (int)id) {
        g_gen_buffers.resize(id + 1, 0);
        g_gen_buffer_exists.resize(id + 1, 0);
        if (g_buffer_datasize.size() <= (size_t)id) g_buffer_datasize.resize(id + 1, 0);
    }
    return 0;
}

static inline int ensure_array_capacity(GLuint id) {
    if ((int)g_gen_arrays.size() <= (int)id) {
        g_gen_arrays.resize(id + 1, 0);
        g_gen_array_exists.resize(id + 1, 0);
        if (g_element_array_buffer_per_vao.size() <= (size_t)id) g_element_array_buffer_per_vao.resize(id + 1, 0);
    }
    return 0;
}

GLuint gen_buffer() {
    if (!g_free_buffer_ids.empty()) {
        GLuint id = g_free_buffer_ids.back();
        g_free_buffer_ids.pop_back();
        ensure_buffer_capacity(id);
        g_gen_buffers[id] = 0;
        g_gen_buffer_exists[id] = 1;
        g_buffer_datasize[id] = 0;
        if (id > (GLuint)maxBufferId) maxBufferId = id;
        return id;
    }
    maxBufferId++;
    ensure_buffer_capacity((GLuint)maxBufferId);
    g_gen_buffers[maxBufferId] = 0;
    g_gen_buffer_exists[maxBufferId] = 1;
    g_buffer_datasize[maxBufferId] = 0;
    return (GLuint)maxBufferId;
}

GLboolean has_buffer(GLuint key) {
    return key < g_gen_buffer_exists.size() ? (g_gen_buffer_exists[key] != 0) : 0;
}

void modify_buffer(GLuint key, GLuint value) {
    if (key >= g_gen_buffers.size()) ensure_buffer_capacity(key);
    g_gen_buffers[key] = value;
    if (key >= g_gen_buffer_exists.size()) g_gen_buffer_exists.resize(key + 1, 0);
    g_gen_buffer_exists[key] = 1;
}

void remove_buffer(GLuint key) {
    if (key < g_gen_buffer_exists.size() && g_gen_buffer_exists[key]) {
        g_gen_buffer_exists[key] = 0;
        g_gen_buffers[key] = 0;
        if (key < g_buffer_datasize.size()) g_buffer_datasize[key] = 0;
        g_free_buffer_ids.push_back(key);
    }
}

GLuint find_real_buffer(GLuint key) {
    if (key < g_gen_buffers.size() && g_gen_buffer_exists[key]) return g_gen_buffers[key];
    return 0;
}

GLuint get_ibo_by_vao(GLuint vao) {
    if (vao < g_element_array_buffer_per_vao.size()) return g_element_array_buffer_per_vao[vao];
    return 0;
}

GLuint find_bound_array() {
    return bound_array;
}

void update_vao_ibo_binding(GLuint vao, GLuint ibo) {
    ensure_array_capacity(vao);
    g_element_array_buffer_per_vao[vao] = ibo;
}

void set_buffer_data_size(GLuint buffer, size_t size) {
    ensure_buffer_capacity(buffer);
    g_buffer_datasize[buffer] = size;
}

size_t get_buffer_data_size(GLuint buffer) {
    if (buffer < g_buffer_datasize.size()) return g_buffer_datasize[buffer];
    return 0;
}

static inline int binding_target_to_index(GLenum target) {
    switch (target) {
    case GL_ARRAY_BUFFER:
        return BI_ARRAY_BUFFER;
    case GL_ATOMIC_COUNTER_BUFFER:
        return BI_ATOMIC_COUNTER;
    case GL_COPY_READ_BUFFER:
        return BI_COPY_READ;
    case GL_COPY_WRITE_BUFFER:
        return BI_COPY_WRITE;
    case GL_DRAW_INDIRECT_BUFFER:
        return BI_DRAW_INDIRECT;
    case GL_DISPATCH_INDIRECT_BUFFER:
        return BI_DISPATCH_INDIRECT;
    case GL_ELEMENT_ARRAY_BUFFER:
        return BI_ELEMENT_ARRAY;
    case GL_PIXEL_PACK_BUFFER:
        return BI_PIXEL_PACK;
    case GL_PIXEL_UNPACK_BUFFER:
        return BI_PIXEL_UNPACK;
    case GL_SHADER_STORAGE_BUFFER:
        return BI_SHADER_STORAGE;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        return BI_TRANSFORM_FEEDBACK;
    case GL_UNIFORM_BUFFER:
        return BI_UNIFORM_BUFFER;
    default:
        return -1;
    }
}

void set_bound_buffer_by_target(GLenum target, GLuint buffer) {
    int idx = binding_target_to_index(target);
    if (idx >= 0) g_bound_buffers_arr[idx] = buffer;
}

GLuint find_bound_buffer(GLenum key) {
    GLenum target = 0;
    switch (key) {
    case GL_ARRAY_BUFFER_BINDING:
        target = GL_ARRAY_BUFFER;
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
    int idx = binding_target_to_index(target);
    if (idx >= 0) return g_bound_buffers_arr[idx];
    return 0;
}

GLuint gen_array() {
    if (!g_free_array_ids.empty()) {
        GLuint id = g_free_array_ids.back();
        g_free_array_ids.pop_back();
        ensure_array_capacity(id);
        g_gen_arrays[id] = 0;
        g_gen_array_exists[id] = 1;
        g_element_array_buffer_per_vao[id] = 0;
        if (id > (GLuint)maxArrayId) maxArrayId = id;
        return id;
    }
    maxArrayId++;
    ensure_array_capacity((GLuint)maxArrayId);
    g_gen_arrays[maxArrayId] = 0;
    g_gen_array_exists[maxArrayId] = 1;
    g_element_array_buffer_per_vao[maxArrayId] = 0;
    return (GLuint)maxArrayId;
}

GLboolean has_array(GLuint key) {
    return key < g_gen_array_exists.size() ? (g_gen_array_exists[key] != 0) : 0;
}

void modify_array(GLuint key, GLuint value) {
    if (key >= g_gen_arrays.size()) ensure_array_capacity(key);
    g_gen_arrays[key] = value;
    if (key >= g_gen_array_exists.size()) g_gen_array_exists.resize(key + 1, 0);
    g_gen_array_exists[key] = 1;
}

void remove_array(GLuint key) {
    if (key < g_gen_array_exists.size() && g_gen_array_exists[key]) {
        g_gen_array_exists[key] = 0;
        g_gen_arrays[key] = 0;
        if (key < g_element_array_buffer_per_vao.size()) g_element_array_buffer_per_vao[key] = 0;
        g_free_array_ids.push_back(key);
    }
}

GLuint find_real_array(GLuint key) {
    if (key < g_gen_arrays.size() && g_gen_array_exists[key]) return g_gen_arrays[key];
    return 0;
}

static GLenum get_binding_query(GLenum target) {
    switch (target) {
    case GL_ARRAY_BUFFER:
        return GL_ARRAY_BUFFER_BINDING;
    case GL_ELEMENT_ARRAY_BUFFER:
        return GL_ELEMENT_ARRAY_BUFFER_BINDING;
    case GL_PIXEL_PACK_BUFFER:
        return GL_PIXEL_PACK_BUFFER_BINDING;
    case GL_PIXEL_UNPACK_BUFFER:
        return GL_PIXEL_UNPACK_BUFFER_BINDING;
    case GL_COPY_WRITE_BUFFER:
        return GL_COPY_WRITE_BUFFER_BINDING;
    case GL_COPY_READ_BUFFER:
        return GL_COPY_READ_BUFFER_BINDING;
    case GL_UNIFORM_BUFFER:
        return GL_UNIFORM_BUFFER_BINDING;
    case GL_SHADER_STORAGE_BUFFER:
        return GL_SHADER_STORAGE_BUFFER_BINDING;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        return GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
    case GL_ATOMIC_COUNTER_BUFFER:
        return GL_ATOMIC_COUNTER_BUFFER_BINDING;
    case GL_DRAW_INDIRECT_BUFFER:
        return GL_DRAW_INDIRECT_BUFFER_BINDING;
    case GL_DISPATCH_INDIRECT_BUFFER:
        return GL_DISPATCH_INDIRECT_BUFFER_BINDING;
    default:
        return 0;
    }
}

void InitBufferMap(size_t expectedSize) {
    g_gen_buffers.reserve(expectedSize + 2);
    g_gen_buffer_exists.reserve(expectedSize + 2);
    g_buffer_datasize.reserve(expectedSize + 2);
    g_gen_buffers.resize(1, 0);
    g_gen_buffer_exists.resize(1, 0);
    g_buffer_datasize.resize(1, 0);
}

void InitVertexArrayMap(size_t expectedSize) {
    g_gen_arrays.reserve(expectedSize + 2);
    g_gen_array_exists.reserve(expectedSize + 2);
    g_element_array_buffer_per_vao.reserve(expectedSize + 2);
    g_gen_arrays.resize(1, 0);
    g_gen_array_exists.resize(1, 0);
    g_element_array_buffer_per_vao.resize(1, 0);
}

// 在文件开头添加
static GLenum convert_flags_to_usage(GLbitfield flags) {
    // 将 GLbufferStorage 标志转换为传统的 GLbufferData usage
    if (flags & GL_DYNAMIC_STORAGE_BIT)
        return GL_DYNAMIC_DRAW;
    if (flags & GL_MAP_WRITE_BIT)
        return GL_STREAM_DRAW;
    return GL_STATIC_DRAW;
}

void glGenBuffers(GLsizei n, GLuint* buffers) {
    LOG()
    LOG_D("glGenBuffers(%i, %p)", n, buffers)
    for (int i = 0; i < n; ++i) {
        buffers[i] = gen_buffer();
    }
}

void glDeleteBuffers(GLsizei n, const GLuint* buffers) {
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
    set_bound_buffer_by_target(target, buffer);
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

struct atomic_buffer {
    GLuint id;
    GLsizeiptr size;
    GLintptr offset;
};

static std::vector<atomic_buffer> g_buffer_map_atomic_buffer_info;
static std::vector<GLuint> g_buffer_map_ssbo_id; // shall we use this in the future?

/*void bindAllAtomicCounterAsSSBO() {
    const size_t count = g_buffer_map_atomic_buffer_info.size();
    for (size_t i = 0; i < count; ++i) {
        atomic_buffer buf = g_buffer_map_atomic_buffer_info[i];
        if (buf.id != 0) {
            GLuint realID = find_real_buffer(buf.id);
            GLES.glBindBufferRange(GL_SHADER_STORAGE_BUFFER, i, realID, buf.offset, buf.size);
            LOG_D("Bound atomic counter buffer %u(real: %u) as SSBO at index %zu", buf, realID, i);
        }
    }
}*/

// 改进原子计数器到 SSBO 的映射
void bindAllAtomicCounterAsSSBO() {
    const size_t count = g_buffer_map_atomic_buffer_info.size();
    if (count == 0) return;
    
    // 动态获取 SSBO 限制值
    GLint max_ssbo_bindings = 0;
    GLint max_ssbo_block_size = 0;
    
    // 获取最大 SSBO 绑定点数
    GLES.glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &max_ssbo_bindings);
    if (glGetError() != GL_NO_ERROR) {
        // 如果不支持，使用保守的默认值
        max_ssbo_bindings = 8; // ES 3.2 最小保证值
        LOG_W("GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS not supported, using default %d", max_ssbo_bindings);
    }
    
    // 获取最大 SSBO 块大小
    GLES.glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &max_ssbo_block_size);
    if (glGetError() != GL_NO_ERROR) {
        // 如果不支持，使用保守的默认值
        max_ssbo_block_size = 16 * 1024 * 1024; // 16MB，ES 3.2 最小保证值
        LOG_W("GL_MAX_SHADER_STORAGE_BLOCK_SIZE not supported, using default %d", max_ssbo_block_size);
    }
    
    LOG_D("SSBO limits - bindings: %d, block size: %d bytes", max_ssbo_bindings, max_ssbo_block_size);
    
    for (size_t i = 0; i < count; ++i) {
        atomic_buffer buf = g_buffer_map_atomic_buffer_info[i];
        if (buf.id != 0) {
            GLuint realID = find_real_buffer(buf.id);
            if (!realID) {
                LOG_W("Atomic counter buffer %u has no real buffer", buf.id);
                continue;
            }
            
            // 检查 SSBO 绑定索引是否超出限制
            if (i >= (size_t)max_ssbo_bindings) {
                LOG_W("Atomic counter index %zu exceeds SSBO bindings limit %d", 
                      i, max_ssbo_bindings);
                continue;
            }
            
            // 检查块大小是否超出限制
            if (buf.size > max_ssbo_block_size) {
                LOG_W("Atomic buffer size %lld exceeds SSBO block size limit %d",
                      (long long)buf.size, max_ssbo_block_size);
                // 可以选择截断或继续但警告
            }
            
            GLES.glBindBufferRange(GL_SHADER_STORAGE_BUFFER, i, realID, buf.offset, buf.size);
            
            // 检查绑定是否成功
            GLenum err = glGetError();
            if (err != GL_NO_ERROR) {
                LOG_E("Failed to bind atomic counter %zu as SSBO: 0x%x", i, err);
            } else {
                LOG_D("Bound atomic counter buffer %u(real: %u) as SSBO at index %zu", 
                      buf.id, realID, i);
            }
        }
    }
}

void glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) {
    LOG()
    LOG_D("glBindBufferRange, target = %s, index = %d, buffer = %d, offset = %p, size = %zi", glEnumToString(target),
          index, buffer, (void*)offset, size)

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
    LOG_D("glBindVertexBuffer, bindingindex = %d, buffer = %d, offset = %p, stride = %i", bindingindex, buffer, offset,
          stride)
    // Todo: should record fake buffer binding here, when glGetVertexArrayIntegeri_v is called, should return fake
    // buffer id
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

size_t get_internal_format_size(GLenum internalformat) {
    switch (internalformat) {
    case GL_R8:
        return 1;
    case GL_R8I:
    case GL_R8UI:
        return 1;
    case GL_R16:
        return 2;
    case GL_R16I:
    case GL_R16UI:
    case GL_R16F:
        return 2;
    case GL_R32I:
    case GL_R32UI:
    case GL_R32F:
        return 4;

    case GL_RG8:
        return 2;
    case GL_RG8I:
    case GL_RG8UI:
        return 2;
    case GL_RG16:
        return 4;
    case GL_RG16I:
    case GL_RG16UI:
    case GL_RG16F:
        return 4;
    case GL_RG32I:
    case GL_RG32UI:
    case GL_RG32F:
        return 8;

    case GL_RGB8:
        return 3;
    case GL_RGB8I:
    case GL_RGB8UI:
        return 3;
    case GL_RGB16:
        return 6;
    case GL_RGB16I:
    case GL_RGB16UI:
    case GL_RGB16F:
        return 6;
    case GL_RGB32I:
    case GL_RGB32UI:
    case GL_RGB32F:
        return 12;

    case GL_RGBA8:
        return 4;
    case GL_RGBA8I:
    case GL_RGBA8UI:
        return 4;
    case GL_RGBA16:
        return 8;
    case GL_RGBA16I:
    case GL_RGBA16UI:
    case GL_RGBA16F:
        return 8;
    case GL_RGBA32I:
    case GL_RGBA32UI:
    case GL_RGBA32F:
        return 16;

    case GL_DEPTH_COMPONENT16:
        return 2;
    case GL_DEPTH_COMPONENT24:
        return 3;
    case GL_DEPTH_COMPONENT32:
        return 4;
    case GL_DEPTH_COMPONENT32F:
        return 4;
    case GL_DEPTH24_STENCIL8:
        return 4;
    case GL_DEPTH32F_STENCIL8:
        return 5;

    case GL_STENCIL_INDEX8:
        return 1;

    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        return 8;
    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
        return 16;

    default:
        LOG_E("Unknown internal format size for %s", glEnumToString(internalformat));
        return 0;
    }
}

extern std::string bufSampelerName;
// Todo: any glGet* related to this function?
void glTexBuffer(GLenum target, GLenum internalformat, GLuint buffer) {
    LOG()
    LOG_D("glTexBuffer, target = %s, internalformat = %s, buffer = %d", glEnumToString(target),
          glEnumToString(internalformat), buffer)
    if (target != GL_TEXTURE_BUFFER) return;

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

    if (hardware->emulate_texture_buffer) {
        LOG_D("Emulating glTexBuffer");

        GLint boundTexture = 0;
        GLint prev_pixel_buffer_binding = 0;

        GLES.glActiveTexture(GL_TEXTURE0 + 15);

        GLES.glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
        LOG_D("Current GL_TEXTURE_BINDING_BUFFER = %d", boundTexture);
        GLES.glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &prev_pixel_buffer_binding);
        LOG_D("Previous GL_PIXEL_UNPACK_BUFFER_BINDING = %d", prev_pixel_buffer_binding);

        if (!boundTexture) {
            LOG_D("No texture bound to GL_TEXTURE_BUFFER, skipping emulation.");
            return;
        }

        GLES.glBindBuffer(GL_PIXEL_UNPACK_BUFFER, real_buffer);
        LOG_D("Bound GL_PIXEL_UNPACK_BUFFER to buffer %u", real_buffer);

        GLint bufferSize;
        GLES.glGetBufferParameteriv(GL_PIXEL_UNPACK_BUFFER, GL_BUFFER_SIZE, &bufferSize);
        LOG_D("Buffer size = %d bytes", bufferSize);

        GLES.glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        GLES.glBindTexture(GL_TEXTURE_2D, boundTexture);
        LOG_D("Binding texture %u to GL_TEXTURE_2D", boundTexture);

        const GLuint MAX_WIDTH = 8192;
        GLuint pixelSize = get_internal_format_size(internalformat);
        GLuint numElements = bufferSize / pixelSize;

        GLuint width = numElements;
        GLuint height = 1;

        if (width > MAX_WIDTH) {
            width = MAX_WIDTH;
            height = (numElements + MAX_WIDTH - 1) / MAX_WIDTH;
        }

        GLint prev_alignment, prev_row_length, prev_skip_pixels, prev_skip_rows;
        GLES.glGetIntegerv(GL_UNPACK_ALIGNMENT, &prev_alignment);
        GLES.glGetIntegerv(GL_UNPACK_ROW_LENGTH, &prev_row_length);
        GLES.glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &prev_skip_pixels);
        GLES.glGetIntegerv(GL_UNPACK_SKIP_ROWS, &prev_skip_rows);

        // why do these 2 params not work
        // GLES.glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // GLES.glPixelStorei(GL_UNPACK_ROW_LENGTH, 0)
        GLES.glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        GLES.glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

        // TODO: Optimize the glTexImage2D call
        GLES.glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, GL_RED_INTEGER, GL_BYTE, nullptr);

        GLES.glBindBuffer(GL_PIXEL_UNPACK_BUFFER, real_buffer);

        for (GLuint row = 0; row < height; ++row) {
            void* offset = (void*)(row * width * pixelSize);
            GLES.glTexSubImage2D(GL_TEXTURE_2D, 0, 0, row, width, 1, GL_RED_INTEGER, GL_BYTE, offset);
        }

        GLES.glPixelStorei(GL_UNPACK_ALIGNMENT, prev_alignment);
        GLES.glPixelStorei(GL_UNPACK_ROW_LENGTH, prev_row_length);
        GLES.glPixelStorei(GL_UNPACK_SKIP_PIXELS, prev_skip_pixels);
        GLES.glPixelStorei(GL_UNPACK_SKIP_ROWS, prev_skip_rows);

        auto tex = mgGetTexObjectByTarget(target);
        tex->target = ConvertGLEnumToTextureTarget(target);
        tex->internal_format = internalformat;
        tex->width = width;
        tex->height = height;
        tex->depth = 1;
        tex->swizzle_param[0] = GL_RED;
        tex->swizzle_param[1] = GL_GREEN;
        tex->swizzle_param[2] = GL_BLUE;
        tex->swizzle_param[3] = GL_ALPHA;

        LOG_D("Called glTexImage2D with internalformat = 0x%X", internalformat);

        GLES.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        GLES.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        GLES.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GLES.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GLES.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        GLES.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        LOG_D("Set texture parameters: MIN_FILTER=NEAREST, MAG_FILTER=NEAREST, WRAP_S/T=CLAMP_TO_EDGE");

        GLES.glBindBuffer(GL_PIXEL_UNPACK_BUFFER, prev_pixel_buffer_binding);

        GLES.glActiveTexture(GL_TEXTURE0 + gl_state->current_tex_unit);

        LOG_D("Restored bindings: GL_PIXEL_UNPACK_BUFFER=%d", prev_pixel_buffer_binding);

        CHECK_GL_ERROR;
        return;
    }

    GLES.glTexBuffer(target, internalformat, real_buffer);
    CHECK_GL_ERROR
}

void glTexBufferRange(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) {
    LOG()
    LOG_D("glTexBufferRange, target = %s, internalformat = %s, buffer = %d, offset = %p, size = %zi",
          glEnumToString(target), glEnumToString(internalformat), buffer, (void*)offset, size)
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

void glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) {
    LOG()
    LOG_D("glBufferData, target = %s, size = %d, data = 0x%x, usage = %s", glEnumToString(target), size, data,
          glEnumToString(usage))
    GLES.glBufferData(target, size, data, usage);
    set_buffer_data_size(find_bound_buffer(target), size);
    CHECK_GL_ERROR
}

void* glMapBuffer(GLenum target, GLenum access) {
    LOG()
    LOG_D("glMapBuffer, target = %s, access = %s", glEnumToString(target), glEnumToString(access))
    if (g_gles_caps.GL_OES_mapbuffer) {
        return GLES.glMapBufferOES(target, access);
    }
    GLint buffer_size;
    GLES.glGetBufferParameteriv(target, GL_BUFFER_SIZE, &buffer_size);
    if (buffer_size <= 0 || glGetError() != GL_NO_ERROR) {
        return nullptr;
    }
    GLbitfield flags = 0;
    switch (access) {
    case GL_READ_ONLY:
        flags = GL_MAP_READ_BIT;
        break;
    case GL_WRITE_ONLY:
        flags = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
        break;
    case GL_READ_WRITE:
        flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
        break;
    default:
        return nullptr;
    }
    void* ptr = glMapBufferRange(target, 0, buffer_size, flags);
    return ptr;
}

#if GLOBAL_DEBUG || DEBUG
#include <fstream>
#define BIN_FILE_PREFIX "/sdcard/MG/buf/"
#endif

#if !defined(__APPLE__)
extern "C"
{
    GLAPI GLAPIENTRY void* glMapBufferARB(GLenum target, GLenum access) __attribute__((alias("glMapBuffer")));
    GLAPI GLAPIENTRY void glBufferDataARB(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
        __attribute__((alias("glBufferData")));
    GLAPI GLAPIENTRY GLboolean glUnmapBufferARB(GLenum target) __attribute__((alias("glUnmapBuffer")));
    GLAPI GLAPIENTRY void glBufferStorageARB(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags)
        __attribute__((alias("glBufferStorage")));
    GLAPI GLAPIENTRY void glBindBufferARB(GLenum target, GLuint buffer) __attribute__((alias("glBindBuffer")));
    
    /* 添加的 ARB 缓冲区函数别名 */
    GLAPI GLAPIENTRY void glGenBuffersARB(GLsizei n, GLuint* buffers) __attribute__((alias("glGenBuffers")));
    GLAPI GLAPIENTRY GLboolean glIsBufferARB(GLuint buffer) __attribute__((alias("glIsBuffer")));
    GLAPI GLAPIENTRY void glDeleteBuffersARB(GLsizei n, const GLuint* buffers) __attribute__((alias("glDeleteBuffers")));
}
#endif

void* glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) {
    LOG()
    if (global_settings.buffer_coherent_as_flush) access &= ~GL_MAP_FLUSH_EXPLICIT_BIT;
    //    access |= GL_MAP_UNSYNCHRONIZED_BIT;
    return GLES.glMapBufferRange(target, offset, length, access);
}

GLboolean glUnmapBuffer(GLenum target) {
    LOG()
    LOG_D("%s(%s)", __func__, glEnumToString(target));
    if (g_gles_caps.GL_OES_mapbuffer) return GLES.glUnmapBuffer(target);

    GLboolean result = GLES.glUnmapBuffer(target);
    CHECK_GL_ERROR
    return result;
}

void glBufferStorage(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags) {
    LOG()
    
    if (GLES.glBufferStorageEXT) {
        GLbitfield es_supported_flags = flags & (GL_DYNAMIC_STORAGE_BIT | 
                                                 GL_MAP_READ_BIT | 
                                                 GL_MAP_WRITE_BIT | 
                                                 GL_MAP_PERSISTENT_BIT_EXT |  // 需要扩展
                                                 GL_MAP_COHERENT_BIT_EXT);    // 需要扩展
        
            GLES.glBufferStorageEXT(target, size, data, es_supported_flags);
        }
    CHECK_GL_ERROR
}


void glFlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length) {
    LOG()
    if (!global_settings.buffer_coherent_as_flush) GLES.glFlushMappedBufferRange(target, offset, length);
}

void glGenVertexArrays(GLsizei n, GLuint* arrays) {
    LOG()
    LOG_D("glGenVertexArrays(%i, %p)", n, arrays)
    for (int i = 0; i < n; ++i) {
        arrays[i] = gen_array();
    }
}

void glDeleteVertexArrays(GLsizei n, const GLuint* arrays) {
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
    set_bound_buffer_by_target(GL_ELEMENT_ARRAY_BUFFER, get_ibo_by_vao(array));

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

// 获取格式/类型组合的字节大小
static GLsizei get_format_type_size(GLenum format, GLenum type) {
    GLsizei component_count = 0;
    switch (format) {
        case GL_RED:
        case GL_GREEN:
        case GL_BLUE:
        case GL_ALPHA:
        case GL_DEPTH_COMPONENT:
        case GL_STENCIL_INDEX:
            component_count = 1;
            break;
        case GL_RG:
            component_count = 2;
            break;
        case GL_RGB:
            component_count = 3;
            break;
        case GL_RGBA:
        case GL_BGRA:
            component_count = 4;
            break;
        case GL_DEPTH_STENCIL:
            component_count = 2;
            break;
        default:
            return 0;
    }

    GLsizei component_size = 0;
    switch (type) {
        case GL_UNSIGNED_BYTE:
        case GL_BYTE:
            component_size = 1;
            break;
        case GL_UNSIGNED_SHORT:
        case GL_SHORT:
        case GL_HALF_FLOAT:
            component_size = 2;
            break;
        case GL_UNSIGNED_INT:
        case GL_INT:
        case GL_FLOAT:
            component_size = 4;
            break;
        default:
            return 0;
    }

    return component_count * component_size;
}

// 深度值转换到特定格式
static void depth_to_format(float depth, GLenum internalformat, GLubyte* output) {
    if (!output) return;
    
    // 钳位深度值到有效范围 [0, 1]
    depth = std::max(0.0f, std::min(1.0f, depth));
    
    switch (internalformat) {
        // 16位深度（无符号归一化整数）
        case GL_DEPTH_COMPONENT16: {
            GLushort* depth_out = (GLushort*)output;
            // 16位深度：0-65535 映射到 0.0-1.0
            *depth_out = (GLushort)(depth * 65535.0f);
            break;
        }
        
        // 24位深度（无符号归一化整数）
        case GL_DEPTH_COMPONENT24: {
            // 24位存储，打包在3个字节中
            GLuint depth_val = (GLuint)(depth * 16777215.0f); // 2^24 - 1
            
            // 小端序：低字节在前
            output[0] = (GLubyte)(depth_val & 0xFF);
            output[1] = (GLubyte)((depth_val >> 8) & 0xFF);
            output[2] = (GLubyte)((depth_val >> 16) & 0xFF);
            break;
        }
        
        // 32位浮点深度
        case GL_DEPTH_COMPONENT32F: {
            GLfloat* depth_out = (GLfloat*)output;
            *depth_out = depth;
            break;
        }
        
        default:
            LOG_W("depth_to_format: Unsupported internalformat %s for depth conversion", 
                  glEnumToString(internalformat));
            break;
    }
}

// 处理打包格式
static std::vector<GLubyte> convert_packed_clear_data(const void* data, GLenum format, 
                                                       GLenum type, GLenum internalformat) {
    std::vector<GLubyte> result;
    GLsizei internal_size = get_internal_format_size(internalformat);
    if (internal_size == 0) return result;
    
    result.resize(internal_size);
    
    // 解析打包格式
    uint32_t packed = 0;
    GLsizei packed_size = 0;
    
    switch (type) {
        case GL_UNSIGNED_SHORT_5_6_5:
            packed = *(GLushort*)data;
            packed_size = 2;
            break;
        case GL_UNSIGNED_SHORT_4_4_4_4:
        case GL_UNSIGNED_SHORT_5_5_5_1:
            packed = *(GLushort*)data;
            packed_size = 2;
            break;
        case GL_UNSIGNED_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
        case GL_UNSIGNED_INT_5_9_9_9_REV:
            packed = *(GLuint*)data;
            packed_size = 4;
            break;
        default:
            return result;
    }
    
    // 提取组件
    float components[4] = {0, 0, 0, 1}; // 默认alpha=1
    
    switch (type) {
        case GL_UNSIGNED_SHORT_5_6_5:
            components[0] = ((packed >> 11) & 0x1F) / 31.0f;  // R
            components[1] = ((packed >> 5) & 0x3F) / 63.0f;   // G
            components[2] = (packed & 0x1F) / 31.0f;          // B
            break;
        case GL_UNSIGNED_SHORT_4_4_4_4:
            components[0] = ((packed >> 12) & 0xF) / 15.0f;   // R
            components[1] = ((packed >> 8) & 0xF) / 15.0f;    // G
            components[2] = ((packed >> 4) & 0xF) / 15.0f;    // B
            components[3] = (packed & 0xF) / 15.0f;           // A
            break;
        case GL_UNSIGNED_SHORT_5_5_5_1:
            components[0] = ((packed >> 11) & 0x1F) / 31.0f;  // R
            components[1] = ((packed >> 6) & 0x1F) / 31.0f;   // G
            components[2] = ((packed >> 1) & 0x1F) / 31.0f;   // B
            components[3] = (packed & 0x1);                  // A
            break;
        case GL_UNSIGNED_INT_2_10_10_10_REV:
            components[0] = (packed & 0x3FF) / 1023.0f;       // R
            components[1] = ((packed >> 10) & 0x3FF) / 1023.0f; // G
            components[2] = ((packed >> 20) & 0x3FF) / 1023.0f; // B
            components[3] = ((packed >> 30) & 0x3) / 3.0f;    // A
            break;
        // 其他打包格式...
    }
    
    // 转换到内部格式
    convert_components_to_internal_format(components, 4, internalformat, result.data());
    
    return result;
}

// 处理深度模板格式
static std::vector<GLubyte> convert_depth_stencil_clear_data(const void* data, GLenum format,
                                                              GLenum type, GLenum internalformat) {
    std::vector<GLubyte> result;
    GLsizei internal_size = get_internal_format_size(internalformat);
    if (internal_size == 0) return result;
    
    result.resize(internal_size);
    memset(result.data(), 0, internal_size);
    
    float depth = 0.0f;
    int stencil = 0;
    
    if (format == GL_DEPTH_STENCIL || format == GL_DEPTH_COMPONENT) {
        // 提取深度值
        switch (type) {
            case GL_UNSIGNED_INT_24_8:
            case GL_FLOAT_32_UNSIGNED_INT_24_8_REV: {
                uint32_t packed = *(GLuint*)data;
                if (type == GL_UNSIGNED_INT_24_8) {
                    depth = (packed >> 8) / 16777215.0f;
                    stencil = packed & 0xFF;
                } else {
                    depth = *(GLfloat*)data;
                    stencil = ((uint32_t*)data)[1] & 0xFF;
                }
                break;
            }
            case GL_FLOAT:
                depth = *(GLfloat*)data;
                break;
            case GL_UNSIGNED_INT:
                depth = *(GLuint*)data / 4294967295.0f;
                break;
            case GL_UNSIGNED_SHORT:
                depth = *(GLushort*)data / 65535.0f;
                break;
        }
    }
    
    if (format == GL_STENCIL_INDEX) {
        // 提取模板值
        switch (type) {
            case GL_UNSIGNED_BYTE:
                stencil = *(GLubyte*)data;
                break;
            case GL_UNSIGNED_INT:
                stencil = *(GLuint*)data;
                break;
        }
    }
    
    // 根据internalformat写入目标格式
    switch (internalformat) {
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32F:
            // 只写深度
            depth_to_format(depth, internalformat, result.data());
            break;
        case GL_DEPTH24_STENCIL8:
            // 24位深度 + 8位模板
            depth_to_format(depth, GL_DEPTH_COMPONENT24, result.data());
            result[3] = stencil & 0xFF;  // 模板值存储在第4字节
            break;
        case GL_DEPTH32F_STENCIL8:
            // 32位浮点深度 + 8位模板
            *(GLfloat*)result.data() = depth;
            result[4] = stencil & 0xFF;  // 模板值存储在第5字节
            break;
    }
    
    return result;
}

// 将组件值转换到内部格式
static void convert_components_to_internal_format(const float* components, int count,
                                                   GLenum internalformat, GLubyte* output) {
    // 获取内部格式的组件数量和类型
    GLenum base_format;
    GLenum data_type;
    GLint internal_components;
    bool is_snorm = false;
    bool is_unorm = false;
    bool is_int = false;
    bool is_uint = false;
    bool is_float = false;
    
    switch (internalformat) {
        // 8-bit
        case GL_R8:
        case GL_R8_SNORM:
            internal_components = 1;
            base_format = GL_RED;
            data_type = is_snorm ? GL_BYTE : GL_UNSIGNED_BYTE;
            is_unorm = !is_snorm;
            break;
        case GL_R8I:
            internal_components = 1;
            base_format = GL_RED_INTEGER;
            data_type = GL_BYTE;
            is_int = true;
            break;
        case GL_R8UI:
            internal_components = 1;
            base_format = GL_RED_INTEGER;
            data_type = GL_UNSIGNED_BYTE;
            is_uint = true;
            break;
            
        // 16-bit
        case GL_R16:
        case GL_R16_SNORM:
            internal_components = 1;
            base_format = GL_RED;
            data_type = is_snorm ? GL_SHORT : GL_UNSIGNED_SHORT;
            is_unorm = !is_snorm;
            break;
        case GL_R16I:
            internal_components = 1;
            base_format = GL_RED_INTEGER;
            data_type = GL_SHORT;
            is_int = true;
            break;
        case GL_R16UI:
            internal_components = 1;
            base_format = GL_RED_INTEGER;
            data_type = GL_UNSIGNED_SHORT;
            is_uint = true;
            break;
        case GL_R16F:
            internal_components = 1;
            base_format = GL_RED;
            data_type = GL_HALF_FLOAT;
            is_float = true;
            break;
            
        // 32-bit
        case GL_R32I:
            internal_components = 1;
            base_format = GL_RED_INTEGER;
            data_type = GL_INT;
            is_int = true;
            break;
        case GL_R32UI:
            internal_components = 1;
            base_format = GL_RED_INTEGER;
            data_type = GL_UNSIGNED_INT;
            is_uint = true;
            break;
        case GL_R32F:
            internal_components = 1;
            base_format = GL_RED;
            data_type = GL_FLOAT;
            is_float = true;
            break;
            
        // RG formats...
        case GL_RG8:
        case GL_RG8_SNORM:
            internal_components = 2;
            base_format = GL_RG;
            data_type = is_snorm ? GL_BYTE : GL_UNSIGNED_BYTE;
            is_unorm = !is_snorm;
            break;
        case GL_RG8I:
            internal_components = 2;
            base_format = GL_RG_INTEGER;
            data_type = GL_BYTE;
            is_int = true;
            break;
        // ... 其他RG格式类似
        
        // RGB formats...
        case GL_RGB8:
        case GL_RGB8_SNORM:
            internal_components = 3;
            base_format = GL_RGB;
            data_type = is_snorm ? GL_BYTE : GL_UNSIGNED_BYTE;
            is_unorm = !is_snorm;
            break;
        // ... 其他RGB格式类似
        
        // RGBA formats...
        case GL_RGBA8:
        case GL_RGBA8_SNORM:
            internal_components = 4;
            base_format = GL_RGBA;
            data_type = is_snorm ? GL_BYTE : GL_UNSIGNED_BYTE;
            is_unorm = !is_snorm;
            break;
        // ... 其他RGBA格式类似
        
        default:
            // 尝试通过get_internal_format_size推断组件数
            int total_size = get_internal_format_size(internalformat);
            // 常见格式：R=1, RG=2, RGB=3, RGBA=4
            // 这里简化处理，实际需要完整格式映射表
            return;
    }
    
    // 写入转换后的数据
    for (int i = 0; i < internal_components; i++) {
        float val = (i < count) ? components[i] : (i == 3 ? 1.0f : 0.0f);
        
        if (is_float) {
            if (data_type == GL_HALF_FLOAT) {
                GLhalf* half_out = (GLhalf*)output;
                half_out[i] = floatToHalf(val);
            } else {
                float* float_out = (float*)output;
                float_out[i] = val;
            }
        } else if (is_int || is_snorm) {
            if (data_type == GL_BYTE) {
                GLbyte* byte_out = (GLbyte*)output;
                byte_out[i] = (GLbyte)(val * (is_snorm ? 127.0f : 1.0f));
            } else if (data_type == GL_SHORT) {
                GLshort* short_out = (GLshort*)output;
                short_out[i] = (GLshort)(val * (is_snorm ? 32767.0f : 1.0f));
            } else if (data_type == GL_INT) {
                GLint* int_out = (GLint*)output;
                int_out[i] = (GLint)val;
            }
        } else if (is_uint || is_unorm) {
            if (data_type == GL_UNSIGNED_BYTE) {
                GLubyte* ubyte_out = (GLubyte*)output;
                ubyte_out[i] = (GLubyte)(val * (is_unorm ? 255.0f : 1.0f));
            } else if (data_type == GL_UNSIGNED_SHORT) {
                GLushort* ushort_out = (GLushort*)output;
                ushort_out[i] = (GLushort)(val * (is_unorm ? 65535.0f : 1.0f));
            } else if (data_type == GL_UNSIGNED_INT) {
                GLuint* uint_out = (GLuint*)output;
                uint_out[i] = (GLuint)val;
            }
        }
    }
}

// Half-float转换辅助函数
static GLhalf floatToHalf(float f) {
    uint32_t u = *(uint32_t*)&f;
    uint32_t sign = (u >> 16) & 0x8000;
    uint32_t exponent = (u >> 23) & 0xFF;
    uint32_t mantissa = u & 0x7FFFFF;
    
    if (exponent > 127 + 15) {
        // 无穷大
        return sign | 0x7C00;
    }
    if (exponent <= 127 - 15) {
        // 非规格化数
        return sign | (mantissa >> (exponent == 0 ? 1 : (127 - 15 - exponent + 1)));
    }
    
    uint32_t half_exp = exponent - 127 + 15;
    uint32_t half_mantissa = mantissa >> 13;
    
    return sign | (half_exp << 10) | half_mantissa;
}

static float halfToFloat(GLhalf h) {
    uint32_t sign = (h >> 15) & 0x1;
    uint32_t exponent = (h >> 10) & 0x1F;
    uint32_t mantissa = h & 0x3FF;
    
    if (exponent == 0) {
        if (mantissa == 0) {
            // 0
            return sign ? -0.0f : 0.0f;
        } else {
            // 非规格化
            exponent = 127 - 15;
            while (!(mantissa & 0x400)) {
                mantissa <<= 1;
                exponent--;
            }
            mantissa &= 0x3FF;
        }
    } else if (exponent == 31) {
        // 无穷大或NaN
        exponent = 255;
    } else {
        exponent += 127 - 15;
    }
    
    uint32_t u = (sign << 31) | (exponent << 23) | (mantissa << 13);
    return *(float*)&u;
}

static std::vector<GLubyte> convert_clear_data(const void* data, GLenum format, 
                                                GLenum type, GLenum internalformat) {
    std::vector<GLubyte> result;
    
    // OpenGL ES 3.2支持的internalformat
    GLsizei internal_size = get_internal_format_size(internalformat);
    if (internal_size == 0) return result;
    
    result.resize(internal_size);
    
    // 解析输入数据
    GLsizei component_count = 0;
    GLenum base_format = format;
    
    // 处理特殊情况
    if (format == GL_DEPTH_STENCIL || internalformat == GL_DEPTH24_STENCIL8 || 
        internalformat == GL_DEPTH32F_STENCIL8) {
        // 处理深度模板格式
        return convert_depth_stencil_clear_data(data, format, type, internalformat);
    }
    
    // 获取组件数量
    switch (format) {
        case GL_RED:
        case GL_GREEN:
        case GL_BLUE:
        case GL_ALPHA:
        case GL_LUMINANCE:
        case GL_DEPTH_COMPONENT:
        case GL_STENCIL_INDEX:
            component_count = 1;
            break;
        case GL_RG:
        case GL_LUMINANCE_ALPHA:
            component_count = 2;
            break;
        case GL_RGB:
            component_count = 3;
            break;
        case GL_RGBA:
            component_count = 4;
            break;
        default:
            LOG_E("convert_clear_data: Unsupported format %s", glEnumToString(format));
            return result;
    }
    
    // 获取每个组件的字节数
    GLsizei component_size = 0;
    bool is_signed = false;
    bool is_float = false;
    bool is_normalized = false;
    
    switch (type) {
        case GL_UNSIGNED_BYTE:
            component_size = 1;
            is_normalized = true;
            break;
        case GL_BYTE:
            component_size = 1;
            is_signed = true;
            is_normalized = true;
            break;
        case GL_UNSIGNED_SHORT:
            component_size = 2;
            is_normalized = true;
            break;
        case GL_SHORT:
            component_size = 2;
            is_signed = true;
            is_normalized = true;
            break;
        case GL_UNSIGNED_INT:
            component_size = 4;
            is_normalized = true;
            break;
        case GL_INT:
            component_size = 4;
            is_signed = true;
            is_normalized = true;
            break;
        case GL_HALF_FLOAT:
            component_size = 2;
            is_float = true;
            break;
        case GL_FLOAT:
            component_size = 4;
            is_float = true;
            break;
        case GL_UNSIGNED_SHORT_5_6_5:
        case GL_UNSIGNED_SHORT_4_4_4_4:
        case GL_UNSIGNED_SHORT_5_5_5_1:
        case GL_UNSIGNED_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
        case GL_UNSIGNED_INT_5_9_9_9_REV:
            // 打包格式，特殊处理
            return convert_packed_clear_data(data, format, type, internalformat);
        default:
            LOG_E("convert_clear_data: Unsupported type %s", glEnumToString(type));
            return result;
    }
    
    // 提取组件值
    std::vector<float> components(4, 0.0f); // RGBA，默认0
    
    for (int i = 0; i < component_count; i++) {
        const char* src = (const char*)data + i * component_size;
        
        if (is_float) {
            if (component_size == 2) {
                // half float
                GLhalf half_val = *(GLhalf*)src;
                components[i] = halfToFloat(half_val);
            } else {
                // float
                components[i] = *(float*)src;
            }
        } else if (is_signed) {
            if (component_size == 1) {
                components[i] = *(GLbyte*)src;
            } else if (component_size == 2) {
                components[i] = *(GLshort*)src;
            } else {
                components[i] = *(GLint*)src;
            }
            if (is_normalized) {
                // 归一化整数转浮点
                if (component_size == 1) components[i] /= 127.0f;
                else if (component_size == 2) components[i] /= 32767.0f;
                else components[i] /= 2147483647.0f;
            }
        } else {
            if (component_size == 1) {
                components[i] = *(GLubyte*)src;
            } else if (component_size == 2) {
                components[i] = *(GLushort*)src;
            } else {
                components[i] = *(GLuint*)src;
            }
            if (is_normalized) {
                // 无符号归一化整数转浮点
                if (component_size == 1) components[i] /= 255.0f;
                else if (component_size == 2) components[i] /= 65535.0f;
                else components[i] /= 4294967295.0f;
            }
        }
    }
    
    // 根据internalformat转换到目标格式
    convert_components_to_internal_format(components.data(), component_count, 
                                         internalformat, result.data());
    
    return result;
}

// 创建默认清除值（全0）
static std::vector<GLubyte> create_default_clear_value(GLenum internalformat) {
    GLsizei size = get_internal_format_size(internalformat);
    return std::vector<GLubyte>(size, 0);
}

void glClearBufferSubData(GLenum target, GLenum internalformat, 
                          GLintptr offset, GLsizeiptr size,
                          GLenum format, GLenum type, 
                          const void* data) {
    LOG()
    LOG_D("glClearBufferSubData: target=%s, internalformat=%s, offset=%ld, size=%ld, format=%s, type=%s",
          glEnumToString(target), glEnumToString(internalformat), offset, size,
          glEnumToString(format), glEnumToString(type));

    // 1. 参数验证
    if (size <= 0 || offset < 0) {
        LOG_W("glClearBufferSubData: Invalid offset or size");
        return;
    }

    // 2. 获取当前绑定的缓冲区
    GLuint bound_buffer = find_bound_buffer(target);
    if (!bound_buffer) {
        LOG_W("glClearBufferSubData: No buffer bound to target %s", glEnumToString(target));
        return;
    }

    // 3. 获取缓冲区大小并验证范围
    GLuint real_buffer = find_real_buffer(bound_buffer);
    if (!real_buffer) {
        LOG_W("glClearBufferSubData: Invalid buffer %d", bound_buffer);
        return;
    }

    // 保存当前绑定的缓冲区状态
    GLenum binding_query = get_binding_query(target);
    GLint prev_buffer = 0;
    if (binding_query) {
        GLES.glGetIntegerv(binding_query, &prev_buffer);
    }

    // 临时绑定目标缓冲区
    GLES.glBindBuffer(target, real_buffer);

    // 检查缓冲区大小
    GLint buffer_size = 0;
    GLES.glGetBufferParameteriv(target, GL_BUFFER_SIZE, &buffer_size);
    if (offset + size > (GLintptr)buffer_size) {
        LOG_W("glClearBufferSubData: Range [%ld, %ld) exceeds buffer size %d", 
              offset, offset + size, buffer_size);
        GLES.glBindBuffer(target, prev_buffer);
        return;
    }

    // 4. 获取内部格式的像素大小（字节数）
    GLsizei internal_format_size = get_internal_format_size(internalformat);
    if (internal_format_size == 0) {
        LOG_E("glClearBufferSubData: Unsupported internalformat %s", glEnumToString(internalformat));
        GLES.glBindBuffer(target, prev_buffer);
        return;
    }

    // 5. 计算清除值的实际大小
    GLsizei clear_value_size = get_format_type_size(format, type);
    if (clear_value_size == 0) {
        LOG_E("glClearBufferSubData: Unsupported format/type combination: %s/%s",
              glEnumToString(format), glEnumToString(type));
        GLES.glBindBuffer(target, prev_buffer);
        return;
    }

    // 6. 处理清除数据
    std::vector<GLubyte> converted_clear_value;
    
    if (data != nullptr) {
        // 根据internalformat转换清除值
        converted_clear_value = convert_clear_data(data, format, type, internalformat);
        if (converted_clear_value.empty()) {
            LOG_E("glClearBufferSubData: Failed to convert clear data");
            GLES.glBindBuffer(target, prev_buffer);
            return;
        }
    } else {
        // 默认清除值为0，根据internalformat创建
        converted_clear_value = create_default_clear_value(internalformat);
    }

    // 7. 确保清除值大小与内部格式匹配
    if ((GLsizei)converted_clear_value.size() != internal_format_size) {
        LOG_W("glClearBufferSubData: Clear value size %zu doesn't match internal format size %d",
              converted_clear_value.size(), internal_format_size);
        // 调整大小
        converted_clear_value.resize(internal_format_size, 0);
    }

    // 8. 执行清除操作 - 使用重复数据填充
    GLsizeiptr remaining = size;
    GLintptr current_offset = offset;
    const GLsizeiptr chunk_size = 1024 * 1024; // 1MB chunks
    std::vector<GLubyte> clear_pattern;

    // 构建清除模式数据
    clear_pattern.reserve(chunk_size);
    while (clear_pattern.size() < (size_t)chunk_size) {
        clear_pattern.insert(clear_pattern.end(), 
                           converted_clear_value.begin(), 
                           converted_clear_value.end());
    }
    clear_pattern.resize(chunk_size);

    // 使用glBufferSubData重复写入清除数据
    while (remaining > 0) {
        GLsizeiptr write_size = std::min(remaining, chunk_size);
        
        // 调整写入大小使其为清除值大小的整数倍
        write_size = (write_size / internal_format_size) * internal_format_size;
        if (write_size == 0) break;

        GLES.glBufferSubData(target, current_offset, write_size, clear_pattern.data());
        
        remaining -= write_size;
        current_offset += write_size;
    }

    // 处理剩余部分（如果有）
    if (remaining > 0) {
        std::vector<GLubyte> last_chunk;
        last_chunk.reserve(remaining);
        while (last_chunk.size() < (size_t)remaining) {
            last_chunk.insert(last_chunk.end(), 
                            converted_clear_value.begin(), 
                            converted_clear_value.end());
        }
        last_chunk.resize(remaining);
        GLES.glBufferSubData(target, current_offset, remaining, last_chunk.data());
    }

    // 9. 恢复之前的缓冲区绑定
    GLES.glBindBuffer(target, prev_buffer);

    CHECK_GL_ERROR
    LOG_D("glClearBufferSubData: Successfully cleared range [%ld, %ld) of buffer %d", 
          offset, offset + size, bound_buffer);
}
