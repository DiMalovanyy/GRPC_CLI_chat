#pragma once
#include <ncurses.h>
#include <vector>

#include "UImenu.h"
#include "utility.h"

namespace Menu {


class ChatMenu : public Menu {
public:
    
    ChatMenu(std::vector<Message>& messages, MenuSizes sizes, const std::string& title = "") : Menu(sizes, title), messages_(messages){}
    
    
    void StartChating() {
        WINDOW* menu_ = GetMenu();
        int highlight = 1; int choice = 0;
        clear(); cbreak(); keypad(menu_, TRUE);
        refresh();
        int y_input_position = GetSizes().height - 3;
        int x_input_position = 4;
//        int chat_height = 
        PrintMenu(menu_, 0, 0);
        echo();
        char c = mvwgetch(menu_, y_input_position, x_input_position);
        
//        switch(c) {
//            case KEY_UP: :break;
//            case KEY_DOWN: :break;
//            default:
//        }
            
        
        
        
        wgetch(menu_);
        refresh();
    }
    
private:
    
    void PrintMenu(WINDOW* menu_, int from, int to) {
        box(menu_, 0, 0);
        int y_position = 2;
        int x_position = 2;
        mvwprintw(menu_, GetSizes().height - 3, 2, std::string(">").data());
        
        
        
        int separator_y = GetSizes().height - 4;
        mvwhline(menu_, separator_y, 1, ACS_HLINE, GetSizes().width -2);
        wrefresh(menu_);
    }
    
    std::vector<Message>& messages_;
};

};
