#include <ncurses.h>
#include <panel.h>

/*
 * Debating if this is actually needed
 */

class Window {
public:
    Window(int x1, int y1, int w1, int h1);
    ~Window();

    virtual void refresh();
    virtual void takeInput();

private:
    int w;
    int h;
    int x;
    int y;
};
