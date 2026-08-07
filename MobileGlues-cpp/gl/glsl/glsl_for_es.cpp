// MobileGlues - gl/glsl/glsl_for_es.cpp
// Copyright (c) 2025-2026 MobileGL-Dev
// Licensed under the GNU Lesser General Public License v2.1:
//   https://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt
// SPDX-License-Identifier: LGPL-2.1-only
// End of Source File Header
#include "glsl_for_es.h"

#include <glslang/Public/ShaderLang.h>
#include <glslang/Include/Types.h>
#include <glslang/Public/ShaderLang.h>
#include <spirv_cross/spirv_cross_c.h>
#include <iostream>
#include <fstream>
#include "../log.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include <string>
#include <regex>
#include <strstream>
#include <algorithm>
#include <sstream>
#include "cache.h"
#include "../../version.h"

#define DEBUG 1

static TBuiltInResource InitResources() {
    TBuiltInResource Resources{};

    Resources.maxLights = 32;
    Resources.maxClipPlanes = 6;
    Resources.maxTextureUnits = 32;
    Resources.maxTextureCoords = 32;
    Resources.maxVertexAttribs = 64;
    Resources.maxVertexUniformComponents = 4096;
    Resources.maxVaryingFloats = 64;
    Resources.maxVertexTextureImageUnits = 32;
    Resources.maxCombinedTextureImageUnits = 80;
    Resources.maxTextureImageUnits = 32;
    Resources.maxFragmentUniformComponents = 4096;
    Resources.maxDrawBuffers = 32;
    Resources.maxVertexUniformVectors = 128;
    Resources.maxVaryingVectors = 8;
    Resources.maxFragmentUniformVectors = 16;
    Resources.maxVertexOutputVectors = 16;
    Resources.maxFragmentInputVectors = 15;
    Resources.minProgramTexelOffset = -8;
    Resources.maxProgramTexelOffset = 7;
    Resources.maxClipDistances = 8;
    Resources.maxComputeWorkGroupCountX = 65535;
    Resources.maxComputeWorkGroupCountY = 65535;
    Resources.maxComputeWorkGroupCountZ = 65535;
    Resources.maxComputeWorkGroupSizeX = 1024;
    Resources.maxComputeWorkGroupSizeY = 1024;
    Resources.maxComputeWorkGroupSizeZ = 64;
    Resources.maxComputeUniformComponents = 1024;
    Resources.maxComputeTextureImageUnits = 16;
    Resources.maxComputeImageUniforms = 8;
    Resources.maxComputeAtomicCounters = 8;
    Resources.maxComputeAtomicCounterBuffers = 1;
    Resources.maxVaryingComponents = 60;
    Resources.maxVertexOutputComponents = 64;
    Resources.maxGeometryInputComponents = 64;
    Resources.maxGeometryOutputComponents = 128;
    Resources.maxFragmentInputComponents = 128;
    Resources.maxImageUnits = 8;
    Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
    Resources.maxCombinedShaderOutputResources = 8;
    Resources.maxImageSamples = 0;
    Resources.maxVertexImageUniforms = 0;
    Resources.maxTessControlImageUniforms = 0;
    Resources.maxTessEvaluationImageUniforms = 0;
    Resources.maxGeometryImageUniforms = 0;
    Resources.maxFragmentImageUniforms = 8;
    Resources.maxCombinedImageUniforms = 8;
    Resources.maxGeometryTextureImageUnits = 16;
    Resources.maxGeometryOutputVertices = 256;
    Resources.maxGeometryTotalOutputComponents = 1024;
    Resources.maxGeometryUniformComponents = 1024;
    Resources.maxGeometryVaryingComponents = 64;
    Resources.maxTessControlInputComponents = 128;
    Resources.maxTessControlOutputComponents = 128;
    Resources.maxTessControlTextureImageUnits = 16;
    Resources.maxTessControlUniformComponents = 1024;
    Resources.maxTessControlTotalOutputComponents = 4096;
    Resources.maxTessEvaluationInputComponents = 128;
    Resources.maxTessEvaluationOutputComponents = 128;
    Resources.maxTessEvaluationTextureImageUnits = 16;
    Resources.maxTessEvaluationUniformComponents = 1024;
    Resources.maxTessPatchComponents = 120;
    Resources.maxPatchVertices = 32;
    Resources.maxTessGenLevel = 64;
    Resources.maxViewports = 16;
    Resources.maxVertexAtomicCounters = 0;
    Resources.maxTessControlAtomicCounters = 0;
    Resources.maxTessEvaluationAtomicCounters = 0;
    Resources.maxGeometryAtomicCounters = 0;
    Resources.maxFragmentAtomicCounters = 8;
    Resources.maxCombinedAtomicCounters = 8;
    Resources.maxAtomicCounterBindings = 1;
    Resources.maxVertexAtomicCounterBuffers = 0;
    Resources.maxTessControlAtomicCounterBuffers = 0;
    Resources.maxTessEvaluationAtomicCounterBuffers = 0;
    Resources.maxGeometryAtomicCounterBuffers = 0;
    Resources.maxFragmentAtomicCounterBuffers = 1;
    Resources.maxCombinedAtomicCounterBuffers = 1;
    Resources.maxAtomicCounterBufferSize = 16384;
    Resources.maxTransformFeedbackBuffers = 4;
    Resources.maxTransformFeedbackInterleavedComponents = 64;
    Resources.maxCullDistances = 8;
    Resources.maxCombinedClipAndCullDistances = 8;
    Resources.maxSamples = 4;
    Resources.maxMeshOutputVerticesNV = 256;
    Resources.maxMeshOutputPrimitivesNV = 512;
    Resources.maxMeshWorkGroupSizeX_NV = 32;
    Resources.maxMeshWorkGroupSizeY_NV = 1;
    Resources.maxMeshWorkGroupSizeZ_NV = 1;
    Resources.maxTaskWorkGroupSizeX_NV = 32;
    Resources.maxTaskWorkGroupSizeY_NV = 1;
    Resources.maxTaskWorkGroupSizeZ_NV = 1;
    Resources.maxMeshViewCountNV = 4;

    Resources.limits.nonInductiveForLoops = true;
    Resources.limits.whileLoops = true;
    Resources.limits.doWhileLoops = true;
    Resources.limits.generalUniformIndexing = true;
    Resources.limits.generalAttributeMatrixVectorIndexing = true;
    Resources.limits.generalVaryingIndexing = true;
    Resources.limits.generalSamplerIndexing = true;
    Resources.limits.generalVariableIndexing = true;
    Resources.limits.generalConstantMatrixVectorIndexing = true;

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
    bool hasPrecisionFloat =
        glslCode.find("precision ") != std::string::npos && glslCode.find("float;") != std::string::npos;
    bool hasPrecisionInt =
        glslCode.find("precision ") != std::string::npos && glslCode.find("int;") != std::string::npos;

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
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) { return !std::isspace(ch); }));
    str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) { return !std::isspace(ch); }).base(), str.end());
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
            scan_pos += 7; // Skip "uniform"

            std::string precision, type;
            bool found_precision = false;

            while (scan_pos < length) {
                while (scan_pos < length && std::isspace(glslCode[scan_pos]))
                    ++scan_pos;

                for (const auto& kw : precision_kws) {
                    if (glslCode.compare(scan_pos, kw.length(), kw) == 0) {
                        precision = " " + kw;
                        scan_pos += kw.length();
                        found_precision = true;
                        break;
                    }
                }
                if (found_precision) break;

                const size_t type_start = scan_pos;
                while (scan_pos < length && (std::isalnum(glslCode[scan_pos]) || glslCode[scan_pos] == '_')) {
                    ++scan_pos;
                }
                type = glslCode.substr(type_start, scan_pos - type_start);
                break;
            }

            while (scan_pos < length) {
                while (scan_pos < length && std::isspace(glslCode[scan_pos]))
                    ++scan_pos;

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

            if (type.empty()) {
                const size_t type_start = scan_pos;
                while (scan_pos < length && (std::isalnum(glslCode[scan_pos]) || glslCode[scan_pos] == '_')) {
                    ++scan_pos;
                }
                type = glslCode.substr(type_start, scan_pos - type_start);
            }

            while (scan_pos < length && std::isspace(glslCode[scan_pos]))
                ++scan_pos;
            const size_t name_start = scan_pos;
            while (scan_pos < length && (std::isalnum(glslCode[scan_pos]) || glslCode[scan_pos] == '_')) {
                ++scan_pos;
            }
            const std::string name = glslCode.substr(name_start, scan_pos - name_start);

            size_t decl_end = glslCode.find(';', scan_pos);
            if (decl_end == std::string::npos)
                decl_end = length;
            else
                ++decl_end;
            const bool has_initializer = (glslCode.find('=', scan_pos) < decl_end);
            if (has_initializer) {
                result.append("uniform").append(precision).append(" ").append(type).append(" ").append(name).append(
                    ";");
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

std::string GLSLtoGLSLES(const char* glsl_code, GLenum glsl_type, uint essl_version, uint glsl_version,
                         int& return_code) {
    std::string sha256_string(glsl_code);
    sha256_string += "\n//" + std::to_string(MAJOR) + "." + std::to_string(MINOR) + "." + std::to_string(REVISION) +
                     "|" + std::to_string(essl_version);
    const char* cachedESSL = Cache::get_instance().get(sha256_string.c_str());
    if (cachedESSL) {
        LOG_D("GLSL Hit Cache:\n%s\n-->\n%s", glsl_code, cachedESSL)
        return_code = 0;
        return (char*)cachedESSL;
    }

    return_code = -1;
    // std::string converted = glsl_version<140? GLSLtoGLSLES_1(glsl_code, glsl_type, essl_version,
    // return_code):GLSLtoGLSLES_2(glsl_code, glsl_type, essl_version, return_code);
    std::string converted = GLSLtoGLSLES_2(glsl_code, glsl_type, essl_version, return_code);
    if (return_code >= 0 && !converted.empty()) {
        converted = process_uniform_declarations(converted);
        Cache::get_instance().put(sha256_string.c_str(), converted.c_str());
    }

    return (return_code >= 0) ? converted : glsl_code;
}

std::string replace_line_starting_with(const std::string& glslCode, const std::string& starting,
                                       const std::string& substitution = "") {
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
        if (current + 5 <= length && glslCode.compare(current, 5, "#line") == 0) {
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
            } else {
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
        } else {
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
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
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
        } else {
            version_end++;
        }
        insertion_point = version_end;
        pos = version_end;
    } else {
        insertion_point = 0;
        pos = 0;
    }

    while (pos < glsl.length()) {
        size_t line_begin = pos;
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
    source = std::regex_replace(source, pattern,
                                "texelFetch($1, ivec2(($2) % u_BufferTexWidth, ($2) / u_BufferTexWidth), 0)");

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

    source = std::regex_replace(source, std::regex("texelFetch\\((\\w+)\\s*,\\s*ivec2\\(([^)]+)\\)\\s*,\\s*0\\)"),
                                "texelFetch($1, bufferCoords($2), 0)");

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

static inline void inject_textureQueryLod(std::string& glsl) {
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

static inline void inject_gl_DepthRange(std::string& glsl) {
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

static inline void inject_subgroup_BigGiftPackage(std::string& glsl) {
    const std::regex defRegex(R"(#define\s+SUBGROUP_SIZE\s+\d+)", std::regex::ECMAScript);

    if (glsl.find("subgroupBallot") == std::string::npos && 
        glsl.find("activeMask") == std::string::npos &&
        glsl.find("gl_SubgroupInvocationID") == std::string::npos &&
        glsl.find("subgroupAdd") == std::string::npos &&
        glsl.find("gl_NumSubgroups") == std::string::npos &&
        glsl.find("gl_SubgroupID") == std::string::npos &&
        glsl.find("subgroupAny") == std::string::npos &&
        glsl.find("subgroupAll") == std::string::npos &&
        glsl.find("subgroupElect") == std::string::npos &&
        glsl.find("subgroupExclusiveAdd") == std::string::npos &&
        glsl.find("subgroupExclusiveMax") == std::string::npos &&
        glsl.find("gl_SubgroupSize") == std::string::npos) {
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
    replace_all(glsl, "subgroupExclusiveAdd", "mg_subgroupExclusiveAdd");
    replace_all(glsl, "subgroupExclusiveMax", "mg_subgroupExclusiveMax");
    replace_all(glsl, "gl_SubgroupSize", "mg_gl_SubgroupSize()");

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

uint mg_gl_SubgroupSize() {
    // 返回子组大小（常量）
    return SUBGROUP_SIZE;
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

bool mg_subgroupElect() {
    // 子组内第一个线程（调用ID=0）被选为领导线程
    return (mg_gl_SubgroupInvocationID() == 0u);
}

// ==================== 全局共享内存声明 ====================
const uint total_workgroup_size = gl_WorkGroupSize.x * gl_WorkGroupSize.y * gl_WorkGroupSize.z;
const uint num_subgroups = (total_workgroup_size + SUBGROUP_SIZE - 1u) / SUBGROUP_SIZE;

// 标量类型共享内存 - 用于加法和最大值操作
shared float s_reduceAdd_float[num_subgroups * SUBGROUP_SIZE];
shared uint s_reduceAdd_uint[num_subgroups * SUBGROUP_SIZE];
shared int s_reduceAdd_int[num_subgroups * SUBGROUP_SIZE];

// 向量类型共享内存 - 用于加法和最大值操作
shared vec2 s_reduceAdd_vec2[num_subgroups * SUBGROUP_SIZE];
shared vec3 s_reduceAdd_vec3[num_subgroups * SUBGROUP_SIZE];
shared vec4 s_reduceAdd_vec4[num_subgroups * SUBGROUP_SIZE];

// 标量类型共享内存 - 用于排他性操作
shared float s_exclusiveAdd_float[num_subgroups * SUBGROUP_SIZE];
shared uint s_exclusiveAdd_uint[num_subgroups * SUBGROUP_SIZE];
shared int s_exclusiveAdd_int[num_subgroups * SUBGROUP_SIZE];

shared float s_exclusiveMax_float[num_subgroups * SUBGROUP_SIZE];
shared uint s_exclusiveMax_uint[num_subgroups * SUBGROUP_SIZE];
shared int s_exclusiveMax_int[num_subgroups * SUBGROUP_SIZE];

// 向量类型共享内存 - 用于排他性操作
shared vec2 s_exclusiveAdd_vec2[num_subgroups * SUBGROUP_SIZE];
shared vec3 s_exclusiveAdd_vec3[num_subgroups * SUBGROUP_SIZE];
shared vec4 s_exclusiveAdd_vec4[num_subgroups * SUBGROUP_SIZE];

shared vec2 s_exclusiveMax_vec2[num_subgroups * SUBGROUP_SIZE];
shared vec3 s_exclusiveMax_vec3[num_subgroups * SUBGROUP_SIZE];
shared vec4 s_exclusiveMax_vec4[num_subgroups * SUBGROUP_SIZE];

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

// ==================== 子组排他性加法模拟 ====================
// 排他性加法：每个线程获取之前所有线程值的总和
float mg_subgroupExclusiveAdd(float value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    // 初始化共享内存
    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_exclusiveAdd_float[i] = 0.0;
    }
    memoryBarrierShared();
    barrier();

    s_exclusiveAdd_float[offset] = value;
    memoryBarrierShared();
    barrier();

    // 执行前缀和（包含性）
    for (uint stride = 1; stride < SUBGROUP_SIZE; stride <<= 1) {
        float temp = 0.0;
        if (laneID >= stride) {
            temp = s_exclusiveAdd_float[offset - stride];
        }
        memoryBarrierShared();
        barrier();
        if (laneID >= stride) {
            s_exclusiveAdd_float[offset] += temp;
        }
        memoryBarrierShared();
        barrier();
    }

    // 转换为排他性：返回前一个线程的包含性前缀和
    if (laneID == 0) {
        return 0.0;
    } else {
        return s_exclusiveAdd_float[offset - 1];
    }
}

uint mg_subgroupExclusiveAdd(uint value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_exclusiveAdd_uint[i] = 0u;
    }
    memoryBarrierShared();
    barrier();

    s_exclusiveAdd_uint[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = 1; stride < SUBGROUP_SIZE; stride <<= 1) {
        uint temp = 0u;
        if (laneID >= stride) {
            temp = s_exclusiveAdd_uint[offset - stride];
        }
        memoryBarrierShared();
        barrier();
        if (laneID >= stride) {
            s_exclusiveAdd_uint[offset] += temp;
        }
        memoryBarrierShared();
        barrier();
    }

    if (laneID == 0) {
        return 0u;
    } else {
        return s_exclusiveAdd_uint[offset - 1];
    }
}

int mg_subgroupExclusiveAdd(int value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_exclusiveAdd_int[i] = 0;
    }
    memoryBarrierShared();
    barrier();

    s_exclusiveAdd_int[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = 1; stride < SUBGROUP_SIZE; stride <<= 1) {
        int temp = 0;
        if (laneID >= stride) {
            temp = s_exclusiveAdd_int[offset - stride];
        }
        memoryBarrierShared();
        barrier();
        if (laneID >= stride) {
            s_exclusiveAdd_int[offset] += temp;
        }
        memoryBarrierShared();
        barrier();
    }

    if (laneID == 0) {
        return 0;
    } else {
        return s_exclusiveAdd_int[offset - 1];
    }
}

vec2 mg_subgroupExclusiveAdd(vec2 value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_exclusiveAdd_vec2[i] = vec2(0.0);
    }
    memoryBarrierShared();
    barrier();

    s_exclusiveAdd_vec2[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = 1; stride < SUBGROUP_SIZE; stride <<= 1) {
        vec2 temp = vec2(0.0);
        if (laneID >= stride) {
            temp = s_exclusiveAdd_vec2[offset - stride];
        }
        memoryBarrierShared();
        barrier();
        if (laneID >= stride) {
            s_exclusiveAdd_vec2[offset] += temp;
        }
        memoryBarrierShared();
        barrier();
    }

    if (laneID == 0) {
        return vec2(0.0);
    } else {
        return s_exclusiveAdd_vec2[offset - 1];
    }
}

vec3 mg_subgroupExclusiveAdd(vec3 value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_exclusiveAdd_vec3[i] = vec3(0.0);
    }
    memoryBarrierShared();
    barrier();

    s_exclusiveAdd_vec3[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = 1; stride < SUBGROUP_SIZE; stride <<= 1) {
        vec3 temp = vec3(0.0);
        if (laneID >= stride) {
            temp = s_exclusiveAdd_vec3[offset - stride];
        }
        memoryBarrierShared();
        barrier();
        if (laneID >= stride) {
            s_exclusiveAdd_vec3[offset] += temp;
        }
        memoryBarrierShared();
        barrier();
    }

    if (laneID == 0) {
        return vec3(0.0);
    } else {
        return s_exclusiveAdd_vec3[offset - 1];
    }
}

vec4 mg_subgroupExclusiveAdd(vec4 value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_exclusiveAdd_vec4[i] = vec4(0.0);
    }
    memoryBarrierShared();
    barrier();

    s_exclusiveAdd_vec4[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = 1; stride < SUBGROUP_SIZE; stride <<= 1) {
        vec4 temp = vec4(0.0);
        if (laneID >= stride) {
            temp = s_exclusiveAdd_vec4[offset - stride];
        }
        memoryBarrierShared();
        barrier();
        if (laneID >= stride) {
            s_exclusiveAdd_vec4[offset] += temp;
        }
        memoryBarrierShared();
        barrier();
    }

    if (laneID == 0) {
        return vec4(0.0);
    } else {
        return s_exclusiveAdd_vec4[offset - 1];
    }
}

// ==================== 子组排他性最大值模拟 ====================
// 排他性最大值：每个线程获取之前所有线程的最大值
float mg_subgroupExclusiveMax(float value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    // 初始化共享内存
    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_exclusiveMax_float[i] = -1.0 / 0.0; // -INF
    }
    memoryBarrierShared();
    barrier();

    s_exclusiveMax_float[offset] = value;
    memoryBarrierShared();
    barrier();

    // 执行前缀最大值（包含性）
    for (uint stride = 1; stride < SUBGROUP_SIZE; stride <<= 1) {
        float temp = -1.0 / 0.0;
        if (laneID >= stride) {
            temp = s_exclusiveMax_float[offset - stride];
        }
        memoryBarrierShared();
        barrier();
        if (laneID >= stride) {
            s_exclusiveMax_float[offset] = max(s_exclusiveMax_float[offset], temp);
        }
        memoryBarrierShared();
        barrier();
    }

    // 转换为排他性：返回前一个线程的包含性前缀最大值
    if (laneID == 0) {
        return -1.0 / 0.0; // -INF
    } else {
        return s_exclusiveMax_float[offset - 1];
    }
}

uint mg_subgroupExclusiveMax(uint value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_exclusiveMax_uint[i] = 0u;
    }
    memoryBarrierShared();
    barrier();

    s_exclusiveMax_uint[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = 1; stride < SUBGROUP_SIZE; stride <<= 1) {
        uint temp = 0u;
        if (laneID >= stride) {
            temp = s_exclusiveMax_uint[offset - stride];
        }
        memoryBarrierShared();
        barrier();
        if (laneID >= stride) {
            s_exclusiveMax_uint[offset] = max(s_exclusiveMax_uint[offset], temp);
        }
        memoryBarrierShared();
        barrier();
    }

    if (laneID == 0) {
        return 0u;
    } else {
        return s_exclusiveMax_uint[offset - 1];
    }
}

