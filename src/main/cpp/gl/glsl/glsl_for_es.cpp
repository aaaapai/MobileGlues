#include "glsl_for_es.h"

#include <glslang/Public/ShaderLang.h>
#include <glslang/Include/Types.h>
#include <glslang/Public/ShaderLang.h>
#include <spirv_cross/spirv_cross_c.h>
#include <iostream>
#include <fstream>
#include "../log.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include <shaderc/shader.h>
#include "preConvertedGlsl.h"
#include <string>
#include <regex>
#include <strstream>
#include <algorithm>
#include <sstream>
#include "cache.h"
#include "../../version.h"

// #define FEATURE_PRE_CONVERTED_GLSL

#define DEBUG 1

#if !defined(__APPLE__)
char* (*MesaConvertShader)(const char *src, GLenum type, unsigned int glsl, unsigned int essl);
#endif

static TBuiltInResource InitResources()
{
    TBuiltInResource Resources{};

    Resources.maxLights                                 = 32;
    Resources.maxClipPlanes                             = 6;
    Resources.maxTextureUnits                           = 32;
    Resources.maxTextureCoords                          = 32;
    Resources.maxVertexAttribs                          = 64;
    Resources.maxVertexUniformComponents                = 4096;
    Resources.maxVaryingFloats                          = 64;
    Resources.maxVertexTextureImageUnits                = 32;
    Resources.maxCombinedTextureImageUnits              = 80;
    Resources.maxTextureImageUnits                      = 32;
    Resources.maxFragmentUniformComponents              = 4096;
    Resources.maxDrawBuffers                            = 32;
    Resources.maxVertexUniformVectors                   = 128;
    Resources.maxVaryingVectors                         = 8;
    Resources.maxFragmentUniformVectors                 = 16;
    Resources.maxVertexOutputVectors                    = 16;
    Resources.maxFragmentInputVectors                   = 15;
    Resources.minProgramTexelOffset                     = -8;
    Resources.maxProgramTexelOffset                     = 7;
    Resources.maxClipDistances                          = 8;
    Resources.maxComputeWorkGroupCountX                 = 65535;
    Resources.maxComputeWorkGroupCountY                 = 65535;
    Resources.maxComputeWorkGroupCountZ                 = 65535;
    Resources.maxComputeWorkGroupSizeX                  = 1024;
    Resources.maxComputeWorkGroupSizeY                  = 1024;
    Resources.maxComputeWorkGroupSizeZ                  = 64;
    Resources.maxComputeUniformComponents               = 1024;
    Resources.maxComputeTextureImageUnits               = 16;
    Resources.maxComputeImageUniforms                   = 8;
    Resources.maxComputeAtomicCounters                  = 8;
    Resources.maxComputeAtomicCounterBuffers            = 1;
    Resources.maxVaryingComponents                      = 60;
    Resources.maxVertexOutputComponents                 = 64;
    Resources.maxGeometryInputComponents                = 64;
    Resources.maxGeometryOutputComponents               = 128;
    Resources.maxFragmentInputComponents                = 128;
    Resources.maxImageUnits                             = 8;
    Resources.maxCombinedImageUnitsAndFragmentOutputs   = 8;
    Resources.maxCombinedShaderOutputResources          = 8;
    Resources.maxImageSamples                           = 0;
    Resources.maxVertexImageUniforms                    = 0;
    Resources.maxTessControlImageUniforms               = 0;
    Resources.maxTessEvaluationImageUniforms            = 0;
    Resources.maxGeometryImageUniforms                  = 0;
    Resources.maxFragmentImageUniforms                  = 8;
    Resources.maxCombinedImageUniforms                  = 8;
    Resources.maxGeometryTextureImageUnits              = 16;
    Resources.maxGeometryOutputVertices                 = 256;
    Resources.maxGeometryTotalOutputComponents          = 1024;
    Resources.maxGeometryUniformComponents              = 1024;
    Resources.maxGeometryVaryingComponents              = 64;
    Resources.maxTessControlInputComponents             = 128;
    Resources.maxTessControlOutputComponents            = 128;
    Resources.maxTessControlTextureImageUnits           = 16;
    Resources.maxTessControlUniformComponents           = 1024;
    Resources.maxTessControlTotalOutputComponents       = 4096;
    Resources.maxTessEvaluationInputComponents          = 128;
    Resources.maxTessEvaluationOutputComponents         = 128;
    Resources.maxTessEvaluationTextureImageUnits        = 16;
    Resources.maxTessEvaluationUniformComponents        = 1024;
    Resources.maxTessPatchComponents                    = 120;
    Resources.maxPatchVertices                          = 32;
    Resources.maxTessGenLevel                           = 64;
    Resources.maxViewports                              = 16;
    Resources.maxVertexAtomicCounters                   = 0;
    Resources.maxTessControlAtomicCounters              = 0;
    Resources.maxTessEvaluationAtomicCounters           = 0;
    Resources.maxGeometryAtomicCounters                 = 0;
    Resources.maxFragmentAtomicCounters                 = 8;
    Resources.maxCombinedAtomicCounters                 = 8;
    Resources.maxAtomicCounterBindings                  = 1;
    Resources.maxVertexAtomicCounterBuffers             = 0;
    Resources.maxTessControlAtomicCounterBuffers        = 0;
    Resources.maxTessEvaluationAtomicCounterBuffers     = 0;
    Resources.maxGeometryAtomicCounterBuffers           = 0;
    Resources.maxFragmentAtomicCounterBuffers           = 1;
    Resources.maxCombinedAtomicCounterBuffers           = 1;
    Resources.maxAtomicCounterBufferSize                = 16384;
    Resources.maxTransformFeedbackBuffers               = 4;
    Resources.maxTransformFeedbackInterleavedComponents = 64;
    Resources.maxCullDistances                          = 8;
    Resources.maxCombinedClipAndCullDistances           = 8;
    Resources.maxSamples                                = 4;
    Resources.maxMeshOutputVerticesNV                   = 256;
    Resources.maxMeshOutputPrimitivesNV                 = 512;
    Resources.maxMeshWorkGroupSizeX_NV                  = 32;
    Resources.maxMeshWorkGroupSizeY_NV                  = 1;
    Resources.maxMeshWorkGroupSizeZ_NV                  = 1;
    Resources.maxTaskWorkGroupSizeX_NV                  = 32;
    Resources.maxTaskWorkGroupSizeY_NV                  = 1;
    Resources.maxTaskWorkGroupSizeZ_NV                  = 1;
    Resources.maxMeshViewCountNV                        = 4;

    Resources.limits.nonInductiveForLoops                 = true;
    Resources.limits.whileLoops                           = true;
    Resources.limits.doWhileLoops                         = true;
    Resources.limits.generalUniformIndexing               = true;
    Resources.limits.generalAttributeMatrixVectorIndexing = true;
    Resources.limits.generalVaryingIndexing               = true;
    Resources.limits.generalSamplerIndexing               = true;
    Resources.limits.generalVariableIndexing              = true;
    Resources.limits.generalConstantMatrixVectorIndexing  = true;

    return Resources;
}

