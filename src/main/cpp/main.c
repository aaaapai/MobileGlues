//
// Created by Swung 0x48 on 2024/10/7.
//

#include <string.h>
#include "includes.h"
#include "gl/gl.h"
#include "egl/egl.h"
#include "egl/loader.h"
#include "gles/loader.h"
#include "gl/envvars.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char* (*MesaConvertShader)(const char *src, unsigned int type, unsigned int glsl, unsigned int essl);
void load_libs();

void proc_init() {
    LOG_V("Initializing %s @ %s", RENDERERNAME, __FUNCTION__);
    clear_log();
    start_log();

    load_libs();
    init_target_egl();
    init_target_gles();

    const char *shaderconv_lib = "libshaderconv";
    const char *func_name = "MesaConvertShader";
    const char *glslconv_name[] = {shaderconv_lib, NULL};
    void* glslconv = open_lib(glslconv_name, shaderconv_lib);
    if (glslconv == NULL) {
        printf("%s not found\n", shaderconv_lib);
    }
    else {
        MesaConvertShader = dlsym(glslconv, func_name);
        if (MesaConvertShader) {
            printf("%s loaded\n", shaderconv_lib);
        } else {
            printf("failed to load %s\n", shaderconv_lib);
        }
    }

    g_initialized = 1;
}

#ifdef __cplusplus
}
#endif
