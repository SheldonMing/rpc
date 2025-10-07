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

    bool Register(uint32_t id, std::string name, std::string pwd)
    {
        std::cout << "doing local service: Register" << std::endl;
        std::cout << "id: " << id << " name: " << name << " pwd: " << pwd << std::endl;
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

        RPC::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("success");
        response->set_sucess(login_result);

        done->Run();
    }

    void Register(::google::protobuf::RpcController *controller,
                  const ::RPC::RegisterRequest *request,
                  ::RPC::RegisterResponse *response,
                  ::google::protobuf::Closure *done) override
    {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool ret = Register(id, name, pwd);

        RPC::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("success");
        response->set_sucess(ret);

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