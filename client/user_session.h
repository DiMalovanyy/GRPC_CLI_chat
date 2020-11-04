#pragma once



#include "utility.h"
#include "server_data_base_operator.h"

#include "menu.h"
#include "chat.h"


class Session {
public:
    
    explicit Session(const User& user) : logined_user(user) {}
    
    
    
    void ShowUsersLists(Menu::ChoiceMenu& users_list_menu, UsersDataBase& users_data_base) {
        const auto all_logins = users_data_base.GetAllLogins();
        for (auto& user : all_logins) {
            if (!(user == logined_user.login)) {
                users_list_menu.AddChoice(user);
            }
        }
        int user_login = users_list_menu.ShowAndGetChoice();
        User chat_mate = users_data_base.GetUserByLogin(all_logins[user_login]);
        Menu::ShowMenu menu("You write to " + chat_mate.login, {20,80});
        menu.Show();
        PrivateChat private_chat(logined_user, chat_mate);
    }
    
private:
    User logined_user;
};

