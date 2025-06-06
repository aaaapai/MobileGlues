//
// Created by Swung 0x48 on 2025/3/18.
//

#include "fpe_shadergen.h"
#include <format>
#include <string_view>

#define DEBUG 0

#if GLOBAL_DEBUG || DEBUG
#pragma clang optimize off
#endif

constexpr std::string_view mg_shader_header =
        "#version 300 es\n"
        "// MobileGlues FPE Shader\n"
        "precision highp float;\n"
        "precision highp int;\n";
constexpr std::string_view mg_vs_header =
        "// ** Vertex Shader **\n";
constexpr std::string_view mg_fs_header =
        "// ** Fragment Shader **\n";
constexpr std::string_view mg_fog_linear_func =
        "float fog_linear(float distance, float start, float end) {\n"
        "    return clamp((end - distance) / (end - start), 0., 1.);\n"
        "}\n";
constexpr std::string_view mg_fog_exp_func =
        "float fog_exp(float distance, float density) {\n"
        "    return clamp(exp(-density * distance), 0., 1.);\n"
        "}\n";
constexpr std::string_view mg_fog_exp2_func =
        "float fog_exp2(float distance, float density) {\n"
        "    float scaled = density * distance;\n"
        "    return clamp(exp(-scaled * scaled), 0., 1.);\n"
        "}\n";
constexpr std::string_view mg_fog_apply_fog_func =
        "vec3 apply_fog(vec3 objColor, vec3 fogColor, float fogFactor) {\n"
        "    return mix(fogColor, objColor, fogFactor);\n"
        "}\n";
constexpr std::string_view mg_fog_struct =
        "struct fog_param_t {\n"
        "    vec4  color;\n"
        "    float density;\n"
        "    float start;\n"
        "    float end;\n"
        "};\n";
constexpr std::string_view mg_fog_uniform =
        "uniform fog_param_t fogParam;\n";

constexpr std::string_view mg_alpharef_uniform =
        "uniform float alpharef;\n";

const std::string alpha_test(GLenum func, const std::string_view varname, const std::string_view alpharef) {
    constexpr std::string_view fmt = R"(
    // Alpha Test, func = {}
    if (!({}.a {} {})) {{
        discard;
    }}
)";

    constexpr std::string_view fmt_eq = R"(
    // Alpha Test, func = GL_EQUAL
    if (!({0}.a - 0.00001 < {1} && {1} < {0}.a - 0.00001)) {{
        discard;
    }}
)";

    constexpr std::string_view fmt_neq = R"(
    // Alpha Test, func = GL_NOTEQUAL
    if ({0}.a - 0.00001 < {1} && {1} < {0}.a - 0.00001) {{
        discard;
    }}
)";

    switch (func) {
        case GL_NEVER:
            return "    // Alpha Test\n"
                   "    discard;\n";
        case GL_LESS:
            return std::format(fmt, glEnumToString(func), varname, "<", alpharef);
        case GL_EQUAL:
            return std::format(fmt_eq, varname, alpharef);
        case GL_LEQUAL:
            return std::format(fmt, glEnumToString(func), varname, "<=", alpharef);
        case GL_GREATER:
            return std::format(fmt, glEnumToString(func), varname, ">", alpharef);
        case GL_NOTEQUAL:
            return std::format(fmt_neq, varname, alpharef);
        case GL_GEQUAL:
            return std::format(fmt, glEnumToString(func), varname, ">=", alpharef);
        case GL_ALWAYS:
            return "    // Alpha Test\n"
                   "    // GL_ALWAYS\n";
    }
    return std::string("    ALPHA TEST ERROR: unknown func: ") + glEnumToString(func);
}

std::string vp2in_name(GLenum vp, int index) {
    switch (vp) {
        case GL_VERTEX_ARRAY:
            return "Position";
        case GL_NORMAL_ARRAY:
            return "Normal";
        case GL_COLOR_ARRAY:
            return "Color";
        case GL_INDEX_ARRAY:
            return "Index";
//        case GL_EDGE_FLAG_ARRAY:
//            return "EdgeFlag";
        case GL_FOG_COORD_ARRAY:
            return "FogCoord";
        case GL_SECONDARY_COLOR_ARRAY:
            return "SecColor";
        default: {
            int texidx = index - 7;
            if (texidx >= 0 && texidx < GL_MAX_TEXTURE_IMAGE_UNITS)
                return "UV" + std::to_string(texidx);
            else break;
        }
    }
    LOG_E("ERROR: 1280 %s(%s, %d)", __func__, glEnumToString(vp), index)
    return "ERROR";
}

