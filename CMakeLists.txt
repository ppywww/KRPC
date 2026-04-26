#设置最低版本和项目名称
cmake_minimum_required(VERSION 3.0)
project(Krpc)

#设置全局的C++标准
set(CMAKE_CXX_STANDARD 11)#在这里我设置了11，代表使用c++11标准
set(CMAKE_CXX_STANDARD_REQUIRED True)#CMAKE_CXX_STANDARD_REQUIRED表示强制使用指定的标准


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#查找porotbuf包
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
#Protobuf_INCLUDE_DIRS表示protobuf头文件目录

# 设置 Muduo 库的路径
set(MUDUO_INCLUDE_DIR ~/muduo)
set(MUDUO_LIBRARY_DIR ~/build/release-cpp11/lib)

# 添加头文件目录
include_directories(${MUDUO_INCLUDE_DIR})

# 添加库文件目录
link_directories(${MUDUO_LIBRARY_DIR})
#设置全局链接库
set(LIBS
    protobuf
    pthread
    zookeeper_mt
    muduo_net
    muduo_base
    glog
)
#添加子目录
add_subdirectory(src)
add_subdirectory(example)
