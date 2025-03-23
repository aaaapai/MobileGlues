cmake_minimum_required(VERSION 3.31.5)

project("mobileglues")

enable_language(CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -pipe -integrated-as -mllvm -polly -mllvm -polly-vectorizer=stripmine -mllvm -polly-invariant-load-hoisting -mllvm -polly-run-inliner -mllvm -polly-run-dce -mllvm -polly-invariant-load-hoisting -flto=thin -mllvm -polly-run-inliner -mllvm -polly-run-dce -mllvm -polly-parallel -fopenmp=libomp -mllvm -polly-omp-backend=LLVM -mllvm -polly-scheduling=dynamic -flto=thin -fno-emulated-tls -fwhole-program-vtables -fdata-sections -ffunction-sections -fmerge-all-constants -mllvm -hot-cold-split=true -mllvm -polly-detect-keep-going -mllvm -polly-ast-use-context -mllvm -regalloc-enable-priority-advisor=release -mllvm -regalloc-priority-interactive-channel-base=foo -DNDEBUG -march=armv8-a+simd -fvisibility=hidden -Wno-format -Wno-return-type -ferror-limit=0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pipe -integrated-as -mllvm -polly -mllvm -polly-vectorizer=stripmine -mllvm -polly-invariant-load-hoisting -mllvm -polly-run-inliner -mllvm -polly-run-dce -mllvm -polly-invariant-load-hoisting -flto=thin -mllvm -polly-run-inliner -mllvm -polly-run-dce -mllvm -polly-parallel -fopenmp=libomp -mllvm -polly-omp-backend=LLVM -mllvm -polly-scheduling=dynamic -flto=thin -fno-emulated-tls -fwhole-program-vtables -fdata-sections -ffunction-sections -fmerge-all-constants -mllvm -hot-cold-split=true -mllvm -polly-detect-keep-going -mllvm -polly-ast-use-context -mllvm -regalloc-enable-priority-advisor=release -mllvm -regalloc-priority-interactive-channel-base=foo -DNDEBUG -march=armv8-a+simd -fvisibility=hidden -Wno-format -Wno-return-type -ferror-limit=0")
set(LINK_OPTIONS "${CMAKE_LINK_OPTIONS} -fuse-ld=lld -flto=thin -Wl,-plugin-opt=-emulated-tls=0")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINK_OPTIONS} -fuse-ld=lld -flto=thin -Wl,-plugin-opt=-emulated-tls=0")
set(CMAKE_ANDROID_STL_TYPE system)
set(CMAKE_ANDROID_LD "lld")

set(CMAKE_BUILD_TYPE Release)

set(PROFILING OFF)

find_library(GLSLANG_LIB glslang PATHS ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/)

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
        gl/mg.cpp
        gl/buffer.cpp
        gl/getter.cpp
        gl/pixel.cpp
        gl/glsl/glsl_for_es.cpp
        gl/glsl/cache.cpp
        gl/glsl/preConvertedGlsl.cpp
        gl/vertexattrib.cpp
        glx/lookup.cpp
        egl/egl.c
        egl/loader.cpp
        gles/loader.cpp
        config/cJSON.c
        config/config.cpp
        config/gpu_utils.cpp
        config/settings.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ./includes)

target_link_libraries(${CMAKE_PROJECT_NAME}
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libglslang.a
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libspirv-cross-c-shared.so
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libshaderconv.so
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libGenericCodeGen.a
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libglslang-default-resource-limits.a
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libMachineIndependent.a
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libOSDependent.a
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libSPIRV-Tools-diff.a
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libSPIRV-Tools-link.a
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libSPIRV-Tools-lint.a
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libSPIRV-Tools-opt.a
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libSPIRV-Tools-reduce.a
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libSPIRV-Tools.a
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libSPIRV.a
        ${CMAKE_SOURCE_DIR}/libraries/arm64-v8a/libSPVRemapper.a
        android
        log
        EGL
        vulkan
)

target_include_directories(mobileglues PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        )


if (PROFILING)
        find_package(Threads)
        include_directories(${CMAKE_SOURCE_DIR}/3rdparty/perfetto/sdk)
        add_library(perfetto STATIC ${CMAKE_SOURCE_DIR}/3rdparty/perfetto/sdk/perfetto.cc)
        target_link_libraries(mobileglues perfetto ${CMAKE_THREAD_LIBS_INIT})
        target_compile_definitions(mobileglues PUBLIC PROFILING=1)
endif()
