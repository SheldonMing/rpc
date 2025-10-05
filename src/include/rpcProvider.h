#pragma once
#include <google/protobuf/service.h>

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <unordered_map>

class RpcProvider
{
public:
    // 发布服务，将服务对象发布到rpc节点上
    void NotifyService(google::protobuf::Service *service);
    // 启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();

private:
    // 保存服务对象
    // std::unique_ptr<muduo::net::TcpServer> m_tcpServerPtr;
    muduo::net::EventLoop m_eventLoop;

    // key: service_name  value: 服务对象+服务方法的映射
    struct ServiceInfo
    {
        google::protobuf::Service *m_service;                                                    // 服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> m_methodMap; // 存储服务方法
    };
    std::unordered_map<std::string, ServiceInfo> serviceMap_; // 存储所有发布的服务

    void OnConnection(const muduo::net::TcpConnectionPtr &conn);
    void OnMessage(const muduo::net::TcpConnectionPtr &conn,
                   muduo::net::Buffer *buffer,
                   muduo::Timestamp receiveTime);
    void SendRpcResponse(const muduo::net::TcpConnectionPtr &conn,
                         google::protobuf::Message *response);
};
