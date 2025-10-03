cmake_minimum_required(VERSION 3.0)
project(mprpc)

# gdb调试
set(CMAKE_BUILD_TYPE "Debug")

# 可执行文件输出路径    
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 库文件输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# protobuf安装路径
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

# 编译头文件搜索路径 -I
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)
# 编译库文件搜索路径 -L
link_directories(${PROJECT_SOURCE_DIR}/lib)

# src包含了mprpc框架的所有源文件
add_subdirectory(src)
# example包含了所有的测试代码
add_subdirectory(example)
