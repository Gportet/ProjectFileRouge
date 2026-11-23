
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

add_library(glad STATIC
        src/glad.c
)
target_include_directories(glad PUBLIC include)


add_library(MathLib STATIC
        src/Platform/CpuFeatures.cpp
        include/Platform/CpuFeatures.h

        include/MathLib/Vec3.h
        include/MathLib/Vec4.h
        include/MathLib/Matrix3x3.h
        include/MathLib/Matrix4x4.h

        include/MathLib/MathLib_SIMD/Vec3_SIMD.h
        src/MathLib/MathLib_SIMD/Vec3_SIMD.cpp

        include/MathLib/MathLib_SIMD/Vec4_SIMD.h
        src/MathLib/MathLib_SIMD/Vec4_SIMD.cpp

        include/MathLib/MathLib_SIMD/Matrix3x3_SIMD.h
        src/MathLib/MathLib_SIMD/Matrix3x3_SIMD.cpp

        include/MathLib/MathLib_SIMD/Matrix4x4_SIMD.h
        src/MathLib/MathLib_SIMD/Matrix4x4_SIMD.cpp

        include/MathLib/MathLib_SIMD/Vec3_ASM.h
        include/MathLib/MathLib_SIMD/Vec3_ASM.asm

        src/Tests/TestHelpers.h
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
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)


FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)


FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(stb)


FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG VER-2-14-3
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(freetype)


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
        GIT_TAG master
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
        GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)


# ------------------------------------------------------------------------------
# Executables
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# BenchmarkApp
# ------------------------------------------------------------------------------

add_executable(BenchmarkApp
        src/Benchmark/main.cpp

        include/benchmarkUtils.h

        src/Benchmark/benchmarkVect3.cpp
        src/Benchmark/benchmarkMatrix3x3.cpp
        src/Benchmark/benchmarkMatrix4x4.cpp
        src/Benchmark/benchmarkVect4.cpp
        src/Benchmark/benchmarkBatch.cpp

        include/MathLib/MathLib_SIMD/Vec4_SIMD.h
        src/MathLib/MathLib_SIMD/Vec4_SIMD.cpp

        include/MathLib/MathLib_SIMD/Matrix4x4_SIMD.h
        src/MathLib/MathLib_SIMD/Matrix4x4_SIMD.cpp

        src/MathLib/MathLib_SIMD/Matrix3x3_SIMD.cpp
        src/MathLib/MathLib_SIMD/Vec3_SIMD.cpp

        include/MathLib/MathLib_SIMD/Vec3_ASM.h
)

target_link_libraries(BenchmarkApp PRIVATE
        MathLib
        benchmark::benchmark
)


# ------------------------------------------------------------------------------
# UnitTests
# ------------------------------------------------------------------------------

enable_testing()
include(GoogleTest)

add_executable(UnitTests
        src/Tests/CpuFeaturesTests.cpp
        src/Tests/Matrix3x3Tests.cpp
        src/Tests/Matrix4x4Tests.cpp
        src/Tests/Vec3Tests.cpp
        src/Tests/Vec4Tests.cpp

        src/Tests/TestHelpers.h

        src/MathLib/MathLib_SIMD/Matrix3x3_SIMD.cpp
)

target_include_directories(UnitTests PRIVATE
        src/Tests
)

target_link_libraries(UnitTests PRIVATE
        MathLib
        gtest_main
)

gtest_discover_tests(UnitTests)


# ------------------------------------------------------------------------------
# ViewerApp
# ------------------------------------------------------------------------------

