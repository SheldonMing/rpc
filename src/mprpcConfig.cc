#include "mprpcConfig.h"

#include <iostream>
#include <string>

// 去掉字符串前后的空格
void MprpcConfig::Trim(std::string &strLine)
{
    if (strLine.empty())
        return;

    // 截取得到首尾均没有空格的字符串
    size_t idx = strLine.find_first_not_of(' ');
    if (idx != std::string::npos)
    {
        strLine = strLine.substr(idx);
    }
    if ((idx = strLine.find_last_not_of(' ')) != std::string::npos)
    {
        strLine = strLine.substr(0, idx + 1);
    }
}

void MprpcConfig::LoadConfigFile(const std::string &filePath)
{
    FILE *file = fopen(filePath.c_str(), "r");
    if (nullptr == file)
    {
        std::cerr << "Cannot open config file: " << filePath << std::endl;
        exit(EXIT_FAILURE);
    }

    while (!feof(file))
    {
        char line[512] = {0};
        fgets(line, 512, file); // 按行读取配置文件
        std::string strLine(line);

        Trim(strLine);

        // 跳过空行和注释行
        if (strLine.empty() || strLine[0] == '#')
        {
            continue;
        }

        size_t idx = 0;
        // 解析配置项
        if ((idx = strLine.find('=')) == std::string::npos)
        {
            continue; // Skip lines without '='
        }

        std::string key = strLine.substr(0, idx);
        Trim(key);
        size_t end = strLine.find('\n', idx);
        std::string value = strLine.substr(idx + 1, end - idx - 1);
        Trim(value);
        m_configMap[key] = value;
    }
    fclose(file);
}

std::string MprpcConfig::Load(const std::string &key) const
{
    auto it = m_configMap.find(key);
    if (it != m_configMap.end())
    {
        return it->second;
    }
    return "";
}
