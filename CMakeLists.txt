cmake_minimum_required(VERSION 3.21)
project(Procedural-3D-Ray-Marching-Engine)

set(CMAKE_CXX_STANDARD 17)
include(CPack)

find_package(OpenGL REQUIRED)

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/Include/GLFW/GLFW")
find_package(glfw3 CONFIG REQUIRED)

set(source_files src/Source.cpp src/maingui.cpp src/Source.h src/glad.c src/Shader.h src/shadersetup.h src/cameraMovement.h src/input.h src/maingui.h src/sceneEditor.h)

include_directories("Include/glm")
include_directories("Include/glad")

include_directories("Include/stb_image")
include_directories("Include/KHR")

# ------------------------------------------------------------------------
#imgui

set(imgui_path "Include/ImGui")

file(GLOB imgui_sources
        ${imgui_path}/imgui.cpp
        ${imgui_path}/imconfig.h
        ${imgui_path}/imgui.h
        ${imgui_path}/imgui_demo.cpp
        ${imgui_path}/imgui_draw.cpp
        ${imgui_path}/imgui_impl_glfw.cpp
        ${imgui_path}/imgui_impl_glfw.h
        ${imgui_path}/imgui_impl_opengl3.cpp
        ${imgui_path}/imgui_impl_opengl3.h
        ${imgui_path}/imgui_impl_opengl3_loader.h
        ${imgui_path}/imgui_internal.h
        ${imgui_path}/imgui_tables.cpp
        ${imgui_path}/imgui_widgets.cpp
        ${imgui_path}/imstb_rectpack.h
        ${imgui_path}/imstb_textedit.h)

add_library("imgui" STATIC ${imgui_sources})
target_include_directories("imgui" PUBLIC ${imgui_path})
# ------------------------------------------------------------------------

include_directories("Include/GLFW")
link_directories("Include/GLFW/lib-mingw-w64")

include_directories(${CMAKE_BINARY_DIR}/Resources/configuration)
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}")
configure_file(Resources/configuration/source_directory.h.in Resources/configuration/source_directory.h)


add_executable(OpenGL_Rendering_Engine WIN32 ${source_files})

target_link_libraries(OpenGL_Rendering_Engine ${OPENGL_LIBRARIES} glfw3 glu32 opengl32 imgui)


