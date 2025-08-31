#include <ncurses.h>
#include <panel.h>
#include <string>
#include <vector>

#include "Cursor.h"

using namespace std;

class Pad {
public:
    Pad(int nx, int ny, int nw, int nh);
    ~Pad();

    void refresh();
    void takeInput(string input);
    void loadFile(string relPath);
    void putNCursor(int x, int y);
    void setPos(int nx, int ny);
    CurPts clearAtCursorBounds();
    void copyClip();
    CurPts pasteClip();

    int x, y;
    int w, h;
    //int& cx, cy;
    int scrx, scry;
    int padWinW;
    vector<string> texts;
    string filePath;
    string fileName;
    vector<Cursor*> cursors;
    Cursor* curc;
    bool touched;

private:
    WINDOW* numWin;
    WINDOW* padWin;
};