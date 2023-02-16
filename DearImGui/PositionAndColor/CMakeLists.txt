cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PositionAndColor)

set(SOURCES
    main.cpp
    ../../Utils/shaderprogram.cpp
    ${CMAKE_PREFIX_PATH}/imgui/imgui.cpp
    ${CMAKE_PREFIX_PATH}/imgui/imgui_demo.cpp
    ${CMAKE_PREFIX_PATH}/imgui/imgui_draw.cpp
    ${CMAKE_PREFIX_PATH}/imgui/imgui_tables.cpp
    ${CMAKE_PREFIX_PATH}/imgui/imgui_widgets.cpp
    ${CMAKE_PREFIX_PATH}/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_PREFIX_PATH}/imgui/backends/imgui_impl_opengl3.cpp)

configure_file(vertex_shader.glsl vertex_shader.glsl COPYONLY)
configure_file(fragment_shader.glsl fragment_shader.glsl COPYONLY)

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

if(WIN32)
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_PREFIX_PATH}/include
        PRIVATE ${CMAKE_PREFIX_PATH}/imgui
        PRIVATE ${CMAKE_PREFIX_PATH}/imgui/backends
        PRIVATE ../../Utils)

    target_link_libraries(${PROJECT_NAME} opengl32)
    find_library(GLEW_LIB glew32)
    target_link_libraries(${PROJECT_NAME} ${GLEW_LIB})
    find_library(GLFW_LIB glfw3dll)
    target_link_libraries(${PROJECT_NAME} ${GLFW_LIB})
endif(WIN32)
