#include <iostream>
#include "mprpcApplication.h"
#include "mprpcController.h"
#include "mprpcChannel.h"
#include "user.pb.h"

int main(int argc, char **argv)
{
    // 调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    // 业务代码 - Login
    RPC::UserServiceRpc_Stub stub(new MprpcChannel());
    RPC::LoginRequest request;
    request.set_name("zhangsan");
    request.set_pwd("123456");
    RPC::LoginResponse response;

    // 发起rpc方法的调用 同步的rpc调用过程  MprpcChannel::CallMethod
    MprpcController controller;
    stub.Login(&controller, &request, &response, nullptr);

    // 一次rpc调用完成，读调用结果
    if (controller.Failed())
    {
        std::cout << "rpc login failed! error: " << controller.ErrorText() << std::endl;
    }
    else
    {
        if (response.result().errcode() == 0)
        {
            std::cout << "rpc login response success: " << response.sucess() << std::endl;
        }
        else
        {
            std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
        }
    }

    // 业务代码2 - Register
    RPC::RegisterRequest req;
    req.set_id(1000);
    req.set_name("zhangsan");
    req.set_pwd("123456");
    RPC::RegisterResponse resp;

    MprpcController controller2;
    stub.Register(&controller2, &req, &resp, nullptr);
    if (controller2.Failed())
    {
        std::cout << "rpc register failed! error: " << controller2.ErrorText() << std::endl;
    }
    else
    {
        if (resp.result().errcode() == 0)
        {
            std::cout << "rpc register response success: " << resp.sucess() << std::endl;
        }
        else
        {
            std::cout << "rpc register response error: " << resp.result().errmsg() << std::endl;
        }
    }

    return 0;
}