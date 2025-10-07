#include "mprpcController.h"

MprpcController::MprpcController()
{
    error_text_ = "";
    failed_ = false;
    canceled_ = false;
    cancel_callback_ = nullptr;
}

MprpcController::~MprpcController() = default;

void MprpcController::Reset()
{
    error_text_ = "";
    failed_ = false;
    canceled_ = false;
    cancel_callback_ = nullptr;
}

bool MprpcController::Failed() const
{
    return failed_;
}
std::string MprpcController::ErrorText() const
{
    return error_text_;
}

void MprpcController::SetFailed(const std::string &reason)
{
    failed_ = true;
    error_text_ = reason;
}
