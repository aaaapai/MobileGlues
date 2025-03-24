//
// Created by Swung 0x48 on 2024/10/7.
//

#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "includes.h"
#include "gl/glcorearb.h"
#include "egl/egl.h"
#include "egl/loader.h"
#include "gles/loader.h"
#include "gl/envvars.h"
#include "gl/log.h"
#include "config/settings.h"
#include "gl/fpe/fpe.hpp"

#define DEBUG 0

__attribute__((used)) const char* license = "GNU LGPL-2.1 License";

extern char* (*MesaConvertShader)(const char *src, unsigned int type, unsigned int glsl, unsigned int essl);
void init_libshaderconv() {
    const char *shaderconv_lib = "libshaderconv";
    const char *func_name = "MesaConvertShader";
    const char *glslconv_name[] = {shaderconv_lib, NULL};
    void* glslconv = open_lib(glslconv_name, shaderconv_lib);
    if (glslconv == NULL) {
        LOG_D("%s not found\n", shaderconv_lib);
    }
    else {
        MesaConvertShader = (char * (*)(const char *,unsigned int,unsigned int,unsigned int))dlsym(glslconv, func_name);
        if (MesaConvertShader) {
            LOG_D("%s loaded\n", shaderconv_lib);
        } else {
            LOG_D("failed to load %s\n", shaderconv_lib);
        }
    }
}

void init_config() {
    if(mkdir(MG_DIRECTORY_PATH, 0755) != 0 && errno != EEXIST) {
        LOG_E("Error creating MG directory.\n")
        return;
    }
    config_refresh();
}

void show_license() {
    LOG_V("The Open Source License of MobileGlues: ");
    LOG_V("  %s", license);
}

GLuint watermark_vao = 0;
GLuint watermark_vbo = 0;
GLuint watermark_ibo = 0;

float watermark_vertices[] = {
        -0.5f, -0.5f, 1.0f,
        0.5f, -0.5f, 1.0f,
        0.0f,  0.5f, 1.0f
};

const char* watermark_vtx_shader_src =
        "#version 320 es\n"
        "precision highp float;\n"
        "precision highp int;\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main() {\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\n";

const char* watermark_frag_shader_src =
        "#version 320 es\n"
        "precision highp float;\n"
        "precision highp int;\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}";

GLuint watermark_vtx_shader = 0;
GLuint watermark_frag_shader = 0;
GLuint watermark_program = 0;

int watermark_inited = 0;

char compile_info[1024];

int init_watermark_res() {
    LOAD_GLES_FUNC(glGenVertexArrays)
    LOAD_GLES_FUNC(glBindVertexArray)
    LOAD_GLES_FUNC(glGenBuffers)
    LOAD_GLES_FUNC(glBindBuffer)
    LOAD_GLES_FUNC(glBufferData)
    LOAD_GLES_FUNC(glCreateShader)
    LOAD_GLES_FUNC(glShaderSource)
    LOAD_GLES_FUNC(glCompileShader)
    LOAD_GLES_FUNC(glCreateProgram)
    LOAD_GLES_FUNC(glAttachShader)
    LOAD_GLES_FUNC(glLinkProgram)
    LOAD_GLES_FUNC(glGetShaderiv)
    LOAD_GLES_FUNC(glGetShaderInfoLog)
    LOAD_GLES_FUNC(glGetProgramiv)
    LOAD_GLES_FUNC(glVertexAttribPointer)
    LOAD_GLES_FUNC(glEnableVertexAttribArray)

    watermark_vtx_shader = gles_glCreateShader(GL_VERTEX_SHADER);
    gles_glShaderSource(watermark_vtx_shader, 1, &watermark_vtx_shader_src, NULL);
    gles_glCompileShader(watermark_vtx_shader);
    int success = 0;
    gles_glGetShaderiv(watermark_vtx_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        gles_glGetShaderInfoLog(watermark_vtx_shader, 1024, NULL, compile_info);
        LOG_E("Watermark vertex shader compile error: %s", compile_info);
        return -1;
    }

    watermark_frag_shader = gles_glCreateShader(GL_FRAGMENT_SHADER);
    gles_glShaderSource(watermark_frag_shader, 1, &watermark_frag_shader_src, NULL);
    gles_glCompileShader(watermark_frag_shader);

    gles_glGetShaderiv(watermark_frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        gles_glGetShaderInfoLog(watermark_frag_shader, 1024, NULL, compile_info);
        LOG_E("Watermark fragment shader compile error: %s", compile_info);
        return -1;
    }

    watermark_program = gles_glCreateProgram();
    gles_glAttachShader(watermark_program, watermark_vtx_shader);
    gles_glAttachShader(watermark_program, watermark_frag_shader);
    gles_glLinkProgram(watermark_program);
    gles_glGetProgramiv(watermark_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(watermark_program, 1024, NULL, compile_info);
        LOG_E("Watermark program link error: %s", compile_info);
        return -1;
    }


    gles_glGenVertexArrays(1, &watermark_vao);

    gles_glGenBuffers(1, &watermark_vbo);
    gles_glGenBuffers(1, &watermark_ibo);

    gles_glBindVertexArray(watermark_vao);

    gles_glBindBuffer(GL_ARRAY_BUFFER, watermark_vbo);
    gles_glBufferData(GL_ARRAY_BUFFER, sizeof(watermark_vertices), watermark_vertices, GL_STATIC_DRAW);

    gles_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    gles_glEnableVertexAttribArray(0);

    gles_glBindBuffer(GL_ARRAY_BUFFER, 0);
    gles_glBindVertexArray(0);


    INIT_CHECK_GL_ERROR

    watermark_inited = 1;

    return 0;
}

