# rpc
<!-- 环境配置 -->
## 1. 安装protobuf 3.7.0 （版本较低，则会缺失port_undef.inc，port_def.inc等文件）
- 下载地址：https://github.com/protocolbuffers/protobuf/releases/tag/v3.7.0
- 解压，进入解压目录
```shell
./configure
make
make check
make install
ldconfig # 刷新动态链接库
```
- 验证
```shell
protoc --version
```
## 2. 安装muduo
- 下载地址：git clone https://github.com/chenshuo/muduo
- 解压，进入解压目录
```shell
git clone https://github.com/chenshuo/muduo
unzip muduo-master.zip
cd muduo-master
mkdir build
cd build
cmake ..
make
make install
ldconfig # 刷新动态链接库
```
- 验证
```shell
ls /usr/local/lib | grep muduo
```
## 3. 编译mprpc框架
- 进入rpc根目录
```shell
cd rpc
mkdir build
cd build
cmake ..
make
```

其他：
zookeeper-3.4.10
jdk1.8.0_192