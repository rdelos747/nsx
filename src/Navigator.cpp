#include "Navigator.h"

Navigator::Navigator() {
    win = newwin(1, 1, 1, 1);
    
    //readDir();
    //loga("STARTUP: NAVIAGTOR", cwd);
}

//test

Navigator::~Navigator() {
    log("deleting navigator");
    delwin(win);
}

void Navigator::setPos(int nx, int ny) {
    x = nx, y = ny;
    mvwin(win, y, x);
}

void Navigator::setSize(int nw, int nh) {
    w = nw, h = nh;
    wresize(win, h, w);
}

void Navigator::refresh() {
    werase(win);
    
    string d = last(split(cwd, '/'));
    wattrset(win, A_STANDOUT);
    mvwprintw(win, 0, 0, "%s", d.c_str());
    wattroff(win, A_STANDOUT);
    
     for (int i = 0; i < items.size(); i++) {
        string s  = last(split(items[i], '/'));
        mvwprintw(win, i + 2, 0, "%s", s.c_str());
    }
    
    mvwvline(win, 0, w - 1, 0, h);
    wrefresh(win);
}

void Navigator::takeInput(string input) {
}

void Navigator::setCWD(string dp) {
    //cwd = filesystem::current_path().string();
    loga("NAVIGATOR SET CWD", dp);
    cwd = dp;
    //filesystem::path p = dp;
    //filesystem::directoryPath
    items.clear();
    for (const auto & entry : filesystem::directory_iterator(cwd)) {
        ///log(last(split(entry.path(), '/')));
        
        string s = entry.path();
        items.push_back(s);
    }
}

void Navigator::setCWDFromFile(string fp) {
    filesystem::path p = fp;
    filesystem::path d = p.parent_path();
    
    setCWD(d.string());
}