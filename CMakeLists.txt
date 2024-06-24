cmake_minimum_required(VERSION 3.15)
project(MyProject)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY)


find_package(Vulkan REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)


function(add_shaders)
 cmake_parse_arguments(ARG "" "PREFIX;OUT_DIR" "SOURCES" ${ARGN})

 # 检查是否提供了源文件和输出目录
 if(NOT ARG_SOURCES)
   message(FATAL_ERROR "No shader sources provided for add_shaders function.")
 endif()

 if(NOT ARG_OUT_DIR)
   message(FATAL_ERROR "No output directory provided for add_shaders function.")
 endif()

 foreach(shader ${ARG_SOURCES})
   get_filename_component(shader_name ${shader} NAME)
   set(output_file "${ARG_OUT_DIR}/${shader_name}.spv")
   
   add_custom_command(OUTPUT ${output_file} COMMAND Vulkan::glslangValidator -V ${shader} -o ${output_file} DEPENDS ${shader}
     COMMENT "Compiling shader ${shader} to ${output_file}"
   )

   list(APPEND compiled_shaders ${output_file})
 endforeach()
 add_custom_target(shaders ALL DEPENDS ${compiled_shaders})
 
endfunction(add_shaders)


file(GLOB_RECURSE shader_files CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/e3d/shaders/*" PATH_SUFFIXES .vert .frag .comp)
add_shaders(shaders SOURCES ${shader_files} PREFIX "${CMAKE_CURRENT_LIST_DIR}/src" OUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders)


file(GLOB_RECURSE srclist CONFIGURE_DEPENDS "src/*" PATH_SUFFIXES .cpp .hpp .h .cc)
add_executable(MyExecutable ${srclist})
target_link_libraries(MyExecutable PRIVATE SDL2::SDL2)
target_link_libraries(MyExecutable PRIVATE Eigen3::Eigen)
target_link_libraries(MyExecutable PRIVATE imgui::imgui)