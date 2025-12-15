#include "NSX.h"

using namespace std;

Nsx::Nsx() {
    HOME_DIR = getenv("HOME");
}

Nsx::~Nsx() {}

void Nsx::start(StartConfig c) {
    clearLog();
    log("======== NSX START ========");
    
    /*
    Configure ncurses
    */
    initscr();
    keypad(stdscr, true);
    nonl();
    noecho();
    raw();
    set_escdelay(1);
    getmaxyx(stdscr, YMAX, XMAX);
    
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    mouseinterval(0);
    printf("\033[?1003h\n");
    
    if (has_colors() == false) {
        log("NO COLOR!!!!!!!");
    }
    
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    
    loga("STARTUP: ncurses configured");
    
    /*
    Startup file
    */
    string cwd = filesystem::current_path().string();
    loga("STARTUP:", getTime(), ", CWD:" ,cwd);
    
    if (c.path == "") {
        log("STARTUP: No filename provided, using blank file.");
    }
    else {
        loga("STARTUP: Using file ", c.path);
    }
    
    /*
    Create ncurses windows
    */
    DIR_WIN = newwin(YMAX - 1, DIR_WIDTH, 1, 0);
    COMMANDER = new Commander(0, 0, XMAX - 1);
    //COMMANDER = new Commander();
    
    CURP = new Pad(DIR_WIDTH, 1, XMAX - DIR_WIDTH, YMAX - 1, c.yoffset);
    //CURP = new Pad(c.yoffset);
    CURP->loadFile(c.path);
    CURP->putNCursor(0, 0);
    PADS.push_back(CURP);
    
    log("STARTUP: COMPLETE");

    updateLayout();
}

void Nsx::finish() {
    log("===== Cleaning up NSX =====");
    delete COMMANDER;
    for (const Pad* p: PADS) {
        delete p;
    }

    printf("\033[?1003l\n");
    endwin();
}

void Nsx::run() {
    running = true;
    while(running) {
        bool layoutChanged = false;
        int ch = getch();
        string input(keyname(ch));
        LAST_INPUT = input;
        
        if (input == "KEY_RESIZE") {
            layoutChanged = true;
        }
        else if (input == "^Q") {
            //running = false;
            tryQuit();
        }
        else if (input == "^N") {
            // I wanted this to be ^M but that is also KEY_ENTER for some reason
            CMD_BOT = !CMD_BOT;
            layoutChanged = true;
        }
        else if (input == "^[") {
            COMMANDER->commanding = !COMMANDER->commanding;
            COMMANDER->reset();
        }
        
        if (layoutChanged) updateLayout();
        if (COMMANDER->commanding) {
            COMMANDER->takeInput(input);
            
            if (input == "^M") { // ENTER
                COMMANDER->runCommand();
            }
        }
        else {
            CURP->takeInput(input);
            COMMANDER->updateT();
        }
        
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
}

void Nsx::updateLayout() {
    log("UPDATING LAYOUT");
    //loga("prev size", to_string(XMAX), to_string(YMAX));
    getmaxyx(stdscr, YMAX, XMAX);
    //loga("now size", to_string(XMAX), to_string(YMAX));
    
    wresize(DIR_WIN, YMAX - 1, DIR_WIDTH);
    COMMANDER->setSize(XMAX - 1);
    
    clear();
    refresh();
    if (CMD_BOT) {
        COMMANDER->setPos(0, YMAX - 1);
        mvwin(DIR_WIN, 0, 0);
        for (Pad* p: PADS) {
            // todo: this will place pads ontop of
            // eachother. need to place them next
            // to eachother if split view
            p->setPos(DIR_WIDTH, 0);
            p->setSize(XMAX - DIR_WIDTH, YMAX - 1);
        }
    }
    else {
        COMMANDER->setPos(0, 0);
        mvwin(DIR_WIN, 1, 0);
        for (Pad* p: PADS) {
            // todo: this will place pads ontop of
            // eachother. need to place them next
            // to eachother if split view
            p->setPos(DIR_WIDTH, 1);
            p->setSize(XMAX - DIR_WIDTH, YMAX - 1);
        }
    }

    mvwvline(DIR_WIN, 0, 9, 0, YMAX - 1);
    wrefresh(DIR_WIN);
}

void Nsx::tryQuit() {
    for (int i = 0; i < PADS.size(); i++) {
        Pad* p = PADS[i];
        if (p->touched) {
            COMMANDER->start("UNSAVED " + p->fileName + ". Are you sure you want to quit? y/n");
            return;
        }
    }
    
    running = false;
}

void Nsx::forceQuit() {
    running = false;
}