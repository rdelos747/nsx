#ifndef PAD_H
#define PAD_H

#include <ncurses.h>
#include <panel.h>
#include <string>
#include <vector>

#include "Cursor.h"
#include "Lore.h"

using namespace std;

struct ClearOp {
    CurPts pts;
    LoreNode* old;
    LoreNode* aft;
};

class Pad {
public:
    Pad(int nx, int ny, int nw, int nh, int yoff);
    ~Pad();

    void refresh();
    void takeInput(string input);
    void loadFile(string relPath);
    void putNCursor(int x, int y);
    void setPos(int nx, int ny);
    CurPts placeLore(LoreNode* old, LoreNode* cur);
    void save();
    CurPts find(string s);
    CurPts findn();

    int x, y;
    int w, h;
    //int& cx, cy;
    int scrx, scry;
    int mx, my;
    bool mouseDragging;
    int padWinW;
    Lore lore;
    vector<string> texts;
    string filePath;
    string fileName;
    vector<Cursor*> cursors;
    Cursor* curc;
    bool touched;
    string curFind;

private:
    void updateCurcAndScroll();
    ClearOp clearAtCursorBounds();
    void copyClip();
    ClearOp pasteClip();
    
    WINDOW* numWin;
    WINDOW* padWin;
};

#endif