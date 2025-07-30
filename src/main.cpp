/*
IDEAS:
- Try not to impliment class behaviors via inheritance, eg:
    WRONG:
    class Window:
        void update:
            ....
    class MyWindow extends Window:
        void update:
            // some override stuff
            ....

    class Window:
        bool scrollable;

        void update:
            if scrollable:
                // call scroll function
*/

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ncurses.h>
#include <panel.h>
#include <sstream>
#include <vector>

#include "utils.h"

#define ctrl(c) (c & 0x1f)

using namespace std;
namespace fs = std::filesystem;

int XMAX, YMAX;
int CX = 0, CY = 0, HX = 0, HY = 0, SX = 0, SY = 0;

WINDOW* MEN_WIN;
WINDOW* DIR_WIN;
WINDOW* TXT_WIN;
WINDOW* NUM_WIN;

bool MEN_BOT = true;
bool SHOW_NUM = true;
int DIR_WIDTH = 10;
int NUM_WIDTH = 4;
string LAST_INPUT;

bool CMD_MODE = false;
string CMD_NAME;
string CMD_VAL;

void clearLog() {
    ofstream logFile;
    logFile.open("log.txt", ios_base::out);
    logFile.close();
}

void log(string text) {
    ofstream logFile;
    logFile.open("log.txt", ios_base::app);
    logFile << text << '\n';
    logFile.close();
}

void updateMenu() {
    werase(MEN_WIN);
    wattrset(MEN_WIN, A_STANDOUT);
    string statStr = vecJoin(vector<int>{SY, SX, CY, CX}, ' ');

    string cmd;
    if (CMD_MODE) {
        cmd = vecJoin(vector<string>{"CMD:"}, ' ');
    }

    string menuString (XMAX - 2 - cmd.length() - statStr.length() - LAST_INPUT.length(), ' ');
    mvwprintw(MEN_WIN, 0, 0, "%s %s %s %s", cmd.c_str(), menuString.c_str(), LAST_INPUT.c_str(), statStr.c_str());
    wrefresh(MEN_WIN);
}

