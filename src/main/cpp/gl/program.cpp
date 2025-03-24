//
// Created by hanji on 2025/2/3.
//

#include <regex.h>
#include "log.h"
#include "shader.h"
#include "program.h"
#include <regex>
#include <cstring>
#include <iostream>
#include "../config/settings.h"

#define DEBUG 0

char* removeLayoutLocations(const char* input) {
    std::string code(input);
    std::regex pattern(R"(layout\s*\(\s*location\s*=\s*\d+\s*\))");
    std::string result = std::regex_replace(code, pattern, "");
    char* output = new char[result.length() + 1];
    strcpy(output, result.c_str());
    return output;
}

void glBindFragDataLocation(GLuint program, GLuint color, const GLchar *name) {
    LOG()
    LOG_D("glBindFragDataLocation(%d, %d, %s)", program, color, name)
//    if (g_gles_caps.GL_EXT_blend_func_extended) {
//        GLES.glBindFragDataLocationEXT(program, color, name);
//    } else {
//        LOG_W("Warning: No GL_EXT_blend_func_extended, skipping glBindFragDataLocation...");
//    }
    
    if (strlen(name) > 8 && strncmp(name, "outColor", 8) == 0) {
        const char* numberStr = name + 8;
        bool isNumber = true;
        for (int i = 0; numberStr[i] != '\0'; ++i) {
            if (!isdigit(numberStr[i])) {
                isNumber = false;
                break;
            }
        }

        if (isNumber) {
            unsigned int extractedColor = static_cast<unsigned int>(std::stoul(numberStr));
            if (extractedColor == color) {
                // outColor was bound in glsl process. exit now
                LOG_D("Find outColor* with color *, skipping")
                return;
            }
        }
    }

    char* origin_glsl = nullptr;
    if (shaderInfo.frag_data_changed) {
        size_t glslLen  = strlen(shaderInfo.frag_data_changed_converted) + 1;
        origin_glsl = (char *)malloc(glslLen);
        if (origin_glsl == nullptr) {
            LOG_E("Memory reallocation failed for frag_data_changed_converted\n")
            return;
        }
        strcpy(origin_glsl, shaderInfo.frag_data_changed_converted);
    } else {
        size_t glslLen  = shaderInfo.converted.length() + 1;
        origin_glsl = (char *)malloc(glslLen);
        if (origin_glsl == nullptr) {
            LOG_E("Memory reallocation failed for converted\n")
            return;
        }
        strcpy(origin_glsl, shaderInfo.converted.c_str());
    }

    int len = strlen(name);
    int tlen = len + 32;
    char *targetPattern = (char*)malloc(sizeof(char) * tlen);
    if (!targetPattern) {
        LOG_E("Memory allocation failed for targetPattern")
        return;
    }
    sprintf(targetPattern, "out[ ]+[A-Za-z0-9 ]+[ ]+%s", name);
    regex_t regex;
    regmatch_t pmatch[1];
    char *origin = nullptr;
    char *result = nullptr;
    if (regcomp(&regex, targetPattern, REG_EXTENDED) != 0) {
        LOG_E("Failed to compile regex\n")
        return;
    }
    char *searchStart = origin_glsl;
    while (regexec(&regex, searchStart, 1, pmatch, 0) == 0) {
        size_t matchLen = pmatch[0].rm_eo - pmatch[0].rm_so;
        origin = (char *) malloc(matchLen + 1);
        if (!origin) {
            LOG_E("Memory allocation failed\n")
            break;
        }
        strncpy(origin, searchStart + pmatch[0].rm_so, matchLen);
        origin[matchLen] = '\0';

        size_t resultLen =
                strlen(origin) + 30; // "layout (location = )" + colorNumber + null terminator
        result = (char *) malloc(resultLen);
        if (!result) {
            LOG_E("Memory allocation failed\n")
            free(origin);
            break;
        }

        origin = removeLayoutLocations(origin);
        
        snprintf(result, resultLen, "layout (location = %d) %s", color, origin);

        char *temp = strstr(searchStart, origin);
        if (temp) {
            size_t prefixLen = temp - origin_glsl;
            size_t suffixLen = strlen(temp + matchLen);
            size_t newLen = prefixLen + strlen(result) + suffixLen + 1;

            char *newConverted = (char *) malloc(newLen);
            if (!newConverted) {
                LOG_E("Memory allocation failed\n")
                free(origin);
                free(result);
                break;
            }

            strncpy(newConverted, origin_glsl, prefixLen);
            newConverted[prefixLen] = '\0';
            strcat(newConverted, result);
            strcat(newConverted, temp + matchLen);

            size_t newLen_2 = strlen(newConverted) + 1;
            shaderInfo.frag_data_changed_converted = (char *) realloc(
                    shaderInfo.frag_data_changed_converted, newLen_2);
            if (shaderInfo.frag_data_changed_converted == nullptr) {
                LOG_E("Memory reallocation failed for frag_data_changed_converted\n")
                return;
            }
            strcpy(shaderInfo.frag_data_changed_converted, newConverted);
            free(newConverted);
        }

        free(origin);
        free(result);

        searchStart += pmatch[0].rm_eo;
    }
    free(targetPattern);
    regfree(&regex);//LOG_D("Patched shader:\n%s", origin_glsl)
    free(origin_glsl);
    shaderInfo.frag_data_changed = 1;
}

void glLinkProgram(GLuint program) {
    LOG()

    LOG_D("glLinkProgram(%d)", program)
    if (!shaderInfo.converted.empty() && shaderInfo.frag_data_changed) {
        GLES.glShaderSource(shaderInfo.id, 1, (const GLchar * const*) &shaderInfo.frag_data_changed_converted, nullptr);
        GLES.glCompileShader(shaderInfo.id);
        GLint status = 0;
        GLES.glGetShaderiv(shaderInfo.id, GL_COMPILE_STATUS, &status);
        if(status!=GL_TRUE) {
            char tmp[500];
            GLES.glGetShaderInfoLog(shaderInfo.id, 500, nullptr, tmp);
            LOG_E("Failed to compile patched shader, log:\n%s", tmp)
        }
        GLES.glDetachShader(program, shaderInfo.id);
        GLES.glAttachShader(program, shaderInfo.id);
        CHECK_GL_ERROR
    }
    shaderInfo.id = 0;
    shaderInfo.converted = "";
    shaderInfo.frag_data_changed_converted = nullptr;
    shaderInfo.frag_data_changed = 0;
    GLES.glLinkProgram(program);

    CHECK_GL_ERROR
}

void glGetProgramiv(GLuint program, GLenum pname, GLint *params) {
    LOG()
    GLES.glGetProgramiv(program, pname, params);
    if(global_settings.ignore_error >= 1 && (pname == GL_LINK_STATUS || pname == GL_VALIDATE_STATUS) && !*params) {
        GLchar infoLog[512];
        GLES.glGetProgramInfoLog(program, 512, nullptr, infoLog);

        LOG_W_FORCE("Program %d linking failed: \n%s", program, infoLog);
        LOG_W_FORCE("Now try to cheat.");
        *params = GL_TRUE;
    }
    CHECK_GL_ERROR
}