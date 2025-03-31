//
// Created by hanji on 2025/3/29.
//

#ifndef MOBILEGLUES_PLUGIN_POINTER_UTILS_H
#define MOBILEGLUES_PLUGIN_POINTER_UTILS_H

#include "../gl.h"
#include "../log.h"

#define DEBUG 0

class PointerUtils {
public:
    static int type_to_bytes(GLenum type);
    static int pname_to_count(GLenum pname);
};

#endif //MOBILEGLUES_PLUGIN_POINTER_UTILS_H