void draw_watermark() {
    LOG()

    LOG_D("Drawing watermark!")
    LOAD_GLES_FUNC(glGetIntegerv)
    LOAD_GLES_FUNC(glBindVertexArray)
    LOAD_GLES_FUNC(glBindBuffer)
    LOAD_GLES_FUNC(glUseProgram)
    LOAD_GLES_FUNC(glDrawArrays)
    LOAD_GLES_FUNC(glEnableVertexAttribArray)

    INIT_CHECK_GL_ERROR

    // Save states
    GLint prev_vao;
    GLint prev_vbo;
    GLint prev_prog;
    gles_glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prev_vao);
    CHECK_GL_ERROR_NO_INIT
    gles_glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev_vbo);
    CHECK_GL_ERROR_NO_INIT
    gles_glGetIntegerv(GL_CURRENT_PROGRAM, &prev_prog);
    CHECK_GL_ERROR_NO_INIT

    LOG_D("prev vao: %d", prev_vao);
    LOG_D("prev vbo: %d", prev_vbo);
    LOG_D("prev program: %d", prev_prog);

    LOG_D("Save ok")

    // Set states
    gles_glBindVertexArray(watermark_vao);
    CHECK_GL_ERROR_NO_INIT
    gles_glBindBuffer(GL_ARRAY_BUFFER, watermark_vbo);
    CHECK_GL_ERROR_NO_INIT
    gles_glUseProgram(watermark_program);
    CHECK_GL_ERROR_NO_INIT
    LOG_D("set ok")

    // Draw!
    gles_glDrawArrays(GL_TRIANGLES, 0, 3);
    CHECK_GL_ERROR_NO_INIT
    LOG_D("draw ok")

    // Restore states
    gles_glBindVertexArray(prev_vao);
    CHECK_GL_ERROR_NO_INIT
    gles_glBindBuffer(GL_ARRAY_BUFFER, prev_vbo);
    CHECK_GL_ERROR_NO_INIT
    gles_glUseProgram(prev_prog);
    CHECK_GL_ERROR_NO_INIT
    LOG_D("restore ok")
}

#if PROFILING

PERFETTO_TRACK_EVENT_STATIC_STORAGE();

void init_perfetto() {
    perfetto::TracingInitArgs args;

    args.backends |= perfetto::kSystemBackend;

    perfetto::Tracing::Initialize(args);
    perfetto::TrackEvent::Register();
}
#endif

void proc_init() {
    init_config();

    clear_log();
    start_log();

    init_settings();

    LOG_V("Initializing %s ...", RENDERERNAME);
    show_license();

    load_libs();
    init_target_egl();
    init_target_gles();

    init_libshaderconv();

    //init_watermark_res();
#if PROFILING
    init_perfetto();
#endif

    // Cleanup
    destroy_temp_egl_ctx();
    g_initialized = 1;
}
