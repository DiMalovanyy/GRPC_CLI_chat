#pragma once



#include "utility.h"
#include "server_data_base_operator.h"

#include "menu.h"


class Session {
public:
    
    explicit Session(const User& user) : logined_user(user) {}
    
    
    
    void ShowUsersLists(Menu::ChoiceMenu& users_list_menu, UsersDataBase& users_data_base) {
        for (auto& user : users_data_base.GetAllLogins()) {
            if (!(user == logined_user.login)) {
                users_list_menu.AddChoice(user);
            }
        }
        auto i = users_list_menu.ShowAndGetChoice();
    }
    
private:
    User logined_user;
};