int mg_subgroupExclusiveMax(int value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_exclusiveMax_int[i] = (-1 << 31); // 最小int值
    }
    memoryBarrierShared();
    barrier();

    s_exclusiveMax_int[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = 1; stride < SUBGROUP_SIZE; stride <<= 1) {
        int temp = (-1 << 31);
        if (laneID >= stride) {
            temp = s_exclusiveMax_int[offset - stride];
        }
        memoryBarrierShared();
        barrier();
        if (laneID >= stride) {
            s_exclusiveMax_int[offset] = max(s_exclusiveMax_int[offset], temp);
        }
        memoryBarrierShared();
        barrier();
    }

    if (laneID == 0) {
        return (-1 << 31); // 最小int值
    } else {
        return s_exclusiveMax_int[offset - 1];
    }
}

vec2 mg_subgroupExclusiveMax(vec2 value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_exclusiveMax_vec2[i] = vec2(-1.0 / 0.0);
    }
    memoryBarrierShared();
    barrier();

    s_exclusiveMax_vec2[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = 1; stride < SUBGROUP_SIZE; stride <<= 1) {
        vec2 temp = vec2(-1.0 / 0.0);
        if (laneID >= stride) {
            temp = s_exclusiveMax_vec2[offset - stride];
        }
        memoryBarrierShared();
        barrier();
        if (laneID >= stride) {
            s_exclusiveMax_vec2[offset] = max(s_exclusiveMax_vec2[offset], temp);
        }
        memoryBarrierShared();
        barrier();
    }

    if (laneID == 0) {
        return vec2(-1.0 / 0.0);
    } else {
        return s_exclusiveMax_vec2[offset - 1];
    }
}

