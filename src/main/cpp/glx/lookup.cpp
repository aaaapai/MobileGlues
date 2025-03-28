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

#define DEBUG 0

void *glXGetProcAddress(const char *name) {
    LOG()
    void* proc = dlsym(RTLD_DEFAULT, (const char*)name);
    LOG_D("glXGetProcAddress(\"%s\")", name)

    if (!proc) {
        fprintf(stderr, "Failed to get OpenGL function %s: %s\n", name, dlerror());
        LOG_W("Failed to get OpenGL function: %s", (const char*)name);
        return NULL;
    }

    return proc;
}

void *glXGetProcAddressARB(const char *name) {
    LOG()
    void* proc = dlsym(RTLD_DEFAULT, (const char*)name);

    LOG_D("glXGetProcAddressARB(\"%s\")", name)
    if (!proc) {
        fprintf(stderr, "Failed to get OpenGL function %s: %s\n", name, dlerror());
        LOG_W("Failed to get OpenGL function: %s", (const char*)name);
        return NULL;
    }

    return proc;
}