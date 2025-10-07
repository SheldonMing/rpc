#include "rpcProvider.h"
#include "mprpcApplication.h"
#include <string>
#include <functional>
#include <google/protobuf/descriptor.h>
#include "rpcHeader.pb.h"

/**
 * service : 服务对象
 * 1. 获取服务对象的描述信息（服务名字，服务方法）
 * 2. 将服务对象存储到本地的服务映射表中
 */
void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo serviceInfo;
    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pServiceDesc = service->GetDescriptor();
    // 获取服务名字
    std::string serviceName = pServiceDesc->name();
    // 获取服务对象的服务方法数量
    int methodCnt = pServiceDesc->method_count();

    for (size_t i = 0; i < methodCnt; ++i)
    {
        // 获取服务对象指定下标的服务方法的描述（抽象描述）
        const google::protobuf::MethodDescriptor *pMethodDesc = pServiceDesc->method(i);
        std::string methodName = pMethodDesc->name();
        // 将服务对象和服务名字保存到ServiceInfo中
        serviceInfo.m_methodMap.insert({methodName, pMethodDesc});
    }
    serviceInfo.m_service = service;
    serviceMap_.insert({serviceName, serviceInfo});
}

void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
    // 绑定连接回调和消息回调方法
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this,
                                        std::placeholders::_1,
                                        std::placeholders::_2,
                                        std::placeholders::_3));
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setThreadNum(4); // 设置线程数量

    // rpc服务端启动
    std::cout << "RpcProvider start at " << ip << ":" << port << std::endl;

    // 启动服务
    server.start();
    m_eventLoop.loop();
}

// 新的socket连接回调
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
    }
}

/**
 * 框架内部，rpcProvider和rpcConsumer之间协商好通信用的protobuf数据类型
 * service_name + method_name + args
 * 在框架中定义proto的Message类型，进展行数据的序列化和反序列化
 *
 * header_size(4 bytes) + header_str + args_str
 *
 * std::string  insert和copy
 */
// 已建立连接用户的读写事件回调，远程rpc调用请求的处理函数
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn,
                            muduo::net::Buffer *buffer,
                            muduo::Timestamp receiveTime)
{
    // 网络上接收的远程rpc调用请求的字符流      方法名+参数
    std::string recv_buf = buffer->retrieveAllAsString();

    // 从字符流中读取前4个字节的header_size
    uint32_t header_size = 0;
    recv_buf.copy((char *)&header_size, 4, 0);

    // 根据header_size读取数据头的原始字符流，反序列化，得到rpc请求的详细信息
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    RPC::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (!rpcHeader.ParseFromString(rpc_header_str))
    {
        std::cout << "rpcHeader.ParseFromString error!" << std::endl;
        return;
    }
    else
    {
        // 获取服务名，方法名，参数大小
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }

    // 获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    // 打印调试信息
    std::cout << "=============provider===================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "========================================" << std::endl;

    // 获取service对象和method对象
    auto it = serviceMap_.find(service_name);
    if (it == serviceMap_.end())
    {
        std::cout << service_name << " not found!" << std::endl;
        return;
    }
    auto it_method = it->second.m_methodMap.find(method_name);
    if (it_method == it->second.m_methodMap.end())
    {
        std::cout << method_name << " not found!" << std::endl;
        return;
    }

    google::protobuf::Service *service = it->second.m_service;            // 获取服务对象
    const google::protobuf::MethodDescriptor *method = it_method->second; // 获取方法对象

    // 生成rpc方法调用的请求request和响应response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))
    {
        std::cout << "request.ParseFromString error!" << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // 给下面的method方法调用，绑定一个Closure回调函数
    google::protobuf::Closure *done =
        google::protobuf::NewCallback<RpcProvider,
                                      const muduo::net::TcpConnectionPtr &,
                                      google::protobuf::Message *>(this, &RpcProvider::SendRpcResponse, conn, response);

    // 在框架上调用当前method方法，执行远程rpc服务
    service->CallMethod(method, nullptr, request, response, done);
}

// Closure回调函数，序列化rpc响应参数并通过网络发送给rpc调用方
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn,
                                  google::protobuf::Message *response)
{
    // 序列化rpc响应参数response
    std::string response_str;
    if (!response->SerializeToString(&response_str))
    {
        std::cout << "response.SerializeToString error!" << std::endl;
        return;
    }
    uint32_t response_size = response_str.size();

    // 将response_size和response一并发送给rpc调用方
    std::string send_buf;
    send_buf.insert(0, (char *)&response_size, 4);
    send_buf += response_str;

    // 通过网络将rpc响应结果发送回rpc调用方
    conn->send(send_buf);
    conn->shutdown(); // 短连接
}