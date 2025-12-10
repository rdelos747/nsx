#ifndef LOG_H
#define LOG_H

#include <curses.h>
#include <string>
#include <vector>
#include <sstream>
#include <type_traits>
#include <stdexcept>
#include <iostream>

#include "globals.h"

using namespace std;

void log(string text);

void clearLog();

template <typename... Args>
void loga(const Args&... args) {
    std::stringstream strm;
    ((strm << std::string(args) << " "), ...);
    log(strm.str());
}

#endif
