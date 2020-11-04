#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "json_parser.h"


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


//struct Message {
//    Message();
//    Message(const User& author, const std::string& value) : author_(author), value_(value), send_date_(boost::gregorian::day_clock::local_day()) {}
//    User author_;
//    std::string value_;
//    boost::gregorian::date send_date_;
//};
//
//std::vector<Message> GetFullChatFromJson(const std::string& json_chat) {
//    std::vector<Message> chat;
//    std::istringstream stream_chat(json_chat);
//    auto node_array = Json::Load(stream_chat).GetRoot().AsArray();
//    for (const auto& node_array : node_array) {
//        auto array_el = node_array.AsMap();
//        Message message; 
//    }
//    return chat;
//}


//std::vector<Grpc::Message> GetFullChatToGrpc(const std::string& json_chat) {
//
//}
