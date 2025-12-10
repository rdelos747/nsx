#ifndef CURSOR_H
#define CURSOR_H

using namespace std;

struct CurPts {
    int sx;
    int sy;
    int ex;
    int ey;
};

class Cursor {
public:
    Cursor();
    ~Cursor();
    void move(int nx, int ny, bool ndrag = false);
    CurPts getBounds();
    
    int x, y;
    int ox, oy; //origin x, y
    bool drag;
};

#endif
