#pragma once
#include "utility.h"
#include "UIchat_menu.h"
#include "chat_data_base_operator.h"

class PrivateChat {
public:
    
    
    PrivateChat(const User& current_user, const User& chat_mate) : current_user_(current_user), chat_mate_(chat_mate) {}
    
    
private:
    void SendMessage(const std::string& message) {
        
    }
    
    User current_user_;
    User chat_mate_;
};
