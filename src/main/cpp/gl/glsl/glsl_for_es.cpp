#include "glsl_for_es.h"

#include <spirv_cross/spirv_cross_c.h>
#include <iostream>
#include <fstream>
#include "../log.h"
#include <string>
#include <regex>
#include <strstream>
#include <algorithm>
#include <sstream>
#include "cache.h"
#include "../../version.h"

#include <shaderc/shader.h>
#include <vector>
#include <map>
#include <set>
#include "../../includes.h"
#include <glslang/Public/ShaderLang.h>
#include <glslang/Include/Types.h>
#include <glslang/Public/ShaderLang.h>
#include "glslang/SPIRV/GlslangToSpv.h"

#define DEBUG 1

const char* atomicCounterEmulatedWatermark = "// Non-opaque atomic uniform converted to SSBO";

#if !defined(__APPLE__)
extern std::string MesaConvertShader (const char *src, GLenum type, unsigned int glsl, unsigned int essl);
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

bool checkIfAtomicCounterBufferEmulated(const std::string& glslCode) {
    return glslCode.find(atomicCounterEmulatedWatermark) != std::string::npos;
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

std::string GLSLtoGLSLES(const char* glsl_code, GLenum glsl_type, uint essl_version, uint glsl_version, int& return_code) {
    std::string sha256_string(glsl_code);
    sha256_string += "\n//" + std::to_string(MAJOR) + "." + std::to_string(MINOR) + "." + std::to_string(REVISION) + "|" + std::to_string(essl_version);
    const char* cachedESSL = Cache::get_instance().get(sha256_string.c_str());
    if (cachedESSL) {
        LOG_D("GLSL Hit Cache:\n%s\n-->\n%s", glsl_code, cachedESSL)
		bool atomicCounterEmulated = checkIfAtomicCounterBufferEmulated(std::string(cachedESSL));
        return_code = atomicCounterEmulated ? 1 : 0;
        return (char*)cachedESSL;
    }
    
    return_code = -1;
    std::string converted = /*glsl_version<140? GLSLtoGLSLES_1(glsl_code, glsl_type, essl_version, return_code):*/GLSLtoGLSLES_2(glsl_code, glsl_type, essl_version, return_code);
    if (return_code >= 0 && !converted.empty()) {
        converted = process_uniform_declarations(converted);
        Cache::get_instance().put(sha256_string.c_str(), converted.c_str());
    }

    return (return_code >= 0) ? converted : glsl_code;
}

std::string replace_line_starting_with(const std::string& glslCode, const std::string& starting, const std::string& substitution = "") {
    std::string result;
    size_t length = glslCode.size();
    size_t start = 0;
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

static size_t find_insertion_point(const std::string& glsl) {
    size_t pos = 0;
    size_t insertion_point = 0;

    size_t version_pos = glsl.find("#version");
    if (version_pos != std::string::npos) {
        size_t version_end = glsl.find('\n', version_pos);
        if (version_end == std::string::npos) {
            version_end = glsl.length();
        }
        else {
            version_end++;
        }
        insertion_point = version_end;
        pos = version_end;
    }
    else {
        insertion_point = 0;
        pos = 0;
    }

    while (pos < glsl.length()) {
        // size_t line_begin = pos;
        while (pos < glsl.length() && std::isspace(glsl[pos])) {
            pos++;
        }
        if (pos >= glsl.length()) break;

        if (glsl[pos] == '#') {
            pos++;
            while (pos < glsl.length() && std::isspace(glsl[pos])) {
                pos++;
            }
            if (glsl.compare(pos, 9, "extension") == 0) {
                size_t ext_end = glsl.find('\n', pos);
                if (ext_end == std::string::npos) {
                    ext_end = glsl.length();
                } else {
                    ext_end++;
                }
                insertion_point = ext_end;
                pos = ext_end;
            } else {
                break;
            }
        } else {
            break;
        }
    }

    return insertion_point;
}

bool process_non_opaque_atomic_to_ssbo(std::string& source) {
    if (source.find("atomicCounter") == std::string::npos) return false;

    std::set<std::string> atomic_vars;
    std::map<std::string, std::string> binding_map;
    std::regex decl_rx(
        R"(layout\s*\(\s*binding\s*=\s*(\d+)\s*(?:,\s*offset\s*=\s*(\d+)\s*)?\)\s*uniform\s+atomic_uint\s+(\w+)\s*;)",
        std::regex::icase
    );

    std::smatch m;
    auto it = source.cbegin();
    while (std::regex_search(it, source.cend(), m, decl_rx)) {
        size_t prefix = std::distance(source.cbegin(), it);
        size_t match_pos = prefix + m.position(0);
        size_t match_len = m.length(0);

        std::string binding = m[1].str();
        std::string var = m[3].str();
        atomic_vars.insert(var);
        binding_map[var] = binding;

        std::string repl =
            "layout(std430, binding=" + binding + ") buffer AtomicCounterSSBO_" + binding + " {\n"
            "    uint " + var + ";\n"
            "};\n";
        source.replace(match_pos, match_len, repl);

        it = source.cbegin() + match_pos + repl.size();
    }

    if (atomic_vars.empty()) return true;

    for (auto& var : atomic_vars) {
        source = std::regex_replace(source,
            std::regex(R"(\batomicCounterIncrement\s*\(\s*)" + var + R"(\s*\))", std::regex::icase),
            "atomicAdd(" + var + ", 1u)"
        );
        source = std::regex_replace(source,
            std::regex(R"(\batomicCounterDecrement\s*\(\s*)" + var + R"(\s*\))", std::regex::icase),
            "atomicAdd(" + var + ", uint(-1))"
        );
        source = std::regex_replace(source,
            std::regex(R"(\batomicCounterAdd\s*\(\s*)" + var + R"(\s*,\s*([^)]+)\s*\))", std::regex::icase),
            "atomicAdd(" + var + ", $1)"
        );
        source = std::regex_replace(source,
            std::regex(R"(\batomicCounter\s*\(\s*)" + var + R"(\s*\))", std::regex::icase),
            var
        );
    }

	// insert memoryBarrierBuffer
    {
        std::regex rx_barrier(
            R"(([ \t]*\batomicAdd\b[^;]*;))",
            std::regex::icase
        );

        std::set<size_t> processed_positions;
        std::string result;
        size_t last_pos = 0;

        for (auto it = std::sregex_iterator(source.begin(), source.end(), rx_barrier);
            it != std::sregex_iterator(); ++it) {

            size_t start_pos = it->position();
            size_t end_pos = start_pos + it->length();

            if (processed_positions.find(start_pos) != processed_positions.end()) {
                continue;
            }

            result += source.substr(last_pos, start_pos - last_pos);

            std::string matched_stmt = it->str();
            result += matched_stmt;

            result += "\n    memoryBarrierBuffer();";

            processed_positions.insert(start_pos);
            last_pos = end_pos;
        }

        result += source.substr(last_pos);
        source = result;
    }

    source += "\n" + std::string(atomicCounterEmulatedWatermark);
    return true;
}

void process_sampler_buffer(std::string& source) { // a simplized version, should be rewritten in the future
    if (source.find("isamplerBuffer") == std::string::npos) {
        return;
    }

    size_t pos = 0;
    while ((pos = source.find("isamplerBuffer", pos)) != std::string::npos) {
        source.replace(pos, 14, "isampler2D");
        pos += 11;
    }

    std::regex pattern(R"(texelFetch\s*\(\s*(\w+)\s*,\s*([^)]+?)\s*\))");
    source = std::regex_replace(
        source,
        pattern,
        "texelFetch($1, ivec2(($2) % u_BufferTexWidth, ($2) / u_BufferTexWidth), 0)"
    );

    const char* boundaryProtection = R"(
ivec2 bufferCoords(int index) {
    int width = u_BufferTexWidth;
    int x = index % width;
    int y = index / width;
    if (y >= u_BufferTexHeight) {
        y = u_BufferTexHeight - 1;
        x = width - 1;
    }
    return ivec2(x, y);
}
)";

    source = std::regex_replace(
        source,
        std::regex("texelFetch\\((\\w+)\\s*,\\s*ivec2\\(([^)]+)\\)\\s*,\\s*0\\)"),
        "texelFetch($1, bufferCoords($2), 0)"
    );

    size_t insertion_point = find_insertion_point(source);
    if (insertion_point != std::string::npos) {
        source.insert(insertion_point, boundaryProtection);
    }

    const char* uniformDecl = R"(
uniform int u_BufferTexWidth;
uniform int u_BufferTexHeight;
)";

    insertion_point = find_insertion_point(source);
    if (insertion_point != std::string::npos) {
        insertion_point = source.find('\n', insertion_point);
        if (insertion_point != std::string::npos) {
            source.insert(insertion_point + 1, uniformDecl);
        }
    }
}

