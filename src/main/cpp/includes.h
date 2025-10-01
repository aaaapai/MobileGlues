//
// Created by Swung 0x48 on 2024/10/7.
//

#ifndef MOBILEGLUES_INCLUDES_H
#define MOBILEGLUES_INCLUDES_H

#define RENDERERNAME "MobileGlues"
#ifndef __APPLE__
#include <android/log.h>
#endif
#include <dlfcn.h>

#include <EGL/egl.h>
#include <GLES3/gl32.h>

#include "egl/egl.h"
#include "egl/loader.h"

#if PROFILING
#include <perfetto.h>
PERFETTO_DEFINE_CATEGORIES(
        perfetto::Category("glcalls")
                .SetDescription("Calls from OpenGL"),
        perfetto::Category("internal")
                .SetDescription("Internal calls"));
#endif

#ifdef __cplusplus
extern "C" {
#endif

static int g_initialized = 0;

void proc_init();

#ifdef __cplusplus
}
#endif

#if UseFastSTL
#include <FastSTL/UnorderedMap.h>

template <
        typename Key,
        typename T,
        class Hash = std::hash<Key>,
        class KeyEqual = std::equal_to<Key>,
        class Allocator = std::allocator<std::pair<const Key, T>>
>
using UnorderedMap = FastSTL::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

#elif UseAnkerl
#include <ankerl/unordered_dense.h>

template <
    typename Key,
    typename T,
    class Hash = ankerl::unordered_dense::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<std::pair<Key, T>>
>
using UnorderedMap = ankerl::unordered_dense::map<Key, T, Hash, KeyEqual, Allocator>;

#elif UseStandard
#include <unordered_map>
template <
        typename Key,
        typename T
>
using UnorderedMap = std::unordered_map<Key, T>;
#else
#error The type of UnorderedMap to be used is not defined!
#endif

#endif //MOBILEGLUES_INCLUDES_H
