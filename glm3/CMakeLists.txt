cmake_minimum_required(VERSION 3.15)

project(glm3)

set(CMAKE_CXX_VISIBILITY_PRESET     hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

set(SOURCES
    include/camera.h
    include/shaderprogram.h
    include/texture.h
    src/texture.cpp
    src/shaderprogram.cpp)

# The preferred practice is not to specify the type of library to be built and
# leave the choice to the user
add_library(${PROJECT_NAME} ${SOURCES})

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

# Add the library's binary directory so other targets find the export file
target_include_directories(${PROJECT_NAME}
    PUBLIC include
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    PRIVATE src)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} GL)
    target_link_libraries(${PROJECT_NAME} GLEW)
    target_link_libraries(${PROJECT_NAME} glfw)
endif()

if(WIN32)
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_PREFIX_PATH}/include)

    target_link_libraries(${PROJECT_NAME} opengl32)
    find_library(GLEW_LIB glew32)
    target_link_libraries(${PROJECT_NAME} ${GLEW_LIB})
    find_library(GLFW_LIB glfw3dll)
    target_link_libraries(${PROJECT_NAME} ${GLFW_LIB})
    find_library(FREEIMAGE_LIB FreeImage)
    target_link_libraries(${PROJECT_NAME} ${FREEIMAGE_LIB})
endif()
