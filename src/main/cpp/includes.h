//
// Created by Swung 0x48 on 2024/10/7.
//

#ifndef MOBILEGLUES_INCLUDES_H
#define MOBILEGLUES_INCLUDES_H

#define RENDERERNAME "MobileGlues"
#include <android/log.h>
#include <dlfcn.h>

#include <EGL/egl.h>
#include <GLES3/gl3.h>

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
    
// include gl4es header files
    
#include "gl/gl4es/config.h"

// egl
#include "gl/gl4es/egl/egl.h"

// gl
#include "gl/gl4es/gl/arbconverter.h"
#include "gl/gl4es/gl/arbgenerator.h"
#include "gl/gl4es/gl/arbhelper.h"
#include "gl/gl4es/gl/arbparser.h"
#include "gl/gl4es/gl/array.h"
#include "gl/gl4es/gl/attributes.h"
#include "gl/gl4es/gl/blend.h"
#include "gl/gl4es/gl/blit.h"
#include "gl/gl4es/gl/buffers.h"
#include "gl/gl4es/gl/build_info.h"
#include "gl/gl4es/gl/const.h"
#include "gl/gl4es/gl/debug.h"
#include "gl/gl4es/gl/decompress.h"
#include "gl/gl4es/gl/depth.h"
#include "gl/gl4es/gl/directstate.h"
#include "gl/gl4es/gl/enum_info.h"
#include "gl/gl4es/gl/envvars.h"
#include "gl/gl4es/gl/eval.h"
#include "gl/gl4es/gl/face.h"
#include "gl/gl4es/gl/fog.h"
#include "gl/gl4es/gl/fpe.h"
#include "gl/gl4es/gl/fpe_cache.h"
#include "gl/gl4es/gl/fpe_shader.h"
#include "gl/gl4es/gl/framebuffers.h"
#include "gl/gl4es/gl/gl4es.h"
#include "gl/gl4es/gl/glcase.h"
#include "gl/gl4es/gl/gles.h"
#include "gl/gl4es/gl/glstate.h"
#include "gl/gl4es/gl/gl_lookup.h"
#include "gl/gl4es/gl/hint.h"
#include "gl/gl4es/gl/init.h"
#include "gl/gl4es/gl/light.h"
#include "gl/gl4es/gl/line.h"
#include "gl/gl4es/gl/list.h"
#include "gl/gl4es/gl/loader.h"
#include "gl/gl4es/gl/logs.h"
#include "gl/gl4es/gl/matrix.h"
#include "gl/gl4es/gl/matvec.h"
#include "gl/gl4es/gl/oldprogram.h"
#include "gl/gl4es/gl/pixel.h"
#include "gl/gl4es/gl/planes.h"
#include "gl/gl4es/gl/pointsprite.h"
#include "gl/gl4es/gl/preproc.h"
#include "gl/gl4es/gl/program.h"
#include "gl/gl4es/gl/queries.h"
#include "gl/gl4es/gl/raster.h"
#include "gl/gl4es/gl/render.h"
#include "gl/gl4es/gl/samplers.h"
#include "gl/gl4es/gl/shader.h"
#include "gl/gl4es/gl/shaderconv.h"
#include "gl/gl4es/gl/shader_hacks.h"
#include "gl/gl4es/gl/stack.h"
#include "gl/gl4es/gl/state.h"
#include "gl/gl4es/gl/stencil.h"
#include "gl/gl4es/gl/string_utils.h"
#include "gl/gl4es/gl/texenv.h"
#include "gl/gl4es/gl/texgen.h"
#include "gl/gl4es/gl/texture.h"
#include "gl/gl4es/gl/uniform.h"
#include "gl/gl4es/gl/vertexattrib.h"

// math
#include "gl/gl4es/gl/math/eval.h"

// wrap
#include "gl/gl4es/gl/wrap/gl4es.h"
#include "gl/gl4es/gl/wrap/gles.h"
#include "gl/gl4es/gl/wrap/skips.h"
#include "gl/gl4es/gl/wrap/stub.h"

// glx
#include "gl/gl4es/glx/drmfunc.h"
#include "gl/gl4es/glx/gbmfunc.h"
#include "gl/gl4es/glx/glx.h"
#include "gl/gl4es/glx/glx_gbm.h"
#include "gl/gl4es/glx/hardext.h"
#include "gl/gl4es/glx/rpi.h"
#include "gl/gl4es/glx/streaming.h"
#include "gl/gl4es/glx/utils.h"


static int g_initialized = 0;

void proc_init();

#ifdef __cplusplus
}
#endif

#endif //MOBILEGLUES_INCLUDES_H
