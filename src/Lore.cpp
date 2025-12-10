#include "Lore.h"

#include "globals.h"
#include "log.h"
#include "utils.h"

LoreNode::LoreNode() {
    prev = nullptr;
    next = nullptr;
    id = NEXT_LORE_IDX++;
    //loga("CREATING NODE", to_string(id));
}

LoreNode::~LoreNode() {}

void LoreNode::add(int line, string text) {
    //loga(
    //    "LORE NODE:", 
    //    to_string(id), 
    //    ", ADD: ", 
    //    to_string(line), 
    //    text
    //);
    pair<int, string> p(line, text);
    lines.insert(p);
}

Lore::Lore() {
    log("creating lore");
    cur = nullptr;
}

Lore::~Lore() {
    log("deleting Lore");
    deleteFuture();
    deletePast();
    delete cur;
}

void Lore::print() {
    log("============ PRINTING LORE ============");
    LoreNode* t = cur;
    while (t != nullptr && t->prev != nullptr) {
        t = t->prev;
    }
    
    if (t == nullptr) {
        log("nothing to print");    
    }
    
    while (t != nullptr) {
        if (t == cur) {
            loga("       \/\/\/\/ CUR \/\/\/\/ ");
        }
        
        loga(
            "node:", to_string(t->id), 
            "(",
            t->prev ? to_string(t->prev->id) : "nullptr",
            t->next ? to_string(t->next->id) : "nullptr",
            ")"
        );
        loga("  targ:", t->targ == ACT ? "ACT" : "OLD");
        loga("  lines:");
        for (auto it = t->lines.begin(); it != t->lines.end(); ++it) {
            loga("     ", to_string(it->first), it->second);
        }
        
        if (t == cur) {
            loga("       /\/\/\/\ CUR /\/\/\/\ ");
        }
        log(""); 
        
        t = t->next;
    }
    
    log("============ END  ============");
}

LoreState Lore::rewind() {
    LoreState ret;
    ret.old = nullptr;
    ret.cur = nullptr;

    if (cur == nullptr || cur->prev == nullptr) {
        //log("  cannot rewind");
        return ret;
    }
    //log("REWINDING FROM: " + to_string(cur->id));
    //if (cur->prev != nullptr) {
    //    log("  prev is: " + to_string(cur->prev->id));
    //}
    //else {
    //    log("  prev is: null");
    //}
    //if (cur->next != nullptr) {
    //    log("  next is: " + to_string(cur->next->id));
    //}
    //else {
    //    log("  next is: null");
    //}
    
    ret.old = cur;
    ret.cur = cur->prev;

    cur = cur->prev;
    //log("    now a: " + to_string(cur->id));
    return ret;
}

LoreState Lore::advance() {
    LoreState ret;
    ret.old = nullptr;
    ret.cur = nullptr;

    if (cur == nullptr || cur->next == nullptr) {
        log("  cannot advance");
        return ret;
    }
    //log("ADVANCING FROM: " + to_string(cur->id));
    //if (cur->prev != nullptr) {
    //    log("  prev is: " + to_string(cur->prev->id));
    //}
    //else {
    //    log("  prev is: null");
    //}
    //if (cur->next != nullptr) {
    //    log("  next is: " + to_string(cur->next->id));
    //}
    //else {
    //    log("  next is: null");
    //}

    ret.old = cur;
    ret.cur = cur->next;
    
    
    cur = cur->next;
    //log("    now at: " + to_string(cur->id));
    return ret;
}

void Lore::deleteFuture() {
    //log("    deleting the future uwu");
    
    if (cur == nullptr || cur->next == nullptr) {
        return;
    }
    
    //log("      starting at: " + to_string(cur->id));
    
    //LoreNode* fut = cur->next;
    LoreNode* fut;
    if (cur->targ == OLD) {
        //log("deleting old");
        fut = cur;
        if (cur->prev == nullptr) {
            cur = nullptr;
        }
        else {
            cur = cur->prev;
        }
    }
    else {
        fut = cur->next;
    }
    
    while (fut != nullptr) {
        LoreNode* temp = fut;
        fut = fut->next;
        //log("      deleting: " + to_string(temp->id));
        delete temp;
    }
    
    if (cur) {
        cur->next = nullptr;
    }
}

void Lore::deletePast() {
    log("    deleting the past uwu");    
    if (cur == nullptr || cur->prev == nullptr) {
        return;
    }
    
    LoreNode* pas = cur->prev;
    while (pas != nullptr) {
        LoreNode* temp = pas;
        pas = pas->prev;
        delete temp;
    }
    cur->prev = nullptr;
}

void Lore::add(LoreNode* pn, LoreNode* cn) {
    deleteFuture();
    
    if (cur == nullptr || cur->lines.size() > 1 || cur->lines.begin()->first != cn->lines.begin()->first) {
        //log("~~~creating new entry from supplied nodes~~~");
        //loga("  adding nodes: ", to_string(pn->id), to_string(cn->id));

        if (cur != nullptr) {
            cur->next = pn;
        }
        pn->targ = OLD;
        cn->targ = ACT;
        
        pn->prev = cur;
        pn->next = cn;
        cn->prev = pn;
        cur = cn;
        
        /*
        if (pn->prev != nullptr) {
            loga("pn prev:", to_string(pn->prev->id));
        }
        else {
           loga("pn prev: nullptr"); 
        }
        loga("pn:", to_string(pn->id));
        loga("pn next:", to_string(pn->next->id));
        
        loga("cn prev:", to_string(cn->prev->id));
        loga("cn:", to_string(cn->id));
        if (cn->next != nullptr) {
            loga("cn next:", to_string(cn->next->id));
        }
        else {
           loga("cn next: nullptr"); 
        }
        */
    }
    else {
        //log("~~~updating existing line~~~");
        delete pn;
        
        cn->targ = ACT;
        
        if (cur->prev == nullptr && cur->next == nullptr) {
            //log("!at base, attach cur");
            cur->next = cn;
            cn->prev = cur;
            cur = cn;
        }
        else {
            //log("attach cur normal");
            LoreNode* temp = cur;
            cur->prev->next = cn;
            cn->prev = cur->prev;
            cur = cn;
            delete temp;
        }
        
        /*
        if (cur->prev != nullptr) {
            loga("cur->prev:", to_string(cur->prev->id));
        }
        else {
            loga("cur->prev: nullptr");
        }
        loga("cur:", to_string(cur->id));
        if (cn->next != nullptr) {
            loga("cur->next:", to_string(cur->next->id));
        }
        else {
            loga("cur->next: nullptr");
        }
        */
    }
}