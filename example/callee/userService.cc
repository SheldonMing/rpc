#include <iostream>
#include <string>

#include "mprpcApplication.h"
#include "rpcProvider.h"
#include "user.pb.h"

class UserService : public RPC::UserServiceRpc
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name: " << name << " pwd: " << pwd << std::endl;
        return true;
    }

    /**
     * 重写基类的虚函数 下面的Login方法是框架直接调用的
     * 1. caller ==> Login(LoginRequest) ==> muduo ==> callee
     * 2. callee ==> Login(LoginRequest, LoginResponse) ==> 框架
     */
    void Login(::google::protobuf::RpcController *controller,
               const ::RPC::LoginRequest *request,
               ::RPC::LoginResponse *response,
               ::google::protobuf::Closure *done) override
    {
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool login_result = Login(name, pwd);

        RPC::ResultCode *code = new RPC::ResultCode();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_allocated_result(code);
        response->set_sucess(login_result);

        done->Run();
    }
};

int main(int argc, char **argv)
{
    MprpcApplication::Init(argc, argv);

    RpcProvider provider;
    provider.NotifyService(new UserService());

    provider.Run();

    return 0;
}