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
        case multidraw_mode_t::DeepSeekOne:
            namestr += "_deepseek_one";
            break;
        default:
            LOG_W("get_multidraw_func() cannot determine multidraw emulation mode!")
            return {};
    }

    return namestr;
}

void *glXGetProcAddress(const char *name) {
    LOG()
    std::string real_func_name = handle_multidraw_func_name(std::string(name));

    static const ankerl::unordered_dense::set<std::string> dsa_blacklist = {
        "glCreateTextures",
        "glTextureParameteri",
        "glTextureParameterIiv",
        "glTextureParameterIuiv",
        "glBindTextureUnit",
        "glBindSamplers",
        "glCopyTextureSubImage1D",
        "glCopyTextureSubImage2D",
        "glCopyTextureSubImage3D",
        "glTextureParameteriv",
        "glTextureSubImage2D",
        "glClearTexSubImage",
        "glGetTextureParameterIuiv",
        "glGetTextureParameterIiv",
        "glGetTextureParameterfv",
        "glGetTextureParameteriv",
        "glGetTextureLevelParameterfv",
        "glGetTextureLevelParameteriv",
        "glGetCompressedTextureImage",
        "glGetTextureImage",
        "glCompressedTextureSubImage1D",
        "glCompressedTextureSubImage2D",
        "glCompressedTextureSubImage3D",
        "glTextureSubImage3D",
        "glTextureStorage3DMultisample",
        "glTextureStorage2DMultisample",
        "glTextureStorage3D",
        "glTextureStorage2D",
        "glCreateFramebuffers",
        "glNamedFramebufferRenderbuffer",
        "glNamedFramebufferDrawBuffer",
        "glNamedFramebufferDrawBuffers",
        "glNamedFramebufferTexture",
        "glNamedFramebufferTextureLayer",
        "glNamedFramebufferReadBuffer",
        "glBlitNamedFramebuffer",
        "glNamedFramebufferParameteri",
        "glGetNamedFramebufferAttachmentParameteriv",
        "glGetNamedFramebufferParameteriv",
        "glCheckNamedFramebufferStatus",
        "glClearNamedFramebufferiv",
        "glClearNamedFramebufferuiv",
        "glNamedBufferSubData",
        "glCreateBuffers"
    };
    if (!global_settings.ext_dsa) {
       if (dsa_blacklist.count(real_func_name)) {
          LOG_D("Blocked DSA function: %s", real_func_name.c_str());
          return nullptr;  // 直接返回 nullptr，禁用该函数
       }
    }

#ifdef __APPLE__
    return dlsym((void*)(~(uintptr_t)0), real_func_name.c_str());
#else
    
    void* proc = nullptr;

    proc = dlsym(RTLD_DEFAULT, real_func_name.c_str());

    if (!proc) {
        LOG_W("Failed to get OpenGL function: %s", real_func_name.c_str())
        return nullptr;
    }

    return proc;
#endif
}

void *glXGetProcAddressARB(const char *name) {
    return glXGetProcAddress(name);
}
