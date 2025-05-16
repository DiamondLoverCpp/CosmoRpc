#include "rpc_config.h"
#include <fstream>
#include <iostream>

void RpcConfig::LoadConfigFile(const std::string& file_path)
{
    std::ifstream file(file_path);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file " << file_path << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line.find('#') != std::string::npos)
        {
            continue;
        }

        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos)
        {
            continue;
        }
        std::string key = trim(line.substr(0, equalPos));
        std::string value = trim(line.substr(equalPos + 1));

        if (!key.empty() && !value.empty())
        {
            m_configMap[key] = value;
        }
    }

    file.close();
}

std::string RpcConfig::trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string RpcConfig::GetValue(const std::string& key)
{
    auto it = m_configMap.find(key);
    if (it == m_configMap.end())
    {
        return "";
    }
    return it->second;
}