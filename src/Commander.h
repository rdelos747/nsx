#ifndef COMMANDER_H
#define COMMANDER_H

#include <ncurses.h>
#include <vector>
#include <string>
#include <panel.h>

using namespace std;

struct CMDRes {
    bool pass;
    string res;
};

class Commander {
public:
    Commander(int nx, int ny, int nw);
    ~Commander();
    
    void reset();
    void start(string c);
    void runCommand();
    void runCommand(string c);
    void setSucc(string res);
    void setError(string error);
    void updateT();
    void refresh(string nleft, string nstat);
    void takeInput(string input);
    void setPos(int nx, int ny);
    void setSize(int nw);
    
    int x, y;
    int w;
    int cx;
    string cmd;
    string stat;
    bool commanding = false;
    int resultT;
private:
    WINDOW* win;
};

#endif
