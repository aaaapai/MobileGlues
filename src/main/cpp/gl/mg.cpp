//
// Created by BZLZHH on 2025/1/27.
//

#include <unistd.h>
#include <fstream>
#include <format>
#include "mg.h"

#define DEBUG 0

hardware_t hardware;
gl_state_t gl_state;

FUNC_GL_STATE_SIZEI(proxy_width)
FUNC_GL_STATE_SIZEI(proxy_height)
FUNC_GL_STATE_ENUM(proxy_intformat)

#ifndef __APPLE__
std::ofstream file;
#endif

void start_log() {
#ifndef __APPLE__
    file.open(log_file_path, std::ios::app);
#endif
}

void write_log(std::string_view format, auto&&... args) {
#ifndef __APPLE__
    if (!file.is_open()) return;
    
    file << std::vformat(format, std::make_format_args(args...)) << "\n";
    file.flush();
    
#if FORCE_SYNC_WITH_LOG_FILE == 1
    sync();
#endif
#endif
}

void write_log_n(std::string_view format, auto&&... args) {
#ifndef __APPLE__
    if (!file.is_open()) return;
    file << std::vformat(format, std::make_format_args(args...));
    file.flush();
#endif
}

void clear_log() {
#ifndef __APPLE__
    file.open(log_file_path, std::ios::trunc);
    if (file.is_open()) {
        file.close();
    }
#endif
}

GLenum pname_convert(GLenum pname) {
    switch (pname) {
        case GL_TEXTURE_LOD_BIAS:
            return GL_TEXTURE_LOD_BIAS_QCOM;
        default:
            return pname;
    }
}

GLenum map_tex_target(GLenum target) {
    switch (target) {
        case GL_TEXTURE_1D:
        case GL_TEXTURE_3D:
        case GL_TEXTURE_RECTANGLE_ARB:
            return GL_TEXTURE_2D;

        case GL_PROXY_TEXTURE_1D:
        case GL_PROXY_TEXTURE_3D:
        case GL_PROXY_TEXTURE_RECTANGLE_ARB:
            return GL_PROXY_TEXTURE_2D;

        default:
            return target;
    }
}
