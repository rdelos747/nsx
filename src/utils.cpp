#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

#include "globals.h"
#include "utils.h"

using namespace std;

string boolstr(bool b) {
    return b ? "true" : "false";
}

string vecJoin(const vector<string>& input, char delim) {
    string out;
    for (int i = 0; i < input.size(); i++) {
        out += input[i];

        if (i < input.size() - 1) {
            out += delim;
        }
    }
    return out;
}

vector<string> split(string input, char delim) {
    vector<string> out;
    string line;
    stringstream stream(input);
    while(getline(stream, line, delim)) {
        out.push_back(line);
    }
    return out;
}

string readFile(string path) {
    size_t readSize = size_t(4096);
    ifstream stream = ifstream(path);

    string out;
    string buffer = string(readSize, '\0');
    while(stream.read(&buffer[0], readSize)) {
        out.append(buffer, 0, stream.gcount());
    }
    out.append(buffer, 0, stream.gcount());
    return out;
}

string getTime() {
    time_t time = std::time(nullptr);
    tm ltm = *localtime(&time);
    ostringstream oss;
    oss << put_time(&ltm, "%Y-%m-%d-%H-%M-%S");

    return oss.str();
}

void saveFile(string data, string path) {
    log("SAVING FILE " + path);
    ofstream out;
    out.open(path, ios_base::out);
    out << data;
    out.close();
}

Pt in_win(WINDOW* win, int x, int y) {
    int wh, ww;
    int wy, wx;
    getbegyx(win, wy, wx);
    getmaxyx(win, wh, ww);
    
    Pt pt;
    pt.x = x - wx;
    pt.y = y - wy;
    pt.in = pt.x >= 0 && pt.x <= ww && pt.y >= 0 && pt.y <= wh;

    return pt;
}