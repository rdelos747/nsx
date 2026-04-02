#include "Navigator.h"

Navigator::Navigator() {
    win = newwin(1, 1, 1, 1);
    
    cwd = filesystem::current_path().string();
    loga("STARTUP: NAVIAGTOR", cwd);
}

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
    mvwvline(win, 0, w - 1, 0, h);
    wrefresh(win);
}

void Navigator::takeInput(string input) {

}