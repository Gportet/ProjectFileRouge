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
)

# Indique à CMake tous les endroits possibles où chercher les .h
target_include_directories(MathLib PUBLIC
        include
        include/MathLib
        include/Platform
        src/Platforml
        Platform
)

# 2. Application Benchmark
add_executable(BenchmarkApp src/Benchmark/main.cpp)
target_link_libraries(BenchmarkApp PRIVATE MathLib)

# 3. Tests unitaires
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

add_executable(UnitTests
        src/Tests/BenchmarkTests.cpp
        src/Tests/CpuFeaturesTests.cpp
        src/Tests/Matrix3x3Tests.cpp
        src/Tests/Matrix4x4Tests.cpp
        src/Tests/Vec3Tests.cpp
        src/Tests/Vec4Tests.cpp
)
target_include_directories(UnitTests PRIVATE src/Tests)
target_link_libraries(UnitTests PRIVATE MathLib gtest_main)
gtest_discover_tests(UnitTests)