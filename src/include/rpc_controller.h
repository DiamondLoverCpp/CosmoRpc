#pragma once
#include <google/protobuf/service.h>
#include <string>

class RpcController : public google::protobuf::RpcController
{
public:
    RpcController();
    void Reset() override;
    bool Failed() const override;
    std::string ErrorText() const override;
    void SetFailed(const std::string &reason) override;

    //目前未实现
    void StartCancel() override;
    bool IsCanceled() const override;
    void NotifyOnCancel(google::protobuf::Closure* callback) override;

private:
    bool m_failed;
    std::string m_errorText;
};