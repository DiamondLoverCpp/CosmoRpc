#pragma once
#include <google/protobuf/service.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <unordered_map>

class RpcServer
{
public:
    void RegisterService(google::protobuf::Service* service);

    void Run();

private:
    muduo::net::EventLoop m_eventloop;
    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    void OnMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer, muduo::Timestamp time);

    struct ServiceInfo
    {
        google::protobuf::Service* m_service;
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;
    };
    std::unordered_map<std::string, ServiceInfo> m_servicesMap;

    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, const google::protobuf::Message* response);
};
