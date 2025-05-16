#include "rpc_channel.h"
#include <string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "rpc_header.pb.h"
#include "rpc_application.h"
#include "rpc_controller.h"

//header_size + service_name method_name args_size + args
void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                            google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                            google::protobuf::Message *response, google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *service_descriptor = method->service();
    std::string service_name = service_descriptor->name();
    std::string method_name = method->name();

    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }else
    {
        controller->SetFailed("serialize request error: ");
        return ;
    }

    diamondlover::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(args_size);

    std::string rpc_header_str;
    uint32_t rpc_header_size = 0;
    if (rpc_header.SerializeToString(&rpc_header_str))
    {
        rpc_header_size = rpc_header_str.size();
    }else
    {
        controller->SetFailed("serialize rpc_header error");
        return;
    }

    std::string send_rpc_str;
    send_rpc_str.insert(0,std::string(reinterpret_cast<char *>(&rpc_header_size),4));
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

    // std::cout << "service_name: " << service_name << std::endl;
    // std::cout << "method_name: " << method_name << std::endl;
    // std::cout << "args_size: " << args_size << std::endl;
    // std::cout << "rpc_header_size: " << rpc_header_size << std::endl;
    // std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    // std::cout << "args_str: " << args_str << std::endl;

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        controller->SetFailed("create socket error");
        return;
    }

    std::string ip = RpcApplication::GetInstance().GetConfig().GetValue("rpc_server_ip");
    uint16_t port = std::stoul(RpcApplication::GetInstance().GetConfig().GetValue("rpc_server_port"));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(port);

    if (connect(client_fd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) == -1)
    {
        controller->SetFailed("connect error");
        close(client_fd);
        return;
    }

    if (send(client_fd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1)
    {
        controller->SetFailed("send error");
        close(client_fd);
        return;
    }

    char recv_buf[1024] = {0};
    int recv_size = 0;
    if ((recv_size = recv(client_fd,recv_buf, sizeof(recv_buf), 0)) == -1)
    {
        controller->SetFailed("recv error");
        close(client_fd);
        return;
    }

    if (!response->ParseFromArray(recv_buf, recv_size))
    {
        controller->SetFailed("rpc response error");
        close(client_fd);
        return;
    }
    close(client_fd);
}
