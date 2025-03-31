//
// Created by hanji on 2025/2/9.
//

#ifndef MOBILEGLUES_PLUGIN_SETTINGS_H
#define MOBILEGLUES_PLUGIN_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <__stddef_size_t.h>

enum class multidraw_mode_t: int {
    Auto = 0,
    PreferIndirect,
    PreferBaseVertex,
    PreferMultidrawIndirect
};

struct global_settings_t {
    int angle; // 0, 1
    int ignore_error; // 0, 1, 2
    int ext_gl43; // 0, 1
    int ext_compute_shader; // 0, 1
    size_t max_glsl_cache_size; // 0~
    int enable_compatible_mode; // 0, 1
    multidraw_mode_t multidraw_mode; // 0, 1, 2, 3
};

extern struct global_settings_t global_settings;

void init_settings();

void init_settings_post();

#ifdef __cplusplus
}
#endif

#endif //MOBILEGLUES_PLUGIN_SETTINGS_H