vec3 mg_subgroupExclusiveMax(vec3 value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_exclusiveMax_vec3[i] = vec3(-1.0 / 0.0);
    }
    memoryBarrierShared();
    barrier();

    s_exclusiveMax_vec3[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = 1; stride < SUBGROUP_SIZE; stride <<= 1) {
        vec3 temp = vec3(-1.0 / 0.0);
        if (laneID >= stride) {
            temp = s_exclusiveMax_vec3[offset - stride];
        }
        memoryBarrierShared();
        barrier();
        if (laneID >= stride) {
            s_exclusiveMax_vec3[offset] = max(s_exclusiveMax_vec3[offset], temp);
        }
        memoryBarrierShared();
        barrier();
    }

    if (laneID == 0) {
        return vec3(-1.0 / 0.0);
    } else {
        return s_exclusiveMax_vec3[offset - 1];
    }
}

vec4 mg_subgroupExclusiveMax(vec4 value) {
    uint subgroupID = mg_gl_SubgroupID();
    uint laneID = mg_gl_SubgroupInvocationID();
    uint offset = subgroupID * SUBGROUP_SIZE + laneID;

    for (uint i = gl_LocalInvocationIndex; i < num_subgroups * SUBGROUP_SIZE; i += total_workgroup_size) {
        s_exclusiveMax_vec4[i] = vec4(-1.0 / 0.0);
    }
    memoryBarrierShared();
    barrier();

    s_exclusiveMax_vec4[offset] = value;
    memoryBarrierShared();
    barrier();

    for (uint stride = 1; stride < SUBGROUP_SIZE; stride <<= 1) {
        vec4 temp = vec4(-1.0 / 0.0);
        if (laneID >= stride) {
            temp = s_exclusiveMax_vec4[offset - stride];
        }
        memoryBarrierShared();
        barrier();
        if (laneID >= stride) {
            s_exclusiveMax_vec4[offset] = max(s_exclusiveMax_vec4[offset], temp);
        }
        memoryBarrierShared();
        barrier();
    }

    if (laneID == 0) {
        return vec4(-1.0 / 0.0);
    } else {
        return s_exclusiveMax_vec4[offset - 1];
    }
}

)";

    size_t insertPos = find_insertion_point(glsl);
    glsl.insert(insertPos, "\n" + subgroup_BigGiftPackageImpl + "\n");
}