void updateLayout() {
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

void saveFile(string data, string path) {
    log("SAVING FILE " + path);
    ofstream out;
    out.open(path, ios_base::out);
    out << data;
    out.close();
}

string getTime() {
    time_t time = std::time(nullptr);
    tm ltm = *localtime(&time);
    ostringstream oss;
    oss << put_time(&ltm, "%Y-%m-%d-%H-%M-%S");

    return oss.str();
}


int main(int argc, char* argv[] ) {
    clearLog();
    log("===== start =====");


    if (!fs::is_directory("backups") || !fs::exists("backups")) {
        log("STARTUP: backups directory not found - creating.");
        fs::create_directory("backups");
    }

    vector<string> texts = {""};

    string cwd = fs::current_path().string();
    log("STARTUP: CWD: " + cwd);

    string fileName = "";
    if (argc == 1) {
        log("STARTUP: No filename provided, using blank file.");
    }
    else if (argc == 2) {
        fileName = argv[1];
        log("STARTUP: Opening file: " + fileName);

        string data = readFile(cwd + fileName);
        vector<string> texts = split(data, '\n');
        saveFile(data, "./backups/backup-" + getTime() + "-" + fileName);
    }


    log("STARTUP: COMPLETE");

    initscr();
    keypad(stdscr, true);
    nonl();
    noecho();
    raw();
    set_escdelay(1);
    //cbreak();
    getmaxyx(stdscr, YMAX, XMAX);

    MEN_WIN = newwin(1, XMAX, 0, 0);
    DIR_WIN = newwin(YMAX - 1, DIR_WIDTH, 1, 0);
    NUM_WIN = newwin(YMAX - 1, NUM_WIDTH, 1, 0);
    TXT_WIN = newwin(YMAX - 1, XMAX - (NUM_WIDTH + DIR_WIDTH), 1, NUM_WIDTH + DIR_WIDTH + 1);

    scrollok(TXT_WIN, true);

    wmove(TXT_WIN, 0, 0);

    updateMenu();
    updateLayout();

    bool running = true;

    while(running) {
        int txt_win_w = (XMAX - 1) - (DIR_WIDTH + NUM_WIDTH); // todo name this better
        bool layoutChanged = false;
        int ch = getch();
        string input(keyname(ch));
        LAST_INPUT = input;

        if (input == "^Q") {
            running = false;
            texts[0] += "YES";
        }
        else if (input == "^N") {
            // I wanted this to be ^M but that is also KEY_ENTER for some reason
            MEN_BOT = !MEN_BOT;
            layoutChanged = true;
        }
        else if (input == "^[") {
            CMD_MODE = ! CMD_MODE;
        }
        else if (input == "KEY_UP") {
            CY = max(CY - 1, 0);
        }
        else if (input == "KEY_DOWN") {
            CY = min(CY + 1, (int)texts.size() - 1);
        }
        else if (input == "KEY_LEFT") {
            CX = max(CX - 1, 0);
        }
        else if (input == "kLFT5") {
            CX = max(CX - 5, 0);
        }
        else if (input == "KEY_RIGHT") {
            CX = CX + 1;
        }
        else if (input == "kRIT5") {
            CX = CX + 5;
        }

        if (input == "KEY_BACKSPACE") {
            string after = texts[CY].substr(CX, texts[CY].length());
            if (CX > 0) {
                string prev = texts[CY].substr(0, CX);

                int n = 1;
                while ((prev.length() - n) % 4 != 0 && prev[prev.length() - n] == ' ') {
                    n += 1;
                }
                prev = prev.substr(0, (int)prev.length() - n);

                texts[CY] = prev + after;
                CX -= n;
            }
            else if (CY > 0) {
                CX = texts[CY - 1].length();
                texts[CY - 1] += after;
                texts.erase(texts.begin() + CY);
                CY -= 1;
            }
        }
        else if (input == "^M") { // ENTER
            string prev = texts[CY].substr(0, CX);
            int n = 0;
            while (texts[CY][n] == ' ' && n < texts[CY].length()) {
                n++;
            }
            string pad (n, ' ');
            string cut;
            if (CX < texts[CY].length()) {
                cut = texts[CY].substr(CX, texts[CY].length());
            }
            cut = pad + cut;

            texts[CY] = prev;
            texts.insert(texts.begin() + CY + 1, cut);
            CY += 1;
            CX = pad.length();
        }
        else if (input == "^I") { // TAB
            texts[CY].insert(CX, string (4, ' '));
            CX += 4;
        }
        else if (input == "^S") {
            string joined = vecJoin(texts, '\n');
            saveFile(joined, cwd + fileName);
        }
        else if (input.size() == 1) {
            texts[CY].insert(CX, input);
            CX += 1;
        }

        if (CX > (int)texts[CY].size()) {
            CX = (int)texts[CY].size();
        }

        // Handle scrolling
        if (CY < SY) {
            SY -= 1;
        }

        if (CY > SY + YMAX - 2) {
            SY += 1;
        }

        SY = min(SY, (int)texts.size() - YMAX + 1 );
        SY = max(SY, 0);


        if (CX < SX) {
            SX -= 1;
        }

        if (CX > SX + txt_win_w - 1) {
            SX += 1;
        }

        if ((int)texts[CY].size() < SX) {
            SX = max(0, (int)texts[CY].size() - txt_win_w);
        }

        if (layoutChanged) updateLayout();

        int txtLim = min(YMAX - 1, (int)texts.size());
        for (int j = 0; j < txtLim; j++) {
            int idx = j + SY;

            string num = to_string(idx + 1);
            string numPad (NUM_WIDTH - num.size(), ' ');
            mvwprintw(NUM_WIN, j, 0, "%s%s", num.c_str(), numPad.c_str());

            string text = texts[idx];
            string sub;
            if (SX < text.size()) {
                sub = text.substr(SX, txt_win_w);
            }

            string pad(max(0, (txt_win_w + 1) - (int)sub.size()), '.');
            mvwaddnstr(TXT_WIN, j, 0, sub.append(pad).c_str(), txt_win_w);
        }

        for (int j = txtLim; j < YMAX - 1; j++) {
            int idx = j + SY;
            string numPad (NUM_WIDTH, ' ');
            mvwprintw(NUM_WIN, j, 0, "%s", numPad.c_str());

            string pad(txt_win_w + 1, '.');
            mvwaddnstr(TXT_WIN, j, 0, pad.c_str(), txt_win_w);
        }

        updateMenu();
        wmove(TXT_WIN, min(CY - SY, YMAX - 2), min(CX - SX, txt_win_w - 1));
        wrefresh(NUM_WIN);
        wrefresh(TXT_WIN);
    }

    endwin();
    return 0;
}
