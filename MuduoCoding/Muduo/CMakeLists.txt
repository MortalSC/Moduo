cmake_minimum_required(VERSION 2.5)
project(mymuduo)

# mymuduo 最终编译生成动态库 .so
# 设置动态库路径，放在根目录的 lib 文件夹目录下
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/dir)
# 设置调试信息 以及 启用 c++11 语言标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

# 定义参与编译的代码源文件
aux_source_directory(. SRC_LIST)

# 编译动态库
add_library(mymuduo SHARED ${SRC_LIST})