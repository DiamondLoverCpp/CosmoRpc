#include <iostream>
#include <string>
#include <user.pb.h>
#include "rpc_application.h"
#include "rpc_server.h"

class UserService : public diamondlover::UserServiceRpc
{
public:
    static bool Login(const std::string &user, const std::string &pwd)
    {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "user: " << user << " pwd: " << pwd << std::endl;
        return true;
    }

    void Login(google::protobuf::RpcController *controller,
                const diamondlover::LoginRequest *request,
                diamondlover::LoginResponse *response,
                google::protobuf::Closure *done) override
    {
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool login_result = Login(name, pwd);

        diamondlover::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        done->Run();
    }
};

int main(int argc, char *argv[])
{
    RpcApplication::Init(argc, argv);

    RpcServer server;
    server.RegisterService(new UserService());

    server.Run();
    return 0;
}
