cmake_minimum_required(VERSION 3.31.5)

project("mobileglues")

enable_language(CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -pipe -integrated-as -mllvm -polly -mllvm -polly-vectorizer=stripmine -mllvm -polly-invariant-load-hoisting -mllvm -polly-run-inliner -mllvm -polly-run-dce -mllvm -polly-invariant-load-hoisting -flto=thin -mllvm -polly-run-inliner -mllvm -polly-run-dce -mllvm -polly-parallel -mllvm -polly-scheduling=dynamic -flto=thin -fno-emulated-tls -fwhole-program-vtables -fdata-sections -ffunction-sections -fmerge-all-constants -mllvm -polly-detect-keep-going -mllvm -polly-ast-use-context -march=armv8-a+simd -fvisibility=hidden -Wall -Wextra -Wno-unused-parameter -fPIC -DPIC -ferror-limit=0 -Wno-return-type")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pipe -integrated-as -mllvm -polly -mllvm -polly-vectorizer=stripmine -mllvm -polly-invariant-load-hoisting -mllvm -polly-run-inliner -mllvm -polly-run-dce -mllvm -polly-invariant-load-hoisting -flto=thin -mllvm -polly-run-inliner -mllvm -polly-run-dce -mllvm -polly-parallel -mllvm -polly-scheduling=dynamic -flto=thin -fno-emulated-tls -fwhole-program-vtables -fdata-sections -ffunction-sections -fmerge-all-constants -mllvm -polly-detect-keep-going -mllvm -polly-ast-use-context -march=armv8-a+simd -fvisibility=hidden -fPIC -DPIC -Wall -Wextra -Wno-unused-parameter -ferror-limit=0 -Wno-return-type")
set(LINK_OPTIONS "${CMAKE_LINK_OPTIONS} -fuse-ld=lld -flto=thin -Wl,-plugin-opt=-emulated-tls=0")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINK_OPTIONS} -fuse-ld=lld -flto=thin -Wl,-plugin-opt=-emulated-tls=0")
set(CMAKE_ANDROID_STL_TYPE system)
set(CMAKE_ANDROID_LD "lld")

#set(CMAKE_BUILD_TYPE Release)

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
        gl/dsa/buffer.cpp
        gl/dsa/framebuffer.cpp
        gl/fpe/fpe.cpp
        gl/fpe/state.cpp
        gl/fpe/transformation.cpp
        gl/fpe/vertexpointer.cpp
        gl/fpe/fpe_shadergen.cpp
        gl/fpe/list.cpp
        gl/fpe/pointer_utils.cpp
        gl/fpe/drawing1x.cpp
        gl/fpe/vertexpointer_utils.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ./includes ./glm)

target_link_libraries(${CMAKE_PROJECT_NAME}
        ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libglslang.a
        ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libspirv-cross-c-shared.so
        ${CMAKE_SOURCE_DIR}/libraries/${ANDROID_ABI}/libshaderconv.so
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
        android
        log
        EGL
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

#find_package(Vulkan COMPONENTS glslc Vulkan::SPIRV-Tools)
#find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)
#find_program(spirv-cross_executable NAMES glslc HINTS Vulkan::SPIRV-Tools)
#
#add_custom_target(mobileglues_shader)
#
#set(VERT_SHADER_FILES
#        ${CMAKE_SOURCE_DIR}/shader/shader.vert)
#set(FRAG_SHADER_FILES
#        ${CMAKE_SOURCE_DIR}/shader/shader.frag)
#set(GLSL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/shader/include)
#
#
##glslc -o out/simple.spv src/simple.vert --target-env=opengl -Iinclude
#
#function(compile_shader target)
#        cmake_parse_arguments(PARSE_ARGV 1 arg "" "ENV;INCLUDE" "SOURCES")
#        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/spv)
#                foreach(source ${arg_SOURCES})
#                get_filename_component(SRC_NAME ${source} NAME)
#                add_custom_command(
#                        TARGET ${target}
#                        PRE_BUILD
##                        OUTPUT ${CMAKE_BINARY_DIR}/spv/${source}.spv
#                        DEPENDS ${source}
#                        COMMAND
#                        ${glslc_executable}
#                        $<$<BOOL:${arg_ENV}>:--target-env=${arg_ENV}>
#                        $<$<BOOL:${arg_INCLUDE}>:-I${arg_INCLUDE}>
#                        -o ${CMAKE_BINARY_DIR}/spv/${SRC_NAME}.spv
#                        ${source}
#                )
##                target_sources(${target} PRIVATE ${source})
#        endforeach()
#endfunction()
#
#
#compile_shader(mobileglues_shader
#        ENV opengl
#        INCLUDE ${GLSL_INCLUDE_DIR}
#        SOURCES ${VERT_SHADER_FILES} ${FRAG_SHADER_FILES}
#)
#
##message(FATAL_ERROR ${CMAKE_BINARY_DIR}/spv)
#
#add_dependencies(mobileglues mobileglues_shader)