std::string vp2out_name(GLenum vp, int index) {
    switch (vp) {
        case GL_VERTEX_ARRAY:
            return "Position";
        case GL_NORMAL_ARRAY:
            return "vertexNormal";
        case GL_COLOR_ARRAY:
            return "vertexColor";
        case GL_INDEX_ARRAY:
            return "vertexIndex";
//        case GL_EDGE_FLAG_ARRAY:
//            return "vertexEdgeFlag";
        case GL_FOG_COORD_ARRAY:
            return "vertexFogCoord";
        case GL_SECONDARY_COLOR_ARRAY:
            return "vertexSecColor";
        default: {
            int texidx = index - 7;
            if (texidx >= 0 && texidx < GL_MAX_TEXTURE_IMAGE_UNITS)
                return "texCoord" + std::to_string(texidx);
            else break;
        }
    }
    LOG_E("ERROR: 1280 %s(%s, %d)", __func__, glEnumToString(vp), index)
    return "ERROR";
}

// TODO: deal with integer flat qualifier
std::string type2str(GLenum type, int size) {
    if (size == 1) {
        switch (type) {
            case GL_UNSIGNED_BYTE:
            case GL_UNSIGNED_INT:
//                return "uint";
            case GL_BYTE:
            case GL_SHORT:
            case GL_INT:
//                return "int";
            case GL_FLOAT:
                return "float";
            case GL_DOUBLE:
                return "double";
            default:
                return "ERROR";
        }
    } else {
        switch (type) {
            case GL_UNSIGNED_BYTE:
            case GL_UNSIGNED_INT:
//                return "uvec" + std::to_string(size);
            case GL_BYTE:
            case GL_SHORT:
            case GL_INT:
//                return "ivec" + std::to_string(size);
            case GL_FLOAT:
                return "vec" + std::to_string(size);
            case GL_DOUBLE:
                return "dvec" + std::to_string(size);
            default:
                return "ERROR";
        }
    }
}

void add_vs_inout(const fixed_function_state_t& state, scratch_t& scratch, std::string& vs) {
    auto& vpa = state.normalized_vpa;
    LOG_D("[shadergen] enabled_ptr: 0x%x", vpa.enabled_pointers)
#if DEBUG || GLOBAL_DEBUG
    vs += std::format("// enabled_ptr: 0x{:x}\n", vpa.enabled_pointers);
#endif
    for (int i = 0; i < VERTEX_POINTER_COUNT; ++i) {
        bool enabled = ((vpa.enabled_pointers >> i) & 1);

        if (enabled || state.fpe_draw.current_data.sizes.data[i] > 0) {
            auto &vp = vpa.attributes[i];

            if (enabled)
                LOG_D("attrib #%d, cidx #%u: type = %s, size = %d, stride = %d, usage = %s, ptr = %p",
                      i, vpa.cidx(i), glEnumToString(vp.type), vp.size, vp.stride, glEnumToString(vp.usage), vp.pointer)
            else
            {
                LOG_D("attrib #%d, cidx #%u: type = %s, usage = %s, size = %d (disabled)",
                      i, vpa.cidx(i), glEnumToString(vp.type), glEnumToString(vp.usage), state.fpe_draw.current_data.sizes.data[i])
            }

            std::string in_name = enabled ? vp2in_name(vp.usage, i) : vp2in_name(idx2vp(i), i);
            std::string type = enabled ? type2str(vp.type, vp.size) : type2str(GL_FLOAT, 4);

            vs += std::format("layout (location = {}) in {} {};\n",
                                        vpa.cidx(i), type, in_name);

            if (vp.usage == GL_VERTEX_ARRAY) { // GL_VERTEX_ARRAY will be written into gl_Position
                continue;
            }

            std::string out_name = enabled ? vp2out_name(vp.usage, i) : vp2out_name(idx2vp(i), i);
            std::string linkage;

            linkage += type;
            linkage += ' ';
            linkage += out_name;
            linkage += ";\n";

            vs += "out ";
            vs += linkage;

            scratch.last_stage_linkage += "in " + linkage;

            // TODO: if not this simple? Fog / Vertex light?
            scratch.vs_body += out_name;
            scratch.vs_body += " = ";
            scratch.vs_body += in_name;
            scratch.vs_body += ";\n";

            if (vp.usage == GL_COLOR_ARRAY)
                scratch.has_vertex_color = true;

            int texid = vp.usage - GL_TEXTURE_COORD_ARRAY;
            if (0 <= texid && texid < MAX_TEX) {
                LOG_D("has_texcoord[%d] = true", texid)
                scratch.has_texcoord[texid] = true;
            }
        }
    }

    if (state.fpe_bools.fog_enable) {
        vs += "out vec3 vViewPosition;\n";
    }
}

