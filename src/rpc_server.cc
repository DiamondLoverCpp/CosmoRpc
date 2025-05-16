#include "rpc_server.h"
#include <string>
#include <google/protobuf/descriptor.h>
#include <muduo/net/InetAddress.h>
#include "rpc_application.h"
#include "rpc_header.pb.h"

void RpcServer::RegisterService(google::protobuf::Service *service)
{
    ServiceInfo service_info;
    service_info.m_service = service;

    const google::protobuf::ServiceDescriptor *service_descriptor = service->GetDescriptor();
    std::string service_name = service_descriptor->name();
    int methodCnt = service_descriptor->method_count();

    for (int i = 0; i < methodCnt; ++i)
    {
        const google::protobuf::MethodDescriptor *method_descriptor = service_descriptor->method(i);
        std::string method_name = method_descriptor->name();
        service_info.m_methodMap[method_name] = method_descriptor;
    }
    m_servicesMap[service_name] = service_info;
}

void RpcServer::Run()
{
    std::string ip = RpcApplication::GetInstance().GetConfig().GetValue("rpc_server_ip");
    uint16_t port = std::stoul(RpcApplication::GetInstance().GetConfig().GetValue("rpc_server_port"));
    muduo::net::InetAddress addr(ip, port);

    muduo::net::TcpServer server(&m_eventloop, addr, "RpcServer");
    server.setConnectionCallback(std::bind(&RpcServer::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcServer::OnMessage, this, std::placeholders::_1,
                                        std::placeholders::_2, std::placeholders::_3));

    server.setThreadNum(4);

    std::cout << "RpcServer start service at ip: " << ip << " port: " << port << std::endl;

    server.start();
    m_eventloop.loop();
}

void RpcServer::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
    }
}

void RpcServer::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp time)
{
    std::string recv_buf = buffer->retrieveAllAsString();

    uint32_t rpc_header_size = 0;
    recv_buf.copy(reinterpret_cast<char *>(&rpc_header_size), 4, 0);

    std::string rpc_header_str = recv_buf.substr(4, rpc_header_size);
    diamondlover::RpcHeader rpc_header;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (rpc_header.ParseFromString(rpc_header_str))
    {
        service_name = rpc_header.service_name();
        method_name = rpc_header.method_name();
        args_size = rpc_header.args_size();
    } else
    {
        std::cout << "rpc_header ParseFromString failed!" << std::endl;
        return;
    }

    std::string args_str = recv_buf.substr(rpc_header_size + 4, args_size);

    // std::cout << "service_name: " << service_name << std::endl;
    // std::cout << "method_name: " << method_name << std::endl;
    // std::cout << "args_size: " << args_size << std::endl;
    // std::cout << "header_size: " << rpc_header_size << std::endl;
    // std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    // std::cout << "args_str: " << args_str << std::endl;

    auto it = m_servicesMap.find(service_name);
    if (it == m_servicesMap.end())
    {
        std::cout << "service not registered!" << std::endl;
        return;
    }

    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end())
    {
        std::cout << "method not registered!" << std::endl;
        return;
    }

    google::protobuf::Service *service = it->second.m_service;
    const google::protobuf::MethodDescriptor *method_descriptor = mit->second;

    google::protobuf::Message *request = service->GetRequestPrototype(method_descriptor).New();
    if (!request->ParseFromString(args_str))
    {
        std::cout << "parse request failed!" << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method_descriptor).New();

    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcServer, const muduo::net::TcpConnectionPtr &,
        const google::protobuf::Message *>(this, &RpcServer::SendRpcResponse, conn, response);

    service->CallMethod(method_descriptor, nullptr, request, response, done);
}

void RpcServer::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, const google::protobuf::Message *response)
{
    std::string response_str;
    if (response->SerializeToString(&response_str))
    {
        conn->send(response_str);
    }else
    {
        std::cout << "response serialize failed!" << std::endl;
    }
    conn->shutdown();
}
