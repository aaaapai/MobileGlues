//
// Created by hanji on 2025/2/9.
//

#include <android/dlext.h>
#include "settings.h"
#include "config.h"
#include "../gl/log.h"
#include "../gl/envvars.h"
#include "gpu_utils.h"
#include <cinttypes>
#include <vector>
#include "nsbypass.h"
#include "vulkan/vulkan_core.h"

#define DEBUG 1

struct global_settings_t global_settings;

void* loadTurnipVulkan() {
    printf("loadTurnipVulkan");
    const char* native_dir = getenv("DRIVER_PATH");
    const char* cache_dir = getenv("TMPDIR");

    if (!native_dir)
        printf("no native_dir");

    if (!linker_ns_load(native_dir))
        printf("!linker_ns_load(native_dir)");

    void* linkerhook = linker_ns_dlopen("liblinkerhook.so", RTLD_LOCAL | RTLD_NOW);
    if (!linkerhook)
        printf("!linkerhook");

    void* turnip_driver_handle = linker_ns_dlopen("libvulkan_freedreno.so", RTLD_LOCAL | RTLD_NOW);
    if (!turnip_driver_handle) {
        printf("(!turnip_driver_handle");
        dlclose(linkerhook);
    }

    void* dl_android = linker_ns_dlopen("libdl_android.so", RTLD_LOCAL | RTLD_LAZY);
    if (!dl_android) {
        dlclose(linkerhook);
        dlclose(turnip_driver_handle);
    }

    void* android_get_exported_namespace = dlsym(dl_android, "android_get_exported_namespace");
    auto linkerhookPassHandles = (void (*)(void*, void* _Nullable(const char* _Nullable __filename, int __flags, const android_dlextinfo* _Nullable __info), void*))dlsym(linkerhook, "linker_hook_set_handles");
    if (!linkerhookPassHandles || !android_get_exported_namespace) {
        dlclose(dl_android);
        dlclose(linkerhook);
        dlclose(turnip_driver_handle);
    }

    linkerhookPassHandles(turnip_driver_handle, android_dlopen_ext, android_get_exported_namespace);
    
    void* libvulkan = linker_ns_dlopen_unique(cache_dir, "libvulkan_freedreno.so", RTLD_LOCAL | RTLD_NOW);
    if (!libvulkan) {
        dlclose(dl_android);
        dlclose(linkerhook);
        dlclose(turnip_driver_handle);
    }

    return libvulkan;
}

