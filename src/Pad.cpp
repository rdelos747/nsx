#include <filesystem>
// test
#include "globals.h"
#include "utils.h"

#include "Pad.h"

Pad::Pad(int x, int y, int w, int h) {
    x = x, y = y;
    w = w, h = h;

    cx = 0, cy = 0;
    sx = 0, sy = 0;

    padWinW = w - NUM_WIDTH - 1;

    texts = {""};

    numWin = newwin(h, NUM_WIDTH, y, x);
    padWin = newwin(h, w - NUM_WIDTH, y, x + NUM_WIDTH);
    scrollok(padWin, true);
}

Pad::~Pad() {
    log("deleting pad");
    delwin(numWin);
    delwin(padWin);
}

void Pad::loadFile(string relPath) {
    vector<string> pathParts = split(relPath, '/');
    string fileName = pathParts[pathParts.size() - 1];
    string cwd = filesystem::current_path().string();

    filePath = cwd + relPath;
    if (relPath.substr(0,2) == "./") {
        filePath = cwd + "/" + relPath;
    }

    log("PAD: Opening file: " + filePath);
    string data = readFile(filePath);
    log(to_string(data.size()));

    texts = split(data, '\n');
    string homeDir = getenv("HOME");
    saveFile(
        data,
        homeDir + "/Documents/nsx/backups/backup-" + getTime() + "-" + fileName
    );
}

void Pad::refresh() {
    /*
        int txtLim = min(YMAX - 1, (int)texts.size());
        for (int j = 0; j < txtLim; j++) {
            int idx = j + SY;

            string num = to_string(idx + 1);
            string numPad (NUM_WIDTH - num.size(), ' ');
            mvwprintw(NUM_WIN, j, 0, "%s%s", num.c_str(), numPad.c_str());

            string text = texts[idx];
            string sub;
            if (SX < text.size()) {
                sub = text.substr(SX, txt_win_w);
            }

            string pad(max(0, (txt_win_w + 1) - (int)sub.size()), '.');
            mvwaddnstr(TXT_WIN, j, 0, sub.append(pad).c_str(), txt_win_w);
        }

        for (int j = txtLim; j < YMAX - 1; j++) {
            int idx = j + SY;
            string numPad (NUM_WIDTH, ' ');
            mvwprintw(NUM_WIN, j, 0, "%s", numPad.c_str());

            string pad(txt_win_w + 1, '.');
            mvwaddnstr(TXT_WIN, j, 0, pad.c_str(), txt_win_w);
        }

        updateMenu();
        wmove(TXT_WIN, min(CY - SY, YMAX - 2), min(CX - SX, txt_win_w - 1));
        wrefresh(NUM_WIN);
        wrefresh(TXT_WIN);
        */
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

        string pad(padWinW + 1, '.');
        mvwaddnstr(padWin, j, 0, pad.c_str(), padWinW);
    }

    //wmove(padWin, min(cy - sy, YMAX - 2), min(cx - sx, padWinW - 1));
    
    mvwchgat(
        padWin, 
        min(cy - sy, YMAX - 2), min(cx - sx, padWinW - 1),
        1,
        WA_NORMAL,
        0,
        NULL
    );
    
    
    wrefresh(numWin);
    wrefresh(padWin);
}

void Pad::takeInput(string input) {
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
}

void Pad::putCursor(int x, int y) {
    wmove(padWin, x, y);
    // todo: update cursor cx,cy,sx,sy vars
}

void Pad::setPos(int x, int y) {
    mvwin(numWin, y, x);
    mvwin(padWin, y, x + NUM_WIDTH);
}