void add_vs_uniforms(const fixed_function_state_t& state, scratch_t& scratch, std::string& vs) {
    // Transformation matrix
    vs += "uniform mat4 ModelViewProjMat;\n";
    if (state.fpe_bools.fog_enable) {
        vs += "uniform mat4 ModelViewMat;\n";
    }
}

void add_vs_body(const fixed_function_state_t& state, scratch_t& scratch, std::string& vs) {
    vs +=
            "void main() {\n"
//            "   gl_Position = ProjMat * ModelViewMat * vec4(Position, 1.0);\n";
            "    gl_Position = ModelViewProjMat * vec4(Position, 1.0);\n";
    if (state.fpe_bools.fog_enable) {
        vs += "    vec4 viewPosition = ModelViewMat * vec4(Position, 1.0);\n"
              "    vViewPosition = viewPosition.xyz;\n";
    }
    vs += scratch.vs_body;
    vs += "}\n";
}

void add_fs_uniforms(const fixed_function_state_t& state, scratch_t& scratch, std::string& fs) {
    // Hardcode a sampler here...
    // TODO: Fix this on multitexture
//    if (scratch.has_texcoord)
//        fs += std::format(
//                "uniform sampler2D Sampler{};\n", 0);
    for (int i = 0; i < MAX_TEX; ++i) {
        if (scratch.has_texcoord[i]) {
            fs += std::format(
                    "uniform sampler2D Sampler{};\n", i);
        }
    }

    if (state.fpe_bools.fog_enable) {
        fs += mg_fog_struct;
        fs += mg_fog_uniform;
    }

    if (state.fpe_bools.alpha_test_enable) {
        fs += mg_alpharef_uniform;
    }
}

void add_fs_inout(const fixed_function_state_t& state, scratch_t& scratch, std::string& fs) {
    // Linking from VS
    fs += scratch.last_stage_linkage;
    fs += "\n";
    if (state.fpe_bools.fog_enable) {
        fs += "in vec3 vViewPosition;\n";
    }
    fs += "out vec4 FragColor;\n";
}

void add_fs_body(const fixed_function_state_t& state, scratch_t& scratch, std::string& fs) {
    // Fog function
    if (state.fpe_bools.fog_enable) {
        fs += mg_fog_apply_fog_func;
        switch (state.fog_mode) {
            case GL_LINEAR:
                fs += mg_fog_linear_func;
                break;
            case GL_EXP:
                fs += mg_fog_exp_func;
                break;
            case GL_EXP2:
                fs += mg_fog_exp2_func;
                break;
        }
    }

    // TODO: Replace this hardcode with something better...
    fs += "void main() {\n";

    if (scratch.has_vertex_color)
        fs += "    vec4 color = vertexColor;\n";
    else
        fs += "    vec4 color = vec4(1., 1., 1., 1.);\n";

//    if (scratch.has_texcoord) {
//        fs += std::format(
//                "    vec4 texcolor{0} = texture(Sampler{0}, texCoord{0});\n"
//                "    color *= texcolor{0};\n", 0);
//    }

    for (int i = 0; i < MAX_TEX; ++i) {
        if (i > 0)
            break;
        if (scratch.has_texcoord[i]) {
            fs += std::format(
                "\n"
                "    // Texturing #{0}\n"
                "    vec4 texcolor{0} = texture(Sampler{0}, texCoord{0});\n"
                "    color *= texcolor{0};\n", i);
        }
    }

    // Alpha test
    if (state.fpe_bools.alpha_test_enable)
        fs += alpha_test(state.alpha_func, "color", "alpharef");
    else
        fs += "    // Alpha Test\n"
              "    // (Disabled)\n";


    // Fog calculation
    if (state.fpe_bools.fog_enable) {
        fs += "    float distance = length(vViewPosition);\n";
        switch (state.fog_mode) {
            case GL_LINEAR:
                fs += "    float fogFactor = fog_linear(distance, fogParam.start, fogParam.end);\n";
                break;
            case GL_EXP:
                fs += "    float fogFactor = fog_exp(distance, fogParam.density);\n";
                break;
            case GL_EXP2:
                fs += "    float fogFactor = fog_exp2(distance, fogParam.density);\n";
                break;
        }
        fs += "    color.rgb = apply_fog(color.rgb, fogParam.color.rgb, fogFactor);\n";
//        fs += "    color = vec4(fogFactor, fogFactor, fogFactor, 1.);\n";
    }

    fs += "   FragColor = color;\n"
          "}";
}

