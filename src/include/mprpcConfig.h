#pragma once

#include <unordered_map>
#include <string>

// Configuration class for the mprpc framework
class MprpcConfig
{
public:
    MprpcConfig() = default;
    ~MprpcConfig() = default;
    // 加载配置文件
    void LoadConfigFile(const std::string &filePath);
    // 获取配置项
    std::string Load(const std::string &key) const;

private:
    void Trim(std::string &strLine);
    std::unordered_map<std::string, std::string> m_configMap;
};