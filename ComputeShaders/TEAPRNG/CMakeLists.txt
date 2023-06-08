project(TEAPRNG LANGUAGES CXX)

set(SOURCES
    main.cpp)

configure_file(compute_shader.glsl  compute_shader.glsl  COPYONLY)
configure_file(vertex_shader.glsl   vertex_shader.glsl   COPYONLY)
configure_file(fragment_shader.glsl fragment_shader.glsl COPYONLY)

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} GL)
    target_link_libraries(${PROJECT_NAME} GLEW)
    target_link_libraries(${PROJECT_NAME} glfw)
    target_link_libraries(${PROJECT_NAME} glm3)
endif()

if(WIN32)
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_PREFIX_PATH}/include)

    target_link_libraries(${PROJECT_NAME} opengl32)
    find_library(GLEW_LIB glew32)
    target_link_libraries(${PROJECT_NAME} ${GLEW_LIB})
    find_library(GLFW_LIB glfw3dll)
    target_link_libraries(${PROJECT_NAME} ${GLFW_LIB})
    target_link_libraries(${PROJECT_NAME} glm3)
endif()
