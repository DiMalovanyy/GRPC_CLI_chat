#pragma once
#include <ncurses.h>
#include <vector>
#include <utility>
#include <string>
#include <unordered_map>



namespace Menu {

struct MenuSizes {
    int height;
    int width;
};

//Интерфейс для Обьекта меню
class Menu {
public:
    Menu(MenuSizes sizes, const std::string& title) : title_(std::move(title)), sizes_(sizes) {
        initscr();
        curs_set(0);
        menu_ = newwin(sizes_.height, sizes_.width, (LINES-sizes_.height)/2, (COLS-sizes_.width)/2);
    }
    
    ~Menu() {
        wborder(menu_, ' ', ' ', ' ',' ',' ',' ',' ',' ');
        wrefresh(menu_);
        delwin(menu_);
        endwin();
    }
    
    WINDOW* GetMenu() { return menu_; }
    
    std::string GetTitle() const { return title_;}
    
    void PrintMsgAtMenuLine(std::string message, int line) {
        mvwprintw(menu_, line, 1, std::string(sizes_.width - 3,' ').data());
        mvwprintw(menu_, line, 2, message.data());
        wrefresh(menu_);
    }
    
    MenuSizes GetSizes() const {return  sizes_;}
  
private:
    
 
    WINDOW* CentralyzedMenu() {
        return newwin(sizes_.height, sizes_.width, (LINES-sizes_.height)/2, (COLS-sizes_.width)/2);
    }
    
    MenuSizes sizes_;
    WINDOW* menu_;
    std::string title_;
};


class ChoiceMenu : public Menu {
public:
    explicit ChoiceMenu(const std::vector<std::string>& menu_items, MenuSizes sizes, const std::string title = "") : Menu(sizes, title), menu_items_(menu_items){
        PrintMsgAtMenuLine(title, GetSizes().height - 2 );
    }
    
    
    void AddChoice(const std::string& choice) {
        menu_items_.push_back(choice);
    }
    
    int ShowAndGetChoice() {
        WINDOW* menu_ = GetMenu();
        int highlight = 0; int choice = 0; int c;
        clear(); noecho(); cbreak(); keypad(menu_, TRUE);
        refresh(); //1
        const int choices_amount = GetSizes().height - 8;
        int from = 0;
        int to = (choices_amount > ItemsCount()) ? ItemsCount() : choices_amount;
        PrintMenu(menu_, highlight, from, to);
        while(true) {
            c = wgetch(menu_);
            switch(c)  {
                case KEY_UP:
                    if(highlight == 0) {
                        highlight = ItemsCount() - 1;
                        to = ItemsCount();
                        from = ItemsCount() < choices_amount ? 0 : ItemsCount() - choices_amount;
                        break;
                    }
                    else if (highlight == from) {
                        --from; --to;
                    }
                    --highlight;
                    break;
                case KEY_DOWN:
                    if(highlight == ItemsCount() - 1) {
                        highlight = 0;
                        from = 0;
                        to = (choices_amount > ItemsCount()) ? ItemsCount() : choices_amount;
                        break;
                    }
                    else if (highlight == to - 1) {
                        ++from; ++to;
                    }
                    ++highlight;
                    break;
                case 10: choice = highlight + 1; break;
            }
            PrintMenu(menu_, highlight, from, to);
            if(choice != 0)    /* User did a choice come out of the infinite loop */
                break;
        }
        clrtoeol();
        refresh();
        return choice;
    }
    
    const std::string* GetItems() const {
        return menu_items_.data();
    }
    size_t ItemsCount() const{
        return menu_items_.size();
    }
    
private:
    
    void PrintMenu(WINDOW* menu_, int highlight, int from, int to) {
        int x, y, i;
        x = 2;
        y = 4;
        wclear(menu_);
        box(menu_, 0, 0);
        if (from != 0) {
            mvwaddch(menu_, y - 1 , x + 3, ACS_UARROW);
        }
        for(i = from; i < to; ++i) {
            if(highlight == i) {
                wattron(menu_, A_REVERSE);
                mvwprintw(menu_, y, x, "%s", menu_items_[i].data());
                wattroff(menu_, A_REVERSE);
            } else {
                mvwprintw(menu_, y, x, "%s", menu_items_[i].data());
            }
            ++y;
        }
        if (to != ItemsCount()) {
            mvwaddch(menu_, y , x + 3, ACS_DARROW);
        }
        wrefresh(menu_);
    }
    std::vector<std::string> menu_items_;
};



class InputMenu : public Menu {
public:
    
