# 基于cpp的名字服务
	本文主要实现基于cpp的名字服务，作为cpp学习的练手
	技术栈主要为：grpc、c++17、cmake

# 初版
## 首先实现一个基础功能
1. 主要思路：能够通过调用grpc注册服务的功能注册ip+port和nameService的映射到cpp服务中，并且能够通过调用grpc的获取元信息服务指定服务名获取ip+port
2. proto文件定义：
```protobuf
syntax = "proto3";

message Response {
  string msg = 1;
  int32 code = 2;
  string request_id =3;
}

message NameServiceMeta {
  string nameService = 1;
  string ip = 2;
  string port = 3;
}

//将ip+port注册到名字服务
message MetaRequest {
  NameServiceMeta meta = 1;
}

message MetaResponse {
  Response resp = 1;
}

//查询名字服务对应的ip+port
message MetaQueryRequest {
  string nameService = 1;
}

message MetaQueryResponse {
  Response resp = 1;
  NameServiceMeta meta= 2;
}

service MetaService {
  rpc register(MetaRequest) returns (MetaResponse);
  rpc query(MetaQueryRequest) returns (MetaQueryResponse);
}
```
3. 根据上面的proto文件定义相关的cmake配置
```cmake
#获取proto文件 *文件需要自己指定  目前先不指定
get_filename_component(rg_proto "./proto/NameService.proto" ABSOLUTE)
get_filename_component(rg_proto_path "${rg_proto}" PATH)

#生成pb文件  route_guide.pb这些命名后面需要自己指定
set(rg_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/NameService.pb.h")
set(rg_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/NameService.pb.cc")
set(rg_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/NameService.grpc.pb.cc")
set(rg_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/NameService.grpc.pb.h")

#调用protoc bin
add_custom_command(
        OUTPUT "${rg_proto_srcs}" "${rg_proto_hdrs}" "${rg_grpc_srcs}" "${rg_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${rg_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${rg_proto}"
        DEPENDS "${rg_proto}")

#将pb.h文件导入到项目
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
```
4. 根据生成的pb文件进行impl继承，然后实现virtual方法，最后启动服务
```c++
void RunServer() {
    std::string server_address("0.0.0.0:8081");
    NameServiceImpl service;

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
```