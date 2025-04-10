//
// Created by Swung 0x48 on 2025/2/8.
//

#include "state.h"
#include <glm/glm/gtc/type_ptr.hpp>
#include "list.h"
#include "pointer_utils.h"

#define DEBUG 0

#if GLOBAL_DEBUG || DEBUG
#pragma clang optimize off
#endif

bool hijack_fpe_states(GLenum cap, bool enable, fixed_function_bool_t* bools) {
    switch (cap) {
        case GL_FOG:
            bools->fog_enable = enable;
            return true;
        case GL_LIGHTING:
            bools->lighting_enable = enable;
            return true;
        // TODO: implement these states
        case GL_COLOR_MATERIAL:
        case GL_ALPHA_TEST:
        case GL_LIGHT0:
        case GL_LIGHT1:
        case GL_LIGHT2:
        case GL_LIGHT3:
        case GL_LIGHT4:
        case GL_LIGHT5:
        case GL_LIGHT6:
        case GL_LIGHT7:
        case GL_TEXTURE_2D:
        case GL_RESCALE_NORMAL:
            return true;
        default:
            break;
    }
    return false;
}

void glEnable(GLenum cap) {
    LOG()
    LOG_D("glEnable, cap = %s", glEnumToString(cap));

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glEnable>({}, cap);
        if (DisplayListManager::shouldFinish())
            return;
    }

    if (hijack_fpe_states(cap, true, &g_glstate.fpe_state.fpe_bools))
        return;

    GLES.glEnable(cap);
    CHECK_GL_ERROR
}

void glDisable(GLenum cap) {
    LOG()
    LOG_D("glDisable, cap = %s", glEnumToString(cap))

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glDisable>({}, cap);
        if (DisplayListManager::shouldFinish())
            return;
    }

    if (hijack_fpe_states(cap, false, &g_glstate.fpe_state.fpe_bools))
        return;

    GLES.glDisable(cap);
    CHECK_GL_ERROR
}

void glClientActiveTexture(GLenum texture) {
    LOG()
    LOG_D("glClientActiveTexture(GL_TEXTURE%d)", texture - GL_TEXTURE0)

    // Todo: this function can be added to displayList when GL 1.3+ is disabled

    g_glstate.fpe_state.client_active_texture = texture;
}

void glAlphaFunc( GLenum func, GLclampf ref ) {
    LOG()
    LOG_D("glAlphaFunc(%s, %f)", glEnumToString(func), ref)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glAlphaFunc>({}, func, ref);
        if (DisplayListManager::shouldFinish())
            return;
    }

    g_glstate.fpe_state.alpha_func = func;
    g_glstate.fpe_uniform.alpha_ref = ref;
}

void glFogf( GLenum pname, GLfloat param ) {
    LOG()
    LOG_D("glFogf(%s, %f)", glEnumToString(pname), param)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glFogf>({}, pname, param);
        if (DisplayListManager::shouldFinish())
            return;
    }

    switch (pname) {
        case GL_FOG_DENSITY:
            g_glstate.fpe_uniform.fog_density = param;
            return;
        case GL_FOG_START:
            g_glstate.fpe_uniform.fog_start = param;
            return;
        case GL_FOG_END:
            g_glstate.fpe_uniform.fog_end = param;
            return;

        // below should not be handled here
        case GL_FOG_MODE:
        case GL_FOG_INDEX:
        case GL_FOG_COORD_SRC:
            SELF_CALL(glFogi, pname, (GLint)param)
            return;

        default:
            LOG_D("ERROR: Invalid %s pname: %s", __func__, pname)
    }
}

