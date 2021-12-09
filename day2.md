# 基于cpp的名字服务
	本文主要实现基于cpp的名字服务，作为cpp学习的练手
	技术栈主要为：grpc、c++17、cmake

# 初版
### day1实现了grpc的server端服务，我们今天实现grpc的client端的请求，验证day1的server端功能
1. 首先创建`NameServiceClient.cpp`,然后通过构造一个_stub的类去实现调用grpc（桩代码）
```c++
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
```
2. 通过指定grpc服务端的ip:port，然后发送grpc请求
```c++
    std::string  target_str = "127.0.0.1:8081";
    NameServiceClient client(grpc::CreateChannel(
            target_str, grpc::InsecureChannelCredentials()));
```
3. 验证功能
```bash
request registerNameService to server, requestId is 1639038152 code is 200 msg is has registered!
request registerNameService to server, requestId is 1639038152 code is 200 msg is this NameService, meta find! ip is 127.0.0.1 port is 80 service is haokaizhao
test complete!
```
4. 这个client端代码仅做测试，后续考虑实现一个Java版的springboot-starter来进行名字服务注册

### bug
request_id用时间戳标识，由于服务太快了 每个requestId都会重复，所以day3要优化一下