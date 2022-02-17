cmake_minimum_required(VERSION 3.5)
project(Opium_App VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.cpp", "src/*/*.cpp", "src/*/*/*.cpp", "src/*/*/*/*.cpp")

# Opium_App
add_executable(${PROJECT_NAME} ${SOURCES})


target_link_libraries(${PROJECT_NAME} Opium_Engine)
target_include_directories(${PROJECT_NAME} PRIVATE Opium_Engine)