void glFogi( GLenum pname, GLint param ) {
    LOG()
    LOG_D("glFogi(%s, %s)", glEnumToString(pname), glEnumToString(param))

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glFogi>({}, pname, param);
        if (DisplayListManager::shouldFinish())
            return;
    }

    switch (pname) {
        case GL_FOG_MODE:
            g_glstate.fpe_state.fog_mode = param;
            break;
        case GL_FOG_INDEX:
            g_glstate.fpe_state.fog_index = param;
            break;
        case GL_FOG_COORD_SRC:
            g_glstate.fpe_state.fog_coord_src = param;
            break;

        // below should not be handled here
        case GL_FOG_DENSITY:
        case GL_FOG_START:
        case GL_FOG_END:
            SELF_CALL(glFogf, pname, (GLfloat)param)
            return;
        default:
            LOG_D("ERROR: Invalid %s pname: %s", __func__, pname)
    }
}

void glFogfv( GLenum pname, const GLfloat *params ) {
    LOG()
    LOG_D("glFogfv(%s, [...])", glEnumToString(pname))

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glFogfv>({{1, PointerUtils::pname_to_count(pname) * sizeof(GLfloat)}}, pname, params);
        if (DisplayListManager::shouldFinish())
            return;
    }

    switch (pname) {
        case GL_FOG_MODE:
        case GL_FOG_INDEX:
        case GL_FOG_COORD_SRC:
            SELF_CALL(glFogi, pname, (GLint)params[0])
            break;
        case GL_FOG_DENSITY:
        case GL_FOG_START:
        case GL_FOG_END:
            SELF_CALL(glFogf, pname, params[0])
            break;
        case GL_FOG_COLOR: {
            auto& fcolor = g_glstate.fpe_uniform.fog_color;
            fcolor = glm::make_vec4(params);
            LOG_D("[...] = [%.2f, %.2f, %.2f, %.2f]", params[0], params[1], params[2], params[3])
            LOG_D("fcolor = [%.2f, %.2f, %.2f, %.2f]", fcolor[0], fcolor[1], fcolor[2], fcolor[3])
            break;
        }
        default:
            LOG_D("ERROR: Invalid %s pname: %s", __func__, pname)
    }
}

void glFogiv( GLenum pname, const GLint *params ) {
    LOG()
    LOG_D("glFogiv(%s, [...])", glEnumToString(pname))

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glFogiv>({{1, PointerUtils::pname_to_count(pname) * sizeof(GLint)}}, pname, params);
        if (DisplayListManager::shouldFinish())
            return;
    }

    switch (pname) {
        case GL_FOG_COLOR: {
            auto& fcolor = g_glstate.fpe_uniform.fog_color;
            fcolor[0] = (GLfloat)params[0] / (GLfloat)INT32_MAX;
            fcolor[1] = (GLfloat)params[1] / (GLfloat)INT32_MAX;
            fcolor[2] = (GLfloat)params[2] / (GLfloat)INT32_MAX;
            fcolor[3] = (GLfloat)params[3] / (GLfloat)INT32_MAX;
            LOG_D("[...] = [%d, %d, %d, %d]", params[0], params[1], params[2], params[3])
            break;
        }
        case GL_FOG_MODE:
        case GL_FOG_INDEX:
        case GL_FOG_COORD_SRC:
            SELF_CALL(glFogi, pname, params[0])
            break;
        case GL_FOG_DENSITY:
        case GL_FOG_START:
        case GL_FOG_END:
            SELF_CALL(glFogf, pname, (GLfloat)params[0])
            break;
        default:
            LOG_D("ERROR: Invalid %s pname: %s", __func__, pname)
    }
}

void glShadeModel( GLenum mode ) {
    LOG()
    LOG_D("glShadeModel(%s)", glEnumToString(mode))

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glShadeModel>({}, mode);
        if (DisplayListManager::shouldFinish())
            return;
    }

    g_glstate.fpe_state.shade_model = mode;
}

