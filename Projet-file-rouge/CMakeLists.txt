cmake_minimum_required(VERSION 3.20)
project(MathStarter CXX ASM_MASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

# 1. Bibliothèque MathLib
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
)

# Indique à CMake tous les endroits possibles où chercher les .h
target_include_directories(MathLib PUBLIC
        include
        include/MathLib
        include/Platform
        src/Platforml
        Platform
)

# --- Dépendances externes via FetchContent ---
include(FetchContent)

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

# 2. Application Benchmark
add_executable(BenchmarkApp src/Benchmark/main.cpp)
target_link_libraries(BenchmarkApp PRIVATE MathLib benchmark::benchmark)

# 3. Tests unitaires
enable_testing()
include(GoogleTest)

add_executable(UnitTests
        src/Tests/CpuFeaturesTests.cpp
        src/Tests/Matrix3x3Tests.cpp
        src/Tests/Matrix4x4Tests.cpp
        src/Tests/Vec3Tests.cpp
        src/Tests/Vec4Tests.cpp
        src/Tests/TestHelpers.h
)
target_include_directories(UnitTests PRIVATE src/Tests)
target_link_libraries(UnitTests PRIVATE MathLib gtest_main)
gtest_discover_tests(UnitTests)