int show_vk_v() {
    const char *envval = getenv("VULKAN_PTR");
    if (!envval) {
        printf( "VULKAN_PTR environment variable not set\n");
        return 1;
    }
    uintptr_t ptr_value;
    if (sscanf(envval, "%" SCNxPTR, &ptr_value) != 1) {
        printf( "Failed to parse VULKAN_PTR value\n");
        return 1;
    }
    void *vulkan_handle = (void *)ptr_value;

    auto TvkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(vulkan_handle, "vkGetInstanceProcAddr");
    if (!TvkGetInstanceProcAddr) {
        printf( "Failed to get TvkGetInstanceProcAddr: %s\n", dlerror());
        return 1;
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Query";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto TvkCreateInstance = (PFN_vkCreateInstance)TvkGetInstanceProcAddr(nullptr, "vkCreateInstance");
    if (!TvkCreateInstance) {
        printf( "Failed to get TvkCreateInstance: %s\n", dlerror());
        return 1;
    }

    VkInstance instance;
    if (TvkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        printf( "Failed to create Vulkan instance\n");
        return 1;
    }
    
    uint32_t version;
    auto TvkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)TvkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion");
    if (TvkEnumerateInstanceVersion) {
        if (TvkEnumerateInstanceVersion(&version) != VK_SUCCESS) {
            version = VK_API_VERSION_1_0;
        }
    } else {
        version = VK_API_VERSION_1_0;
    }

    printf("Vulkan API Version: %d.%d.%d\n",
           VK_VERSION_MAJOR(version),
           VK_VERSION_MINOR(version),
           VK_VERSION_PATCH(version));

    auto TvkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)TvkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices");
    if (!TvkEnumeratePhysicalDevices) {
        printf("vkEnumeratePhysicalDevices not available\n");

        auto TvkDestroyInstance = (PFN_vkDestroyInstance)TvkGetInstanceProcAddr(instance, "vkDestroyInstance");
        if (TvkDestroyInstance) TvkDestroyInstance(instance, nullptr);
        return 1;
    }

    uint32_t deviceCount = 0;
    TvkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        printf("No Vulkan devices found\n");

        auto TvkDestroyInstance = (PFN_vkDestroyInstance)TvkGetInstanceProcAddr(instance, "vkDestroyInstance");
        if (TvkDestroyInstance) TvkDestroyInstance(instance, nullptr);
        return 1;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    TvkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    auto TvkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)TvkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties");
    if (!TvkGetPhysicalDeviceProperties) {
        printf("Cannot get device properties\n");

        auto TvkDestroyInstance = (PFN_vkDestroyInstance)TvkGetInstanceProcAddr(instance, "vkDestroyInstance");
        if (TvkDestroyInstance) TvkDestroyInstance(instance, nullptr);
        return 1;
    }

    printf("\nFound %d Vulkan devices:\n", deviceCount);
    for (uint32_t i = 0; i < deviceCount; i++) {
        VkPhysicalDeviceProperties deviceProperties;
        TvkGetPhysicalDeviceProperties(devices[i], &deviceProperties);

        printf("\nDevice %d:\n", i+1);
        printf("  Name: %s\n", deviceProperties.deviceName);
        printf("  API Version: %d.%d.%d\n",
               VK_VERSION_MAJOR(deviceProperties.apiVersion),
               VK_VERSION_MINOR(deviceProperties.apiVersion),
               VK_VERSION_PATCH(deviceProperties.apiVersion));
        printf("  Driver Version: %d.%d.%d\n",
               VK_VERSION_MAJOR(deviceProperties.driverVersion),
               VK_VERSION_MINOR(deviceProperties.driverVersion),
               VK_VERSION_PATCH(deviceProperties.driverVersion));
        std::string deviceTypeToString(VkPhysicalDeviceType type);
        printf("  Device Type: %s\n", deviceTypeToString(deviceProperties.deviceType).c_str());
    }

    auto TvkDestroyInstance = (PFN_vkDestroyInstance)TvkGetInstanceProcAddr(instance, "vkDestroyInstance");
    if (TvkDestroyInstance) {
        TvkDestroyInstance(instance, nullptr);
    } else {
        printf("Warning: vkDestroyInstance not found\n");
    }

    return 0;
}

std::string deviceTypeToString(VkPhysicalDeviceType type) {
    switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "Integrated GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return "Discrete GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return "Virtual GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU: return "CPU";
        default: return "Other";
    }
}

static void set_vulkan_ptr(void* ptr) {
    printf("set_vulkan_ptr");
    char envval[64];
    sprintf(envval, "%" PRIxPTR, (uintptr_t)ptr);
    setenv("VULKAN_PTR", envval, 1);
}

void load_vulkan() {
    loadTurnipVulkan();
    void* result = loadTurnipVulkan();
    if(result != nullptr) {
        printf("AdrenoSupp: Loaded Turnip, loader address: %p\n", result);
        set_vulkan_ptr(result);
        show_vk_v();
        return;
    }
}

