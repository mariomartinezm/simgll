project(ElementWiseProduct LANGUAGES CXX)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

configure_file(compute_shader.glsl compute_shader.glsl COPYONLY)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} GL)
    target_link_libraries(${PROJECT_NAME} GLEW)
    target_link_libraries(${PROJECT_NAME} glfw)
    target_link_libraries(${PROJECT_NAME} simgll)
endif()

if(WIN32)
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_PREFIX_PATH}/include)

    target_link_libraries(${PROJECT_NAME} opengl32)
    find_library(GLEW_LIB glew32)
    target_link_libraries(${PROJECT_NAME} ${GLEW_LIB})
    find_library(GLFW_LIB glfw3dll)
    target_link_libraries(${PROJECT_NAME} ${GLFW_LIB})
    target_link_libraries(${PROJECT_NAME} simgll)
endif()
