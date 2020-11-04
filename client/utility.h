#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <boost/date_time.hpp>


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

struct Message {
    Message(const std::string& author_login, const std::string& value) : author_login_(author_login), value_(value) {
        date_time_ = boost::posix_time::second_clock::local_time();
    }
    
    Message(const Grpc::Message& grpc_message) :
    date_time_(
            boost::gregorian::date(
                                   grpc_message.send_date().year(),
                                   grpc_message.send_date().month(),
                                   grpc_message.send_date().day()
                                   ),
            boost::posix_time::time_duration(
                                             grpc_message.send_date().hours(),
                                             grpc_message.send_date().minutes(),
                                             0
                                             )
               )
    {
        author_login_ = grpc_message.author_login();
        value_ = grpc_message.value();
    }
    
    Grpc::Message GetGrpcRepresentation() const {
        Grpc::Message message;
        message.set_author_login(author_login_);
        message.set_value(value_);
        message.mutable_send_date() -> set_year(date_time_.date().year());
        message.mutable_send_date() -> set_month(date_time_.date().month());
        message.mutable_send_date() -> set_day(date_time_.date().day());
        message.mutable_send_date() -> set_hours(date_time_.time_of_day().hours());
        message.mutable_send_date() -> set_minutes(date_time_.time_of_day().minutes());
        return message;
    }
    
    std::string author_login_;
    std::string value_;
    boost::posix_time::ptime date_time_;
};


