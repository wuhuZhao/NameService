//
// Created by 招浩楷 on 2021/12/8.
//
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#ifdef BAZEL_BUILD
#include "examples/protos/route_guide.grpc.pb.h"
#else
#include "NameService.grpc.pb.h"
#endif


class NameServiceClient {
public:
    NameServiceClient(std::shared_ptr<grpc::Channel> channel):stub_(NameService::NewStub(channel)) {

    };

    /**
     * 请求注册名字服务测试
     * @param name
     * @param ip
     * @param port
     * @return
     */
    MetaResponse registerNameService(std::string name, std::string ip, std::string port) {
        MetaRequest metaRequest;
        Meta meta;
        meta.set_ip(ip);
        meta.set_nameservice(name);
        meta.set_port(port);
        metaRequest.mutable_meta()->CopyFrom(meta);
        MetaResponse response;
        grpc::Status status = stub_->registerService(new grpc::ClientContext(), metaRequest, &response);
        if (!status.ok()) {
            std::cout << "request registerNameService fail!" << std::endl;
        }
        return response;
    };

    /***
     * 请求名字服务的ip:port
     * @param name
     * @return
     */
    MetaQueryResponse queryNameService(std::string name) {
        MetaQueryRequest request;
        request.set_nameservice(name);
        MetaQueryResponse response;
        grpc::Status status = stub_->queryService(new grpc::ClientContext, request, &response);
        if (!status.ok()) {
            std::cout << "request queryNameService fail!" << std::endl;
        }
        return response;
    }

private:
    std::unique_ptr<NameService::Stub> stub_;
};


int main() {
    std::string  target_str = "127.0.0.1:8081";
    NameServiceClient client(grpc::CreateChannel(
            target_str, grpc::InsecureChannelCredentials()));
    MetaResponse response = client.registerNameService("haokaizhao","127.0.0.1","80");
    std::cout << "request registerNameService to server, requestId is " << response.resp().request_id() << " code is " << response.resp().code() << " msg is " << response.resp().msg() << std::endl;
    MetaQueryResponse queryResponse = client.queryNameService("haokaizhao");
    std::cout << "request registerNameService to server, requestId is " << queryResponse.resp().request_id() << " code is " << queryResponse.resp().code() << " msg is " << queryResponse.resp().msg()
    << " ip is " << queryResponse.meta().ip() << " port is " << queryResponse.meta().port() << " service is " << queryResponse.meta().nameservice() << std::endl;

    std::cout << "test complete!" << std::endl;
}