static void inject_textureQueryLod(std::string& glsl) {
    const std::regex defRegex(R"(vec2\s+mg_textureQueryLod\s*\()", std::regex::ECMAScript);

    if (glsl.find("textureQueryLod") == std::string::npos) {
        return;
    }
    if (std::regex_search(glsl, defRegex)) {
        return;
    }

    const std::string textureQueryLodImpl = R"(
#define textureQueryLod mg_textureQueryLod

vec2 mg_textureQueryLod(sampler2D tex, vec2 uv) {
    vec2 texSizeF = vec2(textureSize(tex, 0));
    vec2 dFdx_uv = dFdx(uv * texSizeF);
    vec2 dFdy_uv = dFdy(uv * texSizeF);
    float maxDerivative = max(length(dFdx_uv), length(dFdy_uv));
    float lod = log2(maxDerivative);
    return vec2(lod);
}
)";

    size_t insertPos = find_insertion_point(glsl);
    glsl.insert(insertPos, "\n" + textureQueryLodImpl + "\n");
}

static void inject_image2D_declarations(std::string& glsl) {
    const std::regex defRegex(R"(layout\s*\(\s*rgba16f\s*\)\s+writeonly\s+restrict\s+uniform\s+image2D\s*;)", std::regex::ECMAScript);
    const std::regex defRegex_one(R"(layout\s*\(\s*rgba8f\s*\)\s+writeonly\s+restrict\s+uniform\s+image2D\s*;)", std::regex::ECMAScript);
    const std::regex defRegex_two(R"(layout\s*\(\s*rgba16f\s*\)\s+writeonly\s+uniform\s+image2D\s*;)", std::regex::ECMAScript);
    const std::regex defRegex_three(R"(layout\s*\(\s*rgba16f\s*\)\s+restrict\s+uniform\s+image2D\s*;)", std::regex::ECMAScript);
    const std::regex defRegex_four(R"(layout\s*\(\s*rgba8f\s*\)\s+writeonly\s+uniform\s+image2D\s*;)", std::regex::ECMAScript);
    const std::regex defRegex_five(R"(layout\s*\(\s*rgba8f\s*\)\s+restrict\s+uniform\s+image2D\s*;)", std::regex::ECMAScript);
    const std::regex defRegex_six(R"(layout\s*\(\s*r32f\s*\)\s+restrict\s+uniform\s+image2D\s*;)", std::regex::ECMAScript);

    if (glsl.find("uniform image2D") == std::string::npos) {
        return;
    }

    if (std::regex_search(glsl, defRegex) && std::regex_search(glsl, defRegex_one) && std::regex_search(glsl, defRegex_two) && std::regex_search(glsl, defRegex_three) && std::regex_search(glsl, defRegex_four) && std::regex_search(glsl, defRegex_five)) {
        return;
    } else {
        replace_all(glsl, "restrict uniform image2D", "layout (r32f) restrict uniform image2D");
        replace_all(glsl, "writeonly restrict uniform image2D", "layout (rgba16f) writeonly restrict uniform image2D");
        replace_all(glsl, "writeonly uniform image2D", "layout (rgba16f) writeonly uniform image2D");
    }

}

