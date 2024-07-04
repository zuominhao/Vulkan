# src/game/CMakeLists.txt

# 递归地搜索 game 目录中的所有源文件
file(GLOB_RECURSE game_src CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*" PATH_SUFFIXES .cpp .hpp .h .cc)

# 包含头文件目录
include_directories(${CMAKE_SOURCE_DIR}/src/e3d/include)

# 创建可执行文件
add_executable(GameExecutable ${game_src})

# 链接 e3d 动态库
target_link_libraries(GameExecutable PRIVATE e3d)