int getGLSLVersion(const char* glsl_code) {
    std::string code(glsl_code);
    static std::regex version_pattern(R"(#version\s+(\d{3}))");
    std::smatch match;
    if (std::regex_search(code, match, version_pattern)) {
        return std::stoi(match[1].str());
    }

    return -1;
}

std::string forceSupporterOutput(const std::string& glslCode) {
    bool hasPrecisionFloat = glslCode.find("precision ") != std::string::npos &&
                             glslCode.find("float;") != std::string::npos;
    bool hasPrecisionInt = glslCode.find("precision ") != std::string::npos &&
                           glslCode.find("int;") != std::string::npos;

    std::string result = glslCode;
    std::string precisionFloat;
    std::string precisionInt;

    if (hasPrecisionFloat && hasPrecisionInt) {
        std::istringstream iss(result);
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(iss, line)) {
            bool isPrecisionLine = (line.find("precision ") != std::string::npos) &&
                                   (line.find("float;") != std::string::npos || line.find("int;") != std::string::npos);
            if (!isPrecisionLine) {
                lines.push_back(line);
            }
        }
        result.clear();
        for (size_t i = 0; i < lines.size(); ++i) {
            if (i != 0) result += '\n';
            result += lines[i];
        }
        precisionFloat = "precision highp float;\n";
        precisionInt = "precision highp int;\n";
    } else {
        precisionFloat = hasPrecisionFloat ? "" : "precision highp float;\n";
        precisionInt = hasPrecisionInt ? "" : "precision highp int;\n";
    }

    size_t lastExtensionPos = result.rfind("#extension");
    size_t insertionPos = 0;

    if (lastExtensionPos != std::string::npos) {
        size_t nextNewline = result.find('\n', lastExtensionPos);
        if (nextNewline != std::string::npos) {
            insertionPos = nextNewline + 1;
        } else {
            insertionPos = result.length();
        }
    } else {
        size_t firstNewline = result.find('\n');
        if (firstNewline != std::string::npos) {
            insertionPos = firstNewline + 1;
        } else {
            result = precisionFloat + precisionInt + result;
            return result;
        }
    }

    result.insert(insertionPos, precisionFloat + precisionInt);
    return result;
}