void glLightf( GLenum light, GLenum pname, GLfloat param ) {
    LOG()
    LOG_D("glLightf(%s, %s, %f)", glEnumToString(light), glEnumToString(pname), param)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glLightf>({}, light, pname, param);
        if (DisplayListManager::shouldFinish())
            return;
    }

    auto& lightref = g_glstate.fpe_uniform.lights[light - GL_LIGHT0];

    switch (pname) {
        case GL_SPOT_EXPONENT:
            lightref.spot_exp = param;
            break;
        case GL_SPOT_CUTOFF:
            lightref.spot_cutoff = param;
            break;
        case GL_CONSTANT_ATTENUATION:
            lightref.constant_attenuation = param;
            break;
        case GL_LINEAR_ATTENUATION:
            lightref.linear_attenuation = param;
            break;
        case GL_QUADRATIC_ATTENUATION:
            lightref.quadratic_attenuation = param;
            break;
        default:
            LOG_D("ERROR: Invalid %s pname: %s", __func__, pname)
    }
}

void glLighti( GLenum light, GLenum pname, GLint param ) {
    LOG()
    LOG_D("glLighti(%s, %s, %d)", glEnumToString(light), glEnumToString(pname), param)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glLighti>({}, light, pname, param);
        if (DisplayListManager::shouldFinish())
            return;
    }

    SELF_CALL(glLightf, light, pname, (GLfloat)param)
}

void glLightfv( GLenum light, GLenum pname,
                                 const GLfloat *params ) {
    LOG()
    LOG_D("glLightfv(%s, %s, [...])", glEnumToString(light), glEnumToString(pname))

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glLightfv>({{2, PointerUtils::pname_to_count(pname) * sizeof(GLfloat)}}, light, pname, params);
        if (DisplayListManager::shouldFinish())
            return;
    }

    switch (pname) {
        case GL_SPOT_CUTOFF:
        case GL_SPOT_EXPONENT:
        case GL_CONSTANT_ATTENUATION:
        case GL_LINEAR_ATTENUATION:
        case GL_QUADRATIC_ATTENUATION:
            SELF_CALL(glLightf, light, pname, params[0])
            break;

        case GL_AMBIENT: {
            auto& lightref = g_glstate.fpe_uniform.lights[light - GL_LIGHT0];
            lightref.ambient = glm::make_vec4(params);
            break;
        }
        case GL_DIFFUSE: {
            auto& lightref = g_glstate.fpe_uniform.lights[light - GL_LIGHT0];
            lightref.diffuse = glm::make_vec4(params);
            break;
        }
        case GL_SPECULAR: {
            auto& lightref = g_glstate.fpe_uniform.lights[light - GL_LIGHT0];
            lightref.specular = glm::make_vec4(params);
            break;
        }
        case GL_POSITION: {
            auto& lightref = g_glstate.fpe_uniform.lights[light - GL_LIGHT0];
            lightref.position = glm::make_vec4(params);
            break;
        }
        case GL_SPOT_DIRECTION: {
            auto& lightref = g_glstate.fpe_uniform.lights[light - GL_LIGHT0];
            lightref.spot_direction = glm::make_vec3(params);
            break;
        }
        default:
            LOG_D("ERROR: Invalid %s pname: %s", __func__, pname);
    }
}

void glLightiv( GLenum light, GLenum pname,
                                 const GLint *params ) {
    LOG()
    LOG_D("glLightiv(%s, %s, [...])", glEnumToString(light), glEnumToString(pname))

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glLightiv>({{2, PointerUtils::pname_to_count(pname) * sizeof(GLint)}}, light, pname, params);
        if (DisplayListManager::shouldFinish())
            return;
    }

    switch (pname) {
        case GL_SPOT_CUTOFF:
        case GL_SPOT_EXPONENT:
        case GL_CONSTANT_ATTENUATION:
        case GL_LINEAR_ATTENUATION:
        case GL_QUADRATIC_ATTENUATION:
            SELF_CALL(glLighti, light, pname, params[0]);
            break;

        case GL_AMBIENT:
        case GL_DIFFUSE:
        case GL_SPECULAR:
        case GL_POSITION: {
            glm::vec4 vec = glm::make_vec4(params);
            SELF_CALL(glLightfv, light, pname, glm::value_ptr(vec))
        }
        case GL_SPOT_DIRECTION: {
            glm::vec3 vec = glm::make_vec3(params);
            SELF_CALL(glLightfv, light, pname, glm::value_ptr(vec))
        }
        default:
            LOG_D("ERROR: Invalid %s pname: %s", __func__, pname)
    }
}

