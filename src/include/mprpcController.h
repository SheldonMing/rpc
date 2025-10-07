#pragma once

#include <google/protobuf/service.h>
#include <string>

class MprpcController : public google::protobuf::RpcController
{
public:
    MprpcController();
    ~MprpcController() override;

    void Reset() override;
    bool Failed() const override;
    std::string ErrorText() const override;
    void SetFailed(const std::string &reason) override;

    void StartCancel() override {};
    bool IsCanceled() const override { return canceled_; };
    void NotifyOnCancel(google::protobuf::Closure *callback) override {};

private:
    std::string error_text_;
    bool failed_ = false;
    bool canceled_ = false;
    google::protobuf::Closure *cancel_callback_ = nullptr;
};