std::string removeLayoutBinding(const std::string& glslCode) {
    static std::regex bindingRegex(R"(layout\s*\(\s*binding\s*=\s*\d+\s*\)\s*)");
    std::string result = std::regex_replace(glslCode, bindingRegex, "");
    static std::regex bindingRegex2(R"(layout\s*\(\s*binding\s*=\s*\d+\s*,)");
    result = std::regex_replace(result, bindingRegex2, "layout(");
    return result;
}

void trim(std::string& str) {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
        return !std::isspace(ch);
    }));
    str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), str.end());
}

// Process all uniform declarations into `uniform <precision> <type> <name>;` form
std::string process_uniform_declarations(const std::string& glslCode) {
    std::string result;
    size_t scan_pos = 0;
    size_t chunk_start = 0;
    const size_t length = glslCode.length();
    const std::vector<std::string> precision_kws = {"highp", "lowp", "mediump"};

    result.reserve(glslCode.length());

    while (scan_pos < length) {
        if (glslCode.compare(scan_pos, 7, "uniform") == 0) {
            if (scan_pos > chunk_start) {
                result.append(glslCode, chunk_start, scan_pos - chunk_start);
            }

            const size_t decl_start = scan_pos;
            scan_pos += 7;  // Skip "uniform"

            // 解析精度限定符和类型
            std::string precision, type;
            bool found_precision = false;

            // 第一轮解析：类型前的精度限定符
            while (scan_pos < length) {
                while (scan_pos < length && std::isspace(glslCode[scan_pos])) ++scan_pos;

                // 检查精度限定符
                for (const auto& kw : precision_kws) {
                    if (glslCode.compare(scan_pos, kw.length(), kw) == 0) {
                        precision = " " + kw;
                        scan_pos += kw.length();
                        found_precision = true;
                        break;
                    }
                }
                if (found_precision) break;

                // 开始提取类型
                const size_t type_start = scan_pos;
                while (scan_pos < length && (std::isalnum(glslCode[scan_pos]) || glslCode[scan_pos] == '_')) {
                    ++scan_pos;
                }
                type = glslCode.substr(type_start, scan_pos - type_start);
                break;
            }

            // 第二轮解析：类型后的精度限定符
            while (scan_pos < length) {
                while (scan_pos < length && std::isspace(glslCode[scan_pos])) ++scan_pos;

                bool found = false;
                for (const auto& kw : precision_kws) {
                    if (glslCode.compare(scan_pos, kw.length(), kw) == 0) {
                        if (precision.empty()) precision = " " + kw;
                        scan_pos += kw.length();
                        found = true;
                        break;
                    }
                }
                if (!found) break;
            }

            // 确保类型被正确提取
            if (type.empty()) {
                const size_t type_start = scan_pos;
                while (scan_pos < length && (std::isalnum(glslCode[scan_pos]) || glslCode[scan_pos] == '_')) {
                    ++scan_pos;
                }
                type = glslCode.substr(type_start, scan_pos - type_start);
            }

            // 提取变量名
            while (scan_pos < length && std::isspace(glslCode[scan_pos])) ++scan_pos;
            const size_t name_start = scan_pos;
            while (scan_pos < length && (std::isalnum(glslCode[scan_pos]) || glslCode[scan_pos] == '_')) {
                ++scan_pos;
            }
            const std::string name = glslCode.substr(name_start, scan_pos - name_start);

            // 定位声明结束
            size_t decl_end = glslCode.find(';', scan_pos);
            if (decl_end == std::string::npos) decl_end = length;
            else ++decl_end;

            // 处理初始化值
            const bool has_initializer = (glslCode.find('=', scan_pos) < decl_end);
            if (has_initializer) {
                result.append("uniform")
                        .append(precision)
                        .append(" ")
                        .append(type)
                        .append(" ")
                        .append(name)
                        .append(";");
            } else {
                result.append(glslCode, decl_start, decl_end - decl_start);
            }

            scan_pos = chunk_start = decl_end;
        } else {
            ++scan_pos;
        }
    }

    if (chunk_start < length) {
        result.append(glslCode, chunk_start, length - chunk_start);
    }

    return result;
}