static inline void inject_subgroup_clustered(std::string& glsl) {
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

static inline void inject_int64_support(std::string& glsl) {
    // 检测是否需要模拟
    bool needs_int64 = (glsl.find("uint64_t") != std::string::npos ||
                        glsl.find("int64_t")  != std::string::npos ||
                        glsl.find("GL_ARB_gpu_shader_int64") != std::string::npos);
    if (!needs_int64) return;

    // 已注入标记（检查是否存在我们的类型定义）
    if (glsl.find("mg_u64") != std::string::npos) return;

    // 注释掉原扩展启用指令
    replace_all(glsl, "#extension GL_ARB_gpu_shader_int64", "// #extension GL_ARB_gpu_shader_int64");

    // 注入模拟代码（使用 uvec2 表示 64 位无符号整数，ivec2 表示有符号）
    const std::string int64_impl = R"(
// ---------- GL_ARB_gpu_shader_int64 simulation (using uvec2/ivec2) ----------
#define mg_u64 uvec2
#define mg_i64 ivec2

// Constructors for 64-bit values from 32-bit low/high parts
mg_u64 mg_uint64(uint lo, uint hi) { return mg_u64(lo, hi); }
mg_i64 mg_int64(int lo, int hi)     { return mg_i64(lo, hi); }

// Construct from a 32-bit literal (low part only, high part = 0)
// Usage: mg_u64 a = mg_u64(1234u);
// But we also provide a macro for convenience: U64(x) -> mg_uint64(x, 0u)
#define U64(x) mg_uint64(uint(x), 0u)
#define I64(x) mg_int64(int(x), 0)

// Addition (carry handled)
mg_u64 mg_add_u64(mg_u64 a, mg_u64 b) {
    uint lo = a.x + b.x;
    uint hi = a.y + b.y;
    if (lo < a.x) hi += 1u;
    return mg_u64(lo, hi);
}

mg_i64 mg_add_i64(mg_i64 a, mg_i64 b) {
    // 对于有符号，先按无符号处理再转换（简化，忽略符号扩展问题）
    mg_u64 ua = mg_u64(uint(a.x), uint(a.y));
    mg_u64 ub = mg_u64(uint(b.x), uint(b.y));
    mg_u64 ur = mg_add_u64(ua, ub);
    return mg_i64(int(ur.x), int(ur.y));
}

// Equality
bool mg_eq_u64(mg_u64 a, mg_u64 b) { return a.x == b.x && a.y == b.y; }
bool mg_eq_i64(mg_i64 a, mg_i64 b) { return a.x == b.x && a.y == b.y; }

// Bitwise AND
mg_u64 mg_and_u64(mg_u64 a, mg_u64 b) { return mg_u64(a.x & b.x, a.y & b.y); }
// ... 可根据需要扩展其他操作
// ---------------------------------------------------------------------------
)";

    size_t insertPos = find_insertion_point(glsl);
    glsl.insert(insertPos, int64_impl);

    // ---------- 处理简单字面量赋值 ----------
    // 手动遍历并替换形如： uint64_t var = 1234; 或 uint64_t var = 1234u;
    std::regex decl_assign(R"(\b(uint64_t|int64_t)\s+(\w+)\s*=\s*(\d+)(u?)\s*;)");
    std::string result;
    size_t last_pos = 0;
    
    auto begin = std::sregex_iterator(glsl.begin(), glsl.end(), decl_assign);
    auto end = std::sregex_iterator();
    
    for (auto it = begin; it != end; ++it) {
        const auto& match = *it;
        result.append(glsl, last_pos, match.position() - last_pos);
        
        std::string type = match[1].str();
        std::string var = match[2].str();
        std::string num = match[3].str();
        bool is_unsigned = (match[4].str() == "u");
        
        if (type == "uint64_t") {
            result += "mg_u64 " + var + " = U64(" + num + ");";
        } else {
            result += "mg_i64 " + var + " = I64(" + num + ");";
        }
        
        last_pos = match.position() + match.length();
    }
    
    result.append(glsl, last_pos, glsl.length() - last_pos);
    glsl = result;

    // 替换所有剩余的类型名（确保只替换用户代码中的，不替换注入代码内的）
    // 注意：我们必须使用正则，并限制匹配不包含 "mg_" 前缀的单词边界
    glsl = std::regex_replace(glsl, std::regex(R"(\buint64_t\b)"), "mg_u64");
    glsl = std::regex_replace(glsl, std::regex(R"(\bint64_t\b)"), "mg_i64");
}


