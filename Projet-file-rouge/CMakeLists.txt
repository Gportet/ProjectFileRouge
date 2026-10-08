cmake_minimum_required(VERSION 3.20)
project(MathStarter CXX ASM_MASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

# Bibliothèque MathLib
add_library(MathLib STATIC
        src/Platform/CpuFeatures.cpp
        src/Tests/TestHelpers.h

        include/MathLib/Vec3.h
        include/MathLib/Vec4.h
        include/MathLib/Matrix3x3.h
        include/MathLib/Matrix4x4.h
        include/Platform/CpuFeatures.h
        include/MathLib/MathLib_SIMD/src/Vec3_SIMD.cpp
        include/MathLib/MathLib_SIMD/include/Vec3_SIMD.h
        include/MathLib/MathLib_SIMD/src/Matrix3x3_SIMD.cpp
        include/MathLib/MathLib_SIMD/include/Matrix3x3_SIMD.h
)

target_include_directories(MathLib PUBLIC
        include
        include/MathLib
        include/Platform
        src/Platforml
        Platform
)

# --- Dépendances externes via FetchContent ---
include(FetchContent)

# Récupération de nlohmann_json via Git
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

# Récupération de Dear ImGui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        docking # Branche très pratique pour le fenêtrage
)
FetchContent_MakeAvailable(imgui)

# Création de la bibliothèque ImGui
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

# Récupération de ImPlot
FetchContent_Declare(
        implot
        GIT_REPOSITORY https://github.com/epezent/implot.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(implot)

# Création de la bibliothèque ImPlot
add_library(implot STATIC
        ${implot_SOURCE_DIR}/implot.cpp
        ${implot_SOURCE_DIR}/implot_items.cpp
        ${implot_SOURCE_DIR}/implot_demo.cpp
)
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot PUBLIC imgui)
# Google Benchmark
FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.0
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

# GoogleTest
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Application Benchmark
add_executable(BenchmarkApp src/Benchmark/main.cpp

        include/benchmarkUtils.h
        src/Benchmark/benchmarkVect3.cpp
        src/Benchmark/benchmarkMatrix3x3.cpp
        src/Benchmark/benchmarkMatrix4x4.cpp
        src/Benchmark/benchmarkVect4.cpp
        include/benchmarkUtils.h
        src/Benchmark/benchmarkMatrix4x4.cpp
        src/Benchmark/benchmarkMatrix3x3.cpp
        src/Benchmark/benchmarkVect3.cpp
        src/Benchmark/benchmarkVect4.cpp)
target_link_libraries(BenchmarkApp PRIVATE MathLib benchmark::benchmark)

# Tests unitaires
enable_testing()
include(GoogleTest)



add_executable(UnitTests
        src/Tests/CpuFeaturesTests.cpp
        src/Tests/Matrix3x3Tests.cpp
        src/Tests/Matrix4x4Tests.cpp
        src/Tests/Vec3Tests.cpp
        src/Tests/Vec4Tests.cpp
        src/Tests/TestHelpers.h
        include/MathLib/MathLib_SIMD/src/Matrix3x3_SIMD.cpp
)
target_include_directories(UnitTests PRIVATE src/Tests)
target_link_libraries(UnitTests PRIVATE MathLib gtest_main)
gtest_discover_tests(UnitTests)



# --- Ajout de GLFW pour le fenêtrage ---
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)


# --- Configuration de l'executable ViewerApp ---
add_executable(ViewerApp
        src/Viewer/main.cpp
        src/Viewer/BenchmarkParser.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        include/Viewer/BenchmarkResult.h
        include/Viewer/BenchmarkParser.h
        include/MathLib/MathLib_SIMD/src/Matrix3x3_SIMD.cpp
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
        opengl32
)