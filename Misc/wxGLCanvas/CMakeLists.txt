cmake_minimum_required(VERSION 3.25)

project(wxGLCanvas LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(wxWidgets_ROOT_DIR $ENV{WX_ROOT_DIR})
    set(wxWidgets_LIB_DIR $ENV{WX_LIB_DIR})
endif()

find_package(wxWidgets COMPONENTS gl core base REQUIRED)
include(${wxWidgets_USE_FILE})

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    trianglewindow.h
    trianglewindow.cpp
    trianglecanvas.h
    trianglecanvas.cpp)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

if(WIN32)
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_PREFIX_PATH}/include)

    find_library(GLEW_LIB glew32)
    target_link_libraries(${PROJECT_NAME} ${GLEW_LIB})
endif()
