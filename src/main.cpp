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

#include <filesystem>
#include <ncurses.h>
#include <panel.h>
#include <string>
#include <vector>

#include "globals.h"
#include "Pad.h"
#include "utils.h"

#define ctrl(c) (c & 0x1f)

using namespace std;

int XMAX, YMAX;

WINDOW* MEN_WIN;
WINDOW* DIR_WIN;
vector<Pad*> PADS;
Pad* CURP;

bool MEN_BOT = true;
bool SHOW_NUM = true;
int DIR_WIDTH = 10;
int NUM_WIDTH = 4;
string LAST_INPUT;

bool CMD_MODE = false;
string CMD_NAME;
string CMD_VAL;

void updateMenu() {
    werase(MEN_WIN);
    wattrset(MEN_WIN, A_STANDOUT);
    string statStr = vecJoin(
        vector<int>{CURP->sx, CURP->sy, CURP->curc->x, CURP->curc->y},
        ' '
    );

    string left;
    if (CMD_MODE) {
        left = vecJoin(vector<string>{"CMD:"}, ' ');
    }
    else {
        if (CURP->fileName == "") {
            left = "untitled";
        } else {
            left = CURP->fileName;
        }
        
        if (CURP->touched) {
            left = left + "*";
        }
    }
    

    string menuString (XMAX - 3 - left.length() - statStr.length() - LAST_INPUT.length(), ' ');
    mvwprintw(MEN_WIN, 0, 0, "%s %s %s %s", left.c_str(), menuString.c_str(), LAST_INPUT.c_str(), statStr.c_str());
    wrefresh(MEN_WIN);
}

void updateLayout() {
    clear();
    refresh();
    if (MEN_BOT) {
        mvwin(MEN_WIN, YMAX - 1, 0);
        mvwin(DIR_WIN, 0, 0);
        //mvwin(NUM_WIN, 0, DIR_WIDTH);
        //mvwin(TXT_WIN, 0, DIR_WIDTH + NUM_WIDTH);
        for (Pad* p: PADS) {
            // todo: this will place pads ontop of
            // eachother. need to place them next
            // to eachother if split view
            p->setPos(DIR_WIDTH, 0);
        }
    }
    else {
        mvwin(MEN_WIN, 0, 0);
        mvwin(DIR_WIN, 1, 0);
        //mvwin(NUM_WIN, 1, DIR_WIDTH);
        //mvwin(TXT_WIN, 1, DIR_WIDTH + NUM_WIDTH);
        for (Pad* p: PADS) {
            // todo: this will place pads ontop of
            // eachother. need to place them next
            // to eachother if split view
            p->setPos(DIR_WIDTH, 1);
        }
    }

    //update_panels();
    mvwvline(DIR_WIN, 0, 9, 0, YMAX - 1);
    wrefresh(MEN_WIN);
    wrefresh(DIR_WIN);
    //wrefresh(NUM_WIN);
    //wrefresh(TXT_WIN);
    //refresh();
}

int main(int argc, char* argv[] ) {
    clearLog();
    log("===== start =====");


    /*
    // todo: make this always in documents/nsx/
    if (!fs::is_directory("backups") || !fs::exists("backups")) {
        log("STARTUP: backups directory not found - creating.");
        fs::create_directory("backups");
    }
    */

    //vector<string> texts = {""};

    string cwd = filesystem::current_path().string();
    log("STARTUP " + getTime() + ": CWD: " + cwd);

    string relPath = "";
    log(to_string(argc));
    if (argc == 1) {
        log("STARTUP: No filename provided, using blank file.");
    }
    else if (argc == 2) {
        relPath = argv[1];
        /*
        vector<string> filePathParts = split(relFilePath, '/');
        string fileName = filePathParts[filePathParts.size() - 1];

        if (relFilePath.substr(0,2) == "./") {
            relFilePath = "/" + relFilePath;
        }

        log("STARTUP: Opening file: " + cwd + relFilePath);

        string data = readFile(cwd + relFilePath);

        log(to_string(data.size()));

        texts = split(data, '\n');
        string homeDir = getenv("HOME");
        saveFile(
            data,
            homeDir + "/Documents/nsx/backups/backup-" + getTime() + "-" + fileName
        );
        */
    }




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
    //NUM_WIN = newwin(YMAX - 1, NUM_WIDTH, 1, 0);
    //TXT_WIN = newwin(YMAX - 1, XMAX - (NUM_WIDTH + DIR_WIDTH), 1, NUM_WIDTH + DIR_WIDTH + 1);

    //scrollok(TXT_WIN, true);

    //wmove(TXT_WIN, 0, 0);


    CURP = new Pad(DIR_WIDTH, 1, XMAX - DIR_WIDTH, YMAX - 1);
    CURP->loadFile(relPath);
    CURP->putCursor(0, 0);
    PADS.push_back(CURP);


    log("STARTUP: COMPLETE");

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
            // texts[0] += "YES";
        }
        else if (input == "^N") {
            // I wanted this to be ^M but that is also KEY_ENTER for some reason
            MEN_BOT = !MEN_BOT;
            layoutChanged = true;
        }
        else if (input == "^[") {
            CMD_MODE = ! CMD_MODE;
        }

        if (layoutChanged) updateLayout();

        CURP->takeInput(input);

        updateMenu();

        CURP->refresh();
    }

    for (const Pad* p: PADS) {
        delete p;
    }

    endwin();
    return 0;
}