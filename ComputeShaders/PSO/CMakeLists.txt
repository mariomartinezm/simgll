cmake_minimum_required(VERSION 3.1)

project(PSO LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    ../../Utils/shaderprogram.cpp)

configure_file(vertex_shader.glsl vertex_shader.glsl COPYONLY)
configure_file(fragment_shader.glsl fragment_shader.glsl COPYONLY)
configure_file(pso.glsl pso.glsl COPYONLY)

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} GL)
    target_link_libraries(${PROJECT_NAME} GLEW)
    target_link_libraries(${PROJECT_NAME} glfw)
endif(UNIX)

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
