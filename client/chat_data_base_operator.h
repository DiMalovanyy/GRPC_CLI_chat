#pragma once
#include <grpc++/grpc++.h>
#include <utility>
#include <string>
#include <memory>
#include <iostream>
#include <optional>
#include <vector>


#include "utility.h"
#include "chat.pb.h"
#include "chat.grpc.pb.h"


class ChatDataase {
public:
    ChatDataase(std::shared_ptr<grpc::Channel> chanel) : stub_(Grpc::ChatService::NewStub(chanel)) {}
    
    
    std::pair<std::vector<Message>, User> GetFullChat(const User& sender, const User& reciver) {
        std::vector<Message> messages;
        
        Grpc::PrivateChatUsers users_request;
        users_request.mutable_sender() -> CopyFrom(sender.GetGrpcRepresentation());
        users_request.mutable_reciver() -> CopyFrom(reciver.GetGrpcRepresentation());

        Grpc::FullChat full_chat;

        grpc::ClientContext context;
        grpc::Status status = stub_ -> GetFullChat(&context, users_request, &full_chat);
        
        for (size_t i = 0; i < full_chat.messages_size(); ++i) {
            Grpc::Message* message = full_chat.mutable_messages(i);
            messages.emplace_back(*message);
        }
        
        return std::make_pair(messages, sender);
    }
    
    bool SendMessage(const User& sender, const User& reciver, const Message& message) {
        Grpc::SendMessageRequest send_message_request;
        send_message_request.mutable_users() -> mutable_sender() -> CopyFrom(sender.GetGrpcRepresentation());
        send_message_request.mutable_users() -> mutable_reciver() -> CopyFrom(reciver.GetGrpcRepresentation());
        send_message_request.mutable_text() -> CopyFrom(message.GetGrpcRepresentation());
        
        Grpc::IsOkResponse response;
        grpc::ClientContext context;
        grpc::Status status = stub_ -> SendMessage(&context, send_message_request, &response);
        
        return response.ok();
    }
    
    
    
private:
    std::unique_ptr<Grpc::ChatService::Stub> stub_;
    
};