// 查找 #version 后的插入点，若没有则找文件开头
static size_t find_shader_insertion_point(const std::string& glsl) {
    std::regex version_regex(R"(#version\s+\d+\s+(es\s+)?\d+\s*\n)", std::regex::ECMAScript);
    std::smatch match;
    if (std::regex_search(glsl, match, version_regex)) {
        return match.position() + match.length();
    }
    return 0; // 无版本声明，直接在开头插入
}

// 检查是否已经注入转换层
static bool is_1d_wrapper_injected(const std::string& glsl) {
    // 通过特征函数或宏判断，例如 texture1D 的定义
    return glsl.find("texture1D(") != std::string::npos ||
           glsl.find("TEXTURE_1D_WRAPPER_INJECTED") != std::string::npos;
}

// 检查是否使用了任何 1D 纹理相关标识符
static bool uses_1d_texture_features(const std::string& glsl) {
    // 类型名
    const std::vector<std::string> types = {
        "sampler1D", "sampler1DShadow", "isampler1D", "usampler1D",
        "image1D", "iimage1D", "uimage1D"
    };
    for (const auto& type : types) {
        if (glsl.find(type) != std::string::npos) return true;
    }

    // 函数调用（粗略检测，避免不必要的注入）
    const std::vector<std::string> funcs = {
        "texture(", "texelFetch(", "textureLod(", "textureSize(",
        "imageStore(", "imageLoad(", "imageSize("
    };
    for (const auto& func : funcs) {
        if (glsl.find(func) != std::string::npos) return true;
    }
    return false;
}