std::string processOutColorLocations(const std::string& glslCode) {
    const static std::regex pattern(R"(\n(out highp vec4 outColor)(\d+);)");
    const std::string replacement = "\nlayout(location=$2) $1$2;";
    return std::regex_replace(glslCode, pattern, replacement);
}

std::string getCachedESSL(const char* glsl_code, uint essl_version) {
    std::string sha256_string(glsl_code);
    sha256_string += "\n//" + std::to_string(MAJOR) + "." + std::to_string(MINOR) + "." + std::to_string(REVISION) + "|" + std::to_string(essl_version);
    const char* cachedESSL = Cache::get_instance().get(sha256_string.c_str());
    if (cachedESSL) {
        LOG_D("GLSL Hit Cache:\n%s\n-->\n%s", glsl_code, cachedESSL)
        return cachedESSL;
    } else return "";
}


std::string GLSLtoGLSLES(const char* glsl_code, GLenum glsl_type, uint essl_version, uint glsl_version) {
    std::string sha256_string(glsl_code);
    sha256_string += "\n//" + std::to_string(MAJOR) + "." + std::to_string(MINOR) + "." + std::to_string(REVISION) + "|" + std::to_string(essl_version);
    const char* cachedESSL = Cache::get_instance().get(sha256_string.c_str());
    if (cachedESSL) {
        LOG_D("GLSL Hit Cache:\n%s\n-->\n%s", glsl_code, cachedESSL)
        return (char*)cachedESSL;
    }
    
    int return_code = -1;
    std::string converted = glsl_version<140? GLSLtoGLSLES_2(glsl_code, glsl_type, essl_version, return_code):GLSLtoGLSLES_2(glsl_code, glsl_type, essl_version, return_code);
    if (return_code == 0 && !converted.empty()) {
        converted = process_uniform_declarations(converted);
        Cache::get_instance().put(sha256_string.c_str(), converted.c_str());
    }

    return (return_code == 0) ? converted : glsl_code;
}

std::string replace_line_starting_with(const std::string& glslCode, const std::string& starting, const std::string& substitution = "") {
    std::string result;
    size_t length = glslCode.size();
    size_t start = 0;  // 当前保留块的起始位置
    size_t current = 0;

    auto append_chunk = [&](size_t end) {
        if (end > start) {
            result.append(glslCode, start, end - start);
        }
    };

    while (current < length) {
        // Skip whitespace at line begin
        size_t lineStart = current;
        while (current < length && (glslCode[current] == ' ' || glslCode[current] == '\t')) {
            current++;
        }

        // Check whether #line directive
        bool isLineDirective = false;
        if (current + 5 <= length && glslCode.compare(current, starting.size(), starting) == 0) {
            isLineDirective = true;
        }

        // Move to line end
        while (current < length && glslCode[current] != '\r' && glslCode[current] != '\n') {
            current++;
        }

        // Handle carriage return
        size_t newlineLength = 0;
        if (current < length) {
            if (glslCode[current] == '\r') {
                newlineLength = (current + 1 < length && glslCode[current + 1] == '\n') ? 2 : 1;
            }
            else {
                newlineLength = 1;
            }
        }

        if (isLineDirective) {
            // Find #line directive ->
            //  1. Append chunk
            append_chunk(lineStart); // from chunk_begin to before `#line`
            // 2. Skip this line (incl. \n)
            current += newlineLength;
            start = current; // 3. Starting from next line

            result += substitution;
        }
        else {
            // move to a new line
            current += newlineLength;
        }
    }

    // append last block
    append_chunk(current);
    return result;
}

