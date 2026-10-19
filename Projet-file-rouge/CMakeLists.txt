cmake_minimum_required(VERSION 3.20)
project(MathStarter C CXX ASM_MASM)

include(FetchContent)
find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/arch:AVX2)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
else()
    add_compile_options(-march=native)
endif()

# ------------------------------------------------------------------------------
# GLAD & MathLib
# ------------------------------------------------------------------------------
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC include)

add_library(MathLib STATIC
        src/Platform/CpuFeatures.cpp
        src/Tests/TestHelpers.h
        include/MathLib/Vec3.h
        include/MathLib/Vec4.h
        include/MathLib/Matrix3x3.h
        include/MathLib/Matrix4x4.h
        include/Platform/CpuFeatures.h
        src/MathLib/MathLib_SIMD/src/Vec3_SIMD.cpp
        include/MathLib/MathLib_SIMD/include/Vec3_SIMD.h
        src/MathLib/MathLib_SIMD/src/Matrix3x3_SIMD.cpp
        include/MathLib/MathLib_SIMD/include/Matrix3x3_SIMD.h
        src/MathLib/MathLib_SIMD/src/Matrix4x4_SIMD.cpp
        include/MathLib/MathLib_SIMD/include/Matrix4x4_SIMD.h
        src/MathLib/MathLib_SIMD/src/Vec4_SIMD.cpp
        include/MathLib/MathLib_SIMD/include/Vec4_SIMD.h
        include/MathLib/MathLib_SIMD/include/Vec3_ASM.h
        include/MathLib/MathLib_SIMD/include/Vec3_ASM.asm
)
target_include_directories(MathLib PUBLIC
        include
        include/MathLib
        include/Platform
        src/Platforml
        Platform
)

# ------------------------------------------------------------------------------
# Dépendances Externes
# ------------------------------------------------------------------------------
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        docking
)
FetchContent_MakeAvailable(imgui)


FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(stb)

add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

FetchContent_Declare(
        implot
        GIT_REPOSITORY https://github.com/epezent/implot.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(implot)

add_library(implot STATIC
        ${implot_SOURCE_DIR}/implot.cpp
        ${implot_SOURCE_DIR}/implot_items.cpp
        ${implot_SOURCE_DIR}/implot_demo.cpp
)
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot PUBLIC imgui)

FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.0
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ------------------------------------------------------------------------------
# Executables
# ------------------------------------------------------------------------------
add_executable(BenchmarkApp
        src/Benchmark/main.cpp
        include/benchmarkUtils.h
        src/Benchmark/benchmarkVect3.cpp
        src/Benchmark/benchmarkMatrix3x3.cpp
        src/Benchmark/benchmarkMatrix4x4.cpp
        src/Benchmark/benchmarkVect4.cpp
        include/MathLib/MathLib_SIMD/include/Vec4_SIMD.h
        src/MathLib/MathLib_SIMD/src/Vec4_SIMD.cpp
        include/MathLib/MathLib_SIMD/include/Matrix4x4_SIMD.h
        src/MathLib/MathLib_SIMD/src/Matrix4x4_SIMD.cpp
        src/MathLib/MathLib_SIMD/src/Matrix3x3_SIMD.cpp
        src/MathLib/MathLib_SIMD/src/Vec3_SIMD.cpp
        include/MathLib/MathLib_SIMD/include/Vec3_ASM.h
        src/Benchmark/benchmarkBatch.cpp
)
target_link_libraries(BenchmarkApp PRIVATE MathLib benchmark::benchmark)

enable_testing()
include(GoogleTest)
add_executable(UnitTests
        src/Tests/CpuFeaturesTests.cpp
        src/Tests/Matrix3x3Tests.cpp
        src/Tests/Matrix4x4Tests.cpp
        src/Tests/Vec3Tests.cpp
        src/Tests/Vec4Tests.cpp
        src/Tests/TestHelpers.h
        src/MathLib/MathLib_SIMD/src/Matrix3x3_SIMD.cpp
)
target_include_directories(UnitTests PRIVATE src/Tests)
target_link_libraries(UnitTests PRIVATE MathLib gtest_main)
gtest_discover_tests(UnitTests)

add_executable(ViewerApp
        src/Viewer/main.cpp
        src/Viewer/BenchmarkParser.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        include/Viewer/BenchmarkResult.h
        include/Viewer/BenchmarkParser.h
        src/MathLib/MathLib_SIMD/src/Matrix3x3_SIMD.cpp
)
target_include_directories(ViewerApp PRIVATE
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(ViewerApp PRIVATE
        MathLib
        nlohmann_json::nlohmann_json
        imgui
        implot
        glfw
        OpenGL::GL
)

add_executable(Affichage_test
        include/Engine/ZBuffer.h
        include/Engine/Camera.h
        include/Engine/DisplayWindow.h
        include/Engine/Mesh.h
        include/Engine/ObjLoader.h
        include/Engine/Pipeline.h
        src/Engine/ZBuffer.cpp
        src/Engine/DisplayWindow.cpp
        src/Engine/ObjLoader.cpp
        src/Engine/Pipeline.cpp
        main.cpp
        src/Engine/Mesh.cpp
)

target_include_directories(Affichage_test PRIVATE
        ${stb_SOURCE_DIR}
)

target_include_directories(Affichage_test PRIVATE
        include
)

target_link_libraries(Affichage_test PRIVATE
        MathLib
        glad
        glfw
        OpenGL::GL
)