static void inject_gl_DepthRange(std::string& glsl) {
   const std::regex defRegex(R"(uniform\s+mg_DepthRangeParameters\s+mg_DepthRange\s*;)", std::regex::ECMAScript);

    if (glsl.find("gl_DepthRange") == std::string::npos) {
        return;
    }
    if (std::regex_search(glsl, defRegex)) {
        return;
    }

    replace_all(glsl, "gl_DepthRange", "mg_DepthRange");
    const std::string gl_DepthRangeImpl = R"(
struct mg_DepthRangeParameters {
    float near;
    float far;
    float diff;
};
uniform mg_DepthRangeParameters mg_DepthRange;
)";

    size_t insertPos = find_insertion_point(glsl);
    glsl.insert(insertPos, "\n" + gl_DepthRangeImpl + "\n");

}

static void inject_subgroup_BigGiftPackage(std::string& glsl) {
    const std::regex defRegex(R"(#define\s+SUBGROUP_SIZE\s+\d+)", std::regex::ECMAScript);

    if (glsl.find("subgroupBallot") == std::string::npos && 
        glsl.find("activeMask") == std::string::npos &&
        glsl.find("gl_SubgroupInvocationID") == std::string::npos &&
        glsl.find("subgroupAdd") == std::string::npos &&
        glsl.find("gl_NumSubgroups") == std::string::npos &&
        glsl.find("gl_SubgroupID") == std::string::npos &&
        glsl.find("subgroupAny") == std::string::npos &&
        glsl.find("subgroupAll") == std::string::npos &&
        glsl.find("subgroupElect") == std::string::npos) {
        return;
    }

    if (std::regex_search(glsl, defRegex)) {
        return;
    }

    replace_all(glsl, "#extension GL_KHR_shader_subgroup", "// #extension GL_KHR_shader_subgroup");
    replace_all(glsl, "#extension GL_KHR_shader_subgroup_basic", "// #extension GL_KHR_shader_subgroup_basic");
    replace_all(glsl, "#extension GL_KHR_shader_subgroup_ballot", "// #extension GL_KHR_shader_subgroup_ballot");
    replace_all(glsl, "#extension GL_KHR_shader_subgroup_arithmetic", "// #extension GL_KHR_shader_subgroup_arithmetic");
    replace_all(glsl, "#extension GL_KHR_shader_subgroup_clustered", "// #extension GL_KHR_shader_subgroup_clustered");
    replace_all(glsl, "subgroupBallot", "mg_subgroupBallot");
    replace_all(glsl, "activeMask", "mg_activeMask");
    replace_all(glsl, "gl_SubgroupInvocationID", "mg_gl_SubgroupInvocationID()");
    replace_all(glsl, "subgroupAdd", "mg_subgroupAdd");
    replace_all(glsl, "gl_NumSubgroups", "mg_gl_NumSubgroups()");
    replace_all(glsl, "gl_SubgroupID", "mg_gl_SubgroupID()");
    replace_all(glsl, "subgroupAny", "mg_subgroupAny");
    replace_all(glsl, "subgroupAll", "mg_subgroupAll");
    replace_all(glsl, "subgroupElect", "mg_subgroupElect");

    const std::string subgroup_BigGiftPackageImpl = R"(
#define SUBGROUP_SIZE 32

// ==================== 基础子组模拟 (核心功能) ====================
// 使用内置变量模拟子组基础功能
uint mg_gl_SubgroupID() {
    // 通过工作组内线性索引计算子组ID
    return gl_LocalInvocationIndex / SUBGROUP_SIZE;
}

uint mg_gl_SubgroupInvocationID() {
    // 子组内调用索引 = 局部线性索引 % 子组大小
    return gl_LocalInvocationIndex % SUBGROUP_SIZE;
}

uint mg_gl_NumSubgroups() {
    // 子组总数 = 工作组大小 / 子组大小（向上取整）
    return (gl_WorkGroupSize.x * gl_WorkGroupSize.y * gl_WorkGroupSize.z + SUBGROUP_SIZE - 1u) / SUBGROUP_SIZE;
}

// ==================== 投票功能模拟 (共享内存实现) ====================
shared uint s_ballot; // 共享存储用于投票结果

uvec2 mg_subgroupBallot(bool condition) {
    // Step 1: 初始化共享变量
    if (gl_LocalInvocationID.x == 0 && gl_LocalInvocationID.y == 0 && gl_LocalInvocationID.z == 0) {
        s_ballot = 0u;
    }
    memoryBarrierShared();
    barrier();

    // Step 2: 原子操作设置比特位
    if (condition) {
        uint mask = 1u << mg_gl_SubgroupInvocationID();
        atomicOr(s_ballot, mask);
    }
    memoryBarrierShared();
    barrier();

    // Step 3: 返回结果（兼容uvec2结构）
    return uvec2(s_ballot, 0u); // 高位始终为0
}

uvec2 activeMask() {
    // 所有活跃线程返回true
    return mg_subgroupBallot(true);
}

// ==================== 条件判断辅助 ====================
bool mg_subgroupAny(bool value) {
    uvec2 mask = mg_subgroupBallot(value);
    return mask.x != 0u;
}

bool mg_subgroupAll(bool value) {
    uvec2 fullMask = activeMask();
    uvec2 valueMask = mg_subgroupBallot(value);
    return fullMask == valueMask;
}


// ==================== 全局共享内存声明 ====================
const uint total_workgroup_size = gl_WorkGroupSize.x * gl_WorkGroupSize.y * gl_WorkGroupSize.z;
const uint num_subgroups = (total_workgroup_size + SUBGROUP_SIZE - 1u) / SUBGROUP_SIZE;

// 标量类型共享内存
shared float s_reduceAdd_float[num_subgroups * SUBGROUP_SIZE];
shared uint s_reduceAdd_uint[num_subgroups * SUBGROUP_SIZE];
shared int s_reduceAdd_int[num_subgroups * SUBGROUP_SIZE];

// 向量类型共享内存
shared vec2 s_reduceAdd_vec2[num_subgroups * SUBGROUP_SIZE];
shared vec3 s_reduceAdd_vec3[num_subgroups * SUBGROUP_SIZE];
shared vec4 s_reduceAdd_vec4[num_subgroups * SUBGROUP_SIZE];

// ==================== 子组加法归约模拟 ====================
float mg_subgroupAdd(float value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    // 初始化共享内存
    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_reduceAdd_float[i] = 0.0;
    }
    memoryBarrierShared();
    barrier();

    s_reduceAdd_float[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = SUBGROUP_SIZE / 2; stride > 0; stride >>= 1) {
        if (laneID < stride) {
            s_reduceAdd_float[offset] += s_reduceAdd_float[offset + stride];
        }
        memoryBarrierShared();
        barrier();
    }

    return s_reduceAdd_float[subgroupID * SUBGROUP_SIZE];
}

