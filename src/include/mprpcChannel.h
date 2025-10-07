#pragma once

#include <google/protobuf/service.h>

class MprpcChannel : public google::protobuf::RpcChannel
{
public:
    MprpcChannel() = default;
    ~MprpcChannel() override = default;

    // 重写基类的虚函数 通过stub代理对象调用的方法会最终调用到这个方法上
    void CallMethod(const google::protobuf::MethodDescriptor *method,
                    google::protobuf::RpcController *controller,
                    const google::protobuf::Message *request,
                    google::protobuf::Message *response,
                    google::protobuf::Closure *done) override;
};