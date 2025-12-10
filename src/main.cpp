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
#include "log.h"
#include "utils.h"

#include "Commander.h"
#include "Pad.h"

#define ctrl(c) (c & 0x1f)

using namespace std;

struct CMDRes {
    bool pass;
    string res;
};


int XMAX, YMAX;

//WINDOW* MEN_WIN;
WINDOW* DIR_WIN;

Commander* COMMANDER;
vector<Pad*> PADS;
Pad* CURP;

bool CMD_BOT = true;
bool SHOW_NUM = true;
int DIR_WIDTH = 10;
int NUM_WIDTH = 4;
int NEXT_LORE_IDX = 0;

string LAST_INPUT;
string HOME_DIR = getenv("HOME");

//bool CMD_MODE = false;
//string CMD_NAME;

//string CMD_VAL;

void updateLayout() {
    clear();
    refresh();
    if (CMD_BOT) {
        //mvwin(, YMAX - 1, 0);
        COMMANDER->setPos(0, YMAX - 1);
        mvwin(DIR_WIN, 0, 0);
        for (Pad* p: PADS) {
            // todo: this will place pads ontop of
            // eachother. need to place them next
            // to eachother if split view
            p->setPos(DIR_WIDTH, 0);
        }
    }
    else {
        //mvwin(MEN_WIN, 0, 0);
        COMMANDER->setPos(0, 0);
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
    //wrefresh(MEN_WIN);
    wrefresh(DIR_WIN);
    //wrefresh(NUM_WIN);
    //wrefresh(TXT_WIN);
    //refresh();
}

CMDRes runCommand(string cmd) {
    vector<string> cmds = split(cmd, ' ');
    loga("got command", cmd);
    
    CMDRes res;
    //res.pass = true;
    //res.res = "Saved";
    res.pass = false;
    res.res = "Error";
    
    return res;
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

    string cwd = filesystem::current_path().string();
    log("STARTUP " + getTime() + ": CWD: " + cwd);

    string relPath = "";
    log(to_string(argc));
    if (argc == 1) {
        log("STARTUP: No filename provided, using blank file.");
    }
    else if (argc == 2) {
        relPath = argv[1];
    }

    initscr();
    keypad(stdscr, true);
    nonl();
    noecho();
    raw();
    set_escdelay(1);
    //cbreak();
    getmaxyx(stdscr, YMAX, XMAX);
    
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    mouseinterval(0);
    printf("\033[?1003h\n");
        
    //MEN_WIN = newwin(1, XMAX, 0, 0);
    DIR_WIN = newwin(YMAX - 1, DIR_WIDTH, 1, 0);
    //NUM_WIN = newwin(YMAX - 1, NUM_WIDTH, 1, 0);
    //TXT_WIN = newwin(YMAX - 1, XMAX - (NUM_WIDTH + DIR_WIDTH), 1, NUM_WIDTH + DIR_WIDTH + 1);

    //scrollok(TXT_WIN, true);

    //wmove(TXT_WIN, 0, 0);


    
    COMMANDER = new Commander(0, 0, XMAX - 1);
    
    CURP = new Pad(DIR_WIDTH, 1, XMAX - DIR_WIDTH, YMAX - 1);
    CURP->loadFile(relPath);
    CURP->putNCursor(0, 0);
    PADS.push_back(CURP);


    log("STARTUP: COMPLETE");

    //updateMenu();
    updateLayout();

    bool running = true;

    while(running) {
        //int txt_win_w = (XMAX - 1) - (DIR_WIDTH + NUM_WIDTH); // todo name this better
        bool layoutChanged = false;
        int ch = getch();
        string input(keyname(ch));
        LAST_INPUT = input;

        if (input == "^Q") {
            running = false;
        }
        else if (input == "^N") {
            // I wanted this to be ^M but that is also KEY_ENTER for some reason
            CMD_BOT = !CMD_BOT;
            layoutChanged = true;
        }
        else if (input == "^[") {
            //CMD_MODE = ! CMD_MODE;
            COMMANDER->commanding = !COMMANDER->commanding;
            COMMANDER->reset();
        }

        if (layoutChanged) updateLayout();
        if (COMMANDER->commanding) {
            COMMANDER->takeInput(input);
            
            if (input == "^M") { // ENTER
                CMDRes res = runCommand(COMMANDER->cmd);
                if (res.pass) {
                    COMMANDER->commanding = false;
                    COMMANDER->setResult(res.res);
                }
                else {
                    COMMANDER->setError(res.res);
                }
            }
        }
        else {
            CURP->takeInput(input);
            COMMANDER->updateT();
        }
        
        //updateMenu();
        string fname = CURP->fileName;
        if (CURP->touched) {
            fname += "*";
        }
        string stat = vecJoin(
            {
                to_string(CURP->scrx),
                to_string(CURP->scry),
                to_string(CURP->curc->x),
                to_string(CURP->curc->y)
            },
            ' '
        );
        COMMANDER->refresh(fname, LAST_INPUT + " " + stat);
        CURP->refresh();
    }

    delete COMMANDER;
    for (const Pad* p: PADS) {
        delete p;
    }

    printf("\033[?1003l\n");
    endwin();
    return 0;
}