uint mg_subgroupAdd(uint value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_reduceAdd_uint[i] = 0u;
    }
    memoryBarrierShared();
    barrier();

    s_reduceAdd_uint[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = SUBGROUP_SIZE / 2; stride > 0; stride >>= 1) {
        if (laneID < stride) {
            s_reduceAdd_uint[offset] += s_reduceAdd_uint[offset + stride];
        }
        memoryBarrierShared();
        barrier();
    }

    return s_reduceAdd_uint[subgroupID * SUBGROUP_SIZE];
}

int mg_subgroupAdd(int value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_reduceAdd_int[i] = 0;
    }
    memoryBarrierShared();
    barrier();

    s_reduceAdd_int[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = SUBGROUP_SIZE / 2; stride > 0; stride >>= 1) {
        if (laneID < stride) {
            s_reduceAdd_int[offset] += s_reduceAdd_int[offset + stride];
        }
        memoryBarrierShared();
        barrier();
    }

    return s_reduceAdd_int[subgroupID * SUBGROUP_SIZE];
}

vec2 mg_subgroupAdd(vec2 value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_reduceAdd_vec2[i] = vec2(0.0);
    }
    memoryBarrierShared();
    barrier();

    s_reduceAdd_vec2[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = SUBGROUP_SIZE / 2; stride > 0; stride >>= 1) {
        if (laneID < stride) {
            s_reduceAdd_vec2[offset] += s_reduceAdd_vec2[offset + stride];
        }
        memoryBarrierShared();
        barrier();
    }

    return s_reduceAdd_vec2[subgroupID * SUBGROUP_SIZE];
}

vec3 mg_subgroupAdd(vec3 value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_reduceAdd_vec3[i] = vec3(0.0);
    }
    memoryBarrierShared();
    barrier();

    s_reduceAdd_vec3[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = SUBGROUP_SIZE / 2; stride > 0; stride >>= 1) {
        if (laneID < stride) {
            s_reduceAdd_vec3[offset] += s_reduceAdd_vec3[offset + stride];
        }
        memoryBarrierShared();
        barrier();
    }

    return s_reduceAdd_vec3[subgroupID * SUBGROUP_SIZE];
}

vec4 mg_subgroupAdd(vec4 value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_reduceAdd_vec4[i] = vec4(0.0);
    }
    memoryBarrierShared();
    barrier();

    s_reduceAdd_vec4[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = SUBGROUP_SIZE / 2; stride > 0; stride >>= 1) {
        if (laneID < stride) {
            s_reduceAdd_vec4[offset] += s_reduceAdd_vec4[offset + stride];
        }
        memoryBarrierShared();
        barrier();
    }

    return s_reduceAdd_vec4[subgroupID * SUBGROUP_SIZE];
}

bool mg_subgroupElect() {
    // 子组内第一个线程（调用ID=0）被选为领导线程
    return (mg_gl_SubgroupInvocationID() == 0u);
}

)";

    size_t insertPos = find_insertion_point(glsl);
    glsl.insert(insertPos, "\n" + subgroup_BigGiftPackageImpl + "\n");

}

