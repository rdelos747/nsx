#include "Cursor.h"

using namespace std;

Cursor::Cursor() {
    x = 0, y = 0;
    ox = 0, oy = 0;
    drag = false;
}

Cursor::~Cursor() {}

void Cursor::move(int nx, int ny, bool ndrag) {
    x = nx;
    y = ny;
    drag = ndrag;
    if (!drag) {
        ox = x;
        oy = y;   
    }
    //drag = ndrag;
}

CurPts Cursor::getBounds() {
    int x1 = ox;
    int x2 = x;
    int y1 = oy;
    int y2 = y;
    
    if (x < ox && y <= oy) {
        x1 = x;
        x2 = ox;
    }
    if (y < oy) {
        y1 = y;
        y2 = oy;
    }
    
    CurPts out;
    out.sx = x1;
    out.ex = x2;
    out.sy = y1;
    out.ey = y2;
    
    return out;
}