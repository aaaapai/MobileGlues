//
// Created by BZLZHH on 2025/1/27.
//

#include "lookup.h"

#include <cstdio>
#include <dlfcn.h>
#include <EGL/egl.h>
#include <cstring>
#include "../includes.h"
#include "../gl/log.h"
#include "../gl/envvars.h"
#include "../config/settings.h"

#define DEBUG 0

const char* handle_multidraw_func_name(const char* name) {
    std::string namestr = name;
    if (namestr != "glMultiDrawElementsBaseVertex" && namestr != "glMultiDrawElements") {
        return name;
    } else {
        namestr = "mg_" + namestr;
    }

    switch (global_settings.multidraw_mode) {
        case multidraw_mode_t::PreferIndirect:
            namestr += "_indirect";
            break;
        case multidraw_mode_t::PreferBaseVertex:
            namestr += "_basevertex";
            break;
        case multidraw_mode_t::PreferMultidrawIndirect:
            namestr += "_multiindirect";
            break;
        case multidraw_mode_t::DrawElements:
            namestr += "_drawelements";
            break;
        case multidraw_mode_t::Compute:
            namestr += "_compute";
            break;
        default:
            LOG_W("get_multidraw_func() cannot determine multidraw emulation mode!")
            return nullptr;
    }

    return namestr.c_str();
}

void *glXGetProcAddress(const char *name) {
    LOG()
    name = handle_multidraw_func_name(name);
#ifdef __APPLE__
    return dlsym((void*)(~(uintptr_t)0), name);
#else
    
    void* proc = nullptr;

    proc = dlsym(RTLD_DEFAULT, (const char*)name);

    if (!proc) {
        fprintf(stderr, "Failed to get OpenGL function %s: %s\n", name, dlerror());
        LOG_W("Failed to get OpenGL function: %s", (const char*)name)
        return nullptr;
    }

    return proc;
#endif
}

void *glXGetProcAddressARB(const char *name) {
    return glXGetProcAddress(name);
}