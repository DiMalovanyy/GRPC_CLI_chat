#pragma once

#include <string>
#include <iostream>
#include <sstream>

#include "json_parser.h"
#include "chat.pb.h"

struct User {
    
    User (const std::string& json_user) {
        std::istringstream stream_user(json_user);
        auto node_map = Json::Load(stream_user).GetRoot().AsMap();
        login = node_map.at("login").AsString();
        password = node_map.at("password").AsString();
        email = node_map.at("mail").AsString();
    }
    
    User(const Grpc::User* user) {
        login = user -> login().login();
        password = user -> password().password();
        email = user -> mail().mail();
    }
    
    Grpc::User GetGrpcRepresentation() const {
        Grpc::User grpc_user;
        grpc_user.mutable_login() -> set_login(login);
        grpc_user.mutable_password() -> set_password(password);
        grpc_user.mutable_mail() -> set_mail(email);
        return grpc_user;
    }
    
    
    User (const std::string& login, const std::string& password, const std::string& email) : login(login), password(password), email(email) {}
    std::string login;
    std::string password;
    std::string email;
    const std::string ToJson() const ;
};

bool operator == (const User& rhs, const User& lhs) {
    return (rhs.login == lhs.login || rhs.email == lhs.email);
}

std::ostream& operator << (std::ostream& stream, const User& user) {
    stream << "{\"login\":\"" << user.login << "\",\"password\":\"" << user.password << "\",\"mail\":\"" << user.email << "\"}";
    return stream;
}

const std::string User::ToJson() const {
    std::ostringstream ss;
    ss << (*this);
    return ss.str();
}


