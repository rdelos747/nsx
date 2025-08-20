#include <ncurses.h>
#include <panel.h>
#include <string>
#include <vector>

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

    int x, y;
    int w, h;
    int cx, cy;
    int sx, sy;
    int padWinW;
    vector<string> texts;
    string filePath;

private:
    WINDOW* numWin;
    WINDOW* padWin;
};
