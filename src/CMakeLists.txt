

#获取当前目录下的所有源文件
file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# 获取 protobuf 的生成文件
file(GLOB PROTO_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.pb.cc)

#创建静态库或共享库
add_library(krpc_core SHARED ${SRC_FILES} ${PROTO_SRCS})

#链接protobuf库
target_link_libraries(krpc_core PUBLIC protobuf)

#设置头文件的路径
target_include_directories(krpc_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加编译选项
target_compile_options(krpc_core PRIVATE -std=c++11 -Wall)