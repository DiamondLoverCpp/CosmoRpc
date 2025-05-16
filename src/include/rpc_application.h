#pragma once
#include <rpc_config.h>
#include "rpc_channel.h"
#include "rpc_controller.h"

class RpcApplication
{
public:
    static void Init(int argc, char* argv[]);
    RpcApplication(const RpcApplication&) = delete;
    RpcApplication& operator=(const RpcApplication&) = delete;

    static RpcApplication& GetInstance();
    static RpcConfig& GetConfig();

private:
    RpcApplication() = default;
    inline static RpcConfig m_config{};
};
