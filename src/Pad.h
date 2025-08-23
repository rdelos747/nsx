#include <ncurses.h>
#include <panel.h>
#include <string>
#include <vector>

#include "Cursor.h"

using namespace std;

class Pad {
public:
    Pad(int x, int y, int w, int h);
    ~Pad();

    void refresh();
    void takeInput(string input);
    void loadFile(string relPath);
    void putCursor(int x, int y);
    void setPos(int x, int y);
    void copyToClipboard(bool isCut);

    int x, y;
    int w, h;
    //int& cx, cy;
    int sx, sy;
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