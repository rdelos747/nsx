#include "utils.h"

#include "Commander.h"
#include "NSX.h"

Commander::Commander(int nx, int ny, int nw) {
    x = x, ny = ny, w = nw;
    commanding = false;
    win = newwin(1, w, y, x);
    reset();
}

Commander::~Commander() {
    log("deleting commander");
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
        NSX.CURP->save();
        //setSucc("SAVED " + NSX.CURP->fileName);
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
    
    setError("I don't recognize this command :(");
}