static void inject_subgroup_clustered(std::string& glsl) {
    const std::regex defRegex(R"(shared\s+uint\s+_cluster_shared_data\s*\[\s*gl_WorkGroupSize\.x\s*\*\s*gl_WorkGroupSize\.y\s*\*\s*gl_WorkGroupSize\.z\s*\]\s*;)", std::regex::ECMAScript);

    // 检查是否使用了扩展中的任何标识符
    if (glsl.find("subgroupClusteredMax") == std::string::npos && 
        glsl.find("subgroupMemoryBarrier") == std::string::npos && 
        glsl.find("subgroupBarrier") == std::string::npos &&
        glsl.find("subgroupClusteredAllEqual") == std::string::npos &&
        glsl.find("subgroupClusteredAny") == std::string::npos &&
        glsl.find("subgroupClusteredAll") == std::string::npos &&
        glsl.find("subgroupClusteredXor") == std::string::npos &&
        glsl.find("subgroupClusteredOr") == std::string::npos &&
        glsl.find("subgroupClusteredAnd") == std::string::npos &&
        glsl.find("subgroupClusteredMin") == std::string::npos &&
        glsl.find("subgroupClusteredMul") == std::string::npos &&
        glsl.find("subgroupClusteredAdd") == std::string::npos)
    {
        return;
    }

    if (std::regex_search(glsl, defRegex)) {
        return;
    }

    replace_all(glsl, "#extension GL_KHR_shader_subgroup_clustered :enable", "// #extension GL_KHR_shader_subgroup_clustered :enable");
    replace_all(glsl, "#extension GL_KHR_shader_subgroup_clustered : enable", "// #extension GL_KHR_shader_subgroup_clustered : enable");
    replace_all(glsl, "#extension GL_KHR_shader_subgroup_clustered: enable", "// #extension GL_KHR_shader_subgroup_clustered: enable");
    replace_all(glsl, "#extension GL_KHR_shader_subgroup_clustered: require", "// #extension GL_KHR_shader_subgroup_clustered : require");
    replace_all(glsl, "#extension GL_KHR_shader_subgroup_clustered : require", "// #extension GL_KHR_shader_subgroup_clustered : require");
    replace_all(glsl, "#extension GL_KHR_shader_subgroup_clustered :require", "// #extension GL_KHR_shader_subgroup_clustered : require"); //防止编译错误
    replace_all(glsl, "subgroupClusteredMax", "mg_subgroupClusteredMax"); //防止编译错误
    replace_all(glsl, "subgroupMemoryBarrier", "mg_subgroupMemoryBarrier"); //防止编译错误
    replace_all(glsl, "subgroupBarrier", "mg_subgroupBarrier"); //防止编译错误
    replace_all(glsl, "subgroupClusteredAllEqual", "mg_subgroupClusteredAllEqual"); //防止编译错误
    replace_all(glsl, "subgroupClusteredAny", "mg_subgroupClusteredAny"); //防止编译错误
    replace_all(glsl, "subgroupClusteredAll", "mg_subgroupClusteredAll"); //防止编译错误
    replace_all(glsl, "subgroupClusteredXor", "mg_subgroupClusteredXor"); //防止编译错误
    replace_all(glsl, "subgroupClusteredOr", "mg_subgroupClusteredOr"); //防止编译错误
    replace_all(glsl, "subgroupClusteredAnd", "mg_subgroupClusteredAnd"); //防止编译错误
    replace_all(glsl, "subgroupClusteredMin", "mg_subgroupClusteredMin"); //防止编译错误
    replace_all(glsl, "subgroupClusteredMul", "mg_subgroupClusteredMul"); //防止编译错误
    replace_all(glsl, "subgroupClusteredAdd", "mg_subgroupClusteredAdd"); //防止编译错误

    const std::string subgroup_clusteredImpl = R"(
precision highp float;
precision highp int;

// 共享内存用于线程间通信
shared uint _cluster_shared_data[gl_WorkGroupSize.x * gl_WorkGroupSize.y * gl_WorkGroupSize.z];

// 一维索引计算（假设工作组为一维）
uint _get_linear_index() {
    return gl_LocalInvocationID.x;
}

// ================== 核心归约函数模板 ==================
uint _clustered_reduce(uint value, uint clusterSize, uint op) {
    uint idx = _get_linear_index();
    uint clusterIdx = idx / clusterSize;
    uint offset = idx % clusterSize;
    uint base = clusterIdx * clusterSize;

    // 存储原始值到共享内存
    _cluster_shared_data[idx] = value;
    barrier();
    memoryBarrierShared();

    // 归约循环（要求clusterSize是2的幂）
    for (uint stride = 1; stride < clusterSize; stride *= 2) {
        if ((offset & (2u * stride - 1u)) == 0u) {
            uint otherIdx = idx + stride;
            if (offset + stride < clusterSize) {
                uint a = _cluster_shared_data[idx];
                uint b = _cluster_shared_data[otherIdx];
                
                // 根据操作类型执行计算
                switch (op) {
                    case 0:  a += b; break;    // Add
                    case 1:  a *= b; break;    // Mul
                    case 2:  a = min(a, b); break; // Min
                    case 3:  a = max(a, b); break; // Max
                    case 4:  a &= b; break;    // And
                    case 5:  a |= b; break;    // Or
                    case 6:  a ^= b; break;    // Xor
                    default: break;
                }
                _cluster_shared_data[idx] = a;
            }
        }
        barrier();
        memoryBarrierShared();
    }
    return _cluster_shared_data[base]; // 返回归约结果
}

// ================== 算术操作实现 ==================
float mg_subgroupClusteredAdd(float val, uint clusterSize) {
    uint u = floatBitsToUint(val);
    u = _clustered_reduce(u, clusterSize, 0);
    return uintBitsToFloat(u);
}

float mg_subgroupClusteredMul(float val, uint clusterSize) {
    uint u = floatBitsToUint(val);
    u = _clustered_reduce(u, clusterSize, 1);
    return uintBitsToFloat(u);
}

float mg_subgroupClusteredMin(float val, uint clusterSize) {
    uint u = floatBitsToUint(val);
    u = _clustered_reduce(u, clusterSize, 2);
    return uintBitsToFloat(u);
}

float mg_subgroupClusteredMax(float val, uint clusterSize) {
    uint u = floatBitsToUint(val);
    u = _clustered_reduce(u, clusterSize, 3);
    return uintBitsToFloat(u);
}

// ================== 按位操作实现 ==================
uint mg_subgroupClusteredAnd(uint val, uint clusterSize) {
    return _clustered_reduce(val, clusterSize, 4);
}

uint mg_subgroupClusteredOr(uint val, uint clusterSize) {
    return _clustered_reduce(val, clusterSize, 5);
}

uint mg_subgroupClusteredXor(uint val, uint clusterSize) {
    return _clustered_reduce(val, clusterSize, 6);
}

// ================== 投票操作实现 ==================
bool mg_subgroupClusteredAll(bool condition, uint clusterSize) {
    uint val = condition ? 0xFFFFFFFFu : 0u;
    uint result = _clustered_reduce(val, clusterSize, 4);
    return (result == 0xFFFFFFFFu);
}

bool mg_subgroupClusteredAny(bool condition, uint clusterSize) {
    uint val = condition ? 0xFFFFFFFFu : 0u;
    uint result = _clustered_reduce(val, clusterSize, 5);
    return (result != 0u);
}

bool mg_subgroupClusteredAllEqual(float value, uint clusterSize) {
    uint idx = _get_linear_index();
    uint clusterIdx = idx / clusterSize;
    uint offset = idx % clusterSize;
    uint base = clusterIdx * clusterSize;

    // 存储原始值
    _cluster_shared_data[idx] = floatBitsToUint(value);
    barrier();
    memoryBarrierShared();

    // 获取第一个元素作为参考
    uint ref = _cluster_shared_data[base];
    
    // 检查所有元素是否等于参考值
    bool equal = (floatBitsToUint(value) == ref);
    uint u = equal ? 0xFFFFFFFFu : 0u;
    uint result = _clustered_reduce(u, clusterSize, 4);
    
    return (result == 0xFFFFFFFFu);
}

// ================== 同步操作 ==================
void mg_subgroupBarrier() {
    barrier();
    memoryBarrierShared();
}

void mg_subgroupMemoryBarrier() {
    memoryBarrierShared();
}
)";

    size_t insertPos = find_insertion_point(glsl);
    glsl.insert(insertPos, "\n" + subgroup_clusteredImpl + "\n");
}

