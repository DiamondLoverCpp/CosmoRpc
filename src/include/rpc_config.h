#pragma once
#include <unordered_map>
#include <string>

class RpcConfig
{
public:
    void LoadConfigFile(const std::string& file_path);
    std::string GetValue(const std::string& key);

private:
    std::unordered_map<std::string, std::string> m_configMap;
    static std::string trim(const std::string& str);
};