add_executable(ViewerApp
        src/Viewer/main.cpp

        include/Viewer/BenchmarkResult.h
        include/Viewer/BenchmarkParser.h
        src/Viewer/BenchmarkParser.cpp

        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp

        src/MathLib/MathLib_SIMD/Matrix3x3_SIMD.cpp
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




add_executable(AssetEngine

        # Main
        include/AssetEngine/main.cpp

        # Asset Engine - Binary
        include/AssetEngine/Binary/BinaryReader.h
        include/AssetEngine/Binary/BinaryWriter.h
        include/AssetEngine/Binary/BinaryTypes.h

        # Asset Engine - Header
        include/AssetEngine/HeaderInit/AssetHeader.h
        src/AssetEngine/HeaderInit/AssetHeader.cpp

        # Asset Engine - Roundtrip
        include/AssetEngine/Roundtrip.h

        # Asset Engine - Asset File
        include/AssetEngine/AssetFile/AssetFileWriter.h
        src/AssetEngine/AssetFile/AssetFileWriter.cpp
        include/AssetEngine/AssetFile/AssetFileReader.h
        src/AssetEngine/AssetFile/AssetFileReader.cpp

        # Asset Engine - Utils
        include/AssetEngine/Utils/CheckSum.h
        src/AssetEngine/Utils/CheckSum.cpp

        # Asset Engine - Types
        include/AssetEngine/AssetType/AssetType.h
        include/AssetEngine/AssetType/PixelFormat.h

        # Asset Engine - Serializers
        include/AssetEngine/Serializer/TextureSerializer.h
        src/AssetEngine/Serializer/TextureSerializer.cpp
        include/AssetEngine/Serializer/MaterialSerializer.h
        src/AssetEngine/Serializer/MaterialSerializer.cpp

        # Engine - Utilities
        include/Engine/utils/Filepath.h
        src/Engine/utils/Filepath.cpp

        # Engine - Resources
        include/Engine/Ressources/Texture.h
        src/Engine/Ressources/Texture.cpp
        include/Engine/Ressources/Material.h
        src/Engine/Ressources/Material.cpp

        # MathLib - Headers
        include/MathLib/MathLib_SIMD/Matrix3x3_SIMD.h
        include/MathLib/MathLib_SIMD/Matrix4x4_SIMD.h
        include/MathLib/MathLib_SIMD/Vec3_ASM.h
        include/MathLib/MathLib_SIMD/Vec3_SIMD.h
        include/MathLib/MathLib_SIMD/Vec4_SIMD.h
        include/AssetEngine/ArchiveValidator.h
)

target_include_directories(AssetEngine PRIVATE
        ${stb_SOURCE_DIR}
        include
)

target_link_libraries(AssetEngine PRIVATE
        MathLib
)



# ------------------------------------------------------------------------------
# Affichage_test
# ------------------------------------------------------------------------------
add_executable(Affichage_test
        main.cpp

        # Engine - Core
        include/Engine/ZBuffer.h
        src/Engine/ZBuffer.cpp

        include/Engine/Camera.h

        include/Engine/DisplayWindow.h
        src/Engine/DisplayWindow.cpp

        include/Engine/Mesh.h
        src/Engine/Mesh.cpp

        include/Engine/ObjLoader.h
        src/Engine/ObjLoader.cpp

        include/Engine/Pipeline.h
        src/Engine/Pipeline.cpp

        include/Engine/Light.h

        # Engine - Resources
        include/Engine/Ressources/Texture.h
        src/Engine/Ressources/Texture.cpp

        include/Engine/Ressources/Material.h
        src/Engine/Ressources/Material.cpp

        include/Engine/Ressources/MaterialLoader.h
        src/Engine/Ressources/MaterialLoader.cpp

        # Engine - Utilities
        include/Engine/utils/Filepath.h
        src/Engine/utils/Filepath.cpp

        include/Engine/utils/Font.h
        src/Engine/utils/Font.cpp

        include/Engine/utils/RandomGenerator.h
        src/Engine/utils/RandomGenerator.cpp

        # Engine - Input
        include/Engine/InputDirectory/Input.h
        src/Engine/InputDirectory/Input.cpp
        
        # Engine - Rendering
        include/Engine/Rendering/TextRendering.h
        src/Engine/Rendering/TextRendering.cpp

        # Engine - Shaders
        include/Engine/Rendering/Shader/Shader.h

        include/Engine/Rendering/Shader/UnlitShader.h
        src/Engine/Rendering/Shader/UnlitShader.cpp

        include/Engine/Rendering/Shader/PhongShader.h
        src/Engine/Rendering/Shader/PhongShader.cpp

        include/Engine/Rendering/Shader/LambertShader.h
        src/Engine/Rendering/Shader/LambertShader.cpp

        include/Engine/Rendering/Shader/BlinnPhongShader.h
        src/Engine/Rendering/Shader/BlinnPhongShader.cpp

        # Engine - Particles
        include/Engine/ParticleGenerator.h
        src/Engine/ParticleGenerator.cpp

        # ECS - Components
        include/ECS/Component.h

        include/ECS/Component/CameraComponent.h
        include/ECS/Component/LightComponent.h
        include/ECS/Component/MeshComponent.h
        include/ECS/Component/TransformComponent.h
        include/ECS/Component/SpinComponent.h

        # ECS - Managers
        include/ECS/Entity.h
        include/ECS/EntityManager.h
        include/ECS/ComponentArray.h
        include/ECS/ComponentManager.h
        src/System/ComponentManager.cpp
        include/ECS/Registry.h

        # ECS - Systems
        include/ECS/System/SpinSystem.h

        include/ECS/System/RenderSystem.h
        src/System/RenderSystem.cpp

        include/ECS/System/CameraMovementSystem.h
        src/System/CameraMovementSystem.cpp
        
        # MathLib - Headers
        include/MathLib/MathLib_SIMD/Matrix3x3_SIMD.h
        include/MathLib/MathLib_SIMD/Matrix4x4_SIMD.h
        include/MathLib/MathLib_SIMD/Vec3_ASM.h
        include/MathLib/MathLib_SIMD/Vec3_SIMD.h
        include/MathLib/MathLib_SIMD/Vec4_SIMD.h

        include/AssetEngine/Binary/BinaryReader.h
        include/AssetEngine/Binary/BinaryWriter.h
        include/AssetEngine/Binary/BinaryTypes.h
        
        include/AssetEngine/HeaderInit/AssetHeader.h
        src/AssetEngine/HeaderInit/AssetHeader.cpp
        
        include/AssetEngine/AssetFile/AssetFileWriter.h
        src/AssetEngine/AssetFile/AssetFileWriter.cpp
        
        include/AssetEngine/AssetFile/AssetFileReader.h
        src/AssetEngine/AssetFile/AssetFileReader.cpp
        src/AssetEngine/Utils/CheckSum.cpp
        include/AssetEngine/Utils/CheckSum.h
        include/AssetEngine/AssetType/AssetType.h
        include/AssetEngine/Serializer/TextureSerializer.h
        src/AssetEngine/Serializer/TextureSerializer.cpp
        include/AssetEngine/AssetType/PixelFormat.h
        include/AssetEngine/Serializer/MaterialSerializer.h
        src/AssetEngine/Serializer/MaterialSerializer.cpp
)

add_custom_command(TARGET Affichage_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Affichage_test>/assets"
        COMMENT "Copie des assets vers le dossier de compilation..."
)

target_include_directories(Affichage_test PRIVATE
        ${stb_SOURCE_DIR}
        include
)


target_link_libraries(Affichage_test PRIVATE
        MathLib
        glad
        glfw
        OpenGL::GL
        freetype
)
