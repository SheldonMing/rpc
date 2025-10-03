aux_source_directory(. SRC_LIST) # 获取当前目录下的所有源文件
add_library(mprpc SHARED ${SRC_LIST}) # 生成动态库
