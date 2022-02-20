cmake_minimum_required(VERSION 3.5)
project(Opium_Engine VERSION 0.0.1 LANGUAGES CXX)

#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Apps/OpiumApp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Apps/OpiumApp)

# add external libraries
add_subdirectory(external/glm)
add_subdirectory(external/glfw)
add_subdirectory(external/spdlog)
add_subdirectory(external/glad)
add_subdirectory(external/imgui)

# find opengl
find_package(OpenGL REQUIRED)

file(GLOB SOURCES "src/*.cpp", "src/*/*.cpp", "src/*/*/*.cpp", "src/*/*/*/*.cpp")

add_library(${PROJECT_NAME} SHARED  ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        glm
        imgui
    PRIVATE
        ${OPENGL_INCLUDE_DIRS}
        glfw
        glad
)

target_precompile_headers(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Config/Precomp.h
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        glm
        spdlog::spdlog_header_only
        imgui
    PRIVATE
        glad
        glfw
        ${OPENGL_LIBRARIES}
)

# Compiler specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
    # Set Definition for OP_BUILD_DLL to correctly determine importing/exporting
    target_compile_definitions(${PROJECT_NAME} PRIVATE OP_BUILD_DLL)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /MDd) # /MTd /W3 /EHsc /c
    # Set Definition for OP_BUILD_DLL to correctly determine importing/exporting
    target_compile_definitions(${PROJECT_NAME} PRIVATE OP_BUILD_DLL OP_ENABLE_ASSERTS GLFW_INCLUDE_NONE)
endif()