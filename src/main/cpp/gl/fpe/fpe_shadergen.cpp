//
// Created by Swung 0x48 on 2025/3/18.
//

#include "fpe_shadergen.h"
#include <format>

#define DEBUG 0

#if GLOBAL_DEBUG || DEBUG
#pragma clang optimize off
#endif

const static std::string mg_shader_header =
        "#version 300 es\n"
        "// MobileGlues FPE Shader\n"
        "precision highp float;\n"
        "precision highp int;\n";
const static std::string mg_vs_header =
        "// ** Vertex Shader **\n";
const static std::string mg_fs_header =
        "// ** Fragment Shader **\n";
const static std::string mg_fog_linear_func =
        "float fog_linear(float distance, float start, float end) {\n"
        "    return clamp((end - distance) / (end - start), 0., 1.);\n"
        "}\n";
const static std::string mg_fog_exp_func =
        "float fog_exp(float distance, float density) {\n"
        "    return clamp(exp(-density * distance), 0., 1.);\n"
        "}\n";
const static std::string mg_fog_exp2_func =
        "float fog_exp2(float distance, float density) {\n"
        "    float scaled = density * distance;\n"
        "    return clamp(exp(-scaled * scaled), 0., 1.);\n"
        "}\n";
const static std::string mg_fog_apply_fog_func =
        "vec4 apply_fog(vec4 objColor, vec4 fogColor, float fogFactor) {\n"
        "    return mix(fogColor, objColor, fogFactor);\n"
        "}\n";
const static std::string mg_fog_struct =
        "struct fog_param_t {\n"
        "    vec4  color;\n"
        "    float density;\n"
        "    float start;\n"
        "    float end;\n"
        "};\n";
const static std::string mg_fog_uniform =
        "uniform fog_param_t fogParam;\n";

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
        case GL_EDGE_FLAG_ARRAY:
            return "EdgeFlag";
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
    LOG_E("ERROR: 1280")
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
        case GL_EDGE_FLAG_ARRAY:
            return "vertexEdgeFlag";
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
    LOG_E("ERROR: 1280")
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
    auto& vpa = state.vertexpointer_array;
    LOG_D("[shadergen] enabled_ptr: 0x%x", vpa.enabled_pointers)
#if DEBUG || GLOBAL_DEBUG
    vs += std::format("// enabled_ptr: 0x{:x}\n", vpa.enabled_pointers);
#endif
    for (int i = 0; i < VERTEX_POINTER_COUNT; ++i) {
        bool enabled = ((vpa.enabled_pointers >> i) & 1);

        // hack in color (static, disabled vertex array) here...
        // TODO: fix this using uniform
        if (enabled || vpa.attributes[i].usage == GL_COLOR_ARRAY) {
            auto &vp = vpa.attributes[i];

            LOG_D("attrib #%d: type = %s, size = %d, stride = %d, usage = %s, ptr = %p",
                  i, glEnumToString(vp.type), vp.size, vp.stride, glEnumToString(vp.usage), vp.pointer)

            std::string in_name = vp2in_name(vp.usage, i);
            std::string type = type2str(vp.type, vp.size);

            vs += "layout (location = ";
            vs += std::to_string(i);
            vs += ") in ";
            vs += type;
            vs += ' ';
            vs += in_name;
            vs += ";\n";

            if (vp.usage != GL_VERTEX_ARRAY) { // GL_VERTEX_ARRAY will be written into gl_Position
                std::string out_name = vp2out_name(vp.usage, i);
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
            }

            if (vp.usage == GL_COLOR_ARRAY)
                scratch.has_vertex_color = true;

            if (vp.usage == GL_TEXTURE_COORD_ARRAY)
                scratch.has_texcoord = true;
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
    if (scratch.has_texcoord)
        fs += "uniform sampler2D Sampler0;\n";

    if (state.fpe_bools.fog_enable) {
        fs += mg_fog_struct;
        fs += mg_fog_uniform;
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

    if (scratch.has_texcoord)
        fs += "   vec4 color = texture(Sampler0, texCoord0);\n";
    else
        fs += "   vec4 color = vec4(1., 1., 1., 1.);\n";

    if (scratch.has_vertex_color)
        fs += "    color *= vertexColor;\n";

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
        fs += "    color = apply_fog(color, fogParam.color, fogFactor);\n";
    }

    fs += "   if (color.a < 0.1) {\n"
          "       discard;\n"
          "   }\n"
          "   FragColor = color;\n"
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
        LOG_E("%s shader compile error: %s\nsrc:\n%s", compile_info,
              (shader_type == GL_VERTEX_SHADER) ? "vertex" : "fragment",
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
