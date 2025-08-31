#include "clip.h"
#include <filesystem>
#include <ncurses.h>

#include "globals.h"
#include "utils.h"

#include "Pad.h"

Pad::Pad(int nx, int ny, int nw, int nh) {
    x = nx, y = ny;
    w = nw, h = nh;
    scrx = 0, scry = 0;
    curc = new Cursor();
    padWinW = w - NUM_WIDTH - 1;
    texts = {""};
    touched = false;
    numWin = newwin(h, NUM_WIDTH, y, x);
    padWin = newwin(h, w - NUM_WIDTH, y, x + NUM_WIDTH);
    scrollok(padWin, true);
    
    string s;
    clip::get_text(s);
    log(s);
}

Pad::~Pad() {
    log("deleting pad");
    
    for(Cursor* c: cursors) {
        delete c;
    }
    
    delwin(numWin);
    delwin(padWin);
}

void Pad::loadFile(string relPath) {
    if (relPath == "") {
        fileName = "";
    }
    else {
        vector<string> pathParts = split(relPath, '/');
        fileName = pathParts[pathParts.size() - 1];
    }
    
    string cwd = filesystem::current_path().string();
    filePath = cwd + "/" + relPath;

    log("PAD: Opening file: " + filePath);
    string data = readFile(filePath);
    log(to_string(data.size()));
    if (data.size() > 0) {
        texts = split(data, '\n');
        
        string homeDir = getenv("HOME");
        saveFile(
            data,
            homeDir + "/Documents/nsx/backups/backup-" + getTime() + "-" + fileName
        );
    }
    else {
        texts = {""};
        touched = true;
    }
}

void Pad::refresh() {
    //log("START REFRESH");
    int txtLim = min(YMAX - 1, (int)texts.size());
    for (int j = 0; j < txtLim; j++) {
        int idx = j + scry;

        string num = to_string(idx + 1);
        string numPad (NUM_WIDTH - num.size(), ' ');
        mvwprintw(numWin, j, 0, "%s%s", num.c_str(), numPad.c_str());

        string text = texts[idx];
        string sub;
        if (scrx < text.size()) {
            sub = text.substr(scrx, padWinW);
        }

        string pad(max(0, (padWinW + 1) - (int)sub.size()), ' ');
        mvwaddnstr(padWin, j, 0, sub.append(pad).c_str(), padWinW);
    }

    for (int j = txtLim; j < YMAX - 1; j++) {
        int idx = j + scry;
        string numPad (NUM_WIDTH, ' ');
        mvwprintw(numWin, j, 0, "%s", numPad.c_str());

        string pad(padWinW + 1, ' ');
        mvwaddnstr(padWin, j, 0, pad.c_str(), padWinW);
    }        

    CurPts pos = curc->getBounds();
    
    for (int j = pos.sy; j <= pos.ey; j++) {
        int hLen = padWinW;
        int hStart = 0;
        
        if (pos.sy == pos.ey) {
            hStart = pos.sx - scrx;
            hLen = (pos.ex - pos.sx) + 1;    
        }
        else if (j == pos.sy) {
            hStart = pos.sx;
            hLen = (padWinW - pos.sx);
        }
        else if (j == pos.ey) {
            hStart = 0;
            hLen = pos.ex; 
        }

        mvwchgat(
            padWin,
            min(j - scry, YMAX - 2),
            hStart,
            hLen,
            WA_STANDOUT,
            0,
            NULL
        );
    }
    
    wrefresh(numWin);
    wrefresh(padWin);
}

