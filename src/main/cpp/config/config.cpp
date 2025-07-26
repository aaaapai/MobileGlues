#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "cJSON.h"
#include "../gl/log.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#define DEBUG 0

std::string DEFAULT_MG_DIRECTORY_PATH = "/sdcard/MG_AP";

std::string mg_directory_path;
std::string config_file_path;
std::string log_file_path;
std::string glsl_cache_file_path;

static cJSON *config_json = nullptr;

int initialized = 0;

std::string concatenate(std::string str1, std::string str2) {
    return str1 + str2;
}

int check_path() {
    mg_directory_path = std::string(std::getenv("MG_DIR_PATH") ? std::getenv("MG_DIR_PATH") : DEFAULT_MG_DIRECTORY_PATH);
    config_file_path = std::string(mg_directory_path) + "/config.json";
    log_file_path = concatenate(mg_directory_path, "/latest.log");
    glsl_cache_file_path = concatenate(mg_directory_path, "/glsl_cache.tmp");

    if (!std::filesystem::exists(mg_directory_path)) {
      if (!std::filesystem::create_directory(mg_directory_path)) {
        LOG_E("Error creating MG directory.\n")
        return 0;
      }
    }

    return 1;
}

int config_refresh() {
    LOG_D("MG_DIRECTORY_PATH=%s", mg_directory_path.c_str())
    LOG_D("CONFIG_FILE_PATH=%s", config_file_path.c_str())
    LOG_D("LOG_FILE_PATH=%s", log_file_path.c_str())
    LOG_D("GLSL_CACHE_FILE_PATH=%s", glsl_cache_file_path.c_str())

    std::ifstream file(config_file_path);
    if (!file.is_open()) {
       LOG_E("Unable to open config file %s", config_file_path.c_str());
       return 0;
    }

    // 读取整个文件内容到 std::string
    std::string file_content((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
    // cJSON 只接受 C 字符串，需要 .c_str()
    config_json = cJSON_Parse(file_content.c_str());

    if (config_json == nullptr) {
        LOG_E("Error parsing config JSON: %s\n", cJSON_GetErrorPtr());
        return 0;
    }

    initialized = 1;
    return 1;
}

int config_get_int(const std::string& name) {
    if (config_json == nullptr) {
        return -1;
    }

    cJSON *item = cJSON_GetObjectItem(config_json, name.c_str());
    if (item == nullptr || !cJSON_IsNumber(item)) {
        LOG_D("Config item '%s' not found or not an integer.\n", name.c_str());
        return -1;
    }

    return item->valueint;
}

std::string config_get_string(const std::string& name) {
    if (config_json == nullptr) {
        return "";
    }

    cJSON *item = cJSON_GetObjectItem(config_json, name.c_str());
    if (item == nullptr || !cJSON_IsString(item)) {
        LOG_D("Config item '%s' not found or not a string.\n", name.c_str());
        return "";
    }

    return std::string(item->valuestring ? item->valuestring : "");
}

void config_cleanup() {
    if (config_json != nullptr) {
        cJSON_Delete(config_json);
        config_json = nullptr;
    }
}