void glLightModelf( GLenum pname, GLfloat param ) {
    LOG()
    LOG_D("glLightModelf(%s, %f)", glEnumToString(pname), param)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glLightModelf>({}, pname, param);
        if (DisplayListManager::shouldFinish())
            return;
    }

    switch (pname) {
        case GL_LIGHT_MODEL_LOCAL_VIEWER:
        case GL_LIGHT_MODEL_COLOR_CONTROL:
        case GL_LIGHT_MODEL_TWO_SIDE:
            SELF_CALL(glLightModeli, pname, (GLint)param)
        default:
            LOG_D("ERROR: Invalid %s pname: %s", __func__, pname)
    }
}

void glLightModeli( GLenum pname, GLint param ) {
    LOG()
    LOG_D("glLightModelf(%s, %d)", glEnumToString(pname), param)

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glLightModeli>({}, pname, param);
        if (DisplayListManager::shouldFinish())
            return;
    }

    switch (pname) {
        case GL_LIGHT_MODEL_COLOR_CONTROL:
            g_glstate.fpe_state.light_model_color_ctrl = param;
            break;
        case GL_LIGHT_MODEL_LOCAL_VIEWER:
            g_glstate.fpe_state.light_model_local_viewer = param;
            break;
        case GL_LIGHT_MODEL_TWO_SIDE:
            g_glstate.fpe_state.light_model_two_side = param;
            break;
        default:
            LOG_D("ERROR: Invalid %s pname: %s", __func__, pname)
    }
}

void glLightModelfv( GLenum pname, const GLfloat *params ) {
    LOG()
    LOG_D("glLightModelfv(%s, [...])", glEnumToString(pname))

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glLightModelfv>({{1, PointerUtils::pname_to_count(pname) * sizeof(GLfloat)}}, pname, params);
        if (DisplayListManager::shouldFinish())
            return;
    }

    switch (pname) {
        case GL_LIGHT_MODEL_AMBIENT:
            g_glstate.fpe_uniform.light_model_ambient = glm::make_vec4(params);
            break;
        case GL_LIGHT_MODEL_COLOR_CONTROL:
        case GL_LIGHT_MODEL_LOCAL_VIEWER:
        case GL_LIGHT_MODEL_TWO_SIDE:
            SELF_CALL(glLightModelf, pname, params[0]);
            break;
        default:
            LOG_D("ERROR: Invalid %s pname: %s", __func__, pname)
    }
}

void glLightModeliv( GLenum pname, const GLint *params ) {
    LOG()
    LOG_D("glLightModeliv(%s, [...])", glEnumToString(pname))

    if (!disableRecording && DisplayListManager::shouldRecord()) {
        displayListManager.record<glLightModeliv>({{1, PointerUtils::pname_to_count(pname) * sizeof(GLint)}}, pname, params);
        if (DisplayListManager::shouldFinish())
            return;
    }

    switch (pname) {
        case GL_LIGHT_MODEL_AMBIENT: {
            glm::vec4 v = glm::make_vec4(params);
            SELF_CALL(glLightModelfv, pname, glm::value_ptr(v))
            break;
        }
        case GL_LIGHT_MODEL_COLOR_CONTROL:
        case GL_LIGHT_MODEL_LOCAL_VIEWER:
        case GL_LIGHT_MODEL_TWO_SIDE:
            SELF_CALL(glLightModeli, pname, params[0]);
            break;
        default:
            LOG_D("ERROR: Invalid %s pname: %s", __func__, pname)
    }
}
