cmake_minimum_required(VERSION 3.5)
project(glad VERSION 0.0.1 LANGUAGES C)


file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c)

add_library(${PROJECT_NAME} STATIC  ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)