// 自动替换 1D 纹理类型和函数调用
static void replace_1d_texture_usage(std::string& glsl) {
    // 类型替换 (单词边界)
    std::vector<std::pair<std::string, std::string>> type_maps = {
        {"sampler1DShadow", "sampler2DShadow"},
        {"sampler1D",       "sampler2D"},
        {"isampler1D",      "isampler2D"},
        {"usampler1D",      "usampler2D"},
        {"image1D",         "image2D"},
        {"iimage1D",        "iimage2D"},
        {"uimage1D",        "uimage2D"}
    };
    for (const auto& [from, to] : type_maps) {
        std::regex from_regex("\\b" + from + "\\b");
        glsl = std::regex_replace(glsl, from_regex, to);
    }

    // 函数调用替换 (注意括号，避免重复替换已转换的)
    std::vector<std::pair<std::string, std::string>> func_maps = {
        {"texture\\s*\\(",  "texture1D("},
        {"texelFetch\\s*\\(", "texelFetch1D("},
        {"textureLod\\s*\\(", "textureLod1D("},
        {"textureSize\\s*\\(", "textureSize1D("},
        {"imageStore\\s*\\(", "imageStore1D("},
        {"imageLoad\\s*\\(", "imageLoad1D("},
        {"imageSize\\s*\\(", "imageSize1D("}
    };
    for (const auto& [from, to] : func_maps) {
        std::regex from_regex(from);
        glsl = std::regex_replace(glsl, from_regex, to);
    }
}