static void inject_shaderDrawParameters(std::string& glsl) {
    const std::regex defRegex(R"(#extension GL_ARB_shader_draw_parameters : enable)", std::regex::ECMAScript);

    // 检查是否使用了扩展中的任何标识符
    if (glsl.find("gl_DrawID") == std::string::npos && 
        glsl.find("gl_DrawIDARB") == std::string::npos && 
        glsl.find("gl_BaseInstanceARB") == std::string::npos &&
        glsl.find("gl_BaseVertexARB") == std::string::npos) {
        return;
    }
    if (std::regex_search(glsl, defRegex)) {
        return;
    }

    const std::string drawParametersImpl = R"(
#extension GL_ARB_shader_draw_parameters : enable
)";

    size_t insertPos = find_insertion_point(glsl);
    glsl.insert(insertPos, "\n" + drawParametersImpl + "\n");
}

static void inject_gl_Vertex(std::string& glsl) {
   const std::regex vertexRegex(R"(\bgl_Vertex\b)"); // 已全词匹配

    // 修改后：mg_Vertex前后均添加单词边界
    const std::regex defRegex(R"(\b(in)\s+(vec4)\s+(mg_Vertex)\b\s*;)");

    if (!std::regex_search(glsl, vertexRegex)) {
        return; // 没有使用 gl_Vertex，无需处理
    }
    if (std::regex_search(glsl, defRegex)) {
        return; // 已定义 mg_Vertex，避免重复注入
    }
    
    // 精确替换所有 gl_Vertex 实例（使用单词边界 \b）
    glsl = std::regex_replace(glsl, vertexRegex, "mg_Vertex");

    const std::string gl_VertexImpl = R"(
in vec4 mg_Vertex;
)";

    size_t insertPos = find_insertion_point(glsl);
    glsl.insert(insertPos, "\n" + gl_VertexImpl + "\n");

}

static inline void inject_fragcolor(std::string& glsl) {
    const std::string fragColorVar = "gl_FragColor";
    const std::string mainStart = "void main()";
    const std::string newFragColorVarname = "mg_FragColor";

    // Not using gl_FragColor
    const auto var_loc = glsl.find(fragColorVar);
    if (var_loc == std::string::npos)
        return;

    const auto main_loc = glsl.find(mainStart);
    // No main(), no inject
    if (main_loc == std::string::npos)
        return;


    glsl.insert(main_loc, "\nout vec4 " + newFragColorVarname + ";\n");
    replace_all(glsl, fragColorVar, newFragColorVarname);


}