    explicit InputMenu(const std::vector<std::string>& input_fields, MenuSizes sizes, const std::string title = "") : Menu(sizes,title), input_fields_(input_fields){
        PrintMsgAtMenuLine(title, GetSizes().height - 2 );
    }
    
    const std::unordered_map<std::string, std::string> GetInput() {
        std::unordered_map<std::string, std::string> input_parameters;
        WINDOW* menu_ = GetMenu();
        int highlight = 1; int choice = 0; int c;
        clear(); noecho(); cbreak(); keypad(menu_, TRUE);
        refresh();
        PrintMenu(menu_, highlight);
        int input_x = MaxInputFieldSize() + 6;
        while(true) {
            echo();
            c = mvwgetch(menu_, 3 + highlight, 2 + input_x);
            switch(c)  {
                case KEY_UP: if(highlight == 1)
                                highlight = input_fields_.size();
                            else
                                --highlight;
                            break;
                case KEY_DOWN:
                            if(highlight == input_fields_.size())
                                highlight = 1;
                            else
                                ++highlight;
                            break;
                case 10: choice = highlight; break;
                case 127 : mvwprintw(menu_, 3+ highlight, input_x, std::string(15, ' ').data()); break;
                default:
                    std::string msg;
                    msg.push_back(char(c));
                    PrintAndGetMessage(menu_, msg, 3 + highlight, 3 + input_x);
                    wrefresh(menu_);
                    refresh();
                    input_parameters[input_fields_[highlight - 1]] = msg;
                    
                    if (input_parameters.size() == input_fields_.size()) {}
                    break;
            }
            PrintMenu(menu_, highlight);
            if(choice != 0)  {
                if (input_parameters.size() != input_fields_.size()) {
                    PrintMsgAtMenuLine("You should fill every field", 1);
                }  else if (input_parameters.find("Repeat Password") != input_parameters.end() &&
                           input_parameters["Password"] != input_parameters["Repeat Password"]) {
                    PrintMsgAtMenuLine("Passwords doesn't match ", 1);
                } else {
                    break;
                }
            }  /* User did a choice come out of the infinite loop */
            noecho();
        }
        clrtoeol();
        refresh();

        
        return input_parameters;
    }
    
    
private:
    
    void PrintMenu(WINDOW* menu_, int highlight) {
        int max_size = MaxInputFieldSize() + 4;
        int x = 2;
        int y = 4;
        box(menu_, 0, 0);
        std::string str;
        for (int i = 0; i < input_fields_.size(); ++i) {
            if (highlight == i + 1) {
                mvwprintw(menu_, y, x, "%s", (input_fields_[i] + std::string(max_size - input_fields_[i].size() - 1, ' ') + ">").data());
            } else {
                mvwprintw(menu_, y, x, (input_fields_[i] + std::string(max_size - input_fields_[i].size() + 1, ' ')).data());
            }
            ++y;
        }
        wrefresh(menu_);
    }
    
    void PrintAndGetMessage(WINDOW* menu_, std::string& message, int print_y, int print_x) {
        echo();
        mvwprintw(menu_, print_y, print_x, std::string(15, ' ').data());
        int i = 0;
        while(true) {
            int ch = mvwgetch(menu_,print_y, print_x + i);
            if (ch == 10) {
                break;
            } else if (ch == 8 || ch == 127) {
                if (message.size() > 0) {
                    message.pop_back();
                    --i;
                    mvwprintw(menu_, print_y, print_x + i, std::string(10, ' ').data());
                } else
                    mvwprintw(menu_, print_y, print_x - 2, std::string(10, ' ').data());
            } else {
                ++i;
                message.push_back(char(ch));
            }
        }
        noecho();
        wrefresh(menu_);
    }
    
    int MaxInputFieldSize() {
        int max = 0;
        for (const auto& field: input_fields_) {
            max  = (field.size() > max) ? field.size() : max;
        }
        return max;
    }
    
    std::vector<std::string> input_fields_;
};


class ShowMenu : public Menu {
public:
    ShowMenu(const std::string& text, MenuSizes sizes, const std::string& title = "") : Menu(sizes, title) {
        lines.push_back(text);
    }
    
    void Show() {
        WINDOW* menu_ = GetMenu();
        PrintMenu(menu_);
        getch();
    }
    
private:

    void Linear(const std::string& line ) {
        std::string current_line;
    }
    
    void PrintMenu(WINDOW* menu_, int highlighted = 0) {
        box(menu_, 0, 0);
        int y_position = 2;
        int x_position = 2;
        for (const auto& line : lines) {
            mvwprintw(menu_, y_position++, x_position, line.data());
        }
        wrefresh(menu_);
    }
    std::vector<std::string> lines;
};


}

