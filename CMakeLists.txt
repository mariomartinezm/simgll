cmake_minimum_required(VERSION 3.1)

project(RayTracing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    ../../Utils/shaderprogram.cpp)

configure_file(compute_shader.glsl compute_shader.glsl COPYONLY)
configure_file(vertex_shader.glsl vertex_shader.glsl COPYONLY)
configure_file(fragment_shader.glsl fragment_shader.glsl COPYONLY)

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

if(WIN32)
    target_include_directories(${PROJECT_NAME}
        PRIVATE ../../ThirdParty/include
        PRIVATE ../../Utils)
    target_link_libraries(${PROJECT_NAME} opengl32)

    find_library(GLEW_LIB glew32 HINTS ../../ThirdParty/lib)
    target_link_libraries(${PROJECT_NAME} ${GLEW_LIB})

    find_library(GLFW_LIB glfw3dll HINTS ../../ThirdParty/lib)
    target_link_libraries(${PROJECT_NAME} ${GLFW_LIB})
endif(WIN32)
