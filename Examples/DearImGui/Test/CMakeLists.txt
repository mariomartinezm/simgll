project(DearImGuiTest LANGUAGES C CXX)

set(SOURCES
    main.cpp
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

if(UNIX)
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_PREFIX_PATH}/imgui
        PRIVATE ${CMAKE_PREFIX_PATH}/imgui/backends)

    target_link_libraries(${PROJECT_NAME} GL)
    target_link_libraries(${PROJECT_NAME} GLEW)
    target_link_libraries(${PROJECT_NAME} glfw)
    target_link_libraries(${PROJECT_NAME} dl)
    target_link_libraries(${PROJECT_NAME} simgll)
endif()

if(WIN32)
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_PREFIX_PATH}/include
        PRIVATE ${CMAKE_PREFIX_PATH}/imgui
        PRIVATE ${CMAKE_PREFIX_PATH}/imgui/backends)

    target_link_libraries(${PROJECT_NAME} opengl32)
    find_library(GLEW_LIB glew32)
    target_link_libraries(${PROJECT_NAME} ${GLEW_LIB})
    find_library(GLFW_LIB glfw3dll)
    target_link_libraries(${PROJECT_NAME} ${GLFW_LIB})
    target_link_libraries(${PROJECT_NAME} simgll)
endif(WIN32)
