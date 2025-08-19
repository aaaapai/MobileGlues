cmake_minimum_required(VERSION 3.22.1)

project("mobileglues")

enable_language(C)
enable_language(CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MACOS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -fvisibility=default -funwind-tables -g -D_THREAD_SAFE -fPIC -stdlib=libc++")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w -g -std=gnu99 -funwind-tables -O3 -fvisibility=hidden")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -pipe -integrated-as -mllvm -polly -mllvm -polly-vectorizer=stripmine -mllvm -polly-invariant-load-hoisting -mllvm -polly-run-inliner -mllvm -polly-run-dce -mllvm -polly-invariant-load-hoisting -flto -mllvm -polly-run-inliner -mllvm -polly-run-dce -fno-emulated-tls -fwhole-program-vtables -fdata-sections -ffunction-sections -fmerge-all-constants -mllvm -polly-detect-keep-going -mllvm -polly-ast-use-context -march=armv8-a+simd -fvisibility=hidden -Wall -Wextra -ferror-limit=0 -Wno-return-type -Wno-unused-parameter -Wno-c++98-compat -Wno-c++98-compat-pedantic -DGLM_FORCE_INTRINSICS -Wno-unicode-whitespace -Wno-format -Wno-unused-function")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pipe -integrated-as -mllvm -polly -mllvm -polly-vectorizer=stripmine -mllvm -polly-invariant-load-hoisting -mllvm -polly-run-inliner -mllvm -polly-run-dce -mllvm -polly-invariant-load-hoisting -flto -mllvm -polly-run-inliner -mllvm -polly-run-dce -fno-emulated-tls -fwhole-program-vtables -fdata-sections -ffunction-sections -fmerge-all-constants -mllvm -polly-detect-keep-going -mllvm -polly-ast-use-context -march=armv8-a+simd -fvisibility=hidden -Wall -Wextra -ferror-limit=0 -Wno-return-type -Wno-unused-parameter -Wno-c++98-compat -Wno-c++98-compat-pedantic -DGLM_FORCE_INTRINSICS -fno-rtti -stdlib=libc++ -Wno-unicode-whitespace -Wno-format -Wno-unused-function -Wno-vla-cxx-extension")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld -flto -O3 -Wl,-plugin-opt=-emulated-tls=0 -Wl,--strip-all -fvisibility=hidden -Wl,-O3 -Wl,--gc-sections -Wl,--as-needed -static-libstdc++")
        set(CMAKE_ANDROID_LD "lld")
endif()

set(CMAKE_BUILD_TYPE Release)

set(PROFILING OFF)

find_library(GLSLANG_LIB glslang PATHS ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/)

add_library(${CMAKE_PROJECT_NAME} SHARED
        init.cpp
        main.cpp
        gl/gl_stub.cpp
        gl/gl_native.cpp
        gl/gl.cpp
        gl/envvars.cpp
        gl/log.cpp
        gl/program.cpp
        gl/shader.cpp
        gl/framebuffer.cpp
        gl/texture.cpp
        gl/drawing.cpp
        gl/multidraw.cpp
        gl/mg.cpp
        gl/buffer.cpp
        gl/getter.cpp
        gl/pixel.cpp
        gl/vertexattrib.cpp
        gl/ExtWrappers/DSAWrapper.cpp
        gl/ExtWrappers/MultiBindWrapper.cpp
        gl/glsl/glsl_for_es.cpp
        gl/glsl/cache.cpp
        gl/FSR1/FSR1.cpp
        glx/lookup.cpp
        egl/egl.cpp
        egl/loader.cpp
        gles/loader.cpp
        config/cJSON.c
        config/config.cpp
        config/gpu_utils.cpp
        config/settings.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ./includes
        ./3rdparty/glm
        ./3rdparty/xxhash)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

if(MACOS)
        # iOS
        set(iOS_LIB_DIR ${CMAKE_SOURCE_DIR}/libraries/ios)
        target_link_libraries(${CMAKE_PROJECT_NAME}
                ${iOS_LIB_DIR}/libglslang.a
                ${iOS_LIB_DIR}/libspirv-cross-c-shared.dylib
                ${iOS_LIB_DIR}/libGenericCodeGen.a
                ${iOS_LIB_DIR}/libglslang-default-resource-limits.a
                ${iOS_LIB_DIR}/libMachineIndependent.a
                ${iOS_LIB_DIR}/libOSDependent.a
                ${iOS_LIB_DIR}/libSPIRV-Tools-diff.a
                ${iOS_LIB_DIR}/libSPIRV-Tools-link.a
                ${iOS_LIB_DIR}/libSPIRV-Tools-lint.a
                ${iOS_LIB_DIR}/libSPIRV-Tools-opt.a
                ${iOS_LIB_DIR}/libSPIRV-Tools-reduce.a
                ${iOS_LIB_DIR}/libSPIRV-Tools.a
                ${iOS_LIB_DIR}/libSPIRV.a
                ${iOS_LIB_DIR}/libSPVRemapper.a
                "-F${CMAKE_SOURCE_DIR}/external"
                "-framework libEGL"
                "-framework libGLESv2"
                "-framework Metal"
                "-framework Foundation"
                "-framework QuartzCore"
        )
else()
        # Android
        find_library(GLSLANG_LIB glslang PATHS ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/)
        target_link_libraries(${CMAKE_PROJECT_NAME}
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libglslang.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libGenericCodeGen.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libglslang-default-resource-limits.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libMachineIndependent.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libOSDependent.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libSPIRV-Tools-diff.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libSPIRV-Tools-link.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libSPIRV-Tools-lint.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libSPIRV-Tools-opt.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libSPIRV-Tools-reduce.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libSPIRV-Tools.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libSPIRV.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libSPVRemapper.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libspirv-cross-c.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libspirv-cross-core.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libspirv-cross-cpp.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libspirv-cross-glsl.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libspirv-cross-hlsl.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libspirv-cross-reflect.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libspirv-cross-util.a
                ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libshaderconv.a
                android
                log
        )
endif()

if (PROFILING)
        find_package(Threads)
        include_directories(${CMAKE_SOURCE_DIR}/3rdparty/perfetto/sdk)
        add_library(perfetto STATIC ${CMAKE_SOURCE_DIR}/3rdparty/perfetto/sdk/perfetto.cc)
        target_link_libraries(${CMAKE_PROJECT_NAME} perfetto ${CMAKE_THREAD_LIBS_INIT})
        target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC PROFILING=1)
endif()
