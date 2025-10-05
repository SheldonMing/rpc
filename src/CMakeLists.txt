aux_source_directory(. SRC_LIST) # 获取当前目录下的所有源文件
add_library(mprpc ${SRC_LIST}) # 生成静态库

target_link_libraries(mprpc muduo_net muduo_base pthread) # 链接muduo_net和protobuf库
