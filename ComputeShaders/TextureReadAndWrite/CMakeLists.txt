project(TextureReadAndWrite)

set(SOURCES
    main.cpp
    ../../Utils/shaderprogram.cpp)

configure_file(compute_shader.glsl compute_shader.glsl COPYONLY)
configure_file(vertex_shader.glsl vertex_shader.glsl COPYONLY)
configure_file(fragment_shader.glsl fragment_shader.glsl COPYONLY)

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

if(UNIX)
    target_include_directories(${PROJECT_NAME}
        PRIVATE ../../Utils)

    target_link_libraries(${PROJECT_NAME} GL)
    target_link_libraries(${PROJECT_NAME} GLEW)
    target_link_libraries(${PROJECT_NAME} glfw)
endif()

if(WIN32)
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_PREFIX_PATH}/include
        PRIVATE ../../Utils)

    target_link_libraries(${PROJECT_NAME} opengl32)
    find_library(GLEW_LIB glew32)
    target_link_libraries(${PROJECT_NAME} ${GLEW_LIB})
    find_library(GLFW_LIB glfw3dll)
    target_link_libraries(${PROJECT_NAME} ${GLFW_LIB})
endif()