// 主函数：自动为 GLSL 代码添加 1D 纹理模拟支持
static inline void inject_1d_texture_compatibility(std::string& glsl) {
    // 如果已经注入过，跳过
    if (is_1d_wrapper_injected(glsl)) return;

    // 如果没有使用任何 1D 纹理特性，跳过
    if (!uses_1d_texture_features(glsl)) return;

    // 执行类型和函数名的自动替换
    replace_1d_texture_usage(glsl);

    // 注入转换层代码（定义所有模拟函数）
    const std::string wrapper_code = R"(
// ----- Auto-generated 1D texture compatibility layer -----
// Simulates sampler1D/image1D using height=1 sampler2D/image2D.

// 1. Normalized sampling
vec4 texture1D(sampler2D tex, float s) { return texture(tex, vec2(s, 0.5)); }
vec4 texture1D(sampler2D tex, float s, float bias) { return texture(tex, vec2(s, 0.5), bias); }
vec4 textureLod1D(sampler2D tex, float s, float lod) { return textureLod(tex, vec2(s, 0.5), lod); }

// 2. Integer texel fetch
vec4 texelFetch1D(sampler2D tex, int x, int lod) { return texelFetch(tex, ivec2(x, 0), lod); }

// 3. Texture size
int textureSize1D(sampler2D tex, int lod) { return textureSize(tex, lod).x; }

// 4. Shadow sampler
float texture1D(sampler2DShadow tex, vec2 coord) { return texture(tex, vec3(coord.x, 0.5, coord.y)); }
float texture1D(sampler2DShadow tex, vec2 coord, float bias) { return texture(tex, vec3(coord.x, 0.5, coord.y), bias); }
float textureLod1D(sampler2DShadow tex, vec2 coord, float lod) { return textureLod(tex, vec3(coord.x, 0.5, coord.y), lod); }

// 5. Signed integer sampler
ivec4 texture1D(isampler2D tex, float s) { return texture(tex, vec2(s, 0.5)); }
ivec4 texture1D(isampler2D tex, float s, float bias) { return texture(tex, vec2(s, 0.5), bias); }
ivec4 textureLod1D(isampler2D tex, float s, float lod) { return textureLod(tex, vec2(s, 0.5), lod); }
ivec4 texelFetch1D(isampler2D tex, int x, int lod) { return texelFetch(tex, ivec2(x, 0), lod); }
int textureSize1D(isampler2D tex, int lod) { return textureSize(tex, lod).x; }

// 6. Unsigned integer sampler
uvec4 texture1D(usampler2D tex, float s) { return texture(tex, vec2(s, 0.5)); }
uvec4 texture1D(usampler2D tex, float s, float bias) { return texture(tex, vec2(s, 0.5), bias); }
uvec4 textureLod1D(usampler2D tex, float s, float lod) { return textureLod(tex, vec2(s, 0.5), lod); }
uvec4 texelFetch1D(usampler2D tex, int x, int lod) { return texelFetch(tex, ivec2(x, 0), lod); }
int textureSize1D(usampler2D tex, int lod) { return textureSize(tex, lod).x; }

// 7. Image stores/loads (base)
void imageStore1D(writeonly highp image2D img, int x, vec4 data) { imageStore(img, ivec2(x, 0), data); }
vec4 imageLoad1D(readonly highp image2D img, int x) { return imageLoad(img, ivec2(x, 0)); }
int imageSize1D(readonly highp image2D img) { return imageSize(img).x; }

void imageStore1D(writeonly highp iimage2D img, int x, ivec4 data) { imageStore(img, ivec2(x, 0), data); }
ivec4 imageLoad1D(readonly highp iimage2D img, int x) { return imageLoad(img, ivec2(x, 0)); }
int imageSize1D(readonly highp iimage2D img) { return imageSize(img).x; }

void imageStore1D(writeonly highp uimage2D img, int x, uvec4 data) { imageStore(img, ivec2(x, 0), data); }
uvec4 imageLoad1D(readonly highp uimage2D img, int x) { return imageLoad(img, ivec2(x, 0)); }
int imageSize1D(readonly highp uimage2D img) { return imageSize(img).x; }

// ----- End of 1D texture wrapper -----
)";

    size_t insert_pos = find_shader_insertion_point(glsl);
    glsl.insert(insert_pos, wrapper_code + "\n");
}
static inline void inject_shaderDrawParameters(std::string& glsl) {
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
static inline void inject_temporal_filter(std::string& glsl) {
    const std::regex defRegex(R"(vec4\s+GI_TemporalFilter\s*\()", std::regex::ECMAScript);

    if (glsl.find("GI_TemporalFilter") == std::string::npos) {
        return;
    }
    if (std::regex_search(glsl, defRegex)) {
        return;
    }

    const std::regex uniformRegex(
        R"(^\s*(?:layout\s*\([^)]*\)\s*)?uniform\s+\w+(?:\s*\[\s*\d+\s*\])?\s+\w+(?:\s*\[\s*\d+\s*\])?\s*;.*$)",
        std::regex::ECMAScript | std::regex::multiline);
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

std::string preprocess_glsl(const std::string& glsl, GLenum shaderType) {
    std::string ret = glsl;
    // Remove lines beginning with `#line`
    ret = replace_line_starting_with(ret, "#line");
    // Act as if disable_GL_ARB_derivative_control is false
    replace_all(ret, "#ifdef GL_ARB_derivative_control", "#if 0");
    replace_all(ret, "#ifndef GL_ARB_derivative_control", "#if 1");
	replace_all(ret, "sample_uniform_sphere", "SampleUniformSphere");
    replace_all(ret, "_uniform_", "UniformMg");

    // Polyfill transpose()
    replace_all(ret, "const mat3 rotInverse = transpose(rot);",
                "const mat3 rotInverse = mat3(rot[0][0], rot[1][0], rot[2][0], rot[0][1], rot[1][1], rot[2][1], "
                "rot[0][2], rot[1][2], rot[2][2]);");

    //replace_all(ret, "#error ", "// #error ");
    replace_all(ret, "vec3 reflection;", "vec3 reflection=vec3(0,0,0);");
    replace_all(ret, "vec3 worldPosDiff", "vec4 worldPosDiff");
    replace_all(ret, "vec3[3](vWorldPos[0] - vWorldPos[1]", "vec4[3](vWorldPos[0] - vWorldPos[1]");

     if (shaderType == GL_VERTEX_SHADER) {
        replace_all(ret, "attribute", "in");
        replace_all(ret, "varying", "out");
    } else if (shaderType == GL_FRAGMENT_SHADER) {
        replace_all(ret, "varying", "in");
	}
	
    replace_all(ret, "texture2D", "texture");
	//inject_1d_texture_compatibility(ret);

    // GI_TemporalFilter injection
    inject_temporal_filter(ret);

	// remove "noperspective"
    const char* str_np = "noperspective";
    const std::size_t len_np = strlen(str_np);
    std::size_t noperspectivePos = ret.find(str_np);
    while (noperspectivePos != std::string::npos) {
                    // + length of "\n"
                    ret = ret.replace(noperspectivePos, len_np, "");
                    noperspectivePos = ret.find(str_np);
	}
	inject_int64_support(ret);
    inject_subgroup_BigGiftPackage(ret);
	inject_subgroup_clustered(ret);
    // inject_gl_DepthRange(ret); please use angle...
    //inject_shaderDrawParameters(ret);
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

    return ret;
}

int get_or_add_glsl_version(std::string& glsl) {
    int glsl_version = getGLSLVersion(glsl.c_str());
    if (glsl_version == -1) {
        glsl_version = 330;
        glsl.insert(0, "#version 330 compatibility\n");
    } else if (glsl_version < 330) {
        // force upgrade glsl version
        glsl = replace_line_starting_with(glsl, "#version", "#version 330 compatibility\n");
        glsl_version = 330;
    }

    LOG_D("GLSL version: %d", glsl_version)
    return glsl_version;
}

std::vector<unsigned int> glsl_to_spirv(GLenum shader_type, int glsl_version, const char* const* shader_src,
                                        int& errc) {
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
        errc = -1;
        return {};
    }

    glslang::TShader shader(shader_language);
    shader.setStrings(shader_src, 1);

    EShMessages messages = static_cast<EShMessages>(
        EShMsgDefault |
        EShMsgRelaxedErrors
    );
                
    std::string preamble = 
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#extension GL_ARB_shading_language_420pack : enable\n"
        "#extension GL_ARB_explicit_attrib_location : enable\n"
        "#extension GL_ARB_shader_texture_image_samples : enable\n"
        "#extension GL_ARB_gpu_shader5 : enable\n"
        "#extension GL_ARB_texture_cube_map_array : enable\n"
        "#extension GL_ARB_shader_storage_buffer_object : enable\n"
        "#extension GL_ARB_shader_image_load_store : enable\n"
        "#extension GL_ARB_enhanced_layouts : enable\n"
        "#extension GL_ARB_fragment_coord_conventions : enable\n";

    using namespace glslang;

	shader.setPreamble(preamble.c_str());
    shader.setEnvInput(EShSourceGlsl, shader_language, EShClientOpenGL, glsl_version);
    shader.setEnvClient(EShClientOpenGL, EShTargetOpenGL_450);
    shader.setEnvTarget(EShTargetSpv, EShTargetSpv_1_5);
    shader.setAutoMapLocations(true);
    //shader.setPreamble("#undef VULKAN\n");
    shader.setAutoMapBindings(true);

    TBuiltInResource TBuiltInResource_resources = InitResources();

    if (!shader.parse(&TBuiltInResource_resources, glsl_version, ECompatibilityProfile, true, true, messages)) {
        LOG_D("GLSL Compiling ERROR: \n%s", shader.getInfoLog())
        errc = -1;
        return {};
    }
    LOG_D("GLSL Compiled.")

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(EShMsgDefault)) {
        LOG_D("Shader Linking ERROR: %s", program.getInfoLog())
        errc = -1;
        return {};
    }
    LOG_D("Shader Linked.")
    std::vector<unsigned int> spirv_code;
    glslang::SpvOptions spvOptions;
    spvOptions.disableOptimizer = false;
    glslang::GlslangToSpv(*program.getIntermediate(shader_language), spirv_code, &spvOptions);
    errc = 0;
    return spirv_code;
}

