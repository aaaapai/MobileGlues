//
// Created by Swung 0x48 on 2025-03-26.
//

#include "buffer.h"

#include "../buffer.h"
#include "../../config/settings.h"
#include "ankerl/unordered_dense.h"

#define DEBUG 1

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

void glNamedBufferData(GLuint buffer, GLsizeiptr size, const void *data, GLenum usage) {

    LOG()

    if (!global_settings.ext_dsa) {
        return;
    }

    // Save the currently bound buffer to restore later
    GLint prev_buffer;
    GLenum prev_target;
    GLenum binding_point = GL_ARRAY_BUFFER; // Default fallback target
    
    // Map desktop OpenGL targets to GLES 3.2 supported targets
    // These are the supported targets in GLES 3.2
    GLenum supported_targets[] = {
        GL_ARRAY_BUFFER,
        GL_ELEMENT_ARRAY_BUFFER,
        GL_COPY_READ_BUFFER,
        GL_COPY_WRITE_BUFFER,
        GL_PIXEL_PACK_BUFFER,
        GL_PIXEL_UNPACK_BUFFER,
        GL_TRANSFORM_FEEDBACK_BUFFER,
        GL_UNIFORM_BUFFER
    };
    
    // For unsupported targets, we'll use ARRAY_BUFFER as fallback with warning
    int target_supported = 0;
    for (unsigned int i = 0; i < sizeof(supported_targets)/sizeof(GLenum); i++) {
        if (supported_targets[i] == binding_point) {
            target_supported = 1;
            break;
        }
    }
    
    if (!target_supported) {
        // For truly unsupported targets that can't be reasonably mapped
        fprintf(stderr, "Warning: Target not directly supported in GLES 3.2. Using GL_ARRAY_BUFFER as fallback.\n");
        binding_point = GL_ARRAY_BUFFER;
    }
    
    // Get the current binding for our target to restore later
    switch (binding_point) {
        case GL_ARRAY_BUFFER:
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev_buffer);
            prev_target = GL_ARRAY_BUFFER;
            break;
        case GL_ELEMENT_ARRAY_BUFFER:
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &prev_buffer);
            prev_target = GL_ELEMENT_ARRAY_BUFFER;
            break;
        case GL_COPY_READ_BUFFER:
            glGetIntegerv(GL_COPY_READ_BUFFER_BINDING, &prev_buffer);
            prev_target = GL_COPY_READ_BUFFER;
            break;
        case GL_COPY_WRITE_BUFFER:
            glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING, &prev_buffer);
            prev_target = GL_COPY_WRITE_BUFFER;
            break;
        case GL_PIXEL_PACK_BUFFER:
            glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &prev_buffer);
            prev_target = GL_PIXEL_PACK_BUFFER;
            break;
        case GL_PIXEL_UNPACK_BUFFER:
            glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &prev_buffer);
            prev_target = GL_PIXEL_UNPACK_BUFFER;
            break;
        case GL_TRANSFORM_FEEDBACK_BUFFER:
            glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, &prev_buffer);
            prev_target = GL_TRANSFORM_FEEDBACK_BUFFER;
            break;
        case GL_UNIFORM_BUFFER:
            glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &prev_buffer);
            prev_target = GL_UNIFORM_BUFFER;
            break;
        default:
            // Shouldn't get here due to our fallback logic
            prev_buffer = 0;
            prev_target = GL_ARRAY_BUFFER;
            break;
    }
    
    // Bind our target buffer to the appropriate target
    glBindBuffer(binding_point, buffer);
    
    // Create and initialize the buffer's data store
    glBufferData(binding_point, size, data, usage);
    
    // Restore the previously bound buffer
    glBindBuffer(prev_target, prev_buffer);
}

void glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data) {
    LOG()
    LOG_D("glNamedBufferSubData, buffer = %d, offset = %d, size = %d, data = 0x%x",
          buffer, offset, size, data)

    if (!global_settings.ext_dsa) {
        return;
    }

    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glBufferSubData(GL_COPY_WRITE_BUFFER, offset, size, data);

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glCreateBuffers(GLsizei n, GLuint* buffers) {
    LOG()
    LOG_D("glCreateBuffers, n = %d", n)

    if (!global_settings.ext_dsa) {
        return;
    }

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

    if (!global_settings.ext_dsa) {
        return;
    }

    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glFlushMappedBufferRange(GL_COPY_WRITE_BUFFER, offset, length);
    CHECK_GL_ERROR_NO_INIT

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glGetNamedBufferParameteriv(GLuint buffer, GLenum pname, GLint* params) {
    LOG()
    LOG_D("glGetNamedBufferParameteriv, buffer = %d, pname = %s", buffer, glEnumToString(pname))

    if (!global_settings.ext_dsa) {
        return;
    }

    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glGetBufferParameteriv(GL_COPY_WRITE_BUFFER, pname, params);
    CHECK_GL_ERROR_NO_INIT

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glGetNamedBufferParameteri64v(GLuint buffer, GLenum pname, GLint64* params) {
    LOG()
    LOG_D("glGetNamedBufferParameteri64v, buffer = %d, pname = %s", buffer, glEnumToString(pname))

    if (!global_settings.ext_dsa) {
        return;
    }

    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glGetBufferParameteri64v(GL_COPY_WRITE_BUFFER, pname, params);
    CHECK_GL_ERROR_NO_INIT

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glGetNamedBufferPointerv(GLuint buffer, GLenum pname, void* *params) {
    LOG()
    LOG_D("glGetNamedBufferPointerv, buffer = %d, pname = %s", buffer, glEnumToString(pname))

    if (!global_settings.ext_dsa) {
        return;
    }

    INIT_CHECK_GL_ERROR

    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    GLES.glGetBufferPointerv(GL_COPY_WRITE_BUFFER, pname, params);
    CHECK_GL_ERROR_NO_INIT

    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glGetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data) {
    LOG()
    LOG_D("glGetNamedBufferSubData, buffer: %u, offset: %lld, size: %lld, data: %p", 
          buffer, (long long)offset, (long long)size, data)

    if (!global_settings.ext_dsa) {
        return;
    }


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

void* glMapNamedBuffer(GLuint buffer, GLenum access)
{

    LOG()
    LOG_D("glMapNamedBuffer, buffer: %u, access: 0x%X", buffer, access);


    if (!global_settings.ext_dsa) {
        return;
    }

    // 获取缓冲区大小
    GLint size = 0;
    LOG_D("glMapNamedBuffer调用已实现的glGetNamedBufferParameteriv函数")
    glGetNamedBufferParameteriv(buffer, GL_BUFFER_SIZE, &size);
    
    // 根据访问模式转换为GLES3可用的标志
    GLbitfield flags = 0;
    switch(access) {
        case GL_READ_ONLY:
            flags = GL_MAP_READ_BIT;
            break;
        case GL_WRITE_ONLY:
            flags = GL_MAP_WRITE_BIT;
            break;
        case GL_READ_WRITE:
            flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
            break;
        default:
            LOG_E("ERROR: glMapNamedBuffer - Invalid access mode (access: 0x%X)", access)
            return nullptr;
    }
    
    LOG_D("glMapNamedBuffer调用已实现的glMapNamedBufferRange函数")
    return glMapNamedBufferRange(buffer, 0, (GLsizeiptr)size, flags);
}

void* glMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access) {
    LOG()
    LOG_D("glMapNamedBufferRange, buffer = %d, offset = %d, length = %d, access = 0x%x",
          buffer, offset, length, access)

    if (!global_settings.ext_dsa) {
        return;
    }

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

    if (!global_settings.ext_dsa) {
        return;
    }


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
        for (size_t i = 0; i < static_cast<size_t>(size); i += elem_size) {
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
    LOG_D("glClearNamedBufferData(buffer=%u, internalformat=%s, format=%s, type=%s, data=%p)",
          buffer, glEnumToString(internalformat), 
          glEnumToString(format), glEnumToString(type), data)

    if (!global_settings.ext_dsa) {
        return;
    }

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

inline GLenum GetBufferBindingTarget(GLenum target) {
    return target == GL_ELEMENT_ARRAY_BUFFER ? GL_ELEMENT_ARRAY_BUFFER_BINDING
         : target == GL_UNIFORM_BUFFER ? GL_UNIFORM_BUFFER_BINDING
         : GL_ARRAY_BUFFER_BINDING;
}
void glClearBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data) {

    LOG()
    LOG_D("glClearBufferSubData, target = 0x%x, offset = %d, size = %d, data = %p", target, offset, size, data)

    if (!global_settings.ext_dsa) {
        return;
    }

    GLint prevBuf;
    glGetIntegerv(GetBufferBindingTarget(target), &prevBuf);
    
    void* ptr = glMapBufferRange(target, offset, size, 
                               GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
    if(ptr) {
        data ? memcpy(ptr, data, size) : memset(ptr, 0, size);
        glUnmapBuffer(target);
    }
    
    glBindBuffer(target, prevBuf);
}

void glClearNamedBufferSubData(GLuint buffer, GLenum internalformat, 
                             GLintptr offset, GLsizeiptr size,
                             GLenum format, GLenum type, const void *data) {
    LOG()
    LOG_D("glClearNamedBufferSubData(buffer=%u, internalformat=%s, offset=%ld, size=%zd, format=%s, type=%s, data=%p)",
          buffer, glEnumToString(internalformat), 
          offset, size, glEnumToString(format), glEnumToString(type), data)


    if (!global_settings.ext_dsa) {
        return;
    }

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

typedef struct {
    GLuint buffer;
    GLbitfield flags;
} BufferMappingInfo;
static BufferMappingInfo* mappingInfos = NULL;
static int mappingInfoCount = 0;
static void registerBufferMappingSupport(GLuint buffer, GLbitfield flags) {
    // 检查是否已注册
    for (int i = 0; i < mappingInfoCount; i++) {
        if (mappingInfos[i].buffer == buffer) {
            mappingInfos[i].flags = flags;
            return;
        }
    }
    
    // 新增注册项
    BufferMappingInfo* newInfos = (BufferMappingInfo*)realloc(mappingInfos, 
                                  (mappingInfoCount + 1) * sizeof(BufferMappingInfo));
    if (!newInfos) {
        LOG_D("Failed to allocate buffer mapping info");
        return;
    }
    
    mappingInfos = newInfos;
    mappingInfos[mappingInfoCount].buffer = buffer;
    mappingInfos[mappingInfoCount].flags = flags;
    mappingInfoCount++;
}
void glNamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags) {
    LOG()
    LOG_D("glNamedBufferStorage: buffer = %u, size = %lld, data = %p, flags = 0x%X", buffer, (long long)size, data, flags)


    if (!global_settings.ext_dsa) {
        return;
    }

    INIT_CHECK_GL_ERROR

    // 保存当前绑定状态并绑定目标缓冲区
    SAVE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)

    // 转换标志为GLES3可用的标志
    GLbitfield glesFlags = 0;
    
    // 动态存储标志转换为使用模式
    GLenum usage = GL_STATIC_DRAW;
    if (flags & GL_DYNAMIC_STORAGE_BIT) {
        usage = GL_DYNAMIC_DRAW;
    }

    // 处理映射相关标志
    if (flags & (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT)) {
        // GLES3的glMapBufferRange需要GL_DYNAMIC_DRAW或GL_STREAM_DRAW
        usage = GL_DYNAMIC_DRAW;
        
        // 设置GLES映射标志
        if (flags & GL_MAP_READ_BIT) glesFlags |= GL_MAP_READ_BIT;
        if (flags & GL_MAP_WRITE_BIT) glesFlags |= GL_MAP_WRITE_BIT;
        if (flags & GL_MAP_PERSISTENT_BIT) {
            LOG_W("Warning: glNamedBufferStorage - GL_MAP_PERSISTENT_BIT not fully supported")
        }
        if (flags & GL_MAP_COHERENT_BIT) {
            LOG_W("Warning: glNamedBufferStorage - GL_MAP_COHERENT_BIT not supported")
        }
    }

    // 分配存储空间
    glBufferData(GL_COPY_WRITE_BUFFER, size, data, usage);
    CHECK_GL_ERROR_NO_INIT

    // 如果需要映射支持，记录缓冲区特性
    if (flags & (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT)) {
        // 这里可以维护一个缓冲区特性表
        // 记录哪些缓冲区支持映射及支持的访问模式
        registerBufferMappingSupport(buffer, flags);
    }

    // 恢复之前的绑定状态
    RESTORE_BUFFER_CTX(GL_COPY_WRITE_BUFFER)
}

void glCopyNamedBufferSubData(GLuint readBuffer, GLuint writeBuffer, 
                             GLintptr readOffset, GLintptr writeOffset, 
                             GLsizeiptr size) {

    LOG()
    LOG_D("glCopyNamedBufferSubData, readBuffer = %u, writeBuffer = %u, readOffset = %d, writeOffset = %d, size = %d", readBuffer, writeBuffer, readOffset, writeOffset, size)

    if (!global_settings.ext_dsa) {
        return;
    }

    // 保存当前绑定状态
    GLint prevReadBuf = 0, prevWriteBuf = 0;
    glGetIntegerv(GL_COPY_READ_BUFFER_BINDING, &prevReadBuf);
    glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING, &prevWriteBuf);
    
    // 绑定源缓冲区和目标缓冲区
    glBindBuffer(GL_COPY_READ_BUFFER, readBuffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, writeBuffer);
    
    // 执行数据复制
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 
                       readOffset, writeOffset, size);
    
    // 恢复之前的绑定状态
    glBindBuffer(GL_COPY_READ_BUFFER, prevReadBuf);
    glBindBuffer(GL_COPY_WRITE_BUFFER, prevWriteBuf);
}

GLboolean glUnmapNamedBuffer(GLuint buffer) {

    LOG()

    if (!global_settings.ext_dsa) {
        return;
    }

    // First, we need to check if the buffer exists and is mapped
    GLint isMapped = GL_FALSE;
    GLint currentlyBoundBuffer = 0;
    GLenum target = 0;
    
    // Get the current binding for each possible target to restore later
    GLES.glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentlyBoundBuffer);
    
    // Check if the buffer is mapped by temporarily binding it to ARRAY_BUFFER
    GLES.glBindBuffer(GL_ARRAY_BUFFER, buffer);
    GLES.glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_MAPPED, &isMapped);
    
    if (!isMapped) {
        // Buffer wasn't mapped, restore previous binding and return error
        GLES.glBindBuffer(GL_ARRAY_BUFFER, currentlyBoundBuffer);
        return GL_FALSE;
    }
    
    // For GLES, we need to find which target the buffer is bound to
    // Since GLES doesn't have direct state access, we'll try common targets
    
    // Try to find which target the buffer is bound to
    GLenum targets[] = {
        GL_ARRAY_BUFFER,
        GL_ELEMENT_ARRAY_BUFFER,
        GL_COPY_READ_BUFFER,
        GL_COPY_WRITE_BUFFER,
        GL_PIXEL_PACK_BUFFER,
        GL_PIXEL_UNPACK_BUFFER,
        GL_TRANSFORM_FEEDBACK_BUFFER,
        GL_UNIFORM_BUFFER,
        // GLES 3.1+ targets
        GL_ATOMIC_COUNTER_BUFFER,
        GL_DISPATCH_INDIRECT_BUFFER,
        GL_DRAW_INDIRECT_BUFFER,
        GL_SHADER_STORAGE_BUFFER
    };
    
    int foundTarget = 0;
    for (size_t i = 0; i < sizeof(targets)/sizeof(targets[0]); i++) {
        GLint boundBuffer = 0;
        GLES.glGetIntegerv(targets[i], &boundBuffer);
        if ((GLuint)boundBuffer == buffer) {
            target = targets[i];
            foundTarget = 1;
            break;
        }
    }
    
    if (!foundTarget) {
        // Couldn't find which target the buffer is bound to
        GLES.glBindBuffer(GL_ARRAY_BUFFER, currentlyBoundBuffer);
        return GL_FALSE;
    }
    
    // Now unmap the buffer using the found target
    GLboolean result = GLES.glUnmapBuffer(target);
    
    // Restore the original binding
    GLES.glBindBuffer(GL_ARRAY_BUFFER, currentlyBoundBuffer);
    
    return result;
}
