cmake_minimum_required(VERSION 3.15)
project(MyProject)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB SOURCES "src/*.cpp" "src/*.hpp")

add_executable(MyExecutable ${SOURCES})

target_link_libraries(MyExecutable PRIVATE glm::glm-header-only)
target_link_libraries(MyExecutable PRIVATE glfw)
target_link_libraries(MyExecutable PRIVATE Vulkan::Vulkan)