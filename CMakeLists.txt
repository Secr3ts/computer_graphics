cmake_minimum_required(VERSION 3.16)
project(final CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
add_compile_options(-g -Wall -Wno-unused-function)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Source files
set(SOURCES
    final.cpp
    common/GLShader.cpp
    common/Vector3.cpp
    common/Point3.cpp
    common/camera.cpp
    common/model.cpp
)

# ImGui sources
set(IMGUI_SOURCES
    common/imgui/imgui.cpp
    common/imgui/imgui_demo.cpp
    common/imgui/imgui_draw.cpp
    common/imgui/imgui_tables.cpp
    common/imgui/imgui_widgets.cpp
    common/imgui/backends/imgui_impl_glfw.cpp
    common/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(final
    ${SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(final PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/common/imgui
    ${CMAKE_SOURCE_DIR}/common/imgui/backends
)

target_link_libraries(final PRIVATE
    OpenGL::GL
    GLEW::GLEW
    glfw
    glm::glm
)

add_custom_command(TARGET final POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/images
        $<TARGET_FILE_DIR:final>/images
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/models
        $<TARGET_FILE_DIR:final>/models
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:final>/shaders
)
