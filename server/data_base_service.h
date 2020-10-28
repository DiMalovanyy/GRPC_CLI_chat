#pragma once

#include <grpc++/grpc++.h>
#include <optional>
#include <string>
#include <utility>
#include "CMAKE_CONSTANTS.h"


#include "chat.grpc.pb.h"
#include "MongoDB_Manager.h"
#include "utility.h"


class DataBaseServiceImpl final : public Grpc::DataBaseService::Service {
public:
    DataBaseServiceImpl(MongoDB::Collection& collection) : collection_(collection) {}
private:
    ::grpc::Status AddUser(::grpc::ServerContext *context, const Grpc::User *request, Grpc::AddUserResponse *response) override {
        collection_.Add(User(request).ToJson());
        response -> set_error_message("");
        return ::grpc::Status::OK;
    }
    
    ::grpc::Status GetUserByLogin(::grpc::ServerContext *context, const ::Grpc::Login *request, ::Grpc::GetUserResponse *response) override {
        std::optional<std::string> maybe_doc = collection_.FindJsonByKeyValue("login", request -> login());
        if (maybe_doc) {
            User user (maybe_doc.value());
            response -> mutable_json_user() -> set_json(maybe_doc.value());
            response -> mutable_user() -> mutable_login() -> set_login(user.login);
            response -> mutable_user() -> mutable_password() -> set_password(user.password);
            response -> mutable_user() -> mutable_mail() -> set_mail(user.email);
            response -> set_error_message("");
        } else {
            response -> set_error_message("User does not exist");
        }
        return ::grpc::Status::OK;
    }
    
    ::grpc::Status GetAllLogins(::grpc::ServerContext *context, const ::Grpc::VoidRequest *request, ::grpc::ServerWriter< ::Grpc::Login> *writer) override {
        for (const auto& login : collection_.FindAllValuesByKey("login")) {
            ::Grpc::Login* grpc_login = new ::Grpc::Login;
            grpc_login -> set_login(login);
            writer -> Write(*grpc_login);
        }
    
        return ::grpc::Status::OK;
    }
    
    ::grpc::Status GetPasswordByLogin(::grpc::ServerContext *context, const ::Grpc::Login *request, ::Grpc::Password *response) override {
        std::string password = collection_.GetValueByKeyInDoc("password", collection_.FindDocByKeyValue("login", request -> login()).value());
        response -> set_password(std::move(password));
        return ::grpc::Status::OK;
    }

    MongoDB::Collection& collection_;
};

