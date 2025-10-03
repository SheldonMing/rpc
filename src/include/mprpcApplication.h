#pragma once
#include "mprpcConfig.h"

class MprpcApplication
{
public:
    static void Init(int argc, char **argv);
    static MprpcApplication &GetInstance();

private:
    static MprpcConfig m_config;

    MprpcApplication() = default;
    MprpcApplication(const MprpcApplication &) = delete;
    MprpcApplication &operator=(const MprpcApplication &) = delete;
};