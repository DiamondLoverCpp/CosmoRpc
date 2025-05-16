#include "rpc_application.h"
#include <cstdlib>
#include <iostream>
#include <unistd.h>

void ShowArgsHelp()
{
    std::cout << "format: command -i <configfile>" << std::endl;
}

void RpcApplication::Init(int argc, char* argv[])
{
    if (argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int opt = 0;
    std::string config_file;
    while ((opt = getopt(argc, argv, "i:")) != -1)
    {
        switch (opt)
        {
            case 'i':
                config_file = optarg;
                break;
            case '?':
                ShowArgsHelp();
                exit(EXIT_FAILURE);
            case ':':
                ShowArgsHelp();
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }

    m_config.LoadConfigFile(config_file);

    // std::cout << "rpc_server_ip: " << m_config.GetValue("rpc_server_ip") << std::endl;
    // std::cout << "rpc_server_port: " << m_config.GetValue("rpc_server_port") << std::endl;
    // std::cout << "zookeeper_ip: " << m_config.GetValue("zookeeper_ip") << std::endl;
    // std::cout << "zookeeper_port: " << m_config.GetValue("zookeeper_port") << std::endl;
}

RpcApplication& RpcApplication::GetInstance()
{
    static RpcApplication app;
    return app;
}

RpcConfig& RpcApplication::GetConfig()
{
    return m_config;
}