static inline void replace_all(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
}

static inline void inject_textureQueryLod(std::string& glsl) {
    const std::string textureQueryLodCall = "textureQueryLod(";
    const std::string textureQueryLodDef = "vec2 mg_textureQueryLod(sampler2D tex, vec2 uv)";
    const std::string mainStart = "void main()";

    // Already defined function
    const auto def_loc = glsl.find(textureQueryLodDef);
    if (def_loc != std::string::npos)
        return;

    // Never called function
    const auto call_loc = glsl.find(textureQueryLodCall);
    if (call_loc == std::string::npos)
        return;


    const auto main_loc = glsl.find(mainStart);
    // No main(), no inject
    if (main_loc == std::string::npos)
        return;

    const std::string textureQueryLodImpl = R"(
vec2 mg_textureQueryLod(sampler2D tex, vec2 uv) {
    vec2 texSizeF = vec2(textureSize(tex, 0));
    vec2 dFdx_uv = dFdx(uv * texSizeF);
    vec2 dFdy_uv = dFdy(uv * texSizeF);
    float maxDerivative = max(length(dFdx_uv), length(dFdy_uv));
    float lod = log2(maxDerivative);
    return vec2(lod);
}
)";
    // Replace all calls to textureQueryLod()
    replace_all(glsl, "textureQueryLod(", "mg_textureQueryLod(");

    // Do injection here
    glsl.insert(main_loc, "\n" + textureQueryLodImpl + "\n");
}

static inline void inject_temporal_filter(std::string& glsl) {
    const std::string temporalFilterCall = "deferredOutput2 = GI_TemporalFilter()";
    const std::string temporalFilterDef = "vec4 GI_TemporalFilter()";
    const std::string mainStart = "void main()";

    // Already defined function
    const auto def_loc = glsl.find(temporalFilterDef);
    if (def_loc != std::string::npos)
        return;

    // Never called function
    const auto call_loc = glsl.find(temporalFilterCall);
    if (call_loc == std::string::npos)
        return;


    const auto main_loc = glsl.find(mainStart);
    // No main(), no inject
    if (main_loc == std::string::npos)
        return;

    const std::string GI_TemporalFilter = R"(
vec4 GI_TemporalFilter() {
    vec2 uv = gl_FragCoord.xy / screenSize;
    uv += taaJitter * pixelSize;
    vec4 currentGI = texture(colortex0, uv);
    float depth = texture(depthtex0, uv).r;
    vec4 clipPos = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 viewPos = gbufferProjectionInverse * clipPos;
    viewPos /= viewPos.w;
    vec4 worldPos = gbufferModelViewInverse * viewPos;
    vec4 prevClipPos = gbufferPreviousProjection * (gbufferPreviousModelView * worldPos);
    prevClipPos /= prevClipPos.w;
    vec2 prevUV = prevClipPos.xy * 0.5 + 0.5;
    vec4 historyGI = texture(colortex1, prevUV);
    float difference = length(currentGI.rgb - historyGI.rgb);
    float thresholdValue = 0.1;
    float adaptiveBlend = mix(0.9, 0.0, smoothstep(thresholdValue, thresholdValue * 2.0, difference));
    vec4 filteredGI = mix(currentGI, historyGI, adaptiveBlend);
    if (difference > thresholdValue * 2.0) {
        filteredGI = currentGI;
    }
    return filteredGI;
}
)";
    // Do injection here
    glsl.insert(main_loc, "\n" + GI_TemporalFilter + "\n");
}

#define xstr(s) str(s)
#define str(s) #s

