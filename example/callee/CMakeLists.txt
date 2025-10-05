set(SRC_LIST userService.cc ../user.pb.cc) # 源文件列表

add_executable(provider ${SRC_LIST}) # 生成可执行文件
target_link_libraries(provider mprpc protobuf) # 链接静态库和protobuf库