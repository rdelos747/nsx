#include <filesystem>
#include <ncurses.h>
#include "globals.h"
#include "utils.h"

#include "Pad.h"

Pad::Pad(int x, int y, int w, int h) {
    x = x, y = y;
    w = w, h = h;

    //cx = 0, cy = 0;
    sx = 0, sy = 0;
    
    curc = new Cursor();
    //cx = curc->x;
    //cy = curc->y;

    padWinW = w - NUM_WIDTH - 1;

    texts = {""};

    numWin = newwin(h, NUM_WIDTH, y, x);
    padWin = newwin(h, w - NUM_WIDTH, y, x + NUM_WIDTH);
    scrollok(padWin, true);
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
    string cwd = filesystem::current_path().string();
    filePath = cwd + "/" + relPath;

    log("PAD: Opening file: " + filePath);
    string data = readFile(filePath);
    log(to_string(data.size()));
    if (data.size() > 0) {
        texts = split(data, '\n');
        
        vector<string> pathParts = split(relPath, '/');
        string fileName = pathParts[pathParts.size() - 1];
        string homeDir = getenv("HOME");
        saveFile(
            data,
            homeDir + "/Documents/nsx/backups/backup-" + getTime() + "-" + fileName
        );
    }
    else {
        texts = {""};
    }
}

void Pad::refresh() {
    log("START REFRESH");
    int txtLim = min(YMAX - 1, (int)texts.size());
    for (int j = 0; j < txtLim; j++) {
        int idx = j + sy;

        string num = to_string(idx + 1);
        string numPad (NUM_WIDTH - num.size(), ' ');
        mvwprintw(numWin, j, 0, "%s%s", num.c_str(), numPad.c_str());

        string text = texts[idx];
        string sub;
        if (sx < text.size()) {
            sub = text.substr(sx, padWinW);
        }

        string pad(max(0, (padWinW + 1) - (int)sub.size()), ' ');
        mvwaddnstr(padWin, j, 0, sub.append(pad).c_str(), padWinW);
    }

    for (int j = txtLim; j < YMAX - 1; j++) {
        int idx = j + sy;
        string numPad (NUM_WIDTH, ' ');
        mvwprintw(numWin, j, 0, "%s", numPad.c_str());

        string pad(padWinW + 1, ' ');
        mvwaddnstr(padWin, j, 0, pad.c_str(), padWinW);
    }

    //wmove(padWin, min(cy - sy, YMAX - 2), min(cx - sx, padWinW - 1));
        
    
    int x1 = curc->ox;
    int x2 = curc->x;
    int y1 = curc->oy;
    int y2 = curc->y;
    
    if (x2 < x1 && y2 <= y1) {
        int temp = x2;
        x2 = x1;
        x1 = temp;
    }
    if (y2 < y1) {
        int temp = y2;
        y2 = y1;
        y1 = temp;
    }
    
    for (int j = y1; j <= y2; j++) {
        int hLen = padWinW;
        int hStart = 0;
        
        if (y1 == y2) {
            hStart = x1;
            hLen = (x2 - x1) + 1;    
        }
        else if (j == y1) {
            hStart = x1;
            hLen = (padWinW - x1);
        }
        else if (j == y2) {
            //hstart =
            hStart = 0;
            hLen = x2; 
        }

        mvwchgat(
            padWin,
            min(j - sy, YMAX - 2),
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
    int& cx = curc->x;
    int& cy = curc->y;
    bool drag = false;
    
    if (input == "KEY_SR") {
        input = "KEY_UP";
        drag = true;
    }
    else if (input == "KEY_SF") {
        input = "KEY_DOWN";
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
    else if (input == "KEY_DOWN") {
        cy = min(cy + 1, (int)texts.size() - 1);
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
    else if (input == "^M") { // ENTER
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
        texts[cy].insert(cx, string (4, ' '));
        cx += 4;
    }
    else if (input == "^S") {
        string joined = vecJoin(texts, '\n');
        saveFile(joined, filePath);
    }
    else if (input.size() == 1) {
        texts[cy].insert(cx, input);
        cx += 1;
    }

    if (cx > (int)texts[cy].size()) {
        cx = (int)texts[cy].size();
    }

    // Handle scrolling
    if (cy < sy) {
        sy -= 1;
    }

    if (cy > sy + YMAX - 2) {
        sy += 1;
    }

    sy = min(sy, (int)texts.size() - YMAX + 1 );
    sy = max(sy, 0);


    if (cx < sx) {
        sx -= 1;
    }

    if (cx > sx + padWinW- 1) {
        sx += 1;
    }

    if ((int)texts[cy].size() < sx) {
        sx = max(0, (int)texts[cy].size() - padWinW);
    }
    
    if (!drag) {
        curc->ox = cx;
        curc->oy = cy;
    }
    else {
        /*
        if (curc->x < curc->ox) {
            int t = curc->ox;
            curc->ox = curc->x;
            curc->x = t;
        }
        
        if (curc->y < curc->oy) {
            int t = curc->y;
            curc->y = curc->oy;
            curc->oy = t;
        }
        */
    }
}

void Pad::putCursor(int x, int y) {
    wmove(padWin, x, y);
    // todo: update cursor cx,cy,sx,sy vars
}

void Pad::setPos(int x, int y) {
    mvwin(numWin, y, x);
    mvwin(padWin, y, x + NUM_WIDTH);
}