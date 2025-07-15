//#include <iostream>
#include <cstdio>
//#include <filesystem>
#include <fstream>
#include <ncurses.h>
#include <panel.h>

#define ctrl(c) (c & 0x1f)

int XMAX, YMAX;
int CX = 0;
int CY = 0;
WINDOW* MEN_WIN;
WINDOW* DIR_WIN;
WINDOW* TXT_WIN;

bool MEN_BOT = true;
int DIR_WIDTH = 10;

void updateMenu() {
    wattrset(MEN_WIN, A_STANDOUT | A_UNDERLINE);
    //const char* navString = "NAV BAR NAV BAR";
    std::string menuString (XMAX - 2, ' ');
    mvwprintw(MEN_WIN, 0, 0, "%s%d", menuString.c_str(), CY);
    wrefresh(MEN_WIN);
}

void updatePositions() {
    clear();
    refresh();

    if (MEN_BOT) {
        mvwin(MEN_WIN, YMAX - 1, 0);
        mvwin(DIR_WIN, 0, 0);
        mvwin(TXT_WIN, 0, DIR_WIDTH);
    }
    else {
        mvwin(MEN_WIN, 0, 0);
        mvwin(DIR_WIN, 1, 0);
        mvwin(TXT_WIN, 1, DIR_WIDTH);
    }

    //update_panels();
    mvwvline(DIR_WIN, 0, 9, 0, YMAX - 1);
    wrefresh(MEN_WIN);
    wrefresh(DIR_WIN);
    wrefresh(TXT_WIN);
    //refresh();
}

std::string readFile(std::string path) {
    //std::ifstream ifs(path, std::ios::in | std::ios::binary | std::ios::ate);
    std::size_t readSize = std::size_t(4096);
    std::ifstream stream = std::ifstream(path);

    std::string out;
    std::string buffer = std::string(readSize, '\0');
    while(stream.read(&buffer[0], readSize)) {
        out.append(buffer, 0, stream.gcount());
    }
    out.append(buffer, 0, stream.gcount());
    return out;
}

int main(int argc, char** ) {
    initscr();
    keypad(stdscr, true);
    nonl();
    noecho();
    raw();
    //cbreak();
    getmaxyx(stdscr, YMAX, XMAX);

    MEN_WIN = newwin(1, XMAX, 0, 0);
    DIR_WIN = newwin(YMAX - 1, DIR_WIDTH, 1, 0);
    TXT_WIN = newwin(YMAX - 1, XMAX - DIR_WIDTH, 1, DIR_WIDTH + 1);

    std::string s = readFile("./test.txt");

    wprintw(TXT_WIN, "%s", s.c_str());
    wmove(TXT_WIN, 0, 0);

    updateMenu();
    updatePositions();

    bool running = true;

    while(running) {
        int ch = getch();
        switch(ch) {
            case ctrl('q'):
                running = false;
                break;
            case ctrl('m'):
                //running = false;
                MEN_BOT = !MEN_BOT;
                updatePositions();
                break;
            case KEY_UP:
                CY = std::max(CY - 1, 0);
                break;
            case KEY_DOWN:
                CY = std::min(CY + 1, 99);
                break;
            case KEY_LEFT:
                CX = std::max(CX - 1, 0);
                break;
            case KEY_RIGHT:
                CX = std::min(CX + 1, 30);
                break;
            default:
                break;
        }

        updateMenu();
        wmove(TXT_WIN, CY, CX);
        wrefresh(TXT_WIN);
        //refresh();
    }

    endwin();
    return 0;
}
