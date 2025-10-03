#pragma once
#include <google/protobuf/service.h>

class RpcProvider
{
public:
    // 发布服务，将服务对象发布到rpc节点上
    void NotifyService(google::protobuf::Service *service);
    // 启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();
};
