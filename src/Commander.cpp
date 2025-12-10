#include "utils.h"

#include "Commander.h"

Commander::Commander(int nx, int ny, int nw) {
    x = x, ny = ny, w = nw;
    commanding = false;
    win = newwin(1, w, y, x);
    reset();
}

Commander::~Commander() {
    delwin(win);
}

void Commander::setPos(int nx, int ny) {
    x = nx, y = ny;
    mvwin(win, y, x);
}

void Commander::reset() {
    resultT = 0;
    cmd = "";
    cx = 0;
}

void Commander::setResult(string res) {
    cmd = res;
    resultT = 5;    
}

void Commander::updateT() {
    resultT = max(resultT - 1, 0);
}

void Commander::setError(string error) {
    cmd = error;
    resultT = -1;
}

void Commander::refresh(string nleft, string ndet) {
    werase(win);
    wattrset(win, A_STANDOUT);
    
    string left;
    if (commanding || resultT > 0) {
        left = "CMD: " + cmd;
    }
    else {
        left = nleft;
    }
    
    //log(to_string(w));
    
    string middle (w - 3 - left.length() - ndet.length(), ' ');
    mvwprintw(win, 0, 0, "%s %s %s", left.c_str(), middle.c_str(), ndet.c_str());
    
    int curMode = A_STANDOUT;
    if (commanding) {
        curMode = A_BLINK;
    }
    
    mvwchgat(win, 0, cx + 5, 1, curMode, 0, NULL);
        
    wrefresh(win);
}

void Commander::takeInput(string input) {
    if (resultT == -1) {
        reset();
    }
    
    if (input == "KEY_LEFT") {
        cx = max(cx - 1, 0);
    }
    else if (input == "KEY_RIGHT") {
        cx = min(cx + 1, (int)cmd.size());
    }
    else if (input == "KEY_UP") {} //TODO: History
    else if (input == "KEY_DOWN") {} //TODO: History
    else if (input == "KEY_BACKSPACE") {
        string prev = cmd.substr(0, cx - 1);
        string after = cmd.substr(cx, cmd.length());
        cmd = prev + after;
        cx = max(cx - 1, 0);
    }
    else if (input.size() == 1) {
        cmd.insert(cx, input);
        cx += 1;
    }
}