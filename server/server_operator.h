#pragma once
#include <grpc++/grpc++.h>



#include "chat.grpc.pb.h"
#include "CMAKE_CONSTANTS.h"
#include "data_base_service.h"


void RunServer() {
    std::string server_address(std::string(SERVER_IP));
    MongoDB::Collection collection("mongodb://" + std::string(DATA_BASE_IP), "main", "user_auth");
    DataBaseServiceImpl service(collection);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc_impl::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    std::cout << "MongoDB listening on " << std::string(DATA_BASE_IP) << std::endl;
    server->Wait();
}



