# src/e3d/CMakeLists.txt

# 递归地搜索 e3d 目录中的所有源文件
file(GLOB_RECURSE e3d_src CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*" "${CMAKE_CURRENT_LIST_DIR}/include/*" PATH_SUFFIXES .cpp .hpp .h .cc)

# 包含头文件目录
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

# 创建动态库
add_library(e3d SHARED ${e3d_src})

# 明确指定生成导入库
set_target_properties(e3d PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

# 链接所需的库
target_link_libraries(e3d PUBLIC SDL2::SDL2 Eigen3::Eigen imgui::imgui)

