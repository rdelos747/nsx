#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <filesystem>
#include <ncurses.h>
#include <panel.h>
#include <string>
#include <vector>

#include "utils.h"

using namespace std;

class Navigator {
public:
    Navigator();
    ~Navigator();
    
    void refresh();
    void takeInput(string input);
    void setCWD(string dp);
    void setCWDFromFile(string fp);
    
    void setPos(int nx, int ny);
    void setSize(int nw, int nh);
    
    int x, y;
    int w, h;
    int cy;
    string cwd;
    vector<string> items;

private:
    WINDOW* win;
};

#endif