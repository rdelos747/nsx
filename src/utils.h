#ifndef UTILS_H
#define UTILS_H

#include <curses.h>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <type_traits>
#include <typeinfo>

#include "log.h"

using namespace std;

extern regex R_SCHR;
extern regex R_NUMB;
extern regex R_NUMB_BEGIN;

string boolstr(bool b);

string vecJoin(const vector<string>& input, char delim);

vector<string> split(string input, char delim);

string readFile(string path);

string getTime();

void saveFile(string data, string path);

struct Pt {
    int x;
    int y;
    bool in;
};

Pt in_win(WINDOW* win, int x, int y);

#endif