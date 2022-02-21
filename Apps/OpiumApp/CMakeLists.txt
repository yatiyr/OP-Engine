cmake_minimum_required(VERSION 3.5)
project(Opium_App VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# I want to use imgui in this application but when I bind imgui static library
# to opium_engine.dll, imgui context becomes null because dll changes boundary
# I have turned imgui libs private in opium engine and statically include the
# imgui library in this application
set (imguiLib ${CMAKE_BINARY_DIR}/Engine/external/imgui/imgui.lib)
set (imguiIncludeDir ${CMAKE_SOURCE_DIR}/Engine/external/imgui)

message(STATUS " ImGui Lib Path has been configured as: ${imguiLib}")
message(STATUS " ImGui Include dir has been configured as: ${imguiIncludeDir}")

file(GLOB SOURCES "src/*.cpp", "src/*/*.cpp", "src/*/*/*.cpp", "src/*/*/*/*.cpp")

# Opium_App
add_executable(${PROJECT_NAME} ${SOURCES})


target_link_libraries(${PROJECT_NAME} Opium_Engine)
target_include_directories(${PROJECT_NAME} PRIVATE Opium_Engine)