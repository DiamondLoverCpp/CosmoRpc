#include <iostream>
#include "rpc_application.h"
#include "user.pb.h"

int main(int argc, char *argv[])
{
    RpcApplication::Init(argc, argv);
    diamondlover::UserServiceRpc_Stub stub(new RpcChannel());

    diamondlover::LoginRequest request;
    request.set_name("diamondlover");
    request.set_pwd("12800");

    diamondlover::LoginResponse response;

    RpcController controller;
    stub.Login(&controller, &request, &response, nullptr);

    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    } else
    {
        if (response.result().errcode() == 0)
        {
            std::cout << "rpc login success: " << response.success() << std::endl;
        } else
        {
            std::cout << "rpc login error: " << response.result().errmsg() << std::endl;
        }
    }

    return 0;
}