void inject_mg_macro_definition(std::string& glslCode) {
    std::string macro_definitions =
            "\n#define MG_MOBILEGLUES\n"
            "#define MG_MOBILEGLUES_VERSION " xstr(MAJOR) xstr(MINOR) xstr(REVISION) xstr(PATCH) "\n";

    size_t versionPos = glslCode.rfind("#version");
    size_t insertionPos = 0;

    if (versionPos != std::string::npos) {
        size_t nextNewline = glslCode.find('\n', versionPos);
        insertionPos = (nextNewline != std::string::npos) ? nextNewline + 1 : glslCode.length();
    } else {
        size_t firstNewline = glslCode.find('\n');
        insertionPos = (firstNewline != std::string::npos) ? firstNewline + 1 : 0;
    }

    glslCode.insert(insertionPos, macro_definitions);
}


std::string preprocess_glsl(const std::string& glsl, GLenum glsl_type) {
    std::string ret = glsl;
    // Remove lines beginning with `#line`
    ret = replace_line_starting_with(ret, "#line");
    // Act as if disable_GL_ARB_derivative_control is false
    replace_all(ret, "#ifdef GL_ARB_derivative_control", "#if 0");
    replace_all(ret, "#ifndef GL_ARB_derivative_control", "#if 1");

    // Polyfill transpose()
    replace_all(ret,
                "const mat3 rotInverse = transpose(rot);",
                "const mat3 rotInverse = mat3(rot[0][0], rot[1][0], rot[2][0], rot[0][1], rot[1][1], rot[2][1], rot[0][2], rot[1][2], rot[2][2]);");

    replace_all(ret, "texture2D", "texture");
    replace_all(ret, "#version 100", "#version 330");
    replace_all(ret, "#version 110", "#version 330");
    replace_all(ret, "#version 120", "#version 330");
    replace_all(ret, "#version 130", "#version 330");
    replace_all(ret, "#version 140", "#version 330");
    replace_all(ret, "#version 150", "#version 330");
    
    replace_all(ret, "vec3 worldPosDiff", "vec4 worldPosDiff");
    replace_all(ret, "vec3[3](vWorldPos[0] - vWorldPos[1]", "vec4[3](vWorldPos[0] - vWorldPos[1]");
    replace_all(ret, "vec3 reflection;", "vec3 reflection=vec3(0,0,0);");

    // Replace deprecated syntax
    if (glsl_type == GL_VERTEX_SHADER) {
        replace_all(ret, "attribute", "in");
        replace_all(ret, "varying", "out");
    } else if (glsl_type == GL_FRAGMENT_SHADER) {
        replace_all(ret, "varying", "in");
    }
      
    // GI_TemporalFilter injection
    inject_temporal_filter(ret);

    // textureQueryLod injection
    if (!g_gles_caps.GL_EXT_texture_query_lod) {
        inject_textureQueryLod(ret);
    }


    // MobileGlues macros injection
    inject_mg_macro_definition(ret);

    return ret;
}

int get_or_add_glsl_version(std::string& glsl) {
    int glsl_version = getGLSLVersion(glsl.c_str());
    if (glsl_version == -1) {
        glsl_version = 330;
        glsl.insert(0, "#version 330\n");
    } else if (glsl_version < 330) {
        // force upgrade glsl version
        glsl = replace_line_starting_with(glsl, "#version", "#version 330\n");
        glsl_version = 330;
    }
    LOG_D("GLSL version: %d",glsl_version)
    return glsl_version;
}

