cmake_minimum_required(VERSION 3.25)

project(OpenGLProjects LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(glm3)
add_subdirectory(Examples EXCLUDE_FROM_ALL)
