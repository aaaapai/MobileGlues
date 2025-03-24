#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "../gl/log.h"

#define DEBUG 0

static cJSON *config_json = NULL;

int initialized = 0;

int config_refresh() {
    FILE *file = fopen(CONFIG_FILE_PATH, "r");
    if (file == NULL) {
        LOG_E("Unable to open config file %s", CONFIG_FILE_PATH);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *file_content = (char *)malloc(file_size + 1);
    if (file_content == NULL) {
        LOG_E("Unable to allocate memory for file content");
        fclose(file);
        return 0;
    }

    fread(file_content, 1, file_size, file);
    fclose(file);
    file_content[file_size] = '\0';

    config_json = cJSON_Parse(file_content);
    free(file_content);

    if (config_json == NULL) {
        LOG_E("Error parsing config JSON: %s\n", cJSON_GetErrorPtr());
        return 0;
    }

    initialized = 1;
    return 1;
}

int config_get_int(const char* name) {
    if (config_json == NULL) {
        return -1;
    }

    cJSON *item = cJSON_GetObjectItem(config_json, name);
    if (item == NULL || !cJSON_IsNumber(item)) {
        LOG_D("Config item '%s' not found or not an integer.\n", name);
        return -1;
    }

    return item->valueint;
}

const char* config_get_string(char* name) {
    if (config_json == NULL) {
        return NULL;
    }

    cJSON *item = cJSON_GetObjectItem(config_json, name);
    if (item == NULL || !cJSON_IsString(item)) {
        LOG_D("Config item '%s' not found or not a string.\n", name);
        return ""; 
    }

    return item->valuestring;
}

void config_cleanup() {
    if (config_json != NULL) {
        cJSON_Delete(config_json);
        config_json = NULL;
    }
}
