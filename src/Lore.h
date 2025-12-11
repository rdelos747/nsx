#ifndef LORE_H
#define LORE_H

#include <map>
#include <string>

using namespace std;

enum LoreTarg {
    OLD,
    ACT
};

class LoreNode {
public:
    LoreNode();
    ~LoreNode();
    void add(int line, string text);
    void setCur(int x, int y);
    
    int id;
    LoreTarg targ;
    LoreNode* prev;
    LoreNode* next;
    map<int, string> lines;
    int cx;
    int cy;
};

struct LoreState {
    LoreNode* old;
    LoreNode* cur;
};

class Lore {
public:
    Lore();
    ~Lore();
    
    void add(LoreNode* prev, LoreNode* after);
    LoreState rewind();
    LoreState advance();
    
    void print();
    
private:
    LoreNode* cur;
    void deleteFuture();
    void deletePast();
};

#endif