//
// Created by BZLZHH on 2025/1/27.
//

#include "lookup.h"

#include <stdio.h>
#include <dlfcn.h>
#include <EGL/egl.h>
#include <string.h>
#include "../includes.h"
#include "../gl/log.h"
#include "../gl/envvars.h"
#include "../config/settings.h"

#define DEBUG 0

void* get_multidraw_func(const char* name) {
    std::string namestr = name;
    if (namestr != "glMultiDrawElementsBaseVertex" && namestr != "glMultiDrawElements") {
        return nullptr;
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

    return dlsym(RTLD_DEFAULT, namestr.c_str());
}

void *glXGetProcAddress(const char *name) {
    LOG()
    void* proc = nullptr;

    proc = get_multidraw_func(name);

    if (!proc)
        proc = dlsym(RTLD_DEFAULT, (const char*)name);

    if (!proc) {
        fprintf(stderr, "Failed to get OpenGL function %s: %s\n", name, dlerror());
        LOG_W("Failed to get OpenGL function: %s", (const char*)name)
        return NULL;
    }

    return proc;
}

void *glXGetProcAddressARB(const char *name) {
    LOG()
    void* proc = nullptr;

    proc = get_multidraw_func(name);

    if (!proc)
        proc = dlsym(RTLD_DEFAULT, (const char*)name);

    LOG_D("glXGetProcAddressARB(\"%s\")", name)
    if (!proc) {
        fprintf(stderr, "Failed to get OpenGL function %s: %s\n", name, dlerror());
        LOG_W("Failed to get OpenGL function: %s", (const char*)name)
        return NULL;
    }

    return proc;
}