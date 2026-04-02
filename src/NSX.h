#ifndef NSX_H
#define NSX_H

#include <filesystem>
#include <ncurses.h>
#include <panel.h>
#include <string>
#include <vector>

//#include "globals.h"
#include "log.h"
#include "utils.h"

#include "Commander.h"
#include "Navigator.h"
#include "Pad.h"

using namespace std;

enum T_MODE {
    T_NORM = 1,
    T_COMM,
    T_QUOT,
    T_SCHR,
    T_NUMB,
    T_BK
};

struct StartConfig {
    string path = "";
    int yoffset = 0;
};

class Nsx {
public:
    Nsx();
    ~Nsx();
    void start(StartConfig c);
    void finish();
    void updateLayout();
    void run();
    void tryQuit();
    void forceQuit();
    
    // global values
    int XMAX, YMAX;
    bool CMD_BOT = true;
    bool SHOW_NUM = true;
    int DIR_W = 10;
    int NUM_W = 6;
    
    // global helper values
    int NEXT_LORE_IDX;
    string LAST_INPUT;
    string HOME_DIR;
    
    // global members
    Commander* COMMANDER;
    vector<Pad*> PADS;
    Pad* CURP;
    //WINDOW* DIR_WIN;
    Navigator* NAV;

private:
    bool running;
};

extern Nsx NSX;

#endif