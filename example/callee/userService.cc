#include <iostream>
#include <string>

#include "user.pb.h"

class UserService : public UserServiceRpc
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name: " << name << " pwd: " << pwd << std::endl;
        return true;
    }

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
        response->set_sucess(login_result);

        done->Run();
    }
};

int main(int argc, char **argv)
{
    MprpcApplication::Init(argc, argv);

    MprpcProvider provider;
    provider.NotifyService(new UserService());

    provider.Run();

    return 0;
}