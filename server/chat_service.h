#pragma once
#include <grpc++/grpc++.h>
#include <utility>
#include <sstream>
#include <google/protobuf/util/json_util.h>
#include <iostream>
#include <unordered_set>


#include "json_parser.h"
#include "utility.h"
#include "chat.pb.h"
#include "chat.grpc.pb.h"
#include "MongoDB_Manager.h"


//TODO: Make class Users Pair

std::pair<std::string, std::string> users_from_grpc(const ::Grpc::PrivateChatUsers& users) {
    const std::string sender_login = users.sender().login().login();
    const std::string reciver_login = users.reciver().login().login();
    return sender_login > reciver_login ? std::make_pair(sender_login, reciver_login) : std::make_pair(reciver_login, sender_login);
}

class UsersPairHash {
public:
    long operator()(const std::pair<std::string, std::string>& users_pair) const {
        long first_user_hash = std::hash<std::string>{}(users_pair.first);
        long second_user_hash = std::hash<std::string>{}(users_pair.second);
        return first_user_hash*31 + second_user_hash * 1501;
    }
};

std::pair<std::string, std::string> GetUsersPairFromJson(const std::string& json_users) {
    std::istringstream json_users_stream(json_users);
    auto UsersArray = Json::Load(json_users_stream).GetRoot().AsMap().at("Users").AsArray();
    return std::make_pair(UsersArray[0].AsString(), UsersArray[1].AsString());
}


class ChatServiceImpl final : public Grpc::ChatService::Service {
public:
    
    ChatServiceImpl(MongoDB::Collection& chat_base) : chat_base_(chat_base) {
        std::vector<std::string> chat_users = chat_base_.FindAllJsonValuesByKey("Users");
        for (const auto& chat_user: chat_users) {
            existing_chats.insert(GetUsersPairFromJson(chat_user));
        }
    }
    
    ::grpc::Status SendMessage(::grpc::ServerContext *context, const ::Grpc::SendMessageRequest *request, ::Grpc::IsOkResponse *response) override {
        const Grpc::Message message = request -> text();
        const std::pair<std::string, std::string> users_pair = users_from_grpc( request -> users());
        if (existing_chats.find(users_pair) == existing_chats.end()) {
            CreateChat(users_pair);
        }
        std::string json_message;
        const auto status = google::protobuf::util::MessageToJsonString(message, &json_message);
        chat_base_.UpdateArrayByKeyValue("Users", users_pair, "Chat", json_message);
        return ::grpc::Status::OK;
    }
    
    ::grpc::Status GetFullChat(::grpc::ServerContext *context, const ::Grpc::PrivateChatUsers *request, ::Grpc::FullChat *response) override {
        const std::pair<std::string, std::string> users_pair = users_from_grpc(*request);
        if (existing_chats.find(users_pair) == existing_chats.end()) {
            CreateChat(users_pair);
        }
        std::istringstream document_json(bsoncxx::to_json(chat_base_.FindDocByKeyValue("Users", users_pair).value()));
        //TODO: Rewrite in GRPCFromJson
//        google::protobuf::util::JsonStringToMessage(document_json, response);
        auto json_message_array = Json::Load(document_json).GetRoot().AsMap().at("Chat").AsArray();
        
        for (const auto& message : json_message_array) {
            auto message_object = message.AsMap();
            Grpc::Message grpc_message;
            grpc_message.set_author_login(message_object.at("authorLogin").AsString());
            grpc_message.set_value(message_object.at("value").AsString());
            auto date_object = message_object.at("sendDate").AsMap();
            grpc_message.mutable_send_date() -> set_year(date_object.at("year").AsInt());
            grpc_message.mutable_send_date() -> set_month(date_object.at("month").AsInt());
            grpc_message.mutable_send_date() -> set_day(date_object.at("day").AsInt());
            grpc_message.mutable_send_date() -> set_hours(date_object.at("hours").AsInt());
            grpc_message.mutable_send_date() -> set_minutes(date_object.at("minutes").AsInt());
            response -> mutable_messages() -> Add(std::move(grpc_message));
        }
        return ::grpc::Status::OK;
    }
    
    
private:
    
    void CreateChat(const std::pair<std::string, std::string>& users_pair) {
        std::ostringstream json;
        json << "{\"Users\":[\"" << users_pair.first << "\",\"" << users_pair.second << "\"],\"Chat\":" << "[]}";
        chat_base_.Add(json.str());
        existing_chats.insert(users_pair);
    }
    
    std::unordered_set<std::pair<std::string, std::string>, UsersPairHash> existing_chats;
    MongoDB::Collection& chat_base_;
    
};