void init_settings() {
    int success = initialized;
    if (!success) {
        success = config_refresh();
        if (!success) {
            LOG_V("Failed to load config. Use default config.")
        }
    }

    int enableANGLE = success ? config_get_int("enableANGLE") : 0;
    int enableNoError = success ? config_get_int("enableNoError") : 0;
    int enableExtGL43 = success ? config_get_int("enableExtGL43") : 0;
    int enableExtComputeShader = success ? config_get_int("enableExtComputeShader") : 0;
    int enableCompatibleMode = success ? config_get_int("enableCompatibleMode") : 0;
    multidraw_mode_t multidrawMode = success ? (multidraw_mode_t)config_get_int("multidrawMode") : multidraw_mode_t::Auto;
//    multidraw_mode_t multidrawMode = multidraw_mode_t::PreferUnroll;
    size_t maxGlslCacheSize = 0;
    if (config_get_int("maxGlslCacheSize") > 0)
        maxGlslCacheSize = success ? config_get_int("maxGlslCacheSize") * 1024 * 1024 : 0;

    if (enableANGLE < 0 || enableANGLE > 3)
        enableANGLE = 0;
    if (enableNoError < 0 || enableNoError > 3)
        enableNoError = 0;
    if (enableExtGL43 < 0 || enableExtGL43 > 1)
        enableExtGL43 = 0;
    if (enableExtComputeShader < 0 || enableExtComputeShader > 1)
        enableExtComputeShader = 0;
    if (enableCompatibleMode < 0 || enableCompatibleMode > 1)
        enableCompatibleMode = 0;
    if ((int)multidrawMode < 0 || (int)multidrawMode > 4)
        multidrawMode = multidraw_mode_t::Auto;

    // 1205
    int fclVersion = 0;
    GetEnvVarInt("FCL_VERSION_CODE", &fclVersion, 0);
    // 140000
    int zlVersion = 0;
    GetEnvVarInt("ZALITH_VERSION_CODE", &zlVersion, 0);
    // unknown
    int pgwVersion = 0;
    GetEnvVarInt("PGW_VERSION_CODE", &pgwVersion, 0);

    char* var = getenv("MG_DIR_PATH");

    if (fclVersion == 0 && zlVersion == 0 && pgwVersion == 0 && !var) {
        LOG_V("Unsupported launcher detected, force using default config.")
        enableANGLE = 0;
        enableNoError = 0;
        enableExtGL43 = 0;
        enableExtComputeShader = 0;
        maxGlslCacheSize = 0;
        enableCompatibleMode = 0;
    }

    // Determining actual ANGLE mode
    const char* gpuString = getGPUInfo();
    LOG_D("GPU: %s", gpuString)

    if (enableANGLE == 2 || enableANGLE == 3) {
        // Force enable / disable
        global_settings.angle = enableANGLE - 2;
    } else {
        int isQcom = isAdreno(gpuString);
        int is740 = isAdreno740(gpuString);
        //int is830 = isAdreno830(gpuString);
        int hasVk13 = 1;

        LOG_D("Is Adreno? = %s", isQcom ? "true" : "false")
        //LOG_D("Is Adreno 830? = %s", is830 ? "true" : "false")
        LOG_D("Is Adreno 740? = %s", is740 ? "true" : "false")
        LOG_D("Has Vulkan 1.3? = %s", hasVk13 ? "true" : "false")

        //if (is830)
        //    global_settings.angle = 1;
        if (is740)
            global_settings.angle = 0;
        else
            global_settings.angle = hasVk13 && enableANGLE;
    }
    LOG_D("enableANGLE = %d", enableANGLE)
    LOG_D("global_settings.angle = %d", global_settings.angle)

//    if (enableANGLE == 1) {
//        global_settings.angle = (isAdreno740(gpuString) || !hasVulkan13()) ? 0 : 1;
//    } else if (enableANGLE == 2 || enableANGLE == 3) {
//        global_settings.angle = enableANGLE - 2;
//    } else {
//        int is830 = isAdreno830(gpuString);
//        LOG_D("Is Adreno 830? = %s", is830 ? "true" : "false")
//        global_settings.angle = is830 ? 1 : 0;
//    }


    if (global_settings.angle) {
        setenv("LIBGL_GLES", "libGLESv2_angle.so", 1);
        setenv("LIBGL_EGL", "libEGL_angle.so", 1);
        load_vulkan();
        printf("load_vulkan");
    }

    if (enableNoError == 1 || enableNoError == 2 || enableNoError == 3) {
        global_settings.ignore_error = enableNoError - 1;
    } else {
        global_settings.ignore_error = 0;
    }

    global_settings.ext_gl43 = enableExtGL43;

    global_settings.ext_compute_shader = enableExtComputeShader;
    
    global_settings.max_glsl_cache_size = maxGlslCacheSize;

    global_settings.enable_compatible_mode = enableCompatibleMode;

    global_settings.multidraw_mode = multidrawMode;

    std::string draw_mode_str;
    switch (global_settings.multidraw_mode) {
        case multidraw_mode_t::PreferIndirect:
            draw_mode_str = "Indirect";
            break;
        case multidraw_mode_t::PreferBaseVertex:
            draw_mode_str = "Unroll";
            break;
        case multidraw_mode_t::PreferMultidrawIndirect:
            draw_mode_str = "Multidraw indirect";
            break;
        case multidraw_mode_t::DrawElements:
            draw_mode_str = "DrawElements";
            break;
        case multidraw_mode_t::Auto:
            draw_mode_str = "Auto";
            break;
        default:
            draw_mode_str = "(Unknown)";
            global_settings.multidraw_mode = multidraw_mode_t::Auto;
            break;
    }

    LOG_V("[MobileGlues] Setting: enableAngle            = %s", global_settings.angle ? "true" : "false")
    LOG_V("[MobileGlues] Setting: ignoreError            = %i", global_settings.ignore_error)
    LOG_V("[MobileGlues] Setting: enableExtComputeShader = %s", global_settings.ext_compute_shader ? "true" : "false")
    LOG_V("[MobileGlues] Setting: enableExtGL43          = %s", global_settings.ext_gl43 ? "true" : "false")
    LOG_V("[MobileGlues] Setting: maxGlslCacheSize       = %i", global_settings.max_glsl_cache_size / 1024 / 1024)
    LOG_V("[MobileGlues] Setting: enableCompatibleMode   = %s", global_settings.enable_compatible_mode ? "true" : "false")
    LOG_V("[MobileGlues] Setting: multidrawMode          = %s", draw_mode_str.c_str())
}

