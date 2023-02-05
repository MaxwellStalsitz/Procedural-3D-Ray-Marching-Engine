cmake_minimum_required(VERSION 3.21)
project(Procedural-3D-Ray-Marching-Engine)

set(CMAKE_CXX_STANDARD 17)
include(CPack)

find_package(OpenGL REQUIRED)

set(source_files src/Source.cpp src/Source.h src/glad.c src/Shader.h)

include_directories("Include/glm")
include_directories("Include/glad")

include_directories("Include/stb_image")
include_directories("Include/KHR")

# ------------------------------------------------------------------------
#imgui

set(imgui_path "ImGui")

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

add_executable(OpenGL_Rendering_Engine ${source_files})

target_link_libraries(OpenGL_Rendering_Engine ${OPENGL_LIBRARIES} glfw3 glu32 opengl32 imgui)