std::vector<unsigned int> glsl_to_spirv(GLenum shader_type, int glsl_version, const char * const *shader_src, int& errc) {
    
    static shaderc_compiler_t compiler = nullptr;
    if(compiler == nullptr) {
        printf("shaderc\n");
        compiler = shaderc_compiler_initialize();
        if(compiler == nullptr) {
            printf("Error: shaderc compiler cannot be created!\n");
            errc = -1;
            return {};
        }
    }

    shaderc_compile_options_t opts = shaderc_compile_options_initialize();
    shaderc_compile_options_set_forced_version_profile(opts, 450, shaderc_profile_core);
    shaderc_compile_options_set_auto_map_locations(opts, true);
    shaderc_compile_options_set_auto_bind_uniforms(opts, true);
    shaderc_compile_options_set_target_env(opts, shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);

    shaderc_compile_options_add_macro_definition(opts, "noperspective", strlen("noperspective"), "highp", strlen("highp"));

    shaderc_compile_options_set_optimization_level(opts, shaderc_optimization_level_performance);

    shaderc_compilation_result_t optimized_glsl_res = shaderc_compile_into_preprocessed_text(
        compiler, 
        *shader_src,
        strlen(*shader_src),
        shader_type == GL_VERTEX_SHADER ? shaderc_glsl_vertex_shader : 
        shader_type == GL_FRAGMENT_SHADER ? shaderc_glsl_fragment_shader :
        shader_type == GL_COMPUTE_SHADER ? shaderc_glsl_compute_shader :
        shader_type == GL_GEOMETRY_SHADER ? shaderc_glsl_geometry_shader :
        shader_type == GL_TESS_CONTROL_SHADER ? shaderc_glsl_tess_control_shader :
        shader_type == GL_TESS_EVALUATION_SHADER ? shaderc_glsl_tess_evaluation_shader :
        shaderc_glsl_infer_from_source,
        "optimized_shader", "main", opts);

    if(shaderc_result_get_compilation_status(optimized_glsl_res) != shaderc_compilation_status_success) {
        printf("There is a problem with shaderc！\n%s\n", shaderc_result_get_error_message(optimized_glsl_res));
        shaderc_result_release(optimized_glsl_res);
        errc = -1;
        return {};
    }

    const char* optimized_glsl = shaderc_result_get_bytes(optimized_glsl_res);
    size_t optimized_glsl_length = shaderc_result_get_length(optimized_glsl_res);
    
    EShLanguage shader_language;
    switch (shader_type) {
        case GL_VERTEX_SHADER:
            shader_language = EShLanguage::EShLangVertex;
            break;
        case GL_FRAGMENT_SHADER:
            shader_language = EShLanguage::EShLangFragment;
            break;
        case GL_COMPUTE_SHADER:
            shader_language = EShLanguage::EShLangCompute;
            break;
        case GL_TESS_CONTROL_SHADER:
            shader_language = EShLanguage::EShLangTessControl;
            break;
        case GL_TESS_EVALUATION_SHADER:
            shader_language = EShLanguage::EShLangTessEvaluation;
            break;
        case GL_GEOMETRY_SHADER:
            shader_language = EShLanguage::EShLangGeometry;
            break;
        default:
            LOG_D("GLSL type not supported!")
            shaderc_result_release(optimized_glsl_res);
            errc = -1;
            return {};
    }

    glslang::TShader shader(shader_language);
    shader.setStrings(&optimized_glsl, 1);

    using namespace glslang;
    shader.setEnvInput(EShSourceGlsl, shader_language, EShClientOpenGL, glsl_version);
    shader.setEnvClient(EShClientOpenGL, EShTargetOpenGL_450);
    shader.setEnvTarget(EShTargetSpv, EShTargetSpv_1_6);
    shader.setAutoMapLocations(true);
    shader.setAutoMapBindings(true);

    TBuiltInResource TBuiltInResource_resources = InitResources();

    if (!shader.parse(&TBuiltInResource_resources, glsl_version, true, EShMsgDefault)) {
        LOG_E("GLSL Compiling ERROR: \n%s",shader.getInfoLog())
        shaderc_result_release(optimized_glsl_res);
        errc = -1;
        return {};
    }
    LOG_D("GLSL Compiled.")

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(EShMsgDefault)) {
        LOG_E("Shader Linking ERROR: %s", program.getInfoLog())
        shaderc_result_release(optimized_glsl_res);
        errc = -1;
        return {};
    }
    LOG_D("Shader Linked." )
    std::vector<unsigned int> spirv_code;
    glslang::SpvOptions spvOptions;
    spvOptions.disableOptimizer = false;
    glslang::GlslangToSpv(*program.getIntermediate(shader_language), spirv_code, &spvOptions);
    shaderc_result_release(optimized_glsl_res);
    errc = 0;
    return spirv_code;
}

