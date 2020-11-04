#pragma once
#include <unordered_map>
#include <string>
#include <exception>
#include <vector>
#include <unordered_map>
#include <exception>



#include "CMAKE_CONSTANTS.h"
#include "UImenu.h"
#ifdef OK
#undef OK
#endif
#include "server_data_base_operator.h"
#include "utility.h"
#include "user_session.h"


class Operator {

public:


    void Operate() {
        UsersDataBase data_base(grpc::CreateChannel(std::string(SERVER_IP), grpc::InsecureChannelCredentials()));

        std::vector<std::string> main_menu_items = {"Login", "Register"};
        std::vector<std::string> login_menu_items = {"Login", "Password"};
        std::vector<std::string> register_menu_items = {"Email", "Login", "Password", "Repeat Password"};

        Menu::MenuSizes menu_sizes = {20,80};

        while(true) {
            Menu::ChoiceMenu* main_menu = new Menu::ChoiceMenu(main_menu_items, menu_sizes, "Login or Register");
            int choice = main_menu -> ShowAndGetChoice();
            if (choice == 1) {
                Menu::InputMenu* login_menu = new Menu::InputMenu(login_menu_items, menu_sizes, "Press Enter to login");
                auto resulting_login_items_map = login_menu -> GetInput();
                if (Login(resulting_login_items_map["Login"], resulting_login_items_map["Password"], data_base)) {
                    Menu::ShowMenu succesfull_login_menu("You succesfully Log In", menu_sizes);
                    succesfull_login_menu.Show();
                    Session session(data_base.GetUserByLogin(resulting_login_items_map["Login"]));
                    Menu::ChoiceMenu* users_list = new Menu::ChoiceMenu({}, menu_sizes, "Choose user to write him");
                    session.ShowUsersLists(*users_list, data_base);
                    delete users_list;
                    break;
                } else {
                    Menu::ShowMenu error_login_menu("Incorrect password or user not exist", menu_sizes);
                    error_login_menu.Show();
                }
                
                delete login_menu;
            } else if (choice == 2) {
                Menu::InputMenu* register_menu= new Menu::InputMenu(register_menu_items, menu_sizes, "Press Enter to register");
                auto resulting_items_map = register_menu -> GetInput();
                User new_user = User( resulting_items_map["Login"],
                                     resulting_items_map["Password"],
                                     resulting_items_map["Email"]);
                if (data_base.IsExist(new_user)) {
                    std::string error_existed_user("The user is already register, try to log in");
                    Menu::ShowMenu existing_user_menu(error_existed_user, menu_sizes);
                    existing_user_menu.Show();
                } else {
                    data_base.AddUser(new_user);
                }
                
                delete register_menu;
            }
            
            delete main_menu;
            
        }

    }
private:
    bool Login(const std::string& login, const std::string& password, UsersDataBase& data_base) {
        std::optional<std::string> password_result;
        try {
        password_result = data_base.GetPasswordByLogin(login);
        } catch (...){
            Menu::ShowMenu existing_user_menu("ERRRRRROOOOORRR", {20,80});
        }
        if (!password_result) {
            return false;
//            throw std::logic_error("User with login : " + login + " does not exist");
        } else if (password_result != password) {
            return false;
//            throw  std::logic_error("The password is incorrect");
        } else {
            return true;
        }
        return false;
    }

};
