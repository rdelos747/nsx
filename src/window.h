#include <ncurses.h>
#include <panel.h>

class Window {
public:
    Window(int x, int y, int width, int height);
    ~Window();

    void refresh();
    void handleInput();

private:
    WINDOW* win;
    int width;
    int height;
    int x;
    int y;
};