std::string spirv_to_essl(std::vector<unsigned int> spirv, uint essl_version, int& errc) {
    spvc_context context = nullptr;
    spvc_parsed_ir ir = nullptr;
    spvc_compiler compiler_glsl = nullptr;
    spvc_compiler_options options = nullptr;
    spvc_resources resources = nullptr;
    const spvc_reflected_resource *list = nullptr;
    const char *result = nullptr;
    size_t count;

    const SpvId *p_spirv = spirv.data();
    size_t word_count = spirv.size();

    LOG_D("spirv_code.size(): %d", spirv.size())
    if(context == nullptr) {
        spvc_context_create(&context);
        if(context == nullptr) {
            printf("SPVC Context could not be created!\n");
        }
    }
    spvc_context_parse_spirv(context, p_spirv, word_count, &ir);
    spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler_glsl);
    spvc_compiler_create_shader_resources(compiler_glsl, &resources);
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &list, &count);
    spvc_compiler_create_compiler_options(compiler_glsl, &options);
    spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_GLSL_VERSION, essl_version >= 320 ? essl_version : 320);
    spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_ES, SPVC_TRUE);
    spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_VULKAN_SEMANTICS, SPVC_FALSE);
    spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_EXTENSIONS_DISABLE, SPVC_TRUE);
    spvc_compiler_install_compiler_options(compiler_glsl, options);
    spvc_compiler_compile(compiler_glsl, &result);

    if (!result) {
        const char* error_msg = spvc_context_get_last_error_string(context);
        if (error_msg) {
            LOG_E("SPIRV-Cross error: %s", error_msg);
        } else {
            LOG_E("SPIRV-Cross failed without error message");
        }
        return "";
    }

    std::string essl = result;

    spvc_context_destroy(context);

    errc = 0;
    return essl;
}

static bool glslang_inited = false;
std::string GLSLtoGLSLES_2(const char *glsl_code, GLenum glsl_type, uint essl_version, int& return_code) {
#ifdef FEATURE_PRE_CONVERTED_GLSL
    if (getGLSLVersion(glsl_code) == 430) {
        char* converted = preConvertedGlsl(glsl_code);
        if (converted) {
            LOG_D("Find pre-converted glsl, now use it.")
            return converted;
        }
    }
#endif
    std::string correct_glsl_str = preprocess_glsl(glsl_code, glsl_type);
    LOG_D("Firstly converted GLSL:\n%s", correct_glsl_str.c_str())
    int glsl_version = get_or_add_glsl_version(correct_glsl_str);

    LOG_D("Firstly converted GLSL:\n%s", correct_glsl_str.c_str())

    if (!glslang_inited) {
        glslang::InitializeProcess();
        glslang_inited = true;
    }
    const char* s[] = { correct_glsl_str.c_str() };
    int errc = 0;
    std::vector<unsigned int> spirv_code = glsl_to_spirv(glsl_type, glsl_version, s, errc);
    if (errc != 0) {
        return_code = -1;
        return "";
    }
    errc = 0;
    std::string essl = spirv_to_essl(spirv_code, essl_version, errc);
    if (errc != 0) {
        return_code = -2;
        return "";
    }

    // Post-processing ESSL
    essl = removeLayoutBinding(essl);
    essl = processOutColorLocations(essl);
    essl = forceSupporterOutput(essl);

    LOG_D("Originally GLSL to GLSL ES Complete: \n%s", essl.c_str())

    return_code = errc;
    return essl;
}

std::string GLSLtoGLSLES_1(const char *glsl_code, GLenum glsl_type, uint esversion, int& return_code) {
#if !defined(__APPLE__)
    LOG_W("Warning: use glsl optimizer to convert shader.")
    if (esversion < 320) esversion = 320;
    std::string result = MesaConvertShader(glsl_code, glsl_type == GL_VERTEX_SHADER ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER, 460LL, esversion);

    return_code = 0;
    return result;
#else
    LOG_W_FORCE("Cannot convert glsl with version %d in MacOS/iOS", esversion);
    return std::string(glsl_code);
#endif
}
