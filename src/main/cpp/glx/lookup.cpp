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
#include <ankerl/unordered_dense.h>

#define DEBUG 0

std::string handle_multidraw_func_name(std::string name) {
    std::string namestr = name;
    if (namestr != "glMultiDrawElementsBaseVertex" && namestr != "glMultiDrawElements" && namestr != "glMultiDrawElementsIndirect") {
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
        case multidraw_mode_t::DeepSeekOne:
            namestr += "_deepseek_one";
            break;
        case multidraw_mode_t::DeepSeekTwo:
            namestr += "_deepseek_two";
            break;
        case multidraw_mode_t::Native:
            namestr += "_native";
            break;
        default:
            LOG_W("get_multidraw_func() cannot determine multidraw emulation mode!")
            return {};
    }

    return namestr;
}


void* get_self_handle() {
    static void* handle = NULL;
    if (!handle) {
        Dl_info info;
        dladdr((void*)&get_self_handle, &info);
        handle = dlopen(info.dli_fname, RTLD_LAZY | RTLD_NOLOAD);
    }
    return handle;
}

__GLXextFuncPtr glXGetProcAddress(const GLubyte * name) {
    LOG()
    std::string real_func_name = handle_multidraw_func_name(std::string((const char*)name));
#ifdef __APPLE__
    return dlsym((void*)(~(uintptr_t)0), real_func_name.c_str());
#else
    
    void* self = get_self_handle();
    void* proc = nullptr;

    proc = dlsym(self, real_func_name.c_str());

    if (!proc) {
        LOG_W("Failed to get OpenGL function: %s", real_func_name.c_str())
        return nullptr;
    }

    return reinterpret_cast<__GLXextFuncPtr>(proc);
#endif
}

__GLXextFuncPtr glXGetProcAddressARB(const GLubyte * name) {
    return glXGetProcAddress(name);
}
