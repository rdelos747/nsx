#include <ncurses.h>
#include <vector>
#include <string>
#include <panel.h>

using namespace std;

class Commander {
public:
    Commander(int nx, int ny, int nw);
    ~Commander();
    
    void reset();
    void setResult(string res);
    void setError(string error);
    void updateT();
    void refresh(string nleft, string nstat);
    void takeInput(string input);
    void setPos(int nx, int ny);
    
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