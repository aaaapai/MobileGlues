#ifndef _MOBILEGLUES_CONFIG_H_
#define _MOBILEGLUES_CONFIG_H_

#include <string>

#ifdef __cplusplus
extern "C" {
#endif

extern std::string mg_directory_path;
extern std::string config_file_path;
extern std::string log_file_path;
extern std::string glsl_cache_file_path;

extern int initialized;

std::string concatenate(std::string str1, std::string str2);

int check_path();

int config_refresh();
int config_get_int(std::string name);
std::string config_get_string(std::string name);
void config_cleanup();

#ifdef __cplusplus
}
#endif

#endif // _MOBILEGLUES_CONFIG_H_
