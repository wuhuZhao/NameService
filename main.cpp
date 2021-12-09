#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#ifdef BAZEL_BUILD
#include "examples/protos/route_guide.grpc.pb.h"
#else
#include "NameService.grpc.pb.h"
#endif


class NameServiceImpl : public NameService::Service {
public:
    NameServiceImpl(std::map<std::string, Meta> map):queryMap(std::move(map)) {};
    grpc::Status registerService(::grpc::ServerContext *context, const ::MetaRequest *request, ::MetaResponse *response) override {
        // 首先构造一个请求，默认已经注册过NameService到名字服务中
        std::time_t t = std::time(0);
        std::cout << "pre time " << t << std::endl;
        Response resp{};
        setOK(resp);
        resp.set_msg("has registered!");
        // 根据Map的映射找不到NameService，注册到名字服务中
        if (queryMap.find(request->meta().nameservice()) == queryMap.end()) {
            queryMap.emplace(std::pair<std::string, Meta>(request->meta().nameservice(), request->meta()));
            resp.set_msg("registered!");
            std::cout << "registered!" << std::endl;
        }
        response->mutable_resp()->CopyFrom(resp);
        std::cout << "end time " << t << std::endl;
        return grpc::Status::OK;
    }

    grpc::Status queryService(::grpc::ServerContext *context, const ::MetaQueryRequest *request,
                              ::MetaQueryResponse *response) override {
        // 首先构造一个请求，找到对应的映射关系，返回对应的Meta数据
        std::time_t t = std::time(0);
        std::cout << "pre time " << t << std::endl;
        Response resp{};
        setOK(resp);
        resp.set_msg("this NameService, meta not find!");
        if (queryMap.find(request->nameservice()) != queryMap.end()) {
            resp.set_msg("this NameService, meta find!");
            response->mutable_meta()->CopyFrom(queryMap.find(request->nameservice())->second);
            std::cout << "find the meta!" << std::endl;
        }
        response->mutable_resp()->CopyFrom(resp);
        std::cout << "end time " << t << std::endl;
        return grpc::Status::OK;
    }


private:
    std::map<std::string, Meta> queryMap;
    void setOK(Response& resp) {
        std::time_t t = std::time(0);
        resp.set_code(200);
        resp.set_request_id(std::to_string(t));
    }


};


void RunServer() {
    std::string server_address("0.0.0.0:8081");
    NameServiceImpl service(std::map<std::string, Meta>{});

    grpc::EnableDefaultHealthCheckService(true);

    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally, assemble the server.
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "NameService Server listening on " << server_address << std::endl;

    // Wait for the server to shut down. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}
int main() {
    RunServer();
    return 0;
}
