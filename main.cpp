//#include <iostream>
#include <cstdio>
//#include <filesystem>
#include <fstream>
#include <ncurses.h>
#include <panel.h>
#include <sstream>
#include <vector>

#define ctrl(c) (c & 0x1f)

int XMAX, YMAX;
int CX = 0, CY = 0, SX = 0, SY = 0;

WINDOW* MEN_WIN;
WINDOW* DIR_WIN;
WINDOW* TXT_WIN;
WINDOW* NUM_WIN;

bool MEN_BOT = true;
bool SHOW_NUM = true;
int DIR_WIDTH = 10;
int NUM_WIDTH = 4;

void updateMenu() {
    werase(MEN_WIN);
    wattrset(MEN_WIN, A_STANDOUT | A_UNDERLINE);
    //const char* navString = "NAV BAR NAV BAR";
    std::string menuString (XMAX - 10, ' ');
    mvwprintw(MEN_WIN, 0, 0, "%s%d %d %d %d", menuString.c_str(),SY, SX, CY + 1, CX + 1);
    wrefresh(MEN_WIN);
}

void updatePositions() {
    clear();
    refresh();
    if (MEN_BOT) {
        mvwin(MEN_WIN, YMAX - 1, 0);
        mvwin(DIR_WIN, 0, 0);
        mvwin(NUM_WIN, 0, DIR_WIDTH);
        mvwin(TXT_WIN, 0, DIR_WIDTH + NUM_WIDTH);
    }
    else {
        mvwin(MEN_WIN, 0, 0);
        mvwin(DIR_WIN, 1, 0);
        mvwin(NUM_WIN, 1, DIR_WIDTH);
        mvwin(TXT_WIN, 1, DIR_WIDTH + NUM_WIDTH);
    }

    //update_panels();
    mvwvline(DIR_WIN, 0, 9, 0, YMAX - 1);
    wrefresh(MEN_WIN);
    wrefresh(DIR_WIN);
    wrefresh(NUM_WIN);
    wrefresh(TXT_WIN);
    //refresh();
}

std::vector<std::string> split(std::string input, char delim) {
    std::vector<std::string> out;
    std::string line;
    std::stringstream stream(input);
    while(std::getline(stream, line, delim)) {
        out.push_back(line);
    }
    return out;
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
    //return split(out, '\n');
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
    NUM_WIN = newwin(YMAX - 1, NUM_WIDTH, 1, 0);
    TXT_WIN = newwin(YMAX - 1, XMAX - (NUM_WIDTH + DIR_WIDTH), 1, NUM_WIDTH + DIR_WIDTH + 1);

    scrollok(TXT_WIN, true);

    std::string data = readFile("./test.txt");
    std::vector<std::string> texts = split(data, '\n');

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
            case ctrl('m'): //this is the same as enter for some reason
                //running = false;
                MEN_BOT = !MEN_BOT;
                updatePositions();
                break;
            case KEY_UP:
                CY = std::max(CY - 1, 0);
                if (CY < SY) {
                    SY -= 1;
                }
                break;
            case KEY_DOWN:
                CY = std::min(CY + 1, (int)texts.size() - 1);
                if (CY > SY + YMAX - 2) {
                    SY += 1;
                }
                break;
            case KEY_LEFT:
                CX = std::max(CX - 1, 0);
                break;
            case KEY_RIGHT:
                CX = CX + 1;
                break;
            default:
                break;
        }

        if (CX > (int)texts[CY].size()) {
            CX = (int)texts[CY].size();
        }

        int txt_win_w = (XMAX - 1) - (DIR_WIDTH + NUM_WIDTH); // todo name this better

        for (int j = 0; j < YMAX - 1; j++) {
            int idx = j + SY;

            std::string num = std::to_string(idx + 1);
            std::string numPad (NUM_WIDTH - num.size(), ' ');
            mvwprintw(NUM_WIN, j, 0, "%s%s", num.c_str(), numPad.c_str());

            std::string text = texts[idx];
            std::string sub = text.substr(CX, 30); // todo: this errors out when CX > text.size() I think.
            std::string pad(std::max(0, txt_win_w - (int)text.size()), '.');
            mvwaddnstr(TXT_WIN, j, 0, text.append(pad).c_str(), txt_win_w);
        }

        updateMenu();
        wmove(TXT_WIN, std::min(CY - SY, YMAX - 2), std::min(CX, XMAX));
        wrefresh(NUM_WIN);
        wrefresh(TXT_WIN);
        //refresh();
    }

    endwin();
    return 0;
}