program_t fpe_shader_generator::generate_program() {
    program_t program;

    program.vs = vertex_shader(state_, scratch_);
    program.fs = fragment_shader(state_, scratch_);

    return program;
}

std::string fpe_shader_generator::vertex_shader(const fixed_function_state_t& state, scratch_t& scratch) {
    std::string shader;
    shader += mg_shader_header;
    shader += mg_vs_header;

    shader += "\n";
    add_vs_inout(state, scratch, shader);
    shader += "\n";
    add_vs_uniforms(state, scratch, shader);
    shader += "\n";
    add_vs_body(state, scratch, shader);

    return shader;
}

std::string fpe_shader_generator::fragment_shader(const fixed_function_state_t& state, scratch_t& scratch) {
    std::string shader;

    shader += mg_shader_header;
    shader += mg_fs_header;

    shader += "\n";
    add_fs_inout(state, scratch, shader);
    shader += "\n";
    add_fs_uniforms(state, scratch, shader);
    shader += "\n";
    add_fs_body(state, scratch, shader);

    return shader;
}

int program_t::get_program() {
    if (program > 0)
        return program;

    int vss = compile_shader(GL_VERTEX_SHADER, vs.c_str());
    if (vss < 0)
        return vss;
    int fss = compile_shader(GL_FRAGMENT_SHADER, fs.c_str());
    if (fss < 0)
        return fss;
    program = link_program(vss, fss);
    return program;
}

int program_t::compile_shader(GLenum shader_type, const char* src) {
    static char compile_info[1024];

    INIT_CHECK_GL_ERROR

    int shader = GLES.glCreateShader(shader_type);
    CHECK_GL_ERROR_NO_INIT
    GLES.glShaderSource(shader, 1, &src, NULL);
    CHECK_GL_ERROR_NO_INIT
    GLES.glCompileShader(shader);
    CHECK_GL_ERROR_NO_INIT
    int success = 0;
    GLES.glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    CHECK_GL_ERROR_NO_INIT
    if (!success) {
        GLES.glGetShaderInfoLog(shader, 1024, NULL, compile_info);
        CHECK_GL_ERROR_NO_INIT
        LOG_E("%s: %s shader compile error: %s\nsrc:\n%s",
              __func__,
              (shader_type == GL_VERTEX_SHADER) ? "vertex" : "fragment",
              compile_info,
              src);
#if DEBUG || GLOBAL_DEBUG
        abort();
#endif
        return -1;
    }

    return shader;
}

int program_t::link_program(GLuint vs, GLuint fs) {
    static char compile_info[1024];

    INIT_CHECK_GL_ERROR

    int program = GLES.glCreateProgram();
    CHECK_GL_ERROR_NO_INIT
    GLES.glAttachShader(program, vs);
    CHECK_GL_ERROR_NO_INIT
    GLES.glAttachShader(program, fs);
    CHECK_GL_ERROR_NO_INIT
    GLES.glLinkProgram(program);
    CHECK_GL_ERROR_NO_INIT
    int success = 0;
    GLES.glGetProgramiv(program, GL_LINK_STATUS, &success);
    CHECK_GL_ERROR_NO_INIT
    if(!success) {
        GLES.glGetProgramInfoLog(program, 1024, NULL, compile_info);
        CHECK_GL_ERROR_NO_INIT
        LOG_E("program link error: %s", compile_info);
#if DEBUG || GLOBAL_DEBUG
        abort();
#endif
        return -1;
    }
    LOG_E("program link success");
    return program;
}
