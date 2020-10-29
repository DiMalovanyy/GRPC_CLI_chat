#pragma once
#include <grpc++/grpc++.h>
#include <string>
#include <memory>
#include <iostream>
#include <optional>
#include <vector>


#include "utility.h"
#include "chat.grpc.pb.h"




class UsersDataBase {
    
public:
    UsersDataBase(std::shared_ptr<grpc::Channel> chanel) : stub_(Grpc::DataBaseService::NewStub(chanel)) {}
    
    
    void AddUser(const User& user) {
        Grpc::User grpc_user = user.GetGrpcRepresentation();
        Grpc::AddUserResponse* server_response = new Grpc::AddUserResponse;

        grpc::ClientContext context;
        grpc::Status status = stub_ -> AddUser(&context, grpc_user, server_response);
        //TODO: Error Handler
        delete server_response;
    }

    bool IsExist(const User& user) {
        Grpc::Login user_login;
        user_login.set_login(user.login);
        Grpc::IsExistResponse* bool_response = new Grpc::IsExistResponse;

        grpc::ClientContext context;
        grpc::Status status = stub_ -> IsExist(&context, user_login, bool_response);
        delete bool_response;
        return bool_response -> exist();
    }
    
    std::optional<std::string> GetPasswordByLogin(const std::string& login) {
        Grpc::Login user_login;
        user_login.set_login(login);
        Grpc::Password* response_password = new Grpc::Password;
        
        grpc::ClientContext context;
        grpc::Status status = stub_ -> GetPasswordByLogin(&context, user_login, response_password);
        if (status.ok()) {
            return response_password -> password();
            delete response_password;
        } else return std::nullopt;
    }
    
    User GetUserByLogin(const std::string& login) {
        Grpc::Login request;
        request.set_login(login);
        Grpc::GetUserResponse* get_user_response = new Grpc::GetUserResponse;
    
        grpc::ClientContext context;
        grpc::Status status = stub_ -> GetUserByLogin(&context, request, get_user_response);
        User user(get_user_response -> mutable_user());
        delete get_user_response;
        return user;
    }
    
    std::vector<std::string> GetAllLogins() {
        Grpc::VoidRequest void_request;
        grpc::ClientContext context;
        
        std::unique_ptr<grpc::ClientReader<Grpc::Login>> reader(stub_ -> GetAllLogins(&context, void_request));
        Grpc::Login readed_login;
        std::vector<std::string> all_logins;
        while( reader -> Read(&readed_login)) {
            all_logins.push_back(readed_login.login());
        }
        return all_logins;
    }
    
    
private:
    std::unique_ptr<Grpc::DataBaseService::Stub> stub_;
};