void Pad::takeInput(string input) {
    int cx = curc->x;
    int cy = curc->y;
    bool drag = false;
    
    if (input == "KEY_SR") {
        input = "KEY_UP";
        drag = true;
    }
    else if (input == "KEY_SPREVIOUS") {
        input = "KEY_PPAGE";
        drag = true;
    }
    else if (input == "KEY_SF") {
        input = "KEY_DOWN";
        drag = true;
    }
    else if (input == "KEY_SNEXT") {
        input = "KEY_NPAGE";
        drag = true;
    }
    else if (input == "KEY_SLEFT") {
        input = "KEY_LEFT";
        drag = true;
    }
    else if (input == "kLFT6") {
        input = "kLFT5";
        drag = true;
    }
    else if (input == "KEY_SRIGHT") {
        input = "KEY_RIGHT";
        drag = true;
    }
    else if (input == "kRIT6") {
        input = "kRIT5";
        drag = true;
    }
    
    if (input == "KEY_UP") {
        cy = max(cy - 1, 0);   
    }
    else if (input == "KEY_PPAGE") {
        cy = max(cy - (YMAX - 1), 0);
    }
    else if (input == "KEY_DOWN") {
        cy = min(cy + 1, (int)texts.size() - 1);
    }
    else if (input == "KEY_NPAGE") {
        cy = min(cy + (YMAX - 1), (int)texts.size() - 1);
    }
    else if (input == "KEY_LEFT") {
        cx = max(cx - 1, 0);
    }
    else if (input == "kLFT5") {
        cx = max(cx - 5, 0);
    }
    else if (input == "KEY_RIGHT") {
        cx = cx + 1;
    }
    else if (input == "kRIT5") {
        cx = cx + 5;
    }    
    else if (input == "KEY_BACKSPACE") {
        touched = true;
        if (curc->drag) {
            CurPts pts = clearAtCursorBounds();
            cx = pts.sx;
            cy = pts.sy;
        }
        else {
            string after = texts[cy].substr(cx, texts[cy].length());
            if (cx > 0) {
                string prev = texts[cy].substr(0, cx);

                int n = 1;
                while ((prev.length() - n) % 4 != 0 && prev[prev.length() - n] == ' ') {
                    n += 1;
                }
                prev = prev.substr(0, (int)prev.length() - n);

                texts[cy] = prev + after;
                cx -= n;
            }
            else if (cy > 0) {
                cx = texts[cy - 1].length();
                texts[cy - 1] += after;
                texts.erase(texts.begin() + cy);
                cy -= 1;
            }
        }
    }
    else if (input == "^M") { // ENTER
        touched = true;
        CurPts pts = clearAtCursorBounds();
        cx = pts.sx;
        cy = pts.sy;
        
        string prev = texts[cy].substr(0, cx);
        int n = 0;
        while (texts[cy][n] == ' ' && n < texts[cy].length()) {
            n++;
        }
        string pad (n, ' ');
        string cut;
        if (cx < texts[cy].length()) {
            cut = texts[cy].substr(cx, texts[cy].length());
        }
        cut = pad + cut;

        texts[cy] = prev;
        texts.insert(texts.begin() + cy + 1, cut);
        cy += 1;
        cx = pad.length();
    }
    else if (input == "^I") { // TAB
        touched = true;
        texts[cy].insert(cx, string (4, ' '));
        cx += 4;
    }
    else if (input == "^S") {
        string joined = vecJoin(texts, '\n');
        saveFile(joined, filePath);
        touched = false;
    }
    else if (input == "^C") {
        copyClip();
    }
    else if (input == "^X") {
        copyClip();
        CurPts after = clearAtCursorBounds();
        cx = after.sx;
        cy = after.sy;
    }
    else if (input == "^V") {
        touched = true;
        CurPts after = pasteClip();
        cx = after.sx;
        cy = after.sy;
    }
    else if (input.size() == 1) {
        touched = true;
        CurPts pts = clearAtCursorBounds();
        cx = pts.sx;
        cy = pts.sy;
        texts[cy].insert(cx, input);
        cx += 1;
    }

    if (cx > (int)texts[cy].size()) {
        cx = (int)texts[cy].size();
    }
    curc->move(cx, cy, drag);

    // Handle scrolling
    if (scry > cy) {
        scry = cy;
    }

    if (scry < cy - (YMAX - 2)) {
        scry = cy - (YMAX - 2);
    }

    scry = min(scry, (int)texts.size() - YMAX + 1 );
    scry = max(scry, 0);


    if (cx < scrx) {
        scrx -= 1;
    }

    if (cx > scrx + padWinW- 1) {
        scrx += 1;
    }

    if ((int)texts[cy].size() < scrx) {
        scrx = max(0, (int)texts[cy].size() - padWinW);
    }
}

void Pad::putNCursor(int x, int y) {
    wmove(padWin, x, y);
}

void Pad::setPos(int nx, int ny) {
    x = nx, y = ny;
    mvwin(numWin, y, x);
    mvwin(padWin, y, x + NUM_WIDTH);
}

CurPts Pad::clearAtCursorBounds() {
    CurPts pts = curc->getBounds();
    string prev = texts[pts.sy].substr(0, pts.sx);
    string after = texts[pts.ey].substr(
        pts.ex, 
        texts[pts.ey].length() - pts.ex
    );
    
    for (int j = pts.ey; j > pts.sy; j--) {
        texts.erase(texts.begin() + j);
    }
    texts[pts.sy] = prev + after;
    
    return pts;
}

void Pad::copyClip() {
    CurPts pts = curc->getBounds();
    string copyStr = "";
    
    for (int j = pts.sy; j <= pts.ey; j++) {
        int sx = 0, ex = texts[j].size() - 1;
        if (j == pts.sy) {
            sx = pts.sx;
        }
        if (j == pts.ey) {
            ex = pts.ex;
        }
        //log("copying");
        //log(texts[j].substr(sx, (ex - sx) + 1));
        
        copyStr += texts[j].substr(sx, (ex - sx) + 1);
        if (j < pts.ey) {
            copyStr += '\n';
        }
    }
    log("=== copying ===");
    log(copyStr);
    clip::set_text(copyStr);
}

CurPts Pad::pasteClip() { 
    string pasteStr;
    clip::get_text(pasteStr);
    vector<string> pastes = split(pasteStr,'\n');
    CurPts pts = curc->getBounds();
    CurPts out = pts;
    
    texts[pts.sy].insert(pts.sx, pastes[0]);
    //curc->move(pts.sx + pastes[0].length(), pts.sy);
    out.sx += pastes[0].length();
    for (int j = 1; j < (int)pastes.size(); j++) {
        texts.insert(texts.begin() + pts.sy + j, pastes[j]);
        out.sx = pastes[j].length();
        out.sy += 1;
    }
    
    return out;
}