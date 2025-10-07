#include "mprpcChannel.h"
#include "rpcHeader.pb.h"
#include "mprpcApplication.h"
#include "mprpcController.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

/**
 * header_size + header_str + args_str
 * header_str: service_name + method_name + args_size
 */
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller,
                              const google::protobuf::Message *request,
                              google::protobuf::Message *response,
                              google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service();
    std::string service_name = sd->name();    // 获取服务名称
    std::string method_name = method->name(); // 获取方法名称

    // 获取参数的序列化字符串长度 args_size
    size_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        char err_txt[1024] = {0};
        sprintf(err_txt, "request SerializeToString error!");
        controller->SetFailed(err_txt);
        return;
    }

    // 定义rpc请求的header
    RPC::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    std::string rpc_header_str;
    if (!rpcHeader.SerializeToString(&rpc_header_str))
    {
        char err_txt[1024] = {0};
        sprintf(err_txt, "rpcHeader SerializeToString error!");
        controller->SetFailed(err_txt);
        return;
    }

    uint32_t header_size = rpc_header_str.size();

    // 组织待发送的rpc请求字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char *)&header_size, 4)); // header_size
    send_rpc_str += rpc_header_str;                               // header_str
    send_rpc_str += args_str;                                     // args_str

    std::cout << "===============Caller===================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "========================================" << std::endl;

    // 使用tcp编程，完成rpc方法的远程调用
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        close(client_fd);
        char err_txt[1024] = {0};
        sprintf(err_txt, "socket error!:%d", errno);
        controller->SetFailed(err_txt);
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;

    std::string ip = MprpcApplication::GetConfig().Load("rpcserverip");
    std::string port = MprpcApplication::GetConfig().Load("rpcserverport");
    server_addr.sin_port = htons(atoi(port.c_str()));
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)))
    {
        close(client_fd);
        char err_txt[1024] = {0};
        sprintf(err_txt, "connect error!:%d", errno);
        controller->SetFailed(err_txt);
        return;
    }

    // 发送rpc请求
    if (send(client_fd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1)
    {
        close(client_fd);
        char err_txt[1024] = {0};
        sprintf(err_txt, "send error!:%d", errno);
        controller->SetFailed(err_txt);
        return;
    }

    // 接收rpc响应结果
    char buf[1024] = {0};
    size_t recv_size = 0;
    if ((recv_size = recv(client_fd, buf, 1024, 0)) == -1)
    {
        close(client_fd);
        char err_txt[1024] = {0};
        sprintf(err_txt, "recv error!:%d", errno);
        controller->SetFailed(err_txt);
        return;
    }

    // 反序列化rpc响应结果
    std::string response_str(buf, 0, recv_size);
    if (!response->ParseFromString(response_str))
    {
        close(client_fd);
        char err_txt[1024] = {0};
        sprintf(err_txt, "response ParseFromString error! response_str:%s", response_str.c_str());
        controller->SetFailed(err_txt);
        return;
    }
    close(client_fd);
}