static inline void inject_temporal_filter(std::string& glsl) {
    const std::regex defRegex(R"(vec4\s+GI_TemporalFilter\s*\()", std::regex::ECMAScript);

    if (glsl.find("GI_TemporalFilter") == std::string::npos) {
        return;
    }
    if (std::regex_search(glsl, defRegex)) {
        return;
    }

    const std::regex uniformRegex(R"(^\s*(?:layout\s*\([^)]*\)\s*)?uniform\s+\w+(?:\s*\[\s*\d+\s*\])?\s+\w+(?:\s*\[\s*\d+\s*\])?\s*;.*$)", std::regex::ECMAScript | std::regex::multiline);
    std::sregex_iterator it(glsl.begin(), glsl.end(), uniformRegex);
    std::sregex_iterator end;
    size_t insertPos = 0;
    for (; it != end; ++it) {
        insertPos = it->position() + it->length();
    }

    const std::string GI_TemporalFilterImpl = R"(
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
    glsl.insert(insertPos, "\n" + GI_TemporalFilterImpl + "\n");
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

std::string preprocess_glsl(const std::string& glsl, GLenum shaderType, bool* atomicCounterEmulated) {
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

	replace_all(ret, "vec3 worldPosDiff", "vec4 worldPosDiff");
    replace_all(ret, "vec3[3](vWorldPos[0] - vWorldPos[1]", "vec4[3](vWorldPos[0] - vWorldPos[1]");
    replace_all(ret, "vec3 reflection;", "vec3 reflection=vec3(0,0,0);");
    replace_all(ret, "writeonly uniform image2D colorimg4;", "layout (rgba16f) writeonly uniform image2D colorimg4;");
    replace_all(ret, "#error ", "// #error ");

	// Replace deprecated syntax
    if (shaderType == GL_VERTEX_SHADER) {
        replace_all(ret, "attribute", "in");
        replace_all(ret, "varying", "out");
    } else if (shaderType == GL_FRAGMENT_SHADER) {
        replace_all(ret, "varying", "in");
	}
	
    replace_all(ret, "texture2D", "texture");
	inject_fragcolor(ret);

	inject_gl_DepthRange(ret);
	inject_image2D_declarations(ret);
	inject_shaderDrawParameters(ret);
	inject_subgroup_BigGiftPackage(ret);
    inject_subgroup_clustered(ret);
    inject_gl_Vertex(ret);
	
	// GI_TemporalFilter injection
    inject_temporal_filter(ret);

    // textureQueryLod injection
    if (!g_gles_caps.GL_EXT_texture_query_lod) {
        inject_textureQueryLod(ret);
    }

    // MobileGlues macros injection
    inject_mg_macro_definition(ret);

    if (hardware->emulate_texture_buffer) {
        // Sampler buffer processing
        process_sampler_buffer(ret);
    }

    *atomicCounterEmulated = process_non_opaque_atomic_to_ssbo(ret);
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

/*std::vector<unsigned int> glsl_to_spirv(GLenum shader_type, int glsl_version, const char* const* shader_src, int& errc) {

	static shaderc_compiler_t compiler = nullptr;
    if(compiler == nullptr) {
        printf("shaderc!\n");
        compiler = shaderc_compiler_initialize();
        if(compiler == nullptr) {
            printf("Error: shaderc compiler cannot be created!\n");
            errc = -1;
            return {};
        }
	}

    // 映射 GL 着色器类型到 shaderc 类型
    shaderc_shader_kind shader_kind;
    switch (shader_type) {
        case GL_VERTEX_SHADER:
            shader_kind = shaderc_vertex_shader;
            break;
        case GL_FRAGMENT_SHADER:
            shader_kind = shaderc_fragment_shader;
            break;
        case GL_COMPUTE_SHADER:
            shader_kind = shaderc_compute_shader;
            break;
        case GL_TESS_CONTROL_SHADER:
            shader_kind = shaderc_tess_control_shader;
            break;
        case GL_TESS_EVALUATION_SHADER:
            shader_kind = shaderc_tess_evaluation_shader;
            break;
        case GL_GEOMETRY_SHADER:
            shader_kind = shaderc_geometry_shader;
            break;
        default:
            LOG_D("GLSL type not supported!")
            errc = -1;
            return {};
    }

    // 初始化编译选项
    shaderc_compile_options_t options = shaderc_compile_options_initialize();
    if (!options) {
        LOG_D("Failed to initialize shaderc compile options")
        shaderc_compiler_release(compiler);
        errc = -1;
        return {};
    }

    // 设置编译选项
    shaderc_compile_options_set_target_env(options, shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
    shaderc_compile_options_set_target_spirv(options, shaderc_spirv_version_1_6);
    shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_zero);
    shaderc_compile_options_set_auto_map_locations(options, true);
    shaderc_compile_options_set_auto_bind_uniforms(options, true);
    
    shaderc_compile_options_set_forced_version_profile(options, 450, shaderc_profile_core);
    shaderc_compile_options_add_macro_definition(options, "noperspective ", strlen("noperspective "), 0, strlen(""));
	shaderc_compile_options_set_generate_debug_info(options);
	
    shaderc_compilation_result_t result = shaderc_compile_into_spv(
        compiler, 
        *shader_src,
        std::strlen(*shader_src),
        shader_kind, 
        "optimized_spirv_code", 
        "main", 
        options
    );

    // 检查编译状态
    shaderc_compilation_status status = shaderc_result_get_compilation_status(result);
    if (status != shaderc_compilation_status_success) {
        LOG_E("GLSL Compiling ERROR: \n%s", shaderc_result_get_error_message(result))
        errc = -1;
        
        // 清理资源
        shaderc_result_release(result);

		return {};
    }
    
    LOG_W("GLSL Compiled. Warnings: %zu", shaderc_result_get_num_warnings(result))

    // 获取 SPIR-V 代码
    size_t spirv_size = shaderc_result_get_length(result);
    const char* spirv_bytes = shaderc_result_get_bytes(result);
    
    // 将 SPIR-V 二进制数据转换为 uint32_t 向量
    std::vector<unsigned int> spirv_code;
    if (spirv_size > 0 && spirv_size % 4 == 0) {
        size_t word_count = spirv_size / 4;
        spirv_code.resize(word_count);
        std::memcpy(spirv_code.data(), spirv_bytes, spirv_size);
    }

    // 清理资源
	shaderc_result_release(result);
    errc = 0;
    return spirv_code;
}*/

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
    shaderc_compile_options_set_forced_version_profile(opts, glsl_version, shaderc_profile_core);
    shaderc_compile_options_set_auto_map_locations(opts, true);
    shaderc_compile_options_set_auto_bind_uniforms(opts, true);
    shaderc_compile_options_set_target_env(opts, shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);

    //shaderc_compile_options_add_macro_definition(opts, "noperspective", strlen("noperspective "), 0, strlen(""));
    
    /*GLint max_draw_buffers;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max_draw_buffers);
    std::cout << "Detected GL_MAX_DRAW_BUFFERS: " << max_draw_buffers << std::endl;
    shaderc_compile_options_set_limit(opts, shaderc_limit_max_draw_buffers, max_draw_buffers);*/

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
        shader_type == GL_GEOMETRY_SHADER ? shaderc_glsl_geometry_shader :
        shaderc_glsl_infer_from_source,
        "optimized_shader", "main", opts);

    if(shaderc_result_get_compilation_status(optimized_glsl_res) != shaderc_compilation_status_success) {
        printf("There is a problem with shaderc！\n%s\n", shaderc_result_get_error_message(optimized_glsl_res));
        shaderc_result_release(optimized_glsl_res);
        errc = -1;
        return {};
    }

    const char* optimized_glsl = shaderc_result_get_bytes(optimized_glsl_res);

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
    shader.setEnvTarget(EShTargetSpv, EShTargetSpv_1_0);
    //shader.setEnvTarget(EShTargetSpv, EShTargetSpv_1_5);
    shader.setAutoMapLocations(true);
    shader.setAutoMapBindings(true);

    TBuiltInResource TBuiltInResource_resources = InitResources();

    if (!shader.parse(&TBuiltInResource_resources, glsl_version, true, EShMsgSpvRules)) {
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
    spvOptions.optimizeSize = true;
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
    spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_GLSL_VERSION, essl_version >= 300 ? essl_version : 300);
    spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_ES, SPVC_TRUE);
	spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_ENABLE_420PACK_EXTENSION, SPVC_FALSE);
    //spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_VULKAN_SEMANTICS, SPVC_FALSE);
    spvc_compiler_install_compiler_options(compiler_glsl, options);
    spvc_compiler_compile(compiler_glsl, &result);

    if (!result) {
        const char* error_msg = spvc_context_get_last_error_string(context);
        if (error_msg) {
            LOG_E("SPIRV-Cross error: %s", error_msg);
        } else {
            LOG_E("SPIRV-Cross failed without error message");
        }
        
        // 检查常见原因
        if (essl_version < 300) {
            LOG_E("Hint: ESSL version %u may be too low", essl_version);
        }
        
        spvc_compiler_get_current_id_bound(compiler_glsl);
        
        errc = -1;
        spvc_context_destroy(context);
        return "";
    }

    std::string essl = result;

    spvc_context_destroy(context);

    errc = 0;
    return essl;
}