// The context owns the ParsedIR, the compiler and every string they hand back, and the only
// destroy used to sit past the early return. A shader the ES backend rejects is a normal
// outcome and failed translations are not cached, so that leaked the lot again on every
// resource-pack reload. Scoped so no exit can skip it.
namespace {
struct spvc_context_guard_t {
    spvc_context context = nullptr;
    spvc_context_guard_t() = default;
    ~spvc_context_guard_t() {
        if (context) spvc_context_destroy(context);
    }
    spvc_context_guard_t(const spvc_context_guard_t&) = delete;
    spvc_context_guard_t& operator=(const spvc_context_guard_t&) = delete;
};
} // namespace

// SPIRV-Cross throws internally and turns that into a result code at its C boundary; on failure
// it leaves the out-parameter untouched. Dropping the code therefore hands the next call a
// handle that was never written, which crashes rather than reporting anything.
static bool spvc_ok(spvc_context context, spvc_result res, const char* what) {
    if (res == SPVC_SUCCESS) {
        return true;
    }
    LOG_E("Error: %s failed in spirv-cross: %s", what, spvc_context_get_last_error_string(context))
    return false;
}

std::string spirv_to_essl(std::vector<unsigned int> spirv, uint essl_version, int& errc) {
    spvc_parsed_ir ir = nullptr;
    spvc_compiler compiler_glsl = nullptr;
    spvc_compiler_options options = nullptr;
    const char* result = nullptr;

    const SpvId *p_spirv = spirv.data();
    size_t word_count = spirv.size();

    LOG_D("spirv_code.size(): %d", spirv.size())

    // Declared before 'essl': the compiled source lives in context-owned memory and is only
    // copied out when the std::string is constructed, so the guard has to outlive it.
    spvc_context_guard_t guard;
    if (spvc_context_create(&guard.context) != SPVC_SUCCESS || !guard.context) {
        LOG_E("Error: could not create a spirv-cross context.")
        errc = -1;
        return "";
    }
    spvc_context context = guard.context;

    if (!spvc_ok(context, spvc_context_parse_spirv(context, p_spirv, word_count, &ir), "spvc_context_parse_spirv") ||
        !ir) {
        errc = -1;
        return "";
    }
    if (!spvc_ok(context,
                 spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP,
                                              &compiler_glsl),
                 "spvc_context_create_compiler") ||
        !compiler_glsl) {
        errc = -1;
        return "";
    }
    if (!spvc_ok(context, spvc_compiler_create_compiler_options(compiler_glsl, &options),
                 "spvc_compiler_create_compiler_options") ||
        !options) {
        errc = -1;
        return "";
    }
    // A silently dropped GLSL_ES option would emit desktop GLSL and hand it straight to the
    // driver, so these are checked too.
    if (!spvc_ok(context,
                 spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_GLSL_VERSION,
                                                essl_version >= 300 ? essl_version : 300),
                 "spvc_compiler_options_set_uint") ||
        !spvc_ok(context, spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_ES, SPVC_TRUE),
                 "spvc_compiler_options_set_bool") ||
        !spvc_ok(context, spvc_compiler_install_compiler_options(compiler_glsl, options),
                 "spvc_compiler_install_compiler_options")) {
        errc = -1;
        return "";
    }
    if (!spvc_ok(context, spvc_compiler_compile(compiler_glsl, &result), "spvc_compiler_compile") || !result) {
        errc = -1;
        return "";
    }

    std::string essl = result;

    errc = 0;
    return essl;
}

static bool glslang_inited = false;
std::string GLSLtoGLSLES_2(const char* glsl_code, GLenum glsl_type, uint essl_version, int& return_code) {
    std::string correct_glsl_str = preprocess_glsl(glsl_code, glsl_type);
    LOG_D("Firstly converted GLSL:\n%s", correct_glsl_str.c_str())
    int glsl_version = get_or_add_glsl_version(correct_glsl_str);

    if (!glslang_inited) {
        glslang::InitializeProcess();
        glslang_inited = true;
    }
    const char* s[] = {correct_glsl_str.c_str()};
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
    return essl;
}

std::string GLSLtoGLSLES_1(const char* glsl_code, GLenum glsl_type, uint esversion, int& return_code) { // useless now
    /*
#if !defined(__APPLE__)
    LOG_W("Warning: use glsl optimizer to convert shader.")
    if (esversion < 300) esversion = 300;
    std::string result = MesaConvertShader(glsl_code, glsl_type == GL_VERTEX_SHADER ? GL_VERTEX_SHADER :
GL_FRAGMENT_SHADER, 460LL, esversion);

    return_code = 0;
    return result;
#else
    LOG_W_FORCE("Cannot convert glsl with version %d in MacOS/iOS", esversion);
    return std::string(glsl_code);
#endif
    */
}