void init_settings_post() {
    bool multidraw = g_gles_caps.GL_EXT_multi_draw_indirect;
    bool basevertex =
            g_gles_caps.GL_OES_draw_elements_base_vertex ||
            (g_gles_caps.major == 3 && g_gles_caps.minor >= 2) || (g_gles_caps.major > 3);
    bool indirect = (g_gles_caps.major == 3 && g_gles_caps.minor >= 1) || (g_gles_caps.major > 3);

    switch (global_settings.multidraw_mode) {
        case multidraw_mode_t::PreferIndirect:
            LOG_V("multidrawMode = PreferIndirect")
            if (indirect) {
                global_settings.multidraw_mode = multidraw_mode_t::PreferIndirect;
                LOG_V("    -> Indirect (OK)")
            } else if (basevertex) {
                global_settings.multidraw_mode = multidraw_mode_t::PreferBaseVertex;
                LOG_V("    -> BaseVertex (Preferred not supported, falling back)")
            } else {
                global_settings.multidraw_mode = multidraw_mode_t::DrawElements;
                LOG_V("    -> DrawElements (Preferred not supported, falling back)")
            }
            break;
        case multidraw_mode_t::PreferBaseVertex:
            LOG_V("multidrawMode = PreferBaseVertex")
            if (basevertex) {
                global_settings.multidraw_mode = multidraw_mode_t::PreferBaseVertex;
                LOG_V("    -> BaseVertex (OK)")
            } else if (multidraw) {
                global_settings.multidraw_mode = multidraw_mode_t::PreferMultidrawIndirect;
                LOG_V("    -> MultidrawIndirect (Preferred not supported, falling back)")
            } else if (indirect) {
                global_settings.multidraw_mode = multidraw_mode_t::PreferIndirect;
                LOG_V("    -> Indirect (Preferred not supported, falling back)")
            } else {
                global_settings.multidraw_mode = multidraw_mode_t::DrawElements;
                LOG_V("    -> DrawElements (Preferred not supported, falling back)")
            }
            break;
        case multidraw_mode_t::DrawElements:
            LOG_V("multidrawMode = DrawElements")
            global_settings.multidraw_mode = multidraw_mode_t::DrawElements;
            break;
        case multidraw_mode_t::Auto:
        default:
            LOG_V("multidrawMode = Auto")
            if (multidraw) {
                global_settings.multidraw_mode = multidraw_mode_t::PreferMultidrawIndirect;
                LOG_V("    -> MultidrawIndirect (Auto detected)")
            } else if (indirect) {
                global_settings.multidraw_mode = multidraw_mode_t::PreferIndirect;
                LOG_V("    -> Indirect (Auto detected)")
            } else if (basevertex) {
                global_settings.multidraw_mode = multidraw_mode_t::PreferBaseVertex;
                LOG_V("    -> BaseVertex (Auto detected)")
            } else {
                global_settings.multidraw_mode = multidraw_mode_t::DrawElements;
                LOG_V("    -> DrawElements (Auto detected)")
            }
            break;
    }
}