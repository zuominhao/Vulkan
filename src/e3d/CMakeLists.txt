# src/e3d/CMakeLists.txt

# 递归地搜索 e3d 目录中的所有源文件
file(GLOB_RECURSE e3d_src CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*" "${CMAKE_CURRENT_LIST_DIR}/include/*" PATH_SUFFIXES .cpp .h .cc)

# 创建动态库
add_library(e3d SHARED ${e3d_src})

# 链接所需的库
target_link_libraries(e3d PRIVATE SDL2::SDL2 Eigen3::Eigen imgui::imgui)

#包含目录
target_include_directories(e3d PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

