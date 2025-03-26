#ifndef _MOBILEGLUES_CONFIG_H_
#define _MOBILEGLUES_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

extern const char* mg_directory_path;
extern const char* config_file_path;
extern const char* log_file_path;
extern const char* glsl_cache_file_path;

extern int initialized;

char* concatenate(const char* str1, const char* str2);

int check_path();

int config_refresh();
int config_get_int(const char* name);
char* config_get_string(const char* name);
void config_cleanup();

#ifdef __cplusplus
}
#endif

#endif // _MOBILEGLUES_CONFIG_H_
