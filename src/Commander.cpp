#include <format>

#include "Commander.h"
#include "NSX.h"
#include "utils.h"

Commander::Commander() {
    commanding = false;
    win = newwin(1, 1, 1, 1);
    
    wbkgd(win, COLOR_PAIR(T_BK) | ' ');
}

Commander::~Commander() {
    log("deleting commander");
    delwin(win);
}

void Commander::setPos(int nx, int ny) {
    x = nx, y = ny;
    mvwin(win, y, x);
}

void Commander::setSize(int nw) {
    w = nw;
    wresize(win, 1, w);
}

void Commander::reset() {
    resultT = 0;
    cmd = "";
    cx = 0;
}

void Commander::start(string c) {
    reset();
    cmd = c + " ";
    cx = cmd.length();
    commanding = true;
}

void Commander::updateT() {
    resultT = max(resultT - 1, 0);
}

void Commander::setSucc(string res) {
    commanding = false;
    cmd = res;
    resultT = 5;    
}

void Commander::setError(string error) {
    commanding = true;
    cmd = error;
    resultT = -1;
}

void Commander::refresh(int tabi, int tabm, string nleft, string ndet) {
    werase(win);
    //wattrset(win, A_STANDOUT);
    //string tab = "[" + to_string(tabi) + "] ";
    string tab = format(
        "[{}/{}]",
        to_string(tabi),
        to_string(tabm)
    );
    
    string left = tab;
    if (commanding || resultT > 0) {
        left += "CMD: " + cmd;
    }
    else {
        left += nleft;
    }
    
    mvwprintw(win, 0, 0, "%s", left.c_str());
    mvwprintw(win, 0, w - ndet.length(), "%s", ndet.c_str());
    
    int curMode = A_STANDOUT;
    if (commanding) {
        curMode = A_BLINK;
    }
    
    mvwchgat(win, 0, cx + tab.length() + 5, 1, curMode, 0, NULL);
    
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

void Commander::runCommand(string c) {
    reset();
    cmd = c;
    runCommand();
}

void Commander::runCommand() {
    vector<string> cmds = split(cmd, ' ');
    loga("RUNNING COMMAND", cmd);
    
    if (cmds.size() == 0) {
        setSucc("No Command");
        return;
    }
    
    if (cmds[0] == "save") {
        if (cmds.size() == 2) {
            start("FNAME " + cmds[1] + " Are you sure you want to save? y/n");
        } else { 
            NSX.CURP->save();
        } 
        return;
    }
    
    if (cmds[0] == "quit") {
        NSX.tryQuit();
        return;
    }
    
    if (cmds[0] == "UNSAVED") {
        string r = cmds[cmds.size() - 1];
        loga("running save quit check, got", r);
        if (r == "y" || r == "yes") {
            NSX.forceQuit();
        }
        else {
            setSucc("QUIT CANCELLED");
        }
        return;
    }
    
    if (cmds[0] == "FNAME") {
        string r = cmds[cmds.size() - 1];
        loga("running new filename check, got", r);
        if (r == "y" || r == "yes") {
            //NSX.forceQuit();
            NSX.CURP->save(cmds[1]);
        }
        else {
            setSucc("SAVE CANCELLED");
        }
        return;
    }
    
    if (cmds[0] == "find") {
        if (cmds.size() < 2) {
            setSucc("NO SEARCH SPECIFIED :(");
            return;
        }
        
        CurPts pts = NSX.CURP->find(cmd.substr(5));
        if (pts.sx >= 0 && pts.sy >= 0) {
            NSX.CURP->curc->move(pts.sx, pts.sy, false);
        }
        return; 
    }
    
    if (cmds[0] == "findn") {
        if (NSX.CURP->curFind == "") {
            setSucc("NO SEARCH SPECIFIED :(");
            return;
        }
        
        CurPts pts = NSX.CURP->findn();
        if (pts.sx >= 0 && pts.sy >= 0) {
            NSX.CURP->curc->move(pts.sx, pts.sy, false);
        }
        return; 
    }
    
    if (cmds[0] == "open") {
        if (cmds.size() < 2) {
            setSucc("NO FILE SPECIFIED :(");
        }
        else if (cmds.size() > 2) {
            setSucc("TOO MANY ARGS :(");
        }
        else {
            NSX.openFile(cmds[1], 0);
        }
        return;
    }
    
    setError("I don't recognize this command :(");
}