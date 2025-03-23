#ifndef _MOBILEGLUES_CONFIG_H_
#define _MOBILEGLUES_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MG_DIRECTORY_PATH "/sdcard/MG"
#define CONFIG_FILE_PATH MG_DIRECTORY_PATH "/config.json"
#define LOG_FILE_PATH MG_DIRECTORY_PATH "/latest.log"
#define GLSL_CACHE_FILE_PATH MG_DIRECTORY_PATH "/glsl_cache.tmp"

extern int initialized;
    
int config_refresh();
int config_get_int(const char* name);
char* config_get_string(char* name);
void config_cleanup();

#ifdef __cplusplus
}
#endif

#endif // _MOBILEGLUES_CONFIG_H_
