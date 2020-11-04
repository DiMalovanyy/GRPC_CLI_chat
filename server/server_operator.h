#pragma once
#include <grpc++/grpc++.h>



#include "chat.grpc.pb.h"
#include "CMAKE_CONSTANTS.h"
#include "data_base_service.h"
#include "chat_service.h"


void RunServer() {
    
    mongocxx::instance inst{};
    std::string server_address(std::string(SERVER_IP));
    MongoDB::Collection useres_collection("mongodb://" + std::string(DATA_BASE_IP), "main", "user_auth");
    MongoDB::Collection chat_collection("mongodb://" + std::string(DATA_BASE_IP), "main", "chat");
    DataBaseServiceImpl users_service(useres_collection);
    ChatServiceImpl chat_service(chat_collection);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&users_service);
    builder.RegisterService(&chat_service);
    std::unique_ptr<grpc_impl::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    std::cout << "MongoDB listening on " << std::string(DATA_BASE_IP) << std::endl;
    server->Wait();

}



