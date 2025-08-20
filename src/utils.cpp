#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

template <typename T>
string str(const T& val) {
    if constexpr (is_arithmetic<T>::value) {
        return to_string(val);
    }
    if constexpr (is_same<T, string>::value) {
        return val;
    }

    stringstream strm;
    strm << "str function: cannot cast type:" << typeid(T).name() << "to string";
    throw invalid_argument(strm.str());
}

template <typename T>
string vecJoin(const vector<T>& input, char delim) {
    string out;
    for (int i = 0; i < input.size(); i++) {
        out += str(input[i]);

        if (i < input.size() - 1) {
            out += delim;
        }
    }
    return out;
}

template string vecJoin<int>(const vector<int>& input, char delim);
template string vecJoin<string>(const vector<string>& input, char delim);

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

void clearLog() {
    ofstream logFile;
    logFile.open("log.txt", ios_base::out);
    logFile.close();
}

void log(string text) {
    ofstream logFile;
    logFile.open("log.txt", ios_base::app);
    logFile << text << '\n';
    logFile.close();
}

void saveFile(string data, string path) {
    log("SAVING FILE " + path);
    ofstream out;
    out.open(path, ios_base::out);
    out << data;
    out.close();
}
