cmake_minimum_required(VERSION 3.15)
project(MyProject)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Vulkan REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

file(GLOB SOURCES "src/*.cpp" "src/*.hpp")


add_executable(MyExecutable ${SOURCES})
target_link_libraries(MyExecutable PRIVATE SDL2::SDL2)
target_link_libraries(MyExecutable PRIVATE Eigen3::Eigen)
target_link_libraries(MyExecutable PRIVATE imgui::imgui)