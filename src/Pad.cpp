#include "clip.h"
#include <filesystem>
#include <ncurses.h>

#include "globals.h"
#include "log.h"
#include "utils.h"

#include "Pad.h"

Pad::Pad(int nx, int ny, int nw, int nh) :
    lore() {
    
    x = nx, y = ny;
    w = nw, h = nh;
    mx = 0, my = 0;
    mouseDragging = 0;
    scrx = 0, scry = 0;
    curc = new Cursor();
    padWinW = w - NUM_WIDTH - 1;
    texts = {""};
    touched = false;
    numWin = newwin(h, NUM_WIDTH, y, x);
    padWin = newwin(h, w - NUM_WIDTH, y, x + NUM_WIDTH);
    scrollok(padWin, true);
    
   // string s;
   // clip::get_text(s);
   // log(s);
}

Pad::~Pad() {
    log("deleting pad");
    log("2");
    
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
    
    filesystem::path fpath(relPath);
    if (fpath.is_absolute()) {
        log("absolute");
        filePath = relPath;
    }
    else if (fpath.is_relative()) {
        log("relative");
        string cwd = filesystem::current_path().string();
        filePath = cwd + "/" + relPath;
    }
    
    //string cwd = filesystem::current_path().string();
    //filePath = cwd + "/" + relPath;

    log(HOME_DIR + "/Documents/nsx/log.txt");
    
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

        string num = to_string(idx);
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
    string shInput = input;
    
    MEVENT event;
    if (input == "KEY_MOUSE") {
        if (getmouse(&event) == OK) {
            Pt pt = in_win(padWin, event.x, event.y);
            
            if (pt.in) {
                if (event.bstate & BUTTON4_PRESSED) {
                    input = "KEY_UP";
                }
                else if (event.bstate & BUTTON5_PRESSED) {
                    input = "KEY_DOWN";
                }
                else if (event.bstate & BUTTON1_PRESSED) {
                    curc->drag = false; //clear the last drag on new press
                    mouseDragging = true;
                }
                else if (event.bstate & BUTTON1_RELEASED) {
                    mouseDragging = false;
                }
                
                if (mouseDragging) {
                    cx = pt.x + scrx;
                    cy = pt.y + scry;
                    
                    if (!(event.bstate & BUTTON1_PRESSED) && (pt.x != mx || pt.y != my)) {
                        drag = true;
                    }
                }
                else if (curc->drag) {   
                    /*
                    New mouse move events will trigger a refresh, causing the drag to clear.
                    If we have dragged previously, maintain it until another event explicitly
                    clears it.
                    */
                    drag = true;
                }
                
                //loga("mouse: ", boolstr(mouseDragging));
                
                if (!(event.bstate & BUTTON_SHIFT)) {
                    //log("mouse not dragging");
                    shInput = input;
                }
            }
        }
    }
    
    if (input == "KEY_SR") {
        // shift + up
        input = "KEY_UP";
    }
    else if (input == "kUP6") {
        // shift + ctrl + up
        input = "kUP5";
    }
    else if (input == "KEY_SPREVIOUS") {
        // shift + page up
        input = "KEY_PPAGE";
    }
    else if (input == "KEY_SF") {
        // shift + down
        input = "KEY_DOWN";
    }
    else if (input == "kDN6") {
        // shift + ctrl + down
        input = "kDN5";
    }
    else if (input == "KEY_SNEXT") {
        // shift + page down
        input = "KEY_NPAGE";
    }
    else if (input == "KEY_SLEFT") {
        // shift + left
        input = "KEY_LEFT";
    }
    else if (input == "kLFT6") {
        // shift + crtl + left
        input = "kLFT5";
    }
    else if (input == "KEY_SRIGHT") {
        // shift + right
        input = "KEY_RIGHT";
    }
    else if (input == "kRIT6") {
        // shift + crtl + left
        input = "kRIT5";
    }
    
    if (shInput != input) {
        // input changed because shift, 
        // user must be dragging.
        drag = true;
    }
    
    if (input == "KEY_UP") {
        // up
        cy = max(cy - 1, 0);   
    }
    else if (input == "kUP5") {
        // ctrl + up
        cy = max(cy - 5, 0);
    }
    else if (input == "KEY_PPAGE") {
        // page up
        cy = max(cy - (YMAX - 1), 0);
    }
    else if (input == "KEY_DOWN") {
        // down
        cy = min(cy + 1, (int)texts.size() - 1);
    }
    else if (input == "kDN5") {
        // ctrl + down
        cy = min(cy + 5, (int)texts.size() - 1);   
    }
    else if (input == "KEY_NPAGE") {
        // page down
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
    else if (input == "^S") {    
        string joined = vecJoin(texts, '\n');
        saveFile(joined, filePath);
        touched = false;
    }
    else if (input == "KEY_BACKSPACE") {
        touched = true;
        if (curc->drag) {
            ClearOp op = clearAtCursorBounds();
            CurPts pts = op.pts;
            op.aft->add(pts.sy,"");
            lore.add(op.old, op.aft);
            cx = pts.sx;
            cy = pts.sy;
        }
        else {
            LoreNode* old = new LoreNode();
            LoreNode* aft = new LoreNode();
            old->add(cy, texts[cy]);
            
            string after = texts[cy].substr(cx, texts[cy].length());
            if (cx > 0) {
                string prevText = texts[cy];
                string prev = texts[cy].substr(0, cx);
                
                int n = 1;
                while ((prev.length() - n) % 4 != 0 && prev[prev.length() - n] == ' ') {
                    n += 1;
                
                }
                prev = prev.substr(0, (int)prev.length() - n);
                texts[cy] = prev + after;
                cx -= n;
                
                aft->add(cy, texts[cy]);
            }
            else if (cy > 0) {
                old->add(cy - 1, texts[cy - 1]);
                cx = texts[cy - 1].length();
                texts[cy - 1] += after;
                aft->add(cy - 1, texts[cy - 1]);
                texts.erase(texts.begin() + cy);
                cy -= 1;
            }
            
            lore.add(old, aft);
        }
    }
    else if (input == "^M") { // ENTER
        touched = true;
        ClearOp op = clearAtCursorBounds();
        CurPts pts = op.pts;
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
        op.aft->add(cy, texts[cy]);
        texts.insert(texts.begin() + cy + 1, cut);
        op.aft->add(cy + 1, texts[cy + 1]);
        cy += 1;
        cx = pad.length();
        
        lore.add(op.old, op.aft);
    }
    else if (input == "^I") { // TAB
        // todo: clear the cursor bounds here, but im lazy rn
        touched = true;
        LoreNode* old = new LoreNode();
        LoreNode* aft = new LoreNode();
        old->add(cy, texts[cy]);
        texts[cy].insert(cx, string (4, ' '));
        aft->add(cy, texts[cy]);
        lore.add(old, aft);
        cx += 4;
    }
    else if (input == "^C") {
        copyClip();
    }
    else if (input == "^X") {
        copyClip();
        ClearOp op = clearAtCursorBounds();
        CurPts pts = op.pts;
        op.aft->add(pts.sy,"");
        lore.add(op.old, op.aft);
        
        cx = pts.sx;
        cy = pts.sy;
    }
    else if (input == "^V") {
        touched = true;
        ClearOp op = pasteClip();
        op.aft->add(op.pts.sy,"");
        lore.add(op.old, op.aft);
        cx = op.pts.sx;
        cy = op.pts.sy;
    }
    else if (input == "^Z") {
        //log("===== STARTING REWIND =====");
        LoreState state = lore.rewind();
        if (state.cur != nullptr && state.cur->targ == ACT) {
            //log("rewinding again");
            state = lore.rewind();
        }
        placeLore(state.old, state.cur);
    }
    else if (input == "^Y") {
        //log("===== STARTING ADVANCE =====");
        LoreState state = lore.advance();
        if (state.cur != nullptr && state.cur->targ == ACT) {
            //log("advancing again");
            lore.advance();
        }
        placeLore(state.old, state.cur);
    }
    else if (input == "^P") { // test command
        //test
        lore.print();
    }
    else if (input == "^_") { // add comment
        CurPts pts = curc->getBounds();
        LoreNode* old = new LoreNode();
        LoreNode* aft = new LoreNode();
        
        bool all = true;
        for (int j = pts.sy; j<= pts.ey; j++) {
            string com = texts[j].substr(pts.sx, min(2, int(texts[j].length())));
            if (com != "//") {
                all = false;
            }
        }
        
        for (int j = pts.sy; j<= pts.ey; j++) {
            old->add(j, texts[j]);
            string prev = texts[j].substr(0, pts.sx);
            if (all) {
                string after = texts[j].substr(pts.sx + 2, texts[j].length());
                texts[j] = prev + after;
            }
            else {
                string after = texts[j].substr(pts.sx, texts[j].length());
                texts[j] = prev + "//" + after;
            }
            aft->add(j, texts[j]);
        }
        
        lore.add(old, aft);
        drag = true;
    }
    else if (input.size() == 1) {
        touched = true;
        ClearOp op = clearAtCursorBounds();
        CurPts pts = op.pts;
        cx = pts.sx;
        cy = pts.sy;
        texts[cy].insert(cx, input);
        op.aft->add(cy, texts[cy]);
        lore.add(op.old, op.aft);
        
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

ClearOp Pad::clearAtCursorBounds() {
    LoreNode* old = new LoreNode();
    LoreNode* aft = new LoreNode();
    CurPts pts = curc->getBounds();
    
    old->add(pts.sy, texts[pts.sy]);
    string prev = texts[pts.sy].substr(0, pts.sx);
    
    string after = texts[pts.ey].substr(
        pts.ex, 
        texts[pts.ey].length() - pts.ex
    );
    
    for (int j = pts.ey; j > pts.sy; j--) {
        old->add(j, texts[j]);
        texts.erase(texts.begin() + j);
    }
    
    texts[pts.sy] = prev + after;
    
    ClearOp op;
    op.pts = pts;
    op.old = old;
    op.aft = aft;
    return op;
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

ClearOp Pad::pasteClip() { 
    string pasteStr;
    clip::get_text(pasteStr);
    vector<string> pastes = split(pasteStr,'\n');
    ClearOp op = clearAtCursorBounds();
    CurPts pts = op.pts;
    
    texts[pts.sy].insert(pts.sx, pastes[0]);
    op.aft->add(pts.sy, texts[pts.sy]);
    pts.sx += pastes[0].length();
    pts.sy += 1;
    for (int j = 1; j < (int)pastes.size(); j++) {
        texts.insert(texts.begin() + pts.sy, pastes[j]);
        op.aft->add(pts.sy, texts[pts.sy]);
        pts.sx = pastes[j].length();
        pts.sy += 1;
    }
    
    return op;
}

void Pad::placeLore(LoreNode* old, LoreNode* cur) {
    if (old) {
        for (auto it = old->lines.rbegin(); it != old->lines.rend(); ++it) {
            //loga("node:", to_string(old->id), "erasing old: ", to_string(it->first), it->second);
            texts.erase(texts.begin() + it->first);
        }       
    }
    if (cur) {
        for (auto it = cur->lines.begin(); it != cur->lines.end(); ++it) {   
            //loga("node:", to_string(cur->id), "adding cur: ", to_string(it->first), it->second);
            texts.insert(texts.begin() + it->first, it->second);
        } 
    }
}