static std::string RemoveUniformInitialization(const std::string& glslCode) {
    // 匹配 "uniform type name = value;" 并替换为 "uniform type name;"
    std::regex uniformInitRegex(R"(uniform\s+(\w+)\s+(\w+)\s*=\s*[^;]+;)");
    return std::regex_replace(glslCode, uniformInitRegex, "uniform $1 $2;");
} // For glslOptimizerV2

static bool glslang_inited = false;
std::string GLSLtoGLSLES_2(const char *glsl_code, GLenum glsl_type, uint essl_version, int& return_code) {
	bool atomicCounterEmulated = false;
    std::string correct_glsl_str = preprocess_glsl(glsl_code, glsl_type, &atomicCounterEmulated);
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

    if (glsl_type != GL_COMPUTE_SHADER) {
        essl = removeLayoutBinding(essl);
    }
    essl = processOutColorLocations(essl);
    essl = forceSupporterOutput(essl);

    LOG_D("Originally GLSL to GLSL ES Complete: \n%s", essl.c_str())
    return_code = errc;
    if (return_code == 0) {
        return_code = atomicCounterEmulated ? 1 : 0;
    }
    return essl;
}

std::string GLSLtoGLSLES_1(const char *glsl_code, GLenum glsl_type, uint esversion, int& return_code) {
#if !defined(__APPLE__)
    LOG_W("Warning: use glsl optimizer to convert shader.")

	bool atomicCounterEmulated = false;
	std::string correct_glsl_str = preprocess_glsl(glsl_code, glsl_type, &atomicCounterEmulated);
    int glsl_version = get_or_add_glsl_version(correct_glsl_str);

	correct_glsl_str = RemoveUniformInitialization(correct_glsl_str);

    if (esversion < 300) esversion = 320;
    std::string result = MesaConvertShader(glsl_code, glsl_type, glsl_version, esversion);

    return_code = 0;
    return result;
#else
    LOG_W_FORCE("Cannot convert glsl with version %d in MacOS/iOS", esversion);
    return std::string(glsl_code);
#endif
}
