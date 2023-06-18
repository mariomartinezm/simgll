project(wxGLCanvas LANGUAGES CXX)

if(WIN32)
    set(wxWidgets_ROOT_DIR $ENV{WX_ROOT_DIR})
    set(wxWidgets_LIB_DIR $ENV{WX_LIB_DIR})
endif()

find_package(wxWidgets COMPONENTS gl core base REQUIRED)
include(${wxWidgets_USE_FILE})

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    glhelper.h
    glhelper.cpp
    glframe.h
    glframe.cpp)

configure_file(vertex_shader.glsl vertex_shader.glsl COPYONLY)
configure_file(fragment_shader.glsl fragment_shader.glsl COPYONLY)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

if(UNIX)
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_PREFIX_PATH}/include)

    target_link_libraries(${PROJECT_NAME} GL)
    target_link_libraries(${PROJECT_NAME} GLEW)
    target_link_libraries(${PROJECT_NAME} glm3)
    target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif()

if(WIN32)
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_PREFIX_PATH}/include)

    find_library(GLEW_LIB glew32)
    target_link_libraries(${PROJECT_NAME} ${GLEW_LIB})
    target_link_libraries(${PROJECT_NAME} glm3)
    target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif()
